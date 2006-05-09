INCLUDES = $(GTK_CFLAGS) $(GNOME_VFS_CFLAGS) $(GCONF_CFLAGS) \
	   $(ESD_CFLAGS) $(LIBMB_CFLAGS) -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/..

lib_LTLIBRARIES = libhildonwidgets.la

EXTRA_DIST = hildon-marshalers.list

libhildonwidgets_la_LDFLAGS = -version-info 5:0:5

libhildonwidgets_la_LIBADD = $(GTK_LIBS) $(GNOME_VFS_LIBS) $(GCONF_LIBS) \
			     $(ESD_LIBS) $(LIBMB_LIBS)

libhildonwidgets_la_SOURCES = \
	hildon-marshalers.c \
	hildon-marshalers.h \
	hildon-composite-widget.c \
	hildon-composite-widget.h \
	hildon-controlbar.c \
	hildon-controlbar.h \
	hildon-seekbar.c \
	hildon-seekbar.h \
	hildon-color-selector.c \
	hildon-color-selector.h \
	hildon-note.c \
	hildon-note.h \
	hildon-volumebar.c \
	hildon-volumebar.h \
	hildon-volumebar-range.c \
	hildon-volumebar-range.h \
	hildon-volumebar-private.h \
	hildon-hvolumebar.c \
	hildon-hvolumebar.h \
	hildon-vvolumebar.c \
	hildon-vvolumebar.h \
	hildon-dialoghelp.c \
	hildon-dialoghelp.h \
	hildon-calendar-popup.c \
	hildon-calendar-popup.h \
	hildon-date-editor.c \
	hildon-date-editor.h \
	hildon-time-editor.c \
	hildon-time-editor.h \
	hildon-private.h \
	hildon-time-picker.c \
	hildon-time-picker.h \
	hildon-weekday-picker.c \
	hildon-weekday-picker.h \
	hildon-telephone-editor.c \
	hildon-telephone-editor.h \
	hildon-number-editor.c \
	hildon-number-editor.h \
	hildon-range-editor.c \
	hildon-range-editor.h \
	hildon-get-password-dialog.c \
	hildon-get-password-dialog.h \
	hildon-set-password-dialog.c \
	hildon-set-password-dialog.h \
	hildon-sort-dialog.c \
	hildon-sort-dialog.h \
	hildon-add-home-dialog.c \
	hildon-add-home-dialog.h \
	hildon-font-selection-dialog.c \
	hildon-font-selection-dialog.h \
	hildon-insert-object-dialog.c \
	hildon-insert-object-dialog.h \
	hildon-grid.c \
	hildon-grid.h \
	hildon-grid-item.c \
	hildon-grid-item.h \
	hildon-grid-item-private.h \
	hildon-file-handling-note.c \
	hildon-file-handling-note.h \
	hildon-name-password-dialog.c \
	hildon-name-password-dialog.h \
	hildon-scroll-area.c \
	hildon-scroll-area.h \
	hildon-wizard-dialog.c \
	hildon-wizard-dialog.h \
	hildon-color-popup.c \
	hildon-color-popup.h \
	hildon-color-button.c \
	hildon-color-button.h \
	hildon-system-sound.c \
	hildon-system-sound.h \
	hildon-app.c \
	hildon-app.h \
	hildon-defines.c \
	hildon-defines.h \
	hildon-appview.c \
	hildon-appview.h \
	hildon-find-toolbar.c \
	hildon-find-toolbar.h \
	gtk-infoprint.c \
	gtk-infoprint.h \
	hildon-banner.c \
	hildon-banner.h \
	hildon-input-mode-hint.h \
	hildon-app-private.h \
	hildon-caption.c \
	hildon-caption.h \
	hildon-window.c \
	hildon-window.h \
	hildon-window-private.h \
	hildon-program.c \
	hildon-program.h \
	hildon-code-dialog.c \
	hildon-code-dialog.h \
	$(hildonlibs_built_headers) \
	$(hildonlibs_built_cfiles)


hildon-marshalers.h: hildon-marshalers.list
	glib-genmarshal --prefix _hildon_marshal --header \
			hildon-marshalers.list >hildon-marshalers.h

hildon-marshalers.c: hildon-marshalers.list hildon-marshalers.h
	echo '#include <hildon-widgets/hildon-marshalers.h>' >hildon-marshalers.c
	glib-genmarshal --prefix _hildon_marshal --body \
			hildon-marshalers.list >>hildon-marshalers.c

hildonwidgetsincludeinstdir=$(includedir)/hildon-widgets
hildonwidgetsincludeinst_DATA = \
	hildon-controlbar.h \
	hildon-note.h \
	hildon-seekbar.h \
	hildon-color-selector.h \
	hildon-volumebar.h \
	hildon-hvolumebar.h \
	hildon-vvolumebar.h \
	hildon-dialoghelp.h \
	hildon-calendar-popup.h \
	hildon-date-editor.h \
	hildon-time-editor.h \
	hildon-time-picker.h \
	hildon-weekday-picker.h \
	hildon-telephone-editor.h \
	hildon-number-editor.h \
	hildon-range-editor.h \
	hildon-get-password-dialog.h \
	hildon-set-password-dialog.h \
	hildon-sort-dialog.h \
	hildon-add-home-dialog.h \
	hildon-font-selection-dialog.h \
	hildon-insert-object-dialog.h \
	hildon-grid.h \
	hildon-grid-item.h \
	hildon-file-handling-note.h \
	hildon-name-password-dialog.h \
	hildon-scroll-area.h \
	hildon-wizard-dialog.h \
	hildon-color-popup.h \
	hildon-color-button.h \
	hildon-system-sound.h \
	hildon-app.h \
	hildon-defines.h \
	hildon-appview.h \
	hildon-find-toolbar.h \
	gtk-infoprint.h \
	hildon-banner.h \
	hildon-input-mode-hint.h \
	hildon-app-private.h \
	hildon-caption.h \
	hildon-window.h \
	hildon-program.h \
	hildon-window-private.h \
	hildon-code-dialog.h

headers_to_scan_for_enums = $(hildonwidgetsincludeinst_DATA)
 
# Generate the enums source code, with glib-mkenums:
# This is based on the same Makefile.am stuff in pango:
hildonlibs_built_headers = hildon-libs-enum-types.h
hildonlibs_built_cfiles = hildon-libs-enum-types.c
 
# Don't build the library until we have built the header that it needs:
$(OBJECTS) $(libhildonwidgets_la_OBJECTS): $(hildonlibs_built_headers)
 
hildon-libs-enum-types.h: @REBUILD@ $(headers_to_scan_for_enums) Makefile
	(cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef __HILDON_LIBS_ENUM_TYPES_H__\n#define __HILDON_LIBS_ENUM_TYPES_H__\n\n#include <glib-object.h>\n\nG_BEGIN_DECLS\n" \
			--fprod "/* enumerations from \"@filename@\" */\n" \
			--vhead "GType @enum_name@_get_type (void);\n#define HILDON_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" 	\
			--ftail "G_END_DECLS\n\n#endif /* __HILDON_LIBS_ENUM_TYPES_H__ */" \
		$(headers_to_scan_for_enums)) > $@

#FIXME: This would be shorter if there was a hildon-libs.h file:
hildon-libs-enum-types.c: @REBUILD@ $(headers_to_scan_for_enums) Makefile
	(cd $(srcdir) && glib-mkenums \
			--fhead "#include <hildon-widgets/hildon-app.h>\n" \
			--fhead "#include <hildon-widgets/hildon-input-mode-hint.h>\n" \
			--fhead "#include <hildon-widgets/hildon-caption.h>\n" \
			--fhead "#include <hildon-widgets/hildon-date-editor.h>\n" \
			--fhead "#include <hildon-widgets/hildon-number-editor.h>\n" \
			--fhead "#include <hildon-widgets/hildon-telephone-editor.h>\n" \
			--fhead "#include <hildon-widgets/hildon-note.h>\n" \
			--fhead "#include <hildon-widgets/hildon-grid.h>\n" \
			--fhead '#include "hildon-libs-enum-types.h"\n' \
			--fhead "#include <glib-object.h>" \
		      	--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(headers_to_scan_for_enums)) > $@
