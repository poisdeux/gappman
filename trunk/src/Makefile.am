## Makefile.am -- Process this file with automake to produce Makefile.in
noinst_HEADERS = changeresolution.h listener.h
bin_PROGRAMS = gappman test 
gappman_SOURCES = changeresolution.c appmanager.c listener.c
# appmanager_HEADERS = parseconf.h changeresolution.h
# appmanager_CFLAGS = @CFLAGS@
gappman_CPPFLAGS = @GTK_INCLUDES@
gappman_CPPFLAGS += @LIBXML_INCLUDES@
gappman_LDADD = @GTK_OBJS@
gappman_LDADD += @LIBXML_OBJS@
gappman_LDADD += $(top_builddir)/libs/parseconf/libparseconf.la
gappman_LDADD += $(top_builddir)/libs/layout/liblayout.la
gappmandir = ../src

test_CPPFLAGS = @GTK_INCLUDES@
test_LDADD = @GTK_OBJS@
test_SOURCES = changeresolution.c test.c
test.$(OBJEXT): changeresolution.o

testlistenerdir = ../src
testlistener_PROGRAMS = testlistener
testlistener_CPPFLAGS = @GTK_INCLUDES@
testlistener_LDADD = @GTK_OBJS@
testlistener_SOURCES = test-listener.c
testlistener.$(OBJEXT): test-listener.o

