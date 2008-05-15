/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
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

/**
 * SECTION:hildon-stackable-window
 * @short_description: Widget representing a stackable, top-level window in the Hildon framework.
 *
 * The HildonStackableWindow is a GTK widget which represents a
 * top-level window in the Hildon framework. It is derived from
 * HildonWindow. Applications that use stackable windows are organized
 * in a hierarchical way so users can one from any window back to the
 * application's root window.
 */

#include                                        "hildon-stackable-window.h"

G_DEFINE_TYPE (HildonStackableWindow, hildon_stackable_window, HILDON_TYPE_WINDOW);

static void
hildon_stackable_window_class_init              (HildonStackableWindowClass *klass)
{
}

static void
hildon_stackable_window_init                    (HildonStackableWindow *self)
{
}

/**
 * hildon_stackable_window_new:
 *
 * Creates a new HildonStackableWindow
 *
 * Return value: A @HildonStackableWindow
 **/
GtkWidget*
hildon_stackable_window_new                     (void)
{
    HildonStackableWindow *newwindow = g_object_new (HILDON_TYPE_STACKABLE_WINDOW, NULL);

    return GTK_WIDGET (newwindow);
}
