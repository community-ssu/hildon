/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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
 * entries (#GtkButton) organized in one or two columns, depending on
 * the size of the screen (the number of columns changes automatically
 * if the screen is resized). Entries are added left to right and top
 * to bottom.
 *
 * Besides that, the #HildonAppMenu can contain a group of filter buttons
 * (#GtkToggleButton or #GtkRadioButton).
 *
 * To use a #HildonAppMenu, add it to a #HildonStackableWindow using
 * hildon_stackable_window_set_main_menu(). The menu will appear when
 * the user presses the window title bar. Alternatively, you can show
 * it by hand using gtk_widget_show().
 *
 * The menu will be automatically hidden when one of its buttons is
 * clicked. Use g_signal_connect_after() when connecting callbacks to
 * buttons to make sure that they're called after the menu
 * disappears. Alternatively, you can add the button to the menu
 * before connecting any callback.
 *
 * Although implemented with a #GtkWindow, #HildonAppMenu behaves like
 * a normal ref-counted widget, so g_object_ref(), g_object_unref(),
 * g_object_ref_sink() and friends will behave just like with any
 * other non-toplevel widget.
 *
 * <example>
 * <title>Creating a HildonAppMenu</title>
 * <programlisting>
 * HildonStackableWindow *win;
 * HildonAppMenu *menu;
 * GtkWidget *button;
 * GtkWidget *filter;
 * <!-- -->
 * win = HILDON_STACKABLE_WINDOW (hildon_stackable_window_new ());
 * menu = HILDON_APP_MENU (hildon_app_menu_new ());
 * <!-- -->
 * // Create a button and add it to the menu
 * button = gtk_button_new_with_label ("Menu command one");
 * g_signal_connect_after (button, "clicked", G_CALLBACK (button_one_clicked), userdata);
 * hildon_app_menu_append (menu, GTK_BUTTON (button));
 * gtk_widget_show (button);
 * <!-- -->
 * // Another button
 * button = gtk_button_new_with_label ("Menu command two");
 * g_signal_connect_after (button, "clicked", G_CALLBACK (button_two_clicked), userdata);
 * hildon_app_menu_append (menu, GTK_BUTTON (button));
 * gtk_widget_show (button);
 * <!-- -->
 * // Create a filter and add it to the menu
 * filter = gtk_radio_button_new_with_label (NULL, "Filter one");
 * gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (filter), FALSE);
 * g_signal_connect_after (filter, "clicked", G_CALLBACK (filter_one_clicked), userdata);
 * hildon_app_menu_add_filter (menu, GTK_BUTTON (filter));
 * gtk_widget_show (filter);
 * <!-- -->
 * // Add a new filter
 * filter = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (filter), "Filter two");
 * gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (filter), FALSE);
 * g_signal_connect_after (filter, "clicked", G_CALLBACK (filter_two_clicked), userdata);
 * hildon_app_menu_add_filter (menu, GTK_BUTTON (filter));
 * gtk_widget_show (filter);
 * <!-- -->
 * // Add the menu to the window
 * hildon_stackable_window_set_main_menu (win, menu);
 * </programlisting>
 * </example>
 *
 */

#include                                        <string.h>
#include                                        <X11/Xatom.h>
#include                                        <gdk/gdkx.h>

#include                                        "hildon-gtk.h"
#include                                        "hildon-app-menu.h"
#include                                        "hildon-app-menu-private.h"
#include                                        "hildon-window.h"

static GdkWindow *
grab_transfer_window_get                        (GtkWidget *widget);

static void
hildon_app_menu_repack_items                    (HildonAppMenu *menu,
                                                 gint           start_from);

static void
hildon_app_menu_repack_filters                  (HildonAppMenu *menu);

static gboolean
can_activate_accel                              (GtkWidget *widget,
                                                 guint      signal_id,
                                                 gpointer   user_data);

static void
item_visibility_changed                         (GtkWidget     *item,
                                                 GParamSpec    *arg1,
                                                 HildonAppMenu *menu);

static void
filter_visibility_changed                       (GtkWidget     *item,
                                                 GParamSpec    *arg1,
                                                 HildonAppMenu *menu);

static void
remove_item_from_list                           (GList    **list,
                                                 gpointer   item);

static void
hildon_app_menu_apply_style                     (GtkWidget *widget);

G_DEFINE_TYPE (HildonAppMenu, hildon_app_menu, GTK_TYPE_WINDOW);

/**
 * hildon_app_menu_new:
 *
 * Creates a new #HildonAppMenu.
 *
 * Return value: A #HildonAppMenu.
 *
 * Since: 2.2
 **/
GtkWidget *
hildon_app_menu_new                             (void)
{
    GtkWidget *menu = g_object_new (HILDON_TYPE_APP_MENU, NULL);
    return menu;
}

/**
 * hildon_app_menu_insert:
 * @menu : A #HildonAppMenu
 * @item : A #GtkButton to add to the #HildonAppMenu
 * @position : The position in the item list where @item is added (from 0 to n-1).
 *
 * Adds @item to @menu at the position indicated by @position.
 *
 * Since: 2.2
 */
void
hildon_app_menu_insert                          (HildonAppMenu *menu,
                                                 GtkButton     *item,
                                                 gint           position)
{
    HildonAppMenuPrivate *priv;

    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    g_return_if_fail (GTK_IS_BUTTON (item));

    priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    /* Force widget size */
    hildon_gtk_widget_set_theme_size (GTK_WIDGET (item),
                                      HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);

    /* Add the item to the menu */
    gtk_widget_show (GTK_WIDGET (item));
    g_object_ref_sink (item);
    priv->buttons = g_list_insert (priv->buttons, item, position);
    hildon_app_menu_repack_items (menu, position);

    /* Enable accelerators */
    gtk_widget_realize (GTK_WIDGET (item));
    g_signal_connect (item, "can-activate-accel", G_CALLBACK (can_activate_accel), NULL);

    /* Close the menu when the button is clicked */
    g_signal_connect_swapped (item, "clicked", G_CALLBACK (gtk_widget_hide), menu);
    g_signal_connect (item, "notify::visible", G_CALLBACK (item_visibility_changed), menu);

    /* Remove item from list when it is destroyed */
    g_object_weak_ref (G_OBJECT (item), (GWeakNotify) remove_item_from_list, &(priv->buttons));
}

/**
 * hildon_app_menu_append:
 * @menu : A #HildonAppMenu
 * @item : A #GtkButton to add to the #HildonAppMenu
 *
 * Adds @item to the end of the menu's item list.
 *
 * Since: 2.2
 */
void
hildon_app_menu_append                          (HildonAppMenu *menu,
                                                 GtkButton     *item)
{
    hildon_app_menu_insert (menu, item, -1);
}

/**
 * hildon_app_menu_prepend:
 * @menu : A #HildonAppMenu
 * @item : A #GtkButton to add to the #HildonAppMenu
 *
 * Adds @item to the beginning of the menu's item list.
 *
 * Since: 2.2
 */
void
hildon_app_menu_prepend                         (HildonAppMenu *menu,
                                                 GtkButton     *item)
{
    hildon_app_menu_insert (menu, item, 0);
}

/**
 * hildon_app_menu_reorder_child:
 * @menu : A #HildonAppMenu
 * @item : A #GtkButton to move
 * @position : The new position to place @item (from 0 to n-1).
 *
 * Moves a #GtkButton to a new position within #HildonAppMenu.
 *
 * Since: 2.2
 */
void
hildon_app_menu_reorder_child                   (HildonAppMenu *menu,
                                                 GtkButton     *item,
                                                 gint           position)
{
    HildonAppMenuPrivate *priv;
    gint old_position;

    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    g_return_if_fail (GTK_IS_BUTTON (item));
    g_return_if_fail (position >= 0);

    priv = HILDON_APP_MENU_GET_PRIVATE (menu);
    old_position = g_list_index (priv->buttons, item);

    g_return_if_fail (old_position >= 0);

    /* Move the item */
    priv->buttons = g_list_remove (priv->buttons, item);
    priv->buttons = g_list_insert (priv->buttons, item, position);

    hildon_app_menu_repack_items (menu, MIN (old_position, position));
}

/**
 * hildon_app_menu_add_filter:
 * @menu : A #HildonAppMenu
 * @filter : A #GtkButton to add to the #HildonAppMenu.
 *
 * Adds the @filter to @menu.
 *
 * Since: 2.2
 */
void
hildon_app_menu_add_filter                      (HildonAppMenu *menu,
                                                 GtkButton *filter)
{
    HildonAppMenuPrivate *priv;

    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    g_return_if_fail (GTK_IS_BUTTON (filter));

    priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    /* Force widget size */
    hildon_gtk_widget_set_theme_size (GTK_WIDGET (filter),
                                      HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);

    /* Add the filter to the menu */
    gtk_widget_show (GTK_WIDGET (filter));
    g_object_ref_sink (filter);
    priv->filters = g_list_append (priv->filters, filter);
    hildon_app_menu_repack_filters (menu);

    /* Enable accelerators */
    gtk_widget_realize (GTK_WIDGET (filter));
    g_signal_connect (filter, "can-activate-accel", G_CALLBACK (can_activate_accel), NULL);

    /* Close the menu when the button is clicked */
    g_signal_connect_swapped (filter, "clicked", G_CALLBACK (gtk_widget_hide), menu);
    g_signal_connect (filter, "notify::visible", G_CALLBACK (filter_visibility_changed), menu);

    /* Remove filter from list when it is destroyed */
    g_object_weak_ref (G_OBJECT (filter), (GWeakNotify) remove_item_from_list, &(priv->filters));
}

static void
hildon_app_menu_set_columns                     (HildonAppMenu *menu,
                                                 guint          columns)
{
    HildonAppMenuPrivate *priv;

    g_return_if_fail (HILDON_IS_APP_MENU (menu));
    g_return_if_fail (columns > 0);

    priv = HILDON_APP_MENU_GET_PRIVATE (menu);

    if (columns != priv->columns) {
        priv->columns = columns;
        hildon_app_menu_repack_items (menu, 0);
    }
}

static void
parent_window_topmost_notify                   (HildonWindow *parent_win,
                                                GParamSpec   *arg1,
                                                GtkWidget    *menu)
{
    if (!hildon_window_get_is_topmost (parent_win))
        gtk_widget_hide (menu);
}

static void
parent_window_unmapped                         (HildonWindow *parent_win,
                                                GtkWidget    *menu)
{
    gtk_widget_hide (menu);
}

void G_GNUC_INTERNAL
hildon_app_menu_set_parent_window              (HildonAppMenu *self,
                                                GtkWindow     *parent_window)
{
    HildonAppMenuPrivate *priv;

    g_return_if_fail (HILDON_IS_APP_MENU (self));
    g_return_if_fail (parent_window == NULL || GTK_IS_WINDOW (parent_window));

    priv = HILDON_APP_MENU_GET_PRIVATE(self);

    /* Disconnect old handlers, if any */
    if (priv->parent_window) {
        g_signal_handlers_disconnect_by_func (priv->parent_window, parent_window_topmost_notify, self);
        g_signal_handlers_disconnect_by_func (priv->parent_window, parent_window_unmapped, self);
    }

    /* Connect a new handler */
    if (parent_window) {
        g_signal_connect (parent_window, "notify::is-topmost", G_CALLBACK (parent_window_topmost_notify), self);
        g_signal_connect (parent_window, "unmap", G_CALLBACK (parent_window_unmapped), self);
    }

    priv->parent_window = parent_window;

    if (parent_window == NULL && GTK_WIDGET_VISIBLE (self))
        gtk_widget_hide (GTK_WIDGET (self));
}

gpointer G_GNUC_INTERNAL
hildon_app_menu_get_parent_window              (HildonAppMenu *self)
{
    HildonAppMenuPrivate *priv;

    g_return_val_if_fail (HILDON_IS_APP_MENU (self), NULL);

    priv = HILDON_APP_MENU_GET_PRIVATE (self);

    return priv->parent_window;
}

static void
screen_size_changed                            (GdkScreen     *screen,
                                                HildonAppMenu *menu)
{
    hildon_app_menu_apply_style (GTK_WIDGET (menu));

    if (gdk_screen_get_width (screen) > gdk_screen_get_height (screen)) {
        hildon_app_menu_set_columns (menu, 2);
    } else {
        hildon_app_menu_set_columns (menu, 1);
    }
}

static gboolean
can_activate_accel                              (GtkWidget *widget,
                                                 guint      signal_id,
                                                 gpointer   user_data)
{
    return GTK_WIDGET_VISIBLE (widget);
}

static void
item_visibility_changed                         (GtkWidget     *item,
                                                 GParamSpec    *arg1,
                                                 HildonAppMenu *menu)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE (menu);

    hildon_app_menu_repack_items (menu, g_list_index (priv->buttons, item));
}

static void
filter_visibility_changed                       (GtkWidget     *item,
                                                 GParamSpec    *arg1,
                                                 HildonAppMenu *menu)
{
    hildon_app_menu_repack_filters (menu);
}

static void
remove_item_from_list                           (GList    **list,
                                                 gpointer   item)
{
    *list = g_list_remove (*list, item);
}

static void
hildon_app_menu_show                            (GtkWidget *widget)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(widget);

    /* Show the menu only if it's not empty */
    if (priv->buttons || priv->filters) {
        GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->show (widget);
    }
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

static void
hildon_app_menu_grab_notify                     (GtkWidget *widget,
                                                 gboolean   was_grabbed)
{
    if (GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->grab_notify)
        GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->grab_notify (widget, was_grabbed);

    if (!was_grabbed && GTK_WIDGET_VISIBLE (widget))
        gtk_widget_hide (widget);
}

static gboolean
hildon_app_menu_hide_idle                       (gpointer widget)
{
    gtk_widget_hide (GTK_WIDGET (widget));
    return FALSE;
}

/* Send keyboard accelerators to the parent window, if necessary.
 * This code is heavily based on gtk_menu_key_press ()
 */
static gboolean
hildon_app_menu_key_press                       (GtkWidget   *widget,
                                                 GdkEventKey *event)
{
    GtkWindow *parent_window;
    HildonAppMenuPrivate *priv;

    g_return_val_if_fail (HILDON_IS_APP_MENU (widget), FALSE);
    g_return_val_if_fail (event != NULL, FALSE);

    if (GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->key_press_event (widget, event))
        return TRUE;

    priv = HILDON_APP_MENU_GET_PRIVATE (widget);
    parent_window = priv->parent_window;

    if (parent_window) {
        guint accel_key, accel_mods;
        GdkModifierType consumed_modifiers;
        GdkDisplay *display;
        GSList *accel_groups;
        GSList *list;

        display = gtk_widget_get_display (widget);

        /* Figure out what modifiers went into determining the key symbol */
        gdk_keymap_translate_keyboard_state (gdk_keymap_get_for_display (display),
                                             event->hardware_keycode, event->state, event->group,
                                             NULL, NULL, NULL, &consumed_modifiers);

        accel_key = gdk_keyval_to_lower (event->keyval);
        accel_mods = event->state & gtk_accelerator_get_default_mod_mask () & ~consumed_modifiers;

        /* If lowercasing affects the keysym, then we need to include SHIFT in the modifiers,
         * We re-upper case when we match against the keyval, but display and save in caseless form.
         */
        if (accel_key != event->keyval)
            accel_mods |= GDK_SHIFT_MASK;

        accel_groups = gtk_accel_groups_from_object (G_OBJECT (parent_window));

        for (list = accel_groups; list; list = list->next) {
            GtkAccelGroup *accel_group = list->data;

            if (gtk_accel_group_query (accel_group, accel_key, accel_mods, NULL)) {
                gtk_window_activate_key (parent_window, event);
                gdk_threads_add_idle (hildon_app_menu_hide_idle, widget);
                break;
            }
        }
    }

    return TRUE;
}

static gboolean
hildon_app_menu_button_press                    (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    int x, y;
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(widget);

    gdk_window_get_position (widget->window, &x, &y);

    /* Whether the button has been pressed outside the widget */
    priv->pressed_outside = (event->x_root < x || event->x_root > x + widget->allocation.width ||
                             event->y_root < y || event->y_root > y + widget->allocation.height);

    if (GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->button_press_event) {
        return GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->button_press_event (widget, event);
    } else {
        return FALSE;
    }
}

static gboolean
hildon_app_menu_button_release                  (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(widget);

    if (priv->pressed_outside) {
        int x, y;
        gboolean released_outside;

        gdk_window_get_position (widget->window, &x, &y);

        /* Whether the button has been released outside the widget */
        released_outside = (event->x_root < x || event->x_root > x + widget->allocation.width ||
                            event->y_root < y || event->y_root > y + widget->allocation.height);

        if (released_outside) {
            gtk_widget_hide (widget);
        }

        priv->pressed_outside = FALSE; /* Always reset pressed_outside to FALSE */
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
    Atom property, window_type;
    Display *xdisplay;
    GdkDisplay *gdkdisplay;
    GdkScreen *screen;

    GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->realize (widget);

    gdk_window_set_decorations (widget->window, GDK_DECOR_BORDER);

    gdkdisplay = gdk_drawable_get_display (widget->window);
    xdisplay = GDK_WINDOW_XDISPLAY (widget->window);

    property = gdk_x11_get_xatom_by_name_for_display (gdkdisplay, "_NET_WM_WINDOW_TYPE");
    window_type = XInternAtom (xdisplay, "_HILDON_WM_WINDOW_TYPE_APP_MENU", False);
    XChangeProperty (xdisplay, GDK_WINDOW_XID (widget->window), property,
                     XA_ATOM, 32, PropModeReplace, (guchar *) &window_type, 1);

    /* Detect any screen changes */
    screen = gtk_widget_get_screen (widget);
    g_signal_connect (screen, "size-changed", G_CALLBACK (screen_size_changed), widget);

    /* Force menu to set the initial layout */
    screen_size_changed (screen, HILDON_APP_MENU (widget));
}

static void
hildon_app_menu_unrealize                       (GtkWidget *widget)
{
    GdkScreen *screen = gtk_widget_get_screen (widget);
    /* Disconnect "size-changed" signal handler */
    g_signal_handlers_disconnect_by_func (screen, G_CALLBACK (screen_size_changed), widget);

    GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->unrealize (widget);
}

static void
hildon_app_menu_apply_style                     (GtkWidget *widget)
{
    GdkScreen *screen;
    gint width;
    guint horizontal_spacing, vertical_spacing, inner_border, external_border;
    HildonAppMenuPrivate *priv;

    priv = HILDON_APP_MENU_GET_PRIVATE (widget);

    gtk_widget_style_get (widget,
                          "horizontal-spacing", &horizontal_spacing,
                          "vertical-spacing", &vertical_spacing,
                          "inner-border", &inner_border,
                          "external-border", &external_border,
                          NULL);

    /* Set spacings */
    gtk_table_set_row_spacings (priv->table, vertical_spacing);
    gtk_table_set_col_spacings (priv->table, horizontal_spacing);
    gtk_box_set_spacing (priv->vbox, vertical_spacing);

    /* Set inner border */
    gtk_container_set_border_width (GTK_CONTAINER (widget), inner_border);

    /* Set default size */
    screen = gtk_widget_get_screen (widget);
    width = gdk_screen_get_width (screen) - external_border * 2;
    gtk_window_set_default_size (GTK_WINDOW (widget), width, -1);
}

static void
hildon_app_menu_style_set                       (GtkWidget *widget,
                                                 GtkStyle  *previous_style)
{
    if (GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->style_set)
        GTK_WIDGET_CLASS (hildon_app_menu_parent_class)->style_set (widget, previous_style);

    hildon_app_menu_apply_style (widget);
}

static void
hildon_app_menu_repack_filters                  (HildonAppMenu *menu)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(menu);
    GList *iter;

    for (iter = priv->filters; iter != NULL; iter = iter->next) {
        GtkWidget *filter = GTK_WIDGET (iter->data);
        GtkWidget *parent = gtk_widget_get_parent (filter);
        if (parent) {
            g_object_ref (filter);
            gtk_container_remove (GTK_CONTAINER (parent), filter);
        }
    }

    for (iter = priv->filters; iter != NULL; iter = iter->next) {
        GtkWidget *filter = GTK_WIDGET (iter->data);
        if (GTK_WIDGET_VISIBLE (filter)) {
            gtk_box_pack_start (GTK_BOX (priv->filters_hbox), filter, TRUE, TRUE, 0);
            g_object_unref (filter);
        }
    }
}

/*
 * When items displayed in the menu change (e.g, a new item is added,
 * an item is hidden or the list is reordered), the layout must be
 * updated. To do this we repack all items starting from a given one.
 */
static void
hildon_app_menu_repack_items                    (HildonAppMenu *menu,
                                                 gint           start_from)
{
    HildonAppMenuPrivate *priv;
    gint row, col;
    GList *iter;

    priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    /* Remove buttons from their parent */
    if (start_from != -1) {
        for (iter = g_list_nth (priv->buttons, start_from); iter != NULL; iter = iter->next) {
            GtkWidget *item = GTK_WIDGET (iter->data);
            GtkWidget *parent = gtk_widget_get_parent (item);
            if (parent) {
                g_object_ref (item);
                gtk_container_remove (GTK_CONTAINER (parent), item);
            }
        }

        /* If items have been removed, recalculate the size of the menu */
        gtk_window_resize (GTK_WINDOW (menu), 1, 1);
    }

    /* Add buttons */
    row = col = 0;
    for (iter = priv->buttons; iter != NULL; iter = iter->next) {
        GtkWidget *item = GTK_WIDGET (iter->data);
        if (GTK_WIDGET_VISIBLE (item)) {
            /* Don't add an item to the table if it's already there */
            if (gtk_widget_get_parent (item) == NULL) {
                gtk_table_attach_defaults (priv->table, item, col, col + 1, row, row + 1);
                g_object_unref (item);
            }
            if (++col == priv->columns) {
                col = 0;
                row++;
            }
        }
    }

    /* The number of rows/columns might have changed, so we have to
     * resize the table */
    if (col == 0) {
        gtk_table_resize (priv->table, MAX (row, 1), priv->columns);
    } else {
        gtk_table_resize (priv->table, row + 1, priv->columns);
    }

    if (GTK_WIDGET_VISIBLE (GTK_WIDGET (menu))) {
        gtk_window_reshow_with_initial_size (GTK_WINDOW (menu));
    }
}

static void
hildon_app_menu_init                            (HildonAppMenu *menu)
{
    GtkWidget *alignment;
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(menu);

    /* Initialize private variables */
    priv->parent_window = NULL;
    priv->transfer_window = NULL;
    priv->pressed_outside = FALSE;
    priv->buttons = NULL;
    priv->filters = NULL;
    priv->columns = 2;

    /* Create boxes and tables */
    priv->filters_hbox = GTK_BOX (gtk_hbox_new (TRUE, 0));
    priv->vbox = GTK_BOX (gtk_vbox_new (FALSE, 0));
    priv->table = GTK_TABLE (gtk_table_new (1, priv->columns, TRUE));

    /* Align the filters to the center */
    alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (alignment), GTK_WIDGET (priv->filters_hbox));

    /* Pack everything */
    gtk_container_add (GTK_CONTAINER (menu), GTK_WIDGET (priv->vbox));
    gtk_box_pack_start (priv->vbox, alignment, TRUE, TRUE, 0);
    gtk_box_pack_start (priv->vbox, GTK_WIDGET (priv->table), TRUE, TRUE, 0);

    /* Make menu a modal window */
    gtk_window_set_modal (GTK_WINDOW (menu), TRUE);

    /* This should be treated like a normal, ref-counted widget */
    g_object_force_floating (G_OBJECT (menu));
    GTK_WINDOW (menu)->has_user_ref_count = FALSE;

    gtk_widget_show_all (GTK_WIDGET (priv->vbox));
}

static void
hildon_app_menu_finalize                        (GObject *object)
{
    HildonAppMenuPrivate *priv = HILDON_APP_MENU_GET_PRIVATE(object);

    if (priv->parent_window) {
        g_signal_handlers_disconnect_by_func (priv->parent_window, parent_window_topmost_notify, object);
        g_signal_handlers_disconnect_by_func (priv->parent_window, parent_window_unmapped, object);
    }

    if (priv->transfer_window)
        gdk_window_destroy (priv->transfer_window);

    g_list_foreach (priv->buttons, (GFunc) g_object_unref, NULL);
    g_list_foreach (priv->filters, (GFunc) g_object_unref, NULL);

    g_list_free (priv->buttons);
    g_list_free (priv->filters);

    g_signal_handlers_destroy (object);
    G_OBJECT_CLASS (hildon_app_menu_parent_class)->finalize (object);
}

static void
hildon_app_menu_class_init                      (HildonAppMenuClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class = (GtkWidgetClass *)klass;

    gobject_class->finalize = hildon_app_menu_finalize;
    widget_class->show = hildon_app_menu_show;
    widget_class->map = hildon_app_menu_map;
    widget_class->unmap = hildon_app_menu_unmap;
    widget_class->realize = hildon_app_menu_realize;
    widget_class->unrealize = hildon_app_menu_unrealize;
    widget_class->grab_notify = hildon_app_menu_grab_notify;
    widget_class->key_press_event = hildon_app_menu_key_press;
    widget_class->button_press_event = hildon_app_menu_button_press;
    widget_class->button_release_event = hildon_app_menu_button_release;
    widget_class->style_set = hildon_app_menu_style_set;

    g_type_class_add_private (klass, sizeof (HildonAppMenuPrivate));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "horizontal-spacing",
            "Horizontal spacing on menu items",
            "Horizontal spacing between each menu item. Does not apply to filter buttons.",
            0, G_MAXUINT, 16,
            G_PARAM_READABLE));

    gtk_widget_class_install_style_property (
        widget_class,
        g_param_spec_uint (
            "vertical-spacing",
            "Vertical spacing on menu items",
            "Vertical spacing between each menu item. Does not apply to filter buttons.",
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
