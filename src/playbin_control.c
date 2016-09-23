#include <glib.h>
#include <gst/gst.h>
#include <gst/video/navigation.h>
#include <string.h>

GMainLoop *loop;

void seek (GstElement *element, gint64 offset)
{
  gint64 cur_time, to_time;
  gst_element_query_position(element, GST_FORMAT_TIME, &cur_time);
  to_time = cur_time + offset;
  if (!gst_element_seek_simple (element, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
        to_time)) 
    g_print ("Seek failed!\n");
}

void change_rate (GstElement *element, double rate)
{
  gint64 cur_time;
  gst_element_query_position(element, GST_FORMAT_TIME, &cur_time);
  if (!gst_element_seek (element, rate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
        GST_SEEK_TYPE_SET, cur_time, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) 
    g_print ("Change rate failed!\n");
}

gboolean bus_callback (GstBus *bus, GstMessage *msg, gpointer data)
{
  GstElement *playbin = (GstElement *) data;
  switch (GST_MESSAGE_TYPE (msg))
  {
    case GST_MESSAGE_ELEMENT:
    {
      GstEvent *event = NULL;
      if (gst_navigation_message_get_type(msg) == GST_NAVIGATION_MESSAGE_EVENT
          && gst_navigation_message_parse_event (msg, &event))
      {
        GstNavigationEventType type = gst_navigation_event_get_type (event);
        switch (type) {
          case GST_NAVIGATION_EVENT_KEY_PRESS:
          {
            const gchar *key;
            gst_navigation_event_parse_key_event (event, &key);
            if (strcmp(key, "Right") == 0)
              seek (playbin, 5 * GST_SECOND); 
            else if (strcmp(key, "Left") == 0)
              seek (playbin, -5 * GST_SECOND); 
            else if (strcmp(key, "f") == 0)
              change_rate (playbin, 2.0); 
            else if (strcmp(key, "r") == 0)
              change_rate (playbin, -2.0); 
            else if (strcmp(key, "n") == 0)
              change_rate (playbin, 1.0); 
            break;
          }
          default:
            break;
        }
      }
      break;
    }
    case GST_MESSAGE_ERROR: 
      {
        GError *err;
        gchar *debug;

        gst_message_parse_error (msg, &err, &debug);
        g_print ("Error: %s\n", err->message);
        g_error_free (err);
        g_free (debug);

        g_main_loop_quit (loop);
        break;
      }
    case GST_MESSAGE_EOS:
      {
        g_main_loop_quit (loop);
        break;
      }
    default:
      break;
  }

  return TRUE;
}

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
