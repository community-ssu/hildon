MAINTAINERCLEANFILES 				= Makefile.in

DOC_MODULE					= hildon

DOC_MAIN_SGML_FILE				= $(DOC_MODULE)-docs.sgml

SCAN_OPTIONS					=

SCANOBJ_OPTIONS					=--type-init-func="gtk_type_init(0)"

DOC_SOURCE_DIR					=../src

HFILE_GLOB					=$(top_srcdir)/src/*.h

CFILE_GLOB					=$(top_srcdir)/src/*.c

IGNORE_HFILES 					= 

INCLUDES 					= $(GTK_CFLAGS) -I$(top_srcdir)

GTKDOC_LIBS 					= $(GTK_LIBS) ../src/libhildon_@PACKAGE_VERSION_MAJOR@.la 

MKDB_OPTIONS					=--sgml-mode --output-format=xml

content_files					= version.xml

version.xml: ../configure.ac
	@echo $(VERSION) > version.xml

HTML_IMAGES 					=

FIXXREF_OPTIONS					=

include $(top_srcdir)/gtk-doc.make


