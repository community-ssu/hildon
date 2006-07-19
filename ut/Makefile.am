outomodule_LTLIBRARIES = \
			libhildonwidgets_unittests.la \
			libhildonwidgets_clock_unittests.la

INCLUDES = $(GTK_CFLAGS) -I$(top_srcdir)/hildon-widgets $(OUTO_CFLAGS)
LDFLAGS = -module -avoid-version

libhildonwidgets_unittests_la_SOURCES = hildon-widgets_tests.c
libhildonwidgets_unittests_la_LIBADD = $(GTK_LIBS) $(top_builddir)/hildon-widgets/libhildonwidgets.la

libhildonwidgets_clock_unittests_la_SOURCES = hildon-clock-widgets_tests.c
libhildonwidgets_clock_unittests_la_LIBADD = $(GTK_LIBS) $(top_builddir)/hildon-widgets/libhildonwidgets.la
