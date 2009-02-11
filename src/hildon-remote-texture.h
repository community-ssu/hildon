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

#ifndef                                         __HILDON_REMOTE_TEXTURE_H__
#define                                         __HILDON_REMOTE_TEXTURE_H__

#include                                        "hildon-window.h"
#include                                        <gtk/gtk.h>
#include                                        <sys/types.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_REMOTE_TEXTURE \
                                                (hildon_remote_texture_get_type())

#define                                         HILDON_REMOTE_TEXTURE(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_REMOTE_TEXTURE, \
                                                HildonRemoteTexture))

#define                                         HILDON_REMOTE_TEXTURE_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_REMOTE_TEXTURE, \
                                                HildonRemoteTextureClass))

#define                                         HILDON_IS_REMOTE_TEXTURE(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                HILDON_TYPE_REMOTE_TEXTURE))

#define                                         HILDON_IS_REMOTE_TEXTURE_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                HILDON_TYPE_REMOTE_TEXTURE))

#define                                         HILDON_REMOTE_TEXTURE_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_REMOTE_TEXTURE, \
                                                HildonRemoteTextureClass))

typedef struct                                  _HildonRemoteTexture HildonRemoteTexture;
typedef struct                                  _HildonRemoteTextureClass HildonRemoteTextureClass;

struct                                          _HildonRemoteTextureClass
{
    GtkWindowClass parent_class;

    /* Padding for future extension */
    void (*_hildon_reserved1)(void);
    void (*_hildon_reserved2)(void);
    void (*_hildon_reserved3)(void);
    void (*_hildon_reserved4)(void);
};

struct                                          _HildonRemoteTexture
{
    GtkWindow parent;
};

GType
hildon_remote_texture_get_type                (void) G_GNUC_CONST;

GtkWidget*
hildon_remote_texture_new                     (void);

void
hildon_remote_texture_send_message (HildonRemoteTexture *self,
                                     guint32 message_type,
                                     guint32 l0,
                                     guint32 l1,
                                     guint32 l2,
                                     guint32 l3,
                                     guint32 l4);

void
hildon_remote_texture_set_image (HildonRemoteTexture *self,
                                 key_t key,
                                 guint width,
                                 guint height,
                                 guint bpp);
void
hildon_remote_texture_update_area (HildonRemoteTexture *self,
                                   gint x,
                                   gint y,
                                   gint width,
                                   gint height);
void
hildon_remote_texture_set_show_full (HildonRemoteTexture *self,
				      gint show,
				      gint opacity);
void
hildon_remote_texture_set_show (HildonRemoteTexture *self,
				 gint show);
void
hildon_remote_texture_set_opacity (HildonRemoteTexture *self,
				    gint opacity);
void
hildon_remote_texture_set_position (HildonRemoteTexture *self,
                                    gint x,
                                    gint y,
                                    gint width,
                                    gint height);
void
hildon_remote_texture_set_offset (HildonRemoteTexture *self,
                                  double x,
                                  double y);
void
hildon_remote_texture_set_scale (HildonRemoteTexture *self,
                                 double x_scale,
                                 double y_scale);
void
hildon_remote_texture_set_parent (HildonRemoteTexture *self,
				   GtkWindow *parent);

G_END_DECLS

#endif                                 /* __HILDON_REMOTE_TEXTURE_H__ */
