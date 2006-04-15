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

#ifndef __HILDON_APP_H__
#define __HILDON_APP_H__

#include <gtk/gtkwindow.h>
#include <gtk/gtkuimanager.h>
#include "hildon-appview.h"

G_BEGIN_DECLS
/**
 * HildonApp:
 *
 * Contains only private data not to be touched by outsiders. 
 */
typedef struct _HildonApp HildonApp;
typedef struct _HildonAppClass HildonAppClass;

#define HILDON_TYPE_APP ( hildon_app_get_type() )

#define HILDON_APP(obj) (GTK_CHECK_CAST (obj, HILDON_TYPE_APP, \
            HildonApp))

#define HILDON_APP_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass), \
            HILDON_TYPE_APP, HildonAppClass))

#define HILDON_IS_APP(obj) (GTK_CHECK_TYPE (obj, HILDON_TYPE_APP))

#define HILDON_IS_APP_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), \
            HILDON_TYPE_APP))


struct _HildonApp {
    GtkWindow parent;
};

struct _HildonAppClass {
    GtkWindowClass parent_class;
    void (*topmost_status_acquire) (HildonApp *self);
    void (*topmost_status_lose) (HildonApp *self);
    void (*switch_to) (HildonApp *self); /* FIXME: Possible browser hack?? */
    void (*im_close) (HildonApp *self);
    void (*clipboard_copy) (HildonApp *self, GtkWidget *widget);
    void (*clipboard_cut) (HildonApp *self, GtkWidget *widget);
    void (*clipboard_paste) (HildonApp *self, GtkWidget *widget);
};

#ifndef HILDON_DISABLE_DEPRECATED
/* Make these values >0 so that we can detect when someone sets SMALL-1
 * zoom level (enum seems to be unsigned int)
 */

/**
 * HildonZoomLevel:
 * @HILDON_ZOOM_SMALL: Smallest font.
 * @HILDON_ZOOM_MEDIUM: Middle size font.
 * @HILDON_ZOOM_LARGE: Largest font.
 *
 * The Hildon zoom levels. Small meaning small font. Large meaning
 * large font. These font are specified in the gtkrc files.
 * This enum is deprecated and should not be used. It's just
 * lecagy stuff from ancient specs.
 */
typedef enum /*< skip >*/
{
    HILDON_ZOOM_SMALL = 1,
    HILDON_ZOOM_MEDIUM = 2,
    HILDON_ZOOM_LARGE = 3
} HildonZoomLevel;

#define HILDON_TYPE_ZOOM_LEVEL (hildon_zoom_level_get_type ())

GType hildon_zoom_level_get_type (void);
#endif /* deprecated */


/* You should use the correct ones from hildon-defines.h 
 *
 * FIXME: These should be removed when API changes are allowed, 
 * they are not used in hildon code anymore (but might be in external). 
 */ 
#define HILDON_MENU_KEY         GDK_F4
#define HILDON_HOME_KEY         GDK_F5
#define HILDON_TOOLBAR_KEY      GDK_T
#define HILDON_FULLSCREEN_KEY   GDK_F6
#define HILDON_INCREASE_KEY     GDK_F7
#define HILDON_DECREASE_KEY     GDK_F8
#define HILDON_TOOLBAR_MODIFIERS (GDK_SHIFT_MASK | GDK_CONTROL_MASK)

#define HILDON_KEYEVENT_IS_MENU_KEY(keyevent) (keyevent->keyval == HILDON_MENU_KEY)
#define HILDON_KEYEVENT_IS_HOME_KEY(keyevent) (keyevent->keyval == HILDON_HOME_KEY)
#define HILDON_KEYEVENT_IS_TOOLBAR_KEY(keyevent) ((keyevent->keyval == HILDON_TOOLBAR_KEY) && \
						   (keyevent->state == HILDON_TOOLBAR_MODIFIERS))
#define HILDON_KEYEVENT_IS_FULLSCREEN_KEY(keyevent) (keyevent->keyval == HILDON_FULLSCREEN_KEY)
#define HILDON_KEYEVENT_IS_INCREASE_KEY(keyevent) (keyevent->keyval == HILDON_INCREASE_KEY)
#define HILDON_KEYEVENT_IS_DECREASE_KEY(keyevent) (keyevent->keyval == HILDON_DECREASE_KEY)

#define TRANSIENCY_MAXITER 50

GType hildon_app_get_type(void) G_GNUC_CONST;
GtkWidget *hildon_app_new(void);
GtkWidget *hildon_app_new_with_appview(HildonAppView * appview);
void hildon_app_set_appview(HildonApp * self, HildonAppView * appview);
HildonAppView *hildon_app_get_appview(HildonApp * self);
void hildon_app_set_title(HildonApp * self, const gchar * newtitle);
const gchar *hildon_app_get_title(HildonApp * self);

#ifndef HILDON_DISABLE_DEPRECATED
void hildon_app_set_zoom(HildonApp * self, HildonZoomLevel newzoom);
HildonZoomLevel hildon_app_get_zoom(HildonApp * self);
PangoFontDescription *hildon_app_get_default_font(HildonApp * self);
PangoFontDescription *hildon_app_get_zoom_font(HildonApp * self);
#endif

void hildon_app_set_two_part_title(HildonApp * self,
                                   gboolean istwoparttitle);
gboolean hildon_app_get_two_part_title(HildonApp * self);

void hildon_app_set_autoregistration(HildonApp *self, gboolean auto_reg);
void hildon_app_register_view(HildonApp *self, gpointer view_ptr);
gboolean hildon_app_register_view_with_id(HildonApp *self,
					  gpointer view_ptr,
					  unsigned long view_id);
void hildon_app_unregister_view(HildonApp *self, gpointer view_ptr);
void hildon_app_unregister_view_with_id(HildonApp *self,
					unsigned long view_id);
unsigned long hildon_app_find_view_id(HildonApp *self, gpointer view_ptr);
void hildon_app_notify_view_changed(HildonApp *self, gpointer view_ptr);

void hildon_app_set_killable(HildonApp *self, gboolean killability);

void hildon_app_set_ui_manager(HildonApp *self, GtkUIManager *uim);
GtkUIManager *hildon_app_get_ui_manager(HildonApp *self);

G_END_DECLS
#endif /* HILDON_APP_H */
