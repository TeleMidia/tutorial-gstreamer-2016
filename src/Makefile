PROGRAMS= mp3 hello ogg filter controls uridecodebin_simple\
          video_filter_v1 playbin_control
MODULES= glib-2.0 gstreamer-1.0 gstreamer-video-1.0
CFLAGS= -g -Wall `pkg-config --cflags $(MODULES)`\
  -Wno-unused-parameter -Wno-switch
LDLIBS= `pkg-config --libs $(MODULES)`
all: $(PROGRAMS)
clean:
	-rm -f $(PROGRAMS)
.PHONY: all clean

# $(PROGRAMS):
# 	$(CC) $(CFLAGS) -o $@ $(LDLIBS) $(@:=.c)
