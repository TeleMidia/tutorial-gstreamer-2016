#include <glib.h>
#include <gst/gst.h>

int main (int argc, char *argv[])
{
  GstElement *pipeline, *src, *dec, *sink;
  GstStateChangeReturn ret;
  GstBus *bus;
  GstMessage *msg;

  gst_init (&argc, &argv);
  if (argc != 2)
    {
      g_printerr ("usage: %s FILE\n", g_get_prgname ());
      return 1;
    }

  pipeline = gst_element_factory_make ("pipeline", "mp3");
  src = gst_element_factory_make ("filesrc", "src");
  dec = gst_element_factory_make ("mad", "dec");
  sink = gst_element_factory_make ("alsasink", "sink");
  g_assert (pipeline && src && dec && sink);

  gst_bin_add_many (GST_BIN (pipeline), src, dec, sink, NULL);
  gst_element_link_many (src, dec, sink, NULL);
  g_object_set (G_OBJECT (src), "location", argv[1], NULL);

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
