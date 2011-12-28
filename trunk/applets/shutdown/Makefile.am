## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/xml-config-files/shutdown.xml $(srcdir)/logos/*.png
bin_PROGRAMS = gmshutdown
gmshutdown_SOURCES = main.c
gmshutdown_CPPFLAGS = $(GTK_CFLAGS)
gmshutdown_CPPFLAGS += $(LIBXML_CFLAGS)
gmshutdown_CPPFLAGS += @GM_INCLUDES@
gmshutdown_LDADD = @GM_OBJS@
gmshutdown_LDADD += $(GTK_LIBS)
gmshutdown_LDADD += $(LIBXML_LIBS)

data_DATA = $(srcdir)/logos/*.png
sysconf_DATA = $(srcdir)/xml-config-files/shutdown.xml
