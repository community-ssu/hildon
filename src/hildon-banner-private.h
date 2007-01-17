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

#ifndef                                         __HILDON_BANNER_PRIVATE_H__
#define                                         __HILDON_BANNER_PRIVATE_H__

G_BEGIN_DECLS 

typedef struct                                  _HildonBannerPrivate HildonBannerPrivate;

#define                                         HILDON_BANNER_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_BANNER, HildonBannerPrivate));

struct                                          _HildonBannerPrivate
{
    GtkWidget *main_item;
    GtkWidget *label;
    GtkWidget *layout;
    guint timeout_id;
    gboolean is_timed; 
};

/* For internal use of hildon libraries only */
void G_GNUC_INTERNAL
hildon_gtk_label_set_text_n_lines               (GtkLabel *label, 
                                                 const gchar *text, 
                                                 gint max_lines);

G_END_DECLS

#endif                                          /* __HILDON_BANNER_PRIVATE_H__ */
