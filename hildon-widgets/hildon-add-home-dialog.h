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

#ifndef __HILDON_ADD_HOME_DIALOG_H__
#define __HILDON_ADD_HOME_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_ADD_HOME_DIALOG \
  ( hildon_add_home_dialog_get_type() )
#define HILDON_ADD_HOME_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_ADD_HOME_DIALOG, HildonAddHomeDialog))
#define HILDON_ADD_HOME_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_ADD_HOME_DIALOG, \
  HildonAddHomeDialogClass))
#define HILDON_IS_ADD_HOME_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_ADD_HOME_DIALOG))
#define HILDON_IS_ADD_HOME_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_ADD_HOME_DIALOG))
typedef struct _HildonAddHomeDialog HildonAddHomeDialog;
typedef struct _HildonAddHomeDialogClass HildonAddHomeDialogClass;

struct _HildonAddHomeDialog {
    GtkDialog parent;
};

struct _HildonAddHomeDialogClass {
    GtkDialogClass parent_class;
};

GType hildon_add_home_dialog_get_type(void);

GtkWidget *hildon_add_home_dialog_new(GtkWindow * parent,
                                      const gchar * name,
                                      const gchar * new_name);
const gchar *hildon_add_home_dialog_get_name(HildonAddHomeDialog * dialog);

G_END_DECLS
#endif /* __HILDON_ADD_HOME_DIALOG_H__ */
