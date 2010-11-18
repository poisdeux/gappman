## Makefile.am -- Process this file with automake to produce Makefile.in
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = libs appmanager applets
dist_doc_DATA = README
EXTRA_DIST = gtk-config/gtkrc xml-config-files/*.xml logos/*.png ChangeLog applets/*/logos applets/*/xml-config-files

dist-hook:
	find -name "*~" -exec rm {} \;
