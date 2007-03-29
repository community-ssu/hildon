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

#ifndef                                         __HILDON_NUMBER_EDITOR_PRIVATE_H__
#define                                         __HILDON_NUMBER_EDITOR_PRIVATE_H__

G_BEGIN_DECLS

typedef struct                                  _HildonNumberEditorPrivate HildonNumberEditorPrivate;

#define                                         HILDON_NUMBER_EDITOR_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_NUMBER_EDITOR, \
                                                HildonNumberEditorPrivate));

struct                                          _HildonNumberEditorPrivate
{
    /* Child widgets */
    GtkWidget *num_entry;
    GtkWidget *plus;
    GtkWidget *minus;

    gint start; /* Minimum */
    gint end;   /* Maximum */
    gint default_val;
    gint button_type; /* Type of button pressed: 1 = plus, -1 = minus */

    /* Timer IDs */
    guint button_event_id; /* Repeat change when button is held */
    guint select_all_idle_id; /* Selection repaint hack
				 see hildon_number_editor_select_all */
};

G_END_DECLS

#endif                                          /* __HILDON_NUMBER_EDITOR_PRIVATE_H__ */
