#include <glib.h>
#include <gst/gst.h>

GstElement *pipeline, *source, *videoconvert, *audioconvert,
           *videosink, *audiosink;

static void on_pad_added (GstElement *element, GstPad *pad, gpointer data)
{
  GstPad *sinkpad = NULL;
  GstCaps *pad_caps = NULL;
  GstStructure *pad_struct = NULL;
  const char *pad_type = NULL;

  pad_caps = gst_pad_query_caps (pad, NULL);
  pad_struct = gst_caps_get_structure (pad_caps, 0);
  pad_type = gst_structure_get_name (pad_struct);

  if (g_str_has_prefix (pad_type, "audio/x-raw"))
    sinkpad = gst_element_get_static_pad (audioconvert, "sink"); 
  else if (g_str_has_prefix (pad_type, "video/x-raw"))
    sinkpad = gst_element_get_static_pad (videoconvert, "sink"); 

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
  char *uri;

  gst_init (&argc, &argv);
  
  pipeline = gst_pipeline_new ("pipeline");

  source        = gst_element_factory_make ("uridecodebin", "source");
  videoconvert  = gst_element_factory_make ("videoconvert", "videoconverter");
  audioconvert  = gst_element_factory_make ("audioconvert", "audioconverter");
  videosink     = gst_element_factory_make ("xvimagesink", "videosink");
  audiosink     = gst_element_factory_make ("autoaudiosink", "audiosink");

  gst_bin_add_many (GST_BIN(pipeline), source, videoconvert, audioconvert,
      videosink, audiosink, NULL);

  gst_element_link (videoconvert, videosink);
  gst_element_link (audioconvert, audiosink);

  uri = gst_filename_to_uri ("bunny.ogg", NULL);
  g_object_set (G_OBJECT (source), "uri", uri, NULL);
  g_free (uri);

  g_signal_connect (source, "pad-added", G_CALLBACK (on_pad_added), NULL);

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
