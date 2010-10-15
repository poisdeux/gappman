## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = introspection

noinst_HEADERS = gm_netmand.h
bin_PROGRAMS = gm_netmand test_daemon
test_daemon_SOURCES = test_daemon.c
test_daemon_CPPFLAGS = @GTK_INCLUDES@
test_daemon_CPPFLAGS += @DBUS_INCLUDES@
test_daemon_LDADD = @GTK_OBJS@ 
test_daemon_LDADD += @DBUS_OBJS@ 
gm_netmand_SOURCES = gm_netmand.c 
gm_netmand_CPPFLAGS = @GTK_INCLUDES@
gm_netmand_CPPFLAGS += @DBUS_INCLUDES@
gm_netmand_LDADD = @GTK_OBJS@ 
gm_netmand_LDADD += @DBUS_OBJS@ 

BUILT_SOURCES = gm_netmand_glue.h

gm_netmand_glue.h: introspection/gm_netmand.xml
	dbus-binding-tool --prefix=gm_netmand --mode=glib-server --output=$@ $<

