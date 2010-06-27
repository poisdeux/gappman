## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = listener.h appmanager.h
bin_PROGRAMS = gappman 
gappman_SOURCES = appmanager.c listener.c
gappman_CPPFLAGS = @GTK_INCLUDES@
gappman_CPPFLAGS += @GTHREAD_INCLUDES@
gappman_CPPFLAGS += @LIBXML_INCLUDES@
gappman_CPPFLAGS += @XRANDR_INCLUDES@
gappman_CPPFLAGS += -I$(top_builddir)/libs/parseconf
gappman_CPPFLAGS += -I$(top_builddir)/libs/layout
gappman_CPPFLAGS += -I$(top_builddir)/libs/generic
gappman_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
gappman_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
gappmandir = $(top_builddir)/src
