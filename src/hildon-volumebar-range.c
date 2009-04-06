/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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

/**
 * SECTION:hildon-volumebar-range
 * @short_description: This widget is an "workhorse" for #HildonVolumebar
 * widget. It is not designed to be used as a standalone widget.
 *
 * Purpose of this widget is to act as an "container" for GtkScale
 * widget. #HildonVolumebarRange changes some event parameters so
 * that #HildonVolumebar can meet its specifications.
 *
 * Currently #HildonVolumebarRange models range of [0..100].
 *
 * <note>
 *   <para>
 * #HildonVolumebarRange has been deprecated since Hildon 2.2 and should not
 * be used in newly written code. See
 * <link linkend="hildon-migrating-volume-bar">Migrating Volume Bars</link>
 * section to know how to migrate this deprecated widget.
 *   </para>
 * </note>
 */

#include                                        <gdk/gdkkeysyms.h>

#include                                        "hildon-volumebar-range.h"

#define                                         VOLUMEBAR_RANGE_INITIAL_VALUE 50.0

#define                                         VOLUMEBAR_RANGE_MINIMUM_VALUE 0.0

#define                                         VOLUMEBAR_RANGE_MAXIMUM_VALUE 100.0

#define                                         VOLUMEBAR_RANGE_STEP_INCREMENT_VALUE 5.0

#define                                         VOLUMEBAR_RANGE_PAGE_INCREMENT_VALUE 5.0

#define                                         VOLUMEBAR_RANGE_PAGE_SIZE_VALUE 0.0

#define                                         CHANGE_THRESHOLD 0.001

static GtkScaleClass*                           parent_class;

static void 
hildon_volumebar_range_class_init               (HildonVolumebarRangeClass*
                                                 volumerange_class);

static void 
hildon_volumebar_range_init                     (HildonVolumebarRange*
                                                 volumerange);

static void 
hildon_volumebar_range_set_property             (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec);

static void 
hildon_volumebar_range_get_property             (GObject *object,
                                                 guint prop_id,
                                                 GValue *value,
                                                 GParamSpec *pspec);

static gint 
hildon_volumebar_range_button_press_event       (GtkWidget *widget,
                                                 GdkEventButton *event);

static gint 
hildon_volumebar_range_button_release_event     (GtkWidget *widget,
                                                 GdkEventButton *event);

static gboolean
hildon_volumebar_range_keypress                 (GtkWidget *widget,
                                                 GdkEventKey *event);

enum 
{
  PROP_0,
  PROP_LEVEL
};

/**
 * hildon_volumebar_range_get_type:
 *
 * Initializes and returns the type of a hildon volumebar range.
 *
 * Returns: GType of #HildonVolumebarRange
 */
GType G_GNUC_CONST
hildon_volumebar_range_get_type                 (void)
{
    static GType volumerange_type = 0;

    if (!volumerange_type) {
        static const GTypeInfo volumerange_info = {
            sizeof (HildonVolumebarRangeClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_volumebar_range_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonVolumebarRange),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_volumebar_range_init,
        };
        volumerange_type = g_type_register_static (GTK_TYPE_SCALE,
                "HildonVolumebarRange",
                &volumerange_info, 0);
    }

    return volumerange_type;
}

static void 
hildon_volumebar_range_class_init               (HildonVolumebarRangeClass *volumerange_class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (volumerange_class);
    GObjectClass *object_class = G_OBJECT_CLASS (volumerange_class);

    parent_class = g_type_class_peek_parent (volumerange_class);

    widget_class->button_press_event =
        hildon_volumebar_range_button_press_event;
    widget_class->button_release_event =
        hildon_volumebar_range_button_release_event;
    widget_class->key_press_event = hildon_volumebar_range_keypress;

    object_class->set_property = hildon_volumebar_range_set_property;
    object_class->get_property = hildon_volumebar_range_get_property; 

    /**
     * HildonVolumebarRange:level:
     *
     * Current volume level. 
     */
    g_object_class_install_property (object_class,
            PROP_LEVEL,
            g_param_spec_double ("level",
                "Level",
                "Current volume level",
                VOLUMEBAR_RANGE_MINIMUM_VALUE,
                VOLUMEBAR_RANGE_MAXIMUM_VALUE,
                VOLUMEBAR_RANGE_INITIAL_VALUE,
                G_PARAM_READWRITE));
    return;
}

static void 
hildon_volumebar_range_init                     (HildonVolumebarRange *volumerange)
{
  /* do nothing. */
}

static void
hildon_volumebar_range_set_property             (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec)
{
    HildonVolumebarRange *range = HILDON_VOLUMEBAR_RANGE (object);

    switch (prop_id) {
        case PROP_LEVEL:
            hildon_volumebar_range_set_level (range, g_value_get_double (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;

    }
}

static void
hildon_volumebar_range_get_property             (GObject *object,
                                                 guint prop_id,
                                                 GValue *value,
                                                 GParamSpec *pspec)
{
    HildonVolumebarRange *range = HILDON_VOLUMEBAR_RANGE (object);

    switch (prop_id) {

        case PROP_LEVEL:
            g_value_set_double (value, hildon_volumebar_range_get_level(range));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static gboolean
hildon_volumebar_range_keypress                 (GtkWidget *widget,
                                                 GdkEventKey *event)
{
    /* Accept arrow keys only if they match the orientation of the widget */
    if (GTK_RANGE (widget)->orientation == GTK_ORIENTATION_HORIZONTAL)
    {
        if (event->keyval == GDK_Up || event->keyval == GDK_Down) {
            return FALSE;
        }
    }
    else
    {
        if (event->keyval == GDK_Left || event->keyval == GDK_Right) {
            return FALSE;
        }
    }

    return ((GTK_WIDGET_CLASS (parent_class)->key_press_event) (widget,
                event));
}

GtkWidget*
hildon_volumebar_range_new                      (GtkOrientation orientation)
{
    GtkAdjustment * adjustment = GTK_ADJUSTMENT (gtk_adjustment_new (VOLUMEBAR_RANGE_INITIAL_VALUE,
                VOLUMEBAR_RANGE_MINIMUM_VALUE,
                VOLUMEBAR_RANGE_MAXIMUM_VALUE,
                VOLUMEBAR_RANGE_STEP_INCREMENT_VALUE,
                VOLUMEBAR_RANGE_PAGE_INCREMENT_VALUE,
                VOLUMEBAR_RANGE_PAGE_SIZE_VALUE));

    HildonVolumebarRange *self =
        g_object_new(HILDON_TYPE_VOLUMEBAR_RANGE,
                "adjustment", adjustment,
                NULL);

    GTK_RANGE (self)->orientation = orientation;

    /* Default vertical range is upside down for purposes of this widget */
    gtk_range_set_inverted (GTK_RANGE (self),
            (orientation == GTK_ORIENTATION_VERTICAL));

    return GTK_WIDGET(self);
}

gdouble 
hildon_volumebar_range_get_level                (HildonVolumebarRange *self)
{
    g_return_val_if_fail (HILDON_IS_VOLUMEBAR_RANGE(self), -1.0);

    return gtk_adjustment_get_value (gtk_range_get_adjustment(GTK_RANGE (self)));
}

void 
hildon_volumebar_range_set_level                (HildonVolumebarRange * self,
                                                 gdouble level)
{
    GtkAdjustment *adjustment;

    g_return_if_fail (HILDON_IS_VOLUMEBAR_RANGE (self));

    adjustment = gtk_range_get_adjustment (GTK_RANGE (self));

    /* Check that value has actually changed. Note that it's not safe to
     * just compare if floats are equivalent or not */
    if (ABS (gtk_adjustment_get_value (adjustment) - level) > CHANGE_THRESHOLD) {
        gtk_adjustment_set_value(adjustment, level);
    }
}

static gint 
hildon_volumebar_range_button_press_event       (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    gboolean result = FALSE;

    /* FIXME: By default, clicking left mouse button on GtkRange moves the
       slider by one step towards the click location. However, we want stylus
       taps to move the slider to the position of the tap, which by default
       is the middle button behaviour. To avoid breaking default GtkRange
       behaviour, this has been implemented by faking a middle button press. */

    event->button = (event->button == 1) ? 2 : event->button;
    if (GTK_WIDGET_CLASS (parent_class)->button_press_event) {
        result = GTK_WIDGET_CLASS (parent_class)->button_press_event(widget, event);
    }

    return result;
}

static gint
hildon_volumebar_range_button_release_event     (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    gboolean result = FALSE;

    /* FIXME: By default, clicking left mouse button on GtkRange moves the
       slider by one step towards the click location. However, we want stylus
       taps to move the slider to the position of the tap, which by default
       is the middle button behaviour. To avoid breaking default GtkRange
       behaviour, this has been implemented by faking a middle button press. */

    event->button = event->button == 1 ? 2 : event->button;
    if (GTK_WIDGET_CLASS (parent_class)->button_release_event) {
        result = GTK_WIDGET_CLASS(parent_class)->button_release_event(widget, event);
    }

    return result;
}

