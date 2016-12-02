#include <glib.h>
#include <gst/gst.h>

static void pad_added_cb (GstElement *src, GstPad *srcpad, gpointer data)
{
  GstElement *pipeline;
  GstCaps *caps;
  const char *name;
  GstPad *sinkpad;
  GstPadLinkReturn ret;
  gint filterno = *((gint *) data);
  static gboolean has_audio_pad = FALSE, has_video_pad = FALSE;

  pipeline = GST_ELEMENT (gst_element_get_parent (src));
  caps = gst_pad_query_caps (srcpad, NULL);
  name = gst_structure_get_name (gst_caps_get_structure (caps, 0));
  if (g_str_equal (name, "audio/x-raw") && !has_audio_pad)
    {
      GstElement *filter = NULL, *conv, *sink;
      switch (filterno)
        {
        case 0:
          filter = gst_element_factory_make ("volume", NULL);
          g_object_set (G_OBJECT (filter), "volume", 2.0, NULL);
          break;
        case 1:
          filter = gst_element_factory_make ("pitch", NULL);
          g_object_set (G_OBJECT (filter), "pitch", .5, NULL);
          break;
        case 2:
          filter = gst_element_factory_make ("audioecho", NULL);
          g_object_set (G_OBJECT (filter), "delay", GST_SECOND, "intensity", 1., NULL);
          break;
        case 3:
          filter = gst_element_factory_make ("freeverb", NULL);
          g_object_set (G_OBJECT (filter), "room-size", 1., "level", 1., NULL);
          break;
        case 4:
          filter = gst_element_factory_make ("equalizer-3bands", NULL);
          g_object_set (G_OBJECT (filter), "band0", 12., "band1", -24., NULL);
          break;
        }
      conv = gst_element_factory_make ("audioconvert", NULL);
      sink = gst_element_factory_make ("autoaudiosink", NULL);
      g_assert (filter && conv && sink);
      gst_bin_add_many (GST_BIN (pipeline), filter, conv, sink, NULL);
      gst_element_sync_state_with_parent (filter);
      gst_element_sync_state_with_parent (conv);
      gst_element_sync_state_with_parent (sink);
      gst_element_link_many (filter, conv, sink, NULL);
      sinkpad = gst_element_get_static_pad (filter, "sink");
      has_audio_pad = TRUE;
    }
  else if (g_str_equal (name, "video/x-raw") && !has_video_pad)
    {
      GstElement *pre, *filter = NULL, *pos, *sink;
      switch (filterno)
        {
        case 0:
          filter = gst_element_factory_make ("coloreffects", NULL);
          g_object_set (G_OBJECT (filter), "preset", 1, NULL);
          break;
        case 1:
          filter = gst_element_factory_make ("dicetv", NULL);
          break;
        case 2:
          filter = gst_element_factory_make ("shagadelictv", NULL);
          break;
        case 3:
          filter = gst_element_factory_make ("edgetv", NULL);
          break;
        case 4:
          filter = gst_element_factory_make ("revtv", NULL);
          break;
        }
      pre = gst_element_factory_make ("videoconvert", NULL);
      pos = gst_element_factory_make ("videoconvert", NULL);
      sink = gst_element_factory_make ("autovideosink", NULL);
      g_assert (pre && filter && pos && sink);
      gst_bin_add_many (GST_BIN (pipeline), pre, filter, pos, sink, NULL);
      gst_element_sync_state_with_parent (pre);
      gst_element_sync_state_with_parent (filter);
      gst_element_sync_state_with_parent (pos);
      gst_element_sync_state_with_parent (sink);
      gst_element_link_many (pre, filter, pos, sink, NULL);
      sinkpad = gst_element_get_static_pad (pre, "sink");
      has_video_pad = TRUE;
    }
  else
    {
      goto done;
    }
  ret = gst_pad_link (srcpad, sinkpad);
  g_assert (GST_PAD_LINK_SUCCESSFUL (ret));
  gst_object_unref (sinkpad);
 done:
  gst_caps_unref (caps);
  gst_object_unref (pipeline);
}

int main (int argc, char *argv[])
{
  GstElement *pipeline, *src;
  gint filter;
  GstStateChangeReturn ret;
  GstBus *bus;
  GstMessage *msg;

  gst_init (&argc, &argv);
  if (argc != 3)
    return (g_printerr ("usage: %s URI [0-4]\n", g_get_prgname ()), 1);
  filter = g_ascii_strtoll (argv[2], NULL, 10) % 5;

  pipeline = gst_pipeline_new ("filter");
  src = gst_element_factory_make ("uridecodebin", "src");
  g_assert (pipeline && src);

  gst_bin_add_many (GST_BIN (pipeline), src, NULL);
  g_object_set (G_OBJECT (src), "uri", argv[1], NULL);
  g_signal_connect (src, "pad-added", G_CALLBACK (pad_added_cb), &filter);

  ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  g_assert (ret != GST_STATE_CHANGE_FAILURE);
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                    GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 0;
}
