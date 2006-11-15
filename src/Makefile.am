INCLUDES 						= -DG_LOG_DOMAIN=\"hildon-widgets\" \
							  -DLOCALEDIR=\"$(localedir)\"

MAINTAINERCLEANFILES 					= Makefile.in 			\
							  hildon-enum-types.c 	\
							  hildon-enum-types.h	\
							  hildon-marshalers.h 	\
							  hildon-marshalers.c

EXTRA_DIST 						= hildon-marshalers.list

lib_LTLIBRARIES 					= libhildonwidgets.la

libhildonwidgets_la_LDFLAGS 				= $(HILDON_WIDGETS_LT_LDFLAGS)

libhildonwidgets_la_LIBADD 				= $(GTK_LIBS) 			\
							  $(GCONF_LIBS) 		\
							  $(ESD_LIBS)			\
							  $(LIBMB_LIBS)

libhildonwidgets_la_CFLAGS				= $(GTK_CFLAGS) 		\
							  $(GCONF_CFLAGS) 		\
							  $(ESD_CFLAGS)			\
							  $(LIBMB_CFLAGS)

libhildonwidgets_la_SOURCES 				= hildon-composite-widget.c \
							  hildon-controlbar.c \
							  hildon-seekbar.c \
							  hildon-color-selector.c \
							  hildon-note.c \
							  hildon-volumebar.c \
							  hildon-volumebar-range.c \
							  hildon-hvolumebar.c \
							  hildon-vvolumebar.c \
							  hildon-calendar-popup.c \
							  hildon-date-editor.c \
							  hildon-time-editor.c \
							  hildon-time-picker.c \
							  hildon-weekday-picker.c \
							  hildon-number-editor.c \
							  hildon-range-editor.c \
							  hildon-get-password-dialog.c \
							  hildon-set-password-dialog.c \
							  hildon-sort-dialog.c \
							  hildon-font-selection-dialog.c \
							  hildon-name-password-dialog.c \
							  hildon-wizard-dialog.c \
							  hildon-color-popup.c \
							  hildon-color-button.c \
							  hildon-defines.c \
							  hildon-find-toolbar.c \
							  gtk-infoprint.c \
							  hildon-banner.c \
							  hildon-caption.c \
							  hildon-window.c \
							  hildon-program.c \
							  hildon-code-dialog.c \
							  hildon-plugin-widget.c \
							  hildon-color-chooser.c \
							  hildon-color-chooser-dialog.c \
							  hildon-color-chooser-button.c \
							  hildon-enum-types.c \
							  hildon-marshalers.c

libhildonwidgets_includedir				= $(includedir)/hildon-widgets

libhildonwidgets_include_HEADERS 			= hildon-controlbar.h \
							  hildon-note.h \
							  hildon-seekbar.h \
							  hildon-color-selector.h \
							  hildon-volumebar.h \
							  hildon-hvolumebar.h \
							  hildon-vvolumebar.h \
							  hildon-calendar-popup.h \
							  hildon-date-editor.h \
							  hildon-time-editor.h \
							  hildon-time-picker.h \
							  hildon-weekday-picker.h \
							  hildon-number-editor.h \
							  hildon-range-editor.h \
							  hildon-get-password-dialog.h \
							  hildon-set-password-dialog.h \
							  hildon-sort-dialog.h \
							  hildon-font-selection-dialog.h \
							  hildon-name-password-dialog.h \
							  hildon-wizard-dialog.h \
							  hildon-color-popup.h \
							  hildon-color-button.h \
							  hildon-defines.h \
							  hildon-find-toolbar.h \
							  gtk-infoprint.h \
							  hildon-banner.h \
							  hildon-input-mode-hint.h \
							  hildon-caption.h \
							  hildon-window.h \
							  hildon-program.h \
							  hildon-window-private.h \
							  hildon-code-dialog.h \
							  hildon-plugin-widget.h \
							  hildon-color-chooser.h \
							  hildon-color-chooser-dialog.h \
							  hildon-color-chooser-button.h \
							  hildon-enum-types.h \
							  hildon-marshalers.h

# Don't build the library until we have built the header that it needs:
$(OBJECTS) $(libhildonwidgets_la_OBJECTS): 		  hildon-enum-types.h

hildon-enum-types.h: 					  @REBUILD@ $(libhildonwidgets_include_HEADERS) Makefile
	(cd $(srcdir) && glib-mkenums \
			--fhead "#ifndef __HILDON_ENUM_TYPES_H__\n" 	\
			--fhead "#define __HILDON_ENUM_TYPES_H__\n" 	\
			--fhead "\n" 						\
			--fhead "#include <glib-object.h>\n"			\
			--fhead "\n"						\
			--fhead "G_BEGIN_DECLS\n" 				\
			--fhead "\n"						\
			--fprod "/* Enumerations from \"@filename@\" */\n" 	\
			--vhead "GType @enum_name@_get_type (void);\n#define HILDON_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "\nG_END_DECLS\n\n#endif /* __HILDON_ENUM_TYPES_H__ */" \
		$(libhildonwidgets_include_HEADERS)) > $@

hildon-enum-types.c: 					  @REBUILD@ $(libhildonwidgets_include_HEADERS) Makefile
	(cd $(srcdir) && glib-mkenums \
			--fhead "#include \"hildon-widgets.h\"\n" \
			--fhead "#include <glib-object.h>" \
		      	--fprod "\n/* enumerations from \"@filename@\" */" \
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(libhildonwidgets_include_HEADERS)) > $@

hildon-marshalers.h: 					hildon-marshalers.list
	glib-genmarshal --prefix _hildon_marshal --header hildon-marshalers.list >hildon-marshalers.h

hildon-marshalers.c: 					hildon-marshalers.list hildon-marshalers.h
	echo '#include "hildon-marshalers.h"' >hildon-marshalers.c
	glib-genmarshal --prefix _hildon_marshal --body hildon-marshalers.list >>hildon-marshalers.c

