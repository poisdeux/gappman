## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = $(srcdir)/xml-config-files/digitalclock.xml
noinst_HEADERS = parseconf.h
lib_LTLIBRARIES = gm_digitalclock.la
gm_digitalclock_la_SOURCES = main.c parseconf.c
gm_digitalclock_la_CPPFLAGS = $(GTK_CFLAGS)
gm_digitalclock_la_CPPFLAGS += @GM_INCLUDES@
gm_digitalclock_la_CPPFLAGS += $(LIBXML_CFLAGS)
gm_digitalclock_la_LIBADD = $(GTK_LIBS)
gm_digitalclock_la_LIBADD += $(LIBXML_LIBS) 
gm_digitalclock_la_LDFLAGS = -module -avoid-version

