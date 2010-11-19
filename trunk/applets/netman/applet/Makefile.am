## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = nm_parseconf.h
lib_LTLIBRARIES = gm_netman.la
gm_netman_la_SOURCES = nm_main.c nm_parseconf.c 
gm_netman_la_CPPFLAGS = @GTK_INCLUDES@
gm_netman_la_CPPFLAGS += @LIBXML_INCLUDES@
gm_netman_la_CPPFLAGS += @DBUS_INCLUDES@
gm_netman_la_CPPFLAGS += -I$(top_builddir)/../libs/layout
gm_netman_la_CPPFLAGS += -I$(top_builddir)/../libs/parseconf
gm_netman_la_CPPFLAGS += -I$(top_builddir)/../libs/generic
gm_netman_la_LIBADD = $(top_builddir)/../libs/parseconf/libgm_parseconf.la
gm_netman_la_LIBADD += @GTK_OBJS@ 
gm_netman_la_LIBADD += @LIBXML_OBJS@ 
gm_netman_la_LIBADD += @DBUS_OBJS@ 
gm_netman_la_LDFLAGS = -module -avoid-version
