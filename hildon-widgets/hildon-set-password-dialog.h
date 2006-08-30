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

#ifndef __HILDON_SET_PASSWORD_DIALOG_H__
#define __HILDON_SET_PASSWORD_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS


#define HILDON_TYPE_SET_PASSWORD_DIALOG \
  ( hildon_set_password_dialog_get_type() )
#define HILDON_SET_PASSWORD_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_SET_PASSWORD_DIALOG,\
   HildonSetPasswordDialog))
#define HILDON_SET_PASSWORD_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_SET_PASSWORD_DIALOG, \
   HildonSetPasswordDialogClass))
#define HILDON_IS_SET_PASSWORD_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_SET_PASSWORD_DIALOG))
#define HILDON_IS_SET_PASSWORD_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_SET_PASSWORD_DIALOG))



#define HILDON_SET_PASSWORD_DIALOG_TITLE                  "ecdg_ti_set_password"
#define HILDON_SET_PASSWORD_DIALOG_PASSWORD               "ecdg_fi_set_passwd_enter_pwd"
#define HILDON_SET_PASSWORD_DIALOG_VERIFY_PASSWORD        "ecdg_fi_set_passwd_confirm"
#define HILDON_SET_PASSWORD_DIALOG_OK                     "ecdg_bd_set_password_dialog_ok"
#define HILDON_SET_PASSWORD_DIALOG_CANCEL                 "ecdg_bd_set_password_dialog_cancel"

#define HILDON_SET_MODIFY_PASSWORD_DIALOG_TITLE           "ckdg_ti_dialog_c_passwd_change_password"
#define HILDON_SET_MODIFY_PASSWORD_DIALOG_LABEL           "ckdg_fi_dialog_c_passwd_pwd_protect"
#define HILDON_SET_MODIFY_PASSWORD_DIALOG_PASSWORD        "ckdg_fi_dialog_c_passwd_new_pwd"
#define HILDON_SET_MODIFY_PASSWORD_DIALOG_VERIFY_PASSWORD "ckdg_fi_dialog_c_passwd_ver_pwd"
#define HILDON_SET_MODIFY_PASSWORD_DIALOG_OK              "ckdg_bd_change_password_dialog_ok"
#define HILDON_SET_MODIFY_PASSWORD_DIALOG_CANCEL          "ckdg_bd_change_password_dialog_cancel"

#define HILDON_SET_PASSWORD_DIALOG_MISMATCH               "ecdg_ib_passwords_do_not_match"
#define HILDON_SET_PASSWORD_DIALOG_EMPTY                  "ecdg_ib_password_is_empty"
#define HILDON_SET_PASSWORD_DIALOG_REMOVE_PROTECTION      "ckdg_nc_dialog_c_passwd_remove_pwd"

#define HILDON_REMOVE_PROTECTION_CONFIRMATION_REMOVE      "ckdg_bd_dialog_c_passwd_remove_button"
#define HILDON_REMOVE_PROTECTION_CONFIRMATION_CANCEL      "ckdg_bd_dialog_c_passwd_cancel_button"



typedef struct _HildonSetPasswordDialog      HildonSetPasswordDialog;
typedef struct _HildonSetPasswordDialogClass HildonSetPasswordDialogClass;

struct _HildonSetPasswordDialog {
    GtkDialog parent;
};

struct _HildonSetPasswordDialogClass {
    GtkDialogClass parent_class;
};



GtkWidget *  hildon_set_password_dialog_new              (GtkWindow   *parent,
                                                          gboolean     modify_protection);

GtkWidget *  hildon_set_password_dialog_new_with_default (GtkWindow   *parent,
                                                          const gchar *password,
                                                          gboolean     modify_protection);

GType        hildon_set_password_dialog_get_type         (void) G_GNUC_CONST;

const gchar *hildon_set_password_dialog_get_password     (HildonSetPasswordDialog *dialog);

gboolean     hildon_set_password_dialog_get_protected    (HildonSetPasswordDialog *dialog);

void         hildon_set_password_dialog_set_domain       (HildonSetPasswordDialog *dialog,
                                                          const gchar             *domain);


G_END_DECLS

#endif /* __HILDON_SET_PASSWORD_DIALOG_H__ */
