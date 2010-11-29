## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4
noinst_HEADERS = listener.h appmanager.h
bin_PROGRAMS = gappman 
gappman_SOURCES = appmanager.c listener.c
gappman_CPPFLAGS = @GTK_INCLUDES@
gappman_CPPFLAGS += @GTHREAD_INCLUDES@
gappman_CPPFLAGS += @LIBXML_INCLUDES@
gappman_CPPFLAGS += @GM_INCLUDES@
gappman_CPPFLAGS += -DSYSCONFDIR="\"${sysconfdir}\""
gappman_LDADD = @GM_OBJS@
gappman_LDADD += @GTHREAD_OBJS@
gappman_LDADD += @LIBXML_OBJS@
gappman_LDADD += @GTK_OBJS@
gappmandir = $(top_builddir)/src
