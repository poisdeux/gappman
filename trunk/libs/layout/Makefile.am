## Makefile.am -- Process this file with automake to produce Makefile.in
#noinst_LTLIBRARIES = libgm_layout.la
lib_LTLIBRARIES = libgm_layout.la
libgm_layout_la_SOURCES = gm_layout.c gm_changeresolution.c
include_HEADERS = gm_layout.h gm_changeresolution.h
libgm_layout_la_CPPFLAGS = @GTK_INCLUDES@
libgm_layout_la_CPPFLAGS += @LIBXML_INCLUDES@
libgm_layout_la_CPPFLAGS += -I$(top_builddir)/libs/parseconf
libgm_layout_la_CPPFLAGS += -I$(top_builddir)/libs/generic
libgm_layout_la_LIBADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la

