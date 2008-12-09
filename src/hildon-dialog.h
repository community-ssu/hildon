/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

#ifndef                                         HILDON_DISABLE_DEPRECATED

#ifndef                                         __HILDON_DIALOG_H__
#define                                         __HILDON_DIALOG_H__

#include                                        <gtk/gtk.h>

#include                                        "hildon-defines.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_DIALOG \
                                                (hildon_dialog_get_type ())

#define                                         HILDON_DIALOG(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_DIALOG, HildonDialog))

#define                                         HILDON_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_DIALOG, HildonDialogClass))

#define                                         HILDON_IS_DIALOG(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_DIALOG))

#define                                         HILDON_IS_DIALOG_CLASS(klass) \
                                                (GTK_CHECH_CLASS_TYPE ((klass), HILDON_TYPE_DIALOG))

#define                                         HILDON_DIALOG_GET_CLASS(obj) \
                                                ((HildonDialogClass *) G_OBJECT_GET_CLASS (obj))

typedef struct                                  _HildonDialog HildonDialog;

typedef struct                                  _HildonDialogClass HildonDialogClass;

struct                                          _HildonDialog
{
    GtkDialog parent;
};

struct                                          _HildonDialogClass
{
    GtkDialogClass parent_class;

    /* Padding for future extension */
    void (*_hildon_reserved1)(void);
    void (*_hildon_reserved2)(void);
    void (*_hildon_reserved3)(void);
};

GType G_GNUC_CONST
hildon_dialog_get_type                          (void);

GtkWidget*
hildon_dialog_new                               (void);

GtkWidget*
hildon_dialog_new_with_buttons                  (const gchar    *title,
                                                 GtkWindow      *parent,
                                                 GtkDialogFlags  flags,
                                                 const gchar    *first_button_text,
                                                 ...);

GtkWidget *
hildon_dialog_add_button                        (HildonDialog *dialog,
                                                 const gchar  *button_text,
                                                 gint          response_id);

void
hildon_dialog_add_buttons                       (HildonDialog *dialog,
                                                 const gchar  *first_button_text,
                                                 ...);

G_END_DECLS

#endif                                          /* __HILDON_DIALOG_H__ */

#endif                                          /* HILDON_DISABLE_DEPRECATED */
