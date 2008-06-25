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
 * SECTION:hildon-app-menu
 * @short_description: Widget representing the application menu in the Hildon framework.
 *
 * The #HildonAppMenu is a GTK widget which represents an application
 * menu in the Hildon framework.
 *
 * This menu opens from the top of the screen and contains a number of
 * entries (#GtkButton) organized in two columns. Entries are added
 * left to right and top to bottom.
 *
 * Besides that, the #HildonAppMenu can contain a group of filter buttons
 * (#GtkToggleButton or #GtkRadioButton).
 *
 * <example>
 * <title>Creating a HildonAppMenu</title>
 * <programlisting>
 * HildonAppMenu *menu;
 * GtkWidget *button;
 * GtkWidget *filter;
 * <!-- -->
 * menu = HILDON_APP_MENU (hildon_app_menu_new ());
 * <!-- -->
 * // Create a button and add it to the menu
 * button = gtk_button_new_with_label ("Menu command one");
 * g_signal_connect (button, "clicked", G_CALLBACK (button_one_clicked), userdata);
 * hildon_app_menu_append (menu, GTK_BUTTON (button));
 * // Another button
 * button = gtk_button_new_with_label ("Menu command two");
 * g_signal_connect (button, "clicked", G_CALLBACK (button_two_clicked), userdata);
 * hildon_app_menu_append (menu, GTK_BUTTON (button));
 * <!-- -->
 * // Create a filter and add it to the menu
 * filter = gtk_radio_button_new_with_label (NULL, "Filter one");
 * gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (filter), FALSE);
 * g_signal_connect (filter, "clicked", G_CALLBACK (filter_two_clicked), userdata);
 * hildon_app_menu_add_filter (menu, GTK_BUTTON (filter));
 * // Add a new filter
 * filter = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (filter), "Filter two");
 * gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (filter), FALSE);
 * g_signal_connect (filter, "clicked", G_CALLBACK (filter_three_clicked), userdata);
 * hildon_app_menu_add_filter (menu, GTK_BUTTON (filter));
 * <!-- -->
 * // Show the menu
 * gtk_widget_show (menu);
 * </programlisting>
 * </example>
 *
 */

#include                                        "hildon-app-menu.h"
#include                                        "hildon-app-menu-private.h"

#include                                        <gdk/gdkx.h>
#include                                        <X11/X.h>
#include                                        <X11/Xatom.h>
#include                                        <string.h>

static GdkWindow *
grab_transfer_window_get                        (GtkWidget *widget);

G_DEFINE_TYPE (HildonAppMenu, hildon_app_menu, GTK_TYPE_WINDOW);

/**
 * hildon_app_menu_new:
 *
 * Creates a new HildonAppMenu.
 *
 * Return value: A @HildonAppMenu.
 **/
GtkWidget *
hildon_app_menu_new                             (void)
{
    GtkWidget *menu = g_object_new (HILDON_TYPE_APP_MENU, NULL);
    return menu;
}

/**
 * hildon_app_menu_append
 * @menu : A @HildonAppMenu
 * @item : A @GtkButton to add to the HildonAppMenu
 *
 * Adds the @item to the @HildonAppMenu
 */
void
hildon_app_menu_append                          (HildonAppMenu *menu,
                                                 GtkButton *item)
{
    HildonAppMenuPrivate *priv;
    int row, col;

    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    g_return_if_fail (GTK_IS_BUTTON (item));

    priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    /* Calculate the row and column number */
    col = priv->nitems % 2;
    row = (priv->nitems - col) / 2;
    priv->nitems++;

    /* GtkTable already calls gtk_table_resize() if necessary */
    gtk_table_attach_defaults (priv->table, GTK_WIDGET (item), col, col + 1, row, row + 1);

    /* Close the menu when the button is clicked */
    g_signal_connect_swapped (item, "clicked", G_CALLBACK (gtk_widget_hide), menu);

    gtk_widget_show (GTK_WIDGET (item));
}

/**
 * hildon_app_menu_add_filter
 * @menu : A @HildonAppMenu
 * @filter : A @GtkButton to add to the #HildonAppMenu.
 *
 * Adds the @filter to the #HildonAppMenu.
 *
 */
void
hildon_app_menu_add_filter                      (HildonAppMenu *menu,
                                                 GtkButton *filter)
{
    HildonAppMenuPrivate *priv;

    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    g_return_if_fail (GTK_IS_BUTTON (filter));

    priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    /* Pack the filter in the group and set its size */
    gtk_box_pack_start (GTK_BOX (priv->filters_hbox), GTK_WIDGET (filter), TRUE, TRUE, 0);
    gtk_size_group_add_widget (priv->sizegroup, GTK_WIDGET (filter));

    /* Close the menu when the button is clicked */
    g_signal_connect_swapped (filter, "clicked", G_CALLBACK (gtk_widget_hide), menu);

    gtk_widget_show (GTK_WIDGET (filter));
}

static void
hildon_app_menu_map                             (GtkWidget *widget)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(widget);

    GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->map (widget);

    /* Grab pointer and keyboard */
    if (priv->transfer_window == NULL) {
        gboolean has_grab = FALSE;

        priv->transfer_window = grab_transfer_window_get (widget);

        if (gdk_pointer_grab (priv->transfer_window, TRUE,
                              GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
                              GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
                              GDK_POINTER_MOTION_MASK, NULL, NULL,
                              GDK_CURRENT_TIME) == GDK_GRAB_SUCCESS) {
            if (gdk_keyboard_grab (priv->transfer_window, TRUE,
                                   GDK_CURRENT_TIME) == GDK_GRAB_SUCCESS) {
                has_grab = TRUE;
            } else {
                gdk_display_pointer_ungrab (gtk_widget_get_display (widget),
                                            GDK_CURRENT_TIME);
            }
        }

        if (has_grab) {
            gtk_grab_add (widget);
        } else {
            gdk_window_destroy (priv->transfer_window);
            priv->transfer_window = NULL;
        }
    }
}

static void
hildon_app_menu_unmap                           (GtkWidget *widget)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(widget);

    /* Remove the grab */
    if (priv->transfer_window != NULL) {
        gdk_display_pointer_ungrab (gtk_widget_get_display (widget),
                                    GDK_CURRENT_TIME);
        gtk_grab_remove (widget);

        gdk_window_destroy (priv->transfer_window);
        priv->transfer_window = NULL;
    }

    GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->unmap (widget);
}

static gboolean
hildon_app_menu_button_release                  (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    int x, y;
    gboolean released_outside;

    gdk_window_get_position (widget->window, &x, &y);

    /* Whether the button has been released outside the widget */
    released_outside = (event->x_root < x || event->x_root > x + widget->allocation.width ||
                        event->y_root < y || event->y_root > y + widget->allocation.height);

    if (released_outside) {
        gtk_widget_hide (widget);
    }

    if (GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->button_release_event) {
        return GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->button_release_event (widget, event);
    } else {
        return FALSE;
    }
}

/* Grab transfer window (based on the one from GtkMenu) */
static GdkWindow *
grab_transfer_window_get                        (GtkWidget *widget)
{
    GdkWindow *window;
    GdkWindowAttr attributes;
    gint attributes_mask;

    attributes.x = 0;
    attributes.y = 0;
    attributes.width = 10;
    attributes.height = 10;
    attributes.window_type = GDK_WINDOW_TEMP;
    attributes.wclass = GDK_INPUT_ONLY;
    attributes.override_redirect = TRUE;
    attributes.event_mask = 0;

    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_NOREDIR;

    window = gdk_window_new (gtk_widget_get_root_window (widget),
                                 &attributes, attributes_mask);
    gdk_window_set_user_data (window, widget);

    gdk_window_show (window);

    return window;
}

static void
hildon_app_menu_realize                         (GtkWidget *widget)
{
    GdkDisplay *display;
    Atom atom;
    const gchar *notification_type = "_HILDON_WM_WINDOW_TYPE_APP_MENU";

    GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->realize (widget);

    gdk_window_set_decorations (widget->window, GDK_DECOR_BORDER);

    display = gdk_drawable_get_display (widget->window);
    atom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_WINDOW_TYPE");
    XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window), GDK_WINDOW_XID (widget->window),
                     atom, XA_STRING, 8, PropModeReplace, (guchar *) notification_type,
                     strlen (notification_type));
}

static void
hildon_app_menu_init                            (HildonAppMenu *menu)
{
    GtkWidget *alignment;
    GdkScreen *screen;
    int width;
    guint horizontal_spacing, vertical_spacing,
            inner_border, external_border;
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    gtk_widget_style_get (GTK_WIDGET (menu),
                          "horizontal-spacing", &horizontal_spacing,
                          "vertical-spacing", &vertical_spacing,
                          "inner-border", &inner_border,
                          "external-border", &external_border,
                          NULL);

    /* Initialize private variables */
    priv->filters_hbox = GTK_BOX (gtk_hbox_new (FALSE, 0));
    priv->vbox = GTK_BOX (gtk_vbox_new (FALSE, vertical_spacing));
    priv->table = GTK_TABLE (gtk_table_new (1, 2, TRUE));
    priv->sizegroup = GTK_SIZE_GROUP (gtk_size_group_new (GTK_SIZE_GROUP_BOTH));
    priv->nitems = 0;
    priv->transfer_window = NULL;

    /* Set spacing between table elements */
    gtk_table_set_row_spacings (priv->table, vertical_spacing);
    gtk_table_set_col_spacings (priv->table, horizontal_spacing);

    /* Align the filters to the center */
    alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (alignment), GTK_WIDGET (priv->filters_hbox));

    /* Pack everything */
    gtk_container_add (GTK_CONTAINER (menu), GTK_WIDGET (priv->vbox));
    gtk_box_pack_start (priv->vbox, alignment, TRUE, TRUE, 0);
    gtk_box_pack_start (priv->vbox, GTK_WIDGET (priv->table), TRUE, TRUE, 0);

    /* Set default size */
    screen = gtk_widget_get_screen (GTK_WIDGET (menu));
    width = gdk_screen_get_width (screen) - external_border * 2;
    gtk_window_set_default_size (GTK_WINDOW (menu), width, -1);

    /* Set inner border */
    gtk_container_set_border_width (GTK_CONTAINER (menu), inner_border);

    gtk_window_set_modal (GTK_WINDOW (menu), TRUE);

    gtk_widget_show_all (GTK_WIDGET (priv->vbox));
}

static void
hildon_app_menu_finalize                        (GObject *object)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(object);

    g_object_unref (priv->sizegroup);
    if (priv->transfer_window)
        gdk_window_destroy (priv->transfer_window);

    g_signal_handlers_destroy (object);
    G_OBJECT_CLASS (hildon_app_menu_parent_class)->finalize (object);
}

static void
hildon_app_menu_class_init                      (HildonAppMenuClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class = (GtkWidgetClass *)klass;

    gobject_class->finalize = hildon_app_menu_finalize;
    widget_class->map = hildon_app_menu_map;
    widget_class->unmap = hildon_app_menu_unmap;
    widget_class->realize = hildon_app_menu_realize;
    widget_class->button_release_event = hildon_app_menu_button_release;

    g_type_class_add_private (klass, sizeof (HildonAppMenuPrivate));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "horizontal-spacing",
            "Horizontal spacing on menu items",
            "Horizontal spacing between each menu item (but not filters)",
            0, G_MAXUINT, 16,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "vertical-spacing",
            "Vertical spacing on menu items",
            "Vertical spacing between each menu item (but not filters)",
            0, G_MAXUINT, 16,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "inner-border",
            "Border between menu edges and buttons",
            "Border between menu edges and buttons",
            0, G_MAXUINT, 16,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "external-border",
            "Border between menu and screen edges",
            "Border between the right and left edges of the menu and the screen edges",
            0, G_MAXUINT, 40,
            G_PARAM_READABLE));
}
