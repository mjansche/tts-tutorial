EXTRA_DIST = thraxgrm.vim thrax-mode.el
pkgdatadir=$(datarootdir)/thrax/utils

install-data-local: $(EXTRA_DIST)
	-mkdir -p -m 755 $(pkgdatadir)
	-cp -p $(srcdir)/thraxgrm.vim $(pkgdatadir)
	-cp -p $(srcdir)/thrax-mode.el $(pkgdatadir)

uninstall-local:
	-rm -rf $(pkgdatadir)
