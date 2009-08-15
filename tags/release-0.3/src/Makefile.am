## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = parseconf.h changeresolution.h
bin_PROGRAMS = appmanager test
appmanager_SOURCES = changeresolution.c parseconf.c appmanager.c
# appmanager_HEADERS = parseconf.h changeresolution.h
# appmanager_CFLAGS = @CFLAGS@
appmanager_CPPFLAGS = @GTK_INCLUDES@
appmanager_CPPFLAGS += @LIBXML_INCLUDES@
appmanager_LDADD = @GTK_OBJS@
appmanager_LDADD += @LIBXML_OBJS@
appmanagerdir = ../src
appmanager.$(OBJEXT): parseconf.o

test_CPPFLAGS = @GTK_INCLUDES@
test_LDADD = @GTK_OBJS@
test_SOURCES = changeresolution.c test.c
test.$(OBJEXT): changeresolution.o


