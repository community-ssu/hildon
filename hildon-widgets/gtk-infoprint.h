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

#ifndef __GTK_INFOPRINT_H__
#define __GTK_INFOPRINT_H__

#include <gtk/gtkwindow.h>

G_BEGIN_DECLS void gtk_infoprint(GtkWindow * parent, const gchar * text);
void gtk_infoprint_with_icon_stock(GtkWindow * parent, const gchar * text,
                                   const gchar * stock_id);
void gtk_infoprint_with_icon_name(GtkWindow * parent, const gchar * text,
                                   const gchar * icon_name);

void gtk_infoprintf(GtkWindow * parent, const gchar * format, ...);
void gtk_infoprint_temporarily_disable_wrap(void);

void gtk_confirmation_banner_with_icon_name(GtkWindow * parent, const gchar * text,
                             const gchar * icon_name);
void gtk_confirmation_banner(GtkWindow * parent, const gchar * text,
                             const gchar * stock_id);

void gtk_banner_show_animation(GtkWindow * parent, const gchar * text);
void gtk_banner_show_bar(GtkWindow * parent, const gchar * text);
void gtk_banner_set_text(GtkWindow * parent, const gchar * text);
void gtk_banner_set_fraction(GtkWindow * parent, gdouble fraction);
void gtk_banner_close(GtkWindow * parent);
void gtk_banner_temporarily_disable_wrap(void);

G_END_DECLS
#endif /* __GTK_INFOPRINT_H__ */
