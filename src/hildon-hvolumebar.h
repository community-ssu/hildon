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

#ifndef                                         HILDON_DISABLE_DEPRECATED

#ifndef                                         __HILDON_HVOLUMEBAR_H__
#define                                         __HILDON_HVOLUMEBAR_H__

#include                                        "hildon-volumebar.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_HVOLUMEBAR (hildon_hvolumebar_get_type())

#define                                         HILDON_HVOLUMEBAR(obj) (GTK_CHECK_CAST (obj,\
                                                HILDON_TYPE_HVOLUMEBAR, HildonHVolumebar))

#define                                         HILDON_HVOLUMEBAR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_HVOLUMEBAR, HildonHVolumebarClass))

#define                                         HILDON_IS_HVOLUMEBAR(obj) (GTK_CHECK_TYPE (obj,\
                                                HILDON_TYPE_HVOLUMEBAR))

#define                                         HILDON_IS_HVOLUMEBAR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass),\
                                                HILDON_TYPE_HVOLUMEBAR))

#define                                         HILDON_HVOLUMEBAR_GET_CLASS(obj) \
                                                ((HildonHVolumebarClass *) G_OBJECT_GET_CLASS(obj))

typedef struct                                  _HildonHVolumebar HildonHVolumebar;

typedef struct                                  _HildonHVolumebarClass HildonHVolumebarClass;

struct                                          _HildonHVolumebar 
{
    HildonVolumebar parent;
};

struct                                          _HildonHVolumebarClass 
{
    HildonVolumebarClass parent_class;
};

GType G_GNUC_CONST 
hildon_hvolumebar_get_type                      (void);

GtkWidget*
hildon_hvolumebar_new                           (void);

G_END_DECLS

#endif                                          /* __HILDON_HVOLUMEBAR_H__ */

#endif                                          /* HILDON_DISABLE_DEPRECATED */
