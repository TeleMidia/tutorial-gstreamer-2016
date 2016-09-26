void seek (GstElement *element, gint64 offset)
{
  gint64 cur_time, to_time;
  gst_element_query_position(element, GST_FORMAT_TIME, &cur_time);
  to_time = cur_time + offset;
  if (!gst_element_seek_simple (element, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
        to_time)) 
    g_print ("Seek failed!\n");
}
