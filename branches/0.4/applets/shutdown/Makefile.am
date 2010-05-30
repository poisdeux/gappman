## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = gmshutdown
gmshutdown_SOURCES = main.c
gmshutdown_CPPFLAGS = @GTK_INCLUDES@
gmshutdown_CPPFLAGS += @LIBXML_INCLUDES@
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/parseconf
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/layout
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/generic
gmshutdown_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
gmshutdown_LDADD += $(top_builddir)/libs/layout/libgm_layout.la

