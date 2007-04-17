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

#ifndef __HILDON_BREAD_CRUMB_H__
#define __HILDON_BREAD_CRUMB_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _HildonBreadCrumb        HildonBreadCrumb;
typedef struct _HildonBreadCrumbClass   HildonBreadCrumbClass;
typedef struct _HildonBreadCrumbPrivate HildonBreadCrumbPrivate;

#define HILDON_TYPE_BREAD_CRUMB                 (hildon_bread_crumb_get_type ())
#define HILDON_BREAD_CRUMB(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumb))
#define HILDON_BREAD_CRUMB_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumbClass))
#define HILDON_IS_BREAD_CRUMB(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_BREAD_CRUMB))
#define HILDON_IS_BREAD_CRUMB_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_BREAD_CRUMB))
#define HILDON_BREAD_CRUMB_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumbClass))

struct _HildonBreadCrumb
{
  GtkButton parent;

  HildonBreadCrumbPrivate *priv;
};

struct _HildonBreadCrumbClass
{
  GtkButtonClass parent_class;

  /* vfuncs */
  void (* get_natural_size) (HildonBreadCrumb *bc,
                             gint *natural_width, gint *natural_height);
};

GType hildon_bread_crumb_get_type (void) G_GNUC_CONST;
GtkWidget *hildon_bread_crumb_new (const gchar* text);
void hildon_bread_crumb_set_text (HildonBreadCrumb *item, const gchar *text);
const gchar* hildon_bread_crumb_get_text (HildonBreadCrumb *item);
void hildon_bread_crumb_get_natural_size (HildonBreadCrumb *item,
                                          gint *width, gint *height);
void hildon_bread_crumb_set_show_separator (HildonBreadCrumb *item,
                                            gboolean show_separator);

G_END_DECLS

#endif
