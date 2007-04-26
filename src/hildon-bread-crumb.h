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

#ifdef HILDON_ENABLE_UNSTABLE_API

#ifndef __HILDON_BREAD_CRUMB_H__
#define __HILDON_BREAD_CRUMB_H__

#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

typedef struct _HildonBreadCrumb        HildonBreadCrumb;
typedef struct _HildonBreadCrumbIface   HildonBreadCrumbIface;

#define HILDON_TYPE_BREAD_CRUMB           (hildon_bread_crumb_get_type ())
#define HILDON_BREAD_CRUMB(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumb))
#define HILDON_BREAD_CRUMB_CLASS(obj)     (G_TYPE_CHECK_CLASS_CAST ((obj), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumbIface))
#define HILDON_IS_BREAD_CRUMB(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_BREAD_CRUMB))
#define HILDON_BREAD_CRUMB_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumbIface))

struct _HildonBreadCrumbIface
{
  GTypeInterface g_iface;

  /* virtual table */
  void (* get_natural_size) (HildonBreadCrumb *bread_crumb,
                             gint *natural_width, gint *natural_height);
  /* signals */
  void (* crumb_activated) (HildonBreadCrumb *bread_crumb);
};

GType hildon_bread_crumb_get_type (void) G_GNUC_CONST;
void hildon_bread_crumb_get_natural_size (HildonBreadCrumb *bread_crumb,
                                          gint *width, gint *height);
void hildon_bread_crumb_activated (HildonBreadCrumb *bread_crumb);

G_END_DECLS

#endif

#endif /* HILDON_ENABLE_UNSTABLE_API */
