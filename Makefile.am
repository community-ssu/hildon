SUBDIRS 				= src 						\
					  pkgconfig 					\
					  examples 					\
					  tests						\
					  doc

EXTRA_DIST 				= debian/changelog 				\
					  debian/control 				\
					  debian/copyright 				\
					  debian/hildon-libs-dev.docs 			\
					  debian/hildon-libs-dev.install 		\
					  debian/hildon-libs0.install 			\
					  debian/hildon-libs0-dbg.install 		\
					  debian/rules

MAINTAINERCLEANFILES			= Makefile.in aclocal.m4 configure 		\
			  		  config.h.in compile 				\
			  		  ltmain.sh depcomp missing 		    	\
			  		  install-sh config.sub 		    	\
			  		  config.guess intltool-extract.in 	    	\
			  		  intltool-merge.in intltool-update.in      	\
			  		  mkinstalldirs build-stamp configure-stamp

deb: dist
	-mkdir debian-build
	cd debian-build && \
	tar zxvf ../$(PACKAGE)-$(VERSION).tar.gz && \
	cd $(PACKAGE)-$(VERSION) && dpkg-buildpackage -rfakeroot

