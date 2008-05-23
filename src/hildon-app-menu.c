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
 * Besides that, the #HildonAppMenu can contain filter buttons
 * (#GtkToggleButton or #GtkRadioButton), which can be grouped.
 *
 * <example>
 * <title>Creating a HildonAppMenu</title>
 * <programlisting>
 * HildonAppMenu *menu;
 * GtkWidget *button;
 * GtkWidget *filter;
 * GtkWidget *filtergroup;
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
 * filter = gtk_toggle_button_new_with_label ("Filter one");
 * g_signal_connect (filter, "clicked", G_CALLBACK (filter_one_clicked), userdata);
 * hildon_app_menu_add_filter (menu, GTK_BUTTON (filter), NULL);
 * <!-- -->
 * // Create another filter and add it to a new filter group
 * filter = gtk_radio_button_new_with_label (NULL, "Filter two");
 * gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (filter), FALSE);
 * g_signal_connect (filter, "clicked", G_CALLBACK (filter_two_clicked), userdata);
 * filtergroup = hildon_app_menu_add_filter (menu, GTK_BUTTON (filter), NULL);
 * // Add a new filter to the same filter group
 * filter = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (filter), "Filter three");
 * gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (filter), FALSE);
 * g_signal_connect (filter, "clicked", G_CALLBACK (filter_three_clicked), userdata);
 * hildon_app_menu_add_filter (menu, GTK_BUTTON (filter), filtergroup);
 * <!-- -->
 * // Pop the menu up
 * hildon_app_menu_popup (menu);
 * </programlisting>
 * </example>
 *
 */

#include "hildon-app-menu.h"
#include "hildon-app-menu-private.h"

static void
hildon_app_menu_popdown                         (HildonAppMenu *menu);

static GdkWindow *
grab_transfer_window_get                        (HildonAppMenu *menu);

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
 * @self : A @HildonAppMenu
 * @item : A @GtkButton to add to the HildonAppMenu
 *
 * Adds the @item to the @HildonAppMenu
 */
void
hildon_app_menu_append                          (HildonAppMenu *self,
                                                 GtkButton *item)
{
    HildonAppMenuPrivate *priv;
    int row, col;

    g_return_if_fail (HILDON_IS_APP_MENU (self));
    g_return_if_fail (GTK_IS_BUTTON (item));

    priv = HILDON_APP_MENU_GET_PRIVATE(self);

    /* Calculate the row and column number */
    col = priv->nitems % 2;
    row = (priv->nitems - col) / 2;
    priv->nitems++;

    /* GtkTable already calls gtk_table_resize() if necessary */
    gtk_table_attach_defaults (priv->table, GTK_WIDGET (item), col, col + 1, row, row + 1);

    /* Close the menu when the button is pressed */
    g_signal_connect_swapped (item, "clicked", G_CALLBACK (hildon_app_menu_popdown), self);

    gtk_widget_show (GTK_WIDGET (item));
}

/**
 * hildon_app_menu_add_filter
 * @self : A @HildonAppMenu
 * @filter : A @GtkButton to add to the HildonAppMenu
 * @group : An existing filter group, or %NULL to create a new one
 *
 * Adds the @filter to the @HildonAppMenu, to the group specified by @group
 *
 * Return value: The filter group where the filter has been added
 */
GtkWidget *
hildon_app_menu_add_filter                      (HildonAppMenu *self,
                                                 GtkButton *filter,
                                                 GtkWidget *group)
{
    HildonAppMenuPrivate *priv;

    g_return_val_if_fail (HILDON_IS_APP_MENU (self), NULL);
    g_return_val_if_fail (GTK_IS_BUTTON (filter), NULL);
    g_return_val_if_fail (!group || GTK_IS_BOX (group), NULL);

    priv = HILDON_APP_MENU_GET_PRIVATE(self);

    /* Create a new group if needed */
    if (!group) {
        group = gtk_hbox_new (TRUE, 0);
        gtk_box_pack_start (priv->filters_hbox, group, TRUE, TRUE, 0);
        gtk_widget_show (group);
    }

    /* Pack the filter in the group and set its size */
    gtk_box_pack_start (GTK_BOX (group), GTK_WIDGET (filter), TRUE, TRUE, 0);
    gtk_size_group_add_widget (priv->sizegroup, GTK_WIDGET (filter));

    /* Close the menu when the button is pressed */
    g_signal_connect_swapped (filter, "clicked", G_CALLBACK (hildon_app_menu_popdown), self);

    gtk_widget_show (GTK_WIDGET (filter));

    return group;
}

/**
 * hildon_app_menu_get_group_from_filter
 * @self : A @HildonAppMenu
 * @filter : A @GtkButton previously added to the menu
 *
 * Gets the filter group from a @filter previously added to a @HildonAppMenu
 *
 * Return value: The group where the @filter is in, or %NULL
 */
GtkWidget *
hildon_app_menu_get_group_from_filter           (HildonAppMenu *self,
                                                 GtkButton *filter)
{
    HildonAppMenuPrivate *priv;
    GList *grouplist;
    GtkWidget *result = NULL;

    g_return_val_if_fail (HILDON_IS_APP_MENU (self), NULL);
    g_return_val_if_fail (GTK_IS_BUTTON (filter), NULL);

    priv = HILDON_APP_MENU_GET_PRIVATE(self);

    /* Get the list of filter groups */
    grouplist = gtk_container_get_children (GTK_CONTAINER (priv->filters_hbox));

    for (; grouplist != NULL && !result; grouplist = grouplist->next) {

        GtkBox *group = GTK_BOX (grouplist->data);
        GList *items = gtk_container_get_children (GTK_CONTAINER (group));

        /* Look for the filter inside each filter group */
        for (; items != NULL && !result; items = items->next) {
            if (filter == items->data) {
                result = GTK_WIDGET (group);
            }
        }
        g_list_free (items);

    }
    g_list_free (grouplist);

    if (!result)
        g_critical("Filter not found in hildon app menu!");

    return result;
}

/**
 * hildon_app_menu_popup
 * @menu : A @HildonAppMenu
 *
 * Displays the @HildonAppMenu on top of the screen
 */
void
hildon_app_menu_popup                           (HildonAppMenu *menu)
{
    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    int xpos;
    GtkRequisition req;
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(menu);
    GdkScreen *screen = gtk_widget_get_screen (GTK_WIDGET (menu));

    /* Grab pointer and keyboard */
    if (priv->transfer_window == NULL) {
        priv->transfer_window = grab_transfer_window_get (menu);
        gdk_pointer_grab (
            priv->transfer_window, TRUE,
            GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
            GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK |
            GDK_POINTER_MOTION_MASK, NULL, NULL, GDK_CURRENT_TIME);
        gdk_keyboard_grab (priv->transfer_window, TRUE, GDK_CURRENT_TIME);
        gtk_grab_add (GTK_WIDGET (menu));
    }

    /* Position the menu in the top center of the screen */
    gtk_widget_size_request (GTK_WIDGET (menu), &req);
    xpos = (gdk_screen_get_width (screen) - req.width) / 2;
    gtk_window_move (GTK_WINDOW (menu), xpos, 0);

    gtk_widget_show (GTK_WIDGET (menu));
}

static void
hildon_app_menu_popdown                         (HildonAppMenu *menu)
{
    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    if (priv->transfer_window != NULL) {
        /* Remove the grab */
        gdk_display_pointer_ungrab (gtk_widget_get_display (GTK_WIDGET (menu)),
                                    GDK_CURRENT_TIME);
        gtk_grab_remove (GTK_WIDGET (menu));

        /* Destroy the transfer window */
        gdk_window_destroy (priv->transfer_window);
        priv->transfer_window = NULL;
    }

    gtk_widget_hide (GTK_WIDGET (menu));
}

static gboolean
hildon_app_menu_button_press                    (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    int x, y;
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(widget);
    gdk_window_get_position(widget->window, &x, &y);
    if (event->window != priv->transfer_window ||
        event->x < x || event->x > x + widget->allocation.width ||
        event->y < y || event->y > y + widget->allocation.height) {
        hildon_app_menu_popdown (HILDON_APP_MENU (widget));
        return TRUE;
    } else if (GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->button_press_event) {
        return GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->button_press_event (widget, event);
    } else {
        return FALSE;
    }
}

/* Grab transfer window (based on the one from GtkMenu) */
static GdkWindow *
grab_transfer_window_get                        (HildonAppMenu *menu)
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

    window = gdk_window_new (gtk_widget_get_root_window (GTK_WIDGET (menu)),
                                 &attributes, attributes_mask);
    gdk_window_set_user_data (window, menu);

    gdk_window_show (window);

    return window;
}

static void
hildon_app_menu_realize                         (GtkWidget *widget)
{
    GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->realize (widget);
    gdk_window_set_override_redirect(widget->window, TRUE);
}

static void
hildon_app_menu_init                            (HildonAppMenu *self)
{
    GtkWidget *alignment;
    guint filter_group_spacing, horizontal_spacing, vertical_spacing;
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(self);

    gtk_widget_style_get (GTK_WIDGET (self),
                          "filter-group-spacing", &filter_group_spacing,
                          "horizontal-spacing", &horizontal_spacing,
                          "vertical-spacing", &vertical_spacing,
                          NULL);

    /* Initialize private variables */
    priv->filters_hbox = GTK_BOX (gtk_hbox_new (FALSE, filter_group_spacing));
    priv->vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));
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
    gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (priv->vbox));
    gtk_box_pack_start (priv->vbox, alignment, TRUE, TRUE, 0);
    gtk_box_pack_start (priv->vbox, GTK_WIDGET (priv->table), TRUE, TRUE, 0);

    gtk_widget_show_all (GTK_WIDGET (priv->vbox));

    gtk_window_set_type_hint (GTK_WINDOW (self), GDK_WINDOW_TYPE_HINT_POPUP_MENU);
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
    widget_class->realize = hildon_app_menu_realize;
    widget_class->button_press_event = hildon_app_menu_button_press;

    g_type_class_add_private (klass, sizeof (HildonAppMenuPrivate));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "filter-group-spacing",
            "Space between filter groups",
            "Space in pixels between the filter groups",
            0, G_MAXUINT, 10,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "horizontal-spacing",
            "Horizontal spacing on menu items",
            "Horizontal spacing between each menu item (but not filters)",
            0, G_MAXUINT, 10,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "vertical-spacing",
            "Vertical spacing on menu items",
            "Vertical spacing between each menu item (but not filters)",
            0, G_MAXUINT, 10,
            G_PARAM_READABLE));
}
