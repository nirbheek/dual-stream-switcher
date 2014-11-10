/* vim: set sts=4 sw=4 et : */

#include <unistd.h>
#include <glib.h>
#include <gst/gst.h>

static GMainLoop *loop;

static void
on_gst_bus_eos (GstBus *bus,
                GstMessage *msg,
                GstElement *playbin)
{
    g_print ("End of file\n");
    g_main_loop_quit (loop);
}

static void
on_gst_bus_error (GstBus *bus,
                  GstMessage *msg,
                  GstElement *playbin)
{
    GError *error = NULL;
    char *tmp = NULL;

    gst_message_parse_error (msg, &error, &tmp);
    g_printerr ("ERROR from element %s: %s\n",
                GST_OBJECT_NAME (msg->src), error->message);
    g_printerr ("Debug info: %s\n", tmp);
    g_error_free (error);
    g_free (tmp);
    g_main_loop_quit (loop);
}

static void
on_decodebin_pad_added (GstElement *decodebin,
                        GstPad     *dsrc,
                        GstElement *pipeline)
{
    GstCaps *caps;
    GstPad *isink;
    GstElement *iselect;
    char *name;

    caps = gst_pad_query_caps (dsrc, NULL);
    name = gst_caps_to_string (caps);

    if (!g_str_has_prefix (name, "video/x-raw"))
        goto meh;
    g_free (name);

    /* Get the correct output selector */
    name = gst_element_get_name (decodebin);
    iselect = gst_bin_get_by_name (GST_BIN (pipeline), "iselect");

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
switch_playing_video (GstElement *pipeline)
{
    char *name;
    GstElement *iselect;
    GstPad *sinkpad_old, *sinkpad_new;

    iselect = gst_bin_get_by_name (GST_BIN (pipeline), "iselect");
    g_object_get (iselect, "active-pad", &sinkpad_old, NULL);
    name = gst_pad_get_name (sinkpad_old);

    if (g_strcmp0 (name, "sink_0") == 0)
        sinkpad_new = gst_element_get_static_pad (iselect, "sink_1");
    else
        sinkpad_new = gst_element_get_static_pad (iselect, "sink_0");

    g_object_set (iselect, "active-pad", sinkpad_new, NULL);

    gst_object_unref (sinkpad_old);
    gst_object_unref (sinkpad_new);
    gst_object_unref (iselect);
    g_free (name);
}

static gboolean
on_char_press (GIOChannel   *source,
               GIOCondition  condition,
               gpointer      user_data)
{
    gchar key = 0;
    GError *error = NULL;
    GstElement *pipeline = user_data;

    switch (g_io_channel_read_chars (source, &key, 1, NULL, &error)) {
        case G_IO_STATUS_NORMAL:
            switch (key) {
                case 'r':
                    switch_playing_video (pipeline);
                    break;
                case 'q':
                    g_main_loop_quit (loop);
            }
            break;
        case G_IO_STATUS_ERROR:
            g_printerr ("ERROR reading character: %s\n", error->message);
            break;
        case G_IO_STATUS_EOF:
            g_print ("No key pressed? (EOF)\n");
            break;
        case G_IO_STATUS_AGAIN:
            g_print ("EAGAIN\n");
            break;
        default:
            g_assert_not_reached ();
    }

    return TRUE;
}

int
main (int   argc,
      char *argv[])
{
    GstBus *bus;
    GIOChannel *channel;
    char *f0, *f1;
    GstElement *pipeline, *src0, *src1, *d0, *d1;
    GstElement *iselect, *sink;

    gst_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    if (argc < 3) {
        g_print ("Usage: %s [file1] [file2]\n", argv[0]);
        return 1;
    }

    f0 = argv[1];
    f1 = argv[2];

    pipeline = gst_pipeline_new ("dual-stream");

    src0 = gst_element_factory_make ("filesrc", "file0");
    g_object_set (src0, "location", f0, NULL);
    src1 = gst_element_factory_make ("filesrc", "file1");
    g_object_set (src1, "location", f1, NULL);

    d0 = gst_element_factory_make ("decodebin", "0");
    d1 = gst_element_factory_make ("decodebin", "1");

    iselect = gst_element_factory_make ("input-selector", "iselect");
    g_object_set (iselect,
                  "cache-buffers", TRUE,
                  "sync-streams", TRUE,
                  NULL);

    sink = gst_element_factory_make ("glimagesink", "videosink");

    gst_bin_add_many (GST_BIN (pipeline), src0, src1, d0, d1,
                      iselect, sink, NULL);
    gst_element_link (iselect, sink);
    gst_element_link (src0, d0);
    gst_element_link (src1, d1);

    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_signal_watch (bus);
    g_signal_connect (bus, "message::eos", G_CALLBACK (on_gst_bus_eos), pipeline);
    g_signal_connect (bus, "message::error", G_CALLBACK (on_gst_bus_error), pipeline);
    g_object_unref (bus);

    g_signal_connect (d0, "pad-added",
                      G_CALLBACK (on_decodebin_pad_added), pipeline);
    g_signal_connect (d1, "pad-added",
                      G_CALLBACK (on_decodebin_pad_added), pipeline);

    gst_element_set_state (pipeline, GST_STATE_PLAYING);

    channel = g_io_channel_unix_new (STDIN_FILENO);
    g_io_add_watch (channel, G_IO_IN, (GIOFunc)on_char_press, pipeline);

    g_main_loop_run (loop);

    g_io_channel_unref (channel);
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);
    gst_deinit ();
    return 0;
}

