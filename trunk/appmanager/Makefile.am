## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4
noinst_HEADERS = listener.h appmanager.h
bin_PROGRAMS = gappman 
gappman_SOURCES = appmanager.c listener.c
gappman_CPPFLAGS = $(GTK_CFLAGS)
gappman_CPPFLAGS += $(GLIB_CFLAGS)
gappman_CPPFLAGS += $(LIBXML_CFLAGS)
gappman_CPPFLAGS += @GM_INCLUDES@
gappman_CPPFLAGS += -DSYSCONFDIR="\"${sysconfdir}\""
gappman_LDADD = @GM_OBJS@
gappman_LDADD += $(GLIB_LDFLAGS)
gappman_LDADD += $(GTK_LDFLAGS)
gappmandir = $(top_builddir)/src
