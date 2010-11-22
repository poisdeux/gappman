## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/logos/*.png
bin_PROGRAMS = gmchangeresolution
gmchangeresolution_SOURCES = main.c
gmchangeresolution_CPPFLAGS = @GTK_INCLUDES@
gmchangeresolution_CPPFLAGS += @LIBXML_INCLUDES@
gmchangeresolution_CPPFLAGS += @GM_INCLUDES@
gmchangeresolution_LDADD = @GM_OBJS@
gmchangeresolution_LDADD += @GTK_OBJS@ 
gmchangeresolution_LDADD += @LIBXML_OBJS@
