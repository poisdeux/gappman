## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = conf.xml.in
sysconf_DATA = conf.xml

edit = sed -e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@libdir[@]|$(libdir)|g' \
	-e 's|@ [@]|$()|g' 
conf.xml: conf.xml.in Makefile
	$(edit) $(srcdir)/$@.in > $@

CLEANFILES = conf.xml
