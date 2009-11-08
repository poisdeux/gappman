## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = shutdown
shutdown_SOURCES = main.c
shutdown_CPPFLAGS = @GTK_INCLUDES@
shutdown_CPPFLAGS += @LIBXML_INCLUDES@
shutdown_CPPFLAGS += -I$(top_builddir)/libs/parseconf
shutdown_CPPFLAGS += -I$(top_builddir)/libs/layout
shutdown_CPPFLAGS += -I$(top_builddir)/libs/connect
shutdown_LDADD = @GTK_OBJS@
shutdown_LDADD += @LIBXML_OBJS@
shutdown_LDADD += $(top_builddir)/libs/parseconf/libparseconf.la
shutdown_LDADD += $(top_builddir)/libs/layout/liblayout.la
shutdown_LDADD += $(top_builddir)/libs/connect/libgm_connect.la

