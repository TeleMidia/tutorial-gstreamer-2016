#include "myvideofilter.h"

/* Cria as funcoes_my_video_filter_get_type e set gst_my_video_filter_parent_class */
G_DEFINE_TYPE (GstMyVideoFilter, gst_my_video_filter, GST_TYPE_VIDEO_FILTER);

/* propriedades */
enum {
  PROP_0,
  PROP_USE_R,
  PROP_USE_G,
  PROP_USE_B
};

static void
gst_my_video_filter_set_property (GObject      *object,
	                          guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec);

static void
gst_my_video_filter_get_property (GObject    *object,
	                          guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec);

static GstStaticPadTemplate gst_my_video_filter_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE("{BGRx}"))
);

static GstStaticPadTemplate gst_my_video_filter_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
  GST_PAD_SINK,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE("{BGRx}"))
);

static GstFlowReturn
gst_my_video_transform_frame (GstVideoFilter *vfilter,
                              GstVideoFrame *inframe, GstVideoFrame *outframe)
{
  GstMyVideoFilter *my_vfilter = GST_MY_VIDEO_FILTER (vfilter);
  gint i, j, h;
  gint w, stride, row_wrap;
  gint pixel_stride;
  gint offsets[3];
  guint8 *data, *indata;
  guint outoffset, inoffset, u;

  /* gst_video_frame_copy(outframe, inframe); */
  indata = GST_VIDEO_FRAME_PLANE_DATA (inframe, 0);

  data = GST_VIDEO_FRAME_PLANE_DATA (outframe, 0);
  stride = GST_VIDEO_FRAME_PLANE_STRIDE (outframe, 0);
  w = GST_VIDEO_FRAME_COMP_WIDTH (outframe, 0);
  h = GST_VIDEO_FRAME_COMP_HEIGHT (outframe, 0);

  offsets[0] = GST_VIDEO_FRAME_COMP_OFFSET (outframe, 0);
  offsets[1] = GST_VIDEO_FRAME_COMP_OFFSET (outframe, 1);
  offsets[2] = GST_VIDEO_FRAME_COMP_OFFSET (outframe, 2);

  pixel_stride = GST_VIDEO_FRAME_COMP_PSTRIDE (outframe, 0);
  row_wrap = stride - pixel_stride * w;

  for (i = 0; i < h; i++)
  {
    for(j = 0; j < w; j++)
    {
/*      outoffset = (i * stride) + j * pixel_stride;
      inoffset = (i * stride) + ((int)(j + i*0.2) * pixel_stride); */

      outoffset = (i*stride) + (j*pixel_stride);
      u = i+20*sin(2*3.14*j/130);
      inoffset = (u*stride) + (j*pixel_stride);
      if (u < h)
      {
        memcpy (data + outoffset, indata + inoffset, pixel_stride);
      }

/*      data[offsets[0]] = (my_vfilter->use_r) ? indata[offsets[0]] : 0;
      data[offsets[1]] = (my_vfilter->use_g) ? indata[offsets[1]] : 0;
      data[offsets[2]] = (my_vfilter->use_b) ? indata[offsets[2]] : 0;*/

      /* anda para o prox. pixel */
      /*data += pixel_stride;
        indata += pixel_stride;*/
    }

    /* anda para a prox. linha */
    /*data += row_wrap;
      indata += row_wrap;*/
  }
 
  return GST_FLOW_OK;
}

static void
gst_my_video_filter_init (GstMyVideoFilter *filter)
{
  filter->use_r = filter->use_g = filter->use_b = FALSE;
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
    "roberto robertogerson at telemidia.puc-rio.br");

  /* adiciona os pads na classe */
  gst_element_class_add_pad_template (gstelement_class,
    gst_static_pad_template_get(&gst_my_video_filter_src_template));
  gst_element_class_add_pad_template (gstelement_class,
    gst_static_pad_template_get(&gst_my_video_filter_sink_template));

  /* define as funcoes virtuais para alterar propriedades */
  gobject_class->set_property = gst_my_video_filter_set_property;
  gobject_class->get_property = gst_my_video_filter_get_property;

  /* define propriedades */
  g_object_class_install_property(gobject_class, PROP_USE_R,
    g_param_spec_boolean ("use_r", "Use r",
                          "Se deve ou nao usar o canal vermelho.",
                          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property(gobject_class, PROP_USE_G,
    g_param_spec_boolean ("use_g", "Use g",
                          "Se deve ou nao usar o canal verde.",
                          FALSE, G_PARAM_READWRITE));

  g_object_class_install_property(gobject_class, PROP_USE_B,
    g_param_spec_boolean ("use_b", "Use b",
                          "Se deve ou nao usar o canal azul.",
                          FALSE, G_PARAM_READWRITE));

  /* define a funcao que será chamada para processar o frame de video */
  vfilter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_my_video_transform_frame);
}

static gboolean
my_video_filter_plugin_init (GstPlugin *myvideofilter)
{
  return gst_element_register(myvideofilter, "myvideofilter", GST_RANK_NONE, GST_TYPE_MY_VIDEO_FILTER);
}

static void
gst_my_video_filter_set_property (GObject      *object,
	                          guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (object);

  switch (prop_id) {
    case PROP_USE_R:
      filter->use_r = g_value_get_boolean (value);
      break;
    case PROP_USE_G:
      filter->use_g = g_value_get_boolean (value);
      break;
    case PROP_USE_B:
      filter->use_b = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_my_video_filter_get_property (GObject    *object,
	                          guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  GstMyVideoFilter *filter = GST_MY_VIDEO_FILTER (object);
                                                                                
  switch (prop_id) {
    case PROP_USE_R:
      g_value_set_boolean (value, filter->use_r);
      break;
    case PROP_USE_G:
      g_value_set_boolean (value, filter->use_g);
      break;
    case PROP_USE_B:
      g_value_set_boolean (value, filter->use_b);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

#define PACKAGE "myvideofilter"

GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  my_video_filter,
  "Template myvideofilter",
  my_video_filter_plugin_init, /* Funcao de inicializacao do plugin */ 
  "1.0.0", "LGPL", "TeleMidia/PUC-Rio", "http://www.telemidia.puc-rio.br")

