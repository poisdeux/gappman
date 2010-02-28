## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = processmanager
processmanager_SOURCES = main.c connect.c
processmanager_CPPFLAGS = @GTK_INCLUDES@
processmanager_CPPFLAGS += @LIBXML_INCLUDES@
processmanager_CPPFLAGS += -I$(top_builddir)/libs/parseconf
processmanager_CPPFLAGS += -I$(top_builddir)/libs/layout
processmanager_CPPFLAGS += -I$(top_builddir)/libs/connect
processmanager_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
processmanager_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
processmanager_LDADD += $(top_builddir)/libs/connect/libgm_connect.la

