#include <glib.h>
#include <gst/gst.h>

static void pad_added_cb (GstElement *demux, GstPad *srcpad, gpointer data)
{
  GstElement *pipeline;
  GstCaps *caps;
  const char *name;
  GstPad *sinkpad;
  GstPadLinkReturn ret;
  static gboolean has_vorbis_pad = FALSE, has_theora_pad = FALSE;

  pipeline = GST_ELEMENT (gst_element_get_parent (demux));
  caps = gst_pad_query_caps (srcpad, NULL);
  name = gst_structure_get_name (gst_caps_get_structure (caps, 0));
  if (g_str_equal (name, "audio/x-vorbis") && !has_vorbis_pad)
    {
      GstElement *dec = gst_element_factory_make ("vorbisdec", NULL);
      GstElement *conv = gst_element_factory_make ("audioconvert", NULL);
      GstElement *sink = gst_element_factory_make ("alsasink", NULL);
      g_assert (dec && conv && sink);
      gst_bin_add_many (GST_BIN (pipeline), dec, conv, sink, NULL);
      gst_element_sync_state_with_parent (dec);
      gst_element_sync_state_with_parent (conv);
      gst_element_sync_state_with_parent (sink);
      gst_element_link_many (dec, conv, sink, NULL);
      sinkpad = gst_element_get_static_pad (dec, "sink");
      has_vorbis_pad = TRUE;
    }
  else if (g_str_equal (name, "video/x-theora") && !has_theora_pad)
    {
      GstElement *queue = gst_element_factory_make ("queue", NULL);
      GstElement *dec = gst_element_factory_make ("theoradec", NULL);
      GstElement *sink = gst_element_factory_make ("xvimagesink", NULL);
      g_assert (queue && dec && sink);
      gst_bin_add_many (GST_BIN (pipeline), queue, dec, sink, NULL);
      gst_element_sync_state_with_parent (queue);
      gst_element_sync_state_with_parent (dec);
      gst_element_sync_state_with_parent (sink);
      gst_element_link_many (queue, dec, sink, NULL);
      sinkpad = gst_element_get_static_pad (queue, "sink");
      has_theora_pad = TRUE;
    }
  else
    {
      goto done;                /* fluxo de tipo desconhecido */
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
  GstElement *pipeline, *src, *demux;
  GstStateChangeReturn ret;
  GstBus *bus;
  GstMessage *msg;

  gst_init (&argc, &argv);
  if (argc != 2)
    return (g_printerr ("usage: %s FILE\n", g_get_prgname ()), 1);

  pipeline = gst_pipeline_new ("ogg");
  src = gst_element_factory_make ("filesrc", "src");
  demux = gst_element_factory_make ("oggdemux", "demux");
  g_assert (pipeline && src && demux);

  gst_bin_add_many (GST_BIN (pipeline), src, demux, NULL);
  gst_element_link_many (src, demux, NULL);
  g_object_set (G_OBJECT (src), "location", argv[1], NULL);
  g_signal_connect (demux, "pad-added", G_CALLBACK (pad_added_cb), NULL);

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
