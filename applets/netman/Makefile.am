## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = xml-config-files
EXTRA_DIST = $(srcdir)/xml-config-files/netman.xml 
EXTRA_DIST += $(srcdir)/logos
noinst_HEADERS = parseconf.h
lib_LTLIBRARIES = gm_netman.la
gm_netman_la_SOURCES = main.c parseconf.c 
gm_netman_la_CPPFLAGS = $(GTK_CFLAGS)
gm_netman_la_CPPFLAGS += $(LIBXML_CFLAGS)
gm_netman_la_CPPFLAGS += $(DBUS_CFLAGS)
gm_netman_la_CPPFLAGS += @GM_INCLUDES@
gm_netman_la_LIBADD = @GM_OBJS@
gm_netman_la_LIBADD += $(GTK_LIBS) 
gm_netman_la_LIBADD += $(LIBXML_LIBS) 
gm_netman_la_LIBADD += $(DBUS_LIBS) 
gm_netman_la_LDFLAGS = -module -avoid-version

data_DATA = logos/*.png
sysconf_DATA = xml-config-files/netman.xml
