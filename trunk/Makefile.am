## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4

if HAVE_DOXYGEN
DOXYDIR = docs/doxygen	
endif

if BUILD_GAPPMAN
GAPPMANDIR = appmanager
endif

if BUILD_LIBS
LIBSDIR = libs
endif

if BUILD_APPLETS
APPLETSDIR = applets
endif

SUBDIRS = $(LIBSDIR) $(GAPPMANDIR) $(APPLETSDIR) $(DOXYDIR)

dist_doc_DATA = README ChangeLog COPYING
dist_sysconf_DATA = gtk-config/gtkrc
EXTRA_DIST = $(srcdir)/gtk-config/gtkrc $(srcdir)/xml-config-files/*.xml $(srcdir)/logos/*.png $(srcdir)/ChangeLog $(srcdir)/debian

dist-hook:
	find $(distdir) -depth \( -name "*~" -o -name ".svn" \) -exec rm -rf {} \;

deb:
	cp ../gappman-$(PACKAGE_VERSION).tar.gz ../gappman_$(PACKAGE_VERSION).orig.tar.gz
	cd debian
	debuild -us -uc

