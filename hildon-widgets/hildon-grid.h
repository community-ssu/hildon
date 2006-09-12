/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License or any later version.
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
 * @file hildon-grid.h
 *
 * This file is a header file for hildon-grid.c, the implementation of
 * #HildonGrid. #HildonGrid is used in views like Home and Control Panel
 * which have single-tap activated items.
 */

#ifndef __HILDON_GRID_H__
#define __HILDON_GRID_H__

#include <gtk/gtkcontainer.h>
#include <hildon-widgets/hildon-grid-item.h>

G_BEGIN_DECLS
#define HILDON_TYPE_GRID            (hildon_grid_get_type ())
#define HILDON_GRID(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                            HILDON_TYPE_GRID, \
                                            HildonGrid))
#define HILDON_GRID_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                            HILDON_TYPE_GRID, \
                                            HildonGridClass))
#define HILDON_IS_GRID(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                            HILDON_TYPE_GRID))
#define HILDON_IS_GRID_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                            HILDON_TYPE_GRID))
#define HILDON_GRID_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                            HILDON_TYPE_GRID, \
                                            HildonGridClass))
typedef struct _HildonGrid HildonGrid;
typedef struct _HildonGridClass HildonGridClass;



struct _HildonGrid {
    GtkContainer parent;
};

struct _HildonGridClass {
    GtkContainerClass parent_class;

    void (*activate_child) (HildonGrid * grid, HildonGridItem * item);
    void (*popup_context_menu) (HildonGrid * grid, HildonGridItem * item);
};

GType hildon_grid_get_type(void);
GtkWidget *hildon_grid_new(void);

/*
 * Use GtkContainer API:
 *
 * void        gtk_container_set_focus_child   (GtkContainer    *container,
 *                                              GtkWidget       *child);
 *
 * GTK_CONTAINER (grid)->focus_child can be used to get focused child.
 */

void hildon_grid_set_style(HildonGrid * grid, const gchar * style_name);
const gchar *hildon_grid_get_style(HildonGrid * grid);

void hildon_grid_set_scrollbar_pos(HildonGrid * grid, gint scrollbar_pos);
gint hildon_grid_get_scrollbar_pos(HildonGrid * grid);


/*
 * We are going to use gtk_container_add/remove, so these are internal.
 * If GridView is not visible, it won't update the view, so it should be
 * hidden when doing massive modifications.
 *
 * 
 * Use GtkContainer API:
 *
 * void        gtk_container_add               (GtkContainer    *container,
 *                                              GtkWidget       *widget);
 *
 * void        gtk_container_remove            (GtkContainer    *container,
 *                                              GtkWidget       *widget);
 */

void hildon_grid_activate_child(HildonGrid * grid, HildonGridItem * item);

G_END_DECLS
#endif /* __HILDON_GRID_H__ */
