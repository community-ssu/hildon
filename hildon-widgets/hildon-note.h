/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License or any later version.
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
#ifndef __HILDON_NOTE_H__
#define __HILDON_NOTE_H__

#include <gtk/gtkdialog.h>
#include <gtk/gtkprogressbar.h>

G_BEGIN_DECLS
#define HILDON_TYPE_NOTE ( hildon_note_get_type() )
#define HILDON_NOTE(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_NOTE, HildonNote))
#define HILDON_NOTE_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_NOTE, HildonNoteClass))
#define HILDON_IS_NOTE(obj) (GTK_CHECK_TYPE (obj, HILDON_TYPE_NOTE))
#define HILDON_IS_NOTE_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_NOTE))

typedef struct _HildonNote      HildonNote;
typedef struct _HildonNoteClass HildonNoteClass;

typedef enum /*< skip >*/
{
    HILDON_NOTE_CONFIRMATION_TYPE = 0,
    HILDON_NOTE_CONFIRMATION_BUTTON_TYPE,
    HILDON_NOTE_INFORMATION_TYPE,
    HILDON_NOTE_INFORMATION_THEME_TYPE,
    HILDON_NOTE_PROGRESSBAR_TYPE
} HildonNoteType;

struct _HildonNote {
    GtkDialog parent;
};

struct _HildonNoteClass {
    GtkDialogClass parent_class;
};

GtkWidget *hildon_note_new_confirmation(GtkWindow * parent,
                                        const gchar * description);

GtkWidget *hildon_note_new_confirmation_add_buttons(GtkWindow * parent,
                                                    const gchar *
                                                    description, ...);
#ifndef HILDON_DISABLE_DEPRECATED
GtkWidget *hildon_note_new_confirmation_with_icon_stock(GtkWindow * parent,
                                                        const gchar *
                                                        description,
                                                        const gchar *
                                                        stock_id);
#endif /* HILDON_DISABLE_DEPRECATED */

GtkWidget *hildon_note_new_confirmation_with_icon_name(GtkWindow * parent,
                                                        const gchar *
                                                        description,
                                                        const gchar *
                                                        icon_name);


GtkWidget *hildon_note_new_cancel_with_progress_bar(GtkWindow * parent,
                                                    const gchar *
                                                    description,
                                                    GtkProgressBar *
                                                    progressbar);

GtkWidget *hildon_note_new_information(GtkWindow * parent,
                                       const gchar * description);
                                       
GtkWidget *hildon_note_new_information_with_icon_name(GtkWindow * parent,
                                                       const gchar *
                                                       description,
                                                       const gchar *
                                                       icon_name);
#ifndef HILDON_DISABLE_DEPRECATED
GtkWidget *hildon_note_new_information_with_icon_stock(GtkWindow * parent,
                                                       const gchar *
                                                       description,
                                                       const gchar *
                                                       stock_id);
                                                       
GtkWidget *hildon_note_new_information_with_icon_theme(GtkWindow * parent,
                                                       const gchar *
                                                       description,
                                                       const gchar *
                                                       icon);
#endif /* HILDON_DISABLE_DEPRECATED */
                                                       
void hildon_note_set_button_text(HildonNote * note, const gchar * text);

void hildon_note_set_button_texts(HildonNote * note, const gchar * textOk,
				 const gchar * textCancel);

GType hildon_note_get_type(void) G_GNUC_CONST;
GType hildon_note_type_get_type(void) G_GNUC_CONST;

G_END_DECLS
#endif /* __HILDON_NOTE_H__ */
