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

#ifndef                                         __HILDON_COLOR_BUTTON_PRIVATE_H__
#define                                         __HILDON_COLOR_BUTTON_PRIVATE_H__

typedef struct                                  _HildonColorButtonPrivate HildonColorButtonPrivate;

#define                                         HILDON_COLOR_BUTTON_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
                                                HILDON_TYPE_COLOR_BUTTON, HildonColorButtonPrivate));

struct                                          _HildonColorButtonPrivate
{
    GtkWidget *dialog;
    gboolean popped;

    GdkColor color;
    GdkGC *gc;
};

#endif                                          /* __HILDON_COLOR_BUTTON_PRIVATE_H__ */
