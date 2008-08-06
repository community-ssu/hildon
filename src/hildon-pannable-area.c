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

#include <gdk/gdkx.h>
#include <cairo.h>
#include <math.h>
#include "hildon-pannable-area.h"
#include "hildon-marshalers.h"
#include "hildon-enum-types.h"

#define SMOOTH_FACTOR 0.85
#define FORCE 5
#define BOUNCE_STEPS 6
#define SCROLL_BAR_MIN_SIZE 5
#define RATIO_TOLERANCE 0.000001
#define DND_THRESHOLD_INC 20

G_DEFINE_TYPE (HildonPannableArea, hildon_pannable_area, GTK_TYPE_BIN)
#define PANNABLE_AREA_PRIVATE(o)                                \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_PANNABLE_AREA, \
                                HildonPannableAreaPrivate))
typedef struct _HildonPannableAreaPrivate HildonPannableAreaPrivate;

struct _HildonPannableAreaPrivate {
  HildonPannableAreaMode mode;
  HildonPannableAreaMovMode mov_mode;
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
  gdouble vfast_factor;
  gdouble decel;
  gdouble scroll_time;
  gdouble vel_factor;
  guint sps;
  gdouble vel_x;
  gdouble vel_y;
  GdkWindow *child;
  gint ix;			/* Initial click mouse co-ordinates */
  gint iy;
  gint cx;			/* Initial click child window mouse co-ordinates */
  gint cy;
  guint idle_id;
  gdouble scroll_to_x;
  gdouble scroll_to_y;
  gint overshot_dist_x;
  gint overshot_dist_y;
  gint overshooting_y;
  gint overshooting_x;
  gdouble scroll_indicator_alpha;
  gint scroll_indicator_timeout;
  gint scroll_indicator_event_interrupt;
  gint scroll_delay_counter;
  gint vovershoot_max;
  gint hovershoot_max;
  gboolean initial_hint;
  gboolean first_drag;

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

/*signals*/
enum {
  HORIZONTAL_MOVEMENT,
  VERTICAL_MOVEMENT,
  LAST_SIGNAL
};

static guint pannable_area_signals [LAST_SIGNAL] = { 0 };

enum {
  PROP_ENABLED = 1,
  PROP_MODE,
  PROP_MOV_MODE,
  PROP_VELOCITY_MIN,
  PROP_VELOCITY_MAX,
  PROP_VELOCITY_FAST_FACTOR,
  PROP_DECELERATION,
  PROP_SPS,
  PROP_VINDICATOR,
  PROP_HINDICATOR,
  PROP_VOVERSHOOT_MAX,
  PROP_HOVERSHOOT_MAX,
  PROP_SCROLL_TIME,
  PROP_INITIAL_HINT
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

  while (window) {
    gint child_x = 0, child_y = 0;
    GList *c, *children = gdk_window_peek_children (window);
    GdkWindow *old_window = window;

    for (c = children; c; c = c->next) {
      GdkWindow *child = (GdkWindow *) c->data;
      gint wx, wy;

      gdk_window_get_geometry (child, &wx, &wy, &width, &height, NULL);

      if ((x >= wx) && (x < (wx + width)) && (y >= wy)
	  && (y < (wy + height))) {
	child_x = x - wx;
	child_y = y - wy;
	window = child;
      }
    }

    if (window == old_window)
      break;

    x = child_x;
    y = child_y;
  }

  if (tx)
    *tx = x;
  if (ty)
    *ty = y;

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
       (priv->last_type == 1)) || (gtk_bin_get_child (GTK_BIN (widget)) == NULL))
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

  priv->scroll_to_x = -1;
  priv->scroll_to_y = -1;

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
   */
  if ((ABS (priv->vel_x) <= (priv->vmax * priv->vfast_factor)) &&
      (ABS (priv->vel_y) <= (priv->vmax * priv->vfast_factor)))
    priv->child =
      hildon_pannable_area_get_topmost (gtk_bin_get_child (GTK_BIN (widget))->window,
					event->x, event->y, &x, &y);
  else
    priv->child = NULL;

  priv->clicked = TRUE;
  /* Stop scrolling on mouse-down (so you can flick, then hold to stop) */
  priv->vel_x = 0;
  priv->vel_y = 0;

  if ((priv->child) && (priv->child != gtk_bin_get_child (GTK_BIN (widget))->window)) {

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
  GtkWidget *widget = gtk_bin_get_child (GTK_BIN (area));
  gboolean vscroll, hscroll;

  if (!widget) {
    priv->vscroll = FALSE;
    priv->hscroll = FALSE;
    return;
  }

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
                             gdouble *scroll_to,
                             gint overshoot_max,
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

      if (overshoot_max!=0) {
        *overshooting = 1;
        *scroll_to = -1;
        *overshot_dist = CLAMP (*overshot_dist + *vel, 0, overshoot_max);
        gtk_widget_queue_resize (GTK_WIDGET (area));
      } else {
        *vel = 0.0;
      }
    } else if (dist > adjust->upper - adjust->page_size) {
      if (s) *s = FALSE;

      dist = adjust->upper - adjust->page_size;

      if (overshoot_max!=0) {
        *overshooting = 1;
        *scroll_to = -1;
        *overshot_dist = CLAMP (*overshot_dist + *vel, -1*overshoot_max, 0);
        gtk_widget_queue_resize (GTK_WIDGET (area));
      } else {
        *vel = 0.0;
      }
    } else {
      if ((*scroll_to) != -1) {
        if (((inc < 0)&&(*scroll_to <= dist))||
            ((inc > 0)&&(*scroll_to >= dist))) {
          dist = *scroll_to;
          *scroll_to = -1;
          *vel = 0;
        }
      }
    }

    gtk_adjustment_set_value (adjust, dist);
  } else {
    if (!priv->clicked) {

      /* When the overshoot has started we continue for BOUNCE_STEPS more steps into the overshoot
       * before we reverse direction. The deceleration factor is calculated based on
       * the percentage distance from the first item with each iteration, therefore always
       * returning us to the top/bottom most element
       */
      if (*overshot_dist > 0) {

        if ((*overshooting < BOUNCE_STEPS) && (*vel > 0)) {
          (*overshooting)++;
          *vel = (((gdouble)*overshot_dist)/overshoot_max) * (*vel);
        } else if ((*overshooting >= BOUNCE_STEPS) && (*vel > 0)) {
          *vel *= -1;
          (*overshooting)--;
        } else if ((*overshooting > 1) && (*vel < 0)) {
          (*overshooting)--;
          /* we add the MAX in order to avoid very small speeds */
          *vel = MIN ((((gdouble)*overshot_dist)/overshoot_max) * (*vel), -10.0);
        }

        *overshot_dist = CLAMP (*overshot_dist + *vel, 0, overshoot_max);

        gtk_widget_queue_resize (GTK_WIDGET (area));

      } else if (*overshot_dist < 0) {

        if ((*overshooting < BOUNCE_STEPS) && (*vel < 0)) {
          (*overshooting)++;
          *vel = (((gdouble)*overshot_dist)/overshoot_max) * (*vel) * -1;
        } else if ((*overshooting >= BOUNCE_STEPS) && (*vel < 0)) {
          *vel *= -1;
          (*overshooting)--;
        } else if ((*overshooting > 1) && (*vel > 0)) {
          (*overshooting)--;
          /* we add the MIN in order to avoid very small speeds */
          *vel = MAX ((((gdouble)*overshot_dist)/overshoot_max) * (*vel) * -1, 10.0);
        }

        *overshot_dist = CLAMP (*overshot_dist + (*vel), -1*overshoot_max, 0);

        gtk_widget_queue_resize (GTK_WIDGET (area));

      } else {
        *overshooting = 0;
        *vel = 0;
        gtk_widget_queue_resize (GTK_WIDGET (area));
      }
    } else {
      if (*overshot_dist > 0) {
        *overshot_dist = CLAMP ((*overshot_dist) + inc, 0, overshoot_max);
      } else if (*overshot_dist < 0) {
        *overshot_dist = CLAMP ((*overshot_dist) + inc, -1 * overshoot_max, 0);
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
                             gdouble x, gdouble y)
{
  gboolean sx, sy;
  HildonPannableAreaPrivate *priv;
  gboolean hscroll, vscroll;

  priv = PANNABLE_AREA_PRIVATE (area);

  if (gtk_bin_get_child (GTK_BIN (area)) == NULL)
    return;

  vscroll = (priv->vadjust->upper - priv->vadjust->lower >
	     priv->vadjust->page_size) ? TRUE : FALSE;
  hscroll = (priv->hadjust->upper - priv->hadjust->lower >
	     priv->hadjust->page_size) ? TRUE : FALSE;

  sx = TRUE;
  sy = TRUE;

  if (vscroll) {
    hildon_pannable_axis_scroll (area, priv->vadjust, &priv->vel_y, y,
                                 &priv->overshooting_y, &priv->overshot_dist_y,
                                 &priv->scroll_to_y, priv->vovershoot_max, &sy);
  }

  if (hscroll) {
    hildon_pannable_axis_scroll (area, priv->hadjust, &priv->vel_x, x,
                                 &priv->overshooting_x, &priv->overshot_dist_x,
                                 &priv->scroll_to_x, priv->hovershoot_max, &sx);
  }

  /* If the scroll on a particular axis wasn't succesful, reset the
   * initial scroll position to the new mouse co-ordinate. This means
   * when you get to the top of the page, dragging down works immediately.
   */
  if (!sx) {
    priv->x = priv->ex;
  }

  if (!sy) {
    priv->y = priv->ey;
  }

}

static gboolean
hildon_pannable_area_timeout (HildonPannableArea * area)
{
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

      /* in case we move to a specific point do not decelerate when arriving */
      if ((priv->scroll_to_x != -1)||(priv->scroll_to_y != -1)) {

        if (ABS (priv->vel_x) >= 1.5) {
          priv->vel_x *= priv->decel;
        }

        if (ABS (priv->vel_y) >= 1.5) {
          priv->vel_y *= priv->decel;
        }

      } else {
        priv->vel_x *= priv->decel;
        priv->vel_y *= priv->decel;

        if ((ABS (priv->vel_x) < 1.0) && (ABS (priv->vel_y) < 1.0)) {
          priv->vel_x = 0;
          priv->vel_y = 0;
          priv->idle_id = 0;
          return FALSE;
        }
      }
    }
  } else if (priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO) {
    priv->idle_id = 0;
    return FALSE;
  }

  hildon_pannable_area_scroll (area, priv->vel_x, priv->vel_y);

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

  if (gtk_bin_get_child (GTK_BIN (widget)) == NULL)
    return TRUE;

  if ((!priv->enabled) || (!priv->clicked) ||
      ((event->time == priv->last_time) && (priv->last_type == 2))) {
    gdk_window_get_pointer (widget->window, NULL, NULL, 0);
    return TRUE;
  }

  if (priv->last_type == 1) {
    priv->first_drag = TRUE;
  }

  /* Only start the scroll if the mouse cursor passes beyond the
   * DnD threshold for dragging.
   */
  g_object_get (G_OBJECT (gtk_settings_get_default ()),
		"gtk-dnd-drag-threshold", &dnd_threshold, NULL);
  x = event->x - priv->x;
  y = event->y - priv->y;

  if (priv->first_drag && (!priv->moved) &&
      ((ABS (x) > (dnd_threshold+DND_THRESHOLD_INC))
       || (ABS (y) > (dnd_threshold+DND_THRESHOLD_INC)))) {
    priv->moved = TRUE;
    x = 0;
    y = 0;

    if (priv->first_drag) {

      if (ABS (priv->click_y - event->y) >=
          ABS (priv->click_x - event->x)) {
        gboolean vscroll;

        g_signal_emit (area,
                       pannable_area_signals[VERTICAL_MOVEMENT],
                       0, (priv->click_y > event->y) ?
                       HILDON_PANNABLE_AREA_MOV_UP :
                       HILDON_PANNABLE_AREA_MOV_DOWN,
                       priv->click_x, priv->click_y);

        vscroll = (priv->vadjust->upper - priv->vadjust->lower >
                   priv->vadjust->page_size) ? TRUE : FALSE;

        if (!((vscroll)&&
              (priv->mov_mode&HILDON_PANNABLE_AREA_MOV_MODE_VERT)))
          priv->moved = FALSE;

      } else {
        gboolean hscroll;

        g_signal_emit (area,
                       pannable_area_signals[HORIZONTAL_MOVEMENT],
                       0, (priv->click_x > event->x) ?
                       HILDON_PANNABLE_AREA_MOV_LEFT :
                       HILDON_PANNABLE_AREA_MOV_RIGHT,
                       priv->click_x, priv->click_y);

        hscroll = (priv->hadjust->upper - priv->hadjust->lower >
                   priv->hadjust->page_size) ? TRUE : FALSE;

        if (!((hscroll)&&
              (priv->mov_mode&HILDON_PANNABLE_AREA_MOV_MODE_HORI)))
          priv->moved = FALSE;
      }
    }

    priv->first_drag = FALSE;

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
      hildon_pannable_area_scroll (area, x, y);
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

      if (priv->mov_mode&HILDON_PANNABLE_AREA_MOV_MODE_HORI) {
        rawvel_x = (((event->x - priv->x) / ABS (delta)) *
                    (gdouble) priv->sps) * FORCE;
        /* we store the direction and after the calculation we
           change it, this reduces the ifs for the calculation */
        direction_x = rawvel_x < 0 ? -1 : 1;
        rawvel_x = ABS (rawvel_x);
        priv->vel_x = priv->vel_x * (1 - SMOOTH_FACTOR) +
          direction_x * rawvel_x * SMOOTH_FACTOR;
        priv->vel_x = priv->vel_x > 0 ? MIN (priv->vel_x, priv->vmax)
          : MAX (priv->vel_x, -1 * priv->vmax);
      } else {
        x = 0;
        priv->vel_x = 0;
      }

      if (priv->mov_mode&HILDON_PANNABLE_AREA_MOV_MODE_VERT) {
        rawvel_y = (((event->y - priv->y) / ABS (delta)) *
                    (gdouble) priv->sps) * FORCE;
        direction_y = rawvel_y < 0 ? -1 : 1;
        rawvel_y = ABS (rawvel_y);
        priv->vel_y = priv->vel_y * (1 - SMOOTH_FACTOR) +
          direction_y * rawvel_y * SMOOTH_FACTOR;
        priv->vel_y = priv->vel_y > 0 ? MIN (priv->vel_y, priv->vmax)
          : MAX (priv->vel_y, -1 * priv->vmax);
      } else {
        y = 0;
        priv->vel_y = 0;
      }

      hildon_pannable_area_scroll (area, x, y);

      if (priv->mov_mode&HILDON_PANNABLE_AREA_MOV_MODE_HORI)
        priv->x = event->x;
      if (priv->mov_mode&HILDON_PANNABLE_AREA_MOV_MODE_VERT)
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

  if (gtk_bin_get_child (GTK_BIN (widget)) == NULL)
    return TRUE;

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

  if (priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO ||
      priv->mode == HILDON_PANNABLE_AREA_MODE_ACCEL) {
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
    hildon_pannable_area_get_topmost (gtk_bin_get_child (GTK_BIN (widget))->window,
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
  gfloat y, height;
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

  /* Check the max y position */
  y = MIN (y, widget->allocation.height -
           (priv->hscroll ? priv->hscroll_rect.height : 0) -
           height);

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
  gfloat x, width;
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

  /* Check the max x position */
  x = MIN (x, widget->allocation.width -
           (priv->vscroll ? priv->vscroll_rect.width : 0) -
           width);

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
  GdkColor back_color = widget->style->bg[GTK_STATE_NORMAL];
  GdkColor scroll_color = widget->style->base[GTK_STATE_SELECTED];

  if (gtk_bin_get_child (GTK_BIN (widget))) {

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
      gint overshot_height;

      overshot_height = MIN (priv->overshot_dist_y, widget->allocation.height -
                             (priv->hscroll ? priv->hscroll_rect.height : 0));

      gdk_draw_rectangle (widget->window,
                          widget->style->bg_gc[GTK_STATE_NORMAL],
                          TRUE,
                          widget->allocation.x,
                          widget->allocation.y,
                          widget->allocation.width -
                          (priv->vscroll ? priv->vscroll_rect.width : 0),
                          overshot_height);
    } else if (priv->overshot_dist_y < 0) {
      gint overshot_height;
      gint overshot_y;

      overshot_height =
        MAX (priv->overshot_dist_y,
             -1*(widget->allocation.height -
                 (priv->hscroll ? priv->hscroll_rect.height : 0)));

      overshot_y = MAX (widget->allocation.y +
                        widget->allocation.height +
                        overshot_height -
                        (priv->hscroll ? priv->hscroll_rect.height : 0), 0);

      gdk_draw_rectangle (widget->window,
                          widget->style->bg_gc[GTK_STATE_NORMAL],
                          TRUE,
                          widget->allocation.x,
                          overshot_y,
                          widget->allocation.width -
                          priv->vscroll_rect.width,
                          -overshot_height);
    }

    if (priv->overshot_dist_x > 0) {
      gint overshot_width;

      overshot_width = MIN (priv->overshot_dist_x, widget->allocation.width -
                             (priv->vscroll ? priv->vscroll_rect.width : 0));

      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
			  widget->allocation.x,
			  widget->allocation.y,
                          overshot_width,
			  widget->allocation.height -
                          (priv->hscroll ? priv->hscroll_rect.height : 0));
    } else if (priv->overshot_dist_x < 0) {
      gint overshot_width;
      gint overshot_x;

      overshot_width =
        MAX (priv->overshot_dist_x,
             -1*(widget->allocation.width -
                 (priv->vscroll ? priv->vscroll_rect.width : 0)));

      overshot_x = MAX (widget->allocation.x +
                        widget->allocation.width +
                        overshot_width -
                        (priv->vscroll ? priv->vscroll_rect.width : 0), 0);

      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
                          overshot_x,
			  widget->allocation.y,
			  -overshot_width,
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
hildon_pannable_area_add (GtkContainer *container, GtkWidget *child)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (container);
  GtkBin *bin;

  bin = GTK_BIN (container);
  g_return_if_fail (bin->child == NULL);

  bin->child = child;
  gtk_widget_set_parent (child, GTK_WIDGET (bin));

  if (!gtk_widget_set_scroll_adjustments (child, priv->hadjust, priv->vadjust)) {
    g_warning ("%s: cannot add non scrollable widget, "
               "wrap it in a viewport", __FUNCTION__);
  }
}

static void
hildon_pannable_area_remove (GtkContainer *container, GtkWidget *child)
{
  g_return_if_fail (HILDON_IS_PANNABLE_AREA (container));
  g_return_if_fail (child != NULL);
  g_return_if_fail (gtk_bin_get_child (GTK_BIN (container)) == child);

  gtk_widget_set_scroll_adjustments (child, NULL, NULL);

  /* chain parent class handler to remove child */
  GTK_CONTAINER_CLASS (hildon_pannable_area_parent_class)->remove (container, child);
}

static void
hildon_pannable_calculate_vel_factor (HildonPannableArea * self)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (self);
  gfloat fct = 0;
  gfloat fct_i = 1;
  gint i, n;

  n = ceil (priv->sps * priv->scroll_time);

  for (i = 0; i < n && fct_i >= RATIO_TOLERANCE; i++) {
    fct_i *= priv->decel;
    fct += fct_i;
  }

    priv->vel_factor = fct;
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
  case PROP_MOV_MODE:
    g_value_set_flags (value, priv->mov_mode);
    break;
  case PROP_VELOCITY_MIN:
    g_value_set_double (value, priv->vmin);
    break;
  case PROP_VELOCITY_MAX:
    g_value_set_double (value, priv->vmax);
    break;
  case PROP_VELOCITY_FAST_FACTOR:
    g_value_set_double (value, priv->vfast_factor);
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
  case PROP_VOVERSHOOT_MAX:
    g_value_set_int (value, priv->vovershoot_max);
    break;
  case PROP_HOVERSHOOT_MAX:
    g_value_set_int (value, priv->hovershoot_max);
    break;
  case PROP_SCROLL_TIME:
    g_value_set_double (value, priv->scroll_time);
    break;
  case PROP_INITIAL_HINT:
    g_value_set_boolean (value, priv->initial_hint);
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
  case PROP_MOV_MODE:
    priv->mov_mode = g_value_get_flags (value);
    break;
  case PROP_VELOCITY_MIN:
    priv->vmin = g_value_get_double (value);
    break;
  case PROP_VELOCITY_MAX:
    priv->vmax = g_value_get_double (value);
    break;
  case PROP_VELOCITY_FAST_FACTOR:
    priv->vfast_factor = g_value_get_double (value);
    break;
  case PROP_DECELERATION:
    hildon_pannable_calculate_vel_factor (HILDON_PANNABLE_AREA (object));

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
  case PROP_VOVERSHOOT_MAX:
    priv->vovershoot_max = g_value_get_int (value);
    break;
  case PROP_HOVERSHOOT_MAX:
    priv->hovershoot_max = g_value_get_int (value);
    break;
  case PROP_SCROLL_TIME:
    priv->scroll_time = g_value_get_double (value);

    hildon_pannable_calculate_vel_factor (HILDON_PANNABLE_AREA (object));
    break;
  case PROP_INITIAL_HINT:
    priv->initial_hint = g_value_get_boolean (value);
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

  if (priv->scroll_indicator_timeout){
    g_source_remove (priv->scroll_indicator_timeout);
    priv->scroll_indicator_timeout = 0;
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

  priv = PANNABLE_AREA_PRIVATE (widget);

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  border_width = GTK_CONTAINER (widget)->border_width;

  attributes.x = widget->allocation.x + border_width;
  attributes.y = widget->allocation.y + border_width;
  attributes.width = MAX (widget->allocation.width - 2 * border_width -
                          (priv->vscroll ? priv->vscroll_rect.width : 0), 0);
  attributes.height = MAX (widget->allocation.height - 2 * border_width -
                           (priv->hscroll ? priv->hscroll_rect.height : 0), 0);
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget)
    | GDK_BUTTON_MOTION_MASK
    | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK
    | GDK_EXPOSURE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;

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
  gboolean hscroll, vscroll;

  priv = PANNABLE_AREA_PRIVATE (widget);

  if (priv->event_window != NULL && !priv->enabled)
    gdk_window_show (priv->event_window);

  (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->map) (widget);

  if (priv->event_window != NULL && priv->enabled)
    gdk_window_show (priv->event_window);

  if (priv->initial_hint) {
    if (((priv->vovershoot_max != 0)||(priv->hovershoot_max != 0)) &&
        ((priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO) ||
         (priv->mode == HILDON_PANNABLE_AREA_MODE_ACCEL))) {
      vscroll = (priv->vadjust->upper - priv->vadjust->lower >
                 priv->vadjust->page_size) ? TRUE : FALSE;
      hscroll = (priv->hadjust->upper - priv->hadjust->lower >
                 priv->hadjust->page_size) ? TRUE : FALSE;
      /* If scrolling is possible in both axes, only hint about scrolling in
         the vertical one. */
      if ((vscroll)&&(priv->vovershoot_max != 0)) {
        priv->overshot_dist_y = priv->vovershoot_max;
        priv->vel_y = priv->vmax * 0.1;
      } else if ((hscroll)&&(priv->hovershoot_max != 0)) {
        priv->overshot_dist_x = priv->hovershoot_max;
        priv->vel_x = priv->vmax * 0.1;
      }

      if (vscroll || hscroll) {
        priv->idle_id = g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
                                       (GSourceFunc)
                                       hildon_pannable_area_timeout, widget);
      }
    }

    if (priv->vscroll || priv->hscroll) {
      priv->scroll_indicator_alpha = 1;

      priv->scroll_indicator_timeout =
        g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
                       (GSourceFunc) hildon_pannable_area_scroll_indicator_fade,
                       widget);
    }
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

  hildon_pannable_area_refresh (HILDON_PANNABLE_AREA (widget));

  priv = PANNABLE_AREA_PRIVATE (widget);

  child_allocation.x = allocation->x + GTK_CONTAINER (widget)->border_width;
  child_allocation.y = allocation->y + GTK_CONTAINER (widget)->border_width;
  child_allocation.width = MAX (allocation->width -
				GTK_CONTAINER (widget)->border_width * 2 -
                                (priv->vscroll ? priv->vscroll_rect.width : 0), 0);
  child_allocation.height = MAX (allocation->height -
				 GTK_CONTAINER (widget)->border_width * 2 -
                                 (priv->hscroll ? priv->hscroll_rect.height : 0), 0);

  if (GTK_WIDGET_REALIZED (widget)) {
    if (priv->event_window != NULL)
      gdk_window_move_resize (priv->event_window,
			      child_allocation.x,
			      child_allocation.y,
			      child_allocation.width,
			      child_allocation.height);
  }

  if (priv->overshot_dist_y > 0) {
    child_allocation.y = MIN (child_allocation.y + priv->overshot_dist_y,
                              allocation->y + child_allocation.height);
    child_allocation.height = MAX (child_allocation.height - priv->overshot_dist_y, 0);
  } else if (priv->overshot_dist_y < 0) {
    child_allocation.height = MAX (child_allocation.height + priv->overshot_dist_y, 0);
  }

  if (priv->overshot_dist_x > 0) {
    child_allocation.x = MIN (child_allocation.x + priv->overshot_dist_x,
                              allocation->x + child_allocation.width);
    child_allocation.width = MAX (child_allocation.width - priv->overshot_dist_x, 0);
  } else if (priv->overshot_dist_x < 0) {
    child_allocation.width = MAX (child_allocation.width + priv->overshot_dist_x, 0);
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
  container_class->remove = hildon_pannable_area_remove;

  klass->horizontal_movement = NULL;
  klass->vertical_movement = NULL;

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
				   PROP_MOV_MODE,
				   g_param_spec_flags ("mov_mode",
                                                       "Scroll movement mode",
                                                       "Controls if the widget can scroll vertically, horizontally or both",
                                                       HILDON_TYPE_PANNABLE_AREA_MOV_MODE,
                                                       HILDON_PANNABLE_AREA_MOV_MODE_BOTH,
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
				   PROP_VELOCITY_FAST_FACTOR,
				   g_param_spec_double ("velocity_fast_factor",
							"Fast velocity factor",
							"Minimum velocity that is considered 'fast': "
							"children widgets won't receive button presses. "
							"Expressed as a fraction of the maximum velocity.",
							0, 1, 0.02,
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
				   PROP_VOVERSHOOT_MAX,
				   g_param_spec_int ("vovershoot_max",
                                                     "Vertical overshoot distance",
                                                     "Space we allow the widget to pass over its vertical limits when hitting the edges, set 0 in order to deactivate overshooting.",
                                                     0, G_MAXINT, 150,
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_HOVERSHOOT_MAX,
				   g_param_spec_int ("hovershoot_max",
                                                     "Horizontal overshoot distance",
                                                     "Space we allow the widget to pass over its horizontal limits when hitting the edges, set 0 in order to deactivate overshooting.",
                                                     0, G_MAXINT, 150,
                                                     G_PARAM_READWRITE |
                                                     G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_SCROLL_TIME,
				   g_param_spec_double ("scroll_time",
							"Time to scroll to a position",
							"The time to scroll to a position when calling the hildon_pannable_scroll_to function"
							"acceleration scrolling mode.",
							1.0, 20.0, 10.0,
							G_PARAM_READWRITE |
							G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
 				   PROP_INITIAL_HINT,
 				   g_param_spec_boolean ("initial-hint",
 							 "Initial hint",
 							 "Whether to hint the user about the pannability of the container.",
 							 TRUE,
							 G_PARAM_READWRITE |
 							 G_PARAM_CONSTRUCT));

  gtk_widget_class_install_style_property (widget_class,
					   g_param_spec_uint
					   ("indicator-width",
					    "Width of the scroll indicators",
					    "Pixel width used to draw the scroll indicators.",
					    0, G_MAXUINT, 8,
					    G_PARAM_READWRITE));

  pannable_area_signals[HORIZONTAL_MOVEMENT] =
    g_signal_new ("horizontal_movement",
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (HildonPannableAreaClass, horizontal_movement),
		  NULL, NULL,
		  _hildon_marshal_VOID__INT_DOUBLE_DOUBLE,
		  G_TYPE_NONE, 3,
                  G_TYPE_INT,
		  G_TYPE_DOUBLE,
		  G_TYPE_DOUBLE);

  pannable_area_signals[VERTICAL_MOVEMENT] =
    g_signal_new ("vertical_movement",
		  G_TYPE_FROM_CLASS (object_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (HildonPannableAreaClass, vertical_movement),
		  NULL, NULL,
		  _hildon_marshal_VOID__INT_DOUBLE_DOUBLE,
		  G_TYPE_NONE, 3,
                  G_TYPE_INT,
		  G_TYPE_DOUBLE,
		  G_TYPE_DOUBLE);


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
  priv->vel_x = 0;
  priv->vel_y = 0;
  priv->scroll_indicator_alpha = 0;
  priv->scroll_indicator_timeout = 0;
  priv->scroll_indicator_event_interrupt = 0;
  priv->scroll_delay_counter = 0;
  priv->scroll_to_x = -1;
  priv->scroll_to_y = -1;
  priv->first_drag = TRUE;

  hildon_pannable_calculate_vel_factor (self);

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

/**
 * hildon_pannable_area_scroll_to:
 * @area: A #HildonPannableArea.
 * @x: The x coordinate of the destination point or -1 to ignore this axis.
 * @y: The y coordinate of the destination point or -1 to ignore this axis.
 *
 * Smoothly scrolls @area to ensure that (@x, @y) is a visible point
 * on the widget. To move in only one coordinate, you must set the other one
 * to -1. Notice that, in %HILDON_PANNABLE_AREA_MODE_PUSH mode, this function
 * works just like hildon_pannable_area_jump_to().
 *
 * This function is useful if you need to present the user with a particular
 * element inside a scrollable widget, like #GtkTreeView. For instance,
 * the following example shows how to scroll inside a #GtkTreeView to
 * make visible an item, indicated by the #GtkTreeIter @iter.
 *
 * <informalexample><programlisting>
 *  GtkTreePath *path;
 *  GdkRectangle *rect;
 *
 *  path = gtk_tree_model_get_path (model, &iter);
 *  gtk_tree_view_get_background_area (GTK_TREE_VIEW (treeview),
 *                                     path, NULL, &rect);
 *  gtk_tree_view_convert_bin_window_to_tree_coords (GTK_TREE_VIEW (treeview),
 *                                                   0, rect.y, NULL, &y);
 *  hildon_pannable_area_scroll_to (panarea, -1, y);
 *  gtk_tree_path_free (path);
 * </programlisting></informalexample>
 *
 * If you want to present a child widget in simpler scenarios,
 * use hildon_pannable_area_scroll_to_child() instead.
 *
 **/
void
hildon_pannable_area_scroll_to (HildonPannableArea *area,
				const gint x, const gint y)
{
  HildonPannableAreaPrivate *priv;
  gint width, height;
  gint dist_x, dist_y;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));

  priv = PANNABLE_AREA_PRIVATE (area);

  if (priv->mode == HILDON_PANNABLE_AREA_MODE_PUSH)
    hildon_pannable_area_jump_to (area, x, y);

  g_return_if_fail (x >= -1 && y >= -1);

  if (x == -1 && y == -1) {
    return;
  }

  width = priv->hadjust->upper - priv->hadjust->lower;
  height = priv->vadjust->upper - priv->vadjust->lower;

  g_return_if_fail (x < width || y < height);

  if (x > -1) {
    priv->scroll_to_x = x - priv->hadjust->page_size/2;
    dist_x = priv->scroll_to_x - priv->hadjust->value;
    if (dist_x == 0) {
      priv->scroll_to_x = -1;
    } else {
      priv->vel_x = - dist_x/priv->vel_factor;
    }
  } else {
    priv->scroll_to_x = -1;
  }

  if (y > -1) {
    priv->scroll_to_y = y - priv->vadjust->page_size/2;
    dist_y = priv->scroll_to_y - priv->vadjust->value;
    if (dist_y == 0) {
      priv->scroll_to_y = -1;
    } else {
      priv->vel_y = - dist_y/priv->vel_factor;
    }
  } else {
    priv->scroll_to_y = y;
  }

  if ((priv->scroll_to_y == -1) && (priv->scroll_to_y == -1)) {
    return;
  }

  priv->scroll_indicator_alpha = 1.0;

  if (priv->scroll_indicator_timeout)
    g_source_remove (priv->scroll_indicator_timeout);
  priv->scroll_indicator_timeout = g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
                                                  (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, area);

  if (priv->idle_id)
    g_source_remove (priv->idle_id);
  priv->idle_id = g_timeout_add ((gint) (1000.0 / (gdouble) priv->sps),
                                 (GSourceFunc)
                                 hildon_pannable_area_timeout, area);
}

/**
 * hildon_pannable_area_jump_to:
 * @area: A #HildonPannableArea.
 * @x: The x coordinate of the destination point or -1 to ignore this axis.
 * @y: The y coordinate of the destination point or -1 to ignore this axis.
 *
 * Jumps the position of @area to ensure that (@x, @y) is a visible
 * point in the widget. In order to move in only one coordinate, you
 * must set the other one to -1. See hildon_pannable_area_scroll_to()
 * function for an example of how to calculate the position of
 * children in scrollable widgets like #GtkTreeview.
 *
 **/
void
hildon_pannable_area_jump_to (HildonPannableArea *area,
                              const gint x, const gint y)
{
  HildonPannableAreaPrivate *priv;
  gint width, height;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));
  g_return_if_fail (x >= -1 && y >= -1);

  if (x == -1 && y == -1) {
    return;
  }

  priv = PANNABLE_AREA_PRIVATE (area);

  width = priv->hadjust->upper - priv->hadjust->lower;
  height = priv->vadjust->upper - priv->vadjust->lower;

  g_return_if_fail (x < width || y < height);

  if (x != -1) {
    gdouble jump_to = x - priv->hadjust->page_size/2;

    if (jump_to > priv->hadjust->upper - priv->hadjust->page_size) {
      jump_to = priv->hadjust->upper - priv->hadjust->page_size;
    }

    gtk_adjustment_set_value (priv->hadjust, jump_to);
  }

  if (y != -1) {
    gdouble jump_to =  y - priv->vadjust->page_size/2;

    if (jump_to > priv->vadjust->upper - priv->vadjust->page_size) {
      jump_to = priv->vadjust->upper - priv->vadjust->page_size;
    }

    gtk_adjustment_set_value (priv->vadjust, jump_to);
  }

  priv->scroll_indicator_alpha = 1.0;

  if (priv->scroll_indicator_timeout) {

    priv->vel_x = 0.0;
    priv->vel_y = 0.0;
    priv->overshooting_x = 0;
    priv->overshooting_y = 0;

    if ((priv->overshot_dist_x>0)||(priv->overshot_dist_y>0)) {
      priv->overshot_dist_x = 0;
      priv->overshot_dist_y = 0;

      gtk_widget_queue_resize (GTK_WIDGET (area));
    }
    g_source_remove (priv->scroll_indicator_timeout);
    priv->scroll_indicator_timeout = 0;
  }

  if (priv->idle_id)
    g_source_remove (priv->idle_id);
  priv->idle_id = 0;
}

/**
 * hildon_pannable_area_scroll_to_child:
 * @area: A #HildonPannableArea.
 * @child: A #GtkWidget, descendant of @area.
 *
 * Smoothly scrolls until @child is visible inside @area. @child must
 * be a descendant of @area. If you need to scroll inside a scrollable
 * widget, e.g., #GtkTreeview, see hildon_pannable_area_scroll_to().
 *
 **/
void
hildon_pannable_area_scroll_to_child (HildonPannableArea *area, GtkWidget *child)
{
  GtkWidget *bin_child;
  gint x, y;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));
  g_return_if_fail (GTK_IS_WIDGET (child));
  g_return_if_fail (gtk_widget_is_ancestor (child, GTK_WIDGET (area)));

  if (GTK_BIN (area)->child == NULL)
    return;

  /* We need to get to check the child of the inside the area */
  bin_child = GTK_BIN (area)->child;

  /* we check if we added a viewport */
  if (GTK_IS_VIEWPORT (bin_child)) {
    bin_child = GTK_BIN (bin_child)->child;
  }

  if (gtk_widget_translate_coordinates (child, bin_child, 0, 0, &x, &y))
    hildon_pannable_area_scroll_to (area, x, y);
}

/**
 * hildon_pannable_area_jump_to_child:
 * @area: A #HildonPannableArea.
 * @child: A #GtkWidget, descendant of @area.
 *
 * Jumps to make sure @child is visible inside @area. @child must
 * be a descendant of @area. If you want to move inside a scrollable
 * widget, like, #GtkTreeview, see hildon_pannable_area_scroll_to().
 *
 **/
void
hildon_pannable_area_jump_to_child (HildonPannableArea *area, GtkWidget *child)
{
  GtkWidget *bin_child;
  gint x, y;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));
  g_return_if_fail (GTK_IS_WIDGET (child));
  g_return_if_fail (gtk_widget_is_ancestor (child, GTK_WIDGET (area)));

  if (gtk_bin_get_child (GTK_BIN (area)) == NULL)
    return;

  /* We need to get to check the child of the inside the area */
  bin_child = gtk_bin_get_child (GTK_BIN (area));

  /* we check if we added a viewport */
  if (GTK_IS_VIEWPORT (bin_child)) {
    bin_child = gtk_bin_get_child (GTK_BIN (bin_child));
  }

  if (gtk_widget_translate_coordinates (child, bin_child, 0, 0, &x, &y))
    hildon_pannable_area_jump_to (area, x, y);
}

/**
 * hildon_pannable_get_child_widget_at:
 * @area: A #HildonPannableArea.
 * @x: horizontal coordinate of the point
 * @y: vertical coordinate of the point
 *
 * Get the widget at the point (x, y) inside the pannable area. In
 * case no widget found it returns NULL.
 *
 * returns: the #GtkWidget if we find a widget, NULL in any other case
 **/
GtkWidget*
hildon_pannable_get_child_widget_at (HildonPannableArea *area,
                                     gdouble x, gdouble y)
{
  GdkWindow *window = NULL;
  GtkWidget *child_widget = NULL;

  window = hildon_pannable_area_get_topmost
    (gtk_bin_get_child (GTK_BIN (area))->window,
     x, y, NULL, NULL);

  gdk_window_get_user_data (window, (void**) &child_widget);

  return child_widget;
}
