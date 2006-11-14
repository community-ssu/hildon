MAINTAINERCLEANFILES = Makefile.in

INCLUDES = $(GTK_CFLAGS) $(GCONF_CFLAGS) -DLOCALEDIR=\"$(localedir)\" \
	   -I$(srcdir)/..

LDFLAGS = -module -avoid-version
LIBADD = -L$(srcdir)/../hildon-widgets/.libs -lhildonwidgets $(GTK_LIBS)

pluginwidgetdir = $(libdir)/hildon-widgets
pluginwidget_LTLIBRARIES = hildoncolorchooser_hsv.la \
			   hildoncolorchooserdialog_hsv.la

hildoncolorchooser_hsv_la_SOURCES = hildon-color-chooser-hsv.c

hildoncolorchooserdialog_hsv_la_SOURCES = hildon-color-chooser-dialog-hsv.c
hildoncolorchooserdialog_hsv_la_LIBADD = $(GCONF_LIBS)

install-exec-local: installdirs
	@echo "Creating default symlink for color chooser..."
	@ln -f -s 'hildoncolorchooser_hsv.so' '$(DESTDIR)/$(libdir)/hildon-widgets/hildoncolorchooser_default.so'
	@echo "Creating default symlink for color chooser dialog..."
	@ln -f -s 'hildoncolorchooserdialog_hsv.so' '$(DESTDIR)/$(libdir)/hildon-widgets/hildoncolorchooserdialog_default.so'





