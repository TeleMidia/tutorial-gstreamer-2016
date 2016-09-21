#ifndef _MYFILTER_
#define _MYFILTER_

#include <gst/gst.h>

typedef struct _GstMyFilter {
  GstElement element;       /* Declaração da classe base. */
  GstPad *sinkpad, *srcpad; /* Declaração dos ponteiros para sink e src pad */
} GstMyFilter;

typedef struct _GstMyFilterClass {
  GstElementClass parent_class;  /* Declaração da classe base. */
} GstMyFilterClass;

/* Macros padrões para a definição de tipos para este elemento */
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

