## Makefile.am -- Process this file with automake to produce Makefile.in
# noinst_HEADERS = 
bin_PROGRAMS = shutdown
shutdown_SOURCES = main.c
shutdown_CPPFLAGS = @GTK_INCLUDES@
shutdown_CPPFLAGS += @LIBXML_INCLUDES@
shutdown_LDADD = @GTK_OBJS@
shutdown_LDADD += @LIBXML_OBJS@
shutdowndir = ../src
shutdown.$(OBJEXT): parseconf.o

