## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/logos/*.png
bin_PROGRAMS = gmprocessmanager
gmprocessmanager_SOURCES = main.c
gmprocessmanager_CPPFLAGS = $(GTK_CFLAGS)
gmprocessmanager_CPPFLAGS += $(LIBXML_CFLAGS)
gmprocessmanager_CPPFLAGS += @GM_INCLUDES@
gmprocessmanager_LDADD = @GM_OBJS@
gmprocessmanager_LDADD += $(GTK_LIBS)
gmprocessmanager_LDADD += $(LIBXML_LIBS)

