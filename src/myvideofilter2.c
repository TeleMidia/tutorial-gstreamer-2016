#include <math.h>
#include <string.h>
#include <glib.h>
#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

typedef struct _GstMyVideoFilter {
  GstVideoFilter parent_instance;
  gdouble factor;
} GstMyVideoFilter;

enum { PROP_0, PROP_FACTOR };

#define GST_TYPE_MY_VIDEO_FILTER (gst_my_video_filter_get_type ())
G_DECLARE_FINAL_TYPE (GstMyVideoFilter, gst_my_video_filter,
                      GST, MY_VIDEO_FILTER, GstVideoFilter)
G_DEFINE_TYPE (GstMyVideoFilter, gst_my_video_filter, GST_TYPE_VIDEO_FILTER)

static GstStaticPadTemplate sink_template =
  GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                           GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{BGRx, RGB}")));

static GstStaticPadTemplate src_template =
  GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
                           GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("{BGRx, RGB}")));

static void gst_my_video_filter_get_property (GObject *obj, guint id,
                                              GValue *val, GParamSpec *pspec)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (obj);
  switch (id)
    {
    case PROP_FACTOR:
      g_value_set_double (val, filter->factor);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, id, pspec);
    }
}

static void gst_my_video_filter_set_property (GObject *obj, guint id,
                                              const GValue *val, GParamSpec *pspec)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (obj);
  switch (id)
    {
    case PROP_FACTOR:
      filter->factor = g_value_get_double (val);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, id, pspec);
    }
}

static GstFlowReturn gst_my_video_transform (GstVideoFilter *vf, GstVideoFrame *in,
                                             GstVideoFrame *out)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (vf);
  gint i, j, w, h;
  gint stride, pixel_stride;
  guint8 *data, *indata;
  guint outoffset, inoffset, u;

  indata = GST_VIDEO_FRAME_PLANE_DATA (in, 0);
  data = GST_VIDEO_FRAME_PLANE_DATA (out, 0);
  stride = GST_VIDEO_FRAME_PLANE_STRIDE (out, 0);
  w = GST_VIDEO_FRAME_COMP_WIDTH (out, 0);
  h = GST_VIDEO_FRAME_COMP_HEIGHT (out, 0);
  pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE (out, 0);
  for (i = 0; i < h; i++)
    {
      for(j = 0; j < w; j++)
        {
          outoffset = (i * stride) + (j * pixel_stride);
          u = i + 20 * sin (filter->factor * j);
          inoffset = (u * stride) + (j * pixel_stride);
          if (u < h)
            memcpy (data + outoffset, indata + inoffset, pixel_stride);
        }
    }
  return GST_FLOW_OK;
}

static void gst_my_video_filter_class_init (GstMyVideoFilterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);
  GstVideoFilterClass *gstvideofilter_class = GST_VIDEO_FILTER_CLASS (klass);

  gobject_class->set_property = gst_my_video_filter_set_property;
  gobject_class->get_property = gst_my_video_filter_get_property;
  g_object_class_install_property
    (gobject_class, PROP_FACTOR, g_param_spec_double
     ("factor", "factor", "distortion factor",
      0.0, 320.0, 2*G_PI/130, G_PARAM_READWRITE));

  gst_element_class_set_static_metadata
    (gstelement_class, "An example video filter", "Example/myvideofilter",
     "Example filter", "roberto@telemidia.puc-rio.br");
  gst_element_class_add_pad_template
    (gstelement_class, gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template
    (gstelement_class, gst_static_pad_template_get (&sink_template));

  gstvideofilter_class->transform_frame = GST_DEBUG_FUNCPTR (gst_my_video_transform);
}

static void gst_my_video_filter_init (GstMyVideoFilter *filter)
{
  filter->factor = 2*G_PI/130;
}

static gboolean my_video_filter_plugin_init (GstPlugin *plugin)
{
  return gst_element_register (plugin, "myvideofilter",
                               GST_RANK_NONE, GST_TYPE_MY_VIDEO_FILTER);
}

#define PACKAGE "myvideofilter"
GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR, GST_VERSION_MINOR,
  myvideofilter, "Contains the myvideofilter element", my_video_filter_plugin_init,
  "1.0", "LGPL", "TeleMidia/PUC-Rio", "http://www.telemidia.puc-rio.br"
)
