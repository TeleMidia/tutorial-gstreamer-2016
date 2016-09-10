CC=gcc
CFLAGS=`pkg-config --cflags gstreamer-1.0` -Wall -Werror
LDFLAGS=`pkg-config --libs gstreamer-1.0`

hello00:
	$(CC) helloworld-00.c -o helloworld-00 $(CFLAGS) $(LDFLAGS)

hello01:
	$(CC) helloworld-01.c -o helloworld-01 $(CFLAGS) $(LDFLAGS)

all:
	hello00 hello01
