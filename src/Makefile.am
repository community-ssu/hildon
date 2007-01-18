INCLUDES 						= -DG_LOG_DOMAIN=\"hildon-@PACKAGE_VERSION_MAJOR@\" \
							  -DLOCALEDIR=\"$(localedir)\"

MAINTAINERCLEANFILES 					= Makefile.in 					\
							  hildon-enum-types.c 				\
							  hildon-enum-types.h				\
							  hildon-marshalers.h 				\
							  hildon-marshalers.c

EXTRA_DIST 						= hildon-marshalers.list

lib_LTLIBRARIES 					= libhildon_@PACKAGE_VERSION_MAJOR@.la

libhildon_@PACKAGE_VERSION_MAJOR@_la_LDFLAGS 		= $(HILDON_WIDGETS_LT_LDFLAGS)

libhildon_@PACKAGE_VERSION_MAJOR@_la_LIBADD 		= $(GTK_LIBS) 					\
							  $(GCONF_LIBS) 				\
							  $(ESD_LIBS)			

libhildon_@PACKAGE_VERSION_MAJOR@_la_CFLAGS		= $(GTK_CFLAGS) 				\
							  $(GCONF_CFLAGS) 				\
							  $(ESD_CFLAGS)	

libhildon_@PACKAGE_VERSION_MAJOR@_la_SOURCES 		= hildon-private.c				\
							  hildon-controlbar.c 				\
							  hildon-seekbar.c 				\
							  hildon-note.c 				\
							  hildon-volumebar.c 				\
							  hildon-sound.c 				\
							  hildon-volumebar-range.c 			\
							  hildon-hvolumebar.c 				\
							  hildon-vvolumebar.c 				\
							  hildon-calendar-popup.c 			\
							  hildon-date-editor.c 				\
							  hildon-time-editor.c 				\
							  hildon-time-picker.c 				\
							  hildon-helper.c				\
							  hildon-weekday-picker.c 			\
							  hildon-number-editor.c 			\
							  hildon-range-editor.c 			\
							  hildon-get-password-dialog.c 			\
							  hildon-set-password-dialog.c 			\
							  hildon-sort-dialog.c 				\
							  hildon-font-selection-dialog.c 		\
							  hildon-login-dialog.c 			\
							  hildon-wizard-dialog.c 			\
							  hildon-color-chooser.c 			\
							  hildon-color-button.c 			\
							  hildon-color-chooser-dialog.c 		\
							  hildon-defines.c 				\
							  hildon-find-toolbar.c 			\
							  hildon-banner.c 				\
							  hildon-caption.c 				\
							  hildon-window.c 				\
							  hildon-program.c 				\
							  hildon-code-dialog.c 				\
							  hildon-enum-types.c 				\
							  hildon-marshalers.c

libhildon_@PACKAGE_VERSION_MAJOR@_includedir		= $(includedir)/$(PACKAGE)-$(PACKAGE_VERSION_MAJOR)/$(PACKAGE)

libhildon_@PACKAGE_VERSION_MAJOR@_include_HEADERS 	= hildon-banner.h 				\
							  hildon-calendar-popup.h 			\
							  hildon-caption.h 				\
							  hildon-code-dialog.h 				\
							  hildon-color-button.h		 		\
							  hildon-color-chooser-dialog.h 		\
							  hildon-color-chooser.h 			\
							  hildon-controlbar.h 				\
							  hildon-date-editor.h 				\
							  hildon-defines.h 				\
							  hildon-enum-types.h 				\
							  hildon-find-toolbar.h 			\
							  hildon-helper.h				\
							  hildon-font-selection-dialog.h 		\
							  hildon-get-password-dialog.h 			\
							  hildon-hvolumebar.h 				\
							  hildon-input-mode-hint.h 			\
							  hildon-login-dialog.h 			\
							  hildon-marshalers.h 				\
							  hildon-note.h 				\
							  hildon-number-editor.h 			\
							  hildon-program.h 				\
							  hildon-range-editor.h 			\
							  hildon-seekbar.h 				\
							  hildon-set-password-dialog.h 			\
							  hildon-sort-dialog.h 				\
							  hildon-sound.h 				\
							  hildon-time-editor.h 				\
							  hildon-time-picker.h 				\
							  hildon-volumebar-range.h 			\
							  hildon-volumebar.h 				\
							  hildon-vvolumebar.h 				\
							  hildon-weekday-picker.h 			\
							  hildon.h 					\
							  hildon-window.h 				\
							  hildon-wizard-dialog.h

noinst_HEADERS						= hildon-banner-private.h 			\
							  hildon-calendar-popup-private.h 		\
							  hildon-private.h				\
							  hildon-caption-private.h 			\
							  hildon-code-dialog-private.h 			\
							  hildon-color-button-private.h 		\
							  hildon-color-chooser-dialog-private.h 	\
							  hildon-color-chooser-private.h 		\
							  hildon-controlbar-private.h 			\
							  hildon-date-editor-private.h 			\
							  hildon-find-toolbar-private.h 		\
							  hildon-font-selection-dialog-private.h 	\
							  hildon-get-password-dialog-private.h 		\
							  hildon-login-dialog-private.h 		\
							  hildon-note-private.h 			\
							  hildon-number-editor-private.h 		\
							  hildon-program-private.h 			\
							  hildon-range-editor-private.h 		\
							  hildon-seekbar-private.h 			\
							  hildon-set-password-dialog-private.h 		\
							  hildon-sort-dialog-private.h 			\
							  hildon-time-editor-private.h 			\
							  hildon-time-picker-private.h 			\
							  hildon-volumebar-private.h 			\
							  hildon-weekday-picker-private.h 		\
							  hildon-window-private.h 			\
							  hildon-wizard-dialog-private.h

# Don't build the library until we have built the header that it needs:
$(OBJECTS) $(libhildon_$(PACKAGE_VERSION_MAJOR)_la_OBJECTS): hildon-enum-types.h hildon-marshalers.c hildon-marshalers.h

hildon-enum-types.h: 					  @REBUILD@ $(libhildon_$(PACKAGE_VERSION_MAJOR)_include_HEADERS) Makefile
	(cd $(srcdir) && glib-mkenums 						\
			--fhead "#ifndef __HILDON_ENUM_TYPES_H__\n" 		\
			--fhead "#define __HILDON_ENUM_TYPES_H__\n" 		\
			--fhead "\n" 						\
			--fhead "#include <glib-object.h>\n"			\
			--fhead "\n"						\
			--fhead "G_BEGIN_DECLS\n" 				\
			--fhead "\n"						\
			--fprod "/* Enumerations from \"@filename@\" */\n" 	\
			--vhead "GType @enum_name@_get_type (void);\n#define HILDON_TYPE_@ENUMSHORT@ (@enum_name@_get_type())\n" \
			--ftail "\nG_END_DECLS\n\n#endif /* __HILDON_ENUM_TYPES_H__ */" \
		$(libhildon_$(PACKAGE_VERSION_MAJOR)_include_HEADERS)) > $@

hildon-enum-types.c: 					  @REBUILD@ $(libhildon_$(PACKAGE_VERSION_MAJOR)_include_HEADERS) Makefile
	(cd $(srcdir) && glib-mkenums 						\
			--fhead "#include \"hildon.h\"\n" 			\
			--fhead "#include <glib-object.h>" 			\
		      	--fprod "\n/* enumerations from \"@filename@\" */" 	\
			--vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n    static const G@Type@Value values[] = {" 	\
			--vprod "      { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			--vtail "      { 0, NULL, NULL }\n    };\n    etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n" \
		$(libhildon_$(PACKAGE_VERSION_MAJOR)_include_HEADERS)) > $@

hildon-marshalers.h: 					hildon-marshalers.list
	glib-genmarshal --prefix _hildon_marshal --header hildon-marshalers.list >hildon-marshalers.h

hildon-marshalers.c: 					hildon-marshalers.list hildon-marshalers.h
	echo '#include "hildon-marshalers.h"' >hildon-marshalers.c
	glib-genmarshal --prefix _hildon_marshal --body hildon-marshalers.list >>hildon-marshalers.c

