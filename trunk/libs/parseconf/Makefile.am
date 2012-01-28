## Makefile.am -- Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgm_parseconf.la
libgm_parseconf_la_SOURCES = gm_parseconf.c gm_parseconf.h
include_HEADERS = gm_parseconf.h
libgm_parseconf_la_CPPFLAGS = $(LIBXML_CFLAGS)
libgm_parseconf_la_CPPFLAGS += $(GTK_CFLAGS)
libgm_parseconf_la_CPPFLAGS += -I$(top_builddir)/generic
libgm_parseconf_la_LIBADD = $(LIBXML_LIBS)
libgm_parseconf_la_LIBADD += $(top_builddir)/generic/libgm_generic.la
libgm_parseconf_la_LIBADD += $(GTK_LIBS)
libgm_parseconf_la_LDFLAGS = -version-info 1:0:1
