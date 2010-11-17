## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/xml-config-files/*.xml $(srcdir)/logos/*.png
bin_PROGRAMS = gmshutdown
gmshutdown_SOURCES = main.c
gmshutdown_CPPFLAGS = @GTK_INCLUDES@
gmshutdown_CPPFLAGS += @LIBXML_INCLUDES@
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/parseconf
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/layout
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/connect
gmshutdown_CPPFLAGS += -I$(top_builddir)/libs/generic
gmshutdown_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
gmshutdown_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
gmshutdown_LDADD += $(top_builddir)/libs/connect/libgm_connect.la
gmshutdown_LDADD += @GTK_OBJS@
gmshutdown_LDADD += @LIBXML_OBJS@

