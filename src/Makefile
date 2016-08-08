CC=gcc
CFLAGS=`pkg-config --cflags gstreamer-1.0` -Wall -Werror
LDFLAGS=`pkg-config --libs gstreamer-1.0`

hello01:
	$(CC) helloworld-01.c -o helloworld-01 $(CFLAGS) $(LDFLAGS)

all:
	hello01
