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
 * SECTION:hildon-volumebar
 * @short_description: Base class for widgets that display a volume bar.
 * @see_also: #HildonHVolumebar, #HildonVVolumebar
 *
 * #HildonVolumebar is a base class for widgets that display a volume bar that
 * allows increasing or decreasing volume within a predefined range, and muting
 * the volume when users click the mute icon.
 */

#undef                                          HILDON_DISABLE_DEPRECATED

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        <gdk/gdkkeysyms.h>

#include                                        "hildon-volumebar.h"
#include                                        "hildon-volumebar-range.h"
#include                                        "hildon-volumebar-private.h"

static GtkContainerClass*                       parent_class;

static void
hildon_volumebar_class_init                     (HildonVolumebarClass* volumebar_class);       

static void 
hildon_volumebar_init                           (HildonVolumebar* volumebar);

static void 
hildon_child_forall                             (GtkContainer * container,
                                                 gboolean include_internals,
                                                 GtkCallback callback,
                                                 gpointer callback_data);

static void 
hildon_volumebar_destroy                        (GtkObject *self);

static void 
hildon_volumebar_set_property                   (GObject* object,
                                                 guint prop_id,
                                                 const GValue* value,
                                                 GParamSpec* pspec);

static void 
hildon_volumebar_get_property                   (GObject * object,
                                                 guint prop_id,
                                                 GValue* value, 
                                                 GParamSpec* pspec);

static void 
mute_toggled                                    (HildonVolumebar *self);

static gboolean
hildon_volumebar_key_press                      (GtkWidget* widget,
                                                 GdkEventKey* event);

static void 
hildon_volumebar_size_allocate                  (GtkWidget *widget,
                                                 GtkAllocation *allocation);

static void 
hildon_volumebar_realize                        (GtkWidget *widget);

static void 
hildon_volumebar_unrealize                      (GtkWidget *widget);

static void 
hildon_volumebar_map                            (GtkWidget *widget);

static void 
hildon_volumebar_unmap                          (GtkWidget *widget);

static void
hildon_volumebar_grab_focus                     (GtkWidget *widget);

static gboolean
hildon_volumebar_focus                          (GtkWidget *widget,
                                                 GtkDirectionType direction);

static void 
hildon_volumebar_notify                         (GObject *self, GParamSpec *param);

enum 
{
    MUTE_TOGGLED_SIGNAL,
    LEVEL_CHANGED_SIGNAL,
    LAST_SIGNAL
};

enum {
    PROP_0,
    PROP_HILDON_HAS_MUTE,
    PROP_HILDON_LEVEL,
    PROP_HILDON_MUTE
};

static guint                                    signals [LAST_SIGNAL] = { 0 };

/**
 * hildon_volumebar_get_type:
 *
 * Initializes and returns the type of a hildon volumebar.
 *
 * Returns: GType of #HildonVolumebar
 */
GType G_GNUC_CONST 
hildon_volumebar_get_type                       (void)
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
hildon_volumebar_class_init                     (HildonVolumebarClass *volumebar_class)
{
    GObjectClass      *gobject_class    = G_OBJECT_CLASS  (volumebar_class);
    GtkObjectClass    *object_class     = GTK_OBJECT_CLASS (volumebar_class);
    GtkWidgetClass    *widget_class     = GTK_WIDGET_CLASS (volumebar_class);
    GtkContainerClass *container_class  = GTK_CONTAINER_CLASS (volumebar_class);

    parent_class = g_type_class_peek_parent (volumebar_class);

    g_type_class_add_private (volumebar_class,
            sizeof (HildonVolumebarPrivate));

   /* Because we derived our widget from GtkContainer, we should also
    * override forall method 
    */
   volumebar_class->mute_toggled        = mute_toggled;
   container_class->forall              = hildon_child_forall;
   widget_class->size_allocate          = hildon_volumebar_size_allocate;
   widget_class->realize                = hildon_volumebar_realize;
   widget_class->unrealize              = hildon_volumebar_unrealize;
   widget_class->map                    = hildon_volumebar_map;
   widget_class->unmap                  = hildon_volumebar_unmap;
   widget_class->grab_focus             = hildon_volumebar_grab_focus;
   widget_class->focus                  = hildon_volumebar_focus;
   widget_class->key_press_event        = hildon_volumebar_key_press;
   object_class->destroy                = hildon_volumebar_destroy;

   signals[MUTE_TOGGLED_SIGNAL] = g_signal_new ("mute_toggled",
           G_OBJECT_CLASS_TYPE
           (object_class),
           G_SIGNAL_RUN_LAST |
           G_SIGNAL_ACTION,
           G_STRUCT_OFFSET
           (HildonVolumebarClass,
            mute_toggled), NULL, NULL,
           g_cclosure_marshal_VOID__VOID,
           G_TYPE_NONE, 0);

   signals[LEVEL_CHANGED_SIGNAL] = g_signal_new ("level_changed",
           G_OBJECT_CLASS_TYPE
           (object_class),
           G_SIGNAL_RUN_LAST |
           G_SIGNAL_ACTION,
           G_STRUCT_OFFSET
           (HildonVolumebarClass,
            level_changed), NULL,
           NULL,
           g_cclosure_marshal_VOID__VOID,
           G_TYPE_NONE, 0);

   gobject_class->notify                = hildon_volumebar_notify;
   gobject_class->set_property          = hildon_volumebar_set_property;
   gobject_class->get_property          = hildon_volumebar_get_property; 

   /**
    * HildonVolumebar:has-mute:
    *
    * Whether the mute button is visibile.
    */
   g_object_class_install_property (gobject_class,
           PROP_HILDON_HAS_MUTE, 
           g_param_spec_boolean ("has_mute",
               "Show/Hide the mute button",
               "Whether the mute button is visible. Default value: TRUE",
               TRUE,
               G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

   /**
    * HildonVolumebar:level:
    *
    * Current volume level.
    */
   g_object_class_install_property (gobject_class,
           PROP_HILDON_LEVEL,
           g_param_spec_double ("level",
               "Level",
               "Current volume level",
               0.0,
               100.0,
               50.0,
               G_PARAM_READWRITE));

   /**
    * HildonVolumebar:mute:
    *
    * Whether volume is muted.
    */
   g_object_class_install_property (gobject_class,
           PROP_HILDON_MUTE,
           g_param_spec_boolean ("mute",
               "Mute",
               "Whether volume is muted",
               FALSE,
               G_PARAM_READWRITE));
}

static void 
hildon_volumebar_init                           (HildonVolumebar *volumebar)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(volumebar);
    g_assert (priv);

    /* Should set GTK_NO_WINDOW flag, because widget is derived from
       GtkContainer */
    GTK_WIDGET_SET_FLAGS (GTK_WIDGET (volumebar), GTK_NO_WINDOW);
    GTK_WIDGET_SET_FLAGS (GTK_WIDGET (volumebar), GTK_CAN_FOCUS);

    /* Initialize mute button */
    priv->tbutton = GTK_TOGGLE_BUTTON (gtk_toggle_button_new ());
    g_object_set (G_OBJECT (priv->tbutton), "can-focus", FALSE, NULL);
}

static void
hildon_volumebar_size_allocate                  (GtkWidget *widget, 
                                                 GtkAllocation *allocation)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv);

    if (GTK_WIDGET_CLASS (parent_class)->size_allocate)
        GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);

    if (GTK_WIDGET_REALIZED (widget))
        gdk_window_move_resize (priv->event_window,
                allocation->x, allocation->y,
                allocation->width, allocation->height);
}

static void
hildon_volumebar_realize                        (GtkWidget *widget)
{
    HildonVolumebarPrivate *priv;
    GdkWindowAttr attributes;
    gint attributes_mask;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (widget);
    g_assert (priv);

    GTK_WIDGET_CLASS (parent_class)->realize (widget);

    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_ONLY;
    attributes.event_mask = GDK_BUTTON_PRESS_MASK;

    attributes_mask = GDK_WA_X | GDK_WA_Y;

    priv->event_window = gdk_window_new (widget->window,
            &attributes, attributes_mask);

    gdk_window_set_user_data (priv->event_window, widget);
}

static void
hildon_volumebar_unrealize                      (GtkWidget *widget)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv);

    if (priv->event_window) {
        gdk_window_set_user_data (priv->event_window, NULL);
        gdk_window_destroy (priv->event_window);
        priv->event_window = NULL;
    }

    GTK_WIDGET_CLASS (parent_class)->unrealize(widget);
}

static void
hildon_volumebar_map                            (GtkWidget *widget)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv);

    GTK_WIDGET_CLASS (parent_class)->map (widget);

    /* the event window must be on top of all other widget windows, so show it
     * last */
    if (! GTK_WIDGET_SENSITIVE (widget))
        gdk_window_show (priv->event_window);
}

static void 
hildon_volumebar_unmap                          (GtkWidget *widget)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (widget);
    g_assert (priv);

    gdk_window_hide (priv->event_window);

    GTK_WIDGET_CLASS (parent_class)->unmap(widget);
}

static void 
hildon_volumebar_grab_focus                     (GtkWidget *widget)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (widget);
    g_assert (priv);

    if (GTK_WIDGET_CAN_FOCUS (widget)) {
        if (gtk_toggle_button_get_active (priv->tbutton))
            gtk_widget_grab_focus (GTK_WIDGET (priv->tbutton));
        else
           gtk_widget_grab_focus (GTK_WIDGET (priv->volumebar));
    }
}

static gboolean
hildon_volumebar_focus                          (GtkWidget *widget,
                                                 GtkDirectionType direction)
{
    HildonVolumebarPrivate *priv;
    GtkOrientation orientation;
    gboolean has_focus;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (widget);
    g_assert (priv);

    orientation = GTK_RANGE (priv->volumebar)->orientation;

    has_focus = (GTK_WIDGET_HAS_FOCUS (GTK_WIDGET (priv->volumebar)) ||
                 GTK_WIDGET_HAS_FOCUS (GTK_WIDGET (priv->tbutton)));

    switch (direction) {
        case GTK_DIR_UP:
        case GTK_DIR_DOWN:
        case GTK_DIR_TAB_FORWARD:
        case GTK_DIR_TAB_BACKWARD:
            if (has_focus && orientation == GTK_ORIENTATION_HORIZONTAL)
                return FALSE;
            break;

        case GTK_DIR_LEFT:
        case GTK_DIR_RIGHT:
            if (has_focus && orientation == GTK_ORIENTATION_VERTICAL)
                return FALSE;
            break;

        default:
            break;
    }

    return GTK_WIDGET_CLASS (parent_class)->focus (widget, direction);
}

static void
hildon_child_forall                             (GtkContainer *container,
                                                 gboolean include_internals,
                                                 GtkCallback callback, 
                                                 gpointer callback_data)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (container);
    g_assert (callback != NULL);
    g_assert (priv);

    /* No external children */
    if (! include_internals)
        return;

    /* Execute callback for both internals */
    (*callback) (GTK_WIDGET (priv->tbutton), callback_data);
    (*callback) (GTK_WIDGET (priv->volumebar), callback_data);
}

static void
hildon_volumebar_notify                         (GObject *self, 
                                                 GParamSpec *param)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
    g_assert (priv);

    if (g_str_equal (param->name, "can-focus")) {
        /* call set_mute() because that updates the widget's UI state */
        hildon_volumebar_set_mute (HILDON_VOLUMEBAR (self),
                                   hildon_volumebar_get_mute (HILDON_VOLUMEBAR (self)));
    }

    if (GTK_WIDGET_MAPPED (self)) {
        /* show/hide the event window on sensitivity change */
        if (g_str_equal (param->name, "sensitive")) {
            if (GTK_WIDGET_SENSITIVE (self))
                gdk_window_hide (priv->event_window);
            else
                gdk_window_show (priv->event_window);
        }
    }

    if (G_OBJECT_CLASS(parent_class)->notify)
        G_OBJECT_CLASS(parent_class)->notify (self, param);
}

static void 
hildon_volumebar_destroy                        (GtkObject *self)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
    g_assert (priv);

    if (priv->tbutton) {
        gtk_widget_unparent (GTK_WIDGET (priv->tbutton));
        priv->tbutton = NULL;
    }
    if (priv->volumebar) {
        gtk_widget_unparent (GTK_WIDGET (priv->volumebar));
        priv->volumebar = NULL;
    }

    if (GTK_OBJECT_CLASS (parent_class)->destroy)
        GTK_OBJECT_CLASS (parent_class)->destroy (self);
}

static void
hildon_volumebar_set_property                   (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec)
{  
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(object);
    g_assert (priv);

    switch (prop_id) {

        case PROP_HILDON_HAS_MUTE:
            /* Mute button always exists but might be hidden */
            if (g_value_get_boolean (value))
                gtk_widget_show (GTK_WIDGET (priv->tbutton));
            else
                gtk_widget_hide (GTK_WIDGET (priv->tbutton));
            break;

        case PROP_HILDON_LEVEL:
            hildon_volumebar_set_level (HILDON_VOLUMEBAR (object),
                    g_value_get_double (value));
            break;

        case PROP_HILDON_MUTE:
            hildon_volumebar_set_mute (HILDON_VOLUMEBAR (object),
                    g_value_get_boolean (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);

            break;
    }
}

static void
hildon_volumebar_get_property                   (GObject *object,
                                                 guint prop_id, 
                                                 GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonVolumebarPrivate *priv;
    HildonVolumebar *vb;
        
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(object);
    g_assert (priv);

    vb = HILDON_VOLUMEBAR (object);

    switch (prop_id) {

        case PROP_HILDON_HAS_MUTE:
            g_value_set_boolean (value, GTK_WIDGET_VISIBLE (priv->tbutton));
            break;

        case PROP_HILDON_LEVEL:
            g_value_set_double (value, hildon_volumebar_get_level (vb));
            break;

        case PROP_HILDON_MUTE:
            g_value_set_boolean (value, hildon_volumebar_get_mute (vb));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;

    }
}

/**
 * hildon_volumebar_set_level:
 * @self: volume bar to change level on
 * @level: new level
 *
 * Sets new volume level for this #HildonVolumebar.
 */
void 
hildon_volumebar_set_level                      (HildonVolumebar *self, 
                                                 gdouble level)
{
    HildonVolumebarPrivate *priv;

    g_return_if_fail(HILDON_IS_VOLUMEBAR (self));

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (self);
    g_assert (priv);

    hildon_volumebar_range_set_level (priv->volumebar, level);
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
hildon_volumebar_get_level                      (HildonVolumebar *self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(HILDON_IS_VOLUMEBAR (self), -1);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
    g_assert (priv);

    return hildon_volumebar_range_get_level (priv->volumebar);
}

/**
 * hildon_volumebar_set_mute:
 * @self: volume bar to work on
 * @mute: mute ON/OFF
 *
 * Sets mute status for this #HildonVolumebar.
 */
void 
hildon_volumebar_set_mute                       (HildonVolumebar *self, 
                                                 gboolean mute)
{
    HildonVolumebarPrivate *priv;
    gboolean has_focus;

    g_return_if_fail (HILDON_IS_VOLUMEBAR (self));

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (self);
    g_assert (priv);

    has_focus = (GTK_WIDGET_HAS_FOCUS (GTK_WIDGET (priv->volumebar)) ||
                 GTK_WIDGET_HAS_FOCUS (GTK_WIDGET (priv->tbutton)));

    /* Slider should be insensitive when mute is on */
    gtk_widget_set_sensitive (GTK_WIDGET (priv->volumebar), !mute);

    if (mute) {
        /* Make mute button focusable since the slider isn't anymore */
        g_object_set (G_OBJECT (priv->tbutton), "can-focus", TRUE, NULL);

        if (has_focus)
            gtk_widget_grab_focus (GTK_WIDGET (priv->tbutton));
    }
    else
    {
        g_object_set (G_OBJECT (priv->tbutton), "can-focus", FALSE, NULL);

        if (has_focus)
            gtk_widget_grab_focus (GTK_WIDGET (priv->volumebar));
    }

    /* Update mute button state and redraw */
    gtk_toggle_button_set_active (priv->tbutton, mute);
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
hildon_volumebar_get_mute                       (HildonVolumebar *self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail (HILDON_IS_VOLUMEBAR (self), TRUE);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
    g_assert (priv);

    return gtk_toggle_button_get_active (priv->tbutton);
}

/**
 * hildon_volumebar_get_adjustment
 * @self : a #HildonVolumebar
 * 
 * Gets the GtkAdjustment used in volume bar. This can be handy
 * to pass to hildon_appview_set_connected_adjustment which
 * will allow changing the volume with 'increase' / 'decrease'
 * hardware buttons.
 *
 * Returns: a #GtkAdjustment used by volume bar.
 */
GtkAdjustment* 
hildon_volumebar_get_adjustment                 (HildonVolumebar *self)
{
    HildonVolumebarPrivate *priv;

    g_return_val_if_fail(HILDON_IS_VOLUMEBAR(self), NULL);

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(self);
    g_assert (priv);

    return gtk_range_get_adjustment (GTK_RANGE (priv->volumebar));
}

static void
mute_toggled                                    (HildonVolumebar *self)
{
    /* This looks like no-op, but it still does something meaningfull!
       set_mute also updates the ui to match new state that
       is already reported by get_mute */

    hildon_volumebar_set_mute (self, hildon_volumebar_get_mute (self));
}

static gboolean
hildon_volumebar_key_press                      (GtkWidget *widget,
                                                 GdkEventKey *event)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv != NULL);

    /* Enter key toggles mute button (unless it is hidden) */
    if (event->keyval == GDK_Return && GTK_WIDGET_VISIBLE (priv->tbutton)) {
        gtk_toggle_button_set_active (priv->tbutton, 
                ! hildon_volumebar_get_mute(HILDON_VOLUMEBAR(widget)));

        return TRUE;
    }

    return GTK_WIDGET_CLASS (parent_class)->key_press_event (widget, event);
}

/* 
 * Sends mute-toggled signal to widget, used as a callback in derived classes.
 */
void G_GNUC_INTERNAL
hildon_volumebar_mute_toggled                   (HildonVolumebar * self)
{
    g_return_if_fail (HILDON_IS_VOLUMEBAR (self));
    /* FIXME Emit by id */
    g_signal_emit_by_name (self, "mute_toggled");
}

void G_GNUC_INTERNAL 
hildon_volumebar_level_change                   (HildonVolumebar *self)
{
    g_return_if_fail (HILDON_IS_VOLUMEBAR (self));

    /* FIXME Use numerical val, id */
    g_signal_emit_by_name (GTK_WIDGET (self), "level_changed");
}

/**
 * hildon_volumebar_set_range_insensitive_message:
 * @widget: A @GtkWidget to assign the banner to 
 * @message: A message to display to the user
 *
 * Used to asign an insensitive message to the slider of the given volumebar.
 * It simply calls hildon_helper_set_insensitive_message on the slider/range of 
 * the volumebar.
 *
 * Deprecated: As of hildon 2.2, it is strongly discouraged to use insensitive messages.
 */
void
hildon_volumebar_set_range_insensitive_message  (HildonVolumebar *widget,
                                                 const gchar *message)
{
    g_return_if_fail (HILDON_IS_VOLUMEBAR (widget));

    HildonVolumebarPrivate *priv;
    priv = HILDON_VOLUMEBAR_GET_PRIVATE (widget);

    hildon_helper_set_insensitive_message ((GtkWidget *) priv->volumebar, message);
}

/**
 * hildon_volumebar_set_range_insensitive_messagef:
 * @widget: A @GtkWidget to assign the banner to 
 * @format : a printf-like format string
 * @varargs : arguments for the format string
 *
 * A helper printf-like variant of hildon_helper_set_insensitive_message.
 *
 * Deprecated: As of hildon 2.2, it is strongly discouraged to use insensitive messages.
 */
void
hildon_volumebar_set_range_insensitive_messagef (HildonVolumebar *widget,
                                                 const gchar *format,
                                                 ...)
{
    g_return_if_fail (HILDON_IS_VOLUMEBAR (widget));

    HildonVolumebarPrivate *priv;
    priv = HILDON_VOLUMEBAR_GET_PRIVATE (widget);

    gchar *message;
    va_list args;

    va_start (args, format);
    message = g_strdup_vprintf (format, args);
    va_end (args);

    hildon_helper_set_insensitive_message ((GtkWidget *) priv->volumebar, message);

    g_free (message);
}


