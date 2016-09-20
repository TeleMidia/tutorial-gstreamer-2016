#ifndef _SIMPLEAUDIOFILTER_
#define _SIMPLEAUDIOFILTER_

#include <gst/gst.h>

/* Definition of strucutre storing data for this element. */
t	
typedef struct _GstMyFilter {
  GstElement element;       // Base class data storage
  GstPad *sinkpad, *srcpad; // Sink and src pad pointer declaration
  // gboolean test_arg;        // Place holder for storing valur of argument
} GstMyFilter;

typedef struct _GstMyFilterClass {
  GstElementClass parent_class;  // Base class declaration
} GstMyFilterClass;

/* Standard macros for defining types for this element.  */
#define GST_TYPE_MY_FILTER (gst_my_filter_get_type())
#define GST_MY_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MY_FILTER,GstMyFilter))
#define GST_MY_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MY_FILTER,GstMyFilterClass))
#define GST_IS_MY_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MY_FILTER))
#define GST_IS_MY_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MY_FILTER))

GType gst_my_filter_get_type (void);

#endif

