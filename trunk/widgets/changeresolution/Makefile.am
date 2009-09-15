## Makefile.am -- Process this file with automake to produce Makefile.in
bin_PROGRAMS = changeresolution
processmanager_SOURCES = main.c
processmanager_CPPFLAGS = @GTK_INCLUDES@
processmanager_CPPFLAGS += @LIBXML_INCLUDES@
processmanager_LDADD = @GTK_OBJS@
processmanager_LDADD += @LIBXML_OBJS@
processmanager_LDADD += ../../libs/parseconf/libparseconf.la
processmanager_LDADD += ../../libs/layout/liblayout.la
