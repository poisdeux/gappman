## Makefile.am -- Process this file with automake to produce Makefile.in
#noinst_LTLIBRARIES = libgm_layout.la
lib_LTLIBRARIES = libgm_layout.la
libgm_layout_la_SOURCES = gm_layout.c gm_changeresolution.c
include_HEADERS = gm_layout.h gm_changeresolution.h
libgm_layout_la_CPPFLAGS = $(GTK_CFLAGS)
libgm_layout_la_CPPFLAGS += $(LIBXML_CFLAGS)
libgm_layout_la_CPPFLAGS += $(XRANDR_CFLAGS)
libgm_layout_la_CPPFLAGS += -I$(top_builddir)/parseconf
libgm_layout_la_CPPFLAGS += -I$(top_builddir)/generic
libgm_layout_la_LIBADD = $(top_builddir)/parseconf/libgm_parseconf.la
libgm_layout_la_LIBADD += $(GTK_LIBS)
libgm_layout_la_LIBADD += $(LIBXML_LIBS)
libgm_layout_la_LIBADD += $(XRANDR_LIBS)
libgm_layout_la_LDFLAGS = -version-info 0:0:0
