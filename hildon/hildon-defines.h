/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
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

/* FIXME: Add documentation for the macros here */

#ifndef                                         __HILDON_DEFINES_H__
#define                                         __HILDON_DEFINES_H__

#include                                        <gtk/gtk.h>
#include                                        <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

/* New hildon icon sizes. */

#define HILDON_ICON_SIZE_XSMALL                  gtk_icon_size_from_name ("hildon-xsmall")

#define HILDON_ICON_SIZE_SMALL                   gtk_icon_size_from_name ("hildon-small")

#define HILDON_ICON_SIZE_STYLUS                  gtk_icon_size_from_name ("hildon-stylus")

#define HILDON_ICON_SIZE_FINGER                  gtk_icon_size_from_name ("hildon-finger")

#define HILDON_ICON_SIZE_THUMB                   gtk_icon_size_from_name ("hildon-thumb")

#define HILDON_ICON_SIZE_LARGE                   gtk_icon_size_from_name ("hildon-large")

#define HILDON_ICON_SIZE_XLARGE                  gtk_icon_size_from_name ("hildon-xlarge")

/* legacy hildon icon sizes, updated for new hildon */

#define                                         HILDON_ICON_SIZE_TOOLBAR \
                                                gtk_icon_size_from_name ("hildon-finger")
/* Actual icon sizes */

/* New sizes */
#define                                         HILDON_ICON_PIXEL_SIZE_XSMALL \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_XSMALL)

#define                                         HILDON_ICON_PIXEL_SIZE_SMALL \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_SMALL)

#define                                         HILDON_ICON_PIXEL_SIZE_STYLUS \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_STYLUS)

#define                                         HILDON_ICON_PIXEL_SIZE_FINGER \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_FINGER)

#define                                         HILDON_ICON_PIXEL_SIZE_THUMB \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_THUMB)

#define                                         HILDON_ICON_PIXEL_SIZE_LARGE \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_LARGE)

#define                                         HILDON_ICON_PIXEL_SIZE_XLARGE \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_XLARGE)

/* legacy sizes */
#define                                         HILDON_ICON_PIXEL_SIZE_TOOLBAR \
                                                hildon_get_icon_pixel_size (HILDON_ICON_SIZE_TOOLBAR)

/* Margins */

#define                                         HILDON_MARGIN_HALF 4

#define                                         HILDON_MARGIN_DEFAULT 8

#define                                         HILDON_MARGIN_DOUBLE 16

#define                                         HILDON_MARGIN_TRIPLE 24

#define                                         HILDON_WINDOW_TITLEBAR_HEIGHT 56

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
