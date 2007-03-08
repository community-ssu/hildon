languages := @ALL_LINGUAS@

CLEANFILES 				= $(foreach mo,$(languages),$(mo).mo)
EXTRA_DIST 				= $(foreach po,$(languages),$(po).po) POTFILES.in README

MAINTAINERCLEANFILES 			= Makefile.in POTFILES

%.mo: %.po
	msgfmt --statistics -v -o$(top_builddir)/po/$@ $(srcdir)/$<

all: $(foreach mo,$(languages),$(mo).mo)

merge:
	for mo in $(languages); do \
		echo "Merging $$mo..."; \
		msgmerge -v $$mo.po hildon-libs.pot >messages.tmp; \
		mv messages.tmp $$mo.po; \
		msgfmt --statistics $$mo.po -o /dev/null; \
	done

install-data-local: all
	for mo in $(languages); do \
		$(mkinstalldirs) $(DESTDIR)$(localedir)/$$mo/LC_MESSAGES; \
		$(INSTALL_DATA) $(top_builddir)/po/$$mo.mo $(DESTDIR)$(localedir)/$$mo/LC_MESSAGES/$(PACKAGE).mo; \
	done

uninstall-local:
	for mo in $(languages); do \
		rm -f $(DESTDIR)$(localedir)/$$mo/LC_MESSAGES/$(PACKAGE).mo; \
	done
