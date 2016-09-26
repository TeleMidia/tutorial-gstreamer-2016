void change_playback_rate (GstElement *element, double rate)
{
  gint64 cur_time;
  gst_element_query_position(element, GST_FORMAT_TIME, &cur_time);
  if (!gst_element_seek (element, rate, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
        GST_SEEK_TYPE_SET, cur_time, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE)) 
    g_print ("Change rate failed!\n");
}
