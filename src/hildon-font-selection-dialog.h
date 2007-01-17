/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#ifndef                                         __HILDON_FONT_SELECTION_DIALOG_H__
#define                                         __HILDON_FONT_SELECTION_DIALOG_H__

#include                                        <gtk/gtkdialog.h>
#include                                        <gtk/gtktextbuffer.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_FONT_SELECTION_DIALOG \
                                                (hildon_font_selection_dialog_get_type ())

#define                                         HILDON_FONT_SELECTION_DIALOG(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
                                                HILDON_TYPE_FONT_SELECTION_DIALOG, HildonFontSelectionDialog))

#define                                         HILDON_FONT_SELECTION_DIALOG_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_FONT_SELECTION_DIALOG,\
                                                HildonFontSelectionDialogClass))

#define                                         HILDON_IS_FONT_SELECTION_DIALOG(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
                                                HILDON_TYPE_FONT_SELECTION_DIALOG))

#define                                         HILDON_IS_FONT_SELECTION_DIALOG_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass),\
                                                HILDON_TYPE_FONT_SELECTION_DIALOG))

#define                                         HILDON_FONT_SELECTION_DIALOG_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj),\
                                                HILDON_TYPE_FONT_SELECTION_DIALOG,\
                                                HildonFontSelectionDialogClass))

typedef struct                                  _HildonFontSelectionDialog HildonFontSelectionDialog;

typedef struct                                  _HildonFontSelectionDialogClass HildonFontSelectionDialogClass;

struct                                          _HildonFontSelectionDialog
{
    GtkDialog parent;
};

struct                                          _HildonFontSelectionDialogClass
{
    GtkDialogClass parent_class;
};

GType G_GNUC_CONST
hildon_font_selection_dialog_get_type           (void);

GtkWidget* 
hildon_font_selection_dialog_new                (GtkWindow *parent,
                                                 const gchar *title);

gchar* 
hildon_font_selection_dialog_get_preview_text   (HildonFontSelectionDialog *fsd);

void
hildon_font_selection_dialog_set_preview_text   (HildonFontSelectionDialog *fsd, 
                                                 const gchar * text);

G_END_DECLS

#endif                                          /* __HILDON_FONT_SELECTION_DIALOG_H__ */
