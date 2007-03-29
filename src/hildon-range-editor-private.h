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

#ifndef                                         __HILDON_RANGE_EDITOR_PRIVATE_H__
#define                                         __HILDON_RANGE_EDITOR_PRIVATE_H__

G_BEGIN_DECLS

#define                                         HILDON_RANGE_EDITOR_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_RANGE_EDITOR, HildonRangeEditorPrivate));

typedef struct                                  _HildonRangeEditorPrivate HildonRangeEditorPrivate;

struct                                          _HildonRangeEditorPrivate
{
    GtkWidget *start_entry; /* Entry for lower  value */
    GtkWidget *end_entry;   /* Entry for higher value */

    GtkWidget *label;

    gint range_limits_start; /* Minimum value allowed for range start/end */
    gint range_limits_end;   /* Maximum value allowed for range start/end */

    gboolean bp; /* Button pressed, don't overwrite selection */
};

G_END_DECLS

#endif                                          /* __HILDON_RANGE_EDITOR_PRIVATE_H__ */
