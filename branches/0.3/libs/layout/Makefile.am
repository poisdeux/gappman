## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_LTLIBRARIES = liblayout.la
liblayout_la_SOURCES = layout.c
include_HEADERS = layout.h
liblayout_la_CPPFLAGS = @GTK_INCLUDES@
liblayout_la_CPPFLAGS += @LIBXML_INCLUDES@
liblayout_la_LIBADD = @GTK_OBJS@
liblayout_la_LIBADD += $(top_builddir)/libs/parseconf/libparseconf.la

