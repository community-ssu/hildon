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
 * This is a standard GtkButton which contains a check box and a
 * label. Functions are provided to get and set the values of both the
 * check box and the label. Standard GtkButton methods must not be
 * used for that.
 *
 * <example>
 * <programlisting>
 * void
 * button_clicked (GtkButton *button, gpointer user_data)
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
 *     hildon_check_button_set_label (GTK_BUTTON (button), "Click me");
 * <!-- -->
 *     g_signal_connect (button, "clicked", G_CALLBACK (button_clicked), NULL);
 * <!-- -->
 *     return button;
 * }
 * </programlisting>
 * </example>

 */

#include                                        "hildon-check-button.h"

static void
check_button_clicked                            (GtkButton             *button,
                                                 GtkCellRendererToggle *renderer)
{
    gboolean current = gtk_cell_renderer_toggle_get_active (renderer);
    gtk_cell_renderer_toggle_set_active (renderer, !current);
}

/**
 * hildon_check_button_set_label:
 * @button: A #GtkButton created with hildon_check_button_new()
 * @label: New text for the label.
 *
 * Sets the text of the button label to @label.
 **/
void
hildon_check_button_set_label                   (GtkButton   *button,
                                                 const gchar *label)
{
    GtkCellRendererText *text_renderer;

    g_return_if_fail (GTK_IS_BUTTON (button));
    text_renderer = GTK_CELL_RENDERER_TEXT (g_object_get_data (G_OBJECT (button), "text-renderer"));
    g_return_if_fail (GTK_IS_CELL_RENDERER_TEXT (text_renderer));

    g_object_set (text_renderer, "text", label, NULL);
}

/**
 * hildon_check_button_get_label:
 * @button: A #GtkButton created with hildon_check_button_new()
 *
 * Gets the text of the label inside the button.
 *
 * Return value: the text of the label. This string is owned by the
 * button and must not be modified or freed.
 **/
const gchar *
hildon_check_button_get_label                   (GtkButton   *button)
{
    GtkCellRendererText *text_renderer;
    const gchar *text;

    g_return_val_if_fail (GTK_IS_BUTTON (button), NULL);
    text_renderer = GTK_CELL_RENDERER_TEXT (g_object_get_data (G_OBJECT (button), "text-renderer"));
    g_return_val_if_fail (GTK_IS_CELL_RENDERER_TEXT (text_renderer), NULL);

    g_object_get (text_renderer, "text", &text, NULL);

    return text;
}

/**
 * hildon_check_button_set_active:
 * @button: A #GtkButton created with hildon_check_button_new()
 * @is_active: new state for the check box
 *
 * Sets the state of the check box.
 **/
void
hildon_check_button_set_active                  (GtkButton *button,
                                                 gboolean   is_active)
{
    GtkCellRendererToggle *toggle_renderer;

    g_return_if_fail (GTK_IS_BUTTON (button));
    toggle_renderer = GTK_CELL_RENDERER_TOGGLE (g_object_get_data (G_OBJECT (button), "toggle-renderer"));
    g_return_if_fail (GTK_IS_CELL_RENDERER_TOGGLE (toggle_renderer));

    gtk_cell_renderer_toggle_set_active (toggle_renderer, is_active);
}

/**
 * hildon_check_button_get_active:
 * @button: A #GtkButton created with hildon_check_button_new()
 *
 * Gets the state of the check box.
 *
 * Return value: %TRUE if the check box is active, %FALSE otherwise.
 **/
gboolean
hildon_check_button_get_active                  (GtkButton *button)
{
    GtkCellRendererToggle *toggle_renderer;

    g_return_val_if_fail (GTK_IS_BUTTON (button), FALSE);
    toggle_renderer = GTK_CELL_RENDERER_TOGGLE (g_object_get_data (G_OBJECT (button), "toggle-renderer"));
    g_return_val_if_fail (GTK_IS_CELL_RENDERER_TOGGLE (toggle_renderer), FALSE);

    return gtk_cell_renderer_toggle_get_active (toggle_renderer);
}

/**
 * hildon_check_button_new:
 * @size: Flags indicating the size of the new button
 *
 * This function creates a #GtkButton containing a label and a check
 * box.
 *
 * This button has specific functions to get and set the values of the
 * label and the check box. You must not use the standard #GtkButton
 * methods for that.
 *
 * To set the alignment you can use gtk_button_set_alignment()
 *
 * Return value: A newly created #GtkButton widget.
 **/
GtkWidget *
hildon_check_button_new                         (HildonSizeType size)
{
    GtkWidget *button = gtk_button_new ();
    GtkWidget *cell_view = gtk_cell_view_new ();
    GtkCellRenderer *toggle_renderer = gtk_cell_renderer_toggle_new ();
    GtkCellRenderer *text_renderer = gtk_cell_renderer_text_new ();
    GtkWidget *alignment = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);

    /* Set the size of the button */
    hildon_gtk_widget_set_theme_size (button, size);

    /* Pack everything */
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (cell_view), toggle_renderer, FALSE);
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (cell_view), text_renderer, FALSE);
    gtk_container_add (GTK_CONTAINER (alignment), cell_view);
    gtk_container_add (GTK_CONTAINER (button), alignment);
    gtk_widget_show_all (GTK_WIDGET (alignment));

    /* Toggle the check box when the button is clicked */
    g_signal_connect (button, "clicked", G_CALLBACK (check_button_clicked), toggle_renderer);

    /* Set the data */
    g_object_set_data (G_OBJECT (button), "toggle-renderer", toggle_renderer);
    g_object_set_data (G_OBJECT (button), "text-renderer", text_renderer);

    return button;
}
