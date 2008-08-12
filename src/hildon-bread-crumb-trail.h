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

#ifndef __HILDON_BREAD_CRUMB_TRAIL_H__
#define __HILDON_BREAD_CRUMB_TRAIL_H__

#include <gtk/gtk.h>

#include "hildon-bread-crumb.h"

G_BEGIN_DECLS

typedef struct _HildonBreadCrumbTrail        HildonBreadCrumbTrail;
typedef struct _HildonBreadCrumbTrailClass   HildonBreadCrumbTrailClass;
typedef struct _HildonBreadCrumbTrailPrivate HildonBreadCrumbTrailPrivate;

#define HILDON_TYPE_BREAD_CRUMB_TRAIL                 (hildon_bread_crumb_trail_get_type ())
#define HILDON_BREAD_CRUMB_TRAIL(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_BREAD_CRUMB_TRAIL, HildonBreadCrumbTrail))
#define HILDON_BREAD_CRUMB_TRAIL_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), HILDON_TYPE_BREAD_CRUMB_TRAIL, HildonBreadCrumbTrailClass))
#define HILDON_IS_BREAD_CRUMB_TRAIL(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_BREAD_CRUMB_TRAIL))
#define HILDON_IS_BREAD_CRUMB_TRAIL_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_BREAD_CRUMB_TRAIL))
#define HILDON_BREAD_CRUMB_TRAIL_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), HILDON_TYPE_BREAD_CRUMB_TRAIL, HildonBreadCrumbTrailClass))

struct _HildonBreadCrumbTrail
{
  GtkContainer parent;

  HildonBreadCrumbTrailPrivate *priv;
};

struct _HildonBreadCrumbTrailClass
{
  GtkContainerClass parent_class;

  void (*crumb_clicked) (HildonBreadCrumbTrail *bct,
                         gpointer id);

  /* Signals used for keybindings */
  void (*move_parent) (HildonBreadCrumbTrail *bct);

  /* Padding for future expansion */
  void (*_hildon_bct_reserved1) (void);
  void (*_hildon_bct_reserved2) (void);
  void (*_hildon_bct_reserved3) (void);
  void (*_hildon_bct_reserved4) (void);
};

GType hildon_bread_crumb_trail_get_type (void) G_GNUC_CONST;
GtkWidget *hildon_bread_crumb_trail_new (void);
void hildon_bread_crumb_trail_push (HildonBreadCrumbTrail *bct, HildonBreadCrumb *item,
                                    gpointer id, GDestroyNotify destroy);
void hildon_bread_crumb_trail_push_text (HildonBreadCrumbTrail *bct, const gchar *text,
                                         gpointer id, GDestroyNotify destroy);
void hildon_bread_crumb_trail_push_icon (HildonBreadCrumbTrail *bct,
                                         const gchar *text,
                                         GtkWidget *icon,
                                         gpointer id,
                                         GDestroyNotify destroy);
void hildon_bread_crumb_trail_pop (HildonBreadCrumbTrail *bct);
void hildon_bread_crumb_trail_clear (HildonBreadCrumbTrail *bct);

G_END_DECLS

#endif

