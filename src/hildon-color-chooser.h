/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
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

#ifndef                                         HILDON_DISABLE_DEPRECATED

#ifndef                                         __HILDON_COLOR_CHOOSER_H__
#define                                         __HILDON_COLOR_CHOOSER_H__

#include                                        <gtk/gtk.h>

#define                                         HILDON_TYPE_COLOR_CHOOSER \
                                                (hildon_color_chooser_get_type())

#define                                         HILDON_COLOR_CHOOSER(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_COLOR_CHOOSER, \
                                                HildonColorChooser))

#define                                         HILDON_COLOR_CHOOSER_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_COLOR_CHOOSER, \
                                                HildonColorChooserClass))

#define                                         HILDON_IS_COLOR_CHOOSER(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                HILDON_TYPE_COLOR_CHOOSER))

typedef struct                                  _HildonColorChooser HildonColorChooser;

typedef struct                                  _HildonColorChooserClass HildonColorChooserClass;

struct                                          _HildonColorChooser
{
    GtkWidget parent;
};

struct _HildonColorChooserClass
{
    GtkWidgetClass parent;

    void (*color_changed) (HildonColorChooser *selection, GdkColor *color);
    void (*set_color) (HildonColorChooser *, GdkColor *);
};

GType G_GNUC_CONST
hildon_color_chooser_get_type                   (void);

GtkWidget*
hildon_color_chooser_new                        (void);

void
hildon_color_chooser_set_color                  (HildonColorChooser *chooser, 
                                                 GdkColor *color);

void
hildon_color_chooser_get_color                  (HildonColorChooser *chooser, 
                                                 GdkColor *color);

#endif                                          /* __HILDON_COLOR_CHOOSER_H__ */

#endif                                          /* HILDON_DISABLE_DEPRECATED */
