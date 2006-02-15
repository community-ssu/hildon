SUBDIRS = hildon-widgets timer ut doc 
EXTRA_DIST = \
	debian/changelog \
	debian/control \
	debian/copyright \
	debian/hildon-libs-dev.docs \
	debian/hildon-libs-dev.install \
	debian/hildon-libs0.install \
	debian/hildon-libs0-dbg.install \
	debian/rules

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hildon-libs.pc

distclean-am: 
	-rm -rf autom4te.cache 
	-rm -f config.status config.cache config.log configure.lineno  
#	-rm -f config.guess config.h.in config.sub configure depcomp
#	-rm -f install-sh ltmain.sh missing mkinstalldirs Makefile.in aclocal.m4

deb: dist
	-mkdir debian-build
	cd debian-build && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot
