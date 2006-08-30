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

#ifndef __HILDON_BANNER_H__
#define __HILDON_BANNER_H__

#include <gtk/gtkwindow.h>
#include <gtk/gtkprogressbar.h>
#include <gtk/gtklabel.h>

G_BEGIN_DECLS 

#define HILDON_TYPE_BANNER (hildon_banner_get_type())

#define HILDON_BANNER(obj)    (G_TYPE_CHECK_INSTANCE_CAST((obj), HILDON_TYPE_BANNER, HildonBanner))
#define HILDON_IS_BANNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), HILDON_TYPE_BANNER))

typedef struct _HildonBanner        HildonBanner;
typedef struct _HildonBannerClass   HildonBannerClass;
typedef struct _HildonBannerPrivate HildonBannerPrivate;

struct _HildonBanner
{
   GtkWindow parent;
   HildonBannerPrivate *priv;
};

struct _HildonBannerClass
{
   GtkWindowClass parent_class;
};

GType      hildon_banner_get_type          (void) G_GNUC_CONST;

void       hildon_banner_show_information  (GtkWidget      *widget, 
                                            const gchar    *icon_name,
                                            const gchar    *text);

void       hildon_banner_show_information_with_markup (GtkWidget   *widget, 
                                                       const gchar *icon_name,
                                                       const gchar *markup);

GtkWidget *hildon_banner_show_animation    (GtkWidget      *widget, 
                                            const gchar    *animation_name,
                                            const gchar    *text);

GtkWidget *hildon_banner_show_progress     (GtkWidget      *widget, 
                                            GtkProgressBar *bar,
                                            const gchar    *text);

void       hildon_banner_set_text          (HildonBanner   *self,
                                            const gchar    *text);

void       hildon_banner_set_markup        (HildonBanner   *self,
                                            const gchar    *markup);

void       hildon_banner_set_fraction      (HildonBanner   *self,
                                            gdouble         fraction);



/* For internal use of hildon libraries only */
void _hildon_gtk_label_set_text_n_lines(GtkLabel *label, const gchar *text, gint max_lines);

G_END_DECLS

#endif /* __HILDON_BANNER_H__ */
