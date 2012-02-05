## Makefile.am -- Process this file with automake to produce Makefile.in
EXTRA_DIST = shutdown.xml.in
sysconf_DATA = shutdown.xml

edit = sed -e 's|@bindir[@]|$(bindir)|g' \
	-e 's|@sysconfdir[@]|$(sysconfdir)|g' \
	-e 's|@datadir[@]|$(datadir)|g' \
	-e 's|@libdir[@]|$(libdir)|g' \
	-e 's|@ [@]|$()|g' 
shutdown.xml: shutdown.xml.in Makefile
	$(edit) $(srcdir)/$@.in > $@

CLEANFILES = shutdown.xml

