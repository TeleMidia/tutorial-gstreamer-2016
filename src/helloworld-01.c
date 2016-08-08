#include <gst/gst.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  GstElement *pipeline = NULL;
  GstElement *playbin = NULL;
  GstBus *bus = NULL;
  GstMessage *msg = NULL;
  GError *error = NULL;
  char *uri;
  
  /* Inicializa o GStreamer */
  gst_init (&argc, &argv);

  /* Cria um pipeline */
  pipeline = gst_pipeline_new ("pipeline");

  /* Cria um elemento playbin */
  playbin = gst_element_factory_make ("playbin", "playbin");

  /* Converte caminho relativo para URI */
  uri = gst_filename_to_uri ("../resources/big_buck_bunny_1080p_stereo.ogg", 
      &error);
  if (uri == NULL)
  {
    fprintf (stderr, "file not found\n");
    g_error_free (error);
    return 0;
  }

  /* Configura o playbin com a URI do vídeo */
  g_object_set (G_OBJECT(playbin), "uri", uri, NULL);

  /* Adiciona o playbin ao pipeline */
  gst_bin_add (GST_BIN(pipeline), playbin);

  /* Muda o estado do pipeline para PLAYING */
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Espera o fim do vídeo ou mensagem de erro */
  bus = gst_element_get_bus (pipeline);
  msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, 
      GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  /* Libera recursos */
  if (msg != NULL)
    gst_message_unref (msg);

  gst_object_unref (bus);
  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (pipeline);
  free (uri);

  return 0;
}
