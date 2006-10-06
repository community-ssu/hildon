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

#ifndef __HILDON_CONTROLBAR_H__
#define __HILDON_CONTROLBAR_H__

#include <gtk/gtkscale.h>

G_BEGIN_DECLS
#define HILDON_TYPE_CONTROLBAR ( hildon_controlbar_get_type() )
#define HILDON_CONTROLBAR(obj) (GTK_CHECK_CAST (obj,\
  HILDON_TYPE_CONTROLBAR, HildonControlbar))
#define HILDON_CONTROLBAR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
  HILDON_TYPE_CONTROLBAR, HildonControlbarClass))
#define HILDON_IS_CONTROLBAR(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_CONTROLBAR))
#define HILDON_IS_CONTROLBAR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass),\
  HILDON_TYPE_CONTROLBAR))

/**
 * HildonControlbar:
 *
 * Contains gboolean variable named 'button_press' whether a button
 * has been pressed.
 */
typedef struct _HildonControlbar HildonControlbar;
typedef struct _HildonControlbarClass HildonControlbarClass;

struct _HildonControlbar {
    GtkScale scale;
};

struct _HildonControlbarClass {
    GtkScaleClass parent_class;
    void (*end_reached) (HildonControlbar *controlbar, gboolean end);
};

GType hildon_controlbar_get_type(void);
GtkWidget *hildon_controlbar_new(void);
void hildon_controlbar_set_value(HildonControlbar * self, gint value);
gint hildon_controlbar_get_value(HildonControlbar * self);
gint hildon_controlbar_get_max(HildonControlbar * self);
gint hildon_controlbar_get_min(HildonControlbar * self);
void hildon_controlbar_set_max(HildonControlbar * self, gint max);
void hildon_controlbar_set_min(HildonControlbar * self, gint min);
void hildon_controlbar_set_range(HildonControlbar * self, gint min,
                                 gint max);

G_END_DECLS
#endif /* __HILDON_CONTROLBAR_H__ */
