/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
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
 * SECTION:hildon-toggle-button
 * @short_description: Widget representing a toggle button in the Hildon framework.
 *
 * The #HildonToggleButton is a GTK widget which represents a toggle
 * button. It is derived from the GtkToggleButton widget and provides
 * additional commodities specific to the Hildon framework.
 *
 * The height of a #HildonToggleButton can be set to either "finger" height
 * or "thumb" height. It can also be configured to use halfscreen or
 * fullscreen width. Alternatively, either dimension can be set to
 * "auto" so it behaves like a standard GtkToggleButton.
 */

#include                                        "hildon-toggle-button.h"
#include                                        "hildon-enum-types.h"

#define FINGER_BUTTON_HEIGHT                    70
#define THUMB_BUTTON_HEIGHT                     105
#define HALFSCREEN_BUTTON_WIDTH                 400
#define FULLSCREEN_BUTTON_WIDTH                 800

G_DEFINE_TYPE                                   (HildonToggleButton, hildon_toggle_button, GTK_TYPE_TOGGLE_BUTTON);

enum {
  PROP_ARRANGEMENT_FLAGS
};

static void
hildon_toggle_button_set_arrangement            (HildonToggleButton      *button,
                                                 HildonToggleButtonFlags  flags);

static void
hildon_toggle_button_set_property               (GObject      *object,
                                                 guint         prop_id,
                                                 const GValue *value,
                                                 GParamSpec   *pspec)
{
    HildonToggleButton *button = HILDON_TOGGLE_BUTTON (object);

    switch (prop_id)
    {
    case PROP_ARRANGEMENT_FLAGS:
        hildon_toggle_button_set_arrangement (button, g_value_get_flags (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
hildon_toggle_button_class_init                 (HildonToggleButtonClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;

    gobject_class->set_property = hildon_toggle_button_set_property;

    g_object_class_install_property (
        gobject_class,
        PROP_ARRANGEMENT_FLAGS,
        g_param_spec_flags (
            "arrangement-flags",
            "Arrangement flags",
            "How the button contents must be arranged",
            HILDON_TYPE_TOGGLE_BUTTON_FLAGS,
            HILDON_TOGGLE_BUTTON_AUTO_WIDTH | HILDON_TOGGLE_BUTTON_AUTO_HEIGHT,
            G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

static void
hildon_toggle_button_init                       (HildonToggleButton *self)
{
}

/**
 * hildon_toggle_button_new:
 * @flags: flags to set the size of the button
 *
 * Creates a new #HildonToggleButton. To add a child widget use gtk_container_add().
 *
 * Returns: a new #HildonToggleButton
 **/
GtkWidget *
hildon_toggle_button_new                        (HildonToggleButtonFlags flags)
{
    GtkWidget *button;
    /* Create widget */
    button = g_object_new (HILDON_TYPE_TOGGLE_BUTTON,
                           "arrangement-flags", flags,
                           NULL);
    return button;
}

/**
 * hildon_toggle_button_new_with_label:
 * @flags: flags to set the size of the button
 * @label: a text to set in the button label
 *
 * Creates a new #HildonToggleButton with a text label.
 *
 * Returns: a new #HildonToggleButton
 **/
GtkWidget *
hildon_toggle_button_new_with_label             (HildonToggleButtonFlags  flags,
                                                 const char              *label)
{
    GtkWidget *button;
    /* Create widget */
    button = g_object_new (HILDON_TYPE_TOGGLE_BUTTON,
                           "arrangement-flags", flags,
                           "label", label,
                           NULL);
    return button;
}

static void
hildon_toggle_button_set_arrangement            (HildonToggleButton      *button,
                                                 HildonToggleButtonFlags  flags)
{
    gint width = -1;
    gint height = -1;
    const char *widget_name = NULL;

    /* Requested height */
    if (flags & HILDON_TOGGLE_BUTTON_FINGER_HEIGHT) {
        height = FINGER_BUTTON_HEIGHT;
        widget_name = "hildon-finger-button";
    } else if (flags & HILDON_TOGGLE_BUTTON_THUMB_HEIGHT) {
        height = THUMB_BUTTON_HEIGHT;
        widget_name = "hildon-thumb-button";
    }

    if (widget_name) {
        gtk_widget_set_name (GTK_WIDGET (button), widget_name);
    }

    /* Requested width */
    if (flags & HILDON_TOGGLE_BUTTON_HALFSCREEN_WIDTH) {
        width = HALFSCREEN_BUTTON_WIDTH;
    } else if (flags & HILDON_TOGGLE_BUTTON_FULLSCREEN_WIDTH) {
        width = FULLSCREEN_BUTTON_WIDTH;
    }

    g_object_set (button,
                  "width-request", width,
                  "height-request", height,
                  NULL);

}
