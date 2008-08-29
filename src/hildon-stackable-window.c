/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
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
 * SECTION:hildon-stackable-window
 * @short_description: Widget representing a stackable, top-level window in the Hildon framework.
 *
 * The #HildonStackableWindow is a GTK+ widget which represents a
 * top-level window in the Hildon framework. It is derived from
 * #HildonWindow. Applications that use stackable windows are organized
 * in a hierarchical way so users can go from any window back to the
 * application's root window.
 *
 * To add a window to the stack, just use gtk_widget_show(). The
 * previous one will be automatically hidden. When the new window is
 * destroyed, the previous one will appear again.
 *
 * Alternatively, you can remove a window from the top of the stack
 * without destroying it by using
 * hildon_program_pop_window_stack(). The window will be automatically
 * hidden and the previous one will appear.
 *
 * <example>
 * <programlisting>
 * void
 * show_first_window (void)
 * {
 *     GtkWidget *win1;
 * <!-- -->
 *     win1 = hildon_stackable_window_new ();
 * <!-- -->
 *     // ... configure first window
 * <!-- -->
 *     gtk_widget_show (win1);
 * }
 * <!-- -->
 * void
 * show_second_window (void)
 * {
 *     GtkWidget *win2;
 * <!-- -->
 *     win2 = hildon_stackable_window_new ();
 * <!-- -->
 *     // ... configure second window
 * <!-- -->
 *     gtk_widget_show (win2);
 * }
 * </programlisting>
 * </example>
 */

#include                                        <X11/X.h>
#include                                        <X11/Xatom.h>

#include                                        "hildon-stackable-window.h"
#include                                        "hildon-stackable-window-private.h"
#include                                        "hildon-program.h"
#include                                        "hildon-window-private.h"
#include                                        "hildon-program-private.h"

G_DEFINE_TYPE (HildonStackableWindow, hildon_stackable_window, HILDON_TYPE_WINDOW);

void G_GNUC_INTERNAL
hildon_stackable_window_set_going_home          (HildonStackableWindow *self,
                                                 gboolean going_home)
{
    HildonStackableWindowPrivate *priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (self);
    priv->going_home = going_home;
}

gboolean G_GNUC_INTERNAL
hildon_stackable_window_get_going_home          (HildonStackableWindow *self)
{
    HildonStackableWindowPrivate *priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (self);
    return priv->going_home;
}

/**
 * hildon_stackable_window_set_main_menu:
 * @self: a #HildonStackableWindow
 * @menu: a #HildonAppMenu to be used for this window
 *
 * Sets the menu to be used for this window. Pass %NULL to remove the
 * current menu. #HildonStackableWindow takes ownership of the passed
 * menu and you're not supposed to free it yourself anymore.
 **/
void
hildon_stackable_window_set_main_menu           (HildonStackableWindow *self,
                                                 HildonAppMenu *menu)
{
    HildonStackableWindowPrivate *priv;

    g_return_if_fail (HILDON_IS_STACKABLE_WINDOW (self));
    g_return_if_fail (!menu || HILDON_IS_APP_MENU (menu));
    priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (self);

    /* Remove reference to old menu */
    if (priv->app_menu)
        g_object_unref (priv->app_menu);

    /* Add new menu */
    priv->app_menu = menu;
    if (priv->app_menu)
        g_object_ref (priv->app_menu);
}

static gboolean
hildon_stackable_window_toggle_menu             (HildonWindow *self,
						 guint button,
						 guint32 time)
{
    HildonStackableWindowPrivate *priv;

    g_return_val_if_fail (HILDON_IS_STACKABLE_WINDOW (self), FALSE);
    priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (self);
    g_assert (priv != NULL);

    if (priv->app_menu)
    {
        if (GTK_WIDGET_MAPPED (GTK_WIDGET (priv->app_menu)))
            gtk_widget_hide (GTK_WIDGET (priv->app_menu));
        else
            gtk_widget_show (GTK_WIDGET (priv->app_menu));

        return TRUE;
    }
    else if (HILDON_WINDOW_CLASS (hildon_stackable_window_parent_class)->toggle_menu)
    {
        return HILDON_WINDOW_CLASS (hildon_stackable_window_parent_class)->toggle_menu (self, button, time);
    }
    else
    {
        return FALSE;
    }
}

static void
hildon_stackable_window_realize                 (GtkWidget *widget)
{
    GdkDisplay *display;
    Atom atom;

    GTK_WIDGET_CLASS (hildon_stackable_window_parent_class)->realize (widget);

    /* Set the window type to "_HILDON_WM_WINDOW_TYPE_STACKABLE", to allow the WM to manage
       it properly.  */
    display = gdk_drawable_get_display (widget->window);
    atom = gdk_x11_get_xatom_by_name_for_display (display, "_HILDON_WM_WINDOW_TYPE_STACKABLE");
    XChangeProperty (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (widget->window),
		     gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_WINDOW_TYPE"),
		     XA_ATOM, 32, PropModeAppend,
		     (guchar *)&atom, 1);
}

static void
hildon_stackable_window_show                    (GtkWidget *widget)
{
    HildonProgram *program = hildon_program_get_instance ();
    HildonStackableWindow *current_win = HILDON_STACKABLE_WINDOW (widget);
    HildonStackableWindow *previous_win = hildon_program_peek_window_stack (program);

    if (previous_win != current_win)
        _hildon_program_add_to_stack (program, current_win);

    GTK_WIDGET_CLASS (hildon_stackable_window_parent_class)->show (widget);

    if (previous_win != NULL && previous_win != current_win)
        gtk_widget_hide (GTK_WIDGET (previous_win));
}

static void
hildon_stackable_window_destroy                 (GtkObject *obj)
{
    HildonProgram *program = hildon_program_get_instance ();
    HildonStackableWindow *topmost = hildon_program_peek_window_stack (program);

    if (_hildon_program_remove_from_stack (program, HILDON_STACKABLE_WINDOW (obj)))
    {
        if (topmost != NULL && GTK_OBJECT (topmost) == obj)
        {
            HildonStackableWindowPrivate *priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (obj);
            topmost = hildon_program_peek_window_stack (program);
            if (topmost != NULL && !priv->going_home)
                gtk_widget_show (GTK_WIDGET (topmost));
        }
    }

    GTK_OBJECT_CLASS (hildon_stackable_window_parent_class)->destroy (obj);
}

static void
hildon_stackable_window_finalize                (GObject *object)
{
    HildonStackableWindowPrivate *priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (object);

    if (priv->app_menu)
        gtk_widget_destroy (GTK_WIDGET (priv->app_menu));

    if (G_OBJECT_CLASS (hildon_stackable_window_parent_class)->finalize)
        G_OBJECT_CLASS (hildon_stackable_window_parent_class)->finalize (object);
}

static void
hildon_stackable_window_class_init              (HildonStackableWindowClass *klass)
{
    GObjectClass      *obj_class    = G_OBJECT_CLASS (klass);
    GtkObjectClass    *gtk_obj_class = GTK_OBJECT_CLASS (klass);
    GtkWidgetClass    *widget_class = GTK_WIDGET_CLASS (klass);
    HildonWindowClass *window_class = HILDON_WINDOW_CLASS (klass);

    obj_class->finalize             = hildon_stackable_window_finalize;

    gtk_obj_class->destroy          = hildon_stackable_window_destroy;

    widget_class->realize           = hildon_stackable_window_realize;
    widget_class->show              = hildon_stackable_window_show;

    window_class->toggle_menu       = hildon_stackable_window_toggle_menu;

    g_type_class_add_private (klass, sizeof (HildonStackableWindowPrivate));
}

static void
hildon_stackable_window_init                    (HildonStackableWindow *self)
{
    HildonStackableWindowPrivate *priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (self);

    priv->going_home = FALSE;
    priv->app_menu = NULL;
}

/**
 * hildon_stackable_window_new:
 *
 * Creates a new #HildonStackableWindow.
 *
 * Return value: A #HildonStackableWindow
 **/
GtkWidget*
hildon_stackable_window_new                     (void)
{
    HildonStackableWindow *newwindow = g_object_new (HILDON_TYPE_STACKABLE_WINDOW, NULL);

    return GTK_WIDGET (newwindow);
}
