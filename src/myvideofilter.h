#ifndef _MYVIDEOFILTER_
#define _MYVIDEOFILTER_

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

typedef struct _GstMyVideoFilter {
  GstVideoFilter videoFilter;       /* Declaracao da classe base. */
  gboolean use_r, use_g, use_b;
} GstMyVideoFilter;

typedef struct _GstMyVideoFilterClass {
  GstVideoFilterClass parent_class;  /* Declaracao da classe base. */
} GstMyVideoFilterClass;

/* Macros padrões para a definição de tipos para este elemento */
#define GST_TYPE_MY_VIDEO_FILTER (gst_my_video_filter_get_type())
#define GST_MY_VIDEO_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MY_VIDEO_FILTER, GstMyVideoFilter))
#define GST_MY_VIDEO_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MY_VIDEO_FILTER, GstMyVideoFilterClass))
#define GST_IS_MY_VIDEO_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MY_VIDEO_FILTER))
#define GST_IS_MY_VIDEO_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MY_VIDEO_FILTER))

GType gst_my_video_filter_get_type (void);

#endif

