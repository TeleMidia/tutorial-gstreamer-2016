#include <stdlib.h>
#include <glib.h>
#include <gst/gst.h>

GstElement *pipeline, *source, *videoconvert, *videofilter, *videoconvert2, 
           *audioconvert, *videosink, *audiosink;

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

  g_assert (sinkpad);
  g_assert (GST_PAD_LINK_SUCCESSFUL(gst_pad_link (pad, sinkpad)));

  gst_object_unref (sinkpad);
  gst_caps_unref (pad_caps);
}

int main (int argc, char *argv[])
{
  GstBus *bus;
  GstMessage *msg;
  char *uri;
  int filter;

  if (argc < 2)
  {
    g_print ("usage: %s FILTER_NUMBER\n", argv[0]);
    return EXIT_SUCCESS;
  }

  gst_init (&argc, &argv);
  
  pipeline = gst_pipeline_new ("pipeline");

  source        = gst_element_factory_make ("uridecodebin", "source");
  videoconvert  = gst_element_factory_make ("videoconvert", "videoconverter");
  videoconvert2 = gst_element_factory_make ("videoconvert", "videoconverter2");
  audioconvert  = gst_element_factory_make ("audioconvert", "audioconverter");
  videosink     = gst_element_factory_make ("autovideosink", "videosink");
  audiosink     = gst_element_factory_make ("autoaudiosink", "audiosink");

  filter = (int) strtoll (argv[1], NULL, 10); 
  switch (filter) {
    case 1:
      videofilter  = gst_element_factory_make ("coloreffects", "videofilter");
      g_object_set (G_OBJECT (videofilter), "preset", 1, NULL);
      break;
    case 2:
      videofilter  = gst_element_factory_make ("rippletv", "videofilter");
      break;
    case 3:
      videofilter  = gst_element_factory_make ("shagadelictv", "videofilter");
      break;
    case 4:
      videofilter  = gst_element_factory_make ("edgetv", "videofilter");
      break;
    case 5:
      videofilter  = gst_element_factory_make ("revtv", "videofilter");
      break;
    default:
      g_print ("%d: invalid value\n", filter);
      return EXIT_FAILURE;
  } 

  gst_bin_add_many (GST_BIN(pipeline), source, videoconvert, videofilter, 
      videoconvert2,  audioconvert, videosink, audiosink, NULL);

  gst_element_link_many (videoconvert, videofilter, videoconvert2, videosink, NULL);
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
