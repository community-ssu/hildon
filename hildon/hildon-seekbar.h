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

#ifndef                                         HILDON_DISABLE_DEPRECATED

#ifndef                                         __HILDON_SEEKBAR_H__
#define                                         __HILDON_SEEKBAR_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_SEEKBAR \
                                                (hildon_seekbar_get_type())

#define                                         HILDON_SEEKBAR(obj) \
                                                (GTK_CHECK_CAST (obj,\
                                                HILDON_TYPE_SEEKBAR, HildonSeekbar))

#define                                         HILDON_SEEKBAR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_SEEKBAR, HildonSeekbarClass))

#define                                         HILDON_IS_SEEKBAR(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_SEEKBAR))

#define                                         HILDON_IS_SEEKBAR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass),\
                                                HILDON_TYPE_SEEKBAR))

#define                                         HILDON_SEEKBAR_GET_CLASS(obj) \
                                                ((HildonSeekBarClass *) G_OBJECT_GET_CLASS(obj))


typedef struct                                  _HildonSeekbar HildonSeekbar;

typedef struct                                  _HildonSeekbarClass HildonSeekbarClass;

struct                                          _HildonSeekbar 
{
    GtkScale parent;
};

struct                                          _HildonSeekbarClass
{
    GtkScaleClass parent_class;
};

GType G_GNUC_CONST
hildon_seekbar_get_type                         (void);

GtkWidget*
hildon_seekbar_new                              (void);

gint
hildon_seekbar_get_total_time                   (HildonSeekbar *seekbar);

void
hildon_seekbar_set_total_time                   (HildonSeekbar *seekbar, 
                                                 gint time);

gint
hildon_seekbar_get_position                     (HildonSeekbar *seekbar);

void 
hildon_seekbar_set_position                     (HildonSeekbar *seekbar, 
                                                 gint time);

void 
hildon_seekbar_set_fraction                     (HildonSeekbar *seekbar, 
                                                 guint fraction);

guint
hildon_seekbar_get_fraction                     (HildonSeekbar *seekbar);

G_END_DECLS

#endif                                          /* __HILDON_SEEKBAR_H__ */

#endif                                          /* HILDON_DISABLE_DEPRECATED */
