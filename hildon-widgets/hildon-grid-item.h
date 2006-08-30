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

/*
 * @file hildon-grid-item.h
 *
 * This file is a header file for he implementation of HildonGridItem.
 * HildonGridItem is an item mainly used in HildonGrid. It has an icon,
 * emblem and a label.
 */

#ifndef __HILDON_GRID_ITEM_H__
#define __HILDON_GRID_ITEM_H__

#include <gtk/gtkcontainer.h>
#include <gtk/gtkitem.h>

G_BEGIN_DECLS
#define HILDON_TYPE_GRID_ITEM       (hildon_grid_item_get_type ())
#define HILDON_GRID_ITEM(obj)       (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                            HILDON_TYPE_GRID_ITEM, \
                                            HildonGridItem))
#define HILDON_GRID_ITEM_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_GRID_ITEM, \
                                                HildonGridItemClass))
#define HILDON_IS_GRID_ITEM(obj)    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                            HILDON_TYPE_GRID_ITEM))
#define HILDON_IS_GRID_ITEM_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_GRID_ITEM))
#define HILDON_GRID_ITEM_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
         HILDON_TYPE_GRID_ITEM, HildonGridItemClass))

typedef enum {
    HILDON_GRID_ITEM_LABEL_POS_BOTTOM = 1,
    HILDON_GRID_ITEM_LABEL_POS_RIGHT
} HildonGridPositionType;

typedef enum {
    HILDON_GRID_ITEM_ICON_27x27 = 1,
    HILDON_GRID_ITEM_ICON_128x128
} HildonGridItemIconSizeType;


typedef struct _HildonGridItem HildonGridItem;
typedef struct _HildonGridItemClass HildonGridItemClass;


struct _HildonGridItem {
    GtkContainer parent;
};

struct _HildonGridItemClass {
    GtkContainerClass parent_class;
};



GType hildon_grid_item_get_type(void);
GtkWidget *hildon_grid_item_new(const gchar * icon_basename);
GtkWidget *hildon_grid_item_new_with_label(const gchar * icon_basename,
                                           const gchar * label);

void hildon_grid_item_set_emblem_type(HildonGridItem * item,
                                      const gchar * emblem_basename);
const gchar *hildon_grid_item_get_emblem_type(HildonGridItem * item);
void hildon_grid_item_set_label(HildonGridItem *item, const gchar *label);


G_END_DECLS
#endif /* __HILDON_GRID_ITEM_H__ */
