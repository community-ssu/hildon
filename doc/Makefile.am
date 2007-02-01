EXTRA_DIST					= gtk-doc.make

MAINTAINERCLEANFILES 				= Makefile.in				\
						  hildon-sections.txt			\
						  hildon-undocumented.txt		\
						  version.xml

DOC_MODULE					= hildon

DOC_MAIN_SGML_FILE				= $(DOC_MODULE)-docs.sgml

SCAN_OPTIONS					=

SCANOBJ_OPTIONS					=--type-init-func="gtk_type_init(0)"

DOC_SOURCE_DIR					=../src

HFILE_GLOB					=$(top_srcdir)/src/*.h

CFILE_GLOB					=$(top_srcdir)/src/*.c

IGNORE_HFILES 					= hildon-banner-private.h 		\
						  hildon-calendar-popup-private.h	\
						  hildon-caption-private.h 		\
						  hildon-code-dialog-private.h 		\
						  hildon-color-button-private.h 	\
						  hildon-color-chooser-dialog-private.h \
						  hildon-color-chooser-private.h 	\
						  hildon-controlbar-private.h 		\
						  hildon-date-editor-private.h 		\
						  hildon-find-toolbar-private.h 	\
						  hildon-font-selection-dialog-private.h\
						  hildon-get-password-dialog-private.h 	\
						  hildon-login-dialog-private.h 	\
						  hildon-note-private.h 		\
						  hildon-number-editor-private.h 	\
						  hildon-private.c 			\
						  hildon-private.h 			\
						  hildon-program-private.h 		\
						  hildon-range-editor-private.h 	\
						  hildon-seekbar-private.h 		\
						  hildon-set-password-dialog-private.h 	\
						  hildon-sort-dialog-private.h 		\
						  hildon-time-editor-private.h 		\
						  hildon-time-picker-private.h 		\
						  hildon-volumebar-private.h 		\
						  hildon-weekday-picker-private.h 	\
						  hildon-window-private.h 		\
						  hildon-wizard-dialog-private.h

INCLUDES 					= $(GTK_CFLAGS) -I$(top_srcdir)

GTKDOC_LIBS 					= $(GTK_LIBS) ../src/libhildon_@PACKAGE_VERSION_MAJOR@.la 

MKDB_OPTIONS					=--sgml-mode

content_files					= version.xml

version.xml: ../configure.ac
	@echo $(VERSION) > version.xml

html: html-build.stamp

HTML_IMAGES 					=

FIXXREF_OPTIONS					=

include $(top_srcdir)/doc/gtk-doc.make

