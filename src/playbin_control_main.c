#include <glib.h>
#include <gst/gst.h>

GMainLoop *loop;

gboolean bus_callback (GstBus *bus, GstMessage *msg, gpointer data);

int main (int argc, char *argv[])
{
  GstElement *playbin;
  char *uri;
  GstStateChangeReturn ret;
  GstBus *bus;
  guint watch_id;

  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);

  playbin = gst_element_factory_make ("playbin", "hello");
  g_assert (playbin);

  uri = gst_filename_to_uri ("bunny.ogg", NULL);
  g_assert_nonnull (uri);
  g_object_set (G_OBJECT (playbin), "uri", uri, NULL);
  g_free (uri);

  ret = gst_element_set_state (playbin, GST_STATE_PLAYING);
  g_assert (ret != GST_STATE_CHANGE_FAILURE);

  bus = gst_element_get_bus (playbin);
  watch_id = gst_bus_add_watch (bus, bus_callback, playbin);
  gst_object_unref (bus);

  g_main_loop_run (loop);

  gst_element_set_state (playbin, GST_STATE_NULL);
  gst_object_unref (playbin);
  g_source_remove (watch_id);
  g_main_loop_unref (loop);
  return 0;
}
