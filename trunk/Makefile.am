## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = libs appmanager applets
dist_doc_DATA = README
EXTRA_DIST = $(srcdir)/gtk-config/gtkrc $(srcdir)/xml-config-files/*.xml $(srcdir)/logos/*.png $(srcdir)/ChangeLog

dist-hook:
	find $(distdir) -depth \( -name "*~" -o -name ".svn" \) -exec rm -rf {} \;
