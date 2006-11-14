/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
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

/*
 * @file hildon-grid-item-private.h
 *
 * This file is a private header file for he implementation of
 * HildonGridItem. HildonGridItem is an item mainly used in HildonGrid. It
 * has an icon, emblem and a label. This private header file exists so that
 * grid can call semi-public functions of an item.
 */

#ifndef __HILDON_GRID_ITEM_PRIVATE_H__
#define __HILDON_GRID_ITEM_PRIVATE_H__

#include <hildon-widgets/hildon-grid-item.h>

G_BEGIN_DECLS


void _hildon_grid_item_set_label(HildonGridItem *item,
                                 const gchar *label);

void _hildon_grid_item_set_emblem_size(HildonGridItem *item,
                                       const gint emblem_size);
void _hildon_grid_item_set_label_pos(HildonGridItem *item,
                                     HildonGridPositionType label_pos);

void _hildon_grid_item_set_icon_size(HildonGridItem *item,
                                     HildonGridItemIconSizeType icon_size);

void _hildon_grid_item_set_focus_margin(HildonGridItem *item,
                                        const gint focus_margin);
void _hildon_grid_item_set_label_height(HildonGridItem *item,
                                        const gint label_height);
void _hildon_grid_item_set_label_icon_margin(HildonGridItem *item,
                                             const gint label_icon_margin);
void _hildon_grid_item_set_icon_width(HildonGridItem *item,
                                      const gint icon_width);
void _hildon_grid_item_set_icon_height(HildonGridItem *item,
                                       const gint icon_height);
void _hildon_grid_item_set_label_height(HildonGridItem *item,
 					const gint label_height);

void _hildon_grid_item_done_updating_settings(HildonGridItem *item);


G_END_DECLS

#endif /* __HILDON_GRID_ITEM_PRIVATE_H__ */
