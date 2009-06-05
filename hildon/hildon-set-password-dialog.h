/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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

#ifndef                                         __HILDON_SET_PASSWORD_DIALOG_H__
#define                                         __HILDON_SET_PASSWORD_DIALOG_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_SET_PASSWORD_DIALOG \
                                                (hildon_set_password_dialog_get_type())

#define                                         HILDON_SET_PASSWORD_DIALOG(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST (obj, \
                                                HILDON_TYPE_SET_PASSWORD_DIALOG,\
                                                HildonSetPasswordDialog))

#define                                         HILDON_SET_PASSWORD_DIALOG_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), HILDON_TYPE_SET_PASSWORD_DIALOG, \
                                                HildonSetPasswordDialogClass))

#define                                         HILDON_IS_SET_PASSWORD_DIALOG(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE (obj, HILDON_TYPE_SET_PASSWORD_DIALOG))

#define                                         HILDON_IS_SET_PASSWORD_DIALOG_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_SET_PASSWORD_DIALOG))

typedef struct                                  _HildonSetPasswordDialog HildonSetPasswordDialog;

typedef struct                                  _HildonSetPasswordDialogClass HildonSetPasswordDialogClass;

struct                                          _HildonSetPasswordDialog 
{
    GtkDialog parent;
};

struct                                          _HildonSetPasswordDialogClass 
{
    GtkDialogClass parent_class;
};

GtkWidget*
hildon_set_password_dialog_new                  (GtkWindow *parent,
                                                 gboolean modify_protection);

GtkWidget*
hildon_set_password_dialog_new_with_default     (GtkWindow *parent,
                                                 const gchar *password,
                                                 gboolean modify_protection);

GType G_GNUC_CONST
hildon_set_password_dialog_get_type             (void);

const gchar*
hildon_set_password_dialog_get_password         (HildonSetPasswordDialog *dialog);

gboolean     
hildon_set_password_dialog_get_protected        (HildonSetPasswordDialog *dialog);

void       
hildon_set_password_dialog_set_message          (HildonSetPasswordDialog *dialog,
                                                 const gchar *message);

G_END_DECLS

#endif                                          /* __HILDON_SET_PASSWORD_DIALOG_H__ */

#endif                                          /* HILDON_DISABLE_DEPRECATED */
