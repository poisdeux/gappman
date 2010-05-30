## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = connect.h
bin_PROGRAMS = gmprocessmanager
gmprocessmanager_SOURCES = main.c connect.c
gmprocessmanager_CPPFLAGS = @GTK_INCLUDES@
gmprocessmanager_CPPFLAGS += @LIBXML_INCLUDES@
gmprocessmanager_CPPFLAGS += -I$(top_builddir)/libs/parseconf
gmprocessmanager_CPPFLAGS += -I$(top_builddir)/libs/layout
gmprocessmanager_CPPFLAGS += -I$(top_builddir)/libs/connect
gmprocessmanager_CPPFLAGS += -I$(top_builddir)/libs/generic
gmprocessmanager_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
gmprocessmanager_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
gmprocessmanager_LDADD += $(top_builddir)/libs/connect/libgm_connect.la

