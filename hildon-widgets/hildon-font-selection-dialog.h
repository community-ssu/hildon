/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
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
#ifndef __HILDON_FONT_SELECTION_DIALOG_H__
#define __HILDON_FONT_SELECTION_DIALOG_H__

#include <gtk/gtkdialog.h>
#include <gtk/gtktextbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define HILDON_TYPE_FONT_SELECTION_DIALOG \
        (hildon_font_selection_dialog_get_type ())
#define HILDON_FONT_SELECTION_DIALOG(obj) \
        (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
         HILDON_TYPE_FONT_SELECTION_DIALOG, HildonFontSelectionDialog))
#define HILDON_FONT_SELECTION_DIALOG_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST ((klass),\
         HILDON_TYPE_FONT_SELECTION_DIALOG,\
         HildonFontSelectionDialogClass))
#define HILDON_IS_FONT_SELECTION_DIALOG(obj) \
        (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
         HILDON_TYPE_FONT_SELECTION_DIALOG))
#define HILDON_IS_FONT_SELECTION_DIALOG_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE ((klass),\
         HILDON_TYPE_FONT_SELECTION_DIALOG))
#define HILDON_FONT_SELECTION_DIALOG_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj),\
         HILDON_TYPE_FONT_SELECTION_DIALOG,\
         HildonFontSelectionDialogClass))

typedef struct _HildonFontSelectionDialog HildonFontSelectionDialog;
typedef struct _HildonFontSelectionDialogClass
 HildonFontSelectionDialogClass;

typedef enum {
    HILDON_POSITIONING_NORMAL,
    HILDON_POSITIONING_SUPER,
    HILDON_POSITIONING_SUB
} HildonPositioning;

typedef enum {
    HILDON_FONT_FACE_NORMAL,
    HILDON_FONT_FACE_ITALIC,
    HILDON_FONT_FACE_BOLD,
    HILDON_FONT_FACE_BOLD_ITALIC
} HildonFontFaceType;


struct _HildonFontSelectionDialog {
    GtkDialog parent;
};

struct _HildonFontSelectionDialogClass {
    GtkDialogClass parent_class;

    /* Padding for future expansion */
    void (*_gtk_reserved1) (void);
    void (*_gtk_reserved2) (void);
    void (*_gtk_reserved3) (void);
    void (*_gtk_reserved4) (void);
};


GType hildon_font_selection_dialog_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_font_selection_dialog_new(GtkWindow * parent,
                                            const gchar * title);

/*To set and to get a font, use these functions*/
PangoAttrList
    * hildon_font_selection_dialog_get_font(HildonFontSelectionDialog *
                                            fsd);
void hildon_font_selection_dialog_set_font(HildonFontSelectionDialog *
                                           fsd, PangoAttrList * list);

void hildon_font_selection_dialog_set_buffer(HildonFontSelectionDialog *fsd, GtkTextBuffer *buffer);
GtkTextTag * hildon_font_selection_dialog_get_text_tag (HildonFontSelectionDialog *fsd);



/* This returns the text in the lbl_preview. You should copy the returned
   text if you need it. */
gchar*
hildon_font_selection_dialog_get_preview_text(HildonFontSelectionDialog * fsd);

/* This sets the text in the lbl_preview.*/
void hildon_font_selection_dialog_set_preview_text
    (HildonFontSelectionDialog * fsd, const gchar * text);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __HILDON_FONTSEL_H__ */
