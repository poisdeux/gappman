## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = shutdown
shutdown_SOURCES = main.c
shutdown_CPPFLAGS = @GTK_INCLUDES@
shutdown_CPPFLAGS += @LIBXML_INCLUDES@
shutdown_CPPFLAGS += -I$(top_builddir)/libs/parseconf
shutdown_CPPFLAGS += -I$(top_builddir)/libs/layout
shutdown_CPPFLAGS += -I$(top_builddir)/libs/connect
shutdown_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
shutdown_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
shutdown_LDADD += $(top_builddir)/libs/connect/libgm_connect.la

