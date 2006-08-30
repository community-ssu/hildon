/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
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

#ifndef __HILDON_NAME_PASSWORD_DIALOG_H__
#define __HILDON_NAME_PASSWORD_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_NAME_PASSWORD_DIALOG \
  ( hildon_name_password_dialog_get_type() )
#define HILDON_NAME_PASSWORD_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_NAME_PASSWORD_DIALOG,\
   HildonNamePasswordDialog))
#define HILDON_NAME_PASSWORD_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_NAME_PASSWORD_DIALOG, \
  HildonNamePasswordDialogClass))
#define HILDON_IS_NAME_PASSWORD_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_NAME_PASSWORD_DIALOG))
#define HILDON_IS_NAME_PASSWORD_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_NAME_PASSWORD_DIALOG))

#define HILDON_NAME_PASSWORD_DIALOG_TITLE "frw_ti_get_user_name_and_pwd"
#define HILDON_NAME_PASSWORD_DIALOG_NAME \
         "frw_ti_get_user_name_and_pwd_enter_user_name"
#define HILDON_NAME_PASSWORD_DIALOG_PASSWORD \
         "frw_ti_get_user_name_and_pwd_enter_pwd"
#define HILDON_NAME_PASSWORD_DIALOG_OK "frw_bd_get_user_name_and_pwd_ok"
#define HILDON_NAME_PASSWORD_DIALOG_CANCEL \
         "frw_bd_get_user_name_and_pwd_cancel"

  
typedef struct _HildonNamePasswordDialog HildonNamePasswordDialog;
typedef struct _HildonNamePasswordDialogClass
    HildonNamePasswordDialogClass;

struct _HildonNamePasswordDialog {
    GtkDialog parent;
};

struct _HildonNamePasswordDialogClass {
    GtkDialogClass parent_class;
};

GType hildon_name_password_dialog_get_type(void) G_GNUC_CONST;

GtkWidget *hildon_name_password_dialog_new(GtkWindow * parent);

GtkWidget *hildon_name_password_dialog_new_with_default(GtkWindow *parent,
							const gchar *name,
							const gchar *pass);


const gchar *hildon_name_password_dialog_get_name(HildonNamePasswordDialog 
                                                  * dialog);
                                                  
const gchar *hildon_name_password_dialog_get_password(HildonNamePasswordDialog
                                                      * dialog);

void hildon_name_password_dialog_set_domain(HildonNamePasswordDialog *dialog, 
					    const gchar *domain);

G_END_DECLS
#endif /* __HILDON_NAME_PASSWORD_DIALOG_H__ */
