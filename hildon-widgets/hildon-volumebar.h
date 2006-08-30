/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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
#ifndef __HILDON_VOLUMEBAR_H__
#define __HILDON_VOLUMEBAR_H__

#include <gtk/gtkcontainer.h>
#include <gtk/gtkadjustment.h>

G_BEGIN_DECLS

#define HILDON_TYPE_VOLUMEBAR ( hildon_volumebar_get_type() )
#define HILDON_VOLUMEBAR(obj) (GTK_CHECK_CAST (obj,\
        HILDON_TYPE_VOLUMEBAR, HildonVolumebar))
#define HILDON_VOLUMEBAR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_VOLUMEBAR, HildonVolumebarClass))
#define HILDON_IS_VOLUMEBAR(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_TYPE_VOLUMEBAR))
#define HILDON_IS_VOLUMEBAR_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_VOLUMEBAR))

typedef struct _HildonVolumebar      HildonVolumebar;
typedef struct _HildonVolumebarClass HildonVolumebarClass;

struct _HildonVolumebar {
    GtkContainer par;
};

struct _HildonVolumebarClass {
    GtkContainerClass parent_class;

    /* signals */
    void (*mute_toggled)  (HildonVolumebar * self);
    void (*level_changed) (HildonVolumebar * self);
};


GType           hildon_volumebar_get_type       (void) G_GNUC_CONST;

double          hildon_volumebar_get_level      (HildonVolumebar *self);
void            hildon_volumebar_set_level      (HildonVolumebar *self,
                                                 gdouble          level);

gboolean        hildon_volumebar_get_mute       (HildonVolumebar *self);
void            hildon_volumebar_set_mute       (HildonVolumebar *self,
                                                 gboolean         mute);

void            hildon_volumebar_level_change   (HildonVolumebar *self);

GtkAdjustment * hildon_volumebar_get_adjustment (HildonVolumebar *self);


G_END_DECLS
#endif /* __HILDON_VOLUMEBAR_H__ */
