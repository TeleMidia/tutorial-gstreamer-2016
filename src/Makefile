TARGETS= hello
LIBS= glib-2.0 gstreamer-1.0
CFLAGS= -g -Wall -Wextra `pkg-config --cflags $(LIBS)`
LDFLAGS= `pkg-config --libs $(LIBS)`
all: $(TARGETS)
clean:
	-rm -f $(TARGETS)
.PHONY: all clean
