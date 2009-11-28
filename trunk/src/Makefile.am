## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = changeresolution.h listener.h
bin_PROGRAMS = gappman 
gappman_SOURCES = changeresolution.c appmanager.c listener.c
# appmanager_HEADERS = parseconf.h changeresolution.h
# appmanager_CFLAGS = @CFLAGS@
gappman_CPPFLAGS = @GTK_INCLUDES@
gappman_CPPFLAGS += @LIBXML_INCLUDES@
gappman_CPPFLAGS += -I$(top_builddir)/libs/parseconf
gappman_CPPFLAGS += -I$(top_builddir)/libs/layout
gappman_LDADD = @GTK_OBJS@
gappman_LDADD += @LIBXML_OBJS@
gappman_LDADD += $(top_builddir)/libs/parseconf/libgm_parseconf.la
gappman_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
gappmandir = $(top_builddir)/src
