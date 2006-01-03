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
#include <gtk/gtkwidget.h>
#include <gtk/gtkwindow.h>
#include "hildon-composite-widget.h"

gboolean
hildon_composite_widget_focus (GtkWidget *widget, GtkDirectionType direction)
{
  GtkWidget *toplevel = NULL;
  GtkWidget *focus_widget = NULL;
  
  /* Get the topmost parent widget */  
  toplevel = gtk_widget_get_toplevel (widget);
  if (!GTK_IS_WINDOW(toplevel))
    return GTK_WIDGET_CLASS (g_type_class_peek_parent (
                     GTK_WIDGET_GET_CLASS (widget)))->focus (widget, direction);
  /* Get focus widget in the topmost parent widget */
  focus_widget = GTK_WINDOW (toplevel)->focus_widget;

  if (!GTK_IS_WIDGET (focus_widget))
    return TRUE;

  if (!gtk_widget_is_ancestor (focus_widget, widget))
    /* Containers grab_focus grabs the focus to the correct widget */
    gtk_widget_grab_focus (widget);
  else
    return GTK_WIDGET_CLASS (g_type_class_peek_parent (
                     GTK_WIDGET_GET_CLASS(widget)))->focus (widget, direction);

  return TRUE;
}
