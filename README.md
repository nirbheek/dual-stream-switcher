Intro
=====
Dual Stream Switcher is a sample project that uses
[GStreamer](http://gstreamer.freedesktop.org/) to switch between
two files when a key is pressed. [GTK+](http://www.gtk.org/) is used for
rendering the video and for event handling.

[Meson](https://github.com/mesonbuild/meson/) is used for building, and since
GStreamer and GTK+ are cross-platform, this means this can be built and run on
Linux, OS X, and Windows.

Building and running:
=====================

```
  $ ./build.sh
  $ cd build/
  $ ./dual-stream-switcher 1.mp4 2.mp4
```

Press `F` to fullscreen. Hold down `R` to switch the video. Releasing `R` will
switch back to the original video.

1.mp4 and 2.mp4 are sample video files from the Honda Civic R ad which inspired
this project:

http://digitalsynopsis.com/advertising/honda-civic-type-r-the-other-side/
