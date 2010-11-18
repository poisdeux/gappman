## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = gmchangeresolution
gmchangeresolution_SOURCES = main.c
gmchangeresolution_CPPFLAGS = @GTK_INCLUDES@
gmchangeresolution_CPPFLAGS += @LIBXML_INCLUDES@
gmchangeresolution_CPPFLAGS += -I$(top_builddir)/../libs/layout 
gmchangeresolution_CPPFLAGS += -I$(top_builddir)/../libs/parseconf
gmchangeresolution_CPPFLAGS += -I$(top_builddir)/../libs/connect
gmchangeresolution_CPPFLAGS += -I$(top_builddir)/../libs/generic
gmchangeresolution_LDADD = $(top_builddir)/../libs/parseconf/libgm_parseconf.la
gmchangeresolution_LDADD += $(top_builddir)/../libs/layout/libgm_layout.la
gmchangeresolution_LDADD += $(top_builddir)/../libs/connect/libgm_connect.la
gmchangeresolution_LDADD += @GTK_OBJS@ 
gmchangeresolution_LDADD += @LIBXML_OBJS@
