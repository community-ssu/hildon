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
 * @file hildon-volumebar.c
 *
 * This file contains the implementation of the Hildon Volumebar.
 * It is a base class for Hildon Vertical Volumebar and for
 * Hildon Horizontal Volumebar classes.
 */
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

static void mute_toggled (HildonVolumebar *self, gpointer data);

static gboolean
hildon_volumebar_key_press(GtkWidget * widget,
                           GdkEventKey * event,
                           gpointer data);


enum 
{
    MUTE_TOGGLED_SIGNAL,
    LEVEL_CHANGED_SIGNAL,
    LAST_SIGNAL
};

enum {
    PROP_NONE = 0,
    PROP_HILDON_HAS_MUTE,
    PROP_HILDON_FOCUSABLE
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
    GtkContainerClass *container_class =
        GTK_CONTAINER_CLASS(volumebar_class);
    GObjectClass *object_class = G_OBJECT_CLASS(volumebar_class);

    parent_class = g_type_class_peek_parent(volumebar_class);

    g_type_class_add_private(volumebar_class,
                             sizeof(HildonVolumebarPrivate));

    /* Because we derived our widget from GtkContainer, we should also
       override forall method */
    container_class->forall = hildon_child_forall;
    GTK_OBJECT_CLASS(volumebar_class)->destroy = hildon_volumebar_destroy;

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
    
    object_class->set_property = hildon_volumebar_set_property;
    object_class->get_property = hildon_volumebar_get_property; 

    /*This kind of property could be usefull in the gtkcontainer*/
    g_object_class_install_property(object_class,
                                    PROP_HILDON_FOCUSABLE, 
                                    g_param_spec_boolean("can-focus",
                                    "The widget focusablility",
                                "The widget focusablility. TRUE is focusable",
                                    TRUE,
                                    G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
                                    PROP_HILDON_HAS_MUTE, 
                                    g_param_spec_boolean("has_mute",
                                    "Show/Hide the mute button",
               "Whether the mute button is visible. Default value: TRUE",
                                    TRUE,
                                    G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

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
    
    priv->tbutton = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
    g_object_set (G_OBJECT (priv->tbutton), "can-focus", FALSE, NULL);
    g_signal_connect (G_OBJECT (volumebar), "mute_toggled",
                      G_CALLBACK (mute_toggled), NULL);
    
    /* set keypress handler (select hardkey) */
    g_signal_connect(G_OBJECT(volumebar), "key-press-event",
            G_CALLBACK(hildon_volumebar_key_press),
            NULL);

}

static void
hildon_child_forall(GtkContainer * container,
                    gboolean include_internals,
                    GtkCallback callback, gpointer callback_data)
{
    HildonVolumebar *vbar;
    HildonVolumebarPrivate *priv;

    vbar = HILDON_VOLUMEBAR(container);
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(vbar);

    g_return_if_fail(callback != NULL);

    if (!include_internals)
        return;

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
    HildonVolumebar *vbar = HILDON_VOLUMEBAR(object);
    HildonVolumebarPrivate *priv;
    gboolean has_mute = TRUE;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(vbar)

    switch (prop_id) {
    case PROP_HILDON_HAS_MUTE:
        has_mute = g_value_get_boolean(value);

        if (has_mute)
        {
            gtk_widget_show(GTK_WIDGET(priv->tbutton));

        }
        else
        {
            gtk_widget_hide(GTK_WIDGET(priv->tbutton));
        }
        
        break;
    case PROP_HILDON_FOCUSABLE:
        g_object_set( G_OBJECT(priv->volumebar), "can-focus", 
                      g_value_get_boolean(value), NULL );
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
  HildonVolumebarPrivate *priv;
  priv = HILDON_VOLUMEBAR_GET_PRIVATE(object)
    switch (prop_id) {
    case PROP_HILDON_HAS_MUTE:
        g_value_set_boolean(value, g_value_get_boolean(value));
        break;
     case PROP_HILDON_FOCUSABLE:
        g_value_set_boolean(value, g_value_get_boolean(value));
        g_object_set( G_OBJECT(priv->volumebar), "can-focus", 
                      g_value_get_boolean(value), NULL );
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/**
 * hildon_volumebar_level_change:
 * @self: a HildonVolumebar widget.
 *
 * Emits "level_changed" signal to the given volumebar. This function
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
 * @self: volumebar to change level on
 * @level: new level
 *
 * Sets new volumelevel for this #HildonVolumebar.
 **/
void 
hildon_volumebar_set_level(HildonVolumebar * self, gdouble level)
{
    HildonVolumebarPrivate *priv;

    g_return_if_fail(self);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
   
    /* No need to clamp the level to legal values here as volumebarrange
     * will do it anyway. And here we don't know the correct values anyway.
     */
    hildon_volumebar_range_set_level(priv->volumebar, level);
}

/**
 * hildon_volumebar_get_level:
 * @self: volumebar to query level on
 *
 * Gets the volumelevel of this #HildonVolumebar.
 *
 * Return value: Volume level.
 **/
gdouble 
hildon_volumebar_get_level(HildonVolumebar * self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(self, -1);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    return hildon_volumebar_range_get_level(priv->volumebar);
}

/**
 * hildon_volumebar_set_mute:
 * @self: volumebar to work on
 * @mute: mute ON/OFF
 *
 * Sets mute status for this #HildonVolumebar.
 *
 **/
void 
hildon_volumebar_set_mute(HildonVolumebar * self, gboolean mute)
{
    HildonVolumebarPrivate *priv;

    g_return_if_fail(self);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
    gtk_widget_set_sensitive(GTK_WIDGET(priv->volumebar), !mute);
    
    if (mute){   
        g_object_set (G_OBJECT (priv->tbutton), "can-focus", TRUE, NULL);
        gtk_widget_grab_focus (GTK_WIDGET(priv->tbutton));
    }
    else
    {
        g_object_set (G_OBJECT (priv->tbutton), "can-focus", FALSE, NULL);
        gtk_widget_grab_focus (GTK_WIDGET (self));
    }
}

/**
 * hildon_volumebar_get_mute:
 * @self: volumebar to query mute status
 *
 * Gets mute status of this #HildonVolumebar (ON/OFF).
 *
 * Return value: Mute status as #gboolean value.
 **/
gboolean 
hildon_volumebar_get_mute(HildonVolumebar * self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(self, TRUE);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    return gtk_toggle_button_get_active(priv->tbutton);
}

/**
 * hildon_volumebar_get_adjustment
 * @self : A #HildonVolumebar
 * 
 * Gets the GtkAdjustment used in volumebar. This can be handy
 * to give to hildon_appview_set_connected_adjustment which
 * will allow changing the volume with increase / decrease
 * hardware buttons.
 *
 * This is a temporary solution until volumebar is restructured to
 * be a child class of GtkRange.
 * 
 * Return value: A @GtkAdjustment used by volumebar.
 **/
GtkAdjustment * 
hildon_volumebar_get_adjustment (HildonVolumebar * self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(self, NULL);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);

    return gtk_range_get_adjustment (GTK_RANGE (priv->volumebar));
}

static void
mute_toggled (HildonVolumebar *self, gpointer data)
{
  /* only call hildon_volumebar_set_mute. everything is done there */
  hildon_volumebar_set_mute (self, hildon_volumebar_get_mute(self));
}

static gboolean
hildon_volumebar_key_press (GtkWidget * widget,
                            GdkEventKey * event,
                            gpointer data)
{
    HildonVolumebarPrivate *priv;
    
    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(event, FALSE);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    
    if (event->keyval == GDK_Return) {
        gtk_toggle_button_set_active(priv->tbutton, 
                !hildon_volumebar_get_mute(HILDON_VOLUMEBAR(widget)));
        return TRUE;
    }

    return FALSE;
}
