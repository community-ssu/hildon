/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
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


#ifndef __HILDON_PLUGIN_WIDGET_H__
#define __HILDON_PLUGIN_WIDGET_H__

#include <glib-object.h>
#include <gtk/gtkwidget.h>

/**
 * HildonPluginWidgetInfo:
 *
 * Contains information about the loaded module which has some widget
 * inherited from some non-plugin widget. The struct should be considered
 * private and should not be used directly.
 */
typedef struct HildonPluginWidgetInfo_ HildonPluginWidgetInfo;


HildonPluginWidgetInfo *hildon_plugin_info_initialize(GType base_type, const gchar *name);


GtkWidget *hildon_plugin_info_construct_widget(HildonPluginWidgetInfo *info);


void hildon_plugin_info_kill(HildonPluginWidgetInfo *info);



#endif /* __HILDON_PLUGIN_WIDGET_H__ */
