/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
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

#ifndef __HILDON_HVOLUMEBAR_H__
#define __HILDON_HVOLUMEBAR_H__

#include <hildon-widgets/hildon-volumebar.h>

G_BEGIN_DECLS

#define HILDON_TYPE_HVOLUMEBAR ( hildon_hvolumebar_get_type() )
#define HILDON_HVOLUMEBAR(obj) (GTK_CHECK_CAST (obj,\
        HILDON_TYPE_HVOLUMEBAR, HildonHVolumebar))
#define HILDON_HVOLUMEBAR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_HVOLUMEBAR, HildonHVolumebarClass))
#define HILDON_IS_HVOLUMEBAR(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_TYPE_HVOLUMEBAR))
#define HILDON_IS_HVOLUMEBAR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass),\
        HILDON_TYPE_HVOLUMEBAR))

typedef struct _HildonHVolumebar HildonHVolumebar;
typedef struct _HildonHVolumebarClass HildonHVolumebarClass;

struct _HildonHVolumebar {
    /* This is our parent class */
    HildonVolumebar volumebar;
};

struct _HildonHVolumebarClass {
    HildonVolumebarClass parent_class;
};

GType hildon_hvolumebar_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_hvolumebar_new(void);

G_END_DECLS
#endif /* __HILDON_HVOLUMEBAR_H__ */
