## Makefile.am -- Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgm_connect.la
libgm_connect_la_SOURCES = gm_connect.c 
if WITH_DBUS_SUPPORT
libgm_connect_la_SOURCES += gm_connect-dbus.c
else
libgm_connect_la_SOURCES += gm_connect-socket.c
endif
include_HEADERS = gm_connect.h
libgm_connect_la_CPPFLAGS = $(GTK_CFLAGS)
if WITH_DBUS_SUPPORT
libgm_connect_la_CPPFLAGS += $(DBUS_CFLAGS)
endif
libgm_connect_la_CPPFLAGS += -I$(top_builddir)/generic
if WITH_DBUS_SUPPORT
libgm_connect_la_LIBADD = $(DBUS_LIBS)
endif
libgm_connect_la_LDFLAGS = -version-info 0:0:0
