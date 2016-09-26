gboolean bus_callback (GstBus *bus, GstMessage *msg, gpointer data)
{
  GstElement *playbin = (GstElement *) data;
  switch (GST_MESSAGE_TYPE (msg))
  {
    case GST_MESSAGE_ERROR: 
    {
      GError *err;
      gchar *debug;

      gst_message_parse_error (msg, &err, &debug);
      g_print ("Error: %s\n", err->message);
      g_print ("Debugging info: %s\n", (debug) ? debug: "none");
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
    case GST_MESSAGE_ELEMENT:
    {
      GstEvent *event = NULL;
      if (gst_navigation_message_get_type(msg) == GST_NAVIGATION_MESSAGE_EVENT
          && gst_navigation_message_parse_event (msg, &event))
      {
        if(gst_navigation_event_get_type (event) == GST_NAVIGATION_EVENT_KEY_PRESS)
        {
          const gchar *key;
          gst_navigation_event_parse_key_event (event, &key);
          if (strcmp (key, "space") == 0)
          {
            if (GST_STATE(playbin) == GST_STATE_PAUSED)
              gst_element_set_state (playbin, GST_STATE_PLAYING);
            else if (GST_STATE(playbin) == GST_STATE_PLAYING) 
              gst_element_set_state (playbin, GST_STATE_PAUSED);
          }
          else if (strcmp(key, "Right") == 0)
            seek (playbin, 5 * GST_SECOND); 
          else if (strcmp(key, "Left") == 0)
            seek (playbin, -5 * GST_SECOND); 
          else if (strcmp(key, "f") == 0)
            change_playback_rate (playbin, 2.0); 
          else if (strcmp(key, "r") == 0)
            change_playback_rate (playbin, -2.0); 
          else if (strcmp(key, "n") == 0)
            change_playback_rate (playbin, 1.0); 
        }
      }
      break;
    }
    default:
      break;
  }
  return TRUE;
}
