/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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
 * SECTION:hildon-check-button
 * @short_description: Button with a check box inside
 *
 * This is a button containing a check box and a label. Functions are
 * provided to get and set the value of the check box. For the label
 * use gtk_button_set_label(). Note that this button does NOT support
 * an image, so don't use gtk_button_set_image()
 *
 * <example>
 * <title>Using a Hildon check button</title>
 * <programlisting>
 * void
 * button_toggled (HildonCheckButton *button, gpointer user_data)
 * {
 *     gboolean active;
 * <!-- -->
 *     active = hildon_check_button_get_active (button);
 *     if (active)
 *        g_debug ("Button is active");
 *     else
 *        g_debug ("Button is not active");
 * }
 * <!-- -->
 * GtkWidget *
 * create_button (void)
 * {
 *     GtkWidget *button;
 * <!-- -->
 *     button = hildon_check_button_new (HILDON_SIZE_AUTO);
 *     gtk_button_set_label (GTK_BUTTON (button), "Click me");
 * <!-- -->
 *     g_signal_connect (button, "toggled", G_CALLBACK (button_toggled), NULL);
 * <!-- -->
 *     return button;
 * }
 * </programlisting>
 * </example>

 */

#include                                        "hildon-check-button.h"

enum {
  TOGGLED,
  LAST_SIGNAL
};

static guint                                    signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE                                   (HildonCheckButton, hildon_check_button, GTK_TYPE_BUTTON);

#define                                         HILDON_CHECK_BUTTON_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_CHECK_BUTTON, HildonCheckButtonPrivate));

struct                                          _HildonCheckButtonPrivate
{
    GtkCellRendererToggle *toggle_renderer;
};

/**
 * hildon_check_button_toggled:
 * @button: A #HildonCheckButton
 *
 * Emits the #HildonCheckButton::toggled signal on the #HildonCheckButton.
 *
 * Since: 2.2
 */
void
hildon_check_button_toggled                     (HildonCheckButton *button)
{
    g_return_if_fail (HILDON_IS_CHECK_BUTTON (button));

    g_signal_emit (button, signals[TOGGLED], 0);
}

/**
 * hildon_check_button_set_active:
 * @button: A #HildonCheckButton
 * @is_active: new state for the button
 *
 * Sets the state of @button to @is_active
 *
 * Since: 2.2
 **/
void
hildon_check_button_set_active                  (HildonCheckButton *button,
                                                 gboolean           is_active)
{
    gboolean prev_is_active;

    g_return_if_fail (HILDON_IS_CHECK_BUTTON (button));

    prev_is_active = hildon_check_button_get_active (button);

    if (prev_is_active != is_active) {
        gtk_button_clicked (GTK_BUTTON (button));
        gtk_widget_queue_draw (GTK_WIDGET (button));
    }
}

/**
 * hildon_check_button_get_active:
 * @button: A #HildonCheckButton
 *
 * Gets the state of the button.
 *
 * Return value: %TRUE if @button is active, %FALSE otherwise.
 *
 * Since: 2.2 
 **/
gboolean
hildon_check_button_get_active                  (HildonCheckButton *button)
{
    g_return_val_if_fail (HILDON_IS_CHECK_BUTTON (button), FALSE);

    return gtk_cell_renderer_toggle_get_active (button->priv->toggle_renderer);
}

/**
 * hildon_check_button_new:
 * @size: Flags indicating the size of the new button
 *
 * This function creates a #HildonCheckButton.
 *
 * Return value: A newly created #HildonCheckButton
 *
 * Since: 2.2
 **/
GtkWidget *
hildon_check_button_new                         (HildonSizeType size)
{
    GtkWidget *button = g_object_new (HILDON_TYPE_CHECK_BUTTON, "xalign", 0.0, NULL);
    hildon_gtk_widget_set_theme_size (button, size);
    return button;
}

static void
hildon_check_button_clicked                     (GtkButton *button)
{
    HildonCheckButton *checkbutton = HILDON_CHECK_BUTTON (button);
    gboolean current = hildon_check_button_get_active (checkbutton);

    gtk_cell_renderer_toggle_set_active (checkbutton->priv->toggle_renderer, !current);

    hildon_check_button_toggled (checkbutton);
}

static void
hildon_check_button_apply_style                 (GtkWidget *widget)
{
    guint checkbox_size;
    HildonCheckButtonPrivate *priv = HILDON_CHECK_BUTTON (widget)->priv;

    gtk_widget_style_get (widget, "checkbox-size", &checkbox_size, NULL);

    g_object_set (priv->toggle_renderer, "indicator-size", checkbox_size, NULL);
}

static void
hildon_check_button_style_set                   (GtkWidget *widget,
                                                 GtkStyle  *previous_style)
{
    if (GTK_WIDGET_CLASS (hildon_check_button_parent_class)->style_set)
        GTK_WIDGET_CLASS (hildon_check_button_parent_class)->style_set (widget, previous_style);

    hildon_check_button_apply_style (widget);
}

static void
hildon_check_button_class_init                  (HildonCheckButtonClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass*) klass;
    GtkWidgetClass *widget_class = (GtkWidgetClass*) klass;
    GtkButtonClass *button_class = (GtkButtonClass*) klass;

    widget_class->style_set = hildon_check_button_style_set;
    button_class->clicked = hildon_check_button_clicked;

    klass->toggled = NULL;

    /**
     * HildonCheckButton::toggled
     *
     * Emitted when the #HildonCheckButton's state is changed
     *
     * Since: 2.2
     */
    signals[TOGGLED] =
        g_signal_new ("toggled",
                      G_OBJECT_CLASS_TYPE (gobject_class),
                      G_SIGNAL_RUN_FIRST,
                      G_STRUCT_OFFSET (HildonCheckButtonClass, toggled),
                      NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "checkbox-size",
            "Size of the check box",
            "Size of the check box",
            0, G_MAXUINT, 26,
            G_PARAM_READABLE));

    g_type_class_add_private (klass, sizeof (HildonCheckButtonPrivate));
}

static void
hildon_check_button_init                        (HildonCheckButton *button)
{
    HildonCheckButtonPrivate *priv = HILDON_CHECK_BUTTON_GET_PRIVATE (button);
    GtkWidget *cell_view = gtk_cell_view_new ();

    /* Store private part */
    button->priv = priv;

    /* Make sure that the check box is always shown, no matter the value of gtk-button-images */
    g_signal_connect (cell_view, "notify::visible", G_CALLBACK (gtk_widget_show), NULL);

    /* Create toggle renderer and pack it into the cell view */
    priv->toggle_renderer = GTK_CELL_RENDERER_TOGGLE (gtk_cell_renderer_toggle_new ());
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (cell_view),
                                GTK_CELL_RENDERER (priv->toggle_renderer), FALSE);

    /* Add cell view to the image */
    gtk_button_set_image (GTK_BUTTON (button), cell_view);

    hildon_check_button_apply_style (GTK_WIDGET (button));
}
