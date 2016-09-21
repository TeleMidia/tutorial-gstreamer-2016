PROGRAMS= mp3 hello ogg uridecodebin_simple video_filter_v1 
MODULES= glib-2.0 gstreamer-1.0
CFLAGS= -g -Wall -Wextra `pkg-config --cflags $(MODULES)` -Wno-unused-parameter
LDFLAGS= `pkg-config --libs $(MODULES)`
all: $(PROGRAMS)
clean:
	-rm -f $(PROGRAMS)
.PHONY: all clean
