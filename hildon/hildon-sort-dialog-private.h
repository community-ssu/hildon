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

#ifndef                                         __HILDON_SORT_DIALOG_PRIVATE_H__
#define                                         __HILDON_SORT_DIALOG_PRIVATEH__

G_BEGIN_DECLS

#define                                         HILDON_SORT_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE \
                                                ((obj), HILDON_TYPE_SORT_DIALOG, HildonSortDialogPrivate));

typedef struct                                  _HildonSortDialogPrivate HildonSortDialogPrivate;

struct                                          _HildonSortDialogPrivate 
{
    /* Sort category widgets */
    GtkWidget *combo_key;
    GtkWidget *caption_key;

    /* Sort order widgets */
    GtkWidget *combo_order;
    GtkWidget *caption_order;

    /* Index value counter */
    gint index_counter;

    /* If the current order displayed is reversed */
    gboolean reversed;

    /* An array for each key representing if a key should be reverse-sorted */
    gboolean *key_reversed;
};

G_END_DECLS

#endif                                          /* __HILDON_SORT_DIALOG_PRIVATE_H__ */
