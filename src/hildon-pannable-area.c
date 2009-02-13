/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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
 * #HildonPannableArea is a container widget that can be "panned" (scrolled)
 * up and down using the touchscreen with fingers. The widget has no scrollbars,
 * but it rather shows small scroll indicators to give an idea of the part of the
 * content that is visible at a time. The scroll indicators appear when a dragging
 * motion is started on the pannable area.
 *
 * The scrolling is "kinetic", meaning the motion can be "flicked" and it will
 * continue from the initial motion by gradually slowing down to an eventual stop.
 * The motion can also be stopped immediately by pressing the touchscreen over the
 * pannable area.
 */

#include <math.h>
#if USE_CAIRO_SCROLLBARS == 1
#include <cairo.h>
#endif
#include <gdk/gdkx.h>

#include "hildon-pannable-area.h"
#include "hildon-marshalers.h"
#include "hildon-enum-types.h"

#define USE_CAIRO_SCROLLBARS 0

#define SMOOTH_FACTOR 0.85
#define FORCE 5
#define BOUNCE_STEPS 6
#define SCROLL_BAR_MIN_SIZE 5
#define RATIO_TOLERANCE 0.000001
#define SCROLLBAR_FADE_DELAY 30
#define SCROLL_FADE_TIMEOUT 10
#define MOTION_EVENTS_PER_SECOND 25

G_DEFINE_TYPE (HildonPannableArea, hildon_pannable_area, GTK_TYPE_BIN)

#define PANNABLE_AREA_PRIVATE(o)                                \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_PANNABLE_AREA, \
                                HildonPannableAreaPrivate))

struct _HildonPannableAreaPrivate {
  HildonPannableAreaMode mode;
  HildonMovementMode mov_mode;
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
  gdouble motion_x;
  gdouble motion_y;
  gint overshot_dist_x;
  gint overshot_dist_y;
  gint overshooting_y;
  gint overshooting_x;
  gdouble scroll_indicator_alpha;
  gint motion_event_scroll_timeout;
  gint scroll_indicator_timeout;
  gint scroll_indicator_event_interrupt;
  gint scroll_delay_counter;
  gint vovershoot_max;
  gint hovershoot_max;
  gboolean initial_hint;
  gboolean initial_effect;
  gboolean first_drag;

  gboolean size_request_policy;
  gboolean hscroll_visible;
  gboolean vscroll_visible;
  GdkRectangle hscroll_rect;
  GdkRectangle vscroll_rect;
  guint area_width;

  GtkAdjustment *hadjust;
  GtkAdjustment *vadjust;

  GtkPolicyType vscrollbar_policy;
  GtkPolicyType hscrollbar_policy;
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
  PROP_MOVEMENT_MODE,
  PROP_VELOCITY_MIN,
  PROP_VELOCITY_MAX,
  PROP_VELOCITY_FAST_FACTOR,
  PROP_DECELERATION,
  PROP_SPS,
  PROP_VSCROLLBAR_POLICY,
  PROP_HSCROLLBAR_POLICY,
  PROP_VOVERSHOOT_MAX,
  PROP_HOVERSHOOT_MAX,
  PROP_SCROLL_TIME,
  PROP_INITIAL_HINT,
  PROP_SIZE_REQUEST_POLICY,
  PROP_HADJUSTMENT,
  PROP_VADJUSTMENT,
  PROP_LAST
};

static void hildon_pannable_area_class_init (HildonPannableAreaClass * klass);
static void hildon_pannable_area_init (HildonPannableArea * area);
static void hildon_pannable_area_get_property (GObject * object,
                                               guint property_id,
                                               GValue * value,
                                               GParamSpec * pspec);
static void hildon_pannable_area_set_property (GObject * object,
                                               guint property_id,
                                               const GValue * value,
                                               GParamSpec * pspec);
static void hildon_pannable_area_dispose (GObject * object);
static void hildon_pannable_area_realize (GtkWidget * widget);
static void hildon_pannable_area_unrealize (GtkWidget * widget);
static void hildon_pannable_area_size_request (GtkWidget * widget,
                                               GtkRequisition * requisition);
static void hildon_pannable_area_size_allocate (GtkWidget * widget,
                                                GtkAllocation * allocation);
static void hildon_pannable_area_style_set (GtkWidget * widget,
                                            GtkStyle * previous_style);
static void hildon_pannable_area_map (GtkWidget * widget);
static void hildon_pannable_area_unmap (GtkWidget * widget);
static void hildon_pannable_area_grab_notify (GtkWidget *widget,
                                              gboolean was_grabbed,
                                              gpointer user_data);
#if USE_CAIRO_SCROLLBARS == 1
static void rgb_from_gdkcolor (GdkColor *color, gdouble *r, gdouble *g, gdouble *b);
#else /* USE_CAIRO_SCROLLBARS */
static void tranparency_color (GdkColor *color,
                               GdkColor colora,
                               GdkColor colorb,
                               gdouble transparency);
#endif /* USE_CAIRO_SCROLLBARS */
static void hildon_pannable_draw_vscroll (GtkWidget * widget,
                                          GdkColor *back_color,
                                          GdkColor *scroll_color);
static void hildon_pannable_draw_hscroll (GtkWidget * widget,
                                          GdkColor *back_color,
                                          GdkColor *scroll_color);
static void hildon_pannable_area_initial_effect (GtkWidget * widget);
static void hildon_pannable_area_redraw (HildonPannableArea * area);
static gboolean hildon_pannable_area_scroll_indicator_fade(HildonPannableArea * area);
static gboolean hildon_pannable_area_expose_event (GtkWidget * widget,
                                                   GdkEventExpose * event);
static GdkWindow * hildon_pannable_area_get_topmost (GdkWindow * window,
                                                     gint x, gint y,
                                                     gint * tx, gint * ty,
                                                     GdkEventMask mask);
static void synth_crossing (GdkWindow * child,
                            gint x, gint y,
                            gint x_root, gint y_root,
                            guint32 time, gboolean in);
static gboolean hildon_pannable_area_button_press_cb (GtkWidget * widget,
                                                      GdkEventButton * event);
static void hildon_pannable_area_refresh (HildonPannableArea * area);
static void hildon_pannable_axis_scroll (HildonPannableArea *area,
                                         GtkAdjustment *adjust,
                                         gdouble *vel,
                                         gdouble inc,
                                         gint *overshooting,
                                         gint *overshot_dist,
                                         gdouble *scroll_to,
                                         gint overshoot_max,
                                         gboolean *s);
static void hildon_pannable_area_scroll (HildonPannableArea *area,
                                         gdouble x, gdouble y);
static gboolean hildon_pannable_area_timeout (HildonPannableArea * area);
static void hildon_pannable_area_calculate_velocity (gdouble *vel,
                                                     gdouble delta,
                                                     gdouble dist,
                                                     gdouble vmax,
                                                     guint sps);
static gboolean hildon_pannable_area_motion_event_scroll_timeout (HildonPannableArea *area);
static void hildon_pannable_area_motion_event_scroll (HildonPannableArea *area,
                                                      gdouble x, gdouble y);
static gboolean hildon_pannable_area_motion_notify_cb (GtkWidget * widget,
                                                       GdkEventMotion * event);
static gboolean hildon_pannable_area_button_release_cb (GtkWidget * widget,
                                                        GdkEventButton * event);
static gboolean hildon_pannable_area_scroll_cb (GtkWidget *widget,
                                                GdkEventScroll *event);
static void hildon_pannable_area_child_mapped (GtkWidget *widget,
                                               GdkEvent  *event,
                                               gpointer user_data);
static void hildon_pannable_area_add (GtkContainer *container, GtkWidget *child);
static void hildon_pannable_area_remove (GtkContainer *container, GtkWidget *child);
static void hildon_pannable_calculate_vel_factor (HildonPannableArea * self);


static void
hildon_pannable_area_class_init (HildonPannableAreaClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);


  g_type_class_add_private (klass, sizeof (HildonPannableAreaPrivate));

  object_class->get_property = hildon_pannable_area_get_property;
  object_class->set_property = hildon_pannable_area_set_property;
  object_class->dispose = hildon_pannable_area_dispose;

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
  widget_class->scroll_event = hildon_pannable_area_scroll_cb;

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
				   PROP_VSCROLLBAR_POLICY,
				   g_param_spec_enum ("vscrollbar_policy",
						      "vscrollbar policy",
						      "Visual policy of the vertical scrollbar",
						      GTK_TYPE_POLICY_TYPE,
						      GTK_POLICY_AUTOMATIC,
						      G_PARAM_READWRITE |
						      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_HSCROLLBAR_POLICY,
				   g_param_spec_enum ("hscrollbar_policy",
						      "hscrollbar policy",
						      "Visual policy of the horizontal scrollbar",
						      GTK_TYPE_POLICY_TYPE,
						      GTK_POLICY_AUTOMATIC,
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
				   PROP_MOVEMENT_MODE,
				   g_param_spec_flags ("mov_mode",
                                                       "Scroll movement mode",
                                                       "Controls if the widget can scroll vertically, horizontally or both",
                                                       HILDON_TYPE_MOVEMENT_MODE,
                                                       HILDON_MOVEMENT_MODE_BOTH,
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
							0, G_MAXDOUBLE, 60,
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
							0, 1.0, 0.90,
							G_PARAM_READWRITE |
							G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_SPS,
				   g_param_spec_uint ("sps",
						      "Scrolls per second",
						      "Amount of scroll events to generate per second.",
						      0, G_MAXUINT, 15,
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
 							 FALSE,
							 G_PARAM_READWRITE |
 							 G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_SIZE_REQUEST_POLICY,
				   g_param_spec_enum ("size-request-policy",
                                                      "Size Requisition policy",
                                                      "Controls the size request policy of the widget",
                                                      HILDON_TYPE_SIZE_REQUEST_POLICY,
                                                      HILDON_SIZE_REQUEST_MINIMUM,
                                                      G_PARAM_READWRITE|
                                                      G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
				   PROP_HADJUSTMENT,
				   g_param_spec_object ("hadjustment",
							"Horizontal Adjustment",
							"The GtkAdjustment for the horizontal position",
							GTK_TYPE_ADJUSTMENT,
							G_PARAM_READABLE));
  g_object_class_install_property (object_class,
				   PROP_VADJUSTMENT,
				   g_param_spec_object ("vadjustment",
							"Vertical Adjustment",
							"The GtkAdjustment for the vertical position",
							GTK_TYPE_ADJUSTMENT,
							G_PARAM_READABLE));

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
hildon_pannable_area_init (HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = PANNABLE_AREA_PRIVATE (area);

  GTK_WIDGET_UNSET_FLAGS (area, GTK_NO_WINDOW);

  area->priv = priv;

  priv->moved = FALSE;
  priv->clicked = FALSE;
  priv->last_time = 0;
  priv->last_type = 0;
  priv->vscroll_visible = TRUE;
  priv->hscroll_visible = TRUE;
  priv->area_width = 6;
  priv->overshot_dist_x = 0;
  priv->overshot_dist_y = 0;
  priv->overshooting_y = 0;
  priv->overshooting_x = 0;
  priv->idle_id = 0;
  priv->vel_x = 0;
  priv->vel_y = 0;
  priv->scroll_indicator_alpha = 0.0;
  priv->scroll_indicator_timeout = 0;
  priv->motion_event_scroll_timeout = 0;
  priv->scroll_indicator_event_interrupt = 0;
  priv->scroll_delay_counter = SCROLLBAR_FADE_DELAY;
  priv->scroll_to_x = -1;
  priv->scroll_to_y = -1;
  priv->first_drag = TRUE;
  priv->initial_effect = TRUE;

  hildon_pannable_calculate_vel_factor (area);

  gtk_widget_add_events (GTK_WIDGET (area), GDK_POINTER_MOTION_HINT_MASK);

  priv->hadjust =
    GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
  priv->vadjust =
    GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

  g_object_ref_sink (G_OBJECT (priv->hadjust));
  g_object_ref_sink (G_OBJECT (priv->vadjust));

  g_signal_connect_swapped (G_OBJECT (priv->hadjust), "value-changed",
			    G_CALLBACK (hildon_pannable_area_redraw), area);
  g_signal_connect_swapped (G_OBJECT (priv->vadjust), "value-changed",
			    G_CALLBACK (hildon_pannable_area_redraw), area);
  g_signal_connect_swapped (G_OBJECT (priv->hadjust), "changed",
			    G_CALLBACK (hildon_pannable_area_refresh), area);
  g_signal_connect_swapped (G_OBJECT (priv->vadjust), "changed",
			    G_CALLBACK (hildon_pannable_area_refresh), area);
  g_signal_connect (G_OBJECT (area), "grab-notify",
                    G_CALLBACK (hildon_pannable_area_grab_notify), NULL);
}

static void
hildon_pannable_area_get_property (GObject * object,
                                   guint property_id,
				   GValue * value,
                                   GParamSpec * pspec)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (object)->priv;

  switch (property_id) {
  case PROP_ENABLED:
    g_value_set_boolean (value, priv->enabled);
    break;
  case PROP_MODE:
    g_value_set_enum (value, priv->mode);
    break;
  case PROP_MOVEMENT_MODE:
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
  case PROP_VSCROLLBAR_POLICY:
    g_value_set_enum (value, priv->vscrollbar_policy);
    break;
  case PROP_HSCROLLBAR_POLICY:
    g_value_set_enum (value, priv->hscrollbar_policy);
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
  case PROP_SIZE_REQUEST_POLICY:
    g_value_set_enum (value, priv->size_request_policy);
    break;
  case PROP_HADJUSTMENT:
    g_value_set_object (value,
                        hildon_pannable_area_get_hadjustment
                        (HILDON_PANNABLE_AREA (object)));
    break;
  case PROP_VADJUSTMENT:
    g_value_set_object (value,
                        hildon_pannable_area_get_vadjustment
                        (HILDON_PANNABLE_AREA (object)));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_pannable_area_set_property (GObject * object,
                                   guint property_id,
				   const GValue * value,
                                   GParamSpec * pspec)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (object)->priv;
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
  case PROP_MOVEMENT_MODE:
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
  case PROP_VSCROLLBAR_POLICY:
    priv->vscrollbar_policy = g_value_get_enum (value);

    gtk_widget_queue_resize (GTK_WIDGET (object));
    break;
  case PROP_HSCROLLBAR_POLICY:
    priv->hscrollbar_policy = g_value_get_enum (value);

    gtk_widget_queue_resize (GTK_WIDGET (object));
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
  case PROP_SIZE_REQUEST_POLICY:
    hildon_pannable_area_set_size_request_policy (HILDON_PANNABLE_AREA (object),
                                                  g_value_get_enum (value));
    break;

  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_pannable_area_dispose (GObject * object)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (object)->priv;
  GtkWidget *child = gtk_bin_get_child (GTK_BIN (object));

  if (priv->idle_id) {
    g_source_remove (priv->idle_id);
    priv->idle_id = 0;
  }

  if (priv->scroll_indicator_timeout){
    g_source_remove (priv->scroll_indicator_timeout);
    priv->scroll_indicator_timeout = 0;
  }

  if (priv->motion_event_scroll_timeout){
    g_source_remove (priv->motion_event_scroll_timeout);
    priv->motion_event_scroll_timeout = 0;
  }

  if (priv->hadjust) {
    g_object_unref (priv->hadjust);
    priv->hadjust = NULL;
  }
  if (priv->vadjust) {
    g_object_unref (priv->vadjust);
    priv->vadjust = NULL;
  }

  if (child) {
    g_signal_handlers_disconnect_by_func (GTK_WIDGET (child),
                                          G_CALLBACK (hildon_pannable_area_child_mapped),
                                          object);
  }

  if (G_OBJECT_CLASS (hildon_pannable_area_parent_class)->dispose)
    G_OBJECT_CLASS (hildon_pannable_area_parent_class)->dispose (object);
}

static void
hildon_pannable_area_realize (GtkWidget * widget)
{
  GdkWindowAttr attributes;
  gint attributes_mask;
  gint border_width;
  HildonPannableAreaPrivate *priv;

  priv = HILDON_PANNABLE_AREA (widget)->priv;

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  border_width = GTK_CONTAINER (widget)->border_width;

  attributes.x = widget->allocation.x + border_width;
  attributes.y = widget->allocation.x + border_width;
  attributes.width = MAX (widget->allocation.width - 2 * border_width, 0);
  attributes.height = MAX (widget->allocation.height - 2 * border_width, 0);
  attributes.window_type = GDK_WINDOW_CHILD;

  /* avoid using the hildon_window */
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
  attributes.wclass = GDK_INPUT_OUTPUT;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
                                   &attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  /* create the events window */
  attributes.event_mask = gtk_widget_get_events (widget)
    | GDK_BUTTON_MOTION_MASK
    | GDK_BUTTON_PRESS_MASK
    | GDK_BUTTON_RELEASE_MASK
    | GDK_SCROLL_MASK
    | GDK_EXPOSURE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;
  attributes.wclass = GDK_INPUT_ONLY;

  attributes_mask = GDK_WA_X | GDK_WA_Y;

  priv->event_window = gdk_window_new (widget->window,
				       &attributes, attributes_mask);
  gdk_window_set_user_data (priv->event_window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

static void
hildon_pannable_area_unrealize (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv;

  priv = HILDON_PANNABLE_AREA (widget)->priv;

  if (priv->event_window != NULL) {
    gdk_window_set_user_data (priv->event_window, NULL);
    gdk_window_destroy (priv->event_window);
    priv->event_window = NULL;
  }

  if (GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->unrealize)
    (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->unrealize)(widget);
}

static void
hildon_pannable_area_size_request (GtkWidget * widget,
				   GtkRequisition * requisition)
{
  GtkRequisition child_requisition = {0};
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
  GtkWidget *child = gtk_bin_get_child (GTK_BIN (widget));

  if (child && GTK_WIDGET_VISIBLE (child))
    {
      gtk_widget_size_request (child, &child_requisition);
    }

  if (priv->hscrollbar_policy == GTK_POLICY_NEVER) {
    requisition->width = child_requisition.width;
  } else {
    switch (priv->size_request_policy) {
      case HILDON_SIZE_REQUEST_CHILDREN:
        requisition->width = child_requisition.width;
        break;
      case HILDON_SIZE_REQUEST_MINIMUM:
      default:
        requisition->width = priv->area_width;
      }
  }

  if (priv->vscrollbar_policy == GTK_POLICY_NEVER) {
    requisition->height = child_requisition.height;
  } else {
    switch (priv->size_request_policy) {
      case HILDON_SIZE_REQUEST_CHILDREN:
        requisition->height = child_requisition.height;
        break;
      case HILDON_SIZE_REQUEST_MINIMUM:
      default:
        requisition->height = priv->area_width;
      }
  }

  requisition->width += 2 * GTK_CONTAINER (widget)->border_width;
  requisition->height += 2 * GTK_CONTAINER (widget)->border_width;
}

static void
hildon_pannable_area_size_allocate (GtkWidget * widget,
				    GtkAllocation * allocation)
{
  GtkAllocation child_allocation;
  HildonPannableAreaPrivate *priv;
  GtkWidget *child = gtk_bin_get_child (GTK_BIN (widget));
  gint border_width;

  border_width = GTK_CONTAINER (widget)->border_width;

  widget->allocation = *allocation;

  priv = HILDON_PANNABLE_AREA (widget)->priv;

  child_allocation.x = border_width;
  child_allocation.y = border_width;
  child_allocation.width = MAX (allocation->width - 2 * border_width -
                                (priv->vscroll_visible ? priv->vscroll_rect.width : 0), 0);
  child_allocation.height = MAX (allocation->height - 2 * border_width -
                                 (priv->hscroll_visible ? priv->hscroll_rect.height : 0), 0);

  if (GTK_WIDGET_REALIZED (widget)) {
      gdk_window_move_resize (widget->window,
			      allocation->x + border_width,
			      allocation->y  + border_width,
			      allocation->width  - border_width * 2,
			      allocation->height - border_width * 2);
      gdk_window_move_resize (priv->event_window,
			      0,
			      0,
			      allocation->width  - border_width * 2,
			      allocation->height - border_width * 2);
  }

  if (priv->overshot_dist_y > 0) {
    child_allocation.y = MIN (child_allocation.y + priv->overshot_dist_y,
                              child_allocation.height);
    child_allocation.height = MAX (child_allocation.height - priv->overshot_dist_y, 0);
  } else if (priv->overshot_dist_y < 0) {
    child_allocation.height = MAX (child_allocation.height + priv->overshot_dist_y, 0);
  }

  if (priv->overshot_dist_x > 0) {
    child_allocation.x = MIN (child_allocation.x + priv->overshot_dist_x,
                              child_allocation.width);
    child_allocation.width = MAX (child_allocation.width - priv->overshot_dist_x, 0);
  } else if (priv->overshot_dist_x < 0) {
    child_allocation.width = MAX (child_allocation.width + priv->overshot_dist_x, 0);
  }

  if (child)
    gtk_widget_size_allocate (child, &child_allocation);

  /* we have to do this after child size_allocate because page_size is
   * changed when we allocate the size of the children */
  if (priv->overshot_dist_y < 0) {
    gtk_adjustment_set_value (priv->vadjust, priv->vadjust->upper -
                              priv->vadjust->page_size);
  }

  if (priv->overshot_dist_x < 0) {
    gtk_adjustment_set_value (priv->hadjust, priv->hadjust->upper -
                              priv->hadjust->page_size);
  }

  hildon_pannable_area_refresh (HILDON_PANNABLE_AREA (widget));
}

static void
hildon_pannable_area_style_set (GtkWidget * widget,
                                GtkStyle * previous_style)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;

  GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->
    style_set (widget, previous_style);

  gtk_widget_style_get (widget, "indicator-width", &priv->area_width, NULL);
}

static void
hildon_pannable_area_map (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv;

  priv = HILDON_PANNABLE_AREA (widget)->priv;

  gdk_window_show (widget->window);

  if (priv->event_window != NULL && !priv->enabled)
    gdk_window_show (priv->event_window);

  (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->map) (widget);

  if (priv->event_window != NULL && priv->enabled)
    gdk_window_show (priv->event_window);
}

static void
hildon_pannable_area_unmap (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv;

  priv = HILDON_PANNABLE_AREA (widget)->priv;

  if (priv->event_window != NULL)
    gdk_window_hide (priv->event_window);

  gdk_window_hide (widget->window);

  (*GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->unmap) (widget);
}

static void
hildon_pannable_area_grab_notify (GtkWidget *widget,
                                  gboolean was_grabbed,
                                  gpointer user_data)
{
  /* an internal widget has grabbed the focus and now has returned it,
     we have to do some release actions */
  if (was_grabbed) {
    HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;

    priv->scroll_indicator_event_interrupt = 0;

    if ((!priv->scroll_indicator_timeout)&&(priv->scroll_indicator_alpha)>0.1) {
      priv->scroll_delay_counter = SCROLLBAR_FADE_DELAY;

      priv->scroll_indicator_timeout = gdk_threads_add_timeout
        ((gint) (1000.0 / (gdouble) SCROLL_FADE_TIMEOUT),
         (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, widget);
    }

    priv->last_type = 3;
    priv->moved = FALSE;
  }
}

#if USE_CAIRO_SCROLLBARS == 1

static void
rgb_from_gdkcolor (GdkColor *color, gdouble *r, gdouble *g, gdouble *b)
{
  *r = (color->red >> 8) / 255.0;
  *g = (color->green >> 8) / 255.0;
  *b = (color->blue >> 8) / 255.0;
}

static void
hildon_pannable_draw_vscroll (GtkWidget * widget,
                              GdkColor *back_color,
                              GdkColor *scroll_color)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
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
  y = (priv->vadjust->value / priv->vadjust->upper) *
    (widget->allocation.height -
     (priv->hscroll_visible ? priv->area_width : 0));
  height = (((priv->vadjust->value +
              priv->vadjust->page_size) /
             priv->vadjust->upper) *
            (widget->allocation.height -
             (priv->hscroll_visible ? priv->area_width : 0))) - y;

  /* Set a minimum height */
  height = MAX (SCROLL_BAR_MIN_SIZE, height);

  /* Check the max y position */
  y = MIN (y, widget->allocation.height -
           (priv->hscroll_visible ? priv->hscroll_rect.height : 0) -
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
hildon_pannable_draw_hscroll (GtkWidget * widget,
                              GdkColor *back_color,
                              GdkColor *scroll_color)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
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
  x = (priv->hadjust->value / priv->hadjust->upper) *
    (widget->allocation.width - (priv->vscroll_visible ? priv->area_width : 0));
  width =(((priv->hadjust->value +
            priv->hadjust->page_size) / priv->hadjust->upper) *
          (widget->allocation.width -
           (priv->vscroll_visible ? priv->area_width : 0))) - x;

  /* Set a minimum width */
  width = MAX (SCROLL_BAR_MIN_SIZE, width);

  /* Check the max x position */
  x = MIN (x, widget->allocation.width -
           (priv->vscroll_visible ? priv->vscroll_rect.width : 0) -
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

#else /* USE_CAIRO_SCROLLBARS */

static void
tranparency_color (GdkColor *color,
                   GdkColor colora,
                   GdkColor colorb,
                   gdouble transparency)
{
  gdouble diff;

  diff = colora.red - colorb.red;
  color->red = colora.red-diff*transparency;

  diff = colora.green - colorb.green;
  color->green = colora.green-diff*transparency;

  diff = colora.blue - colorb.blue;
  color->blue = colora.blue-diff*transparency;
}

static void
hildon_pannable_draw_vscroll (GtkWidget *widget,
                              GdkColor *back_color,
                              GdkColor *scroll_color)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
  gfloat y, height;
  GdkColor transp_color;

  gdk_draw_rectangle (widget->window,
                      widget->style->bg_gc[GTK_STATE_NORMAL],
                      TRUE,
                       priv->vscroll_rect.x, priv->vscroll_rect.y,
                      priv->vscroll_rect.width,
                      priv->vscroll_rect.height);

  y = (priv->vadjust->value / priv->vadjust->upper) *
    (widget->allocation.height - (priv->hscroll_visible ? priv->area_width : 0));
  height = (((priv->vadjust->value + priv->vadjust->page_size) /
             priv->vadjust->upper) *
            (widget->allocation.height -
             (priv->hscroll_visible ? priv->area_width : 0))) - y;

  /* Set a minimum height */
  height = MAX (SCROLL_BAR_MIN_SIZE, height);

  /* Check the max y position */
  y = MIN (y, widget->allocation.height -
           (priv->hscroll_visible ? priv->hscroll_rect.height : 0) -
           height);

  tranparency_color (&transp_color, *back_color, *scroll_color,
                     priv->scroll_indicator_alpha);

  gdk_gc_set_rgb_fg_color (widget->style->fg_gc[GTK_STATE_INSENSITIVE],
                           &transp_color);

  gdk_draw_rectangle (widget->window,
                      widget->style->fg_gc[GTK_STATE_INSENSITIVE],
                      TRUE, priv->vscroll_rect.x, y,
                      priv->vscroll_rect.width, height);
}

static void
hildon_pannable_draw_hscroll (GtkWidget *widget,
                              GdkColor *back_color,
                              GdkColor *scroll_color)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
  gfloat x, width;
  GdkColor transp_color;

  gdk_draw_rectangle (widget->window,
                      widget->style->bg_gc[GTK_STATE_INSENSITIVE],
                      TRUE,
                      priv->hscroll_rect.x, priv->hscroll_rect.y,
                      priv->hscroll_rect.width,
                      priv->hscroll_rect.height);

  /* calculate the scrollbar width and position */
  x = (priv->hadjust->value / priv->hadjust->upper) *
    (widget->allocation.width - (priv->vscroll_visible ? priv->area_width : 0));
  width =(((priv->hadjust->value +
            priv->hadjust->page_size) / priv->hadjust->upper) *
          (widget->allocation.width -
           (priv->vscroll_visible ? priv->area_width : 0))) - x;

  /* Set a minimum width */
  width = MAX (SCROLL_BAR_MIN_SIZE, width);

  /* Check the max x position */
  x = MIN (x, widget->allocation.width -
           (priv->vscroll_visible ? priv->vscroll_rect.width : 0) -
           width);

  tranparency_color (&transp_color, *back_color, *scroll_color,
                     priv->scroll_indicator_alpha);

  gdk_gc_set_rgb_fg_color (widget->style->fg_gc[GTK_STATE_INSENSITIVE],
                           &transp_color);

  gdk_draw_rectangle (widget->window,
                      widget->style->fg_gc[GTK_STATE_INSENSITIVE],
                      TRUE, x, priv->hscroll_rect.y, width,
                      priv->hscroll_rect.height);
}

#endif /* USE_CAIRO_SCROLLBARS */

static void
hildon_pannable_area_initial_effect (GtkWidget * widget)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
  gboolean hscroll_visible, vscroll_visible;

  if (priv->initial_hint) {
    if (((priv->vovershoot_max != 0)||(priv->hovershoot_max != 0)) &&
        ((priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO) ||
         (priv->mode == HILDON_PANNABLE_AREA_MODE_ACCEL))) {
      vscroll_visible = (priv->vadjust->upper - priv->vadjust->lower >
                 priv->vadjust->page_size);
      hscroll_visible = (priv->hadjust->upper - priv->hadjust->lower >
                 priv->hadjust->page_size);
      /* If scrolling is possible in both axes, only hint about scrolling in
         the vertical one. */
      if ((vscroll_visible)&&(priv->vovershoot_max != 0)) {
        priv->overshot_dist_y = priv->vovershoot_max;
        priv->vel_y = priv->vmax * 0.1;
      } else if ((hscroll_visible)&&(priv->hovershoot_max != 0)) {
        priv->overshot_dist_x = priv->hovershoot_max;
        priv->vel_x = priv->vmax * 0.1;
      }

      if (vscroll_visible || hscroll_visible) {
        if (!priv->idle_id)
          priv->idle_id = gdk_threads_add_timeout ((gint) (1000.0 / (gdouble) priv->sps),
                                                   (GSourceFunc)
                                                   hildon_pannable_area_timeout, widget);
      }
    }

    if (priv->vscroll_visible || priv->hscroll_visible) {
      priv->scroll_indicator_alpha = 1.0;

      if (!priv->scroll_indicator_timeout)
        priv->scroll_indicator_timeout =
          gdk_threads_add_timeout ((gint) (1000.0 / (gdouble) SCROLL_FADE_TIMEOUT),
                                   (GSourceFunc) hildon_pannable_area_scroll_indicator_fade,
                                   widget);
    }
  }
}

static void
hildon_pannable_area_redraw (HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (area)->priv;

  hildon_pannable_area_refresh (area);

  /* Redraw scroll indicators */
  if (GTK_WIDGET_DRAWABLE (area)) {
      if (priv->hscroll_visible) {
        gdk_window_invalidate_rect (GTK_WIDGET (area)->window,
                                    &priv->hscroll_rect, FALSE);
      }

      if (priv->vscroll_visible) {
        gdk_window_invalidate_rect (GTK_WIDGET (area)->window,
                                    &priv->vscroll_rect, FALSE);
      }
  }
}

static gboolean
hildon_pannable_area_scroll_indicator_fade(HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = area->priv;

  /* if moving do not fade out */
  if (((ABS (priv->vel_y)>1.0)||
       (ABS (priv->vel_x)>1.0))&&(!priv->clicked)) {

    return TRUE;
  }

  if (priv->scroll_indicator_event_interrupt) {
    /* Stop a fade out, and fade back in */
    if (priv->scroll_indicator_alpha > 0.9) {
      priv->scroll_indicator_alpha = 1.0;
      priv->scroll_indicator_timeout = 0;

      return FALSE;
    } else {
      priv->scroll_indicator_alpha += 0.2;
      hildon_pannable_area_redraw (area);

      return TRUE;
    }
  }

  if ((priv->scroll_indicator_alpha > 0.9) &&
      (priv->scroll_delay_counter > 0)) {
    priv->scroll_delay_counter--;

    return TRUE;
  }

  if (!priv->scroll_indicator_event_interrupt) {
    /* Continue fade out */
    if (priv->scroll_indicator_alpha < 0.1) {
      priv->scroll_indicator_timeout = 0;
      priv->scroll_indicator_alpha = 0.0;

      return FALSE;
    } else {
      priv->scroll_indicator_alpha -= 0.2;
      hildon_pannable_area_redraw (area);

      return TRUE;
    }
  }

  return TRUE;
}

static gboolean
hildon_pannable_area_expose_event (GtkWidget * widget,
                                   GdkEventExpose * event)
{

  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
#if USE_CAIRO_SCROLLBARS == 1
  GdkColor back_color = widget->style->bg[GTK_STATE_NORMAL];
  GdkColor scroll_color = widget->style->base[GTK_STATE_SELECTED];
#else /* USE_CAIRO_SCROLLBARS */
  GdkColor back_color = widget->style->bg[GTK_STATE_NORMAL];
  GdkColor scroll_color = widget->style->fg[GTK_STATE_INSENSITIVE];
#endif

  if (gtk_bin_get_child (GTK_BIN (widget))) {

    if (priv->scroll_indicator_alpha > 0.1) {
      if (priv->vscroll_visible) {
        hildon_pannable_draw_vscroll (widget, &back_color, &scroll_color);
      }
      if (priv->hscroll_visible) {
        hildon_pannable_draw_hscroll (widget, &back_color, &scroll_color);
      }
    }

    /* draw overshooting rectangles */
    if (priv->overshot_dist_y > 0) {
      gint overshot_height;

      overshot_height = MIN (priv->overshot_dist_y, widget->allocation.height -
                             (priv->hscroll_visible ? priv->hscroll_rect.height : 0));

      gdk_draw_rectangle (widget->window,
                          widget->style->bg_gc[GTK_STATE_NORMAL],
                          TRUE,
                          0,
                          0,
                          widget->allocation.width -
                          (priv->vscroll_visible ? priv->vscroll_rect.width : 0),
                          overshot_height);
    } else if (priv->overshot_dist_y < 0) {
      gint overshot_height;
      gint overshot_y;

      overshot_height =
        MAX (priv->overshot_dist_y,
             -(widget->allocation.height -
               (priv->hscroll_visible ? priv->hscroll_rect.height : 0)));

      overshot_y = MAX (widget->allocation.height +
                        overshot_height -
                        (priv->hscroll_visible ? priv->hscroll_rect.height : 0), 0);

      gdk_draw_rectangle (widget->window,
                          widget->style->bg_gc[GTK_STATE_NORMAL],
                          TRUE,
                          0,
                          overshot_y,
                          widget->allocation.width -
                          priv->vscroll_rect.width,
                          -overshot_height);
    }

    if (priv->overshot_dist_x > 0) {
      gint overshot_width;

      overshot_width = MIN (priv->overshot_dist_x, widget->allocation.width -
                             (priv->vscroll_visible ? priv->vscroll_rect.width : 0));

      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
			  0,
			  0,
                          overshot_width,
			  widget->allocation.height -
                          (priv->hscroll_visible ? priv->hscroll_rect.height : 0));
    } else if (priv->overshot_dist_x < 0) {
      gint overshot_width;
      gint overshot_x;

      overshot_width =
        MAX (priv->overshot_dist_x,
             -(widget->allocation.width -
               (priv->vscroll_visible ? priv->vscroll_rect.width : 0)));

      overshot_x = MAX (widget->allocation.width +
                        overshot_width -
                        (priv->vscroll_visible ? priv->vscroll_rect.width : 0), 0);

      gdk_draw_rectangle (widget->window,
			  widget->style->bg_gc[GTK_STATE_NORMAL],
			  TRUE,
                          overshot_x,
			  0,
			  -overshot_width,
			  widget->allocation.height -
			  priv->hscroll_rect.height);
    }

  }

  if (G_UNLIKELY (priv->initial_effect)) {

    hildon_pannable_area_initial_effect (widget);

    priv->initial_effect = FALSE;
  }

  return GTK_WIDGET_CLASS (hildon_pannable_area_parent_class)->expose_event (widget, event);
}

static GdkWindow *
hildon_pannable_area_get_topmost (GdkWindow * window,
                                  gint x, gint y,
                                  gint * tx, gint * ty,
                                  GdkEventMask mask)
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

      gdk_drawable_get_size (GDK_DRAWABLE (child), &width, &height);
      gdk_window_get_position (child, &wx, &wy);

      if ((x >= wx) && (x < (wx + width)) && (y >= wy) && (y < (wy + height))) {
        gpointer widget;

        gdk_window_get_user_data (child, &widget);

        if ((gdk_window_is_visible (child)) &&
            (gdk_window_peek_children (child) ||
             (gdk_window_get_events (child)&mask))) {
          child_x = x - wx;
          child_y = y - wy;
          window = child;
          break;
        }
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
		gint x_root, gint y_root,
                guint32 time, gboolean in)
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
hildon_pannable_area_button_press_cb (GtkWidget * widget,
				      GdkEventButton * event)
{
  gint x, y;
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;

  if ((!priv->enabled) || (event->button != 1) ||
      ((event->time == priv->last_time) &&
       (priv->last_type == 1)) || (gtk_bin_get_child (GTK_BIN (widget)) == NULL))
    return TRUE;

  priv->scroll_indicator_event_interrupt = 1;

  if (!priv->scroll_indicator_timeout) {
    priv->scroll_indicator_timeout = gdk_threads_add_timeout
      ((gint) (1000.0 / (gdouble) (SCROLL_FADE_TIMEOUT)),
       (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, widget);
  }

  priv->last_time = event->time;
  priv->last_type = 1;

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

  /* Don't allow a click if we're still moving fast */
  if ((ABS (priv->vel_x) <= (priv->vmax * priv->vfast_factor)) &&
      (ABS (priv->vel_y) <= (priv->vmax * priv->vfast_factor)))
    priv->child =
      hildon_pannable_area_get_topmost (gtk_bin_get_child (GTK_BIN (widget))->window,
					event->x, event->y, &x, &y, GDK_BUTTON_PRESS_MASK);
  else
    priv->child = NULL;

  priv->clicked = TRUE;

  /* Stop scrolling on mouse-down (so you can flick, then hold to stop) */
  priv->vel_x = 0;
  priv->vel_y = 0;

  if (priv->child) {

    g_object_add_weak_pointer ((GObject *) priv->child,
			       (gpointer) & priv->child);

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
hildon_pannable_area_refresh (HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = area->priv;
  gboolean prev_hscroll_visible, prev_vscroll_visible;

  if (!gtk_bin_get_child (GTK_BIN (area))) {
    priv->vscroll_visible = FALSE;
    priv->hscroll_visible = FALSE;
    return;
  }

  prev_hscroll_visible = priv->hscroll_visible;
  prev_vscroll_visible = priv->vscroll_visible;

  switch (priv->hscrollbar_policy) {
  case GTK_POLICY_ALWAYS:
    priv->hscroll_visible = TRUE;
    break;
  case GTK_POLICY_NEVER:
    priv->hscroll_visible = FALSE;
    break;
  default:
    priv->hscroll_visible = (priv->hadjust->upper - priv->hadjust->lower >
                             priv->hadjust->page_size);
  }

  switch (priv->vscrollbar_policy) {
  case GTK_POLICY_ALWAYS:
    priv->vscroll_visible = TRUE;
    break;
  case GTK_POLICY_NEVER:
    priv->vscroll_visible = FALSE;
    break;
  default:
    priv->vscroll_visible = (priv->vadjust->upper - priv->vadjust->lower >
                             priv->vadjust->page_size);
  }

  /* Store the vscroll/hscroll areas for redrawing */
  if (priv->vscroll_visible) {
    GtkAllocation *allocation = &GTK_WIDGET (area)->allocation;
    priv->vscroll_rect.x = allocation->width - priv->area_width;
    priv->vscroll_rect.y = 0;
    priv->vscroll_rect.width = priv->area_width;
    priv->vscroll_rect.height = allocation->height -
      (priv->hscroll_visible ? priv->area_width : 0);
  }
  if (priv->hscroll_visible) {
    GtkAllocation *allocation = &GTK_WIDGET (area)->allocation;
    priv->hscroll_rect.y = allocation->height - priv->area_width;
    priv->hscroll_rect.x = 0;
    priv->hscroll_rect.height = priv->area_width;
    priv->hscroll_rect.width = allocation->width -
      (priv->vscroll_visible ? priv->area_width : 0);
  }

  if (GTK_WIDGET_DRAWABLE (area)) {
    if (priv->hscroll_visible != prev_hscroll_visible) {
      gtk_widget_queue_resize (GTK_WIDGET (area));
    }

    if (priv->vscroll_visible != prev_vscroll_visible) {
      gtk_widget_queue_resize (GTK_WIDGET (area));
    }
  }

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
  HildonPannableAreaPrivate *priv = area->priv;

  dist = gtk_adjustment_get_value (adjust) - inc;

  /* Overshooting
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
        *overshot_dist = CLAMP (*overshot_dist + *vel, -overshoot_max, 0);
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

        *overshot_dist = CLAMP (*overshot_dist + (*vel), -overshoot_max, 0);

        gtk_widget_queue_resize (GTK_WIDGET (area));

      } else {
        *overshooting = 0;
        *vel = 0;
        gtk_widget_queue_resize (GTK_WIDGET (area));
      }
    } else {

      gint overshot_dist_old = *overshot_dist;

      if (*overshot_dist > 0) {
        *overshot_dist = CLAMP ((*overshot_dist) + inc, 0, overshoot_max);
      } else if (*overshot_dist < 0) {
        *overshot_dist = CLAMP ((*overshot_dist) + inc, -1 * overshoot_max, 0);
      } else {
        *overshooting = 0;
        gtk_adjustment_set_value (adjust, dist);
      }

      if (*overshot_dist != overshot_dist_old)
        gtk_widget_queue_resize (GTK_WIDGET (area));
    }
  }
}

static void
hildon_pannable_area_scroll (HildonPannableArea *area,
                             gdouble x, gdouble y)
{
  gboolean sx, sy;
  HildonPannableAreaPrivate *priv = area->priv;
  gboolean hscroll_visible, vscroll_visible;

  if (gtk_bin_get_child (GTK_BIN (area)) == NULL)
    return;

  vscroll_visible = (priv->vadjust->upper - priv->vadjust->lower >
	     priv->vadjust->page_size);
  hscroll_visible = (priv->hadjust->upper - priv->hadjust->lower >
	     priv->hadjust->page_size);

  sx = TRUE;
  sy = TRUE;

  if (vscroll_visible) {
    hildon_pannable_axis_scroll (area, priv->vadjust, &priv->vel_y, y,
                                 &priv->overshooting_y, &priv->overshot_dist_y,
                                 &priv->scroll_to_y, priv->vovershoot_max, &sy);
  } else {
    priv->vel_y = 0;
  }

  if (hscroll_visible) {
    hildon_pannable_axis_scroll (area, priv->hadjust, &priv->vel_x, x,
                                 &priv->overshooting_x, &priv->overshot_dist_x,
                                 &priv->scroll_to_x, priv->hovershoot_max, &sx);
  } else {
    priv->vel_x = 0;
  }

  /* If the scroll on a particular axis wasn't succesful, reset the
   * initial scroll position to the new mouse co-ordinate. This means
   * when you get to the top of the page, dragging down works immediately.
   */
  if (priv->mode == HILDON_PANNABLE_AREA_MODE_ACCEL) {
      if (!sx) {
        priv->x = priv->ex;
      }

      if (!sy) {
        priv->y = priv->ey;
      }
    }
}

static gboolean
hildon_pannable_area_timeout (HildonPannableArea * area)
{
  HildonPannableAreaPrivate *priv = area->priv;

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

  return TRUE;
}

static void
hildon_pannable_area_calculate_velocity (gdouble *vel,
                                         gdouble delta,
                                         gdouble dist,
                                         gdouble vmax,
                                         guint sps)
{
  gdouble rawvel;

  if (ABS (dist) >= 0.00001) {
    rawvel = ((dist / ABS (delta)) *
              (gdouble) sps) * FORCE;
    *vel = *vel * (1 - SMOOTH_FACTOR) +
      rawvel * SMOOTH_FACTOR;
    *vel = *vel > 0 ? MIN (*vel, vmax)
      : MAX (dist, -1 * vmax);
  }
}

static gboolean
hildon_pannable_area_motion_event_scroll_timeout (HildonPannableArea *area)
{
  HildonPannableAreaPrivate *priv = area->priv;

  if ((priv->motion_x != 0)||(priv->motion_y != 0))
    hildon_pannable_area_scroll (area, priv->motion_x, priv->motion_y);

  priv->motion_event_scroll_timeout = 0;

  return FALSE;
}

static void
hildon_pannable_area_motion_event_scroll (HildonPannableArea *area,
                                          gdouble x, gdouble y)
{
  HildonPannableAreaPrivate *priv = area->priv;

  if (priv->motion_event_scroll_timeout) {

    priv->motion_x += x;
    priv->motion_y += y;

  } else {

  /* we do not delay the first event but the next ones */
    hildon_pannable_area_scroll (area, x, y);

    priv->motion_x = 0;
    priv->motion_y = 0;

    priv->motion_event_scroll_timeout = gdk_threads_add_timeout
      ((gint) (1000.0 / (gdouble) MOTION_EVENTS_PER_SECOND),
       (GSourceFunc) hildon_pannable_area_motion_event_scroll_timeout, area);
  }
}

static gboolean
hildon_pannable_area_motion_notify_cb (GtkWidget * widget,
				       GdkEventMotion * event)
{
  HildonPannableArea *area = HILDON_PANNABLE_AREA (widget);
  HildonPannableAreaPrivate *priv = area->priv;
  gint dnd_threshold;
  gdouble x, y;
  gdouble delta;

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
      ((ABS (x) > (dnd_threshold))
       || (ABS (y) > (dnd_threshold)))) {
    priv->moved = TRUE;
    x = 0;
    y = 0;

    if (priv->first_drag) {

      if (ABS (priv->iy - event->y) >=
          ABS (priv->ix - event->x)) {
        gboolean vscroll_visible;

        g_signal_emit (area,
                       pannable_area_signals[VERTICAL_MOVEMENT],
                       0, (priv->iy > event->y) ?
                       HILDON_MOVEMENT_UP :
                       HILDON_MOVEMENT_DOWN,
                       (gdouble)priv->ix, (gdouble)priv->iy);

        vscroll_visible = (priv->vadjust->upper - priv->vadjust->lower >
                   priv->vadjust->page_size);

        if (!((vscroll_visible)&&
              (priv->mov_mode&HILDON_MOVEMENT_MODE_VERT)))
          priv->moved = FALSE;

      } else {
        gboolean hscroll_visible;

        g_signal_emit (area,
                       pannable_area_signals[HORIZONTAL_MOVEMENT],
                       0, (priv->ix > event->x) ?
                       HILDON_MOVEMENT_LEFT :
                       HILDON_MOVEMENT_RIGHT,
                       (gdouble)priv->ix, (gdouble)priv->iy);

        hscroll_visible = (priv->hadjust->upper - priv->hadjust->lower >
                           priv->hadjust->page_size);

        if (!((hscroll_visible)&&
              (priv->mov_mode&HILDON_MOVEMENT_MODE_HORIZ)))
          priv->moved = FALSE;
      }
    }

    priv->first_drag = FALSE;

    if ((priv->mode != HILDON_PANNABLE_AREA_MODE_PUSH) &&
	(priv->mode != HILDON_PANNABLE_AREA_MODE_AUTO)) {

      if (!priv->idle_id)
        priv->idle_id = gdk_threads_add_timeout ((gint)
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
      hildon_pannable_area_motion_event_scroll (area, x, y);
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

      if (priv->mov_mode&HILDON_MOVEMENT_MODE_VERT) {
        gdouble dist = event->y - priv->y;

        hildon_pannable_area_calculate_velocity (&priv->vel_y,
                                                 delta,
                                                 dist,
                                                 priv->vmax,
                                                 priv->sps);
      } else {
        y = 0;
        priv->vel_y = 0;
      }

      if (priv->mov_mode&HILDON_MOVEMENT_MODE_HORIZ) {
        gdouble dist = event->x - priv->x;

        hildon_pannable_area_calculate_velocity (&priv->vel_x,
                                                 delta,
                                                 dist,
                                                 priv->vmax,
                                                 priv->sps);
      } else {
        x = 0;
        priv->vel_x = 0;
      }

      hildon_pannable_area_motion_event_scroll (area, x, y);

      if (priv->mov_mode&HILDON_MOVEMENT_MODE_HORIZ)
        priv->x = event->x;
      if (priv->mov_mode&HILDON_MOVEMENT_MODE_VERT)
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
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;
  gint x, y;
  GdkWindow *child;

  if  (((event->time == priv->last_time) && (priv->last_type == 3))
       || (gtk_bin_get_child (GTK_BIN (widget)) == NULL)
       || (!priv->clicked) || (!priv->enabled) || (event->button != 1))
    return TRUE;

  priv->scroll_indicator_event_interrupt = 0;
  priv->scroll_delay_counter = SCROLLBAR_FADE_DELAY;

  if ((ABS (priv->vel_y) > 1.0)||
      (ABS (priv->vel_x) > 1.0)) {
    priv->scroll_indicator_alpha = 1.0;
  }

  if (!priv->scroll_indicator_timeout) {
    priv->scroll_indicator_timeout = gdk_threads_add_timeout
      ((gint) (1000.0 / (gdouble) SCROLL_FADE_TIMEOUT),
       (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, widget);
  }

  /* move all the way to the last position */
  if (priv->motion_event_scroll_timeout) {
    g_source_remove (priv->motion_event_scroll_timeout);
    hildon_pannable_area_motion_event_scroll_timeout (HILDON_PANNABLE_AREA (widget));
  }

  priv->clicked = FALSE;

  if (priv->mode == HILDON_PANNABLE_AREA_MODE_AUTO ||
      priv->mode == HILDON_PANNABLE_AREA_MODE_ACCEL) {

    /* If overshoot has been initiated with a finger down, on release set max speed */
    if (priv->overshot_dist_y != 0) {
      priv->overshooting_y = BOUNCE_STEPS; /* Hack to stop a bounce in the finger down case */
      priv->vel_y = priv->vmax;
    }

    if (priv->overshot_dist_x != 0) {
      priv->overshooting_x = BOUNCE_STEPS; /* Hack to stop a bounce in the finger down case */
      priv->vel_x = priv->vmax;
    }

    if (!priv->idle_id)
      priv->idle_id = gdk_threads_add_timeout ((gint) (1000.0 / (gdouble) priv->sps),
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
				      event->x, event->y, &x, &y, GDK_BUTTON_RELEASE_MASK);

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
				(gpointer) & priv->child);

  priv->moved = FALSE;
  gdk_event_free ((GdkEvent *) event);

  return TRUE;
}

/* utility event handler */
static gboolean
hildon_pannable_area_scroll_cb (GtkWidget *widget,
                                GdkEventScroll *event)
{
  GtkAdjustment *adj = NULL;
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (widget)->priv;

  if ((!priv->enabled) ||
      (gtk_bin_get_child (GTK_BIN (widget)) == NULL))
    return TRUE;

  priv->scroll_indicator_event_interrupt = 0;
  priv->scroll_indicator_alpha = 1.0;
  priv->scroll_delay_counter = SCROLLBAR_FADE_DELAY + 20;

  if (!priv->scroll_indicator_timeout) {
    priv->scroll_indicator_timeout = gdk_threads_add_timeout
      ((gint) (1000.0 / (gdouble) (SCROLL_FADE_TIMEOUT)),
       (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, widget);
  }

  /* Stop inertial scrolling */
  if (priv->idle_id) {
    priv->vel_x = 0.0;
    priv->vel_y = 0.0;
    priv->overshooting_x = 0;
    priv->overshooting_y = 0;

    if ((priv->overshot_dist_x>0)||(priv->overshot_dist_y>0)) {
      priv->overshot_dist_x = 0;
      priv->overshot_dist_y = 0;

      gtk_widget_queue_resize (GTK_WIDGET (widget));
    }

    g_source_remove (priv->idle_id);
    priv->idle_id = 0;
  }

  if (event->direction == GDK_SCROLL_UP || event->direction == GDK_SCROLL_DOWN)
    adj = priv->vadjust;
  else
    adj = priv->hadjust;

  if (adj)
    {
      gdouble delta, new_value;

      /* from gtkrange.c calculate delta*/
      delta = pow (adj->page_size, 2.0 / 3.0);

      if (event->direction == GDK_SCROLL_UP ||
          event->direction == GDK_SCROLL_LEFT)
        delta = - delta;

      new_value = CLAMP (adj->value + delta, adj->lower, adj->upper - adj->page_size);

      gtk_adjustment_set_value (adj, new_value);
    }

  return TRUE;
}

static void
hildon_pannable_area_child_mapped (GtkWidget *widget,
                                   GdkEvent  *event,
                                   gpointer user_data)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (user_data)->priv;

  if (priv->event_window != NULL && priv->enabled)
    gdk_window_raise (priv->event_window);
}

static void
hildon_pannable_area_add (GtkContainer *container, GtkWidget *child)
{
  HildonPannableAreaPrivate *priv = HILDON_PANNABLE_AREA (container)->priv;

  g_return_if_fail (gtk_bin_get_child (GTK_BIN (container)) == NULL);

  gtk_widget_set_parent (child, GTK_WIDGET (container));
  GTK_BIN (container)->child = child;

  g_signal_connect_after (child, "map-event",
                          G_CALLBACK (hildon_pannable_area_child_mapped),
                          container);

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

  g_signal_handlers_disconnect_by_func (GTK_WIDGET (child),
                                        G_CALLBACK (hildon_pannable_area_child_mapped),
                                        container);

  /* chain parent class handler to remove child */
  GTK_CONTAINER_CLASS (hildon_pannable_area_parent_class)->remove (container, child);
}

static void
hildon_pannable_calculate_vel_factor (HildonPannableArea * self)
{
  HildonPannableAreaPrivate *priv = self->priv;
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

/**
 * hildon_pannable_area_new:
 *
 * Create a new pannable area widget
 *
 * Returns: the newly created #HildonPannableArea
 *
 * Since: 2.2
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
 *
 * Since: 2.2
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
 *
 * Since: 2.2
 */

void
hildon_pannable_area_add_with_viewport (HildonPannableArea * area,
					GtkWidget * child)
{
  GtkBin *bin;
  GtkWidget *viewport;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));
  g_return_if_fail (GTK_IS_WIDGET (child));
  g_return_if_fail (child->parent == NULL);

  bin = GTK_BIN (area);

  if (bin->child != NULL)
    {
      g_return_if_fail (GTK_IS_VIEWPORT (bin->child));
      g_return_if_fail (GTK_BIN (bin->child)->child == NULL);

      viewport = bin->child;
    }
  else
    {
      HildonPannableAreaPrivate *priv = area->priv;

      viewport = gtk_viewport_new (priv->hadjust,
                                   priv->vadjust);
      gtk_viewport_set_shadow_type (GTK_VIEWPORT (viewport), GTK_SHADOW_NONE);
      gtk_container_add (GTK_CONTAINER (area), viewport);
    }

  gtk_widget_show (viewport);
  gtk_container_add (GTK_CONTAINER (viewport), child);
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
 * <example>
 * <programlisting>
 *  GtkTreePath *path;
 *  GdkRectangle *rect;
 *  <!-- -->
 *  path = gtk_tree_model_get_path (model, &amp;iter);
 *  gtk_tree_view_get_background_area (GTK_TREE_VIEW (treeview),
 *                                     path, NULL, &amp;rect);
 *  gtk_tree_view_convert_bin_window_to_tree_coords (GTK_TREE_VIEW (treeview),
 *                                                   0, rect.y, NULL, &amp;y);
 *  hildon_pannable_area_scroll_to (panarea, -1, y);
 *  gtk_tree_path_free (path);
 * </programlisting>
 * </example>
 *
 * If you want to present a child widget in simpler scenarios,
 * use hildon_pannable_area_scroll_to_child() instead.
 *
 * There is a precondition to this function: the widget must be
 * already realized. Check the hildon_pannable_area_jump_to_child() for
 * more tips regarding how to call this function during
 * initialization.
 *
 * Since: 2.2
 **/
void
hildon_pannable_area_scroll_to (HildonPannableArea *area,
				const gint x, const gint y)
{
  HildonPannableAreaPrivate *priv;
  gint width, height;
  gint dist_x, dist_y;
  gboolean hscroll_visible, vscroll_visible;

  g_return_if_fail (GTK_WIDGET_REALIZED (area));
  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));

  priv = area->priv;

  vscroll_visible = (priv->vadjust->upper - priv->vadjust->lower >
	     priv->vadjust->page_size);
  hscroll_visible = (priv->hadjust->upper - priv->hadjust->lower >
	     priv->hadjust->page_size);

  if (((!vscroll_visible)&&(!hscroll_visible)) ||
      (x == -1 && y == -1)) {
    return;
  }

  if (priv->mode == HILDON_PANNABLE_AREA_MODE_PUSH)
    hildon_pannable_area_jump_to (area, x, y);

  width = priv->hadjust->upper - priv->hadjust->lower;
  height = priv->vadjust->upper - priv->vadjust->lower;

  g_return_if_fail (x < width || y < height);

  if ((x > -1)&&(hscroll_visible)) {
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

  if ((y > -1)&&(vscroll_visible)) {
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

  if (!priv->scroll_indicator_timeout)
    priv->scroll_indicator_timeout = gdk_threads_add_timeout
      ((gint) (1000.0 / (gdouble) SCROLL_FADE_TIMEOUT),
       (GSourceFunc) hildon_pannable_area_scroll_indicator_fade, area);

  if (!priv->idle_id)
    priv->idle_id = gdk_threads_add_timeout ((gint) (1000.0 / (gdouble) priv->sps),
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
 * There is a precondition to this function: the widget must be
 * already realized. Check the hildon_pannable_area_jump_to_child() for
 * more tips regarding how to call this function during
 * initialization.
 *
 * Since: 2.2
 **/
void
hildon_pannable_area_jump_to (HildonPannableArea *area,
                              const gint x, const gint y)
{
  HildonPannableAreaPrivate *priv;
  gint width, height;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));
  g_return_if_fail (GTK_WIDGET_REALIZED (area));
  g_return_if_fail (x >= -1 && y >= -1);

  if (x == -1 && y == -1) {
    return;
  }

  priv = area->priv;

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
    g_source_remove (priv->scroll_indicator_timeout);
    priv->scroll_indicator_timeout = 0;
  }

  if (priv->idle_id) {
    priv->vel_x = 0.0;
    priv->vel_y = 0.0;
    priv->overshooting_x = 0;
    priv->overshooting_y = 0;

    if ((priv->overshot_dist_x>0)||(priv->overshot_dist_y>0)) {
      priv->overshot_dist_x = 0;
      priv->overshot_dist_y = 0;

      gtk_widget_queue_resize (GTK_WIDGET (area));
    }

    g_source_remove (priv->idle_id);
    priv->idle_id = 0;
  }
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
 * There is a precondition to this function: the widget must be
 * already realized. Check the hildon_pannable_area_jump_to_child() for
 * more tips regarding how to call this function during
 * initialization.
 *
 * Since: 2.2
 **/
void
hildon_pannable_area_scroll_to_child (HildonPannableArea *area, GtkWidget *child)
{
  GtkWidget *bin_child;
  gint x, y;

  g_return_if_fail (GTK_WIDGET_REALIZED (area));
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
 * There is a precondition to this function: the widget must be
 * already realized. You can control if the widget is ready with the
 * GTK_WIDGET_REALIZED macro. If you want to call this function during
 * the initialization process of the widget do it inside a callback to
 * the ::realize signal, using g_signal_connect_after() function.
 *
 * Since: 2.2
 **/
void
hildon_pannable_area_jump_to_child (HildonPannableArea *area, GtkWidget *child)
{
  GtkWidget *bin_child;
  gint x, y;

  g_return_if_fail (GTK_WIDGET_REALIZED (area));
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
 *
 * Since: 2.2
 **/
GtkWidget*
hildon_pannable_get_child_widget_at (HildonPannableArea *area,
                                     gdouble x, gdouble y)
{
  GdkWindow *window = NULL;
  GtkWidget *child_widget = NULL;

  window = hildon_pannable_area_get_topmost
    (gtk_bin_get_child (GTK_BIN (area))->window,
     x, y, NULL, NULL, GDK_ALL_EVENTS_MASK);

  gdk_window_get_user_data (window, (gpointer) &child_widget);

  return child_widget;
}


/**
 * hildon_pannable_area_get_hadjustment:
 * @area: A #HildonPannableArea.
 *
 * Returns the horizontal adjustment
 *
 * returns: The horizontal #GtkAdjustment
 **/
GtkAdjustment*
hildon_pannable_area_get_hadjustment            (HildonPannableArea *area)
{

  g_return_val_if_fail (HILDON_IS_PANNABLE_AREA (area), NULL);

  return area->priv->hadjust;
}

/**
 * hildon_pannable_area_get_vadjustment:
 * @area: A #HildonPannableArea.
 *
 * Returns the vertical adjustment
 *
 * returns: The vertical #GtkAdjustment
 **/
GtkAdjustment*
hildon_pannable_area_get_vadjustment            (HildonPannableArea *area)
{
  g_return_val_if_fail (HILDON_IS_PANNABLE_AREA (area), NULL);

  return area->priv->vadjust;
}


/**
 * hildon_pannable_area_get_size_request_policy:
 * @area: A #HildonPannableArea.
 *
 * This function returns the current size request policy of the
 * widget. That policy controls the way the size_request is done in
 * the pannable area. Check
 * hildon_pannable_area_set_size_request_policy() for a more detailed
 * explanation.
 *
 * returns: the policy is currently being used in the widget
 * #HildonSizeRequestPolicy.
 **/
HildonSizeRequestPolicy
hildon_pannable_area_get_size_request_policy (HildonPannableArea *area)
{
  HildonPannableAreaPrivate *priv;

  g_return_val_if_fail (HILDON_IS_PANNABLE_AREA (area), FALSE);

  priv = area->priv;

  return priv->size_request_policy;
}

/**
 * hildon_pannable_area_set_size_request_policy:
 * @area: A #HildonPannableArea.
 * @size_request_policy: One of the allowed #HildonSizeRequestPolicy
 *
 * This function sets the pannable area size request policy. That
 * policy controls the way the size_request is done in the pannable
 * area. Pannable can use the size request of its children
 * (#HILDON_SIZE_REQUEST_CHILDREN) or the minimum size required for
 * the area itself (#HILDON_SIZE_REQUEST_MINIMUM), the latter is the
 * default. Recall this size depends on the scrolling policy you are
 * requesting to the pannable area, if you set #GTK_POLICY_NEVER this
 * parameter will not have any effect with
 * #HILDON_SIZE_REQUEST_MINIMUM set.
 *
 **/
void
hildon_pannable_area_set_size_request_policy (HildonPannableArea *area,
                                              HildonSizeRequestPolicy size_request_policy)
{
  HildonPannableAreaPrivate *priv;

  g_return_if_fail (HILDON_IS_PANNABLE_AREA (area));

  priv = area->priv;

  if (priv->size_request_policy == size_request_policy)
    return;

  priv->size_request_policy = size_request_policy;

  gtk_widget_queue_resize (GTK_WIDGET (area));

  g_object_notify (G_OBJECT (area), "size-request-policy");
}

