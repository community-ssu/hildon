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

#ifndef __HILDON_VOLUMEBAR_RANGE_H__
#define __HILDON_VOLUMEBAR_RANGE_H__

#include <gtk/gtkscale.h>

G_BEGIN_DECLS

/**
 * HILDON_VOLUMEBAR_RANGE_TYPE
 *
 * Macro for getting type of volumebar range.
 * Since: 0.12.10
 */
#define HILDON_TYPE_VOLUMEBAR_RANGE ( hildon_volumebar_range_get_type() )

/**
 * HILDON_VOLUMEBAR_RANGE_TYPE
 *
 * Deprecated: use #HILDON_VOLUMEBAR_RANGE_TYPE instead
 */
#define HILDON_VOLUMEBAR_RANGE_TYPE HILDON_TYPE_VOLUMEBAR_RANGE


#define HILDON_VOLUMEBAR_RANGE(obj) (GTK_CHECK_CAST (obj,\
        HILDON_TYPE_VOLUMEBAR_RANGE, HildonVolumebarRange))
#define HILDON_VOLUMEBAR_RANGE_CLASS(klass) \
        (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_VOLUMEBAR_RANGE, HildonVolumebarRangeClass))
#define HILDON_IS_VOLUMEBAR_RANGE(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_TYPE_VOLUMEBAR_RANGE))
#define HILDON_IS_VOLUMEBAR_RANGE_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_VOLUMEBAR_RANGE))

typedef struct _HildonVolumebarRange      HildonVolumebarRange;
typedef struct _HildonVolumebarRangeClass HildonVolumebarRangeClass;

struct _HildonVolumebarRange {
    GtkScale scale;
};

struct _HildonVolumebarRangeClass {
    GtkScaleClass parent_class;
};

GType       hildon_volumebar_range_get_type  (void) G_GNUC_CONST;
GtkWidget * hildon_volumebar_range_new       (GtkOrientation orientation);
gdouble     hildon_volumebar_range_get_level (HildonVolumebarRange *self);
void        hildon_volumebar_range_set_level (HildonVolumebarRange *self,
                                              gdouble               level);


G_END_DECLS

#endif /* __HILDON_VOLUMEBAR_RANGE_H__ */
