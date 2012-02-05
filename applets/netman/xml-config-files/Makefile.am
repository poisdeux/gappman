## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = netman.xml.in
sysconf_DATA = netman.xml

edit = sed -e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@libdir[@]|$(libdir)|g' \
	-e 's|@ [@]|$()|g' 
netman.xml: netman.xml.in Makefile
	$(edit) $(srcdir)/$@.in > $@

CLEANFILES = netman.xml

