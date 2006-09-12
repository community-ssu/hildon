/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License or any later version.
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


#ifndef __HILDON_WINDOW_PRIVATE_H__
#define __HILDON_WINDOW_PRIVATE_H__

G_BEGIN_DECLS

void
hildon_window_set_program (HildonWindow *self, GObject *program);

void
hildon_window_unset_program (HildonWindow *self);

void 
hildon_window_set_can_hibernate_property (HildonWindow *self, 
                                          gpointer can_hibernate);

void
hildon_window_take_common_toolbar (HildonWindow *self);

void
hildon_window_update_topmost (HildonWindow *self, Window window_id);

Window
hildon_window_get_active_window (void);

void
hildon_window_update_title (HildonWindow *window);

G_END_DECLS
#endif /* __HILDON_WINDOW_PRIVATE_H__ */
