## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = gm_netmand
gm_netmand_SOURCES = gm_netmand.c
gm_netmand_CPPFLAGS = @GTK_INCLUDES@
gm_netmand_CPPFLAGS += @LIBXML_INCLUDES@
gm_netmand_CPPFLAGS += -I$(top_builddir)/libs/parseconf
gm_netmand_CPPFLAGS += -I$(top_builddir)/libs/layout
gm_netmand_CPPFLAGS += -I$(top_builddir)/libs/generic
gm_netmand_LDADD = $(top_builddir)/libs/parseconf/libgm_parseconf.la
gm_netmand_LDADD += $(top_builddir)/libs/layout/libgm_layout.la
gm_netmand_LDADD += @GTK_OBJS@
gm_netmand_LDADD += @LIBXML_OBJS@

