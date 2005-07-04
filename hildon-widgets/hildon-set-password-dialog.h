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
/**
 * HildonSetPasswordDialog:
 * 
 * Contains only private data.
 */
typedef struct _HildonSetPasswordDialog HildonSetPasswordDialog;
typedef struct _HildonSetPasswordDialogClass HildonSetPasswordDialogClass;

struct _HildonSetPasswordDialog {
    GtkDialog parent;
};

struct _HildonSetPasswordDialogClass {
    GtkDialogClass parent_class;
};

/*
 * Public methods
 */

GtkWidget *hildon_set_password_dialog_new(GtkWindow * parent,
                                          gboolean modify_protection);

GtkWidget *hildon_set_password_dialog_new_with_default
                                         (GtkWindow *parent,
					  gchar *password,
                                          gboolean modify_protection);

GType hildon_set_password_dialog_get_type(void);

const gchar
    *hildon_set_password_dialog_get_password(HildonSetPasswordDialog *
                                             dialog);

gboolean hildon_set_password_dialog_get_protected(HildonSetPasswordDialog *
                                                  dialog);

G_END_DECLS
#endif /* HILDON_SET_PASSWORD_DIALOG_H */
