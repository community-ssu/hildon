/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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

/*
 * @file hildon-program.c
 *
 * This file implements the HildonProgram object
 *
 */

#include <config.h>
#include "hildon-program.h"
#include "hildon-window-private.h"
#include <X11/Xatom.h>

#define HILDON_PROGRAM_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_PROGRAM, HildonProgramPriv));


typedef struct _HildonProgramPriv HildonProgramPriv;

struct _HildonProgramPriv
{
    gboolean killable;
    gboolean is_topmost;
    GdkWindow *group_leader;
    guint window_count;
    GtkWidget *common_menu;
    GtkWidget *common_toolbar;
    GSList *windows;
    Window window_group;
    gchar *name;
};

static void
hildon_program_init (HildonProgram *self);

static void
hildon_program_finalize (GObject *self);

static void
hildon_program_class_init (HildonProgramClass *self);

static void
hildon_program_get_property(GObject * object, guint property_id,
                        GValue * value, GParamSpec * pspec);
static void
hildon_program_set_property (GObject * object, guint property_id,
                             const GValue * value, GParamSpec * pspec);

enum
{
    PROP_0,
    PROP_IS_TOPMOST,
    PROP_KILLABLE
};


GType
hildon_program_get_type (void)
{
    static GType program_type = 0;

    if (!program_type)
    {
        static const GTypeInfo program_info =
        {
            sizeof(HildonProgramClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_program_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonProgram),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_program_init,
        };
        program_type = g_type_register_static(G_TYPE_OBJECT,
                "HildonProgram", &program_info, 0);
    }
    return program_type;
}

static void
hildon_program_init (HildonProgram *self)
{
    HildonProgramPriv *priv = HILDON_PROGRAM_GET_PRIVATE (self);

    priv->killable = FALSE;
    priv->window_count = 0;
    priv->is_topmost = FALSE;
    priv->window_group = GDK_WINDOW_XID (gdk_display_get_default_group
                                  (gdk_display_get_default()));
    priv->common_toolbar = NULL;
    priv->name = NULL;
}

static void
hildon_program_finalize (GObject *self)
{
    HildonProgramPriv *priv = HILDON_PROGRAM_GET_PRIVATE (HILDON_PROGRAM (self));
    
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

    g_free (priv->name);

}

static void
hildon_program_class_init (HildonProgramClass *self)
{
    GObjectClass *object_class = G_OBJECT_CLASS(self);

    g_type_class_add_private (self, sizeof(HildonProgramPriv));

    /* Set up object virtual functions */
    object_class->finalize = hildon_program_finalize;
    object_class->set_property = hildon_program_set_property;
    object_class->get_property = hildon_program_get_property;

    /* Install properties */
    g_object_class_install_property (object_class, PROP_IS_TOPMOST,
                g_param_spec_boolean ("is-topmost",
                "Is top-most",
                "Whether one of the program's window or dialog currently "
                "is activated by window manager",
                FALSE,
                G_PARAM_READABLE)); 
    
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
hildon_program_set_property (GObject * object, guint property_id,
                             const GValue * value, GParamSpec * pspec)
{
    switch (property_id){
        case PROP_KILLABLE:
            hildon_program_set_can_hibernate (HILDON_PROGRAM (object),
                                         g_value_get_boolean (value));
            break;
            
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
            break;
    }

}

static void
hildon_program_get_property (GObject * object, guint property_id,
                             GValue * value, GParamSpec * pspec)
{
    HildonProgramPriv *priv = HILDON_PROGRAM_GET_PRIVATE (object);

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

/* Utilities */
static gint 
hildon_program_window_list_compare (gconstpointer window_a, 
                                    gconstpointer window_b)
{
    g_return_val_if_fail (1, HILDON_IS_WINDOW(window_a) && 
                             HILDON_IS_WINDOW(window_b));

    return window_a != window_b;
}

/*
 * foreach function, checks if a window is topmost and acts consequently
 */
static void
hildon_program_window_list_is_is_topmost (gpointer data, gpointer window_id_)
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
hildon_program_update_top_most (HildonProgram *program)
{
    XWMHints *wm_hints;
    Window active_window;
    HildonProgramPriv *priv;

    priv = HILDON_PROGRAM_GET_PRIVATE (program);
    
    active_window = hildon_window_get_active_window();

    if (active_window)
    {
      wm_hints = XGetWMHints (GDK_DISPLAY (), active_window);

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


/* Event filter */

/*
 * We keep track of the _MB_CURRENT_APP_WINDOW property on the root window,
 * to detect when a window belonging to this program was is_topmost. This
 * is based on the window group WM hint.
 */
static GdkFilterReturn
hildon_program_root_window_event_filter (GdkXEvent *xevent,
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
    

/**
 * hildon_program_common_toolbar_topmost_window:
 * @window: A @HildonWindow to be informed about its new common toolbar
 * @data: Not used, it is here just to respect the API
 *
 * Checks if the window is the topmost window of the program and in
 * that case forces the window to take the common toolbar.
 **/
static void
hildon_program_common_toolbar_topmost_window (gpointer window, gpointer data)
{
    if (HILDON_IS_WINDOW (window) && 
            hildon_window_get_is_topmost (HILDON_WINDOW (window)))
    {
        hildon_window_take_common_toolbar (HILDON_WINDOW (window));
    }
}

/* Public methods */

/**
 * hildon_program_get_instance:
 *
 * Return value: Returns the #HildonProgram for the current process.
 * The object is created on the first call.
 **/
HildonProgram *
hildon_program_get_instance ()
{
    static HildonProgram *program = NULL;

    if (!program)
    {
        program = g_object_new (HILDON_TYPE_PROGRAM, NULL);
    }

    return program;
}

/**
 * hildon_program_add_window:
 * @program: The @HildonProgram to which the window should be registered
 * @window: A @HildonWindow to be added
 *
 * Registers a @HildonWindow as belonging to a given @HildonProgram. This
 * allows to apply program-wide settings as all the registered windows,
 * such as hildon_program_set_common_menu() and
 * hildon_pogram_set_common_toolbar()
 **/
void
hildon_program_add_window (HildonProgram *self, HildonWindow *window)
{
    HildonProgramPriv *priv;
    
    g_return_if_fail (self && HILDON_IS_PROGRAM (self));
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);

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
                          gdk_window_get_events (gdk_get_default_root_window ())                          | GDK_PROPERTY_CHANGE_MASK);
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
hildon_program_remove_window (HildonProgram *self, HildonWindow *window)
{
    HildonProgramPriv *priv;
    
    g_return_if_fail (self && HILDON_IS_PROGRAM (self));
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);
    
    hildon_window_unset_program (window);

    priv->windows = g_slist_remove (priv->windows, window);

    priv->window_count --;

    if (!priv->window_count)
    {
        gdk_window_remove_filter (gdk_get_default_root_window(),
                hildon_program_root_window_event_filter,
                self);
    }
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
hildon_program_set_can_hibernate (HildonProgram *self, gboolean killable)
{
    HildonProgramPriv *priv;
    
    g_return_if_fail (self && HILDON_IS_PROGRAM (self));
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);

    if (priv->killable != killable)
    {
        g_slist_foreach (priv->windows, 
                (GFunc)hildon_window_set_can_hibernate_property, &killable);
    }

    priv->killable = killable;

}

/**
 * hildon_program_get_can_hibernate:
 * @self: The #HildonProgram which can hibernate or not
 * 
 * Return value: Whether or not this #HildonProgram is set to be
 * support hibernation from the Hildon task navigator
 **/
gboolean
hildon_program_get_can_hibernate (HildonProgram *self)
{
    HildonProgramPriv *priv;
    
    g_return_val_if_fail (self && HILDON_IS_PROGRAM (self), FALSE);
   
    priv = HILDON_PROGRAM_GET_PRIVATE (self);

    return priv->killable;

}

/**
 * hildon_program_set_common_menu:
 * @self: The #HildonProgram in which the common menu should be used
 * @menu: A GtkMenu to use as common menu for the program
 *
 * Sets a GtkMenu that will appear in all the @HildonWindow registered
 * to the #HildonProgram. Only one common GtkMenu can be set, further
 * call will detach the previous common GtkMenu. A @HildonWindow
 * can use it's own GtkMenu with @hildon_window_set_menu
 **/
void
hildon_program_set_common_menu (HildonProgram *self, GtkMenu *menu)
{
    HildonProgramPriv *priv;

    g_return_if_fail (self && HILDON_IS_PROGRAM (self));

    priv = HILDON_PROGRAM_GET_PRIVATE (self);

    if (priv->common_menu)
    {
        if (GTK_WIDGET_VISIBLE (priv->common_menu))
        {
            gtk_menu_popdown (GTK_MENU(priv->common_menu));
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
 * #HildonProgram, or NULL of no common menu was set.
 **/
GtkMenu *
hildon_program_get_common_menu (HildonProgram *self)
{
    HildonProgramPriv *priv;

    g_return_val_if_fail (self && HILDON_IS_PROGRAM (self), NULL);

    priv = HILDON_PROGRAM_GET_PRIVATE (self);

    return GTK_MENU (priv->common_menu);
}

/**
 * hildon_program_set_common_toolbar:
 * @self: The #HildonProgram in which the common toolbar should be used
 * @toolbar: A GtkToolbar to use as common toolbar for the program
 *
 * Sets a GtkToolbar that will appear in all the @HildonWindow registered
 * to the #HildonProgram. Only one common GtkToolbar can be set, further
 * call will detach the previous common GtkToolbar. A @HildonWindow
 * can use its own GtkToolbar with @hildon_window_set_toolbar. Both
 * #HildonProgram and @HildonWindow specific toolbars will be shown
 **/
void
hildon_program_set_common_toolbar (HildonProgram *self, GtkToolbar *toolbar)
{
    HildonProgramPriv *priv;

    g_return_if_fail (self && HILDON_IS_PROGRAM (self));

    priv = HILDON_PROGRAM_GET_PRIVATE (self);

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
 * #HildonProgram, or NULL of no common menu was set.
 **/
GtkToolbar *
hildon_program_get_common_toolbar (HildonProgram *self)
{
    HildonProgramPriv *priv;

    g_return_val_if_fail (self && HILDON_IS_PROGRAM (self), NULL);

    priv = HILDON_PROGRAM_GET_PRIVATE (self);

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
hildon_program_get_is_topmost (HildonProgram *self)
{
    HildonProgramPriv *priv;

    g_return_val_if_fail (self && HILDON_IS_PROGRAM (self), FALSE);
    
    priv = HILDON_PROGRAM_GET_PRIVATE (self);

    return priv->is_topmost;
}


