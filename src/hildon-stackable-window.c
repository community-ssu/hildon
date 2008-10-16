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
 * <title>Basic HildonStackableWindow example</title>
 * <programlisting>
 * static void
 * show_new_window (void)
 * {
 *     GtkWidget *win;
 * <!-- -->
 *     win = hildon_stackable_window_new ();
 * <!-- -->
 *     // ... configure new window
 * <!-- -->
 *     // This automatically hides the previous window
 *     gtk_widget_show (win);
 * }
 * <!-- -->
 * int
 * main (int argc, char **argv)
 * {
 *     GtkWidget *win;
 *     GtkWidget *button;
 * <!-- -->
 *     gtk_init (&amp;argc, &amp;args);
 * <!-- -->
 *     win = hildon_stackable_window_new ();
 *     gtk_window_set_title (GTK_WINDOW (win), "Main window);
 * <!-- -->
 *     // ... add some widgets to the window
 * <!-- -->
 *     g_signal_connect (button, "clicked", G_CALLBACK (show_new_window), NULL);
 *     g_signal_connect (win, "destroy", G_CALLBACK (gtk_main_quit), NULL);
 * <!-- -->
 *     gtk_widget_show_all (win);
 *     gtk_main ();
 * <!-- -->
 *     return 0;
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
 * current menu. Any reference to a previous menu will be dropped.
 * #HildonStackableWindow takes ownership of the passed menu and
 * you're not supposed to free it yourself anymore.
 *
 * Note that #HildonStackableWindow widgets use #HildonAppMenu rather
 * than #GtkMenu, so you're not supposed to use
 * hildon_window_set_main_menu() with a #HildonStackableWindow.
 **/
void
hildon_stackable_window_set_main_menu           (HildonStackableWindow *self,
                                                 HildonAppMenu *menu)
{
    HildonStackableWindowPrivate *priv;
    HildonAppMenu *old_menu;

    g_return_if_fail (HILDON_IS_STACKABLE_WINDOW (self));
    g_return_if_fail (!menu || HILDON_IS_APP_MENU (menu));
    priv = HILDON_STACKABLE_WINDOW_GET_PRIVATE (self);

    old_menu = priv->app_menu;

    /* Add new menu */
    priv->app_menu = menu;
    if (menu)
        g_object_ref_sink (menu);

    /* Unref old menu */
    if (old_menu)
        g_object_unref (old_menu);
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
    unsigned long val = 1;

    GTK_WIDGET_CLASS (hildon_stackable_window_parent_class)->realize (widget);

    /* Set additional property "_HILDON_STACKABLE_WINDOW", to allow the WM to manage
       it as a stackable window. */
    display = gdk_drawable_get_display (widget->window);
    atom = gdk_x11_get_xatom_by_name_for_display (display, "_HILDON_STACKABLE_WINDOW");
    XChangeProperty (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (widget->window), atom,
                     XA_ATOM, 32, PropModeReplace,
                     (unsigned char *) &val, 1);
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
}

static void
hildon_stackable_window_hide                    (GtkWidget *widget)
{
    HildonProgram *program = hildon_program_get_instance ();
    _hildon_program_remove_from_stack (program, HILDON_STACKABLE_WINDOW (widget));
    GTK_WIDGET_CLASS (hildon_stackable_window_parent_class)->hide (widget);
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
    GtkWidgetClass    *widget_class = GTK_WIDGET_CLASS (klass);
    HildonWindowClass *window_class = HILDON_WINDOW_CLASS (klass);

    obj_class->finalize             = hildon_stackable_window_finalize;

    widget_class->realize           = hildon_stackable_window_realize;
    widget_class->show              = hildon_stackable_window_show;
    widget_class->hide              = hildon_stackable_window_hide;

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
