EXTRA_DIST = byte.grm bytestringcopy.grm numbers.grm example.grm \
	ernest.txt assert.grm paradigms_and_features.grm pdt.grm mpdt.grm replace.grm \
	README
pkgdatadir=$(datarootdir)/thrax/grammars

install-data-local: $(EXTRA_DIST)
	-mkdir -p -m 755 $(pkgdatadir)
	-cp -p $(srcdir)/README $(pkgdatadir)
	-cp -p $(srcdir)/assert.grm $(pkgdatadir)
	-cp -p $(srcdir)/byte.grm $(pkgdatadir)
	-cp -p $(srcdir)/bytestringcopy.grm $(pkgdatadir)
	-cp -p $(srcdir)/example.grm $(pkgdatadir)
	-cp -p $(srcdir)/ernest.txt $(pkgdatadir)
	-cp -p $(srcdir)/mpdt.grm $(pkgdatadir)
	-cp -p $(srcdir)/numbers.grm $(pkgdatadir)
	-cp -p $(srcdir)/paradigms_and_features.grm $(pkgdatadir)
	-cp -p $(srcdir)/pdt.grm $(pkgdatadir)
	-cp -p $(srcdir)/replace.grm $(pkgdatadir)

uninstall-local:
	-rm -rf $(pkgdatadir)
