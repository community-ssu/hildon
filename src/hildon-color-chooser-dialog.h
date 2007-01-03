/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
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

#ifndef                                         __HILDON_COLOR_CHOOSER_DIALOG_H__
#define                                         __HILDON_COLOR_CHOOSER_DIALOG_H__

#include                                        <gdk/gdkcolor.h>
#include                                        <gtk/gtkdialog.h>

#define                                         HILDON_TYPE_COLOR_CHOOSER_DIALOG \
                                                (hildon_color_chooser_dialog_get_type())

#define                                         HILDON_COLOR_CHOOSER_DIALOG(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_COLOR_CHOOSER_DIALOG, \
                                                HildonColorChooserDialog))

#define                                         HILDON_COLOR_CHOOSER_DIALOG_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_COLOR_CHOOSER_DIALOG, \
                                                HildonColorChooserDialogClass))

#define                                         HILDON_IS_COLOR_CHOOSER_DIALOG(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                HILDON_TYPE_COLOR_CHOOSER_DIALOG))

typedef struct                                  _HildonColorChooserDialog HildonColorChooserDialog;

typedef struct                                  _HildonColorChooserDialogClass HildonColorChooserDialogClass;

struct                                          _HildonColorChooserDialog
{
    GtkDialog parent;

    GdkColor color;
};

struct                                          _HildonColorChooserDialogClass 
{
    GtkDialogClass parent_class;
};

GType G_GNUC_CONST
hildon_color_chooser_dialog_get_type            (void);

GtkWidget*
hildon_color_chooser_dialog_new                 (void);

void
hildon_color_chooser_dialog_set_color           (HildonColorChooserDialog *chooser, 
                                                 GdkColor *color);

void
hildon_color_chooser_dialog_get_color           (HildonColorChooserDialog *chooser,
                                                 GdkColor *color);

#endif                                          /* __HILDON_COLOR_CHOOSER_DIALOG_H__ */
