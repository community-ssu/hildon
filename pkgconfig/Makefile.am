MAINTAINERCLEANFILES			= Makefile.in

all-local: hildon-@API_VERSION_MAJOR@.pc

hildon-@API_VERSION_MAJOR@.pc: hildon.pc
	cp $< $@

pkgconfigdir				= $(libdir)/pkgconfig
pkgconfig_DATA				= hildon-@API_VERSION_MAJOR@.pc

EXTRA_DIST				= hildon.pc.in
CLEANFILES				= hildon-@API_VERSION_MAJOR@.pc

