## Process this file with automake to produce Makefile.in

# This is a blank Makefile.am for using gtk-doc.
# Copy this to your project's API docs directory and modify the variables to
# suit your project. See the GTK+ Makefiles in gtk+/docs/reference for examples
# of using the various options.

# The name of the module, e.g. 'glib'.
DOC_MODULE=hildon-libs

# The top-level SGML file. Change it if you want.
DOC_MAIN_SGML_FILE=$(DOC_MODULE)-docs.sgml

# Extra options to supply to gtkdoc-scan.
SCAN_OPTIONS=

# Extra options to pass to gtkdoc-scanobj or gtkdoc-scangobj.
SCANOBJ_OPTIONS=--type-init-func="gtk_type_init(0)"

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting functions and macros.
DOC_SOURCE_DIR=../hildon-widgets

# Used for dependencies.
HFILE_GLOB=$(top_srcdir)/hildon-widgets/*.h
CFILE_GLOB=$(top_srcdir)/hildon-widgets/*.c

# Header files to ignore when scanning.
IGNORE_HFILES = \
		hildon-app-private.h            \
		hildon-grid-item-private.h      \
		hildon-volumebar-private.h      \
		hildon-volumebar-range.h        \
		hildon-window-private.h

# CFLAGS and LDFLAGS for compiling scan program. Only needed if 
# $(DOC_MODULE).types is non-empty.
INCLUDES = $(GTK_CFLAGS) -I$(top_srcdir) $(GNOME_VFS_CFLAGS)

GTKDOC_LIBS = $(GTK_LIBS) ../hildon-widgets/libhildonwidgets.la 

# Extra options to supply to gtkdoc-mkdb.
MKDB_OPTIONS=--sgml-mode --output-format=xml

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE).
content_files= \
	version.xml

version.xml: ../configure.ac
	@echo $(VERSION) > version.xml

# Images to copy into HTML directory.
HTML_IMAGES =

# Extra options to supply to gtkdoc-fixref.
FIXXREF_OPTIONS=

include $(top_srcdir)/gtk-doc.make


