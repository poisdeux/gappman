## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = changeresolution
changeresolution_SOURCES = main.c
changeresolution_CPPFLAGS = @GTK_INCLUDES@
changeresolution_CPPFLAGS += @LIBXML_INCLUDES@
changeresolution_LDADD = @GTK_OBJS@
changeresolution_LDADD += @LIBXML_OBJS@
changeresolution_LDADD += ../../libs/parseconf/libparseconf.la
changeresolution_LDADD += ../../libs/layout/liblayout.la
