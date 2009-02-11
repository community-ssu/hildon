/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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

#ifndef                                         __HILDON_REMOTE_TEXTURE_PRIVATE_H__
#define                                         __HILDON_REMOTE_TEXTURE_PRIVATE_H__


#include <gtk/gtk.h>
#include <sys/types.h>

G_BEGIN_DECLS

typedef struct                                  _HildonRemoteTexturePrivate HildonRemoteTexturePrivate;

#define                                         HILDON_REMOTE_TEXTURE_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_REMOTE_TEXTURE, HildonRemoteTexturePrivate));

struct                                          _HildonRemoteTexturePrivate
{
    guint   ready : 1;

    guint   set_shm : 1;
    guint   set_damage : 1;
    guint   set_show : 1;
    guint   set_position : 1;
    guint   set_offset : 1;
    guint   set_scale : 1;
    guint   set_parent : 1;

    key_t   shm_key;
    guint   shm_width;
    guint   shm_height;
    guint   shm_bpp;

    gint    damage_x1;
    gint    damage_y1;
    gint    damage_x2;
    gint    damage_y2;

    guint   show;
    guint   opacity;

    gint    x;
    gint    y;
    gint    width;
    gint    height;

    double   offset_x;
    double   offset_y;

    double  scale_x;
    double  scale_y;

    GtkWindow* parent;
    gulong  parent_map_event_cb_id;

    gulong  map_event_cb_id;
};

G_END_DECLS

#endif                                          /* __HILDON_REMOTE_TEXTURE_PRIVATE_H__ */
