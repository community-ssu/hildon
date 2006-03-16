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

/**
 * SECTION:hildon-volumebar
 * @short_description: Base class for widgets that display a volume bar
 * @see_also: #HildonHVolumebar, #HildonVVolumebar
 *
 * #HildonVolumebar is a base class for widgets that display a volume bar that
 * allows increasing or decreasing volume within a predefined range, and muting
 * the volume when users click the mute icon.
 */

#include <gtk/gtkwindow.h>
#include <gtk/gtksignal.h>
#include <gdk/gdkkeysyms.h>

#include "hildon-volumebar.h"
#include "hildon-volumebar-range.h"
#include "hildon-volumebar-private.h"

static GtkContainerClass *parent_class;

static void
hildon_volumebar_class_init(HildonVolumebarClass * volumebar_class);       
static void 
hildon_volumebar_init(HildonVolumebar * volumebar);

static void 
hildon_child_forall(GtkContainer * container,
		    gboolean include_internals,
		    GtkCallback callback,
		    gpointer callback_data);
static void 
hildon_volumebar_destroy(GtkObject * self);

static void hildon_volumebar_set_property(GObject * object,
                                           guint prop_id,
                                           const GValue * value,
                                           GParamSpec * pspec);
static void hildon_volumebar_get_property(GObject * object,
                                           guint prop_id,
                                           GValue * value, 
                                           GParamSpec * pspec);

static void mute_toggled (HildonVolumebar *self);

static gboolean
hildon_volumebar_key_press(GtkWidget * widget,
                           GdkEventKey * event);


enum 
{
    MUTE_TOGGLED_SIGNAL,
    LEVEL_CHANGED_SIGNAL,
    LAST_SIGNAL
};

enum {
    PROP_NONE = 0,
    PROP_HILDON_HAS_MUTE,
    PROP_HILDON_FOCUSABLE,
    PROP_HILDON_LEVEL,
    PROP_HILDON_MUTE
};

static guint signals[LAST_SIGNAL] = { 0 };

GType 
hildon_volumebar_get_type(void)
{
    static GType volumebar_type = 0;

    if (!volumebar_type) {
        static const GTypeInfo volumebar_info = {
            sizeof(HildonVolumebarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_volumebar_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonVolumebar),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_volumebar_init,
        };
        volumebar_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                                "HildonVolumebar",
                                                &volumebar_info, 0);
    }
    return volumebar_type;
}

static void 
hildon_volumebar_class_init(HildonVolumebarClass *volumebar_class)
{
    GObjectClass      *gobject_class   = G_OBJECT_CLASS  (volumebar_class);
    GtkObjectClass    *object_class    = GTK_OBJECT_CLASS(volumebar_class);
    GtkWidgetClass    *widget_class    = GTK_WIDGET_CLASS(volumebar_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(volumebar_class);

    parent_class = g_type_class_peek_parent(volumebar_class);

    g_type_class_add_private(volumebar_class,
                             sizeof(HildonVolumebarPrivate));

    /* Because we derived our widget from GtkContainer, we should also
       override forall method */
    volumebar_class->mute_toggled = mute_toggled;
    container_class->forall = hildon_child_forall;
    widget_class->key_press_event = hildon_volumebar_key_press;
    object_class->destroy = hildon_volumebar_destroy;

    signals[MUTE_TOGGLED_SIGNAL] = g_signal_new("mute_toggled",
                                                G_OBJECT_CLASS_TYPE
                                                (object_class),
                                                G_SIGNAL_RUN_LAST |
                                                G_SIGNAL_ACTION,
                                                G_STRUCT_OFFSET
                                                (HildonVolumebarClass,
                                                 mute_toggled), NULL, NULL,
                                                gtk_marshal_VOID__VOID,
                                                G_TYPE_NONE, 0);

    signals[LEVEL_CHANGED_SIGNAL] = g_signal_new("level_changed",
                                                 G_OBJECT_CLASS_TYPE
                                                 (object_class),
                                                 G_SIGNAL_RUN_LAST |
                                                 G_SIGNAL_ACTION,
                                                 G_STRUCT_OFFSET
                                                 (HildonVolumebarClass,
                                                  level_changed), NULL,
                                                 NULL,
                                                 gtk_marshal_VOID__VOID,
                                                 G_TYPE_NONE, 0);
    
    gobject_class->set_property = hildon_volumebar_set_property;
    gobject_class->get_property = hildon_volumebar_get_property; 

    /*This kind of property could be usefull in the gtkcontainer*/
    g_object_class_install_property(gobject_class,
                                    PROP_HILDON_FOCUSABLE, 
                                    g_param_spec_boolean("can-focus",
                                    "The widget focusablility",
                                "The widget focusablility. TRUE is focusable",
                                    TRUE,
                                    G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class,
                                    PROP_HILDON_HAS_MUTE, 
                                    g_param_spec_boolean("has_mute",
                                    "Show/Hide the mute button",
               "Whether the mute button is visible. Default value: TRUE",
                                    TRUE,
                                    G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class,
				    PROP_HILDON_LEVEL,
				    g_param_spec_double("level",
							"Level",
							"Current volume level",
							0.0,
							100.0,
							50.0,
							G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class,
				    PROP_HILDON_MUTE,
				    g_param_spec_boolean("mute",
							 "Mute",
							 "Whether volume is muted",
							 FALSE,
							 G_PARAM_READWRITE));
}

static void 
hildon_volumebar_init(HildonVolumebar * volumebar)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(volumebar);

    /* Should set GTK_NO_WINDOW flag, because widget is derived from
       GtkContainer */
    GTK_WIDGET_SET_FLAGS(GTK_WIDGET(volumebar), GTK_NO_WINDOW);
    GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(volumebar), GTK_CAN_FOCUS);

    /* Initialize mute button */
    priv->tbutton = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
    g_object_set (G_OBJECT (priv->tbutton), "can-focus", FALSE, NULL);
}

static void
hildon_child_forall(GtkContainer * container,
                    gboolean include_internals,
                    GtkCallback callback, gpointer callback_data)
{
    HildonVolumebarPrivate *priv;

    g_assert(HILDON_IS_VOLUMEBAR(container));
    g_assert(callback != NULL);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(container);

    /* No external children */
    if (!include_internals)
        return;

    /* Execute callback for both internals */
    (*callback) (GTK_WIDGET(priv->tbutton), callback_data);
    (*callback) (GTK_WIDGET(priv->volumebar), callback_data);
}

static void 
hildon_volumebar_destroy(GtkObject * self)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    if (priv->tbutton) {
        gtk_widget_unparent(GTK_WIDGET(priv->tbutton));
        priv->tbutton = NULL;
    }
    if (priv->volumebar) {
        gtk_widget_unparent(GTK_WIDGET(priv->volumebar));
        priv->volumebar = NULL;
    }

    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        GTK_OBJECT_CLASS(parent_class)->destroy(self);
}

static void
hildon_volumebar_set_property(GObject * object,
                              guint prop_id,
                              const GValue * value, 
                              GParamSpec * pspec)
{  
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_HILDON_HAS_MUTE:
        /* Mute button always exists, but might be hidden */
        if (g_value_get_boolean(value))
            gtk_widget_show(GTK_WIDGET(priv->tbutton));
        else
            gtk_widget_hide(GTK_WIDGET(priv->tbutton));
        break;
    case PROP_HILDON_FOCUSABLE:
        g_object_set( G_OBJECT(priv->volumebar), "can-focus", 
                      g_value_get_boolean(value), NULL );
        break;
    case PROP_HILDON_LEVEL:
        hildon_volumebar_set_level(HILDON_VOLUMEBAR(priv->volumebar),
				   g_value_get_double(value));
        break;
    case PROP_HILDON_MUTE:
        hildon_volumebar_set_mute(HILDON_VOLUMEBAR(priv->volumebar),
				  g_value_get_boolean(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);

        break;
    }
}

static void
hildon_volumebar_get_property(GObject * object,
                              guint prop_id, GValue * value, 
                              GParamSpec * pspec)
{
    HildonVolumebar *vb = HILDON_VOLUMEBAR(object);
    HildonVolumebarPrivate *priv = HILDON_VOLUMEBAR_GET_PRIVATE(vb);

    switch (prop_id) {
    case PROP_HILDON_HAS_MUTE:
        g_value_set_boolean(value, GTK_WIDGET_VISIBLE(priv->tbutton));
        break;
    case PROP_HILDON_FOCUSABLE:
        g_value_set_boolean(value, GTK_WIDGET_CAN_FOCUS(priv->volumebar));
	break;
    case PROP_HILDON_LEVEL:
        g_value_set_double(value, hildon_volumebar_get_level(vb));
	break;
    case PROP_HILDON_MUTE:
        g_value_set_boolean(value, hildon_volumebar_get_mute(vb));
	break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/**
 * hildon_volumebar_level_change:
 * @self: a #HildonVolumebar widget
 *
 * Emits "level_changed" signal to the given volume bar. This function
 * is mainly used by derived classes.
 */
void 
hildon_volumebar_level_change(HildonVolumebar * self)
{
    g_return_if_fail(HILDON_IS_VOLUMEBAR(self));
    g_signal_emit_by_name(GTK_WIDGET(self), "level_changed");
}

/**
 * hildon_volumebar_set_level:
 * @self: volume bar to change level on
 * @level: new level
 *
 * Sets new volume level for this #HildonVolumebar.
 */
void 
hildon_volumebar_set_level(HildonVolumebar * self, gdouble level)
{
    HildonVolumebarPrivate *priv;

    g_return_if_fail(HILDON_IS_VOLUMEBAR(self));

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
   
    hildon_volumebar_range_set_level(priv->volumebar, level);
}

/**
 * hildon_volumebar_get_level:
 * @self: volume bar to query level on
 *
 * Gets the volume level of this #HildonVolumebar.
 *
 * Returns: volume level or -1 on error
 */
gdouble 
hildon_volumebar_get_level(HildonVolumebar * self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(HILDON_IS_VOLUMEBAR(self), -1);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    return hildon_volumebar_range_get_level(priv->volumebar);
}

/**
 * hildon_volumebar_set_mute:
 * @self: volume bar to work on
 * @mute: mute ON/OFF
 *
 * Sets mute status for this #HildonVolumebar.
 */
void 
hildon_volumebar_set_mute(HildonVolumebar * self, gboolean mute)
{
    HildonVolumebarPrivate *priv;
    gboolean focusable = TRUE;
    
    g_return_if_fail(HILDON_IS_VOLUMEBAR(self));

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    /* Slider should be insensitive when mute is on */
    gtk_widget_set_sensitive(GTK_WIDGET(priv->volumebar), !mute);
    
    focusable = GTK_WIDGET_CAN_FOCUS (GTK_WIDGET (priv->volumebar));
    
    if (mute){   
        if (focusable){
	    /* Make mute button focusable since the slider isn't anymore */
            g_object_set (G_OBJECT (priv->tbutton), "can-focus", TRUE, NULL);
            gtk_widget_grab_focus (GTK_WIDGET(priv->tbutton));
        }
    }
    else
    {
        g_object_set (G_OBJECT (priv->tbutton), "can-focus", FALSE, NULL);
        
	/* Mute off grabs focus */
        if (focusable){
            gtk_widget_grab_focus (GTK_WIDGET (self));
        }
        else{
	    /* If volumebar is not focusable, focus the parent window instead */
            GtkWidget *win = gtk_widget_get_ancestor (GTK_WIDGET (self), 
                                                      GTK_TYPE_WINDOW);
            gtk_window_set_focus (GTK_WINDOW (win), NULL);
        }
    }

    /* Update mute button state and redraw */
    gtk_toggle_button_set_active(priv->tbutton, mute);

    gtk_widget_queue_draw (GTK_WIDGET (self));
}

/**
 * hildon_volumebar_get_mute:
 * @self: volume bar to query mute status
 *
 * Gets mute status of this #HildonVolumebar (ON/OFF).
 *
 * Returns: Mute status as #gboolean value.
 */
gboolean 
hildon_volumebar_get_mute(HildonVolumebar * self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(HILDON_IS_VOLUMEBAR(self), TRUE);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    return gtk_toggle_button_get_active(priv->tbutton);
}

/**
 * hildon_volumebar_get_adjustment
 * @self : a #HildonVolumebar
 * 
 * Gets the GtkAdjustment used in volume bar. This can be handy
 * to give to hildon_appview_set_connected_adjustment which
 * will allow changing the volume with increase / decrease
 * hardware buttons.
 *
 * This is a temporary solution until volume bar is restructured to
 * be a child class of GtkRange.
 * 
 * Returns: a #GtkAdjustment used by volume bar.
 */
GtkAdjustment * 
hildon_volumebar_get_adjustment (HildonVolumebar * self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(HILDON_IS_VOLUMEBAR(self), NULL);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    return gtk_range_get_adjustment (GTK_RANGE (priv->volumebar));
}

static void
mute_toggled (HildonVolumebar *self)
{
  /* This looks like no-op, but it still does something meaningfull!
     set_mute also updates the ui to match new state that
     is already reported by get_mute */
  hildon_volumebar_set_mute (self, hildon_volumebar_get_mute(self));
}

static gboolean
hildon_volumebar_key_press (GtkWidget * widget,
                            GdkEventKey * event)
{
    HildonVolumebarPrivate *priv;
    
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);

    /* Enter key toggles mute button (unless it is hidden) */
    if (event->keyval == GDK_Return && GTK_WIDGET_VISIBLE(priv->tbutton)) {
        gtk_toggle_button_set_active(priv->tbutton, 
                !hildon_volumebar_get_mute(HILDON_VOLUMEBAR(widget)));
        return TRUE;
    }

    return GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);
}

/* Sends mute-toggled signal to widget, used as a callback in derived classes
   Just keep this "protected" in order to avoid introducing new API. */
void
_hildon_volumebar_mute_toggled(HildonVolumebar * self)
{
    g_return_if_fail(HILDON_IS_VOLUMEBAR(self));
    g_signal_emit_by_name(self, "mute_toggled");
}
