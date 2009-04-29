ACLOCAL_AMFLAGS                         = -I m4
SUBDIRS 				= src 						\
					  pkgconfig 					\
					  examples 					\
					  tests						\
					  doc						\
					  po

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

