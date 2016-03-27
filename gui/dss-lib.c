/* vim: set sts=2 sw=2 et :
 *
 *  Copyright (C) 2014-16 Nirbheek Chauhan <nirbheek.chauhan@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "dss-lib.h"

#define VIDEO_MIXER "compositor"
#define AUDIO_MIXER "audiomixer"

#define VIDEO_SINK_NAME "videosink"
#define AUDIO_SINK_NAME "audiosink"

static void
on_decodebin_pad_added (GstElement * decodebin, GstPad * dsrc,
    GstElement * pipeline)
{
  GstPad *sink;
  GstCaps *caps;
  GstElement *mixer;
  const gchar *name;
  gchar *element_name;

  caps = gst_pad_query_caps (dsrc, NULL);
  name = gst_structure_get_name (gst_caps_get_structure (caps, 0));

  if (g_strcmp0 (name, "video/x-raw") == 0)
    goto video;
  else if (g_strcmp0 (name, "audio/x-raw") == 0)
    goto audio;
  else
    goto meh;

video:
  /* Get the correct mixer */
  mixer = gst_bin_get_by_name (GST_BIN (pipeline), VIDEO_MIXER);
  sink = gst_element_get_request_pad (mixer, "sink_%u");
  /* Connect decodebin to input selector */
  gst_pad_link (dsrc, sink);
  /* Mute */
  element_name = gst_element_get_name (decodebin);
  if (g_strcmp0 (element_name, "1") == 0)
    g_object_set (sink, "alpha", 0.0, NULL);

  goto done;

audio:
  /* Get the correct mixer */
  mixer = gst_bin_get_by_name (GST_BIN (pipeline), AUDIO_MIXER);
  sink = gst_element_get_request_pad (mixer, "sink_%u");
  /* Connect decodebin to input selector */
  gst_pad_link (dsrc, sink);
  /* Mute */
  element_name = gst_element_get_name (decodebin);
  if (g_strcmp0 (element_name, "1") == 0)
    g_object_set (sink, "mute", TRUE, NULL);

done:
  gst_object_unref (sink);
  gst_object_unref (mixer);
  g_free (element_name);
meh:
  gst_caps_unref (caps);
}

static gboolean
dss_mixer_get_two_pads (GstElement * mixer, GstPad ** pad0, GstPad ** pad1)
{
  GstIterator *iter;
  GValue sinkpad = G_VALUE_INIT;

  iter = gst_element_iterate_sink_pads (mixer);
  /* Get first sinkpad */
  if (gst_iterator_next (iter, &sinkpad) != GST_ITERATOR_OK) {
    g_critical ("Unable to iterate #1 on audiomixer");
    return FALSE;
  }
  *pad0 = g_value_get_object (&sinkpad);

  /* Get second sinkpad */
  g_value_reset (&sinkpad);
  if (gst_iterator_next (iter, &sinkpad) != GST_ITERATOR_OK) {
    g_critical ("Unable to iterate #2 on audiomixer");
    return FALSE;
  }
  *pad1 = g_value_get_object (&sinkpad);

  /* Done */
  g_value_reset (&sinkpad);
  if (gst_iterator_next (iter, &sinkpad) != GST_ITERATOR_DONE)
    g_warning ("Too many pads on audiomixer");
  g_value_unset (&sinkpad);
  gst_iterator_free (iter);
  return TRUE;
}

static void
dss_videomixer_switch_muted_pads (GstElement * mixer)
{
  gdouble alpha;
  GstPad *sinkpad0, *sinkpad1;

  if (!dss_mixer_get_two_pads (mixer, &sinkpad0, &sinkpad1))
    return;

  g_object_get (sinkpad0, "alpha", &alpha, NULL);
  if (alpha > 0.0) {
    g_object_set (sinkpad0, "alpha", 0.0, NULL);
    g_object_set (sinkpad1, "alpha", 1.0, NULL);
  } else {
    g_object_set (sinkpad0, "alpha", 1.0, NULL);
    g_object_set (sinkpad1, "alpha", 0.0, NULL);
  }
}

static void
dss_audiomixer_switch_muted_pads (GstElement * mixer)
{
  gboolean muted;
  GstPad *sinkpad0, *sinkpad1;

  if (!dss_mixer_get_two_pads (mixer, &sinkpad0, &sinkpad1))
    return;

  g_object_get (sinkpad0, "mute", &muted, NULL);
  if (muted) {
    g_object_set (sinkpad0, "mute", FALSE, NULL);
    g_object_set (sinkpad1, "mute", TRUE, NULL);
  } else {
    g_object_set (sinkpad0, "mute", TRUE, NULL);
    g_object_set (sinkpad1, "mute", FALSE, NULL);
  }
}

gboolean
dss_pipeline_toggle_video (GstElement * pipeline)
{
  GstElement *mixer;

  /* Switch video pads */
  mixer = gst_bin_get_by_name (GST_BIN (pipeline), VIDEO_MIXER);
  dss_videomixer_switch_muted_pads (mixer);
  gst_object_unref (mixer);

  /* Switch audio pads */
  mixer = gst_bin_get_by_name (GST_BIN (pipeline), AUDIO_MIXER);
  dss_audiomixer_switch_muted_pads (mixer);
  gst_object_unref (mixer);

  return TRUE;
}

/* ONLY CALL THIS FROM THE GTK MAIN THREAD */
GstElement *
dss_pipeline_new (GFile * file0, GFile * file1, GtkWidget ** out_video_widget)
{
  gchar *uri0, *uri1;
  GstElement *p, *d0, *d1;
  GstElement *vmix, *amix, *vsink, *asink;

  p = gst_pipeline_new ("DSS");

  d0 = gst_element_factory_make ("uridecodebin", "0");
  uri0 = g_file_get_uri (file0);
  g_object_set (d0, "uri", uri0, NULL);
  g_free (uri0);

  d1 = gst_element_factory_make ("uridecodebin", "1");
  uri1 = g_file_get_uri (file1);
  g_object_set (d1, "uri", uri1, NULL);
  g_free (uri1);

  vmix = gst_element_factory_make ("compositor", VIDEO_MIXER);
  g_object_set (vmix, "background", 1, NULL);
  amix = gst_element_factory_make ("audiomixer", AUDIO_MIXER);

  vsink = gst_element_factory_make ("gtksink", VIDEO_SINK_NAME);
  if (!vsink) {
    g_printerr ("ERROR: Unable to find 'gtksink' element\n");
    goto fail;
  }
  asink = gst_element_factory_make ("autoaudiosink", AUDIO_SINK_NAME);

  gst_bin_add_many (GST_BIN (p), d0, d1, vmix, amix, vsink, asink, NULL);
  gst_element_link_many (vmix, vsink, NULL);
  gst_element_link_many (amix, asink, NULL);

  g_signal_connect (d0, "pad-added", G_CALLBACK (on_decodebin_pad_added), p);
  g_signal_connect (d1, "pad-added", G_CALLBACK (on_decodebin_pad_added), p);

  g_object_get (vsink, "widget", out_video_widget, NULL);

  return p;
fail:
  g_object_unref (p);
  g_object_unref (d0);
  g_object_unref (d1);
  g_object_unref (vmix);
  g_object_unref (amix);
  return NULL;
}
