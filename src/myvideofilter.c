#include "myvideofilter.h"

/* Cria as funcoes_my_video_filter_get_type e set gst_my_video_filter_parent_class */
G_DEFINE_TYPE (GstMyVideoFilter, gst_my_video_filter, GST_TYPE_VIDEO_FILTER);

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
gst_my_video_transform_frame_ip (GstVideoFilter *vfilter,
                                 GstVideoFrame *inframe, GstVideoFrame *outframe)
{
  gint i, j, h;
  gint w, stride, row_wrap;
  gint pixel_stride;
  gint offsets[3];
  guint8 *data, *indata;

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

  for (i = 0; i < w; i++)
  {
    for(j = 0; j < h; j++)
    {
      data[offsets[0]] = indata[offsets[0]];
      data[offsets[1]] = 255;
      data[offsets[2]] = 255;

      data += pixel_stride;
      indata += pixel_stride;
    }
    data += row_wrap;
    indata += row_wrap;
  }
 
  return GST_FLOW_OK;
}

static void
gst_my_video_filter_class_init(GstMyVideoFilterClass *klass)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS(klass);
  GstVideoFilterClass *vfilter_class = GST_VIDEO_FILTER_CLASS(klass);

  /* Define os metadados da classe */
  gst_element_class_set_static_metadata (
    gstelement_class,
    "A VideoFilter example plugin",
    "Example/FirstExample",
    "Shows the basic structure of a plugin",
    "roberto robertogerson at telemidia.puc-rio.br");

  gst_element_class_add_pad_template (gstelement_class,
    gst_static_pad_template_get(&gst_my_video_filter_src_template));
  gst_element_class_add_pad_template (gstelement_class,
    gst_static_pad_template_get(&gst_my_video_filter_sink_template));

  vfilter_class->transform_frame =
    GST_DEBUG_FUNCPTR(gst_my_video_transform_frame_ip);
}

static void
gst_my_video_filter_init (GstMyVideoFilter *filter)
{

}

static gboolean
my_video_filter_plugin_init (GstPlugin *myvideofilter)
{
  return gst_element_register(myvideofilter, "myvideofilter", GST_RANK_NONE, GST_TYPE_MY_VIDEO_FILTER);
}

#define PACKAGE "myfirstfilter"

GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  my_video_filter,
  "Template myvideofilter",
  my_video_filter_plugin_init, /* Funcao de inicializacao do plugin */ 
  "1.0.0", "LGPL", "TeleMidia/PUC-Rio", "http://www.telemidia.puc-rio.br")

