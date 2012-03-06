## Makefile.am -- Process this file with automake to produce Makefile.in
#noinst_LTLIBRARIES = libgm_keybinder.la
lib_LTLIBRARIES = libgm_keybinder.la
libgm_keybinder_la_SOURCES = bind.c
include_HEADERS = gm_keybinder.h
libgm_keybinder_la_CPPFLAGS = $(GTK_CFLAGS)
libgm_keybinder_la_LIBADD = $(GTK_LIBS)
libgm_keybinder_la_LDFLAGS = -version-info 1:0:0
libgm_pkgconfigdir = $(libdir)/pkgconfig
libgm_pkgconfig_DATA = keybinder.pc
