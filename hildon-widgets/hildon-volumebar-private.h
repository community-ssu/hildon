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
#ifndef __HILDON_VOLUMEBAR_PRIVATE__
#define __HILDON_VOLUMEBAR_PRIVATE__

#define HILDON_VOLUMEBAR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	HILDON_TYPE_VOLUMEBAR, HildonVolumebarPrivate));

typedef struct _HildonVolumebarPrivate HildonVolumebarPrivate;

struct _HildonVolumebarPrivate {
  HildonVolumebarRange *volumebar;

  GtkOrientation orientation;   /* Orientation of range *//* FIXME: XXX This isn't needed */
  GtkOrientation ownorientation;/* Orientation of widget *//* FIXME: XXX This isn't needed */

  GtkToggleButton *tbutton;

  gboolean is_toolbar;/* Is inside toolbar (for horizontal volumebar) */
};

#endif /* __HILDON_VOLUMEBAR_PRIVATE__ */
