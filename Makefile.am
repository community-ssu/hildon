SUBDIRS = hildon-widgets timer doc 
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


#luc: will do this properly later,
clean-all:
	rm -rf configure Makefile.in config.log depcomp config.guess config.h ltmain.sh config.sub gtk-doc.make hildon-libs.pc Makefile stamp-h.in mkinstalldirs config.status stamp-h1 config.h.in libtool autom4te.cache missing aclocal.m4 install-sh timer/Makefile.in hildon-widgets/hildon-libs-enum-types.c hildon-widgets/hildon-libs-enum-types.h hildon-widgets/Makefile.in hildon-widgets/.deps hildon-widgets/.libs debian/build doc/hildon-libs-decl-list.txt doc/xml doc/Makefile.in doc/version.xml doc/html doc/hildon-libs-decl.txt doc/hildon-libs-sections.txt doc/hildon-libs-undocumented.txt ut/Makefile.in ut/.libs ut/.deps ut/Makefile po/porules.mk po/Makefile.in po/POTFILES po/Makefile timer/.deps timer/Makefile hildon-widgets/Makefile doc/Makefile build-stamp timer/libtimer.a debian/hildon-libs0.substvars debian/tmp debian/files debian/hildon-libs0 debian/hildon-libs0-dbg
