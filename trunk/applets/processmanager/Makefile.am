## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/logos/*.png
noinst_HEADERS = connect.h
bin_PROGRAMS = gmprocessmanager
gmprocessmanager_SOURCES = main.c connect.c
gmprocessmanager_CPPFLAGS = @GTK_INCLUDES@
gmprocessmanager_CPPFLAGS += @LIBXML_INCLUDES@
gmprocessmanager_CPPFLAGS += @GM_INCLUDES@
gmprocessmanager_LDADD = @GM_OBJS@
gmprocessmanager_LDADD += @GTK_OBJS@
gmprocessmanager_LDADD += @LIBXML_OBJS@

