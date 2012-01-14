## Makefile.am -- Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgm_network.la
include_HEADERS = gm_network.h gm_network-generic.h
libgm_network_la_SOURCES = gm_network.c gm_network-generic.c
if WITH_DBUS_SUPPORT
include_HEADERS += gm_network-dbus.h
libgm_network_la_SOURCES += gm_network-dbus.c
else
include_HEADERS += gm_network-socket.h
libgm_network_la_SOURCES += gm_network-socket.c
endif
libgm_network_la_CPPFLAGS = $(GTK_CFLAGS)
if WITH_DBUS_SUPPORT
libgm_network_la_CPPFLAGS += $(DBUS_CFLAGS)
endif
libgm_network_la_CPPFLAGS += -I$(top_builddir)/generic
if WITH_DBUS_SUPPORT
libgm_network_la_LIBADD = $(DBUS_LIBS)
endif
libgm_network_la_LDFLAGS = -version-info 1:0:0
