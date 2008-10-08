/*
 * This file is a part of hildon
 *
 * Copyright (C) 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
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
 * SECTION:hildon-program
 * @short_description: An object that represents an application running in the Hildon framework.
 * @see_also: #HildonWindow, #HildonStackableWindow
 *
 * The #HildonProgram is an object used to represent an application running
 * in the Hildon framework.
 *
 * Such an application is thought to have one or more #HildonWindow. These
 * shall be registered to the #HildonProgram with hildon_program_add_window(),
 * and can be unregistered similarly with hildon_program_remove_window().
 *
 * The #HildonProgram provides the programmer with commodities such
 * as applying a common toolbar and menu to all the #HildonWindow
 * registered to it. This is done with hildon_program_set_common_menu()
 * and hildon_program_set_common_toolbar().
 *
 * The #HildonProgram is also used to apply program-wide properties that
 * are specific to the Hildon framework. For instance
 * hildon_program_set_can_hibernate() sets whether or not an application
 * can be set to hibernate by the Hildon task navigator, in situations of
 * low memory.
 *
 * The #HildonProgram also contains a stack of
 * #HildonStackableWindow. Such windows will be automatically added to
 * the stack when shown, and removed when destroyed. The developer can
 * use the stack with hildon_program_pop_window_stack(),
 * hildon_program_peek_window_stack() and hildon_program_go_to_root_window().
 *
 * <example>
 * <programlisting>
 * HildonProgram *program;
 * HildonWindow *window1;
 * HildonWindow *window2;
 * GtkToolbar *common_toolbar, *window_specific_toolbar;
 * GtkMenu *menu;
 * <!-- -->
 * program = HILDON_PROGRAM (hildon_program_get_instance ());
 * <!-- -->
 * window1 = HILDON_WINDOW (hildon_window_new ());
 * window2 = HILDON_WINDOW (hildon_window_new ());
 * <!-- -->
 * common_toolbar = create_common_toolbar ();
 * window_specific_toolbar = create_window_specific_toolbar ();
 * <!-- -->
 * menu = create_menu ();
 * <!-- -->
 * hildon_program_add_window (program, window1);
 * hildon_program_add_window (program, window2);
 * <!-- -->
 * hildon_program_set_common_menu (program, menu);
 * <!-- -->
 * hildon_program_set_common_toolbar (program, common_toolbar);
 * hildon_window_add_toolbar (window1, window_specific_toolbar);
 * <!-- -->
 * hildon_program_set_can_hibernate (program, TRUE);
 * </programlisting>
 * </example>
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        <X11/Xatom.h>

#include                                        "hildon-program.h"
#include                                        "hildon-program-private.h"
#include                                        "hildon-window-private.h"
#include                                        "hildon-stackable-window-private.h"

static void
hildon_program_init                             (HildonProgram *self);

static void
hildon_program_finalize                         (GObject *self);

static void
hildon_program_class_init                       (HildonProgramClass *self);

static void
hildon_program_get_property                     (GObject *object, 
                                                 guint property_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec);
static void
hildon_program_set_property                     (GObject *object, 
                                                 guint property_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec);

enum
{
    PROP_0,
    PROP_IS_TOPMOST,
    PROP_KILLABLE
};

GType G_GNUC_CONST
hildon_program_get_type                         (void)
{
    static GType program_type = 0;

    if (! program_type)
    {
        static const GTypeInfo program_info =
        {
            sizeof (HildonProgramClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_program_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonProgram),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_program_init,
        };
        program_type = g_type_register_static(G_TYPE_OBJECT,
                "HildonProgram", &program_info, 0);
    }
    return program_type;
}

static void
hildon_program_init                             (HildonProgram *self)
{
    HildonProgramPrivate *priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);
    
    priv->killable = FALSE;
    priv->window_count = 0;
    priv->is_topmost = FALSE;
    priv->window_group = GDK_WINDOW_XID (gdk_display_get_default_group (gdk_display_get_default()));
    priv->common_toolbar = NULL;
    priv->windows = NULL;
    priv->window_stack = NULL;
}

static void
hildon_program_finalize                         (GObject *self)
{
    HildonProgramPrivate *priv = HILDON_PROGRAM_GET_PRIVATE (HILDON_PROGRAM (self));
    g_assert (priv);
    
    if (priv->common_toolbar)
    {
        g_object_unref (priv->common_toolbar);
        priv->common_toolbar = NULL;
    }

    if (priv->common_menu)
    {
        g_object_unref (priv->common_menu);
        priv->common_menu = NULL;
    }
}

static void
hildon_program_class_init                       (HildonProgramClass *self)
{
    GObjectClass *object_class = G_OBJECT_CLASS (self);

    g_type_class_add_private (self, sizeof (HildonProgramPrivate));

    /* Set up object virtual functions */
    object_class->finalize      = hildon_program_finalize;
    object_class->set_property  = hildon_program_set_property;
    object_class->get_property  = hildon_program_get_property;

    /* Install properties */

    /**
     * HildonProgram:is-topmost:
     *
     * Whether one of the program's window or dialog currently
     * is activated by window manager. 
     */
    g_object_class_install_property (object_class, PROP_IS_TOPMOST,
                g_param_spec_boolean ("is-topmost",
                "Is top-most",
                "Whether one of the program's window or dialog currently "
                "is activated by window manager",
                FALSE,
                G_PARAM_READABLE)); 

    /**
     * HildonProgram:can-hibernate:
     *
     * Whether the program should be set to hibernate by the Task
     * Navigator in low memory situation.
     */
    g_object_class_install_property (object_class, PROP_KILLABLE,
                g_param_spec_boolean ("can-hibernate",
                "Can hibernate",
                "Whether the program should be set to hibernate by the Task "
                "Navigator in low memory situation",
                FALSE,
                G_PARAM_READWRITE)); 
    return;
}

static void
hildon_program_set_property                     (GObject *object, 
                                                 guint property_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec)
{
    switch (property_id) {

        case PROP_KILLABLE:
            hildon_program_set_can_hibernate (HILDON_PROGRAM (object), g_value_get_boolean (value));
            break;
            
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

static void
hildon_program_get_property                     (GObject *object, 
                                                 guint property_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonProgramPrivate *priv = HILDON_PROGRAM_GET_PRIVATE (object);
    g_assert (priv);

    switch (property_id)
    {
        case PROP_KILLABLE:
            g_value_set_boolean (value, priv->killable);
            break;

        case PROP_IS_TOPMOST:
            g_value_set_boolean (value, priv->is_topmost);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }
}

/**
 * hildon_program_pop_window_stack:
 * @self: A #HildonProgram
 *
 * The #HildonProgram object maintains a list of stackable
 * windows. Each time a #HildonStackableWindow is shown, it is
 * automatically added to the top of the stack. Windows are removed
 * from the stack when they are destroyed.
 *
 * This function removes the #HildonStackableWindow from the top of
 * the stack and returns it. If the stack is empty, %NULL is returned.
 *
 * If the window was visible then it will be hidden. The next window
 * from the stack (if any) will be shown automatically.
 *
 * If the window was not visible then all other windows will remain
 * hidden.
 *
 * No window is destroyed in any case.
 *
 * Returns: A #HildonStackableWindow, or %NULL.
 */
HildonStackableWindow *
hildon_program_pop_window_stack                 (HildonProgram *self)
{
    HildonStackableWindow *top;

    top = hildon_program_peek_window_stack (self);

    if (top)
    {
        HildonStackableWindow *next;

        /* Remove the window from the stack and get the next one */
        _hildon_program_remove_from_stack (self, top);
        next = hildon_program_peek_window_stack (self);

        /* Hide the window just removed and show the next one if necessary */
        if (GTK_WIDGET_VISIBLE (GTK_WIDGET (top)) && next != NULL);
            gtk_widget_show (GTK_WIDGET (next));

        gtk_widget_hide (GTK_WIDGET (top));
    }

    return top;
}

/**
 * hildon_program_peek_window_stack:
 * @self: A #HildonProgram
 *
 * The #HildonProgram object maintains a list of stackable
 * windows. Each time a #HildonStackableWindow is shown, it is
 * automatically added to the top of the stack. Windows are removed
 * from the stack when they are destroyed.
 *
 * This function returns the #HildonStackableWindow from the top of
 * the stack, or %NULL if the stack is empty. The stack is never modified.
 *
 * Returns: A #HildonStackableWindow, or %NULL.
 */
HildonStackableWindow *
hildon_program_peek_window_stack                (HildonProgram *self)
{
    HildonStackableWindow *top = NULL;
    HildonProgramPrivate *priv;

    g_return_val_if_fail (HILDON_IS_PROGRAM (self), NULL);

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    if (priv->window_stack != NULL)
        top = HILDON_STACKABLE_WINDOW (priv->window_stack->data);

    return top;
}

void G_GNUC_INTERNAL
_hildon_program_add_to_stack                    (HildonProgram         *self,
                                                 HildonStackableWindow *win)
{
    HildonProgramPrivate *priv;

    g_return_if_fail (HILDON_IS_PROGRAM (self));
    g_return_if_fail (HILDON_IS_STACKABLE_WINDOW (win));

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    if (g_slist_find (priv->window_stack, win) == NULL)
    {
        priv->window_stack = g_slist_prepend (priv->window_stack, win);
    }
    else
    {
        g_critical ("%s: window already in the stack!", __FUNCTION__);
    }

}

gboolean G_GNUC_INTERNAL
_hildon_program_remove_from_stack               (HildonProgram         *self,
                                                 HildonStackableWindow *win)
{
    GSList *pos;
    HildonProgramPrivate *priv;

    g_return_val_if_fail (HILDON_IS_PROGRAM (self), FALSE);
    g_return_val_if_fail (HILDON_IS_STACKABLE_WINDOW (win), FALSE);

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    pos = g_slist_find (priv->window_stack, win);

    if (pos != NULL)
        priv->window_stack = g_slist_delete_link (priv->window_stack, pos);

    return (pos != NULL);
}

/* Utilities */
static gint 
hildon_program_window_list_compare              (gconstpointer window_a, 
                                                 gconstpointer window_b)
{
    g_return_val_if_fail (HILDON_IS_WINDOW(window_a) && 
                          HILDON_IS_WINDOW(window_b), 1);

    return window_a != window_b;
}

/*
 * foreach function, checks if a window is topmost and acts consequently
 */
static void
hildon_program_window_list_is_is_topmost        (gpointer data, 
                                                 gpointer window_id_)
{
    if (data && HILDON_IS_WINDOW (data))
    {
        HildonWindow *window = HILDON_WINDOW (data);
        Window window_id = * (Window*)window_id_;

        hildon_window_update_topmost (window, window_id);
    }
}

/*
 * Check the _MB_CURRENT_APP_WINDOW on the root window, and update
 * the top_most status accordingly
 */
static void
hildon_program_update_top_most                  (HildonProgram *program)
{
    XWMHints *wm_hints;
    Window active_window;
    HildonProgramPrivate *priv;

    priv = HILDON_PROGRAM_GET_PRIVATE (program);
    g_assert (priv);
    
    active_window = hildon_window_get_active_window();

    if (active_window)
    {
      gint xerror;
      
      gdk_error_trap_push ();
      wm_hints = XGetWMHints (GDK_DISPLAY (), active_window);
      xerror = gdk_error_trap_pop ();
      if (xerror)
        return;

      if (wm_hints)
      {

          if (wm_hints->window_group == priv->window_group)
          {
              if (!priv->is_topmost)
              {
                  priv->is_topmost = TRUE;
                  g_object_notify (G_OBJECT (program), "is-topmost");
              }
          }
          else if (priv->is_topmost)
          {
            priv->is_topmost = FALSE;
            g_object_notify (G_OBJECT (program), "is-topmost");
          }
      }
      XFree (wm_hints);
    }

    /* Check each window if it was is_topmost */
    g_slist_foreach (priv->windows, 
            (GFunc)hildon_program_window_list_is_is_topmost, &active_window);
}

/*
 * We keep track of the _MB_CURRENT_APP_WINDOW property on the root window,
 * to detect when a window belonging to this program was is_topmost. This
 * is based on the window group WM hint.
 */
static GdkFilterReturn
hildon_program_root_window_event_filter         (GdkXEvent *xevent,
                                                 GdkEvent *event,
                                                 gpointer data)
{
    XAnyEvent *eventti = xevent;
    HildonProgram *program = HILDON_PROGRAM (data);
    Atom active_app_atom =
            XInternAtom (GDK_DISPLAY (), "_MB_CURRENT_APP_WINDOW", False);

    if (eventti->type == PropertyNotify)
    {
        XPropertyEvent *pevent = xevent;

        if (pevent->atom == active_app_atom)
        {
            hildon_program_update_top_most (program);
        }
    }

    return GDK_FILTER_CONTINUE;
}

/* 
 * Checks if the window is the topmost window of the program and in
 * that case forces the window to take the common toolbar.
 */
static void
hildon_program_common_toolbar_topmost_window    (gpointer window, 
                                                 gpointer data)
{
    if (HILDON_IS_WINDOW (window) && hildon_window_get_is_topmost (HILDON_WINDOW (window)))
        hildon_window_take_common_toolbar (HILDON_WINDOW (window));
}

/**
 * hildon_program_get_instance:
 *
 * Return value: Returns the #HildonProgram for the current process.
 * The object is created on the first call. Note that you're not supposed 
 * to unref the returned object since it's not reffed in the first place.
 **/
HildonProgram*
hildon_program_get_instance                     (void)
{
    static HildonProgram *program = NULL;

    if (! program)
    {
        program = g_object_new (HILDON_TYPE_PROGRAM, NULL);
    }

    return program;
}

/**
 * hildon_program_add_window:
 * @self: The #HildonProgram to which the window should be registered
 * @window: A #HildonWindow to be added
 *
 * Registers a #HildonWindow as belonging to a given #HildonProgram. This
 * allows to apply program-wide settings as all the registered windows,
 * such as hildon_program_set_common_menu() and
 * hildon_pogram_set_common_toolbar()
 **/
void
hildon_program_add_window                       (HildonProgram *self, 
                                                 HildonWindow *window)
{
    HildonProgramPrivate *priv;
    
    g_return_if_fail (HILDON_IS_PROGRAM (self));
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    if (g_slist_find_custom (priv->windows, window,
           hildon_program_window_list_compare) )
    {
        /* We already have that window */
        return;
    }

    if (!priv->window_count)
    {
        hildon_program_update_top_most (self);
        
        /* Now that we have a window we should start keeping track of
         * the root window */
        gdk_window_set_events (gdk_get_default_root_window (),
                gdk_window_get_events (gdk_get_default_root_window ()) | GDK_PROPERTY_CHANGE_MASK);

        gdk_window_add_filter (gdk_get_default_root_window (),
                hildon_program_root_window_event_filter, self );
    }
    
    hildon_window_set_can_hibernate_property (window, &priv->killable);

    hildon_window_set_program (window, G_OBJECT (self));

    priv->windows = g_slist_append (priv->windows, window);
    priv->window_count ++;
}

/**
 * hildon_program_remove_window:
 * @self: The #HildonProgram to which the window should be unregistered
 * @window: The @HildonWindow to unregister
 *
 * Used to unregister a window from the program. Subsequent calls to
 * hildon_program_set_common_menu() and hildon_pogram_set_common_toolbar()
 * will not affect the window
 **/
void
hildon_program_remove_window                    (HildonProgram *self, 
                                                 HildonWindow *window)
{
    HildonProgramPrivate *priv;
    
    g_return_if_fail (HILDON_IS_PROGRAM (self));
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);
    
    hildon_window_unset_program (window);

    priv->windows = g_slist_remove (priv->windows, window);

    priv->window_count --;

    if (! priv->window_count)
        gdk_window_remove_filter (gdk_get_default_root_window(),
                hildon_program_root_window_event_filter,
                self);
}

/**
 * hildon_program_set_can_hibernate:
 * @self: The #HildonProgram which can hibernate or not
 * @can_hibernate: whether or not the #HildonProgram can hibernate
 *
 * Used to set whether or not the Hildon task navigator should
 * be able to set the program to hibernation in case of low memory
 **/
void
hildon_program_set_can_hibernate                (HildonProgram *self, 
                                                 gboolean can_hibernate)
{
    HildonProgramPrivate *priv;
    
    g_return_if_fail (HILDON_IS_PROGRAM (self));
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    if (priv->killable != can_hibernate)
    {
        g_slist_foreach (priv->windows, 
                (GFunc) hildon_window_set_can_hibernate_property, &can_hibernate);
    }

    priv->killable = can_hibernate;
}

/**
 * hildon_program_get_can_hibernate:
 * @self: The #HildonProgram which can hibernate or not
 * 
 * Return value: Whether or not this #HildonProgram is set to be
 * support hibernation from the Hildon task navigator
 **/
gboolean
hildon_program_get_can_hibernate                (HildonProgram *self)
{
    HildonProgramPrivate *priv;
    
    g_return_val_if_fail (HILDON_IS_PROGRAM (self), FALSE);
   
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    return priv->killable;
}

/**
 * hildon_program_set_common_menu:
 * @self: The #HildonProgram in which the common menu should be used
 * @menu: A GtkMenu to use as common menu for the program
 *
 * Sets a GtkMenu that will appear in all the #HildonWindow registered
 * with the #HildonProgram. Only one common GtkMenu can be set, further
 * calls will detach the previous common GtkMenu. A #HildonWindow
 * can use it's own GtkMenu with hildon_window_set_menu()
 *
 * This method does not support #HildonAppMenu objects.
 **/
void
hildon_program_set_common_menu                  (HildonProgram *self, 
                                                 GtkMenu *menu)
{
    HildonProgramPrivate *priv;

    g_return_if_fail (HILDON_IS_PROGRAM (self));

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    if (priv->common_menu)
    {
        if (GTK_WIDGET_VISIBLE (priv->common_menu))
        {
            gtk_menu_popdown (GTK_MENU (priv->common_menu));
            gtk_menu_shell_deactivate (GTK_MENU_SHELL (priv->common_menu));
        }

        if (gtk_menu_get_attach_widget (GTK_MENU (priv->common_menu)))
        {
            gtk_menu_detach (GTK_MENU (priv->common_menu));
        }
        else
        {
            g_object_unref (priv->common_menu);
        }
    }

    priv->common_menu = GTK_WIDGET (menu);

    if (priv->common_menu)
    {
        g_object_ref (menu);
        gtk_object_sink (GTK_OBJECT (menu));
        gtk_widget_show_all (GTK_WIDGET (menu));
    }
}

/**
 * hildon_program_get_common_menu:
 * @self: The #HildonProgram from which to retrieve the common menu
 *
 * Return value: the GtkMenu that was set as common menu for this
 * #HildonProgram, or %NULL of no common menu was set.
 **/
GtkMenu*
hildon_program_get_common_menu                  (HildonProgram *self)
{
    HildonProgramPrivate *priv;

    g_return_val_if_fail (HILDON_IS_PROGRAM (self), NULL);

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    return GTK_MENU (priv->common_menu);
}

/**
 * hildon_program_set_common_toolbar:
 * @self: The #HildonProgram in which the common toolbar should be used
 * @toolbar: A GtkToolbar to use as common toolbar for the program
 *
 * Sets a GtkToolbar that will appear in all the #HildonWindow registered
 * to the #HildonProgram. Only one common GtkToolbar can be set, further
 * call will detach the previous common GtkToolbar. A #HildonWindow
 * can use its own GtkToolbar with hildon_window_add_toolbar(). Both
 * #HildonProgram and #HildonWindow specific toolbars will be shown
 **/
void
hildon_program_set_common_toolbar               (HildonProgram *self, 
                                                 GtkToolbar *toolbar)
{
    HildonProgramPrivate *priv;

    g_return_if_fail (HILDON_IS_PROGRAM (self));

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    if (priv->common_toolbar)
    {
        if (priv->common_toolbar->parent)
        {
            gtk_container_remove (GTK_CONTAINER (priv->common_toolbar->parent), 
                                  priv->common_toolbar);
        }
        
        g_object_unref (priv->common_toolbar);
    }

    priv->common_toolbar = GTK_WIDGET (toolbar);

    if (priv->common_toolbar)
    {
        g_object_ref (priv->common_toolbar);
        gtk_object_sink (GTK_OBJECT (priv->common_toolbar) );
    }

    /* if the program is the topmost we have to update the common
       toolbar right now for the topmost window */
    if (priv->is_topmost)
      {
        g_slist_foreach (priv->windows, 
                         (GFunc) hildon_program_common_toolbar_topmost_window, NULL);
      }
}

/**
 * hildon_program_get_common_toolbar:
 * @self: The #HildonProgram from which to retrieve the common toolbar
 *
 * Return value: the GtkToolbar that was set as common toolbar for this
 * #HildonProgram, or %NULL of no common menu was set.
 **/
GtkToolbar*
hildon_program_get_common_toolbar               (HildonProgram *self)
{
    HildonProgramPrivate *priv;

    g_return_val_if_fail (HILDON_IS_PROGRAM (self), NULL);

    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    return priv->common_toolbar ? GTK_TOOLBAR (priv->common_toolbar) : NULL;
}

/**
 * hildon_program_get_is_topmost:
 * @self: A #HildonWindow
 *
 * Return value: Whether or not one of the program's window or dialog is 
 * currenltly activated by the window manager.
 **/
gboolean
hildon_program_get_is_topmost                   (HildonProgram *self)
{
    HildonProgramPrivate *priv;

    g_return_val_if_fail (HILDON_IS_PROGRAM (self), FALSE);
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    return priv->is_topmost;
}

/**
 * hildon_program_go_to_root_window:
 * @self: A #HildonProgram
 *
 * Will close all windows in the #HildonProgram but the first one (the
 * root window) by sending them a delete event. If any of the windows
 * refuses to close (by capturing the event) no further events will be
 * sent. Only windows of type #HildonStackableWindow will be affected
 * by this.
 */
void
hildon_program_go_to_root_window                (HildonProgram *self)
{
    HildonProgramPrivate *priv;
    GSList *windows, *iter;
    gboolean windows_left;

    g_return_if_fail (HILDON_IS_PROGRAM (self));
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    g_assert (priv);

    /* List of stacked windows (starting from the topmost one) */
    windows = g_slist_copy (priv->window_stack);
    iter = windows;

    /* Destroy all the windows but the last one (which is the root
     * window, as the list is reversed) */
    windows_left = (iter != NULL && iter->next != NULL);
    while (windows_left)
    {
        if (HILDON_IS_STACKABLE_WINDOW (iter->data))
        {
            GdkEvent *event;
            HildonStackableWindow *win;

            /* Mark the window as "going home" */
            win = HILDON_STACKABLE_WINDOW (iter->data);
            hildon_stackable_window_set_going_home (win, TRUE);

            /* Set win pointer to NULL if the window is destroyed */
            g_object_add_weak_pointer (G_OBJECT (win), (gpointer) &win);

            /* Send a delete event */
            event = gdk_event_new (GDK_DELETE);
            event->any.window = g_object_ref (GTK_WIDGET (win)->window);
            gtk_main_do_event (event);
            gdk_event_free (event);

            /* Continue sending delete events if the window has been destroyed */
            if (win == NULL)
            {
                iter = iter->next;
                windows_left = (iter != NULL && iter->next != NULL);
            }
            else
            {
                g_object_remove_weak_pointer (G_OBJECT (win), (gpointer) &win);
                hildon_stackable_window_set_going_home (win, FALSE);
                windows_left = FALSE;
            }
        }
        else
        {
            g_critical ("Window list contains a non-stackable window");
            windows_left = FALSE;
        }
    }

    /* Show the last window that hasn't been destroyed */
    if (iter != NULL && GTK_IS_WIDGET (iter->data))
    {
        gtk_widget_show (GTK_WIDGET (iter->data));
    }

    g_slist_free (windows);
}
