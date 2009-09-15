## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_LTLIBRARIES = libparseconf.la
libparseconf_la_SOURCES = parseconf.c parseconf.h
include_HEADERS = parseconf.h
libparseconf_la_CPPFLAGS = @LIBXML_INCLUDES@
libparseconf_la_CPPFLAGS += @GTK_INCLUDES@
libparseconf_la_LIBADD = @LIBXML_OBJS@
libparseconf_la_LIBADD += @GTK_OBJS@
