#!/bin/sh
case "$2" in
0) af='volume volume=2';                       vf='coloreffects preset=1';;
1) af='pitch pitch=.5';                        vf='rippletv';;
2) af='audioecho delay=500000000 intensity=1'; vf='shagadelictv';;
3) af='freeverb room-size=1 level=1';          vf='edgetv';;
4) af='equalizer-3bands band0=12 band1=-24';   vf='revtv';;
*) ;;
esac
if test -z "$1" || test -z "$vf" || test -z "$af" ; then
  echo >&2 "usage: ${0##*/} URI [0-4]"
  exit 1
fi
gst-launch-1.0 uridecodebin uri="$1" name=decbin\
  decbin. ! $af ! audioconvert ! autoaudiosink\
  decbin. ! videoconvert ! $vf ! videoconvert ! autovideosink
