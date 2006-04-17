/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2006 Nokia Corporation.
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


#ifndef HILDON_WINDOW_H
#define HILDON_WINDOW_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtktoolbar.h>
#include <gdk/gdkx.h>

#include "hildon-defines.h"

G_BEGIN_DECLS

#define HILDON_WINDOW_LONG_PRESS_TIME 1500 /* in ms */

#define HILDON_TYPE_WINDOW ( hildon_window_get_type() )
#define HILDON_WINDOW(obj) \
    (GTK_CHECK_CAST (obj, HILDON_TYPE_WINDOW, HildonWindow))
#define HILDON_WINDOW_CLASS(klass) \
    (GTK_CHECK_CLASS_CAST ((klass),\
     HILDON_TYPE_WINDOW, HildonWindowClass))
#define HILDON_IS_WINDOW(obj) (GTK_CHECK_TYPE (obj, HILDON_TYPE_WINDOW))
#define HILDON_IS_WINDOW_CLASS(klass) \
    (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_WINDOW))

typedef struct _HildonWindow      HildonWindow;
typedef struct _HildonWindowClass HildonWindowClass;

/**
 * HildonWindowPrivate:
 *
 * This structure contains just internal data.
 * It should not be accessed directly.
 */
typedef struct _HildonWindowPrivate HildonWindowPrivate;

struct _HildonWindow
{
    GtkWindow parent;

    /*private*/
    HildonWindowPrivate *priv;
};

struct _HildonWindowClass
{
    GtkWindowClass parent_class;

    /* Padding for future extension */
    void (*_hildon_reserved1)(void);
    void (*_hildon_reserved2)(void);
    void (*_hildon_reserved3)(void);
    void (*_hildon_reserved4)(void);
};


GType       hildon_window_get_type          (void);

GtkWidget * hildon_window_new               (void);

void        hildon_window_add_with_scrollbar(HildonWindow *self,
                                             GtkWidget    *child);

GtkMenu *   hildon_window_get_menu          (HildonWindow *self);
void        hildon_window_set_menu          (HildonWindow *self,
                                             GtkMenu      *menu);

void        hildon_window_add_toolbar       (HildonWindow *self,
                                             GtkToolbar   *toolbar);

void        hildon_window_remove_toolbar    (HildonWindow *self,
                                             GtkToolbar   *toolbar);

gboolean    hildon_window_get_is_topmost    (HildonWindow *self);


G_END_DECLS
#endif /* HILDON_WINDOW_H */
