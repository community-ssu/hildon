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

/**
 * SECTION:hildon-defines
 * @short_description: A collection of usefull defines. 
 *
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        <gtk/gtk.h>
#include                                        "hildon-defines.h"

/**
 * hildon_get_icon_pixel_size:
 * @size: the icon size to get pixel size for
 *
 * Returns the icon size (height) for the given, named icon.
 * In most cases it's much more convienient to call one of the 
 * predefined macros instead of this function directly.
 *
 * Returns: the height/width of icon to use. O if icon could not be found. 
 */
gint
hildon_get_icon_pixel_size                      (GtkIconSize size)
{
    gint w, h;
    
    if (gtk_icon_size_lookup (size, &w, &h))
        return h;
    else
        return 0;
}


