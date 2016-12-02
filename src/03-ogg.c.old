#include <glib.h>
#include <gst/gst.h>

static void pad_added_cb (GstElement *demux, GstPad *srcpad, gpointer data)
{
  GstElement *pipeline, *queue;
  GstCaps *caps;
  const char *name, *queue_name;
  GstPad *sinkpad;
  GstPadLinkReturn ret;

  pipeline = GST_ELEMENT (gst_element_get_parent (demux));
  caps = gst_pad_query_caps (srcpad, NULL);
  name = gst_structure_get_name (gst_caps_get_structure (caps, 0));
  queue_name = (g_str_equal (name, "audio/x-vorbis")) ? "audioqueue" :
               (g_str_equal (name, "video/x-theora")) ? "videoqueue" : NULL;
  gst_caps_unref (caps);
  if (queue_name == NULL)
    goto done;                  /* fluxo de tipo desconhecido */

  queue = gst_bin_get_by_name (GST_BIN (pipeline), queue_name);
  sinkpad = gst_element_get_static_pad (queue, "sink");
  ret = gst_pad_link (srcpad, sinkpad);
  g_assert (GST_PAD_LINK_SUCCESSFUL (ret));

  gst_object_unref (sinkpad);
  gst_object_unref (queue);
 done:
  gst_object_unref (pipeline);
}

int main (int argc, char *argv[])
{
  GstElement *pipeline, *src, *demux,
             *audioqueue, *audiodec, *audioconv, *audiosink,
             *videoqueue, *videodec, *videoconv, *videosink;
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

  audioqueue = gst_element_factory_make ("queue", "audioqueue");
  audiodec = gst_element_factory_make ("vorbisdec", "audiodec");
  audioconv = gst_element_factory_make ("audioconvert", "audioconv");
  audiosink = gst_element_factory_make ("alsasink", "audiosink");
  g_assert (audioqueue && audiodec && audioconv && audiosink);

  videoqueue = gst_element_factory_make ("queue", "videoqueue");
  videodec = gst_element_factory_make ("theoradec", "videodec");
  videoconv = gst_element_factory_make ("videoconvert", "videoconv");
  videosink = gst_element_factory_make ("xvimagesink", "videosink");
  g_assert (videoqueue && videodec && videoconv && videosink);

  gst_bin_add_many (GST_BIN (pipeline), src, demux,
                    audioqueue, audiodec, audioconv, audiosink,
                    videoqueue, videodec, videoconv, videosink, NULL);

  gst_element_link_many (src, demux, NULL);
  gst_element_link_many (audioqueue, audiodec, audioconv, audiosink, NULL);
  gst_element_link_many (videoqueue, videodec, videoconv, videosink, NULL);

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
