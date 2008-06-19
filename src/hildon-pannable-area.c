/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
 *
 * This widget is based on MokoFingerScroll from libmokoui
 * OpenMoko Application Framework UI Library
 * Authored by Chris Lord <chris@openedhand.com>
 * Copyright (C) 2006-2007 OpenMoko Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation; version 2 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 */

/**
 * SECTION: hildon-pannable-area
 * @short_description: A scrolling widget designed for touch screens
 * @see_also: #GtkScrolledWindow
 *
 * #HildonPannableArea implements a scrolled window designed to be used with a
 * touch screen interface. The user scrolls the child widget by activating the
 * pointing device and dragging it over the widget.
 *
 */

/* TODO:
 * todo items should be marked with a number and a relative position in the code where they are relevant.
 *
 * 1  x Make OVERSHOOT a property, the default of this property is fixed
 * 2  - Add method of scrolling to the selected element of a treeview and have it align to half way point
 * 3  - Add a property for initial scrolling of the widget, so it scrolls on realize
 * 4  - Add a method of removing children from the alignment, gtk-container-remove override?
 * 5  x Remove elasticity if it is no longer required?
 * 6  x Reduce calls to queue_resize to improve performance
 * 7  - Make 'fast' factor a property
 * 8  - Strip out g_print/g_debug calls
 * 9  - Scroll policies (horizontal/vertical/both) suggest 1,2,3 for different policies (binary OR'd)
 * 10 x Delay click mode (only send synthetic clicks on mouse-up, as in previous
 *      versions.
 * 11 - 'Physical' mode for acceleration scrolling
 * 12 - Add a property to disable overshoot entirely
 * 13 x Fix problem with kinetics when pannable has been in overshoot (a flick stops rather than continues) (Search TODO 13)
 * 14 - Review the other modes, probably they are not working well after overshooting patches
 * 15 x Very small scrollbars when overshooting are not correctly rendered (fixed adding a minimum size)
 */

#include <gdk/gdkx.h>
#include <cairo.h>
#include <math.h>
#include "hildon-pannable-area.h"

#define SMOOTH_FACTOR 0.85
#define FORCE 5
#define BOUNCE_STEPS 6
#define SCROLL_BAR_MIN_SIZE 5

G_DEFINE_TYPE (HildonPannableArea, hildon_pannable_area, GTK_TYPE_BIN)
#define PANNABLE_AREA_PRIVATE(o)                                \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_PANNABLE_AREA, \
                                HildonPannableAreaPrivate))
typedef struct _HildonPannableAreaPrivate HildonPannableAreaPrivate;

struct _HildonPannableAreaPrivate {
  HildonPannableAreaMode mode;
  GdkWindow *event_window;
  gdouble x;		/* Used to store mouse co-ordinates of the first or */
  gdouble y;		/* previous events in a press-motion pair */
  gdouble ex;		/* Used to store mouse co-ordinates of the last */
  gdouble ey;		/* motion event in acceleration mode */
  gboolean enabled;
  gboolean clicked;
  guint32 last_time;	/* Last event time, to stop infinite loops */
  gint last_type;
  gboolean moved;
  gdouble vmin;
  gdouble vmax;
  gdouble decel;
  guint sps;
  gdouble vel_x;
  gdouble vel_y;
  GdkWindow *child;
  gint ix;			/* Initial click mouse co-ordinates */
  gint iy;
  gint cx;			/* Initial click child window mouse co-ordinates */
  gint cy;
  guint idle_id;
  gint overshot_dist_x;
  gint overshot_dist_y;
  gint overshooting_y;
  gint overshooting_x;
  gdouble scroll_indicator_alpha;
  gint scroll_indicator_timeout;
  gint scroll_indicator_event_interrupt;
  gint scroll_delay_counter;
  gint overshoot_max;

  GtkWidget *align;
  gboolean hscroll;
  gboolean vscroll;
  GdkRectangle hscroll_rect;
  GdkRectangle vscroll_rect;
  guint area_width;

  GtkAdjustment *hadjust;
  GtkAdjustment *vadjust;

  gdouble click_x;
  gdouble click_y;

  guint event_mode;

  HildonPannableAreaIndicatorMode vindicator_mode;
  HildonPannableAreaIndicatorMode hindicator_mode;

};

enum {
  PROP_ENABLED = 1,
  PROP_MODE,
  PROP_VELOCITY_MIN,
  PROP_VELOCITY_MAX,
  PROP_DECELERATION,
  PROP_SPS,
  PROP_VINDICATOR,
  PROP_HINDICATOR,
  PROP_OVERSHOOT_MAX
};

static GdkWindow *hildon_pannable_area_get_topmost (GdkWindow * window,
                                                    gint x, gint y,
                                                    gint * tx, gint * ty)
{
  /* Find the GdkWindow at the given point, by recursing from a given
   * parent GdkWindow. Optionally return the co-ordinates transformed
   * relative to the child window.
   */
  gint width, height;

  gdk_drawable_get_size (GDK_DRAWABLE (window), &width, &height);
  if ((x < 0) || (x >= width) || (y < 0) || (y >= height))
    return NULL;

  /*g_debug ("Finding window at (%d, %d) in %p", x, y, window); */

  while (window) {
    gint child_x = 0, child_y = 0;
    GList *c, *children = gdk_window_get_children (window);
    GdkWindow *old_window = window;

    for (c = children; c; c = c->next) {
      GdkWindow *child = (GdkWindow *) c->data;
      gint wx, wy;

      gdk_window_get_geometry (child, &wx, &wy, &width, &height, NULL);
      /*g_debug ("Child: %p, (%dx%d+%d,%d)", child,
         width, height, wx, wy); */

      if ((x >= wx) && (x < (wx + width)) && (y >= wy)
	  && (y < (wy + height))) {
	child_x = x - wx;
	child_y = y - wy;
	window = child;
      }
    }

    g_list_free (children);

    /*g_debug ("\\|/"); */
    if (window == old_window)
      break;

    x = child_x;
    y = child_y;
  }

  if (tx)
    *tx = x;
  if (ty)
    *ty = y;

  /*g_debug ("Returning: %p", window); */

  return window;
}

static void
synth_crossing (GdkWindow * child,
		gint x, gint y,
		gint x_root, gint y_root, guint32 time, gboolean in)
{
  GdkEventCrossing *crossing_event;
  GdkEventType type = in ? GDK_ENTER_NOTIFY : GDK_LEAVE_NOTIFY;

  /* Send synthetic enter event */
  crossing_event = (GdkEventCrossing *) gdk_event_new (type);
  ((GdkEventAny *) crossing_event)->type = type;
  ((GdkEventAny *) crossing_event)->window = g_object_ref (child);
  ((GdkEventAny *) crossing_event)->send_event = FALSE;
  crossing_event->subwindow = g_object_ref (child);
  crossing_event->time = time;
  crossing_event->x = x;
  crossing_event->y = y;
  crossing_event->x_root = x_root;
  crossing_event->y_root = y_root;
  crossing_event->mode = GDK_CROSSING_NORMAL;
  crossing_event->detail = GDK_NOTIFY_UNKNOWN;
  crossing_event->focus = FALSE;
  crossing_event->state = 0;
  gdk_event_put ((GdkEvent *) crossing_event);
  gdk_event_free ((GdkEvent *) crossing_event);
}

static gboolean
hildon_pannable_area_scroll_indicator_fade(HildonPannableArea * area)
{
  gint retval = TRUE;
  HildonPannableAreaPrivate *priv;

  GDK_THREADS_ENTER ();

  priv = PANNABLE_AREA_PRIVATE (area);

  /* if moving do not fade out */
  if (((ABS (priv->vel_y)>1.0)||
       (ABS (priv->vel_x)>1.0))&&(!priv->clicked)) {
    return TRUE;
  }

  if (!priv->scroll_indicator_timeout) {
    return FALSE;
  }

  if (priv->scroll_indicator_event_interrupt) {
    /* Stop a fade out, and fade back in */
    if (priv->scroll_indicator_alpha >= 0.9) {
      priv->scroll_indicator_timeout = 0;
      priv->scroll_indicator_alpha = 1;
      retval = FALSE;
    } else {
      priv->scroll_indicator_alpha += 0.2;
    }
    gtk_widget_queue_draw_area (GTK_WIDGET(area),
                                priv->vscroll_rect.x,
                                priv->vscroll_rect.y,
                                priv->vscroll_rect.width,
                                priv->vscroll_rect.height);

    gtk_widget_queue_draw_area (GTK_WIDGET(area),
                                priv->hscroll_rect.x,
                                priv->hscroll_rect.y,
                                priv->hscroll_rect.width,
                                priv->hscroll_rect.height);

  }

  if ((priv->scroll_indicator_alpha > 0.9) &&
      (priv->scroll_delay_counter < 20)) {
    priv->scroll_delay_counter++;
    return TRUE;
  }

  if (!priv->scroll_indicator_event_interrupt) {
    /* Continue fade out */
    if (priv->scroll_indicator_alpha <= 0.1) {
      priv->scroll_indicator_timeout = 0;
      priv->scroll_delay_counter = 0;
      priv->scroll_indicator_alpha = 0;
      retval = FALSE;
    } else {
      priv->scroll_indicator_alpha -= 0.2;
    }
    gtk_widget_queue_draw_area (GTK_WIDGET(area),
                                priv->vscroll_rect.x,
                                priv->vscroll_rect.y,
                                priv->vscroll_rect.width,
                                priv->vscroll_rect.height);

    gtk_widget_queue_draw_area (GTK_WIDGET(area),
                                priv->hscroll_rect.x,
                                priv->hscroll_rect.y,
                                priv->hscroll_rect.width,
                                priv->hscroll_rect.height);
  }

  GDK_THREADS_LEAVE ();

  return retval;
}

static gboolean
hildon_pannable_area_button_press_cb (GtkWidget * widget,
				      GdkEventButton * event)
{
  gint x, y;
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (widget);

  if ((!priv->enabled) || (event->button != 1) ||
      ((event->time == priv->last_time) &&
       (priv->last_type == 1)))
    return TRUE;

  priv->scroll_indicator_event_interrupt = 1;
  if (priv->scroll_indicator_timeout){
    g_source_remove (priv->scroll_indicator_timeout);
  }
  priv->scroll_indicator_timeout = g_timeout_add ((gint) (1000.0 / (gdouble) (priv->sps*2)),
                     (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, widget);
  priv->last_time = event->time;
  priv->last_type = 1;

  priv->click_x = event->x;
  priv->click_y = event->y;

  if (priv->clicked && priv->child) {
    /* Widget stole focus on last click, send crossing-out event */
    synth_crossing (priv->child, 0, 0, event->x_root, event->y_root,
		    event->time, FALSE);
  }

  priv->x = event->x;
  priv->y = event->y;
  priv->ix = priv->x;
  priv->iy = priv->y;
  /* Don't allow a click if we're still moving fast, where fast is
   * defined as a quarter of our top possible speed.
   * TODO 7: Make 'fast' a property
   */
  if ((ABS (priv->vel_x) < (priv->vmax * 0.25)) &&
      (ABS (priv->vel_y) < (priv->vmax * 0.25)))
    priv->child =
      hildon_pannable_area_get_topmost (GTK_BIN (priv->align)->child->window,
					event->x, event->y, &x, &y);
  else
    priv->child = NULL;

  priv->clicked = TRUE;
  /* Stop scrolling on mouse-down (so you can flick, then hold to stop) */
  priv->vel_x = 0;
  priv->vel_y = 0;

  if ((priv->child) && (priv->child != GTK_BIN (priv->align)->child->window)) {

    g_object_add_weak_pointer ((GObject *) priv->child,
			       (gpointer *) & priv->child);

    event = (GdkEventButton *) gdk_event_copy ((GdkEvent *) event);
    event->x = x;
    event->y = y;
    priv->cx = x;
    priv->cy = y;

    synth_crossing (priv->child, x, y, event->x_root,
		    event->y_root, event->time, TRUE);

    /* Send synthetic click (button press/release) event */
    ((GdkEventAny *) event)->window = g_object_ref (priv->child);

    gdk_event_put ((GdkEvent *) event);
    gdk_event_free ((GdkEvent *) event);
  } else
    priv->child = NULL;

  return TRUE;
}

static void
hildon_pannable_area_redraw (HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (area);

  /* Redraw scroll indicators */
  if (priv->hscroll) {
    if (GTK_WIDGET (area)->window) {
      gdk_window_invalidate_rect (GTK_WIDGET (area)->window,
				  &priv->hscroll_rect, FALSE);
    }
  }
  if (priv->vscroll) {
    if (GTK_WIDGET (area)->window) {
      gdk_window_invalidate_rect (GTK_WIDGET (area)->window,
				  &priv->vscroll_rect, FALSE);
    }
  }
}

static void
hildon_pannable_area_refresh (HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (area);
  GtkWidget *widget = GTK_BIN (priv->align)->child;
  gboolean vscroll, hscroll;

  if (!widget)
    return;

  /* Calculate if we need scroll indicators */
  gtk_widget_size_request (widget, NULL);

  switch (priv->hindicator_mode) {
  case HILDON_PANNABLE_AREA_INDICATOR_MODE_SHOW:
    hscroll = TRUE;
    break;
  case HILDON_PANNABLE_AREA_INDICATOR_MODE_HIDE:
    hscroll = FALSE;
    break;
  default:
    hscroll = (priv->hadjust->upper - priv->hadjust->lower >
	       priv->hadjust->page_size) ? TRUE : FALSE;
  }

  switch (priv->vindicator_mode) {
  case HILDON_PANNABLE_AREA_INDICATOR_MODE_SHOW:
    vscroll = TRUE;
    break;
  case HILDON_PANNABLE_AREA_INDICATOR_MODE_HIDE:
    vscroll = FALSE;
    break;
  default:
    vscroll = (priv->vadjust->upper - priv->vadjust->lower >
	       priv->vadjust->page_size) ? TRUE : FALSE;
  }

  /* TODO: Read ltr settings to decide which corner gets scroll
   * indicators?
   */
  if ((priv->vscroll != vscroll) || (priv->hscroll != hscroll)) {
    gtk_alignment_set_padding (GTK_ALIGNMENT (priv->align), 0,
			       hscroll ? priv->area_width : 0, 0,
			       vscroll ? priv->area_width : 0);
  }

  /* Store the vscroll/hscroll areas for redrawing */
  if (vscroll) {
    GtkAllocation *allocation = &GTK_WIDGET (area)->allocation;
    priv->vscroll_rect.x = allocation->x + allocation->width -
      priv->area_width;
    priv->vscroll_rect.y = allocation->y;
    priv->vscroll_rect.width = priv->area_width;
    priv->vscroll_rect.height = allocation->height -
      (hscroll ? priv->area_width : 0);
  }
  if (hscroll) {
    GtkAllocation *allocation = &GTK_WIDGET (area)->allocation;
    priv->hscroll_rect.y = allocation->y + allocation->height -
      priv->area_width;
    priv->hscroll_rect.x = allocation->x;
    priv->hscroll_rect.height = priv->area_width;
    priv->hscroll_rect.width = allocation->width -
      (vscroll ? priv->area_width : 0);
  }

  priv->vscroll = vscroll;
  priv->hscroll = hscroll;

  hildon_pannable_area_redraw (area);
}

/* Scroll by a particular amount (in pixels). Optionally, return if
 * the scroll on a particular axis was successful.
 */
static void
hildon_pannable_axis_scroll (HildonPannableArea *area,
                             GtkAdjustment *adjust,
                             gdouble *vel,
                             gdouble inc,
                             gint *overshooting,
                             gint *overshot_dist,
                             gboolean *s)
{
  gdouble dist;
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (area);

  dist = gtk_adjustment_get_value (adjust) - inc;

  /** Overshooting
   * We use overshot_dist to define the distance of the current overshoot,
   * and overshooting to define the direction/whether or not we are overshot
   */
  if (!(*overshooting)) {

    /* Initiation of the overshoot happens when the finger is released
     * and the current position of the pannable contents are out of range
     */
    if (dist < adjust->lower) {
      if (s) *s = FALSE;

      dist = adjust->lower;

      *overshooting = 1;
      *overshot_dist = CLAMP (*overshot_dist + *vel, 0, priv->overshoot_max);
      gtk_widget_queue_resize (GTK_WIDGET (area));
    } else if (dist > adjust->upper - adjust->page_size) {
      if (s) *s = FALSE;

      dist = adjust->upper - adjust->page_size;

      *overshooting = 1;
      *overshot_dist = CLAMP (*overshot_dist + *vel, -1*priv->overshoot_max, 0);
      gtk_widget_queue_resize (GTK_WIDGET (area));
    } else {
      gtk_adjustment_set_value (adjust, dist);

      if (s) {
        *s = TRUE;
      }
    }
  } else {
    if (!priv->clicked) {
      if (s) *s = TRUE;

      /* When the overshoot has started we continue for BOUNCE_STEPS more steps into the overshoot
       * before we reverse direction. The deceleration factor is calculated based on
       * the percentage distance from the first item with each iteration, therefore always
       * returning us to the top/bottom most element
       */
      if (*overshot_dist > 0) {

        if ((*overshooting < BOUNCE_STEPS) && (*vel > 0)) {
          (*overshooting)++;
          *vel = (((gdouble)*overshot_dist)/priv->overshoot_max) * (*vel);
        } else if ((*overshooting >= BOUNCE_STEPS) && (*vel > 0)) {
          *vel *= -1;
          (*overshooting)--;
        } else if ((*overshooting > 1) && (*vel < 0)) {
          (*overshooting)--;
          /* we add the MAX in order to avoid very small speeds */
          *vel = MIN ((((gdouble)*overshot_dist)/priv->overshoot_max) * (*vel), -10.0);
        }

        *overshot_dist = CLAMP (*overshot_dist + *vel, 0, priv->overshoot_max);

        gtk_widget_queue_resize (GTK_WIDGET (area));

      } else if (*overshot_dist < 0) {

        if ((*overshooting < BOUNCE_STEPS) && (*vel < 0)) {
          (*overshooting)++;
          *vel = (((gdouble)*overshot_dist)/priv->overshoot_max) * (*vel) * -1;
        } else if ((*overshooting >= BOUNCE_STEPS) && (*vel < 0)) {
          *vel *= -1;
          (*overshooting)--;
        } else if ((*overshooting > 1) && (*vel > 0)) {
          (*overshooting)--;
          /* we add the MIN in order to avoid very small speeds */
          *vel = MAX ((((gdouble)*overshot_dist)/priv->overshoot_max) * (*vel) * -1, 10.0);
        }

        *overshot_dist = CLAMP (*overshot_dist + (*vel), -1*priv->overshoot_max, 0);

        gtk_widget_queue_resize (GTK_WIDGET (area));

      } else {
        *overshooting = 0;
        *vel = 0;
        gtk_widget_queue_resize (GTK_WIDGET (area));
      }
    } else {
      if (*overshot_dist > 0) {
        *overshot_dist = CLAMP ((*overshot_dist) + inc, 0, priv->overshoot_max);
      } else if (*overshot_dist < 0) {
        *overshot_dist = CLAMP ((*overshot_dist) + inc, -1 * priv->overshoot_max, 0);
      } else {
        *overshooting = 0;
        gtk_adjustment_set_value (adjust, dist);
      }
      gtk_widget_queue_resize (GTK_WIDGET (area));
    }
  }
}

static void
hildon_pannable_area_scroll (HildonPannableArea *area,
                             gdouble x, gdouble y,
			     gboolean *sx, gboolean *sy)
{
  HildonPannableAreaPrivate *priv;
  gboolean hscroll, vscroll;

  priv = PANNABLE_AREA_PRIVATE (area);

  if (!GTK_BIN (priv->align)->child) return;

  vscroll = (priv->vadjust->upper - priv->vadjust->lower >
	     priv->vadjust->page_size) ? TRUE : FALSE;
  hscroll = (priv->hadjust->upper - priv->hadjust->lower >
	     priv->hadjust->page_size) ? TRUE : FALSE;

  if (vscroll) {
    hildon_pannable_axis_scroll (area, priv->vadjust, &priv->vel_y, y,
                                 &priv->overshooting_y, &priv->overshot_dist_y,
                                 sy);
  }

  if (hscroll) {
    hildon_pannable_axis_scroll (area, priv->hadjust, &priv->vel_x, x,
                                 &priv->overshooting_x, &priv->overshot_dist_x,
                                 sx);
  }

  /* If the scroll on a particular axis wasn't succesful, reset the
   * initial scroll position to the new mouse co-ordinate. This means
   * when you get to the top of the page, dragging down works immediately.
   */
  if ((sx)&&(!(*sx))) {
    priv->x = priv->ex;
  }

  if ((sy)&&(!(*sy))) {
    priv->y = priv->ey;
  }

}

static gboolean
hildon_pannable_area_timeout (HildonPannableArea * area)
{
  gboolean sx, sy;
  HildonPannableAreaPrivate *priv;

  GDK_THREADS_ENTER ();

  priv = PANNABLE_AREA_PRIVATE (area);

  if ((!priv->enabled) || (priv->mode == HILDON_PANNABLE_AREA_MODE_PUSH)) {
    priv->idle_id = 0;
    return FALSE;
  }

  if (!priv->clicked) {
    /* Decelerate gradually when pointer is raised */
    if ((!priv->overshot_dist_y) &&
        (!priv->overshot_dist_x)) {
      priv->vel_x *= priv->decel;
      priv->vel_y *= priv->decel;
      if ((ABS (priv->vel_x) < 1.0) && (ABS (priv->vel_y) < 1.0)) {
	priv->idle_id = 0;
	return FALSE;
      }
    }
  } else if (priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO) {
    priv->idle_id = 0;
    return FALSE;
  }

  hildon_pannable_area_scroll (area, priv->vel_x, priv->vel_y, &sx, &sy);

  GDK_THREADS_LEAVE ();

  return TRUE;
}

static gboolean
hildon_pannable_area_motion_notify_cb (GtkWidget * widget,
				       GdkEventMotion * event)
{
  HildonPannableArea *area = HILDON_PANNABLE_AREA (widget);
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (area);
  gint dnd_threshold;
  gdouble x, y;
  gdouble delta, rawvel_x, rawvel_y;
  gint direction_x, direction_y;

  if ((!priv->enabled) || (!priv->clicked) ||
      ((event->time == priv->last_time) && (priv->last_type == 2))) {
    gdk_window_get_pointer (widget->window, NULL, NULL, 0);
    return TRUE;
  }

  /* Only start the scroll if the mouse cursor passes beyond the
   * DnD threshold for dragging.
   */
  g_object_get (G_OBJECT (gtk_settings_get_default ()),
		"gtk-dnd-drag-threshold", &dnd_threshold, NULL);
  x = event->x - priv->x;
  y = event->y - priv->y;

  if ((!priv->moved) && ((ABS (x) > dnd_threshold)
			 || (ABS (y) > dnd_threshold))) {
    priv->moved = TRUE;

    if ((priv->mode != HILDON_PANNABLE_AREA_MODE_PUSH) &&
	(priv->mode != HILDON_PANNABLE_AREA_MODE_AUTO)) {

      if (priv->idle_id)
	g_source_remove (priv->idle_id);

      priv->idle_id = g_timeout_add ((gint)
				     (1000.0 / (gdouble) priv->sps),
				     (GSourceFunc)
				     hildon_pannable_area_timeout, area);
    }
  }

  if (priv->moved) {
    switch (priv->mode) {
    case HILDON_PANNABLE_AREA_MODE_PUSH:
      /* Scroll by the amount of pixels the cursor has moved
       * since the last motion event.
       */
      hildon_pannable_area_scroll (area, x, y, NULL, NULL);
      priv->x = event->x;
      priv->y = event->y;
      break;
    case HILDON_PANNABLE_AREA_MODE_ACCEL:
      /* Set acceleration relative to the initial click */
      priv->ex = event->x;
      priv->ey = event->y;
      priv->vel_x = ((x > 0) ? 1 : -1) *
	(((ABS (x) /
	   (gdouble) widget->allocation.width) *
	  (priv->vmax - priv->vmin)) + priv->vmin);
      priv->vel_y = ((y > 0) ? 1 : -1) *
	(((ABS (y) /
	   (gdouble) widget->allocation.height) *
	  (priv->vmax - priv->vmin)) + priv->vmin);
      break;
    case HILDON_PANNABLE_AREA_MODE_AUTO:

      delta = event->time - priv->last_time;

      rawvel_x = (((event->x - priv->x) / ABS (delta)) *
		  (gdouble) priv->sps) * FORCE;
      rawvel_y = (((event->y - priv->y) / ABS (delta)) *
		  (gdouble) priv->sps) * FORCE;

      /* we store the direction and after the calculation we
         change it, this reduces the ifs for the calculation */
      direction_x = rawvel_x < 0 ? -1 : 1;
      direction_y = rawvel_y < 0 ? -1 : 1;

      rawvel_y = ABS (rawvel_y);
      rawvel_x = ABS (rawvel_x);

      priv->vel_x = priv->vel_x * (1 - SMOOTH_FACTOR) +
	direction_x * rawvel_x * SMOOTH_FACTOR;
      priv->vel_y = priv->vel_y * (1 - SMOOTH_FACTOR) +
	direction_y * rawvel_y * SMOOTH_FACTOR;

      priv->vel_x = priv->vel_x > 0 ? MIN (priv->vel_x, priv->vmax)
	: MAX (priv->vel_x, -1 * priv->vmax);
      priv->vel_y = priv->vel_y > 0 ? MIN (priv->vel_y, priv->vmax)
	: MAX (priv->vel_y, -1 * priv->vmax);

      hildon_pannable_area_scroll (area, x, y, NULL, NULL);

      priv->x = event->x;
      priv->y = event->y;

      break;

    default:
      break;
    }
  }

  if (priv->child) {
    /* Send motion notify to child */
    priv->last_time = event->time;
    priv->last_type = 2;
    event = (GdkEventMotion *) gdk_event_copy ((GdkEvent *) event);
    event->x = priv->cx + (event->x - priv->ix);
    event->y = priv->cy + (event->y - priv->iy);
    event->window = g_object_ref (priv->child);
    gdk_event_put ((GdkEvent *) event);
    gdk_event_free ((GdkEvent *) event);
  }

  gdk_window_get_pointer (widget->window, NULL, NULL, 0);

  return TRUE;
}

static gboolean
hildon_pannable_area_button_release_cb (GtkWidget * widget,
					GdkEventButton * event)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (widget);
  gint x, y;
  GdkWindow *child;

  priv->scroll_indicator_event_interrupt = 0;
  priv->scroll_delay_counter = 0;

  if (priv->scroll_indicator_timeout) {
    g_source_remove (priv->scroll_indicator_timeout);
  }

  if ((ABS (priv->vel_y) > 1.0)||
      (ABS (priv->vel_x) > 1.0)) {
    priv->scroll_indicator_alpha = 1.0;
  }

  priv->scroll_indicator_timeout = g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
                     (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, widget);

  if ((!priv->clicked) || (!priv->enabled) || (event->button != 1) ||
      ((event->time == priv->last_time) && (priv->last_type == 3)))
    return TRUE;

  priv->clicked = FALSE;

  if (priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO) {
    if (priv->idle_id)
      g_source_remove (priv->idle_id);

    /* If overshoot has been initiated with a finger down, on release set max speed */
    if (priv->overshot_dist_y != 0) {
      priv->overshooting_y = BOUNCE_STEPS; /* Hack to stop a bounce in the finger down case */
      priv->vel_y = priv->vmax;
    }

    if (priv->overshot_dist_x != 0) {
      priv->overshooting_x = BOUNCE_STEPS; /* Hack to stop a bounce in the finger down case */
      priv->vel_x = priv->vmax;
    }

    priv->idle_id = g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
				   (GSourceFunc)
				   hildon_pannable_area_timeout, widget);
  }

  priv->last_time = event->time;
  priv->last_type = 3;

  if (!priv->child) {
    priv->moved = FALSE;
    return TRUE;
  }

  child =
    hildon_pannable_area_get_topmost (GTK_BIN (priv->align)->child->window,
				      event->x, event->y, &x, &y);

  event = (GdkEventButton *) gdk_event_copy ((GdkEvent *) event);
  event->x = x;
  event->y = y;

  /* Leave the widget if we've moved - This doesn't break selection,
   * but stops buttons from being clicked.
   */
  if ((child != priv->child) || (priv->moved)) {
    /* Send synthetic leave event */
    synth_crossing (priv->child, x, y, event->x_root,
		    event->y_root, event->time, FALSE);
    /* Send synthetic button release event */
    ((GdkEventAny *) event)->window = g_object_ref (priv->child);
    gdk_event_put ((GdkEvent *) event);
  } else {
    /* Send synthetic button release event */
    ((GdkEventAny *) event)->window = g_object_ref (child);
    gdk_event_put ((GdkEvent *) event);
    /* Send synthetic leave event */
    synth_crossing (priv->child, x, y, event->x_root,
		    event->y_root, event->time, FALSE);
  }
  g_object_remove_weak_pointer ((GObject *) priv->child,
				(gpointer *) & priv->child);

  priv->moved = FALSE;
  gdk_event_free ((GdkEvent *) event);

  return TRUE;
}

static void
rgb_from_gdkcolor (GdkColor *color, gdouble *r, gdouble *g, gdouble *b)
{
  *r = (color->red >> 8) / 255.0;
  *g = (color->green >> 8) / 255.0;
  *b = (color->blue >> 8) / 255.0;
}

static void
hildon_pannable_draw_vscroll (GtkWidget * widget, GdkColor *back_color, GdkColor *scroll_color)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (widget);
  gint y, height;
  cairo_t *cr;
  cairo_pattern_t *pattern;
  gdouble r, g, b;
  gint radius = (priv->vscroll_rect.width/2) - 1;

  cr = gdk_cairo_create(widget->window);

  /* Draw the background */
  rgb_from_gdkcolor (back_color, &r, &g, &b);
  cairo_set_source_rgb (cr, r, g, b);
  cairo_rectangle(cr, priv->vscroll_rect.x, priv->vscroll_rect.y,
                  priv->vscroll_rect.width,
                  priv->vscroll_rect.height);
  cairo_fill_preserve (cr);
  cairo_clip (cr);

  /* Calculate the scroll bar height and position */
  y = widget->allocation.y +
    ((priv->vadjust->value / priv->vadjust->upper) *
     (widget->allocation.height -
      (priv->hscroll ? priv->area_width : 0)));
  height = (widget->allocation.y +
            (((priv->vadjust->value +
               priv->vadjust->page_size) /
              priv->vadjust->upper) *
             (widget->allocation.height -
              (priv->hscroll ? priv->area_width : 0)))) - y;

  /* Set a minimum height */
  height = MAX (SCROLL_BAR_MIN_SIZE, height);

  /* Draw the scrollbar */
  rgb_from_gdkcolor (scroll_color, &r, &g, &b);

  pattern = cairo_pattern_create_linear(radius+1, y, radius+1,y + height);
  cairo_pattern_add_color_stop_rgb(pattern, 0, r, g, b);
  cairo_pattern_add_color_stop_rgb(pattern, 1, r/2, g/2, b/2);
  cairo_set_source(cr, pattern);
  cairo_fill(cr);
  cairo_pattern_destroy(pattern);

  cairo_arc(cr, priv->vscroll_rect.x + radius + 1, y + radius + 1, radius, G_PI, 0);
  cairo_line_to(cr, priv->vscroll_rect.x + (radius * 2) + 1, y + height - radius);
  cairo_arc(cr, priv->vscroll_rect.x + radius + 1, y + height - radius, radius, 0, G_PI);
  cairo_line_to(cr, priv->vscroll_rect.x + 1, y + height - radius);
  cairo_clip (cr);

  cairo_paint_with_alpha(cr, priv->scroll_indicator_alpha);

  cairo_destroy(cr);
}

static void
hildon_pannable_draw_hscroll (GtkWidget * widget, GdkColor *back_color, GdkColor *scroll_color)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (widget);
  gint x, width;
  cairo_t *cr;
  cairo_pattern_t *pattern;
  gdouble r, g, b;
  gint radius = (priv->hscroll_rect.height/2) - 1;

  cr = gdk_cairo_create(widget->window);

  /* Draw the background */
  rgb_from_gdkcolor (back_color, &r, &g, &b);
  cairo_set_source_rgb (cr, r, g, b);
  cairo_rectangle(cr, priv->hscroll_rect.x, priv->hscroll_rect.y,
                  priv->hscroll_rect.width,
                  priv->hscroll_rect.height);
  cairo_fill_preserve (cr);
  cairo_clip (cr);

  /* calculate the scrollbar width and position */
  x = widget->allocation.x +
    ((priv->hadjust->value / priv->hadjust->upper) *
     (widget->allocation.width - (priv->vscroll ? priv->area_width : 0)));
  width =
    (widget->allocation.x +
     (((priv->hadjust->value +
        priv->hadjust->page_size) / priv->hadjust->upper) *
      (widget->allocation.width -
       (priv->vscroll ? priv->area_width : 0)))) - x;

  /* Set a minimum width */
  width = MAX (SCROLL_BAR_MIN_SIZE, width);

  /* Draw the scrollbar */
  rgb_from_gdkcolor (scroll_color, &r, &g, &b);

  pattern = cairo_pattern_create_linear(x, radius+1, x+width, radius+1);
  cairo_pattern_add_color_stop_rgb(pattern, 0, r, g, b);
  cairo_pattern_add_color_stop_rgb(pattern, 1, r/2, g/2, b/2);
  cairo_set_source(cr, pattern);
  cairo_fill(cr);
  cairo_pattern_destroy(pattern);

  cairo_arc_negative(cr, x + radius + 1, priv->hscroll_rect.y + radius + 1, radius, 3*G_PI_2, G_PI_2);
  cairo_line_to(cr, x + width - radius, priv->hscroll_rect.y + (radius * 2) + 1);
  cairo_arc_negative(cr, x + width - radius, priv->hscroll_rect.y + radius + 1, radius, G_PI_2, 3*G_PI_2);
  cairo_line_to(cr, x + width - radius, priv->hscroll_rect.y + 1);
  cairo_clip (cr);

  cairo_paint_with_alpha(cr, priv->scroll_indicator_alpha);

  cairo_destroy(cr);
}

static gboolean
hildon_pannable_area_expose_event (GtkWidget * widget, GdkEventExpose * event)
{

  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (widget);
  GdkColor back_color = widget->style->base[GTK_STATE_NORMAL];
  GdkColor scroll_color = widget->style->base[GTK_STATE_SELECTED];

  if (GTK_BIN (priv->align)->child) {

    if (priv->scroll_indicator_alpha > 0) {
      if (priv->vscroll) {
        hildon_pannable_draw_vscroll (widget, &back_color, &scroll_color);
      }
      if (priv->hscroll) {
        hildon_pannable_draw_hscroll (widget, &back_color, &scroll_color);
      }
    }

    /* draw overshooting rectangles */
    if (priv->overshot_dist_y > 0) {
      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
			  widget->allocation.x,
			  widget->allocation.y,
			  widget->allocation.width -
			  priv->vscroll_rect.width,
                          priv->overshot_dist_y);
    } else if (priv->overshot_dist_y < 0) {
      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
			  widget->allocation.x,
			  widget->allocation.y +
			  widget->allocation.height +
			  priv->overshot_dist_y -
                          (priv->hscroll ? priv->hscroll_rect.height : 0),
			  widget->allocation.width -
			  priv->vscroll_rect.width,
			  -1 * priv->overshot_dist_y);
    }

    if (priv->overshot_dist_x > 0) {
      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
			  widget->allocation.x,
			  widget->allocation.y,
                          priv->overshot_dist_x,
			  widget->allocation.height -
			  priv->hscroll_rect.height);
    } else if (priv->overshot_dist_x < 0) {
      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
			  widget->allocation.x +
			  widget->allocation.width +
			  priv->overshot_dist_x -
                          (priv->vscroll ? priv->vscroll_rect.width : 0),
			  widget->allocation.y,
			  -1 * priv->overshot_dist_x,
			  widget->allocation.height -
			  priv->hscroll_rect.height);
    }

  }

  return GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->expose_event (widget, event);
}

static void
hildon_pannable_area_destroy (GtkObject * object)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (object);

  if (priv->hadjust) {
    g_object_unref (G_OBJECT (priv->hadjust));
    priv->hadjust = NULL;
  }

  if (priv->vadjust) {
    g_object_unref (G_OBJECT (priv->vadjust));
    priv->vadjust = NULL;
  }

  GTK_OBJECT_CLASS (hildon_pannable_area_parent_class)->destroy (object);
}

static void
parent_set_cb (GtkWidget * widget, GtkObject * parent,
	       HildonPannableArea * area)
{
  if (!parent) {
    g_signal_handlers_disconnect_by_func (widget,
					  hildon_pannable_area_refresh, area);
    g_signal_handlers_disconnect_by_func (widget, gtk_widget_queue_resize,
					  area);
    g_signal_handlers_disconnect_by_func (widget, parent_set_cb, area);
    gtk_widget_set_scroll_adjustments (widget, NULL, NULL);
  }
}

static void
hildon_pannable_area_add (GtkContainer * container, GtkWidget * child)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (container);

  gtk_container_add (GTK_CONTAINER (priv->align), child);
  g_signal_connect_swapped (child, "size-allocate",
			    G_CALLBACK (hildon_pannable_area_refresh),
			    container);
  g_signal_connect_swapped (child, "size-request",
			    G_CALLBACK (gtk_widget_queue_resize), container);
  g_signal_connect (child, "parent-set", G_CALLBACK (parent_set_cb),
		    container);

  if (!gtk_widget_set_scroll_adjustments
      (child, priv->hadjust, priv->vadjust))
    g_warning ("%s: cannot add non scrollable widget, "
	       "wrap it in a viewport", __FUNCTION__);
}

static void
hildon_pannable_area_get_property (GObject * object, guint property_id,
				   GValue * value, GParamSpec * pspec)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (object);

  switch (property_id) {
  case PROP_ENABLED:
    g_value_set_boolean (value, priv->enabled);
    break;
  case PROP_MODE:
    g_value_set_enum (value, priv->mode);
    break;
  case PROP_VELOCITY_MIN:
    g_value_set_double (value, priv->vmin);
    break;
  case PROP_VELOCITY_MAX:
    g_value_set_double (value, priv->vmax);
    break;
  case PROP_DECELERATION:
    g_value_set_double (value, priv->decel);
    break;
  case PROP_SPS:
    g_value_set_uint (value, priv->sps);
    break;
  case PROP_VINDICATOR:
    g_value_set_enum (value, priv->vindicator_mode);
    break;
  case PROP_HINDICATOR:
    g_value_set_enum (value, priv->hindicator_mode);
    break;
  case PROP_OVERSHOOT_MAX:
    g_value_set_int (value, priv->overshoot_max);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_pannable_area_set_property (GObject * object, guint property_id,
				   const GValue * value, GParamSpec * pspec)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (object);
  gboolean enabled;

  switch (property_id) {
  case PROP_ENABLED:
    enabled = g_value_get_boolean (value);

    if ((priv->enabled != enabled) && (GTK_WIDGET_REALIZED (object))) {
      if (enabled)
	gdk_window_raise (priv->event_window);
      else
	gdk_window_lower (priv->event_window);
    }

    priv->enabled = enabled;
    break;
  case PROP_MODE:
    priv->mode = g_value_get_enum (value);
    break;
  case PROP_VELOCITY_MIN:
    priv->vmin = g_value_get_double (value);
    break;
  case PROP_VELOCITY_MAX:
    priv->vmax = g_value_get_double (value);
    break;
  case PROP_DECELERATION:
    priv->decel = g_value_get_double (value);
    break;
  case PROP_SPS:
    priv->sps = g_value_get_uint (value);
    break;
  case PROP_VINDICATOR:
    priv->vindicator_mode = g_value_get_enum (value);
    break;
  case PROP_HINDICATOR:
    priv->hindicator_mode = g_value_get_enum (value);
    break;
  case PROP_OVERSHOOT_MAX:
    priv->overshoot_max = g_value_get_int (value);
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_pannable_area_dispose (GObject * object)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (object);

  if (priv->idle_id) {
    g_source_remove (priv->idle_id);
    priv->idle_id = 0;
  }

  if (priv->hadjust) {
    g_object_unref (priv->hadjust);
    priv->hadjust = NULL;
  }
  if (priv->vadjust) {
    g_object_unref (priv->vadjust);
    priv->vadjust = NULL;
  }

  if (G_OBJECT_CLASS (hildon_pannable_area_parent_class)->dispose)
    G_OBJECT_CLASS (hildon_pannable_area_parent_class)->dispose (object);
}

static void
hildon_pannable_area_finalize (GObject * object)
{
  G_OBJECT_CLASS (hildon_pannable_area_parent_class)->finalize (object);
}

static void
hildon_pannable_area_realize (GtkWidget * widget)
{
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint border_width;
  HildonPannableAreaPrivate *priv;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  border_width = GTK_CONTAINER (widget)->border_width;

  attributes.x = widget->allocation.x + border_width;
  attributes.y = widget->allocation.y + border_width;
  attributes.width = widget->allocation.width - 2 * border_width;
  attributes.height = widget->allocation.height - 2 * border_width;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget)
    | GDK_BUTTON_MOTION_MASK
    | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK
    | GDK_EXPOSURE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;

  priv = PANNABLE_AREA_PRIVATE (widget);

  widget->window = gtk_widget_get_parent_window (widget);
  g_object_ref (widget->window);

  attributes.wclass = GDK_INPUT_ONLY;
  attributes_mask = GDK_WA_X | GDK_WA_Y;

  priv->event_window = gdk_window_new (widget->window,
				       &attributes, attributes_mask);
  gdk_window_set_user_data (priv->event_window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
}

static void
hildon_pannable_area_unrealize (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv;

  priv = PANNABLE_AREA_PRIVATE (widget);

  if (priv->event_window != NULL) {
    gdk_window_set_user_data (priv->event_window, NULL);
    gdk_window_destroy (priv->event_window);
    priv->event_window = NULL;
  }

  if (GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->unrealize)
    (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->
     unrealize) (widget);
}

static void
hildon_pannable_area_map (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv;

  priv = PANNABLE_AREA_PRIVATE (widget);

  if (priv->event_window != NULL && !priv->enabled)
    gdk_window_show (priv->event_window);

  (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->map) (widget);

  if (priv->event_window != NULL && priv->enabled)
    gdk_window_show (priv->event_window);

  if (priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO) {
    /* TODO 3: This should only be active if this hint has been set
     *priv->overshot_dist_y = priv->overshoot_max;
     *
     *priv->vel_y = priv->vmax * 0.1;
     *
     * priv->idle_id = g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
     *				   (GSourceFunc)
     *				   hildon_pannable_area_timeout, widget);
    */
  }

}

static void
hildon_pannable_area_unmap (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv;

  priv = PANNABLE_AREA_PRIVATE (widget);

  if (priv->event_window != NULL)
    gdk_window_hide (priv->event_window);

  (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->unmap) (widget);
}

static void
hildon_pannable_area_size_request (GtkWidget * widget,
				   GtkRequisition * requisition)
{
  /* Request tiny size, seeing as we have no decoration of our own. */
  requisition->width = 32;
  requisition->height = 32;
}

static void
hildon_pannable_area_size_allocate (GtkWidget * widget,
				    GtkAllocation * allocation)
{
  GtkBin *bin;
  GtkAllocation child_allocation;
  HildonPannableAreaPrivate *priv;

  widget->allocation = *allocation;
  bin = GTK_BIN (widget);

  priv = PANNABLE_AREA_PRIVATE (widget);

  child_allocation.x = allocation->x + GTK_CONTAINER (widget)->border_width;
  child_allocation.y = allocation->y + GTK_CONTAINER (widget)->border_width;
  child_allocation.width = MAX (allocation->width -
				GTK_CONTAINER (widget)->border_width * 2, 0);
  child_allocation.height = MAX (allocation->height -
				 GTK_CONTAINER (widget)->border_width * 2, 0);

  if (GTK_WIDGET_REALIZED (widget)) {
    if (priv->event_window != NULL)
      gdk_window_move_resize (priv->event_window,
			      child_allocation.x,
			      child_allocation.y,
			      child_allocation.width,
			      child_allocation.height);
  }

  if (priv->overshot_dist_y > 0) {
    child_allocation.y += priv->overshot_dist_y;
    child_allocation.height -= priv->overshot_dist_y;

    gtk_adjustment_set_value (priv->vadjust, priv->vadjust->lower);

  } else if (priv->overshot_dist_y < 0) {
    child_allocation.height += priv->overshot_dist_y;
  }

  if (priv->overshot_dist_x > 0) {
    child_allocation.x += priv->overshot_dist_x;
    child_allocation.width -= priv->overshot_dist_x;

    gtk_adjustment_set_value (priv->hadjust, priv->hadjust->lower);

  } else if (priv->overshot_dist_x < 0) {
    child_allocation.width += priv->overshot_dist_x;
  }

  if (bin->child)
    gtk_widget_size_allocate (bin->child, &child_allocation);

  /* we have to this after child size_allocate because page_size is
   * changed when we allocate the size of the children */
  if (priv->overshot_dist_y < 0) {
    gtk_adjustment_set_value (priv->vadjust, priv->vadjust->upper -
                              priv->vadjust->page_size);
  }

  if (priv->overshot_dist_x < 0) {
    gtk_adjustment_set_value (priv->hadjust, priv->hadjust->upper -
                              priv->hadjust->page_size);
  }

}

static void
hildon_pannable_area_style_set (GtkWidget * widget, GtkStyle * previous_style)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (widget);

  GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->
    style_set (widget, previous_style);

  gtk_widget_style_get (widget, "indicator-width", &priv->area_width, NULL);
}

static void
hildon_pannable_area_class_init (HildonPannableAreaClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HildonPannableAreaPrivate));

  object_class->get_property = hildon_pannable_area_get_property;
  object_class->set_property = hildon_pannable_area_set_property;
  object_class->dispose = hildon_pannable_area_dispose;
  object_class->finalize = hildon_pannable_area_finalize;

  gtkobject_class->destroy = hildon_pannable_area_destroy;

  widget_class->realize = hildon_pannable_area_realize;
  widget_class->unrealize = hildon_pannable_area_unrealize;
  widget_class->map = hildon_pannable_area_map;
  widget_class->unmap = hildon_pannable_area_unmap;
  widget_class->size_request = hildon_pannable_area_size_request;
  widget_class->size_allocate = hildon_pannable_area_size_allocate;
  widget_class->expose_event = hildon_pannable_area_expose_event;
  widget_class->style_set = hildon_pannable_area_style_set;
  widget_class->button_press_event = hildon_pannable_area_button_press_cb;
  widget_class->button_release_event = hildon_pannable_area_button_release_cb;
  widget_class->motion_notify_event = hildon_pannable_area_motion_notify_cb;

  container_class->add = hildon_pannable_area_add;

  g_object_class_install_property (object_class,
				   PROP_ENABLED,
				   g_param_spec_boolean ("enabled",
							 "Enabled",
							 "Enable or disable finger-scroll.",
							 TRUE,
							 G_PARAM_READWRITE |
							 G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_VINDICATOR,
				   g_param_spec_enum ("vindicator_mode",
						      "vindicator mode",
						      "Mode of the vertical scrolling indicator",
						      HILDON_TYPE_PANNABLE_AREA_INDICATOR_MODE,
						      HILDON_PANNABLE_AREA_INDICATOR_MODE_AUTO,
						      G_PARAM_READWRITE |
						      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_HINDICATOR,
				   g_param_spec_enum ("hindicator_mode",
						      "hindicator mode",
						      "Mode of the horizontal scrolling indicator",
						      HILDON_TYPE_PANNABLE_AREA_INDICATOR_MODE,
						      HILDON_PANNABLE_AREA_INDICATOR_MODE_AUTO,
						      G_PARAM_READWRITE |
						      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_MODE,
				   g_param_spec_enum ("mode",
						      "Scroll mode",
						      "Change the finger-scrolling mode.",
						      HILDON_TYPE_PANNABLE_AREA_MODE,
						      HILDON_PANNABLE_AREA_MODE_AUTO,
						      G_PARAM_READWRITE |
						      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_VELOCITY_MIN,
				   g_param_spec_double ("velocity_min",
							"Minimum scroll velocity",
							"Minimum distance the child widget should scroll "
							"per 'frame', in pixels.",
							0, G_MAXDOUBLE, 0,
							G_PARAM_READWRITE |
							G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_VELOCITY_MAX,
				   g_param_spec_double ("velocity_max",
							"Maximum scroll velocity",
							"Maximum distance the child widget should scroll "
							"per 'frame', in pixels.",
							0, G_MAXDOUBLE, 80,
							G_PARAM_READWRITE |
							G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_DECELERATION,
				   g_param_spec_double ("deceleration",
							"Deceleration multiplier",
							"The multiplier used when decelerating when in "
							"acceleration scrolling mode.",
							0, 1.0, 0.85,
							G_PARAM_READWRITE |
							G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_SPS,
				   g_param_spec_uint ("sps",
						      "Scrolls per second",
						      "Amount of scroll events to generate per second.",
						      0, G_MAXUINT, 25,
						      G_PARAM_READWRITE |
						      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_OVERSHOOT_MAX,
				   g_param_spec_int ("overshoot",
                                                     "Overshoot distance",
                                                     "Space we allow the widget to pass over its limits when hitting the edges.",
                                                     G_MININT, G_MAXINT, 150,
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_CONSTRUCT));

  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_uint
					   ("indicator-width",
					    "Width of the scroll indicators",
					    "Pixel width used to draw the scroll indicators.",
					    0, G_MAXUINT, 8,
					    G_PARAM_READWRITE));
}

static void
hildon_pannable_area_init (HildonPannableArea * self)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (self);

  priv->moved = FALSE;
  priv->clicked = FALSE;
  priv->last_time = 0;
  priv->last_type = 0;
  priv->vscroll = TRUE;
  priv->hscroll = TRUE;
  priv->area_width = 6;
  priv->overshot_dist_x = 0;
  priv->overshot_dist_y = 0;
  priv->overshooting_y = 0;
  priv->overshooting_x = 0;
  priv->idle_id = 0;
  priv->scroll_indicator_alpha = 0;
  priv->scroll_indicator_timeout = 0;
  priv->scroll_indicator_event_interrupt = 0;
  priv->scroll_delay_counter = 0;

  priv->align = gtk_alignment_new (0.5, 0.5, 1.0, 1.0);
  GTK_CONTAINER_CLASS (hildon_pannable_area_parent_class)->
    add (GTK_CONTAINER (self), priv->align);
  gtk_alignment_set_padding (GTK_ALIGNMENT (priv->align), 0, priv->area_width,
			     0, priv->area_width);
  gtk_widget_show (priv->align);

  gtk_widget_add_events (GTK_WIDGET (self), GDK_POINTER_MOTION_HINT_MASK);

  priv->hadjust =
    GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
  priv->vadjust =
    GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

  g_object_ref_sink (G_OBJECT (priv->hadjust));
  g_object_ref_sink (G_OBJECT (priv->vadjust));

  g_signal_connect_swapped (G_OBJECT (priv->hadjust), "changed",
			    G_CALLBACK (hildon_pannable_area_refresh), self);
  g_signal_connect_swapped (G_OBJECT (priv->vadjust), "changed",
			    G_CALLBACK (hildon_pannable_area_refresh), self);
  g_signal_connect_swapped (G_OBJECT (priv->hadjust), "value-changed",
			    G_CALLBACK (hildon_pannable_area_redraw), self);
  g_signal_connect_swapped (G_OBJECT (priv->vadjust), "value-changed",
			    G_CALLBACK (hildon_pannable_area_redraw), self);
}

/**
 * hildon_pannable_area_new:
 *
 * Create a new pannable area widget
 *
 * Returns: the newly created #HildonPannableArea
 */

GtkWidget *
hildon_pannable_area_new (void)
{
  return g_object_new (HILDON_TYPE_PANNABLE_AREA, NULL);
}

/**
 * hildon_pannable_area_new_full:
 * @mode: #HildonPannableAreaMode
 * @enabled: Value for the enabled property
 * @vel_min: Value for the velocity-min property
 * @vel_max: Value for the velocity-max property
 * @decel: Value for the deceleration property
 * @sps: Value for the sps property
 *
 * Create a new #HildonPannableArea widget and set various properties
 *
 * returns: the newly create #HildonPannableArea
 */

GtkWidget *
hildon_pannable_area_new_full (gint mode, gboolean enabled,
			       gdouble vel_min, gdouble vel_max,
			       gdouble decel, guint sps)
{
  return g_object_new (HILDON_TYPE_PANNABLE_AREA,
		       "mode", mode,
		       "enabled", enabled,
		       "velocity_min", vel_min,
		       "velocity_max", vel_max,
		       "deceleration", decel, "sps", sps, NULL);
}

/**
 * hildon_pannable_area_add_with_viewport:
 * @area: A #HildonPannableArea
 * @child: Child widget to add to the viewport
 *
 * Convenience function used to add a child to a #GtkViewport, and add the
 * viewport to the scrolled window.
 */

void
hildon_pannable_area_add_with_viewport (HildonPannableArea * area,
					GtkWidget * child)
{
  GtkWidget *viewport = gtk_viewport_new (NULL, NULL);
  gtk_viewport_set_shadow_type (GTK_VIEWPORT (viewport), GTK_SHADOW_NONE);
  gtk_container_add (GTK_CONTAINER (viewport), child);
  gtk_widget_show (viewport);
  gtk_container_add (GTK_CONTAINER (area), viewport);
}
