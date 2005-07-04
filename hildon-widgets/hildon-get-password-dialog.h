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

#ifndef __HILDON_GET_PASSWORD_DIALOG_H__
#define __HILDON_GET_PASSWORD_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_GET_PASSWORD_DIALOG \
  ( hildon_get_password_dialog_get_type() )
#define HILDON_GET_PASSWORD_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_GET_PASSWORD_DIALOG,\
   HildonGetPasswordDialog))
#define HILDON_GET_PASSWORD_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_GET_PASSWORD_DIALOG, \
  HildonGetPasswordDialogClass))
#define HILDON_IS_GET_PASSWORD_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_GET_PASSWORD_DIALOG))
#define HILDON_IS_GET_PASSWORD_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_GET_PASSWORD_DIALOG))
typedef struct _HildonGetPasswordDialog HildonGetPasswordDialog;
typedef struct _HildonGetPasswordDialogClass HildonGetPasswordDialogClass;

struct _HildonGetPasswordDialog {
    GtkDialog parent;
};

struct _HildonGetPasswordDialogClass {
    GtkDialogClass parent_class;
};

GType hildon_get_password_dialog_get_type(void);

GtkWidget *hildon_get_password_dialog_new(GtkWindow * parent,
                                          gboolean get_old_password_title);

GtkWidget *hildon_get_password_dialog_new_with_default(GtkWindow *parent,
					               gchar *password,
					  gboolean get_old_password_title);

const gchar
    *hildon_get_password_dialog_get_password(HildonGetPasswordDialog *
                                             dialog);

G_END_DECLS
#endif
