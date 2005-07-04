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

#ifndef __HILDON_FIND_OBJECT_DIALOG_H__
#define __HILDON_FIND_OBJECT_DIALOG_H__

#ifndef __DEPRACATED__
#define __DEPRACATED__
#endif

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_FIND_OBJECT_DIALOG \
  ( hildon_find_object_dialog_get_type() )
#define HILDON_FIND_OBJECT_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_FIND_OBJECT_DIALOG,\
   HildonFindObjectDialog))
#define HILDON_FIND_OBJECT_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_FIND_OBJECT_DIALOG, \
  HildonFindObjectDialogClass))
#define HILDON_IS_FIND_OBJECT_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_FIND_OBJECT_DIALOG))
#define HILDON_IS_FIND_OBJECT_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_FIND_OBJECT_DIALOG))
typedef struct _HildonFindObjectDialog HildonFindObjectDialog;
typedef struct _HildonFindObjectDialogClass HildonFindObjectDialogClass;

struct _HildonFindObjectDialog {
    GtkDialog parent;
};

struct _HildonFindObjectDialogClass {
    GtkDialogClass parent_class;
};

GType hildon_find_object_dialog_get_type(void);

GtkWidget *hildon_find_object_dialog_new(GtkWindow * parent);
GtkWidget *hildon_find_object_dialog_new_with_options(GtkWindow * parent,
                                                      GtkDialog * dialog);
const gchar *hildon_find_object_dialog_get_text(HildonFindObjectDialog *
                                                dialog);
void hildon_find_object_dialog_set_label(HildonFindObjectDialog * dialog,
                                         const gchar * label);

G_END_DECLS
#endif
