#include <gst/gst.h>
#include <gst/player/player.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  GstPlayer *play = gst_player_new();

  g_object_set(play, "uri", "../resources/big_bunny_1080p_stereo.ogg", NULL);

  gst_player_play(play);

  sleep(10000);

  return 0;
}

