/*
 * This file is a part of hildon
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

/* FIXME: Add documentation for the macros here */

#ifndef                                         __HILDON_DEFINES_H__
#define                                         __HILDON_DEFINES_H__

#include                                        <gtk/gtk.h>
#include                                        <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

#define                                         HILDON_ICON_SIZE_SMALL \
                                                gtk_icon_size_from_name ("gtk-menu")

#define                                         HILDON_ICON_SIZE_TOOLBAR \
                                                gtk_icon_size_from_name ("gtk-menu")

#define                                         HILDON_ICON_SIZE_WIZARD \
                                                gtk_icon_size_from_name ("gtk-dialog")

#define                                         HILDON_ICON_SIZE_BIG_NOTE \
                                                gtk_icon_size_from_name ("gtk-dialog")

#define                                         HILDON_ICON_SIZE_NOTE \
                                                gtk_icon_size_from_name ("gtk-menu")

/* Actual icon sizes */

#define                                         HILDON_ICON_PIXEL_SIZE_SMALL \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_SMALL)

#define                                         HILDON_ICON_PIXEL_SIZE_TOOLBAR \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_TOOLBAR)

#define                                         HILDON_ICON_PIXEL_SIZE_WIZARD \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_WIZARD)

#define                                         HILDON_ICON_PIXEL_SIZE_BIG_NOTE \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_BIG_NOTE)

#define                                         HILDON_ICON_PIXEL_SIZE_NOTE \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_NOTE)

/* Margins */

#define                                         HILDON_MARGIN_HALF 3

#define                                         HILDON_MARGIN_DEFAULT 6

#define                                         HILDON_MARGIN_DOUBLE 12

#define                                         HILDON_MARGIN_TRIPLE 18

/* Hard keys */

#define                                         HILDON_HARDKEY_UP GDK_Up

#define                                         HILDON_HARDKEY_LEFT GDK_Left

#define                                         HILDON_HARDKEY_RIGHT GDK_Right

#define                                         HILDON_HARDKEY_DOWN GDK_Down

#define                                         HILDON_HARDKEY_SELECT GDK_Return

#define                                         HILDON_HARDKEY_MENU GDK_F4

#define                                         HILDON_HARDKEY_HOME GDK_F5

#define                                         HILDON_HARDKEY_ESC GDK_Escape

#define                                         HILDON_HARDKEY_FULLSCREEN GDK_F6

#define                                         HILDON_HARDKEY_INCREASE GDK_F7

#define                                         HILDON_HARDKEY_DECREASE GDK_F8

gint
hildon_get_icon_pixel_size                      (GtkIconSize size);

G_END_DECLS

#endif                                          /* HILDON_DEFINES_H */
