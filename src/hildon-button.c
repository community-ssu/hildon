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
 * The HildonButton is a GTK widget which represent a clickable
 * button. It is derived from the GtkButton widget and provides
 * additional commodities specific to the Hildon framework.
 */

#include                                        "hildon-button.h"

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
};

enum {
  PROP_TITLE = 1,
  PROP_VALUE
};

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

    gtk_widget_set_name (GTK_WIDGET (priv->title), "hildon-button-title");
    gtk_widget_set_name (GTK_WIDGET (priv->value), "hildon-button-value");

    gtk_misc_set_alignment (GTK_MISC (priv->title), 0, 0.5);
    gtk_misc_set_alignment (GTK_MISC (priv->value), 0, 0.5);
}

void
hildon_button_set_size_groups                   (HildonButton *button,
                                                 GtkSizeGroup *title_size_group,
                                                 GtkSizeGroup *value_size_group)
{
    HildonButtonPrivate *priv;

    g_return_if_fail (HILDON_IS_BUTTON (button));

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    if (title_size_group)
        gtk_size_group_add_widget (title_size_group, GTK_WIDGET (priv->title));

    if (value_size_group)
        gtk_size_group_add_widget (value_size_group, GTK_WIDGET (priv->value));
}

GtkWidget *
hildon_button_new                               (HildonButtonFlags  flags,
                                                 const char        *title,
                                                 const char        *value)
{
    return hildon_button_new_full (flags, title, value, NULL, NULL);
}

GtkWidget *
hildon_button_new_full                          (HildonButtonFlags  flags,
                                                 const char        *title,
                                                 const char        *value,
                                                 GtkSizeGroup      *title_size_group,
                                                 GtkSizeGroup      *value_size_group)
{
    GtkWidget *button;
    GtkWidget *box;
    GtkWidget *alignment;
    HildonButtonPrivate *priv;
    guint horizontal_spacing;
    guint vertical_spacing;
    gint width = -1;
    gint height = -1;
    const char *widget_name = "hildon-button";

    /* Requested height */
    if (flags & HILDON_BUTTON_FINGER_HEIGHT) {
        height = FINGER_BUTTON_HEIGHT;
        widget_name = "hildon-finger-button";
    } else if (flags & HILDON_BUTTON_THUMB_HEIGHT) {
        height = THUMB_BUTTON_HEIGHT;
        widget_name = "hildon-thumb-button";
    }

    /* Requested width */
    if (flags & HILDON_BUTTON_HALFSCREEN_WIDTH) {
        width = HALFSCREEN_BUTTON_WIDTH;
    } else if (flags & HILDON_BUTTON_FULLSCREEN_WIDTH) {
        width = FULLSCREEN_BUTTON_WIDTH;
    }

    /* Create widget */
    button = g_object_new (HILDON_TYPE_BUTTON,
                           "title", title,
                           "value", value,
                           "width-request", width,
                           "height-request", height,
                           "name", widget_name,
                           NULL);

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    /* Pack everything */
    gtk_widget_style_get (button,
                          "horizontal-spacing", &horizontal_spacing,
                          "vertical-spacing", &vertical_spacing,
                          NULL);

    if (flags & HILDON_BUTTON_WITH_VERTICAL_VALUE) {
        box = gtk_vbox_new (FALSE, vertical_spacing);
    } else {
        box = gtk_hbox_new (FALSE, horizontal_spacing);
    }

    alignment = gtk_alignment_new (0.5, 0.5, 0, 0);

    gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (priv->title), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (box), GTK_WIDGET (priv->value), TRUE, TRUE, 0);

    gtk_container_add (GTK_CONTAINER (alignment), box);
    gtk_container_add (GTK_CONTAINER (button), alignment);

    /* Set size groups */
    if (title_size_group || value_size_group)
        hildon_button_set_size_groups (HILDON_BUTTON (button), title_size_group, value_size_group);

    gtk_widget_show_all (alignment);

    return button;
}

void
hildon_button_set_title                         (HildonButton *button,
                                                 const char   *title)
{
    HildonButtonPrivate *priv;

    g_return_if_fail (HILDON_IS_BUTTON (button));

    priv = HILDON_BUTTON_GET_PRIVATE (button);
    gtk_label_set_text (priv->title, title);

    g_object_notify (G_OBJECT (button), "title");
}

void
hildon_button_set_value                         (HildonButton *button,
                                                 const char   *value)
{
    HildonButtonPrivate *priv;

    g_return_if_fail (HILDON_IS_BUTTON (button));

    priv = HILDON_BUTTON_GET_PRIVATE (button);
    gtk_label_set_text (priv->value, value);

    g_object_notify (G_OBJECT (button), "value");
}

const char *
hildon_button_get_title                         (HildonButton *button)
{
    HildonButtonPrivate *priv;

    g_return_val_if_fail (HILDON_IS_BUTTON (button), NULL);

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    return gtk_label_get_text (priv->title);
}

const char *
hildon_button_get_value                         (HildonButton *button)
{
    HildonButtonPrivate *priv;

    g_return_val_if_fail (HILDON_IS_BUTTON (button), NULL);

    priv = HILDON_BUTTON_GET_PRIVATE (button);

    return gtk_label_get_text (priv->value);
}

void
hildon_button_set_title_and_value               (HildonButton *button,
                                                 const char   *title,
                                                 const char   *value)
{
    hildon_button_set_title (button, title);
    hildon_button_set_value (button, value);
}
