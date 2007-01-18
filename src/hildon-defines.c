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

/**
 * SECTION:hildon-defines
 * @short_description: A collection of usefull defines. 
 *
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        <gtk/gtk.h>
#include                                        "hildon-defines.h"

const HildonIconSizes*                          hildoniconsizes = NULL;

static HildonIconSizes                          iis; /* hildon internal icon sizes */

void 
hildon_icon_sizes_init                          (void)
{
    if (hildoniconsizes != NULL)
        return;

    hildoniconsizes = &iis;

    iis.icon_size_list                       = gtk_icon_size_register ("hildon_icon_size_list",                64, 64);
    iis.icon_size_small                      = gtk_icon_size_register ("*icon_size_small",                     26, 26);
    iis.icon_size_toolbar                    = gtk_icon_size_register ("icon_size_toolbar",                    26, 26);
    iis.icon_size_widg                       = gtk_icon_size_register ("icon_size_widg",                       26, 26);
    iis.icon_size_widg_wizard                = gtk_icon_size_register ("icon_size_widg_wizard",                50, 50);
    iis.icon_size_grid                       = gtk_icon_size_register ("icon_size_grid",                       64, 64);
    iis.icon_size_big_note                   = gtk_icon_size_register ("icon_size_big_note",                   50, 50);
    iis.icon_size_note                       = gtk_icon_size_register ("icon_size_note",                       26, 26);
    iis.icon_size_statusbar                  = gtk_icon_size_register ("icon_size_statusbar",                  40, 40);
    iis.icon_size_indi_video_player_pre_roll = gtk_icon_size_register ("icon_size_indi_video_player_pre_roll", 64, 64);
    iis.icon_size_indi_key_pad_lock          = gtk_icon_size_register ("icon_size_indi_key_pad_lock",          50, 50);
    iis.icon_size_indi_copy                  = gtk_icon_size_register ("icon_size_indi_copy",                  64, 64);
    iis.icon_size_indi_delete                = gtk_icon_size_register ("icon_size_indi_delete",                64, 64);
    iis.icon_size_indi_process               = gtk_icon_size_register ("icon_size_indi_process",               64, 64);
    iis.icon_size_indi_progressball          = gtk_icon_size_register ("icon_size_indi_progressball",          64, 64);
    iis.icon_size_indi_send                  = gtk_icon_size_register ("icon_size_indi_send",                  64, 64);
    iis.icon_size_indi_offmode_charging      = gtk_icon_size_register ("icon_size_indi_offmode_charging",      50, 50);
    iis.icon_size_indi_tap_and_hold          = gtk_icon_size_register ("icon_size_indi_tap_and_hold",          34, 34);
    iis.icon_size_indi_send_receive          = gtk_icon_size_register ("icon_size_indi_send_receive",          64, 64);
    iis.icon_size_indi_wlan_strength         = gtk_icon_size_register ("icon_size_indi_wlan_strength",         64, 64);

    iis.image_size_indi_nokia_logo           = gtk_icon_size_register ("image_size_indi_nokia_logo",           64, 64);
    iis.image_size_indi_startup_failed       = gtk_icon_size_register ("image_size_indi_startup_failed",       64, 64);
    iis.image_size_indi_startup_nokia_logo   = gtk_icon_size_register ("image_size_indi_startup_nokia_logo",   64, 64);
    iis.image_size_indi_nokia_hands          = gtk_icon_size_register ("image_size_indi_nokia_hands",          64, 64);
}

