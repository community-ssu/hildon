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


#ifndef HILDON_APPVIEW_H
#define HILDON_APPVIEW_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkbin.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtktoolbar.h>
#include <gdk/gdkx.h>


G_BEGIN_DECLS
#define HILDON_TYPE_APPVIEW ( hildon_appview_get_type() )
#define HILDON_APPVIEW(obj) \
    (GTK_CHECK_CAST (obj, HILDON_TYPE_APPVIEW, HildonAppView))
#define HILDON_APPVIEW_CLASS(klass) \
    (GTK_CHECK_CLASS_CAST ((klass),\
     HILDON_TYPE_APPVIEW, HildonAppViewClass))
#define HILDON_IS_APPVIEW(obj) (GTK_CHECK_TYPE (obj, HILDON_TYPE_APPVIEW))
#define HILDON_IS_APPVIEW_CLASS(klass) \
    (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_APPVIEW))
typedef struct _HildonAppView HildonAppView;
typedef struct _HildonAppViewClass HildonAppViewClass;

/**
 * HildonAppViewPrivate:
 *
 * This structure contains just internal data. It should not
 * be accessed directly.
 */
typedef struct _HildonAppViewPrivate HildonAppViewPrivate;

struct _HildonAppView {
    GtkBin parent;

    /*public*/
    GtkWidget *vbox;

    /*private*/
    HildonAppViewPrivate *priv;
};

struct _HildonAppViewClass {
    GtkBinClass parent_class;
    void (*toolbar_changed) (HildonAppView * self);
    void (*toolbar_toggle_request) (HildonAppView * self);
    void (*fullscreen_state_change) (HildonAppView * self,
                                     gboolean is_fullscreen);
    void (*title_change) (HildonAppView * self);
    void (*switched_to) (HildonAppView * self);
    void (*switched_from) (HildonAppView * self);
    void (*increase_button_event) (HildonAppView * self,
                                    guint newkeytype);
    void (*decrease_button_event) (HildonAppView * self,
                                    guint newkeytype);
};


GType hildon_appview_get_type(void);
GtkWidget *hildon_appview_new(const gchar * title);
void hildon_appview_add_with_scrollbar(HildonAppView * self,
                                       GtkWidget * child);
void hildon_appview_set_fullscreen_key_allowed(HildonAppView * self,
                                               gboolean allow);
gboolean hildon_appview_get_fullscreen_key_allowed(HildonAppView * self);

gboolean hildon_appview_get_fullscreen(HildonAppView * self);
void hildon_appview_set_fullscreen(HildonAppView * self,
                                   gboolean fullscreen);
GtkMenu *hildon_appview_get_menu(HildonAppView * self);
#ifndef HILDON_DISABLE_DEPRECATED
void hildon_appview_set_toolbar(HildonAppView * self,
                                GtkToolbar * toolbar);
GtkToolbar *hildon_appview_get_toolbar(HildonAppView * self);
#endif
void hildon_appview_set_title(HildonAppView * self, const gchar * newname);
const gchar *hildon_appview_get_title(HildonAppView * self);

void _hildon_appview_toggle_menu(HildonAppView * self,
                                 Time button_event_time);
gboolean _hildon_appview_menu_visible(HildonAppView * self);

void hildon_appview_set_connected_adjustment (HildonAppView * self,
					      GtkAdjustment * adjustment);
GtkAdjustment * hildon_appview_get_connected_adjustment (HildonAppView * self);

void _hildon_appview_increase_button_state_changed (HildonAppView * self,
						    guint newkeytype);
void _hildon_appview_decrease_button_state_changed (HildonAppView * self,
						    guint newkeytype);

void hildon_appview_set_menu_ui(HildonAppView *self, const gchar *ui_string);
const gchar *hildon_appview_get_menu_ui(HildonAppView *self);

G_END_DECLS
#endif /* HILDON_APPVIEW_H */
