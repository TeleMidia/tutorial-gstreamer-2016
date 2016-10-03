#include <glib.h>
#include <gst/gst.h>

typedef struct _GstMyFilter {
  GstElement parent_instance;
  GstPad *sinkpad, *srcpad;
} GstMyFilter;

#define GST_TYPE_MY_FILTER (gst_my_filter_get_type ())
G_DECLARE_FINAL_TYPE (GstMyFilter, gst_my_filter, GST, MY_FILTER, GstElement)
G_DEFINE_TYPE (GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT)

static GstStaticPadTemplate sink_template =
  GST_STATIC_PAD_TEMPLATE ("sink", GST_PAD_SINK,
                           GST_PAD_ALWAYS, GST_STATIC_CAPS ("ANY"));

static GstStaticPadTemplate src_template =
  GST_STATIC_PAD_TEMPLATE ("src", GST_PAD_SRC,
                           GST_PAD_ALWAYS, GST_STATIC_CAPS ("ANY"));

static GstFlowReturn gst_my_filter_chain (GstPad *pad, GstObject *obj, GstBuffer *buf)
{
  GstMyFilter *filter = GST_MY_FILTER (obj);
  g_print ("Processed %" G_GSIZE_FORMAT" bytes\n", gst_buffer_get_size (buf));
  return gst_pad_push (filter->srcpad, buf);
}

static void gst_my_filter_class_init (GstMyFilterClass *klass)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);
  gst_element_class_set_static_metadata
    (gstelement_class, "An example filter", "Example/myfilter",
     "Example filter", "roberto@telemidia.puc-rio.br");
  gst_element_class_add_pad_template
    (gstelement_class, gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template
    (gstelement_class, gst_static_pad_template_get (&sink_template));
}

static void gst_my_filter_init (GstMyFilter *filter)
{
  GstPad *sink = gst_pad_new_from_static_template (&sink_template, "sink");
  GstPad *src = gst_pad_new_from_static_template (&src_template, "src");
  gst_pad_set_chain_function (sink, GST_DEBUG_FUNCPTR (gst_my_filter_chain));
  GST_PAD_SET_PROXY_CAPS (sink);
  gst_element_add_pad (GST_ELEMENT (filter), sink);
  filter->sinkpad = sink;
  GST_PAD_SET_PROXY_CAPS (src);
  gst_element_add_pad (GST_ELEMENT (filter), src);
  filter->srcpad = src;
}

static gboolean my_filter_plugin_init (GstPlugin *plugin)
{
  return gst_element_register (plugin, "myfilter", GST_RANK_NONE, GST_TYPE_MY_FILTER);
}

#define PACKAGE "myfilter"
GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR, GST_VERSION_MINOR,
  myfilter, "Contains the myfilter element", my_filter_plugin_init,
  "1.0", "LGPL", "TeleMidia/PUC-Rio", "http://www.telemidia.puc-rio.br"
)
