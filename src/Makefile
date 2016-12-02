PROGRAMS= 01-hello \
          02-mp3 \
          03-ogg \
          04-filter \
          05-controls \
          06-uridecodebin_simple \
          07-video_filter_v1 \
          08-playbin_control
MODULES= glib-2.0 gstreamer-1.0 gstreamer-video-1.0
CFLAGS= -g -Wall `pkg-config --cflags $(MODULES)`\
  -Wno-unused-parameter -Wno-switch
LDLIBS= `pkg-config --libs $(MODULES)`
all: $(PROGRAMS)
clean:
	-rm -f $(PROGRAMS)
.PHONY: all clean
