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

#ifndef HILDON_VVOLUMEBAR_H
#define HILDON_VVOLUMEBAR_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtkframe.h>
#include <gtk/gtktogglebutton.h>
#include <hildon-widgets/hildon-volumebar.h>

G_BEGIN_DECLS
#define HILDON_TYPE_VVOLUMEBAR ( hildon_vvolumebar_get_type() )
#define HILDON_VVOLUMEBAR(obj) (GTK_CHECK_CAST (obj,\
        HILDON_TYPE_VVOLUMEBAR, HildonVVolumebar))
#define HILDON_VVOLUMEBAR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_VVOLUMEBAR, HildonVVolumebarClass))
#define HILDON_IS_VVOLUMEBAR(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_TYPE_VVOLUMEBAR))
#define HILDON_IS_VVOLUMEBAR_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_VVOLUMEBAR))
typedef struct _HildonVVolumebar HildonVVolumebar;
typedef struct _HildonVVolumebarClass HildonVVolumebarClass;

struct _HildonVVolumebar {
    /* This is our parent class */
    HildonVolumebar volumebar;
};

struct _HildonVVolumebarClass {
    HildonVolumebarClass parent_class;
};

GType hildon_vvolumebar_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_vvolumebar_new(void);

G_END_DECLS
#endif /* HILDON_VVOLUMEBAR */
