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

#ifndef                                         __HILDON_SORT_DIALOG_H__
#define                                         __HILDON_SORT_DIALOG_H__

#include                                        <gtk/gtkdialog.h>

G_BEGIN_DECLS

typedef struct                                  _HildonSortDialog HildonSortDialog;

typedef struct                                  _HildonSortDialogClass HildonSortDialogClass;


#define                                         HILDON_TYPE_SORT_DIALOG \
                                                (hildon_sort_dialog_get_type())

#define                                         HILDON_SORT_DIALOG(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_SORT_DIALOG, HildonSortDialog))

#define                                         HILDON_SORT_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_SORT_DIALOG, \
                                                HildonSortDialogClass))

#define                                         HILDON_IS_SORT_DIALOG(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_SORT_DIALOG))

#define                                         HILDON_IS_SORT_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_SORT_DIALOG))

#define                                         HILDON_SORT_DIALOG_GET_CLASS(obj) \
                                                ((HildonSortDialogClass *) G_OBJECT_GET_CLASS(obj))

struct                                          _HildonSortDialog 
{
    GtkDialog parent;
};

struct _HildonSortDialogClass 
{
    GtkDialogClass parent_class;
};

GType G_GNUC_CONST
hildon_sort_dialog_get_type                     (void);

GtkWidget*
hildon_sort_dialog_new                          (GtkWindow *parent);

gint 
hildon_sort_dialog_get_sort_key                 (HildonSortDialog *dialog);

GtkSortType
hildon_sort_dialog_get_sort_order               (HildonSortDialog *dialog);

void 
hildon_sort_dialog_set_sort_key                 (HildonSortDialog *dialog,
                                                 int key);

void 
hildon_sort_dialog_set_sort_order               (HildonSortDialog *dialog,
                                                 GtkSortType order);

gint 
hildon_sort_dialog_add_sort_key                 (HildonSortDialog *dialog,
                                                 const gchar *sort_key);

gint
hildon_sort_dialog_add_sort_key_reversed        (HildonSortDialog *dialog,
                                                 const gchar *sort_key);

G_END_DECLS

#endif                                          /* __HILDON_SORT_DIALOG_H__ */
