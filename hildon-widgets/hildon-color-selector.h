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

#ifndef __HILDON_COLOR_SELECTOR_H__
#define __HILDON_COLOR_SELECTOR_H__

#include <gtk/gtkcontainer.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_COLOR_SELECTOR \
  ( hildon_color_selector_get_type() )
#define HILDON_COLOR_SELECTOR(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_COLOR_SELECTOR, HildonColorSelector))
#define HILDON_COLOR_SELECTOR_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_COLOR_SELECTOR,\
   HildonColorSelectorClass))
#define HILDON_IS_COLOR_SELECTOR(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_COLOR_SELECTOR))
#define HILDON_IS_COLOR_SELECTOR_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_COLOR_SELECTOR_CLASS))
    GType hildon_color_selector_get_type(void);

typedef struct _HildonColorSelector HildonColorSelector;
typedef struct _HildonColorSelectorClass HildonColorSelectorClass;

/**
 * HildonColorSelectorPriv:
 *
 * Internal struct for color selector.
 */
typedef struct _HildonColorSelectorPriv HildonColorSelectorPriv;

struct _HildonColorSelector {
    GtkDialog parent;
    HildonColorSelectorPriv *priv;
};

struct _HildonColorSelectorClass {
    GtkDialogClass parent_class;
};

GType hildon_color_selector_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_color_selector_new(GtkWindow * parent);
G_CONST_RETURN GdkColor *hildon_color_selector_get_color(HildonColorSelector * selector);
void hildon_color_selector_set_color(HildonColorSelector * selector,
                                     GdkColor * color);

G_END_DECLS
#endif
