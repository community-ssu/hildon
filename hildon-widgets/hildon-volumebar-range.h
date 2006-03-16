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

#ifndef HILDON_VOLUMEBAR_RANGE_H
#define HILDON_VOLUMEBAR_RANGE_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkscale.h>

G_BEGIN_DECLS
#define HILDON_VOLUMEBAR_RANGE_TYPE ( hildon_volumebar_range_get_type() )
#define HILDON_VOLUMEBAR_RANGE(obj) (GTK_CHECK_CAST (obj,\
        HILDON_VOLUMEBAR_RANGE_TYPE, HildonVolumebarRange))
#define HILDON_VOLUMEBAR_RANGE_CLASS(klass) \
        (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_VOLUMEBAR_RANGE_TYPE, HildonVolumebarRangeClass))
#define HILDON_IS_VOLUMEBAR_RANGE(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_VOLUMEBAR_RANGE_TYPE))
#define HILDON_IS_VOLUMEBAR_RANGE_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_VOLUMEBAR_RANGE_TYPE))
typedef struct _HildonVolumebarRange HildonVolumebarRange;
typedef struct _HildonVolumebarRangeClass HildonVolumebarRangeClass;

struct _HildonVolumebarRange {
    GtkScale scale;
};

struct _HildonVolumebarRangeClass {
    GtkScaleClass parent_class;
};

GType hildon_volumebar_range_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_volumebar_range_new(GtkOrientation orientation);
gdouble hildon_volumebar_range_get_level(HildonVolumebarRange * self);
void hildon_volumebar_range_set_level(HildonVolumebarRange * self,
                                      gdouble level);


G_END_DECLS
#endif
