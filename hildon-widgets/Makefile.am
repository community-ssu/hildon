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
	hildon-defines.c \
	hildon-app.h \
	hildon-defines.h \
	hildon-appview.c \
	hildon-find-toolbar.c \
	gtk-infoprint.c \
	hildon-appview.h \
	hildon-find-toolbar.h \
	gtk-infoprint.h \
	hildon-caption.c \
	hildon-input-mode-hint.h \
	hildon-app-private.h \
	hildon-caption.h


hildon-marshalers.h: hildon-marshalers.list
	glib-genmarshal --prefix _hildon_marshal --header \
			hildon-marshalers.list >hildon-marshalers.h

hildon-marshalers.c: hildon-marshalers.list hildon-marshalers.h
	echo '#include "hildon-marshalers.h"' >hildon-marshalers.c
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
	hildon-input-mode-hint.h \
	hildon-app-private.h \
	hildon-caption.h
