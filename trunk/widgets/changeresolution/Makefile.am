## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = changeresolution
changeresolution_SOURCES = main.c
changeresolution_CPPFLAGS = @GTK_INCLUDES@
changeresolution_CPPFLAGS += @LIBXML_INCLUDES@
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/layout 
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/parseconf
changeresolution_CPPFLAGS += -I$(top_builddir)/libs/connect
changeresolution_LDADD = @GTK_OBJS@
changeresolution_LDADD += @LIBXML_OBJS@
changeresolution_LDADD += $(top_builddir)/libs/parseconf/libparseconf.la
changeresolution_LDADD += $(top_builddir)/libs/layout/liblayout.la
changeresolution_LDADD += $(top_builddir)/libs/connect/libgm_connect.la
