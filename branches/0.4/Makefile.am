## Makefile.am -- Process this file with automake to produce Makefile.in
SUBDIRS = libs src applets
dist_doc_DATA = README
EXTRA_DIST = xml-config-files/*.xml logos/*.png ChangeLog applets/*/logos applets/*/xml-config-files

dist-hook:
	find -name "*~" -exec rm {} \;
