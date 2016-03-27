project('dual-stream-switcher', 'c',
  version : '1.0',
  meson_version : '>= 0.27.0',
  default_options : [ 'warning_level=1',
                      'c_std=gnu99',
                      'buildtype=debug' ])
add_global_arguments('-fdiagnostics-color=always', language : 'c')

glib_dep = [dependency('glib-2.0'), dependency('gio-2.0')]
gtk_dep = dependency('gtk+-3.0', version : '>=3.10')
gst_dep = dependency('gstreamer-1.0', version : '>=1.6')
# For gtksink, but we don't need to link to it
dependency('gstreamer-plugins-bad-1.0', version : '>=1.6.1')

# Compile and link executable
dss_sources = [
  'gui/dss-lib.c',
  'gui/dss-app.c',
  'gui/dss-appwin.c',
  'gui/dss-main.c',
]

gnome = import('gnome')
dss_res = gnome.compile_resources('dss_res', 'gui/dss.gresource.xml',
  source_dir : 'gui',
  c_name : 'dss')

executable('dual-stream-switcher',
  dss_sources, dss_res,
  install: true,
  dependencies : glib_dep + [gtk_dep, gst_dep])
