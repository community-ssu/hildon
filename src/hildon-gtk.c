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
 * SECTION:hildon-gtk
 * @short_description: Additional functions for Gtk widgets
 * @see_also: #HildonButton
 *
 * Hildon provides some functions to extend the functionality of
 * existing Gtk widgets. This also includes convenience functions to
 * easily perform frequent tasks.
 */

#include <X11/Xatom.h>
#include <gdk/gdkx.h>

#include "hildon-gtk.h"

static void
image_visible_changed_cb                        (GtkWidget  *image,
                                                 GParamSpec *arg1,
                                                 gpointer   oldparent)
{
    if (!GTK_WIDGET_VISIBLE (image))
        gtk_widget_show (image);
}

static void
parent_changed_cb                               (GtkWidget  *image,
                                                 GParamSpec *arg1,
                                                 gpointer   oldparent)
{
    /* If the parent has really changed, remove the old signal handlers */
    if (image->parent != oldparent) {
        g_signal_handlers_disconnect_by_func (image, parent_changed_cb, oldparent);
        g_signal_handlers_disconnect_by_func (image, image_visible_changed_cb, NULL);
    }
}

static void
image_changed_cb                                (GtkButton  *button,
                                                 GParamSpec *arg1,
                                                 gpointer    user_data)
{
    GtkWidget *image = gtk_button_get_image (button);

    g_return_if_fail (image == NULL || GTK_IS_WIDGET (image));

    if (image != NULL) {
        /* If the button has a new image, show it */
        gtk_widget_show (image);
        /* Show the image no matter the value of gtk-button-images */
        g_signal_connect (image, "notify::visible", G_CALLBACK (image_visible_changed_cb), NULL);
        /* If the image is removed from the button, disconnect these handlers */
        g_signal_connect (image, "notify::parent", G_CALLBACK (parent_changed_cb), image->parent);
    }
}

static void
button_common_init                              (GtkWidget      *button,
                                                 HildonSizeType  size)
{
    /* Set requested size */
    hildon_gtk_widget_set_theme_size (button, size);

    /* Make sure that all images in this button are always shown */
    g_signal_connect (button, "notify::image", G_CALLBACK (image_changed_cb), NULL);
}

/**
 * hildon_gtk_menu_new:
 *
 * This is a convenience function to create a #GtkMenu setting its
 * widget name to allow Hildon specific styling.
 *
 * Return value: A newly created #GtkMenu widget.
 **/
GtkWidget *
hildon_gtk_menu_new                             (void)
{
    GtkWidget *menu = gtk_menu_new ();
    gtk_widget_set_name (menu, "hildon-context-sensitive-menu");
    return menu;
}

/**
 * hildon_gtk_button_new:
 * @size: Flags indicating the size of the new button
 *
 * This is a convenience function to create a #GtkButton setting its
 * size to one of the pre-defined Hildon sizes.
 *
 * Buttons created with this function also override the
 * "gtk-button-images" setting. Images set using
 * gtk_button_set_image() are always shown.
 *
 * Return value: A newly created #GtkButton widget.
 **/
GtkWidget *
hildon_gtk_button_new                           (HildonSizeType size)
{
    GtkWidget *button = gtk_button_new ();
    button_common_init (button, size);
    return button;
}

/**
 * hildon_gtk_toggle_button_new:
 * @size: Flags indicating the size of the new button
 *
 * This is a convenience function to create a #GtkToggleButton setting
 * its size to one of the pre-defined Hildon sizes.
 *
 * Buttons created with this function also override the
 * "gtk-button-images" setting. Images set using
 * gtk_button_set_image() are always shown.
 *
 * Return value: A newly created #GtkToggleButton widget.
 **/
GtkWidget *
hildon_gtk_toggle_button_new                    (HildonSizeType size)
{
    GtkWidget *button = gtk_toggle_button_new ();
    button_common_init (button, size);
    return button;
}

/**
 * hildon_gtk_radio_button_new:
 * @size: Flags indicating the size of the new button
 * @group: An existing radio button group, or %NULL if you are
 * creating a new group
 *
 * This is a convenience function to create a #GtkRadioButton setting
 * its size to one of the pre-defined Hildon sizes.
 *
 * Buttons created with this function also override the
 * "gtk-button-images" setting. Images set using
 * gtk_button_set_image() are always shown.
 *
 * Return value: A newly created #GtkRadioButton widget.
 **/
GtkWidget *
hildon_gtk_radio_button_new                     (HildonSizeType  size,
                                                 GSList         *group)
{
    GtkWidget *button = gtk_radio_button_new (group);
    button_common_init (button, size);
    return button;
}

/**
 * hildon_gtk_radio_button_new_from_widget:
 * @size: Flags indicating the size of the new button
 * @radio_group_member: widget to get radio group from or %NULL
 *
 * This is a convenience function to create a #GtkRadioButton setting
 * its size to one of the pre-defined Hildon sizes.
 *
 * Buttons created with this function also override the
 * "gtk-button-images" setting. Images set using
 * gtk_button_set_image() are always shown.
 *
 * Return value: A newly created #GtkRadioButton widget.
 **/
GtkWidget *
hildon_gtk_radio_button_new_from_widget         (HildonSizeType  size,
                                                 GtkRadioButton *radio_group_member)
{
    GtkWidget *button = gtk_radio_button_new_from_widget (radio_group_member);
    button_common_init (button, size);
    return button;
}

#ifdef MAEMO_GTK
/**
 * hildon_gtk_tree_view_new:
 * @mode: the Hildon UI mode
 *
 * Creates a new #GtkTreeView widget with the Hildon UI mode set to
 * @mode
 *
 * Return value: A newly created #GtkTreeView widget.
 **/
GtkWidget *
hildon_gtk_tree_view_new                        (HildonUIMode mode)
{
    return g_object_new (GTK_TYPE_TREE_VIEW, "hildon-ui-mode", mode,
                         "enable-search", FALSE, NULL);
}

/**
 * hildon_gtk_tree_view_new_with_model:
 * @mode: the Hildon UI mode
 * @model: the model.
 *
 * Creates a new #GtkTreeView widget with the Hildon UI mode set to
 * @mode and the model initialized to @model.
 *
 * Return value: A newly created #GtkTreeView widget.
 **/
GtkWidget *
hildon_gtk_tree_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model)
{
    return g_object_new (GTK_TYPE_TREE_VIEW, "hildon-ui-mode", mode, "model", model, NULL);
}

/**
 * hildon_gtk_tree_view_set_ui_mode:
 * @treeview: A #GtkTreeView
 * @mode: The new #HildonUIMode
 *
 * Sets the UI mode of @treeview to @mode.
 **/
void
hildon_gtk_tree_view_set_ui_mode                (GtkTreeView  *treeview,
                                                 HildonUIMode  mode)
{
    g_return_if_fail (GTK_IS_TREE_VIEW (treeview));
    g_object_set (treeview, "hildon-ui-mode", mode, NULL);
}

/**
 * hildon_gtk_icon_view_new:
 * @mode: the Hildon UI mode
 *
 * Creates a new #GtkIconView widget with the Hildon UI mode set to
 * @mode
 *
 * Return value: A newly created #GtkIconView widget
 **/
GtkWidget *
hildon_gtk_icon_view_new                        (HildonUIMode mode)
{
    return g_object_new (GTK_TYPE_ICON_VIEW, "hildon-ui-mode", mode, NULL);
}

/**
 * hildon_gtk_icon_view_new_with_model:
 * @mode: the Hildon UI mode
 * @model: The model.
 *
 * Creates a new #GtkIconView widget with the Hildon UI mode set to
 * @mode and the model intitialized to @model.
 *
 * Return value: A newly created #GtkIconView widget.
 **/
GtkWidget *
hildon_gtk_icon_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model)
{
    return g_object_new (GTK_TYPE_ICON_VIEW, "hildon-ui-mode", mode, "model", model, NULL);
}

/**
 * hildon_gtk_icon_view_set_ui_mode:
 * @iconview: A #GtkIconView
 * @mode: The new #HildonUIMode
 *
 * Sets the UI mode of @iconview to @mode.
 **/
void
hildon_gtk_icon_view_set_ui_mode                (GtkIconView  *iconview,
                                                 HildonUIMode  mode)
{
    g_return_if_fail (GTK_IS_ICON_VIEW (iconview));
    g_object_set (iconview, "hildon-ui-mode", mode, NULL);
}
#endif /* MAEMO_GTK */

/**
 * hildon_gtk_window_set_progress_indicator:
 * @window: The window, we want to define its state
 * @state: The state we want to set: 1 -> show progress indicator, 0
 *          -> hide progress indicator.
 *
 * This functions notifies the window manager that it should add a
 * progress indicator in the title of the window. It applies to
 * #HildonDialog and #HildonWindow.
 *
 **/
void
hildon_gtk_window_set_progress_indicator        (GtkWindow    *window,
                                                 guint        state)
{
  GtkWidget *widget = GTK_WIDGET (window);
  GdkDisplay *display;
  Atom atom;

  display = gdk_drawable_get_display (widget->window);
  atom = gdk_x11_get_xatom_by_name_for_display (display, "_HILDON_WM_WINDOW_PROGRESS_INDICATOR");

  XChangeProperty (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (widget->window),
                   atom, XA_INTEGER, 32, PropModeReplace,
                   (guchar *)&state, 1);
}

/**
 * hildon_gtk_hscale_new:
 * @void: 
 *
 * Creates a new horizontal scale widget that lets the user select
 * a value. The value is technically a double between 0.0 and 1.0.
 * See gtk_adjustment_configure() for reconfiguring the adjustment.
 *
 * The scale is hildonized, which means that a click or tap immediately
 * jumps to the desired position, see gtk_range_set_relative_steps().
 * Further more the value is not displayed, see gtk_scale_set_draw_value().
 *
 * Returns: a new hildonized #GtkHScale
 *
 * Since: 2.2
 **/
GtkWidget*
hildon_gtk_hscale_new                           (void)
{
  GtkWidget *scale = gtk_hscale_new_with_range (0.0, 1.0, 0.1);
  g_object_set (scale,
                "draw-value", FALSE,
#ifdef MAEMO_GTK
                "relative-steps", FALSE,
#endif
                NULL);

  return scale;
}
