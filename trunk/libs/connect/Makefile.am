## Makefile.am -- Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgm_connect.la
include_HEADERS = gm_connect.h gm_connect-generic.h
libgm_connect_la_SOURCES = gm_connect.c gm_connect-generic.c
if WITH_DBUS_SUPPORT
include_HEADERS += gm_connect-dbus.h
libgm_connect_la_SOURCES += gm_connect-dbus.c
else
include_HEADERS += gm_connect-socket.h
libgm_connect_la_SOURCES += gm_connect-socket.c
endif
libgm_connect_la_CPPFLAGS = $(GTK_CFLAGS)
if WITH_DBUS_SUPPORT
libgm_connect_la_CPPFLAGS += $(DBUS_CFLAGS)
endif
libgm_connect_la_CPPFLAGS += -I$(top_builddir)/generic
if WITH_DBUS_SUPPORT
libgm_connect_la_LIBADD = $(DBUS_LIBS)
endif
libgm_connect_la_LDFLAGS = -version-info 0:1:0
