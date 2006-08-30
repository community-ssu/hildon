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

#ifndef __HILDON_COLOR_POPUP_H__
#define __HILDON_COLOR_POPUP_H__

#include <gtk/gtkcontainer.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkdialog.h>

G_BEGIN_DECLS

typedef struct
{
  GtkWidget *ctrlbar_red;
  GtkWidget *ctrlbar_green;
  GtkWidget *ctrlbar_blue;

} HildonColorPopup;

GtkWidget *hildon_color_popup_new(GtkWindow        *parent,
                                  const GdkColor   *initial_color,
                                  HildonColorPopup *popupdata);

void       hildon_color_popup_set_color_from_sliders(GdkColor *color,
                                                     HildonColorPopup *popupdata);


G_END_DECLS

#endif /* __HILDON_COLOR_POPUP_H__ */
