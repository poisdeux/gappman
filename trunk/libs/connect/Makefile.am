## Makefile.am -- Process this file with automake to produce Makefile.in
lib_LTLIBRARIES = libgm_connect.la
libgm_connect_la_SOURCES = gm_connect.c 
include_HEADERS = gm_connect.h
libgm_connect_la_CPPFLAGS = $(GTK_CFLAGS)
libgm_connect_la_CPPFLAGS += -I$(top_builddir)/generic
libgm_connect_la_LIBADD = $(GTK_LDFLAGS)
libgm_connect_la_LDFLAGS = -version-info 0:0:0
