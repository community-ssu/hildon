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

#ifndef                                         __HILDON_NAME_PASSWORD_DIALOG_H__
#define                                         __HILDON_NAME_PASSWORD_DIALOG_H__

#include                                        <gtk/gtkdialog.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_LOGIN_DIALOG \
                                                (hildon_login_dialog_get_type ())

#define                                         HILDON_LOGIN_DIALOG(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_LOGIN_DIALOG, \
                                                HildonLoginDialog))

#define                                         HILDON_LOGIN_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_LOGIN_DIALOG, \
                                                HildonLoginDialogClass))

#define                                         HILDON_IS_LOGIN_DIALOG(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_LOGIN_DIALOG))

#define                                         HILDON_IS_LOGIN_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), \
                                                HILDON_TYPE_LOGIN_DIALOG))
 
typedef struct                                  _HildonLoginDialog HildonLoginDialog;

typedef struct                                  _HildonLoginDialogClass HildonLoginDialogClass;

struct                                          _HildonLoginDialog 
{
    GtkDialog parent;
};

struct                                          _HildonLoginDialogClass 
{
    GtkDialogClass parent_class;
};

GType G_GNUC_CONST
hildon_login_dialog_get_type                    (void);

GtkWidget*
hildon_login_dialog_new                         (GtkWindow *parent);

GtkWidget*
hildon_login_dialog_new_with_default            (GtkWindow *parent,
                                                 const gchar *username,
                                                 const gchar *pass);

const gchar*
hildon_login_dialog_get_username                (HildonLoginDialog *dialog);
                                                  
const gchar*
hildon_login_dialog_get_password                (HildonLoginDialog *dialog);

void 
hildon_login_dialog_set_message                 (HildonLoginDialog *dialog, 
                                                 const gchar *msg);

G_END_DECLS

#endif                                          /* __HILDON_LOGIN_DIALOG_H__ */
