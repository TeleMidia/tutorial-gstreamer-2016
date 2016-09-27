#include "myvideofilter.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

/* Cria as funcoes_my_video_filter_get_type e set gst_my_video_filter_parent_class */
G_DEFINE_TYPE (GstMyVideoFilter, gst_my_video_filter, GST_TYPE_VIDEO_FILTER);

static GstStaticPadTemplate gst_my_video_filter_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE("{BGRx, RGB}"))
);

static GstStaticPadTemplate gst_my_video_filter_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE("{BGRx, RGB}"))
);

/* propriedades */
#define DEFAULT_FACTOR 2*3.1415 / 130

enum {
  PROP_0,
  PROP_WAVE_FACTOR
};

static void
gst_my_video_filter_set_property (GObject *object, guint prop_id, const GValue *value,
                                  GParamSpec *pspec)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (object);

  switch (prop_id) {
    case PROP_WAVE_FACTOR:
      filter->factor = g_value_get_double (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_my_video_filter_get_property (GObject  *object, guint prop_id, GValue *value,
                                  GParamSpec *pspec)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (object);
                                                                                
  switch (prop_id) {
    case PROP_WAVE_FACTOR:
      g_value_set_double (value, filter->factor);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static GstFlowReturn
gst_my_video_transform_frame (GstVideoFilter *vfilter,
                              GstVideoFrame *inframe, GstVideoFrame *outframe)
{
  GstMyVideoFilter *my_vfilter = GST_MY_VIDEO_FILTER (vfilter);
  gint i, j, w, h;
  gint stride, pixel_stride;
  guint8 *data, *indata;
  guint outoffset, inoffset, u;

  indata = GST_VIDEO_FRAME_PLANE_DATA (inframe, 0);

  data = GST_VIDEO_FRAME_PLANE_DATA (outframe, 0);
  stride = GST_VIDEO_FRAME_PLANE_STRIDE (outframe, 0);
  w = GST_VIDEO_FRAME_COMP_WIDTH (outframe, 0);
  h = GST_VIDEO_FRAME_COMP_HEIGHT (outframe, 0);

  pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE (outframe, 0);

  for (i = 0; i < h; i++)
  {
    for(j = 0; j < w; j++)
    {
      outoffset = (i*stride) + (j*pixel_stride);
      u = i + 20*sin(my_vfilter->factor * j);
      inoffset = (u*stride) + (j*pixel_stride);

      if (u < h)
        memcpy (data + outoffset, indata + inoffset, pixel_stride);
    }
  }
 
  return GST_FLOW_OK;
}

static void
gst_my_video_filter_class_init(GstMyVideoFilterClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS(klass);
  GstVideoFilterClass *vfilter_class = GST_VIDEO_FILTER_CLASS(klass);

  /* Define os metadados da classe */
  gst_element_class_set_static_metadata (
    gstelement_class,
    "A VideoFilter example plugin",
    "Example/FirstExample",
    "Shows the basic structure of a plugin",
    "TeleMidia/PUC-Rio");

  /* adiciona os pads na classe */
  gst_element_class_add_pad_template (gstelement_class,
    gst_static_pad_template_get(&gst_my_video_filter_src_template));
  gst_element_class_add_pad_template (gstelement_class,
    gst_static_pad_template_get(&gst_my_video_filter_sink_template));

  /* define as funcoes virtuais para alterar propriedades */
  gobject_class->set_property = gst_my_video_filter_set_property;
  gobject_class->get_property = gst_my_video_filter_get_property;

  /* define propriedades */
  g_object_class_install_property(gobject_class, PROP_WAVE_FACTOR,
    g_param_spec_double ("factor", "Factor",
                         "Valor do fator",
                         0.0, 320.0,
                         DEFAULT_FACTOR,
                         G_PARAM_READWRITE));

  /* define a funcao que será chamada para processar o frame de video */
  vfilter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_my_video_transform_frame);
}

static void
gst_my_video_filter_init (GstMyVideoFilter *filter)
{
  filter->factor =  DEFAULT_FACTOR;
}

static gboolean
my_video_filter_plugin_init (GstPlugin *myvideofilter)
{
  return gst_element_register (myvideofilter, "myvideofilter",
                               GST_RANK_NONE, GST_TYPE_MY_VIDEO_FILTER);
}

#define PACKAGE "myvideofilter"

GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  my_video_filter,
  "Template myvideofilter",
  my_video_filter_plugin_init, /* Funcao de inicializacao do plugin */ 
  "1.0.0", "LGPL", "TeleMidia/PUC-Rio", "http://www.telemidia.puc-rio.br")

