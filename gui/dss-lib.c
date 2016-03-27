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

#define VIDEO_SELECTOR "iselectv"
#define AUDIO_SELECTOR "iselecta"

#define VIDEO_SINK_NAME "videosink"
#define AUDIO_SINK_NAME "audiosink"

static void
on_decodebin_pad_added (GstElement * decodebin, GstPad * dsrc,
    GstElement * pipeline)
{
  GstCaps *caps;
  GstPad *isink;
  GstElement *iselect;
  char *name, *selector_name;

  caps = gst_pad_query_caps (dsrc, NULL);
  name = gst_caps_to_string (caps);

  if (g_str_has_prefix (name, "video/x-raw"))
    selector_name = VIDEO_SELECTOR;
  else if (g_str_has_prefix (name, "audio/x-raw"))
    selector_name = AUDIO_SELECTOR;
  else
    goto meh;
  g_free (name);

  /* Get the correct output selector */
  name = gst_element_get_name (decodebin);
  iselect = gst_bin_get_by_name (GST_BIN (pipeline), selector_name);

  /* Connect decodebin to input selector */
  isink = gst_element_get_request_pad (iselect, "sink_%u");
  g_object_set (isink, "always-ok", TRUE, NULL);
  gst_pad_link (dsrc, isink);

  if (g_strcmp0 (name, "0") == 0)
    g_object_set (iselect, "active-pad", isink, NULL);

  gst_object_unref (isink);
  gst_object_unref (iselect);
meh:
  gst_caps_unref (caps);
  g_free (name);
}

static void
_switch_activepad (GstElement * iselect)
{
  char *name;
  GstPad *sinkpad_old, *sinkpad_new;

  g_object_get (iselect, "active-pad", &sinkpad_old, NULL);
  name = gst_pad_get_name (sinkpad_old);

  if (g_strcmp0 (name, "sink_0") == 0)
    sinkpad_new = gst_element_get_static_pad (iselect, "sink_1");
  else
    sinkpad_new = gst_element_get_static_pad (iselect, "sink_0");

  g_object_set (iselect, "active-pad", sinkpad_new, NULL);

  gst_object_unref (sinkpad_old);
  gst_object_unref (sinkpad_new);
  g_free (name);
}

gboolean
dss_pipeline_toggle_video (GstElement * pipeline)
{
  GstElement *iselect;

  /* Switch video pads */
  iselect = gst_bin_get_by_name (GST_BIN (pipeline), VIDEO_SELECTOR);
  _switch_activepad (iselect);
  gst_object_unref (iselect);

  /* Switch audio pads */
  iselect = gst_bin_get_by_name (GST_BIN (pipeline), AUDIO_SELECTOR);
  _switch_activepad (iselect);
  gst_object_unref (iselect);

  return TRUE;
}

/* ONLY CALL THIS FROM THE GTK MAIN THREAD */
GstElement *
dss_pipeline_new (GFile * file0, GFile * file1, GtkWidget ** out_video_widget)
{
  gchar *uri0, *uri1;
  GstElement *p, *d0, *d1;
  GstElement *isv, *isa, *vconv, *aconv, *vsink, *asink;

  p = gst_pipeline_new ("DSS");

  d0 = gst_element_factory_make ("uridecodebin", "0");
  uri0 = g_file_get_uri (file0);
  g_object_set (d0, "uri", uri0, NULL);
  g_free (uri0);

  d1 = gst_element_factory_make ("uridecodebin", "1");
  uri1 = g_file_get_uri (file1);
  g_object_set (d1, "uri", uri1, NULL);
  g_free (uri1);

  isv = gst_element_factory_make ("input-selector", VIDEO_SELECTOR);
  g_object_set (isv, "cache-buffers", TRUE, "sync-streams", TRUE, NULL);
  isa = gst_element_factory_make ("input-selector", AUDIO_SELECTOR);
  g_object_set (isa, "cache-buffers", TRUE, "sync-streams", TRUE, NULL);

  vconv = gst_element_factory_make ("videoconvert", NULL);
  aconv = gst_element_factory_make ("audioconvert", NULL);

  vsink = gst_element_factory_make ("gtksink", VIDEO_SINK_NAME);
  if (!vsink) {
    g_printerr ("ERROR: Unable to find 'gtksink' element\n");
    goto fail;
  }
  asink = gst_element_factory_make ("autoaudiosink", AUDIO_SINK_NAME);

  gst_bin_add_many (GST_BIN (p), d0, d1, isv, isa, vconv, aconv, vsink, asink,
      NULL);
  gst_element_link_many (isv, vconv, vsink, NULL);
  gst_element_link_many (isa, aconv, asink, NULL);

  g_signal_connect (d0, "pad-added", G_CALLBACK (on_decodebin_pad_added), p);
  g_signal_connect (d1, "pad-added", G_CALLBACK (on_decodebin_pad_added), p);

  g_object_get (vsink, "widget", out_video_widget, NULL);

  return p;
fail:
  g_object_unref (p);
  g_object_unref (d0);
  g_object_unref (d1);
  g_object_unref (isv);
  g_object_unref (isa);
  return NULL;
}
