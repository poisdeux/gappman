## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = introspection

ACLOCAL_AMFLAGS = -I m4
noinst_HEADERS = listener.h appmanager.h listener-dbus.h
bin_PROGRAMS = gappman 
gappman_SOURCES = appmanager.c listener.c 
if WITH_DBUS_SUPPORT
gappman_SOURCES += dbus-listener.c
endif
gappman_CPPFLAGS = $(DBUS_CFLAGS)
gappman_CPPFLAGS += $(GTK_CFLAGS)
gappman_CPPFLAGS += $(GLIB_CFLAGS)
gappman_CPPFLAGS += $(LIBXML_CFLAGS)
gappman_CPPFLAGS += @GM_INCLUDES@
gappman_CPPFLAGS += -DSYSCONFDIR="\"${sysconfdir}\""
gappman_LDADD = @GM_OBJS@
gappman_LDADD += $(DBUS_LIBS)
gappman_LDADD += $(GLIB_LIBS)
gappman_LDADD += $(GTHREAD_LIBS)
gappman_LDADD += $(GTK_LIBS)
gappman_LDADD += $(LIBXML_LIBS)
gappmandir = $(top_builddir)/src

BUILT_SOURCES = gm_listener_glue.h

gm_listener_glue.h: introspection/gm_appmanager.xml
	dbus-binding-tool --prefix=gm_appmanager --mode=glib-server --output=$@ $<

