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
 
#ifndef                                         __HILDON_CAPTION_PRIVATE_H__
#define                                         __HILDON_CAPTION_PRIVATE_H__

G_BEGIN_DECLS

typedef struct                                  _HildonCaptionPrivate HildonCaptionPrivate;

#define                                         HILDON_CAPTION_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_CAPTION, HildonCaptionPrivate));

struct                                          _HildonCaptionPrivate
{
    GtkWidget *caption_area;
    GtkWidget *label;
    GtkWidget *icon;
    GtkWidget *icon_align; /* Arbitrary icon widgets do not support alignment */
    GtkSizeGroup *group;
    gchar *text;
    gchar *separator;
    guint is_focused : 1;
    guint expand : 1;
    HildonCaptionStatus status;
    HildonCaptionIconPosition icon_position;
};

G_END_DECLS

#endif                                          /* __HILDON_CAPTION_PRIVATE_H__ */
