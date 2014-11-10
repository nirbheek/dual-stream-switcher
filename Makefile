CC				?= gcc
CFLAGS				?= -O2 -Wall
LDFLAGS 			?= 
LIBS				 = $(shell pkg-config --libs --cflags gstreamer-1.0)

DUAL_STREAM_SWITCHER		 = dual-stream-switcher
DUAL_STREAM_SWITCHER_GTK	 = dual-stream-switcher-gtk

$(DUAL_STREAM_SWITCHER): $(DUAL_STREAM_SWITCHER).c
	$(CC) $< $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@

all: $(DUAL_STREAM_SWITCHER)

clean: $(DUAL_STREAM_SWITCHER)
