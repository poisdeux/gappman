## Makefile.am -- Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgm_netman.la
libgm_netman_la_SOURCES = gm_netman.c
libgm_netman_la_CPPFLAGS = @GTK_INCLUDES@
libgm_netman_la_CPPFLAGS += @LIBXML_INCLUDES@
libgm_netman_la_CPPFLAGS += -I$(top_builddir)/libs/parseconf
libgm_netman_la_LIBADD = @GTK_OBJS@
libgm_netman_la_LIBADD += $(top_builddir)/libs/parseconf/libgm_parseconf.la

