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

/* 
 * @file hildon-seekbar.c
 *
 * This file contains the API implementation for Hildon Seekbar widget.
 * Based on seekbar.gob rev 1.10
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#include <stdio.h>
#include <math.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkframe.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtktoolbar.h>
#include <gdk/gdkkeysyms.h>

#include "hildon-seekbar.h"

#define _(String) dgettext(PACKAGE, String)

#define HILDON_SEEKBAR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
         HILDON_TYPE_SEEKBAR, HildonSeekbarPrivate));

typedef struct _HildonSeekbarPrivate HildonSeekbarPrivate;

/* our parent class */
static GtkScaleClass *parent_class = NULL;

/* Init functions */
static void hildon_seekbar_class_init(HildonSeekbarClass * seekbar_class);
static void hildon_seekbar_init(HildonSeekbar * seekbar);
static void hildon_seekbar_finalize(GObject * self);

/* property functions */
static void hildon_seekbar_set_property(GObject * object, guint prop_id,
                                        const GValue * value,
                                        GParamSpec * pspec);

static void hildon_seekbar_get_property(GObject * object, guint prop_id,
                                        GValue * value,
                                        GParamSpec * pspec);

/* virtual functions */
static void hildon_seekbar_size_request(GtkWidget * widget,
                                        GtkRequisition * event);
static void hildon_seekbar_size_allocate(GtkWidget * widget,
                                         GtkAllocation * allocation);
static gboolean hildon_seekbar_expose(GtkWidget * widget,
                                      GdkEventExpose * event);
static gboolean hildon_seekbar_button_press_event(GtkWidget * widget,
                                                  GdkEventButton * event);
static gboolean hildon_seekbar_button_release_event(GtkWidget * widget,
                                                    GdkEventButton * event);
static gboolean hildon_seekbar_keypress(GtkWidget * widget,
                                        GdkEventKey * event);

/* private stuff */

/*
 * Some constants
 * These should be named so that they are quite self
 * explanatory
 */
#define MINIMUM_WIDTH 115
#define DEFAULT_HEIGHT 58
/* Toolbar width and height defines */
#define TOOL_MINIMUM_WIDTH 75
#define TOOL_DEFAULT_HEIGHT 40

#define DEFAULT_DISPLAYC_BORDER 10
#define BUFFER_SIZE 32
#define EXTRA_SIDE_BORDER 20
#define TOOL_EXTRA_SIDE_BORDER 0

/* the number of steps it takes to move from left to right */
#define SECONDS_PER_MINUTE 60
#define NUM_STEPS 20

/* the number of digits precision for the internal range.
 * note, this needs to be enough so that the step size for
 * small total_times doesn't get rounded off. Currently set to 3
 * this is because for the smallest total time ( i.e 1 ) and the current
 * num steps ( 20 ) is: 1/20 = 0.05.  0.05 is 2 digits, and we
 * add one for safety */
#define MAX_ROUND_DIGITS 3

/* enum for properties */
enum {
    PROP_TOTAL_TIME = 1,
    PROP_POSITION
};

/* private variables */
struct _HildonSeekbarPrivate {
    GtkWidget *label;
    gboolean draw_value;
    gboolean is_toolbar;
    guint fraction; /* This is the amount of time that has progressed from
                       the beginning. It should be an integer between the
                       minimum and maximum values of the corresponding
                       adjustment, ie. adjument->lower and ->upper.. */
};

/**
 * Initialises, and returns the type of a hildon seekbar.
 */
GType hildon_seekbar_get_type(void)
{
    static GType seekbar_type = 0;

    if (!seekbar_type) {
        static const GTypeInfo seekbar_info = {
            sizeof(HildonSeekbarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_seekbar_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonSeekbar),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_seekbar_init,
        };
        seekbar_type = g_type_register_static(GTK_TYPE_SCALE,
                                              "HildonSeekbar",
                                              &seekbar_info, 0);
    }
    return seekbar_type;
}

/**
 * Initialises the seekbar class.
 */
static void hildon_seekbar_class_init(HildonSeekbarClass * seekbar_class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(seekbar_class);
    GObjectClass *object_class = G_OBJECT_CLASS(seekbar_class);

    /* set the global parent_class */
    parent_class = g_type_class_peek_parent(seekbar_class);

    g_type_class_add_private(seekbar_class, sizeof(HildonSeekbarPrivate));

    /* setup our widgets v-table */
    widget_class->size_request = hildon_seekbar_size_request;
    widget_class->size_allocate = hildon_seekbar_size_allocate;
    widget_class->expose_event = hildon_seekbar_expose;
    widget_class->button_press_event = hildon_seekbar_button_press_event;
    widget_class->button_release_event =
        hildon_seekbar_button_release_event;
    widget_class->key_press_event = hildon_seekbar_keypress;

    /* now the object stuff */
    object_class->set_property = hildon_seekbar_set_property;
    object_class->get_property = hildon_seekbar_get_property;
    object_class->finalize = hildon_seekbar_finalize;

    /* install the properties */
    g_object_class_install_property(object_class, PROP_TOTAL_TIME,
        g_param_spec_double("total_time",
                            "total time",
                            "Total playing time of this media file",
                            0,           /* min value */
                            G_MAXDOUBLE, /* max value */
                            0,           /* default */
                            G_PARAM_READABLE | G_PARAM_WRITABLE));

    g_object_class_install_property(object_class, PROP_POSITION,
        g_param_spec_double("position",
                            "position",
                            "Current position in this media file",
                            0,           /* min value */
                            G_MAXDOUBLE, /* max value */
                            0,           /* default */
                            G_PARAM_READABLE | G_PARAM_WRITABLE));
    /* readable and writable */
}


static void hildon_seekbar_init(HildonSeekbar * seekbar)
{
    HildonSeekbarPrivate *priv;
    GtkRange *range = GTK_RANGE(seekbar);

    priv = HILDON_SEEKBAR_GET_PRIVATE(seekbar);

    range->orientation = GTK_ORIENTATION_HORIZONTAL;
    range->flippable = TRUE;
    range->has_stepper_a = TRUE;
    range->has_stepper_d = TRUE;
    range->round_digits = MAX_ROUND_DIGITS;

    gtk_scale_set_draw_value (GTK_SCALE (seekbar), FALSE);
}

/* is this even necessary? */
static void hildon_seekbar_finalize( GObject *obj_self )	
{	
    HildonSeekbar *self;	
    HildonSeekbarPrivate *priv;	

    self = HILDON_SEEKBAR ( obj_self );	
    priv = HILDON_SEEKBAR_GET_PRIVATE (self);	

    if( G_OBJECT_CLASS( parent_class )->finalize )	
        G_OBJECT_CLASS( parent_class )->finalize( obj_self );	
}


/* handle keypress events */
static gboolean hildon_seekbar_keypress(GtkWidget * widget,
                                        GdkEventKey * event)
{
    if (event->keyval == GDK_Up || event->keyval == GDK_Down)
        return FALSE;
    return ((GTK_WIDGET_CLASS(parent_class)->key_press_event) (widget,
                                                               event));
}

/* handle setting of seekbar properties */
static void
hildon_seekbar_set_property(GObject * object, guint prop_id,
                            const GValue * value, GParamSpec * pspec)
{
    GtkRange *range = GTK_RANGE(object);

    switch (prop_id) {
    case PROP_TOTAL_TIME:
        range->adjustment->upper = g_value_get_double(value);
        break;
    case PROP_POSITION:
        range->adjustment->value = g_value_get_double(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/* handle getting of seekbar properties */
static void
hildon_seekbar_get_property(GObject * object, guint prop_id,
                            GValue * value, GParamSpec * pspec)
{
    GtkRange *range = GTK_RANGE(object);

    switch (prop_id) {
    case PROP_TOTAL_TIME:
        g_value_set_double(value, range->adjustment->upper);
        break;
    case PROP_POSITION:
        g_value_set_double(value, range->adjustment->value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/**
 * hildon_seekbar_new:
 *
 * Creates a new #HildonSeekbar widget.
 * 
 * Return value: A #GtkWidget pointer of #HildonSeekbar widget.
 */
GtkWidget *hildon_seekbar_new(void)
{
    /* return a new object */
    return g_object_new(HILDON_TYPE_SEEKBAR, NULL);
}

/**
 * hildon_seekbar_get_total_time:
 * @seekbar: Pointer to #HildonSeekbar widget.
 *
 * Accessor method for getting total playing time of stream
 * in seconds.
 *
 * Returns: Total time as gint.
 **/
gint hildon_seekbar_get_total_time(HildonSeekbar *seekbar)
{
    GtkWidget *widget;
    widget = GTK_WIDGET (seekbar);
    g_return_val_if_fail(HILDON_IS_SEEKBAR(seekbar), 0);
    g_return_val_if_fail(GTK_RANGE(widget)->adjustment, 0);
    return GTK_RANGE(widget)->adjustment->upper;
}

/**
 * hildon_seekbar_set_total_time:
 * @seekbar: Pointer to #HildonSeekbar widget.
 * @time: Time within range of > 0 && < G_MAXINT
 *
 * Accessor method for setting total playing time of stream
 * in seconds.
 *
 **/
void hildon_seekbar_set_total_time(HildonSeekbar *seekbar, gint time)
{
    GtkAdjustment *adj;
    GtkWidget *widget;
    gboolean value_changed = FALSE;

    g_return_if_fail(HILDON_IS_SEEKBAR(seekbar));
    widget = GTK_WIDGET (seekbar);

    if (time <= 0) {
        return;
    }

    g_return_if_fail(GTK_RANGE(widget)->adjustment);

    adj = GTK_RANGE(widget)->adjustment;
    adj->upper = time;

    if (adj->value > time) {
        adj->value = time;
        value_changed = TRUE;
    }

    adj->step_increment = adj->upper / NUM_STEPS;
    adj->page_increment = adj->step_increment;

    gtk_adjustment_changed(adj);

    if (value_changed) {
        gtk_adjustment_value_changed(adj);
        hildon_seekbar_set_fraction(seekbar,
                                    MIN(hildon_seekbar_get_fraction(seekbar),
                                        time));
        hildon_seekbar_set_position(seekbar,
                                    MIN(hildon_seekbar_get_position(seekbar),
                                        time));
    }
}

/**
 * hildon_seekbar_get_fraction:
 * @seekbar: Pointer to #HildonSeekbar widget.
 *
 * Accessor method for getting current fraction related to the progress indicator.
 * It should be between min and max of seekbar range.
 *
 * Returns: Current fraction.
 **/
guint hildon_seekbar_get_fraction( HildonSeekbar *seekbar )
{
  g_return_val_if_fail( HILDON_IS_SEEKBAR( seekbar ), 0 );

  return osso_gtk_range_get_stream_position (GTK_RANGE(seekbar));
}

/**
 * hildon_seekbar_set_fraction:
 * @seekbar: Pointer to #HildonSeekbar widget.
 * @fraction: The new position of the progress indicator.
 *
 * Method for setting current value related to the progress indicator.
 * It should be between the minimal and maximal values of the range in seekbar.
 **/
void hildon_seekbar_set_fraction( HildonSeekbar *seekbar, guint fraction )
{
  GtkRange *range = NULL;
  g_return_if_fail( HILDON_IS_SEEKBAR( seekbar ) );

  range = GTK_RANGE(GTK_WIDGET(seekbar));
  
  g_return_if_fail(fraction <= range->adjustment->upper &&
                   fraction >= range->adjustment->lower);
  
  /* Set to show stream indicator. */
  g_object_set (G_OBJECT (seekbar), "stream_indicator", TRUE, NULL);

  fraction = CLAMP(fraction, range->adjustment->lower,
                   range->adjustment->upper);
  
  osso_gtk_range_set_stream_position( range, fraction );
  
  if (fraction < hildon_seekbar_get_position(seekbar))
    hildon_seekbar_set_position(seekbar, fraction);
}

/**
 * hildon_seekbar_get_position:
 * @seekbar: Pointer to #HildonSeekbar widget.
 *
 * Accessor method for getting current position in stream
 * in seconds.
 *
 *
 * Returns: Current position in stream in seconds.
 **/
gint hildon_seekbar_get_position(HildonSeekbar *seekbar)
{
  g_return_val_if_fail(HILDON_IS_SEEKBAR(seekbar), 0);
  g_return_val_if_fail(GTK_RANGE(seekbar)->adjustment, 0);

  return GTK_RANGE(seekbar)->adjustment->value;
}

/**
 * hildon_seekbar_set_position:
 * @seekbar: Pointer to #HildonSeekbar widget.
 * @time: Time within range of >= 0 && < G_MAXINT
 *
 * Accessor method for setting current position in stream
 * in seconds.
 **/
void hildon_seekbar_set_position(HildonSeekbar *seekbar, gint time)
{
    GtkRange *range;
    GtkAdjustment *adj;
    gint value;

    g_return_if_fail(time >= 0);
    g_return_if_fail(HILDON_IS_SEEKBAR(seekbar));
    range = GTK_RANGE(seekbar);
    adj = range->adjustment;
    g_return_if_fail(adj);
    
    /* only change value if it is a different int. this allows us to have
       smooth scrolls for small total_times */
    value = floor(adj->value);
    if (time != value) {
      value = (time < adj->upper) ? time : adj->upper;
      if (value <= osso_gtk_range_get_stream_position (range)) {
        adj->value = value;
        gtk_adjustment_value_changed(adj);
      }
    }
}

static void hildon_seekbar_size_request(GtkWidget * widget,
                                        GtkRequisition * req)
{
    HildonSeekbar *self = NULL;
    HildonSeekbarPrivate *priv = NULL;
    GtkWidget *parent = NULL;

    self = HILDON_SEEKBAR(widget);
    priv = HILDON_SEEKBAR_GET_PRIVATE(self)

    parent =
        gtk_widget_get_ancestor(GTK_WIDGET(self), GTK_TYPE_TOOLBAR);

    priv->is_toolbar = parent ? TRUE : FALSE;

    if (GTK_WIDGET_CLASS(parent_class)->size_request)
        GTK_WIDGET_CLASS(parent_class)->size_request(widget, req);

    req->width = priv->is_toolbar ? TOOL_MINIMUM_WIDTH : MINIMUM_WIDTH;
    req->height = priv->is_toolbar ? TOOL_DEFAULT_HEIGHT : DEFAULT_HEIGHT;
}

static void hildon_seekbar_size_allocate(GtkWidget * widget,
                                         GtkAllocation * allocation)
{
    HildonSeekbarPrivate *priv;

    priv = HILDON_SEEKBAR_GET_PRIVATE(HILDON_SEEKBAR(widget));

    if (priv->is_toolbar == TRUE)
      {
        if (allocation->height > TOOL_DEFAULT_HEIGHT) {
            allocation->y +=
	      (allocation->height - TOOL_DEFAULT_HEIGHT) / 2;
            allocation->height = TOOL_DEFAULT_HEIGHT;
          }
      }
    else
      {
        if (allocation->height > DEFAULT_HEIGHT) {
            allocation->y +=
	      (allocation->height - DEFAULT_HEIGHT) / 2;
            allocation->height = DEFAULT_HEIGHT;
          }
      }

    if (priv->is_toolbar == TRUE)
      {
        allocation->x += TOOL_EXTRA_SIDE_BORDER;
        allocation->width -= 2 * TOOL_EXTRA_SIDE_BORDER;
      }
    else
      {
        allocation->x += EXTRA_SIDE_BORDER;
        allocation->width -= 2 * EXTRA_SIDE_BORDER;
      }
   
    if (GTK_WIDGET_CLASS(parent_class)->size_allocate)
        GTK_WIDGET_CLASS(parent_class)->size_allocate(widget, allocation);
}

static gboolean hildon_seekbar_expose(GtkWidget * widget,
                                      GdkEventExpose * event)
{
    HildonSeekbarPrivate *priv;
    gint extra_side_borders = 0;

    priv = HILDON_SEEKBAR_GET_PRIVATE(HILDON_SEEKBAR(widget));
   
    extra_side_borders = priv->is_toolbar ? TOOL_EXTRA_SIDE_BORDER :
                                            EXTRA_SIDE_BORDER;

    if (GTK_WIDGET_DRAWABLE(widget)) {
        gtk_paint_box(widget->style, widget->window,
                      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                      NULL, widget, "seekbar",
                      widget->allocation.x - extra_side_borders,
                      widget->allocation.y,
                      widget->allocation.width + 2 * extra_side_borders,
                      widget->allocation.height);

        (*GTK_WIDGET_CLASS(parent_class)->expose_event) (widget, event);
    }

    return FALSE;
}

/*
 * Event handler for button_press_event
 */
static gboolean
hildon_seekbar_button_press_event(GtkWidget * widget,
                                  GdkEventButton * event)
{
    HildonSeekbar *self;
    HildonSeekbarPrivate *priv;

    gint result = FALSE;

    self = HILDON_SEEKBAR(widget);
    priv = HILDON_SEEKBAR_GET_PRIVATE(self);

    event->button = event->button == 1 ? 2 : event->button;

    /* call the parent handler */
    if (GTK_WIDGET_CLASS(parent_class)->button_press_event)
        result = GTK_WIDGET_CLASS(parent_class)->button_press_event(widget,
                                                                    event);

    return result;
}

/*
 * Event handler for button_release_event
 */
static gboolean
hildon_seekbar_button_release_event(GtkWidget * widget,
                                    GdkEventButton * event)
{
    HildonSeekbar *self;
    HildonSeekbarPrivate *priv;
    gboolean result = FALSE;

    self = HILDON_SEEKBAR(widget);
    priv = HILDON_SEEKBAR_GET_PRIVATE(self);

    event->button = event->button == 1 ? 2 : event->button;

    /* call the parent handler */
    if (GTK_WIDGET_CLASS(parent_class)->button_release_event)
        result =
            GTK_WIDGET_CLASS(parent_class)->button_release_event(widget,
                                                                 event);
    return result;
}
