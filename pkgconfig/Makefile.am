MAINTAINERCLEANFILES			= Makefile.in

all-local: hildon-@PACKAGE_VERSION_MAJOR@.pc

hildon-@PACKAGE_VERSION_MAJOR@.pc: hildon.pc
	cp $< $@

pkgconfigdir				= $(libdir)/pkgconfig
pkgconfig_DATA				= hildon-@PACKAGE_VERSION_MAJOR@.pc

EXTRA_DIST				= hildon.pc.in
CLEANFILES				= hildon-@PACKAGE_VERSION_MAJOR@.pc

