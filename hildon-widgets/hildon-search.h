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
#ifndef __HILDON_SEARCH_H__
#define __HILDON_SEARCH_H__

#include <gdk/gdk.h>
#include <gtk/gtkbin.h>

G_BEGIN_DECLS
#define HILDON_TYPE_SEARCH (hildon_search_get_type ())
#define HILDON_SEARCH(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
        HILDON_TYPE_SEARCH, HildonSearch))
#define HILDON_SEARCH_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_SEARCH, HildonSearchClass))
#define HILDON_IS_SEARCH(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
        HILDON_TYPE_SEARCH))
#define HILDON_IS_SEARCH_CLASS(klass)(G_TYPE_CHECK_CLASS_TYPE ((klass),\
        HILDON_TYPE_SEARCH))
#define HILDON_SEARCH_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj),\
        HILDON_TYPE_SEARCH, HildonSearchClass))
/**
 * HildonSearch:
 * 
 * Contains only private data.
 */
typedef struct _HildonSearch HildonSearch;
typedef struct _HildonSearchClass HildonSearchClass;


struct _HildonSearch {
    GtkBin bin;
};

struct _HildonSearchClass {
    GtkBinClass parent_class;
};


GType hildon_search_get_type(void);
GtkWidget *hildon_search_new(const gchar * image);
void hildon_search_set_image_stock(HildonSearch * search,
                                   const gchar * stock_icon);

G_END_DECLS
#endif /* __HILDON_SEARCH_H__ */
