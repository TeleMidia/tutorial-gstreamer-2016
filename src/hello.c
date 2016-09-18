#include <glib.h>
#include <gst/gst.h>

int main (int argc, char *argv[])
{
  GstElement *playbin;
  char *uri;
  GstStateChangeReturn ret;
  GstBus *bus;
  GstMessage *msg;

  gst_init (&argc, &argv);

  playbin = gst_element_factory_make ("playbin", "hello");
  g_assert_nonnull (playbin);

  uri = gst_filename_to_uri ("bunny.ogg", NULL);
  g_assert_nonnull (uri);
  g_object_set (G_OBJECT (playbin), "uri", uri, NULL);
  g_free (uri);

  ret = gst_element_set_state (playbin, GST_STATE_PLAYING);
  g_assert (ret != GST_STATE_CHANGE_FAILURE);

  bus = gst_element_get_bus (playbin);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
                                    GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
  gst_message_unref (msg);
  gst_object_unref (bus);
  gst_element_set_state (playbin, GST_STATE_NULL);
  gst_object_unref (playbin);

  return 0;
}
