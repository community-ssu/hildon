/*
 * This file is part of hildon-libs
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
 * SECTION:hildon-scroll-area
 * @short_description: A helper to create Maemo specific views,
 * which are using scrollable area
 *
 * #GtkScrollArea combines a large widget that needs scrolling (like a
 * text editor or a tree view) and other widgets that wouldn't fit one
 * the screen normally without scrolling (like entries, toolbars etc.)
 * into one scrollable area.
 */

#include "hildon-scroll-area.h"
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkfixed.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>
#include <string.h>

typedef struct
  {
    GtkWidget *fixed;

    /* Scrolled windows */
    GtkWidget *swouter;
    GtkWidget *swinner;

    /* Widget that's being contained */
    GtkWidget *child;

    /* Vertical adjustment for scrolled windows */
    GtkAdjustment *outadj;
    GtkAdjustment *inadj;

  } HildonScrollArea;


static void hildon_scroll_area_outer_value_changed (GtkAdjustment *adjustment,
						    HildonScrollArea *sc);
static void hildon_scroll_area_inner_value_changed (GtkAdjustment *adjustment,
						    HildonScrollArea *sc);
static void hildon_scroll_area_size_allocate (GtkWidget *widget,
					      GtkAllocation *allocation,
					      HildonScrollArea *sc);
static void hildon_scroll_area_child_requisition (GtkWidget *widget,
						  GtkRequisition *req,
						  HildonScrollArea *sc);
static void hildon_scroll_area_fixed_allocate (GtkWidget *widget,
					       GtkAllocation *allocation,
					       HildonScrollArea *sc);

static int calculate_size (GtkWidget *widget);

/**
 * hildon_scroll_area_new:
 * @sw: #GtkWidget - #GtkScrolledWindow
 * @child: #GtkWidget - child to be place inside the sw
 *
 * This is not a widget. It's a helper function to create
 * hildon-specific scrolling methods.
 * A common situation where the scroll area should be used
 * might be following.  A view containing @GtkTreeView based widget,
 * (or any similar widget which has built-in @GtkScrolledWindow support)
 * and eg. couple buttons.  Normaly @GtkScrolledWindow can not handle
 * the situation so that the @GtkTreeView built-in support
 * would work.  The scroll area is connecting this built-in system to
 * the scrolled window and also noticing the buttons.  To use, one should
 * create a box to which pack the buttons and the scroll area.
 * The scroll area then contains the problematic widget eg. the @GtkTreeView.
 * Then the box should be placed in the @GtkScrolledWindow.
 * The function is currently assuming that the newly created scroll area
 * hierarchy is not modified in anyway.  Or if it is, it may lead to
 * unwanted problems.  Also assumed, that the @child will be packed
 * to the @sw.
 *
 * Returns: a @GtkFixed
 */
GtkWidget *hildon_scroll_area_new (GtkWidget *sw, GtkWidget *child)
{
  GtkWidget *swi;
  GtkWidget *fixed;
  HildonScrollArea *sc;

  g_return_val_if_fail (GTK_IS_SCROLLED_WINDOW (sw)
			&& GTK_IS_WIDGET (child), NULL);

  swi = gtk_scrolled_window_new (NULL, NULL);
  fixed = gtk_fixed_new ();
  sc = g_malloc (sizeof (HildonScrollArea));
  memset (sc, 0, sizeof (HildonScrollArea));

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swi),
				  GTK_POLICY_NEVER, GTK_POLICY_NEVER);

  gtk_container_add (GTK_CONTAINER (swi), child);
  gtk_fixed_put (GTK_FIXED (fixed), swi, 0, 0);

  sc->fixed = fixed;
  sc->swouter = sw;
  sc->swinner = swi;
  sc->child = child;
  sc->outadj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (sw));
  sc->inadj = gtk_scrolled_window_get_vadjustment (GTK_SCROLLED_WINDOW (swi));

  g_signal_connect_after (G_OBJECT (child), "size-request",
			  G_CALLBACK (hildon_scroll_area_child_requisition), sc);

  g_signal_connect_after (G_OBJECT (sc->outadj), "value_changed",
			  G_CALLBACK (hildon_scroll_area_outer_value_changed), sc);
  g_signal_connect_after (G_OBJECT (sc->inadj), "value_changed",
			  G_CALLBACK (hildon_scroll_area_inner_value_changed), sc);

  g_signal_connect_after (G_OBJECT (sw), "size-allocate",
			  G_CALLBACK (hildon_scroll_area_size_allocate), sc);
  g_signal_connect (G_OBJECT (sc->fixed), "size-allocate",
		    G_CALLBACK (hildon_scroll_area_fixed_allocate), sc);
  g_signal_connect_swapped (G_OBJECT (sw), "destroy",
		    G_CALLBACK (g_free), sc);

  gtk_widget_show_all (sw);
  return fixed;
}

static void hildon_scroll_area_fixed_allocate (GtkWidget *widget,
					       GtkAllocation *allocation,
					       HildonScrollArea *sc)
{
  gtk_widget_set_size_request (sc->swinner, -1,
			       MIN (sc->outadj->page_size, allocation->height));
}


static int calculate_size (GtkWidget *widget)
{
  int size = 0;

  if (GTK_IS_TEXT_VIEW (widget))
    return 0;

  if (GTK_IS_CONTAINER (widget)) {
    GList *children = gtk_container_get_children (GTK_CONTAINER (widget));
    while (children != NULL) {
      GtkWidget *wid = GTK_WIDGET (children->data);
      gint sz = calculate_size (wid);
      if ((GTK_WIDGET_VISIBLE (wid))) {
        size += sz;
      }

      children = g_list_next (children);
    }
  } else { 
    size = widget->allocation.height;
  }

  return size;
}

static void hildon_scroll_area_child_requisition (GtkWidget *widget,
						  GtkRequisition *req,
						  HildonScrollArea *sc)
{
  /* Limit height to fixed height */
  gint new_req = MAX (req->height, sc->fixed->allocation.height);
  new_req = MIN (sc->outadj->page_size - adjust_factor, new_req);
  gint adjust_factor = calculate_size (sc->swouter) * 0.7;
  adjust_factor = MAX (0, adjust_factor - sc->outadj->value);

  gtk_widget_set_size_request (sc->fixed, -1, req->height);
  /* Request inner scrolled window at most page size */
  gtk_widget_set_size_request (sc->swinner, -1, new_req);
}

static void hildon_scroll_area_outer_value_changed (GtkAdjustment *adjustment,
                                                    HildonScrollArea *sc)
{
  GtkRequisition req;
  gtk_widget_size_request (sc->child, &req);

  /* Update inner adjustment position based on outer one, update fixed position */
  if ((sc->outadj->value + sc->outadj->page_size) > sc->fixed->allocation.y
      && sc->outadj->value < (sc->fixed->allocation.y + req.height))
    {
      gdouble new_pos = 0;

      new_pos = MAX (sc->outadj->value - sc->fixed->allocation.y, 0);
      new_pos = MIN (new_pos, req.height - sc->inadj->page_size);
      new_pos = MAX (new_pos, 0);

      gtk_fixed_move (GTK_FIXED (sc->fixed), sc->swinner, 0, new_pos);
      gtk_adjustment_set_value (sc->inadj, new_pos);
    }
}

static void hildon_scroll_area_inner_value_changed (GtkAdjustment *adjustment,
						    HildonScrollArea *sc)
{
  /* Update outer adjustment based on inner adjustment position */
  if (sc->outadj->value != sc->fixed->allocation.y + adjustment->value)
    gtk_adjustment_set_value (sc->outadj,
			      sc->fixed->allocation.y + adjustment->value);
}

__inline__ static gint calculate_width (HildonScrollArea *sc)
{
  GtkScrolledWindow *scwin = GTK_SCROLLED_WINDOW (sc->swouter);
  return (scwin->hscrollbar_visible * scwin->hscrollbar->allocation.width);
}

static void hildon_scroll_area_size_allocate (GtkWidget *widget,
					      GtkAllocation *allocation,
					      HildonScrollArea *sc)
{
  gtk_widget_set_size_request (sc->fixed, calculate_width (sc), sc->fixed->allocation.height);
  gtk_widget_set_size_request (sc->child, sc->fixed->allocation.width, -1);
}
