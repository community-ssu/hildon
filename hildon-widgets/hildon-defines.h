/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
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

#ifndef __HILDON_DEFINES_H__
#define __HILDON_DEFINES_H__

#include <gtk/gtkwindow.h>
#include <gdk/gdkkeysyms.h>
#include "hildon-appview.h"

G_BEGIN_DECLS

void hildon_icon_sizes_init (void);

typedef struct _HildonIconSizes HildonIconSizes;

struct _HildonIconSizes
{
  GtkIconSize icon_size_list;
  GtkIconSize icon_size_small;
  GtkIconSize icon_size_toolbar;
  GtkIconSize icon_size_widg;
  GtkIconSize icon_size_widg_wizard;
  GtkIconSize icon_size_grid;
  GtkIconSize icon_size_big_note;
  GtkIconSize icon_size_note;
  GtkIconSize icon_size_statusbar;
  GtkIconSize icon_size_indi_video_player_pre_roll;
  GtkIconSize icon_size_indi_key_pad_lock;
  GtkIconSize icon_size_indi_copy;
  GtkIconSize icon_size_indi_delete;
  GtkIconSize icon_size_indi_process;
  GtkIconSize icon_size_indi_progressball;
  GtkIconSize icon_size_indi_send;
  GtkIconSize icon_size_indi_offmode_charging;
  GtkIconSize icon_size_indi_tap_and_hold;
  GtkIconSize icon_size_indi_send_receive;
  GtkIconSize icon_size_indi_wlan_strength;
  GtkIconSize image_size_indi_nokia_logo;
  GtkIconSize image_size_indi_startup_failed;
  GtkIconSize image_size_indi_startup_nokia_logo;
  GtkIconSize image_size_indi_nokia_hands;
};

extern const HildonIconSizes *hildoniconsizes;

#define HILDON_ICON_SIZE_CHECK_AND_GET(iconvar) (!hildoniconsizes ? hildon_icon_sizes_init (), hildoniconsizes->iconvar : hildoniconsizes->iconvar)

#define HILDON_ICON_SIZE_LIST HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_list)
#define HILDON_ICON_SIZE_SMALL HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_small)
#define HILDON_ICON_SIZE_TOOLBAR HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_toolbar)
#define HILDON_ICON_SIZE_WIDG HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_widg)
#define HILDON_ICON_SIZE_WIDG_WIZARD HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_widg_wizard)
#define HILDON_ICON_SIZE_GRID HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_grid)
#define HILDON_ICON_SIZE_BIG_NOTE HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_big_note)
#define HILDON_ICON_SIZE_NOTE HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_note)
#define HILDON_ICON_SIZE_STATUSBAR HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_statusbar)
#define HILDON_ICON_SIZE_INDI_VIDEO_PLAYER_PRE_ROLL HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_video_player_pre_roll)
#define HILDON_ICON_SIZE_INDI_COPY HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_copy)
#define HILDON_ICON_SIZE_INDI_DELETE HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_delete)
#define HILDON_ICON_SIZE_INDI_PROCESS HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_process)
#define HILDON_ICON_SIZE_INDI_PROGRESSBALL HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_progressball)
#define HILDON_ICON_SIZE_INDI_SEND HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_send)
#define HILDON_ICON_SIZE_INDI_OFFMODE_CHARGING HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_offmode)
#define HILDON_ICON_SIZE_INDI_TAP_AND_HOLD HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_tap_and_hold)
#define HILDON_ICON_SIZE_INDI_SEND_RECEIVE HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_send_receive)
#define HILDON_ICON_SIZE_INDI_WLAN_STRENGTH HILDON_ICON_SIZE_CHECK_AND_GET(icon_size_indi_wlan_strength)

#define HILDON_IMAGE_SIZE_INDI_NOKIA_LOGO HILDON_ICON_SIZE_CHECK_AND_GET(image_size_indi_nokia_logo)
#define HILDON_IMAGE_SIZE_INDI_STARTUP_FAILED HILDON_ICON_SIZE_CHECK_AND_GET(image_size_indi_startup_nokia_failed)
#define HILDON_IMAGE_SIZE_INDI_STARTUP_NOKIA_LOGO HILDON_ICON_SIZE_CHECK_AND_GET(image_size_indi_startup_nokia_logo)
#define HILDON_IMAGE_SIZE_INDI_NOKIA_HAND HILDON_ICON_SIZE_CHECK_AND_GET(image_size_indi_nokia_hands)

#define HILDON_ICON_PIXEL_SIZE_LIST 64
#define HILDON_ICON_PIXEL_SIZE_SMALL 26
#define HILDON_ICON_PIXEL_SIZE_TOOLBAR 26
#define HILDON_ICON_PIXEL_SIZE_WIDG 26
#define HILDON_ICON_PIXEL_SIZE_WIDG_WIZARD 50
#define HILDON_ICON_PIXEL_SIZE_GRID 64
#define HILDON_ICON_PIXEL_SIZE_BIG_NOTE 50
#define HILDON_ICON_PIXEL_SIZE_NOTE 26
#define HILDON_ICON_PIXEL_SIZE_STATUSBAR 40
#define HILDON_ICON_PIXEL_SIZE_INDI_VIDEO_PLAYER_PRE_ROLL 64
#define HILDON_ICON_PIXEL_SIZE_INDI_KEY_PAD_LOCK 50
#define HILDON_ICON_PIXEL_SIZE_INDI_COPY 64
#define HILDON_ICON_PIXEL_SIZE_INDI_DELETE 64
#define HILDON_ICON_PIXEL_SIZE_INDI_PROCESS 64
#define HILDON_ICON_PIXEL_SIZE_INDI_PROGRESSBALL 64
#define HILDON_ICON_PIXEL_SIZE_INDI_SEND 64
#define HILDON_ICON_PIXEL_SIZE_INDI_OFFMODE_CHARGING 50
#define HILDON_ICON_PIXEL_SIZE_INDI_TAP_AND_HOLD 34
#define HILDON_ICON_PIXEL_SIZE_INDI_SEND_RECEIVE 64
#define HILDON_ICON_PIXEL_SIZE_INDI_WLAN_STRENGTH 64

#define HILDON_IMAGE_PIXEL_SIZE_INDI_NOKIA_LOGO 64
#define HILDON_IMAGE_PIXEL_SIZE_INDI_STARTUP_FAILED 64
#define HILDON_IMAGE_PIXEL_SIZE_INDI_STARTUP_NOKIA_LOGO 64
#define HILDON_IMAGE_PIXEL_SIZE_INDI_NOKIA_HANDS 64

#define HILDON_MARGIN_HALF 3
#define HILDON_MARGIN_DEFAULT 6
#define HILDON_MARGIN_DOUBLE 12
#define HILDON_MARGIN_TRIPLE 18

#define HILDON_HARDKEY_UP         GDK_Up
#define HILDON_HARDKEY_LEFT       GDK_Left
#define HILDON_HARDKEY_RIGHT      GDK_Right
#define HILDON_HARDKEY_DOWN       GDK_Down
#define HILDON_HARDKEY_SELECT     GDK_Return
#define HILDON_HARDKEY_HOME       GDK_F5
#define HILDON_HARDKEY_ESC        GDK_Escape
#define HILDON_HARDKEY_FULLSCREEN GDK_F6
#define HILDON_HARDKEY_INCREASE   GDK_F7
#define HILDON_HARDKEY_DECREASE   GDK_F8
#define HILDON_HARDKEY_MENU       GDK_F10

gulong hildon_gtk_widget_set_logical_font (GtkWidget *widget, const gchar *logicalfontname);
gulong hildon_gtk_widget_set_logical_color (GtkWidget *widget, GtkRcFlags rcflags,
				    GtkStateType state, const gchar *logicalcolorname);

G_END_DECLS
#endif /* HILDON_DEFINES_H */
