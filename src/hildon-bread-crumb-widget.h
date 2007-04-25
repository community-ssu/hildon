/*
 * This file is a part of hildon
 *
 * Copyright (C) 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 * Author: Xan Lopez <xan.lopez@nokia.com>
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

#ifndef __HILDON_BREAD_CRUMB_WIDGET_H__
#define __HILDON_BREAD_CRUMB_WIDGET_H__

#include "hildon-bread-crumb.h"

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _HildonBreadCrumbWidget        HildonBreadCrumbWidget;
typedef struct _HildonBreadCrumbWidgetClass   HildonBreadCrumbWidgetClass;
typedef struct _HildonBreadCrumbWidgetPrivate HildonBreadCrumbWidgetPrivate;

#define HILDON_TYPE_BREAD_CRUMB_WIDGET                 (hildon_bread_crumb_widget_get_type ())
#define HILDON_BREAD_CRUMB_WIDGET(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_BREAD_CRUMB_WIDGET, HildonBreadCrumbWidget))
#define HILDON_BREAD_CRUMB_WIDGET_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), HILDON_TYPE_BREAD_CRUMB_WIDGET, HildonBreadCrumbWidgetClass))
#define HILDON_IS_BREAD_CRUMB_WIDGET(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_BREAD_CRUMB_WIDGET))
#define HILDON_IS_BREAD_CRUMB_WIDGET_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_BREAD_CRUMB_WIDGET))
#define HILDON_BREAD_CRUMB_WIDGET_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), HILDON_TYPE_BREAD_CRUMB_WIDGET, HildonBreadCrumbWidgetClass))

struct _HildonBreadCrumbWidget
{
  GtkButton parent;

  GtkWidget *contents;

  HildonBreadCrumbWidgetPrivate *priv;
};

struct _HildonBreadCrumbWidgetClass
{
  GtkButtonClass parent_class;
};

GType hildon_bread_crumb_widget_get_type (void) G_GNUC_CONST;
GtkWidget *_hildon_bread_crumb_widget_new (void);
GtkWidget *_hildon_bread_crumb_widget_new_with_text (const gchar *label);
GtkWidget *_hildon_bread_crumb_widget_new_with_icon (GtkWidget *icon, const gchar *text);
void _hildon_bread_crumb_widget_set_text (HildonBreadCrumbWidget *item, const gchar *text);
const gchar *_hildon_bread_crumb_widget_get_text (HildonBreadCrumbWidget *item);
void _hildon_bread_crumb_widget_set_show_separator (HildonBreadCrumbWidget *item,
                                                    gboolean show_separator);
void _hildon_bread_crumb_widget_set_icon (HildonBreadCrumbWidget *bread_crumb_widget,
                                          GtkWidget *icon);
void _hildon_bread_crumb_widget_set_icon_position (HildonBreadCrumbWidget *bread_crumb,
                                                   GtkPositionType icon_position);

G_END_DECLS

#endif
