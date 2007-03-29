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

#ifndef                                         __HILDON_GET_PASSWORD_DIALOG_H__
#define                                         __HILDON_GET_PASSWORD_DIALOG_H__

#include                                        <gtk/gtkdialog.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_GET_PASSWORD_DIALOG \
                                                (hildon_get_password_dialog_get_type())

#define                                         HILDON_GET_PASSWORD_DIALOG(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_GET_PASSWORD_DIALOG,\
                                                HildonGetPasswordDialog))

#define                                         HILDON_GET_PASSWORD_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_GET_PASSWORD_DIALOG, \
                                                HildonGetPasswordDialogClass))

#define                                         HILDON_IS_GET_PASSWORD_DIALOG(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_GET_PASSWORD_DIALOG))

#define                                         HILDON_IS_GET_PASSWORD_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_GET_PASSWORD_DIALOG))

typedef struct                                  _HildonGetPasswordDialog HildonGetPasswordDialog;

typedef struct                                  _HildonGetPasswordDialogClass HildonGetPasswordDialogClass;

struct                                          _HildonGetPasswordDialog 
{
    GtkDialog parent;
};

struct                                          _HildonGetPasswordDialogClass 
{
    GtkDialogClass parent_class;
};

GType G_GNUC_CONST
hildon_get_password_dialog_get_type             (void);

GtkWidget*
hildon_get_password_dialog_new                  (GtkWindow *parent,
                                                 gboolean get_old);

GtkWidget*
hildon_get_password_dialog_new_with_default     (GtkWindow *parent,
                                                 const gchar *password,
                                                 gboolean get_old);

void       
hildon_get_password_dialog_set_message          (HildonGetPasswordDialog *dialog, 
                                                 const gchar *message);

void       
hildon_get_password_dialog_set_caption          (HildonGetPasswordDialog *dialog,
                                                 const gchar *new_caption);

void       
hildon_get_password_dialog_set_max_characters   (HildonGetPasswordDialog *dialog,
                                                 gint max_characters);

const gchar* 
hildon_get_password_dialog_get_password         (HildonGetPasswordDialog *dialog);

G_END_DECLS

#endif                                          /* __HILDON_GET_PASSWORD_DIALOG_H__ */
