## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = parseconf.h
bin_PROGRAMS = shutdown
shutdown_SOURCES = main.c parseconf.c
shutdown_CPPFLAGS = @GTK_INCLUDES@
shutdown_CPPFLAGS += @LIBXML_INCLUDES@
shutdown_LDADD = @GTK_OBJS@
shutdown_LDADD += @LIBXML_OBJS@
shutdown_LDADD += ../../libs/parseconf/libparseconf.la
shutdowndir = ../src
shutdown.$(OBJEXT): parseconf.o

