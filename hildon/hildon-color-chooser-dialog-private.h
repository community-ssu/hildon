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

#ifndef                                         __HILDON_COLOR_CHOOSER_DIALOG_PRIVATE_H__
#define                                         __HILDON_COLOR_CHOOSER_DIALOG_PRIVATE_H__

#include                                        <gconf/gconf-client.h>

typedef struct                                  _HildonColorChooserDialogPrivate HildonColorChooserDialogPrivate;

#define                                         HILDON_COLOR_CHOOSER_DIALOG_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
                                                HILDON_TYPE_COLOR_CHOOSER_DIALOG, HildonColorChooserDialogPrivate));

typedef struct 
{
    GtkBorder radio_sizes;
    GtkBorder cont_sizes;
    GtkBorder num_buttons;
    GtkBorder last_num_buttons;

    GdkColor default_color;
}                                               HildonColorChooserStyleInfo;

struct                                          _HildonColorChooserDialogPrivate 
{
    GtkWidget *hbox;
    GtkWidget *vbox;

    GtkWidget *align_custom, *align_defined;
    GtkWidget *area_custom, *area_defined;
    GtkWidget *separator;

    GtkWidget *chooser;

    GdkColor *colors_custom, *colors_defined;
    GdkColor color;
    GdkGC **gc_array;

    gint selected;

    HildonColorChooserStyleInfo style_info;

    gint has_style;

    GdkColor pending_color;

    GConfClient *gconf_client;
};

#endif                                          /* __HILDON_COLOR_CHOOSER_DIALOG_PRIVATE_H__ */
