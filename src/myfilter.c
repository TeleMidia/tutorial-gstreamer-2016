#include "myfilter.h"

/* Cria as funcoes_my_filter_get_type e set gst_my_filter_parent_class */
G_DEFINE_TYPE (GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE (
  "sink",          /* A short name for the pad */
  GST_PAD_SINK,    /* pad direction */
  GST_PAD_ALWAYS,  /* Existence property. viz. always, sometimes, request */
  GST_STATIC_CAPS("ANY")); /* supported capability list */

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE (
  "src",
  GST_PAD_SRC,
  GST_PAD_ALWAYS,
  GST_STATIC_CAPS("ANY") );

static GstFlowReturn
gst_my_filter_chain (GstPad *pad, GstObject *parent, GstBuffer *buf)
{
  GstMyFilter *filter = GST_MY_FILTER (parent);

  g_print ("Have data of size %" G_GSIZE_FORMAT" bytes!\n",
        gst_buffer_get_size (buf));

  return gst_pad_push (filter->srcpad, buf);
}

static void
gst_my_filter_class_init(GstMyFilterClass *klass)
{
  GstElementClass *element_class = GST_ELEMENT_CLASS(klass);

  /* Define os metadados da classe */
  gst_element_class_set_static_metadata (
    element_class,
    "An example plugin",
    "Example/FirstExample",
    "Shows the basic structure of a plugin",
    "roberto robertogerson at telemidia.puc-rio.br");

  /* Adiciona um src e um sink padpara o elemento */
  gst_element_class_add_pad_template (element_class,
                                      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (element_class,
                                      gst_static_pad_template_get (&sink_factory));

}

static void
gst_my_filter_init (GstMyFilter *filter)
{
  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_chain_function (filter->sinkpad, GST_DEBUG_FUNCPTR(gst_my_filter_chain));
  GST_PAD_SET_PROXY_CAPS (filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT(filter), filter->sinkpad);

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  GST_PAD_SET_PROXY_CAPS (filter->srcpad);
  gst_element_add_pad (GST_ELEMENT(filter), filter->srcpad);
}

/* inicializacao do plugin */
static gboolean
myfilter_plugin_init (GstPlugin *myfilter)
{
  return gst_element_register(myfilter, "myfilter", GST_RANK_NONE, GST_TYPE_MY_FILTER);
}

#define PACKAGE "myfirstfilter"

GST_PLUGIN_DEFINE (
  GST_VERSION_MAJOR,
  GST_VERSION_MINOR,
  myfilter,
  "Template myfilter",
  myfilter_plugin_init, /* Funcao de inicializacao do plugin */ 
  "1.0.0", "LGPL", "TeleMidia/PUC-Rio", "http://www.telemidia.puc-rio.br")

