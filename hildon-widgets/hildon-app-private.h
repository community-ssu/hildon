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


#ifndef HILDON_APP_PRIVATE_H
#define HILDON_APP_PRIVATE_H

G_BEGIN_DECLS

enum {
    TOPMOST_STATUS_ACQUIRE,
    TOPMOST_STATUS_LOSE,
    SWITCH_TO,
    IM_CLOSE,
    CLIPBOARD_COPY,
    CLIPBOARD_CUT,
    CLIPBOARD_PASTE,

    HILDON_APP_LAST_SIGNAL
};

struct _HildonAppPrivate {
    GList *children;
    gchar *title;
#ifndef HILDON_DISABLE_DEPRECATED
    HildonZoomLevel zoom;
#endif

    /* Used to keep track of menu key press/release */
    gint lastmenuclick;

    gulong curr_view_id;
    gulong view_id_counter;
    GSList *view_ids;
    gboolean scroll_control;

    guint twoparttitle: 1;
    guint is_topmost: 1;
    gboolean killable;
    gboolean autoregistration;

    guint escape_timeout;
    guint key_snooper;
    
    GtkUIManager *uim;
    
    guint active_menu_id;
};

typedef struct {
  gpointer view_ptr;
  unsigned long view_id;
} view_item;

G_END_DECLS

#endif /* HILDON_APP_PRIVATE_H */
