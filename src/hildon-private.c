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

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-private.h"
#include                                        <gtk/gtkwidget.h>
#include                                        <gtk/gtkwindow.h>
#include                                        "hildon-date-editor.h"
#include                                        "hildon-time-editor.h"

/* This function is a private function of hildon. It hadles focus 
 * changing for composite hildon widgets: HildonDateEditor, 
 * HildonNumberEditor, HildonTimeEditor, HildonWeekdayPicker. 
 * Its purpose is to focus the first widget (from left) inside the container 
 * regardless of where the focus is coming from.
 */
gboolean G_GNUC_INTERNAL
hildon_private_composite_focus                  (GtkWidget *widget, 
                                                 GtkDirectionType direction)
{
  GtkWidget *toplevel = NULL;
  GtkWidget *focus_widget = NULL;
  gboolean coming_from_outside = FALSE;
  GtkDirectionType effective_direction;

  toplevel = gtk_widget_get_toplevel (widget);

  focus_widget = GTK_WINDOW (toplevel)->focus_widget;

  if (focus_widget == NULL || gtk_widget_is_ancestor (focus_widget, widget) == FALSE)
    {
      /* When coming from outside we want to give focus to the first
         item in the widgets */
      effective_direction = GTK_DIR_TAB_FORWARD;
      coming_from_outside = TRUE;
    }
  else
    effective_direction = direction;

  switch (direction) {
      case GTK_DIR_UP:
      case GTK_DIR_DOWN:
      case GTK_DIR_TAB_FORWARD:
      case GTK_DIR_TAB_BACKWARD:
        if ((HILDON_IS_DATE_EDITOR (widget) || HILDON_IS_TIME_EDITOR(widget)) &&
            !coming_from_outside)
          return FALSE;
        /* fall through */
      default:
        return GTK_WIDGET_CLASS (g_type_class_peek_parent
                                 (GTK_WIDGET_GET_CLASS(widget)))->focus (widget, effective_direction);
  }

  g_assert_not_reached ();
  return TRUE;
}


