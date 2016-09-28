#include <glib.h>
#include <gst/gst.h>
#include <gst/video/navigation.h>

static gboolean bus_cb (GstBus *bus, GstMessage *msg, gpointer data);

int main (int argc, char *argv[])
{
  GstElement *playbin;
  char *uri;
  GstStateChangeReturn ret;
  GstBus *bus;
  GMainLoop *loop;

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
  loop = g_main_loop_new (NULL, FALSE);
  g_object_set_data (G_OBJECT (bus), "pipeline", playbin);
  g_object_set_data (G_OBJECT (bus), "loop", loop);
  gst_bus_add_watch (bus, bus_cb, NULL);
  gst_object_unref (bus);

  g_main_loop_run (loop);       /* event loop */

  g_main_loop_unref (loop);
  gst_element_set_state (playbin, GST_STATE_NULL);
  gst_object_unref (playbin);
  return 0;
}

static void toggle_pause (GstElement *pipeline)
{
  switch (GST_STATE (pipeline))
    {
    case GST_STATE_PAUSED:
      gst_element_set_state (pipeline, GST_STATE_PLAYING);
      break;
    case GST_STATE_PLAYING:
      gst_element_set_state (pipeline, GST_STATE_PAUSED);
      break;
    }
}

static void seek (GstElement *pipeline, GstClockTimeDiff offset)
{
  GstClockTimeDiff from, to;
  gst_element_query_position (pipeline, GST_FORMAT_TIME, &from);
  to = MAX (from + offset, 0);
  gst_element_seek_simple (pipeline, GST_FORMAT_TIME,
                           GST_SEEK_FLAG_ACCURATE
                           | GST_SEEK_FLAG_FLUSH
                           | GST_SEEK_FLAG_TRICKMODE, to);
}

static void speed (GstElement *pipeline, double rate)
{
  GstClockTimeDiff from;
  GstSeekType start, stop;
  GstClockTimeDiff start_time, stop_time;

  gst_element_query_position (pipeline, GST_FORMAT_TIME, &from);
  if (rate >= 0.)
    {
      start = GST_SEEK_TYPE_SET;
      start_time = from;
      stop = GST_SEEK_TYPE_NONE;
      stop_time = GST_CLOCK_TIME_NONE;
    }
  else
    {
      start = GST_SEEK_TYPE_NONE;
      start_time = GST_CLOCK_TIME_NONE;
      stop = GST_SEEK_TYPE_SET;
      stop_time = from;
    }
  gst_element_seek (pipeline, rate, GST_FORMAT_TIME,
                    GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_TRICKMODE,
                    start, start_time, stop, stop_time);
}

static gboolean bus_cb (GstBus *bus, GstMessage *msg, gpointer data)
{
  GstElement *pipeline = g_object_get_data (G_OBJECT (bus), "pipeline");
  GMainLoop *loop = g_object_get_data (G_OBJECT (bus), "loop");
  switch (GST_MESSAGE_TYPE (msg))
    {
    case GST_MESSAGE_ERROR:
    case GST_MESSAGE_EOS:
      goto quit;
    case GST_MESSAGE_ELEMENT:
      {
        GstEvent *evt;
        const char *key;
        if (gst_navigation_message_get_type (msg) != GST_NAVIGATION_MESSAGE_EVENT)
          break;
        if (!gst_navigation_message_parse_event (msg, &evt))
          break;
        if (gst_navigation_event_get_type (evt) != GST_NAVIGATION_EVENT_KEY_PRESS)
          break;
        gst_navigation_event_parse_key_event (evt, &key);
        if (g_ascii_strcasecmp (key, "space") == 0)
          toggle_pause (pipeline);
        else if (g_ascii_strcasecmp (key, "right") == 0)
          seek (pipeline, 5 * GST_SECOND);
        else if (g_ascii_strcasecmp (key, "left") == 0)
          seek (pipeline, -5 * GST_SECOND);
        else if (g_ascii_strcasecmp (key, "f") == 0)
          speed (pipeline, 2.);
        else if (g_ascii_strcasecmp (key, "r") == 0)
          speed (pipeline, -2.);
        else if (g_ascii_strcasecmp (key, "n") == 0)
          speed (pipeline, 1.);
        else if (g_ascii_strcasecmp (key, "q") == 0)
          goto quit;
        break;
      }
    }
  return TRUE;
 quit:
  g_main_loop_quit (loop);
  return FALSE;
}
