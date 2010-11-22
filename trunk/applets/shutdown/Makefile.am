## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/xml-config-files/*.xml $(srcdir)/logos/*.png
bin_PROGRAMS = gmshutdown
gmshutdown_SOURCES = main.c
gmshutdown_CPPFLAGS = @GTK_INCLUDES@
gmshutdown_CPPFLAGS += @LIBXML_INCLUDES@
gmshutdown_CPPFLAGS += @GM_INCLUDES@
gmshutdown_LDADD = @GM_OBJS@
gmshutdown_LDADD += @GTK_OBJS@
gmshutdown_LDADD += @LIBXML_OBJS@

