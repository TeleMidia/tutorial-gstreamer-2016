#include <glib.h>
#include <gst/gst.h>

GstElement *pipeline, *filesrc, *demux, *videodecoder, *videoconvert,
           *audiodecoder, *audioconvert, *videosink, *videoqueue, *audioqueue,
           *audiosink;

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data)
{
  GstPad *sinkpad = NULL;
  GstCaps *pad_caps = NULL;
  GstStructure *pad_struct = NULL;
  const char *pad_type = NULL;

  pad_caps = gst_pad_query_caps (pad, NULL);
  pad_struct = gst_caps_get_structure (pad_caps, 0);
  pad_type = gst_structure_get_name (pad_struct);

  if (g_str_has_prefix (pad_type, "audio/x-vorbis"))
    sinkpad = gst_element_get_static_pad (audioqueue, "sink"); 
  else if (g_str_has_prefix (pad_type, "video/x-theora"))
    sinkpad = gst_element_get_static_pad (videoqueue, "sink"); 

  if (sinkpad != NULL)
  {
    GstPadLinkReturn ret = gst_pad_link (pad, sinkpad);
    if (GST_PAD_LINK_SUCCESSFUL(ret))
      g_print ("Pads linked\n");
    else
      g_print ("Pads not linked\n");

    gst_object_unref (sinkpad);
  }

  gst_caps_unref (pad_caps);
}

int main (int argc, char *argv[])
{
  GstBus *bus;
  GstMessage *msg;

  gst_init (&argc, &argv);
  
  pipeline = gst_pipeline_new ("pipeline");

  filesrc       = gst_element_factory_make ("filesrc", "source");
  demux         = gst_element_factory_make ("oggdemux", "demux");
  videodecoder  = gst_element_factory_make ("theoradec", "videodecoder");
  videoconvert  = gst_element_factory_make ("videoconvert", "videoconverter");
  videoqueue    = gst_element_factory_make ("queue", "videoqueue");
  audioconvert  = gst_element_factory_make ("audioconvert", "audioconverter");
  audiodecoder  = gst_element_factory_make ("vorbisdec", "audiodecoder");
  audioqueue    = gst_element_factory_make ("queue", "audioqueue");
  videosink     = gst_element_factory_make ("xvimagesink", "videosink");
  audiosink     = gst_element_factory_make ("autoaudiosink", "audiosink");

  g_assert (filesrc);
  g_assert (demux);
  g_assert (videodecoder);
  g_assert (audiodecoder);
  g_assert (videosink);
  g_assert (audiosink);

  gst_bin_add_many (GST_BIN(pipeline), filesrc, demux, videodecoder, 
      videoconvert, videoqueue, audiodecoder, audioconvert, audioqueue,
      videosink, audiosink, NULL);

  gst_element_link (filesrc, demux);
  gst_element_link_many (videoqueue, videodecoder, videoconvert, videosink, NULL);
  gst_element_link_many (audioqueue, audiodecoder, audioconvert, audiosink, NULL);

  g_object_set (G_OBJECT (filesrc), "location", "bunny.ogg", NULL);

  g_signal_connect (demux, "pad-added", G_CALLBACK (on_pad_added), NULL);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                    GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);

  return 0;
}
