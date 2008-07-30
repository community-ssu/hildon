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
 * SECTION:hildon-button
 * @short_description: Widget representing a button in the Hildon framework.
 *
 * The #HildonButton is a GTK widget which represents a clickable
 * button. It is derived from the GtkButton widget and provides
 * additional commodities specific to the Hildon framework.
 *
 * The height of a #HildonButton can be set to either "finger" height
 * or "thumb" height. It can also be configured to use halfscreen or
 * fullscreen width. Alternatively, either dimension can be set to
 * "auto" so it behaves like a standard GtkButton.
 *
 * The #HildonButton can hold any valid child widget, but it usually
 * contains two labels: title and value (the latter being optional).
 */

#include                                        "hildon-button.h"
#include                                        "hildon-enum-types.h"

#define FINGER_BUTTON_HEIGHT                    70
#define THUMB_BUTTON_HEIGHT                     105
#define HALFSCREEN_BUTTON_WIDTH                 400
#define FULLSCREEN_BUTTON_WIDTH                 800

G_DEFINE_TYPE                                   (HildonButton, hildon_button, GTK_TYPE_BUTTON);

#define                                         HILDON_BUTTON_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_BUTTON, HildonButtonPrivate));

typedef struct                                  _HildonButtonPrivate HildonButtonPrivate;

struct                                          _HildonButtonPrivate
{
    GtkLabel *title;
    GtkLabel *value;
    GtkWidget *alignment;
};

enum {
  PROP_TITLE = 1,
  PROP_VALUE,
  PROP_ARRANGEMENT_FLAGS
};

static void
hildon_button_set_arrangement                   (HildonButton      *button,
                                                 HildonButtonFlags  flags);

static void
hildon_button_construct_child                   (HildonButton *button);

static void
hildon_button_set_property                      (GObject      *object,
                                                 guint         prop_id,
                                                 const GValue *value,
                                                 GParamSpec   *pspec)
{
    HildonButton *button = HILDON_BUTTON (object);

    switch (prop_id)
    {
    case PROP_TITLE:
        hildon_button_set_title (button, g_value_get_string (value));
        break;
    case PROP_VALUE:
        hildon_button_set_value (button, g_value_get_string (value));
        break;
    case PROP_ARRANGEMENT_FLAGS:
        hildon_button_set_arrangement (button, g_value_get_flags (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
hildon_button_get_property                      (GObject    *object,
                                                 guint       prop_id,
                                                 GValue     *value,
                                                 GParamSpec *pspec)
{
    HildonButton *button = HILDON_BUTTON (object);
    HildonButtonPrivate *priv = HILDON_BUTTON_GET_PRIVATE (button);

    switch (prop_id)
    {
    case PROP_TITLE:
        g_value_set_string (value, gtk_label_get_text (priv->title));
        break;
    case PROP_VALUE:
        g_value_set_string (value, gtk_label_get_text (priv->value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
}

static void
hildon_button_class_init                        (HildonButtonClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class = (GtkWidgetClass *)klass;

    gobject_class->set_property = hildon_button_set_property;
    gobject_class->get_property = hildon_button_get_property;

    g_object_class_install_property (
        gobject_class,
        PROP_TITLE,
        g_param_spec_string (
            "title",
            "Title",
            "Text of the title label inside the button",
            NULL,
            G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_object_class_install_property (
        gobject_class,
        PROP_VALUE,
        g_param_spec_string (
            "value",
            "Value",
            "Text of the value label inside the button",
            NULL,
            G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    g_object_class_install_property (
        gobject_class,
        PROP_ARRANGEMENT_FLAGS,
        g_param_spec_flags (
            "arrangement-flags",
            "Arrangement flags",
            "How the button contents must be arranged",
            HILDON_TYPE_BUTTON_FLAGS,
            HILDON_BUTTON_WITH_HORIZONTAL_VALUE,
            G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "horizontal-spacing",
            "Horizontal spacing between labels",
            "Horizontal spacing between the title and value labels, when in horizontal mode",
            0, G_MAXUINT, 25,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "vertical-spacing",
            "Vertical spacing between labels",
            "Vertical spacing between the title and value labels, when in vertical mode",
            0, G_MAXUINT, 5,
            G_PARAM_READABLE));

    g_type_class_add_private (klass, sizeof (HildonButtonPrivate));
}

static void
hildon_button_init                              (HildonButton *self)
{
    HildonButtonPrivate *priv = HILDON_BUTTON_GET_PRIVATE (self);

    priv->title = GTK_LABEL (gtk_label_new (NULL));
    priv->value = GTK_LABEL (gtk_label_new (NULL));
    priv->alignment = gtk_alignment_new (0.5, 0.5, 0, 0);

    gtk_widget_set_name (GTK_WIDGET (priv->title), "hildon-button-title");
    gtk_widget_set_name (GTK_WIDGET (priv->value), "hildon-button-value");

    gtk_misc_set_alignment (GTK_MISC (priv->title), 0, 0.5);
    gtk_misc_set_alignment (GTK_MISC (priv->value), 0, 0.5);

    /* The value label is not shown automatically, see hildon_button_set_value() */
    gtk_widget_set_no_show_all (GTK_WIDGET (priv->value), TRUE);
}

/**
 * hildon_button_set_size_groups:
 * @button: a #HildonButton
 * @title_size_group: A #GtkSizeGroup for the button title (main label), or %NULL
 * @value_size_group: A #GtkSizeGroup group for the button value (secondary label), or %NULL
 *
 * Adds the title and value labels of @button to @title_size_group and
 * @value_size_group respectively. %NULL size groups will be ignored.
 **/
void
hildon_button_set_size_groups                   (HildonButton *button,
                                                 GtkSizeGroup *title_size_group,
                                                 GtkSizeGroup *value_size_group)
{
    HildonButtonPrivate *priv;

    g_return_if_fail (HILDON_IS_BUTTON (button));
    g_return_if_fail (!title_size_group || GTK_IS_SIZE_GROUP (title_size_group));
    g_return_if_fail (!value_size_group || GTK_IS_SIZE_GROUP (value_size_group));

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    if (title_size_group)
        gtk_size_group_add_widget (title_size_group, GTK_WIDGET (priv->title));

    if (value_size_group)
        gtk_size_group_add_widget (value_size_group, GTK_WIDGET (priv->value));
}

/**
 * hildon_button_new:
 * @flags: flags to set the size and layout of the button
 *
 * Creates a new #HildonButton. To add a child widget use gtk_container_add().
 *
 * Returns: a new #HildonButton
 **/
GtkWidget *
hildon_button_new                               (HildonButtonFlags  flags)
{
    return hildon_button_new_full (flags, NULL, NULL, NULL, NULL);
}

/**
 * hildon_button_new_with_text:
 * @flags: flags to set the size and layout of the button
 * @title: Title of the button (main label)
 * @value: Value of the button (secondary label), or %NULL
 *
 * Creates a new #HildonButton with two labels, @title and @value.
 *
 * If you just want to use the main label, set @value to %NULL. You
 * can set it to a non-%NULL value at any time later.
 *
 * Returns: a new #HildonButton
 **/
GtkWidget *
hildon_button_new_with_text                     (HildonButtonFlags  flags,
                                                 const char        *title,
                                                 const char        *value)
{
    return hildon_button_new_full (flags, title, value, NULL, NULL);
}

/**
 * hildon_button_new_full:
 * @flags: flags to set the size and layout of the button
 * @title: Title of the button (main label)
 * @value: Value of the button (secondary label), or %NULL
 * @title_size_group: a #GtkSizeGroup for the @title label, or %NULL
 * @value_size_group: a #GtkSizeGroup for the @value label, or %NULL
 *
 * Creates a new #HildonButton with two labels, @title and @value, and
 * their respective size groups.
 *
 * If you just want to use the main label, set @value to %NULL. You
 * can set it to a non-%NULL value at any time later.
 *
 * @title and @value will be added to @title_size_group and
 * @value_size_group, respectively, if present.
 *
 * Returns: a new #HildonButton
 **/
GtkWidget *
hildon_button_new_full                          (HildonButtonFlags  flags,
                                                 const char        *title,
                                                 const char        *value,
                                                 GtkSizeGroup      *title_size_group,
                                                 GtkSizeGroup      *value_size_group)
{
    GtkWidget *button;

    /* Create widget */
    button = g_object_new (HILDON_TYPE_BUTTON,
                           "arrangement-flags", flags,
                           "title", title,
                           "value", value,
                           "name", "hildon-button",
                           NULL);
    /* Set size groups */
    if (title_size_group || value_size_group)
        hildon_button_set_size_groups (HILDON_BUTTON (button), title_size_group, value_size_group);

    return button;
}

static void
hildon_button_set_arrangement (HildonButton *button,
                               HildonButtonFlags flags)
{
    GtkWidget *box;
    HildonButtonPrivate *priv;
    guint horizontal_spacing;
    guint vertical_spacing;
    gint width = -1;
    gint height = -1;
    const char *widget_name = NULL;

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    /* Requested height */
    if (flags & HILDON_BUTTON_FINGER_HEIGHT) {
        height = FINGER_BUTTON_HEIGHT;
        widget_name = "hildon-finger-button";
    } else if (flags & HILDON_BUTTON_THUMB_HEIGHT) {
        height = THUMB_BUTTON_HEIGHT;
        widget_name = "hildon-thumb-button";
    }

    if (widget_name) {
        gtk_widget_set_name (GTK_WIDGET (button), widget_name);
    }

    /* Requested width */
    if (flags & HILDON_BUTTON_HALFSCREEN_WIDTH) {
        width = HALFSCREEN_BUTTON_WIDTH;
    } else if (flags & HILDON_BUTTON_FULLSCREEN_WIDTH) {
        width = FULLSCREEN_BUTTON_WIDTH;
    }

    g_object_set (button,
                  "width-request", width,
                  "height-request", height,
                  NULL);

    /* Pack everything */
    gtk_widget_style_get (GTK_WIDGET (button),
                          "horizontal-spacing", &horizontal_spacing,
                          "vertical-spacing", &vertical_spacing,
                          NULL);

    if (flags & HILDON_BUTTON_WITH_VERTICAL_VALUE) {
        box = gtk_vbox_new (FALSE, vertical_spacing);
    } else {
        box = gtk_hbox_new (FALSE, horizontal_spacing);
    }

    gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (priv->title), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (priv->value), TRUE, TRUE, 0);

    gtk_container_add (GTK_CONTAINER (priv->alignment), box);
}

/**
 * hildon_button_set_title:
 * @button: a #HildonButton
 * @title: a new title (main label) for the button.
 *
 * Sets the title (main label) of @button to @title.
 *
 * This will clear the previously set title.
 **/
void
hildon_button_set_title                         (HildonButton *button,
                                                 const char   *title)
{
    HildonButtonPrivate *priv;

    g_return_if_fail (HILDON_IS_BUTTON (button));

    priv = HILDON_BUTTON_GET_PRIVATE (button);
    gtk_label_set_text (priv->title, title);

    if (title)
        hildon_button_construct_child (button);

    g_object_notify (G_OBJECT (button), "title");
}

/**
 * hildon_button_set_value:
 * @button: a #HildonButton
 * @value: a new value (secondary label) for the button, or %NULL
 *
 * Sets the value (secondary label) of @button to @value.
 *
 * This will clear the previously set value.
 *
 * If @value is set to %NULL, the value label will be hidden and the
 * title label will be realigned.
 *
 **/
void
hildon_button_set_value                         (HildonButton *button,
                                                 const char   *value)
{
    HildonButtonPrivate *priv;

    g_return_if_fail (HILDON_IS_BUTTON (button));

    priv = HILDON_BUTTON_GET_PRIVATE (button);
    gtk_label_set_text (priv->value, value);

    /* If the button has no value, hide the label so the title is
     * properly aligned */
    if (value)
        gtk_widget_show (GTK_WIDGET (priv->value));
    else
        gtk_widget_hide (GTK_WIDGET (priv->value));

    g_object_notify (G_OBJECT (button), "value");
}

/**
 * hildon_button_get_title:
 * @button: a #HildonButton
 *
 * Gets the text from the main label (title) of @button, or %NULL if
 * none has been set.
 *
 * Returns: The text of the title label. This string is owned by the
 * widget and must not be modified or freed.
 **/
const char *
hildon_button_get_title                         (HildonButton *button)
{
    HildonButtonPrivate *priv;

    g_return_val_if_fail (HILDON_IS_BUTTON (button), NULL);

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    return gtk_label_get_text (priv->title);
}

/**
 * hildon_button_get_value:
 * @button: a #HildonButton
 *
 * Gets the text from the secondary label (value) of @button, or %NULL
 * if none has been set.
 *
 * Returns: The text of the value label. This string is owned by the
 * widget and must not be modified or freed.
 **/
const char *
hildon_button_get_value                         (HildonButton *button)
{
    HildonButtonPrivate *priv;

    g_return_val_if_fail (HILDON_IS_BUTTON (button), NULL);

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    return gtk_label_get_text (priv->value);
}

/**
 * hildon_button_set_text:
 * @button: a #HildonButton
 * @title: new text for the button title (main label)
 * @value: new text for the button value (secondary label)
 *
 * Convenience function to change both labels of a #HildonButton
 **/
void
hildon_button_set_text                          (HildonButton *button,
                                                 const char   *title,
                                                 const char   *value)
{
    hildon_button_set_title (button, title);
    hildon_button_set_value (button, value);
}

static void
hildon_button_construct_child                   (HildonButton *button)
{
    HildonButtonPrivate *priv = HILDON_BUTTON_GET_PRIVATE (button);
    GtkBin *bin = GTK_BIN (button);

    /* Return if there's nothing to do */
    if (bin->child == priv->alignment)
        return;

    if (bin->child) {
        gtk_container_remove (GTK_CONTAINER (button), bin->child);
    }

    gtk_container_add (GTK_CONTAINER (button), priv->alignment);
    gtk_widget_show_all (priv->alignment);
}
