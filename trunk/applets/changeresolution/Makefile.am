## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = changeresolution
changeresolution_SOURCES = main.c
changeresolution_CPPFLAGS = @GTK_INCLUDES@
changeresolution_CPPFLAGS += @LIBXML_INCLUDES@
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/layout 
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/parseconf
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/connect
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/generic
changeresolution_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
changeresolution_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
changeresolution_LDADD += $(top_builddir)/libs/connect/libgm_connect.la
