docpkg = $(PACKAGE_TARNAME)-doxy-$(PACKAGE_VERSION).tar.gz

dist_doc_DATA = mainpage

doc_DATA = $(docpkg)

$(docpkg): doxygen.stamp
				tar chof - html | gzip -9 -c >$@

doxygen.stamp: Doxyfile mainpage
				$(DOXYGEN) $<
				echo stamp > $@

install-data-hook:
				cd $(DESTDIR)$(docdir) && tar xf $(docpkg)

uninstall-hook:
				cd $(DESTDIR)$(docdir) && rm -rf html

CLEANFILES = doxygen.stamp $(docpkg)

clean-local:
				rm -rf html latex

