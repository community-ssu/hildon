/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
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

/**
 * SECTION:hildon-app
 * @short_description: A base widget to present application. Deprecated, use #HildonProgram instead.
 * @see_also: #HildonAppView
 *
 * #HildonApp is the base for any hildon application.
 * It controls basic looks and functionality of an application, like a title.
 *
 * This widget is deprecated use #HildonProgram instead.
 */

#include <gdk/gdk.h>
#include "hildon-app.h"
#include "hildon-app-private.h"
#include "hildon-appview.h"
#include "gtk-infoprint.h"

#include <gdk/gdkevents.h>
#include <gdk/gdkkeysyms.h>
#include <X11/Xatom.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkeditable.h>
#include <gtk/gtktextview.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkuimanager.h>
#include <gtk/gtkactiongroup.h>
#include <gtk/gtkdialog.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkcombobox.h>

#include <libintl.h>
#include <string.h>

#include <libmb/mbutil.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define TITLE_DELIMITER " - "

/*
 * 'Magic' values for the titlebar menu area limits
 */
#define MENUAREA_LEFT_LIMIT 80
#define MENUAREA_RIGHT_LIMIT MENUAREA_LEFT_LIMIT + 307
#define MENUAREA_TOP_LIMIT 0
#define MENUAREA_BOTTOM_LIMIT 39

#define KILLABLE "CANKILL"

#define _(String) dgettext(PACKAGE, String)

#define HILDON_APP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
      HILDON_TYPE_APP, HildonAppPrivate));

static GtkWindowClass *parent_class;
static guint app_signals[HILDON_APP_LAST_SIGNAL] = { 0 };

typedef struct _HildonAppPrivate HildonAppPrivate;

static gboolean
hildon_app_key_press (GtkWidget *widget, GdkEventKey *keyevent);
static gboolean
hildon_app_key_release (GtkWidget *widget, GdkEventKey *keyevent);
static gboolean
hildon_app_key_snooper (GtkWidget *widget, GdkEventKey *keyevent, HildonApp *app);
static GdkFilterReturn
hildon_app_event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer data);
static void
hildon_app_construct_title (HildonApp *self);
static void
hildon_app_finalize (GObject *obj_self);
static void
hildon_app_destroy (GtkObject *obj);
static void
hildon_app_init (HildonApp *self);
static void
hildon_app_class_init (HildonAppClass *app_class);
static void
hildon_app_real_topmost_status_acquire (HildonApp *self);
static void
hildon_app_real_topmost_status_lose (HildonApp *self);
static void
hildon_app_real_switch_to (HildonApp *self);
static gboolean
hildon_app_button (GtkWidget *widget, GdkEventButton *event);
static GdkWindow *
find_window (GdkWindow *window, gint by, gint co);
static void
hildon_app_clipboard_copy(HildonApp *self, GtkWidget *widget);
static void
hildon_app_clipboard_cut(HildonApp *self, GtkWidget *widget);
static void
hildon_app_clipboard_paste(HildonApp *self, GtkWidget *widget);
static gboolean hildon_app_escape_timeout(gpointer data);
	
static void hildon_app_set_property(GObject * object, guint property_id,
                                    const GValue * value, GParamSpec * pspec);
static void hildon_app_get_property(GObject * object, guint property_id,
                                    GValue * value, GParamSpec * pspec);

static void hildon_app_add (GtkContainer *container, GtkWidget *child);
static void hildon_app_remove (GtkContainer *container, GtkWidget *child);
static void hildon_app_forall (GtkContainer *container, gboolean include_internals,
			       GtkCallback callback, gpointer callback_data);

enum {
  PROP_0,
  PROP_SCROLL_CONTROL,
  /* FIXME: Zoom is deprecated, should be removed */
  PROP_ZOOM,
  PROP_TWO_PART_TITLE,
  PROP_APP_TITLE,
  PROP_KILLABLE,
  PROP_AUTOREGISTRATION,
  PROP_APPVIEW,
  PROP_UI_MANAGER
};

static gpointer find_view(HildonApp *self, unsigned long view_id);

/* FIXME: Zoom level is deprecated, should be removed */
/**
 * hildon_zoom_level_get_type:
 *
 * Initialises, and returns the type of a hildon zoom level
 * 
 * Returns: GType of #HildonZoomLevel
 */

GType
hildon_zoom_level_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { HILDON_ZOOM_SMALL, "HILDON_ZOOM_SMALL", "small" },
      { HILDON_ZOOM_MEDIUM, "HILDON_ZOOM_MEDIUM", "medium" },
      { HILDON_ZOOM_LARGE, "HILDON_ZOOM_LARGE", "large" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("HildonZoomLevel", values);
  }
  return etype;
}

GType hildon_app_get_type(void)
{
    static GType app_type = 0;

    if (!app_type)
      {
        static const GTypeInfo app_info =
          {
            sizeof(HildonAppClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_app_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonApp),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_app_init,
          };
        app_type = g_type_register_static(GTK_TYPE_WINDOW,
                                          "HildonApp", &app_info, 0);
      }
    return app_type;
}

/*
 * Sets or delete a custom property into the XServer, according
 * to the boolean value of HildonAppPrivate::killable
 */ 
static void hildon_app_apply_killable(HildonApp *self)
{
    HildonAppPrivate *priv;
    Atom killability_atom = XInternAtom (GDK_DISPLAY(),
				       "_HILDON_APP_KILLABLE", False);
    priv = HILDON_APP_GET_PRIVATE (self);

    g_assert (HILDON_IS_APP (self) );
    g_assert(GTK_WIDGET_REALIZED(self));

    if (priv->killable)
    {
      /* Set the atom to specific value, because perhaps in the future,
	       there may be other possible values? */
      XChangeProperty(GDK_DISPLAY(),
		      GDK_WINDOW_XID(GTK_WIDGET(self)->window),
		      killability_atom, XA_STRING, 8,
		      PropModeReplace, (unsigned char *)KILLABLE,
		      strlen(KILLABLE));
    }
    else
    {
      XDeleteProperty(GDK_DISPLAY(),
		      GDK_WINDOW_XID(GTK_WIDGET(self)->window),
		      killability_atom);
    }
}

/*
 * Updates the _NET_CLIENT_LIST property into the XServer.
 * It is the list of the views associated to the HildonApp.
 * It will be used by the Task Navigator in order to be able to show a list
 * of all the views, and let the user switch and navigate them.
 */
static void hildon_app_apply_client_list(HildonApp *self)
{
  HildonAppPrivate *priv;
  Window *win_array;
  GSList *list_ptr;
  int loopctr = 0;
  Atom clientlist;

  g_assert (HILDON_IS_APP (self) );
  g_assert(GTK_WIDGET_REALIZED(self));

  /* Get the client list handle */
  clientlist = XInternAtom (GDK_DISPLAY(),
    "_NET_CLIENT_LIST", False);

  /* Allocate a new array for window IDs */
  priv = HILDON_APP_GET_PRIVATE(self);
  win_array = g_new(Window, g_slist_length(priv->view_ids));
  
  /* Fill the contents of the window array with current view IDs */
  for (list_ptr = priv->view_ids; list_ptr; list_ptr = list_ptr->next)
  {
      win_array[loopctr] = 
	(unsigned long)(((view_item *)(list_ptr->data))->view_id);
      loopctr++;
  }

  /* Update the details of current view IDs to our X property */
  XChangeProperty(GDK_DISPLAY(), GDK_WINDOW_XID(GTK_WIDGET(self)->window),
		  clientlist, XA_WINDOW, 32, PropModeReplace,
		  (unsigned char *)win_array,
		  g_slist_length(priv->view_ids));

  XFlush(GDK_DISPLAY());
  g_free(win_array);
}

/*
 * Performs the standard gtk realize function.
 */
static void hildon_app_realize(GtkWidget *widget)
{
    HildonApp *self;
    HildonAppPrivate *priv;
    GdkWindow *window;
    Atom *old_atoms, *new_atoms;
    gint atom_count;
    Display *disp;

    g_assert(widget != NULL);

    self = HILDON_APP(widget);
    priv = HILDON_APP_GET_PRIVATE(self);

    /*
     * Of course we need to realize the parent.
     * parent_class got already initialised in the hildon_app_init function
     */
    GTK_WIDGET_CLASS(parent_class)->realize(widget);

    /* some initialisation code */
    hildon_app_apply_killable(self); 
    hildon_app_construct_title(self);
    hildon_app_apply_client_list(self);
    hildon_app_notify_view_changed(self, hildon_app_get_appview(self));
    window = widget->window;
    disp = GDK_WINDOW_XDISPLAY(window);

    /* Install a key snooper for the Home button - so that it works everywhere */
    priv->key_snooper = gtk_key_snooper_install 
        ((GtkKeySnoopFunc) hildon_app_key_snooper, widget);

    /* Get the list of Atoms for the WM_PROTOCOLS property... */
    XGetWMProtocols(disp, GDK_WINDOW_XID(window), &old_atoms, &atom_count);
    new_atoms = g_new(Atom, atom_count + 1);

    memcpy(new_atoms, old_atoms, sizeof(Atom) * atom_count);

    /* ... creates a new Atom... */
    new_atoms[atom_count++] =
        XInternAtom(disp, "_NET_WM_CONTEXT_CUSTOM", False);

    /* ... and finally update the property within the XServer */
    XSetWMProtocols(disp, GDK_WINDOW_XID(window), new_atoms, atom_count);

    XFree(old_atoms);
    g_free(new_atoms);

    /* Add the GDK_SUBSTRUCTURE_MASK (receive events about window configuration
     * changes of child windows) to the window.
     */
    gdk_window_set_events(window, gdk_window_get_events(window) | GDK_SUBSTRUCTURE_MASK);
}

/*
 * Performs the standard gtk unrealize function.
 */
static void hildon_app_unrealize(GtkWidget *widget)
{
  HildonAppPrivate *priv = HILDON_APP_GET_PRIVATE(widget);

  if (priv->key_snooper)
  {
    /* removing the snooper that handles MENU key presses */
    gtk_key_snooper_remove(priv->key_snooper);
    priv->key_snooper = 0;
  }

  gdk_window_remove_filter(NULL, hildon_app_event_filter, widget);
  GTK_WIDGET_CLASS(parent_class)->unrealize(widget);
}

/*
 * Class initialisation.
 */
static void hildon_app_class_init (HildonAppClass *app_class)
{
    /* get convenience variables */
    GObjectClass *object_class = G_OBJECT_CLASS(app_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS (app_class);
    GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS(app_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(app_class);

    /* set the global parent_class here */
    parent_class = g_type_class_peek_parent(app_class);

    g_type_class_add_private(app_class, sizeof(HildonAppPrivate));

    /* now the object stuff */
    object_class->finalize = hildon_app_finalize;
    object_class->set_property = hildon_app_set_property;
    object_class->get_property = hildon_app_get_property;

    gtkobject_class->destroy = hildon_app_destroy;

    widget_class->key_press_event = hildon_app_key_press;
    widget_class->key_release_event = hildon_app_key_release;
    widget_class->button_press_event = hildon_app_button;
    widget_class->button_release_event = hildon_app_button;
    widget_class->realize = hildon_app_realize;
    widget_class->unrealize = hildon_app_unrealize;

    container_class->add = hildon_app_add;
    container_class->remove = hildon_app_remove;
    container_class->forall = hildon_app_forall;

    app_class->topmost_status_acquire =
        hildon_app_real_topmost_status_acquire;
    app_class->topmost_status_lose = hildon_app_real_topmost_status_lose;
    app_class->switch_to = hildon_app_real_switch_to;

    /* create the signals */
    app_signals[TOPMOST_STATUS_ACQUIRE] =
        g_signal_new("topmost_status_acquire",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppClass,
                                     topmost_status_acquire), NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    app_signals[TOPMOST_STATUS_LOSE] =
        g_signal_new("topmost_status_lose",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppClass, topmost_status_lose),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    app_signals[SWITCH_TO] =
      g_signal_new("switch_to",
		   G_OBJECT_CLASS_TYPE(object_class),
		   G_SIGNAL_RUN_FIRST,
		   G_STRUCT_OFFSET(HildonAppClass, switch_to),
		   NULL, NULL,
		   g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);
		   
    app_signals[IM_CLOSE] =
      g_signal_new("im_close",
		   G_OBJECT_CLASS_TYPE(object_class),
		   G_SIGNAL_RUN_FIRST,
		   G_STRUCT_OFFSET(HildonAppClass, im_close),
		   NULL, NULL,
		   g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    app_signals[CLIPBOARD_COPY] =
      g_signal_new("clipboard_copy",
		   G_OBJECT_CLASS_TYPE(object_class),
		   G_SIGNAL_RUN_FIRST,
		   G_STRUCT_OFFSET(HildonAppClass, clipboard_copy),
		   NULL, NULL,
		   g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1,
		   GTK_TYPE_WIDGET);
    app_signals[CLIPBOARD_CUT] =
      g_signal_new("clipboard_cut",
		   G_OBJECT_CLASS_TYPE(object_class),
		   G_SIGNAL_RUN_FIRST,
		   G_STRUCT_OFFSET(HildonAppClass, clipboard_cut),
		   NULL, NULL,
		   g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1,
		   GTK_TYPE_WIDGET);
    app_signals[CLIPBOARD_PASTE] =
      g_signal_new("clipboard_paste",
		   G_OBJECT_CLASS_TYPE(object_class),
		   G_SIGNAL_RUN_FIRST,
		   G_STRUCT_OFFSET(HildonAppClass, clipboard_paste),
		   NULL, NULL,
		   g_cclosure_marshal_VOID__OBJECT, G_TYPE_NONE, 1,
		   GTK_TYPE_WIDGET);

    /* properties */
    g_object_class_install_property(object_class, PROP_SCROLL_CONTROL,
        g_param_spec_boolean("scroll-control",
                            "Scroll control",
                            "Set the scroll control ON/OFF",
			     TRUE, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_TWO_PART_TITLE,
				    g_param_spec_boolean("two-part-title",
							 "Two part title",
							 "Use two part title or not",
							 FALSE, G_PARAM_READWRITE));
#ifndef HILDON_DISABLE_DEPRECATED
    g_object_class_install_property(object_class, PROP_ZOOM,
				    g_param_spec_enum ("zoom",
						       "Zoom level",
						       "Set the zoom level",
						       HILDON_TYPE_ZOOM_LEVEL,
						       HILDON_ZOOM_MEDIUM,
						       G_PARAM_READWRITE));
#endif
    g_object_class_install_property(object_class, PROP_APP_TITLE,
				    g_param_spec_string ("app-title",
							 "Application title",
							 "Set the application title",
							 "",
							 G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_KILLABLE,
				    g_param_spec_boolean("killable",
							 "Killable",
							 "Whether the application is killable or not",
							 FALSE,
							 G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_AUTOREGISTRATION,
				    g_param_spec_boolean("autoregistration",
							 "Autoregistration",
							 "Whether the application views should be registered automatically",
							 TRUE,
							 G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_APPVIEW,
				    g_param_spec_object("appview",
							 "Appplication View",
							 "The currently active application view",
							 HILDON_TYPE_APPVIEW,
							 G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_UI_MANAGER,
				    g_param_spec_object("ui-manager",
							 "UIManager",
							 "The associated GtkUIManager for this app",
							 GTK_TYPE_UI_MANAGER,
							 G_PARAM_READWRITE));
}

/*
 * Performs the standard gtk finalize function, freeing allocated
 * memory and propagating the finalization to the parent.
 */
static void
hildon_app_finalize (GObject *obj)
{
  HildonAppPrivate *priv = NULL;
  
  g_assert (obj != NULL);

  priv = HILDON_APP_GET_PRIVATE (obj);

  g_free (priv->title);

  if (G_OBJECT_CLASS(parent_class)->finalize)
    G_OBJECT_CLASS(parent_class)->finalize(obj);

  /* FIXME: This is legacy code, but cannot be removed 
     without changing functionality */
  gtk_main_quit ();
}

/*
 * Removes the long escape ("cancel" hw key) press timeout.
 */
static void
hildon_app_remove_timeout(HildonAppPrivate *priv)
{
  g_assert(priv != NULL);

  if (priv->escape_timeout > 0)
    {
      g_source_remove (priv->escape_timeout);
      priv->escape_timeout = 0;
    }
}

/*
 * Frees all the resources and propagates the destroy call to the parent.
 */
static void
hildon_app_destroy (GtkObject *obj)
{
  HildonAppPrivate *priv = NULL;

  g_assert (obj != NULL);
  
  priv = HILDON_APP_GET_PRIVATE (obj);

  /* Just in case a GDK_Escape key was pressed shortly before the propagation
   * of this event, it is safer to remove the timeout that will generate a
   * GDK_DELETE key press. We are destroying the app, so we're not interested
   * anymore in processing key presses.
   */
  hildon_app_remove_timeout(priv);

  if (priv->uim != NULL)
    {
      g_object_unref (G_OBJECT (priv->uim));
      priv->uim = NULL;
    }

  /* Free all the views */
  if (priv->view_ids)
    {
      g_slist_foreach (priv->view_ids, (GFunc)g_free, NULL);
      g_slist_free (priv->view_ids);
      priv->view_ids = NULL;
    }

  if (GTK_OBJECT_CLASS (parent_class)->destroy)
    GTK_OBJECT_CLASS (parent_class)->destroy(obj);
}

/*
 * Overrides gtk_container_forall, calling the callback function for each of
 * the children of HildonAppPrivate.
 */
static void hildon_app_forall (GtkContainer *container, gboolean include_internals,
			       GtkCallback callback, gpointer callback_data)
{
  HildonAppPrivate *priv = NULL;

  g_return_if_fail (container != NULL);
  g_return_if_fail (callback != NULL);

  priv = HILDON_APP_GET_PRIVATE (container);

  /* Note! we only have user added children, no internals */
  g_list_foreach (priv->children, (GFunc)callback, callback_data);
}

/*
 * An accessor to set private properties of HildonAppPrivate.
 */
static void hildon_app_set_property(GObject * object, guint property_id,
                                    const GValue * value, GParamSpec * pspec)
{
    HildonAppPrivate *priv = HILDON_APP_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_SCROLL_CONTROL:
        priv->scroll_control = g_value_get_boolean(value);
        break;
#ifndef HILDON_DISABLE_DEPRECATED
    case PROP_ZOOM:
        hildon_app_set_zoom( HILDON_APP (object), g_value_get_enum (value) );
        break; 
#endif
    case PROP_TWO_PART_TITLE:
 	hildon_app_set_two_part_title( HILDON_APP (object), 
				       g_value_get_boolean (value) );
 	break;
     case PROP_APP_TITLE:
 	hildon_app_set_title( HILDON_APP (object), g_value_get_string (value));
 	break;
    case PROP_KILLABLE:
        hildon_app_set_killable( HILDON_APP (object), 
			       g_value_get_boolean (value));
 	break;
    case PROP_AUTOREGISTRATION:
        hildon_app_set_autoregistration( HILDON_APP (object), 
			       g_value_get_boolean (value));
 	break;
    case PROP_APPVIEW:
        hildon_app_set_appview( HILDON_APP (object), 
			       HILDON_APPVIEW (g_value_get_object (value)));
 	break;
    case PROP_UI_MANAGER:
        hildon_app_set_ui_manager( HILDON_APP (object), 
			          GTK_UI_MANAGER (g_value_get_object (value)));
 	break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

/*
 * An accessor to get private properties of HildonAppPrivate.
 */
static void hildon_app_get_property(GObject * object, guint property_id,
                                    GValue * value, GParamSpec * pspec)
{
    HildonAppPrivate *priv = HILDON_APP_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_SCROLL_CONTROL:
        g_value_set_boolean( value, priv->scroll_control );
        break;
#ifndef HILDON_DISABLE_DEPRECATED
    case PROP_ZOOM:
 	g_value_set_enum( value, priv->zoom);
 	break;
#endif
    case PROP_TWO_PART_TITLE:
 	g_value_set_boolean( value, priv->twoparttitle);
 	break;
    case PROP_APP_TITLE:
 	g_value_set_string (value, priv->title);
 	break;
    case PROP_KILLABLE:
 	g_value_set_boolean (value, priv->killable);
 	break;
    case PROP_AUTOREGISTRATION:
 	g_value_set_boolean (value, priv->autoregistration);
 	break;
    case PROP_APPVIEW:
 	g_value_set_object (value, hildon_app_get_appview (HILDON_APP (object)));
 	break;
    case PROP_UI_MANAGER:
 	g_value_set_object (value, hildon_app_get_ui_manager (HILDON_APP (object)));
 	break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

/*
 * Adds a child widget to HildonApp.
 */
static void hildon_app_add (GtkContainer *container, GtkWidget *child)
{
  HildonApp        *app  = NULL;
  HildonAppPrivate *priv = NULL;
  
  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_WIDGET (child));

  app  = HILDON_APP (container);
  priv = HILDON_APP_GET_PRIVATE (app);

  /* Check if child is already added here */
  if (g_list_find (priv->children, child) != NULL)
    return;

  priv->children = g_list_append (priv->children, child);
  GTK_BIN (container)->child = child;
  gtk_widget_set_parent (child, GTK_WIDGET (app));

  /* If the default direction (RTL/LTR) is different from the real
   default, change it This happens if the locale has been changed
   but this appview was orphaned and thus never got to know about
   it. "default_direction" could be RTL, but the widget direction
   of the view might still be LTR. Thats what we're fixing here. */

  /* FIXME: This is legacy stuff */
  if (gtk_widget_get_default_direction () !=
      gtk_widget_get_direction (GTK_WIDGET (child)))
    {
        gtk_widget_set_direction (GTK_WIDGET (child),
      			      gtk_widget_get_default_direction ());
    }

  if (HILDON_IS_APPVIEW (child))
    {
      g_signal_connect_swapped (G_OBJECT (child), "title_change",
                                G_CALLBACK (hildon_app_construct_title), app);
      if (priv->autoregistration)
        hildon_app_register_view (app, child);
    }
}

/*
 * Removes a child widget from HildonApp.
 */
static void hildon_app_remove (GtkContainer *container, GtkWidget *child)
{
  HildonAppPrivate *priv;
  GtkBin *bin;
  HildonApp *app;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_WIDGET (child));

  priv = HILDON_APP_GET_PRIVATE (container);
  bin = GTK_BIN (container);
  app = HILDON_APP (bin);

  /* Make sure that child is found in the list */
  if (g_list_find (priv->children, child) == NULL)
    return;

  priv->children = g_list_remove (priv->children, child);

  if (HILDON_IS_APPVIEW (child))
    {
  /* FIXME: This is a compilation workaround for gcc > 3.3, since glib-2 API is buggy.
   * see http://bugzilla.gnome.org/show_bug.cgi?id=310175 */

G_GNUC_EXTENSION

      g_signal_handlers_disconnect_by_func (G_OBJECT (child),
                                            (gpointer)hildon_app_construct_title, app);

      if (priv->autoregistration)
        hildon_app_unregister_view (app, HILDON_APPVIEW (child));
    }

  /* If that was our visible child, we need to recalculate size.
     We could chain up to parent as well... */
  gtk_widget_unparent (child);

  if (bin->child == child)
    {
      bin->child = NULL;
      gtk_widget_queue_resize (GTK_WIDGET (bin));
    }
}


/*
 * Long escape keypress handler.
 * Long press of the escape key means "close this window", so we fake a delete-event
 * for our GdkWindow to make it act the same as if the user had closed the window the
 * usual way. This allows any application code to gracefully exit.
 *
 * It returns FALSE in order to get called only once.
 */
static gboolean
hildon_app_escape_timeout(gpointer app)
{
	HildonAppPrivate *priv;
	GdkEvent *event;

	GDK_THREADS_ENTER ();

	g_assert(GTK_WIDGET_REALIZED(app));

	priv = HILDON_APP_GET_PRIVATE(app);

  /* Send fake event, simulation a situation that user 
     pressed 'x' from the corner */
  event = gdk_event_new(GDK_DELETE);
  ((GdkEventAny *)event)->window = GTK_WIDGET(app)->window;
  gtk_main_do_event(event);
  gdk_event_free(event);

	priv->escape_timeout = 0;

	GDK_THREADS_LEAVE ();
	
	return FALSE;	
}

/*
 * Looks for the visible window to whom the point of coordinates (co,by)
 * belongs. Search recursively all the children of the window.
 *
 * This functionality is only needed for scrollbar remote control.
 */
static GdkWindow *find_window (GdkWindow *window, gint by, gint co)
{
  GdkWindow *child;
  GList *children = gdk_window_peek_children (window);

	/* If the window has no children, then the coordinates must match it */
  if (!children)
    return window;

  if (!(child = (GdkWindow *)children->data))
    return window;

  do
    {
      /* It makes sense to process a child window only if it's visible and it's
       * capable to get the GDK_BUTTON_PRESS_MASK event. */
      if (gdk_window_is_visible (child) &&
	  gdk_window_get_events (child) & GDK_BUTTON_PRESS_MASK)
	{
	  gint x, width, y, height;

	  gdk_window_get_geometry (child, &x, &y, &width, &height, NULL);
      /* This checks that the the point of coordinates (co,by) is in the rectangle
       * made by (x,y,x+width,y+height). If so, then we spotted which child of the
       * original window is in the point (co,by). We can now recursively search
       * its own children.
       */
	  if (x < co && x + width > co && y < by && y + height > by)
	    return find_window (child, by, co);
	}

      /* If the window has no more children, then it's the one we're looking for */ 
      if (!(children = g_list_next (children)))
        return window;

    } while ( (child = children->data));

  return NULL;
}

/*
 * This is the callback function that gets called on a mouse button press
 * event. If the press is happens in a "sensitive border" area in the right side of
 * the window, the event is translated to the left edge of that border (which
 * usually will contain a scrollbar).
 *
 * This functionality is used for right-hand side scroll control feature (dragging on the
 * right edge of the window controls the rightmost scrollbar if present).
 */
static gboolean
hildon_app_button (GtkWidget *widget, GdkEventButton *event)
{
    HildonAppPrivate *priv = NULL;

    /* FIXME: This is an estimate, but the AppView does not expose the
    width of it's borders so we default to something */
    gint sensitive_border = 31;

    if (!GTK_WIDGET_REALIZED(widget))
    {
        return FALSE;
    }
    
    priv = HILDON_APP_GET_PRIVATE (widget);

    if (!priv->scroll_control)
    {
        return FALSE;
    }
    
    /* We can easily get the location of the vertical scrollbar and get the exact
    * area for the scroll_control *if* the setup is such that the HildonAppview
    * contains a GtkScrolledWindow, so we check for it before defaulting to
    * the previous guess. More complex situations are not feasible to autodetect.
    * Applications should provide the GtkAdjustment to be changed for this to work
    * flawlessly.
    */
    if (HILDON_IS_APPVIEW(GTK_BIN(widget)->child))
    {
        GtkBin *avbin = GTK_BIN(GTK_BIN(widget)->child);
        if (GTK_IS_SCROLLED_WINDOW(avbin->child))
        {
            GtkScrolledWindow *win;
            win = GTK_SCROLLED_WINDOW(avbin->child);
            
            if (GTK_WIDGET_VISIBLE(win->vscrollbar))
            {
                /* Calculate the distance between the AppView's right border and
                 * the scrollbars center
                 */
                sensitive_border = (GTK_WIDGET(avbin)->allocation.x +
                                                GTK_WIDGET(avbin)->allocation.width) -
                                                (win->vscrollbar->allocation.x +
                                                 win->vscrollbar->allocation.width / 2);
            }
        }
    }

    /* If the press event comes to the sensitive area, we send a fake event to the
     * area we think the scrollbar is in to make it think the button press happened on it
     */
    if (event->x > widget->allocation.width - sensitive_border)
    {
        GdkWindow *window = NULL;
        gint co = widget->allocation.width - sensitive_border;

        /* We now need to know in which window the _modified_ coordinates are */
        if ((window = find_window (widget->window, event->y, co)))
        {
            GdkEventButton nevent;

            if (window == widget->window)
                return FALSE;

            /* Build a new event and associate the proper window to it */
            nevent = *event;
            nevent.x = 8;
            nevent.window = window;
            g_object_ref (nevent.window);
            gtk_main_do_event ((GdkEvent*)&nevent);
        }
    }
    return FALSE;
}

/*
 * Performs the initialisation of the widget.
 */
static void
hildon_app_init (HildonApp *self)
{
    HildonAppPrivate *priv;

    priv = HILDON_APP_GET_PRIVATE(self);

    /* init private */
    priv->title = g_strdup("");
#ifndef HILDON_DISABLE_DEPRECATED
    priv->zoom = HILDON_ZOOM_MEDIUM;
#endif
    priv->twoparttitle = FALSE;
    priv->lastmenuclick = 0;
    priv->is_topmost = FALSE;
    priv->curr_view_id = 0;
    priv->view_id_counter = 1;
    priv->view_ids = NULL;
    priv->killable = FALSE;
    priv->autoregistration = TRUE;
    priv->scroll_control = TRUE;
    priv->uim = NULL;
    priv->active_menu_id = 0;

    /* grab the events here since HildonApp isn't necessarily ever shown */
    gdk_window_set_events(gdk_get_default_root_window(),
                          gdk_window_get_events(gdk_get_default_root_window()) |
                          GDK_PROPERTY_CHANGE_MASK);

    /* For some reason, the titlebar menu has problems with the grab
       (bugzilla bug 1527). This is part of somewhat ugly fix for it to
       get it to work until a more satisfactory solution is found */
    gdk_window_add_filter(NULL, hildon_app_event_filter, self);

    gtk_widget_set_events (GTK_WIDGET(self), GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK |
                           GDK_POINTER_MOTION_MASK);
}

/*public functions */

/**
 * hildon_app_new:
 *
 * Creates a new #HildonApp
 *
 * Returns: pointer to a new #HildonApp structure
 */
GtkWidget *
hildon_app_new (void)
{
    return GTK_WIDGET(g_object_new(HILDON_TYPE_APP, NULL));
}

/**
 * hildon_app_new_with_appview:
 * @appview : a #HildonAppView
 * 
 * Creates an app, and sets it's initial appview.
 * 
 * Returns: pointer to a new #HildonApp structure
 */
GtkWidget *
hildon_app_new_with_appview (HildonAppView *appview)
{
    GtkWidget *app;

    g_return_val_if_fail (HILDON_IS_APPVIEW (appview), NULL);

    app = hildon_app_new ();

    hildon_app_set_appview(HILDON_APP(app), appview);
    
    return app;
}

/**
 * hildon_app_get_appview:
 * @self : a #HildonApp
 *
 * Gets the currently shown appview.
 * 
 * Returns: the currently shown appview in this HildonApp.
 *          If no appview is currently set for this HildonApp,
 *          returns NULL.
 */
HildonAppView *
hildon_app_get_appview (HildonApp *self)
{
  GtkBin *bin;

  g_return_val_if_fail (HILDON_IS_APP (self), NULL);
  bin = GTK_BIN (self);
  if (HILDON_IS_APPVIEW (bin->child))
    {
      return HILDON_APPVIEW (bin->child);
    }
  
  return NULL;
}

/**
 * hildon_app_set_appview:
 * @self : a #HildonApp
 * @appview : a #HildonAppView
 * 
 * Sets (switches to) appview.
 */
void
hildon_app_set_appview (HildonApp *app, HildonAppView *view)
{
  HildonAppPrivate *priv;
  GtkBin *bin;
  GtkWidget *widget; /*(view to be set)*/
  gchar *menu_ui;

  g_return_if_fail (HILDON_IS_APP (app));
  g_return_if_fail (HILDON_IS_APPVIEW (view));

  bin = GTK_BIN (app);
  priv = HILDON_APP_GET_PRIVATE (app);
  widget = GTK_WIDGET (view);

  if (widget == bin->child)
    return;

  /* Make old appview dissapear */
  if (bin->child)
    {
      gtk_widget_hide (bin->child);
      g_signal_emit_by_name (bin->child, "switched_from", NULL);
      
      if (priv->active_menu_id > 0)
      {
        if (priv->uim != NULL)
          {
            gtk_ui_manager_remove_ui (priv->uim,
                                      priv->active_menu_id);
          }
        priv->active_menu_id = 0;
      }
      
      bin->child = NULL;
    }

  /* Ensure that new view is in our child list */
  if (!g_list_find (priv->children, widget))
    gtk_container_add (GTK_CONTAINER (app), widget);

  bin->child = widget;

  gtk_widget_show (widget);

  /* UI manager support, merge menu for activated view */
  g_object_get (G_OBJECT (view),
                "menu-ui", &menu_ui,
                NULL);

  if (menu_ui && priv->uim)
    {
     
      priv->active_menu_id =
        gtk_ui_manager_add_ui_from_string (priv->uim, menu_ui, -1, NULL);
      
      gtk_ui_manager_ensure_update (priv->uim);

    }

  g_free (menu_ui);

  g_signal_emit_by_name (widget, "switched_to", NULL);

  /* Inform task navigator about changed view */
  hildon_app_notify_view_changed (app, view);

  /* Update title to show currently activated view */
  hildon_app_construct_title (app);
  gtk_widget_child_focus (widget, GTK_DIR_TAB_FORWARD);
}

/**
 * hildon_app_set_title:
 * @self : a #HildonApp
 * @newtitle : the new title assigned to the application
 *
 * Sets title of the application.
 */
void
hildon_app_set_title (HildonApp *self, const gchar *newtitle)
{
    HildonAppPrivate *priv;
    gchar *oldstr;

    g_return_if_fail(HILDON_IS_APP(self));

    priv = HILDON_APP_GET_PRIVATE(self);
    oldstr = priv->title;

    if (newtitle)
      {
        priv->title = g_strdup(newtitle);
        g_strstrip(priv->title);
      }
    else
        priv->title = g_strdup("");

    if (oldstr)
        g_free(oldstr);

    hildon_app_construct_title(self);
}

/**
 * hildon_app_get_title:
 * @self : a #HildonApp
 *
 * Gets the title of the application.
 *
 * Returns: the title currently assigned to the application. This
 *  value is not to be freed or modified by the calling application
 */
const gchar *
hildon_app_get_title (HildonApp *self)
{
    HildonAppPrivate *priv;

    g_return_val_if_fail (HILDON_IS_APP(self), NULL);
    priv = HILDON_APP_GET_PRIVATE(self);
    return priv->title;
}

/* FIXME: Zoom is deprecated, remove */
/**
 * hildon_app_set_zoom:
 * @self : a #HildonApp
 * @newzoom: the zoom level of type #HildonZoomLevel to be assigned to an
 *  application
 *
 * Sets the zoom level. Warning! This function is deprecated and
 * should not be used. It's lecacy stuff from ancient specs.
 */
void
hildon_app_set_zoom (HildonApp *self, HildonZoomLevel newzoom)
{
    HildonAppPrivate *priv;

    g_return_if_fail(HILDON_IS_APP(self));

    priv = HILDON_APP_GET_PRIVATE(self);

    if (newzoom != priv->zoom)
      {
        if (newzoom < HILDON_ZOOM_SMALL)
          {
            newzoom = HILDON_ZOOM_SMALL;
            gtk_infoprint(GTK_WINDOW(self), _("ckct_ib_min_zoom_level_reached"));
          }
        else if (newzoom > HILDON_ZOOM_LARGE) {
            newzoom = HILDON_ZOOM_LARGE;
            gtk_infoprint(GTK_WINDOW(self), _("ckct_ib_max_zoom_level_reached"));
          }
        priv->zoom = newzoom;
      }
}

/**
 * hildon_app_get_zoom:
 * @self : a #HildonApp
 *
 * Gets the zoom level. Warning! This function is deprecated and
 * should not be used. It's lecacy stuff from ancient specifications.
 *
 * Returns: the zoom level of the Hildon application. The
 *  returned zoom level is of type #HildonZoomLevel.
 */
HildonZoomLevel
hildon_app_get_zoom (HildonApp *self)
{
    HildonAppPrivate *priv;

    g_return_val_if_fail(HILDON_IS_APP(self), HILDON_ZOOM_MEDIUM);
    priv = HILDON_APP_GET_PRIVATE(self);
    return priv->zoom;
}

/**
 * hildon_app_get_default_font:
 * @self : a #HildonApp
 *
 * Gets default font. Warning! This function is deprecated and should
 * not be used. It's legacy stuff from ancient version of specification.
 *
 * Returns: pointer to PangoFontDescription for the default,
 *  normal size font
 */
PangoFontDescription *
hildon_app_get_default_font (HildonApp *self)
{
    PangoFontDescription *font_desc = NULL;
    GtkStyle *fontstyle = NULL;

    g_return_val_if_fail(HILDON_IS_APP(self), NULL);

    fontstyle =
        gtk_rc_get_style_by_paths (gtk_widget_get_settings
                                  (GTK_WIDGET(self)), NULL, NULL,
                                   gtk_widget_get_type());

    if (!fontstyle)
      {
        g_print("WARNING : default font not found. "
                "Defaulting to swissa 19\n");
        font_desc = pango_font_description_from_string("swissa 19");

      }
    else
        font_desc = pango_font_description_copy(fontstyle->font_desc);

    return font_desc;
}

/**
 * hildon_app_get_zoom_font:
 * @self : a #HildonApp
 *
 * Gets the description of the default font. Warning! This function
 * is deprecated and should not be used. It's legacy stuff from
 * ancient specs.
 * 
 * Returns: pointer to PangoFontDescription for the default,
 *  normal size font
 */
PangoFontDescription *
hildon_app_get_zoom_font (HildonApp *self)
{
    HildonAppPrivate *priv;
    PangoFontDescription *font_desc = NULL;
    gchar *style_name = 0;
    GtkStyle *fontstyle = NULL;

    g_return_val_if_fail(HILDON_IS_APP(self), NULL);

    priv = HILDON_APP_GET_PRIVATE(self);
    if (priv->zoom == HILDON_ZOOM_SMALL)
        style_name = g_strdup("hildon-zoom-small");
    else if (priv->zoom == HILDON_ZOOM_MEDIUM)
        style_name = g_strdup("hildon-zoom-medium");
    else if (priv->zoom == HILDON_ZOOM_LARGE)
        style_name = g_strdup("hildon-zoom-large");
    else
      {
        g_warning("Invalid Zoom Value\n");
        style_name = g_strdup("");
      }

    fontstyle =
        gtk_rc_get_style_by_paths (gtk_widget_get_settings
                                  (GTK_WIDGET(self)), style_name, NULL,
                                   G_TYPE_NONE);
    g_free (style_name);

    if (!fontstyle)
      {
        g_print("WARNING : theme specific zoomed font not found. "
                "Defaulting to preset zoom-specific fonts\n");
        if (priv->zoom == HILDON_ZOOM_SMALL)
            font_desc = pango_font_description_from_string("swissa 16");
        else if (priv->zoom == HILDON_ZOOM_MEDIUM)
            font_desc = pango_font_description_from_string("swissa 19");
        else if (priv->zoom == HILDON_ZOOM_LARGE)
            font_desc = pango_font_description_from_string("swissa 23");
      }
    else
        font_desc = pango_font_description_copy(fontstyle->font_desc);

    return font_desc;
}

 /*  FIXME: Zoom is deprecated, remove the code above */

/**
 * hildon_app_set_two_part_title:
 * @self : a #HildonApp
 * @istwoparttitle : a gboolean indicating wheter to activate
 *  a title that has both the application title and application
 *  view title separated by a triangle
 * 
 * Sets the two part title.
 */
void
hildon_app_set_two_part_title (HildonApp *self, gboolean istwoparttitle)
{
    HildonAppPrivate *priv;
    g_return_if_fail(HILDON_IS_APP(self));
    priv = HILDON_APP_GET_PRIVATE(self);

    if (istwoparttitle != priv->twoparttitle)
      {
        priv->twoparttitle = istwoparttitle;
        hildon_app_construct_title(self);
      }
}

/**
 * hildon_app_get_two_part_title:
 * @self : a #HildonApp
 *
 * Gets the 'twopart' represention of the title inside #HildonApp.
 * 
 * Returns: a boolean indicating wheter title shown has both
 *  application, and application view title separated by a triangle.
 */
gboolean
hildon_app_get_two_part_title (HildonApp *self)
{
    HildonAppPrivate *priv;

    g_return_val_if_fail(HILDON_IS_APP(self), FALSE);
    priv = HILDON_APP_GET_PRIVATE(self);
    return priv->twoparttitle;
}


/* private functions */


/*
 * Handles the key press of the Escape, Increase and Decrease keys. Other keys
 * are handled by the parent GtkWidgetClass.
 */
static gboolean
hildon_app_key_press (GtkWidget *widget, GdkEventKey *keyevent)
{
  HildonApp *app = HILDON_APP (widget);
  HildonAppView *appview;
  HildonAppPrivate *priv = HILDON_APP_GET_PRIVATE(app);

  if (HILDON_IS_APPVIEW(GTK_BIN (app)->child))
    {
      appview = HILDON_APPVIEW (GTK_BIN (app)->child);
    }
  else
    {
      return FALSE;
    }

    if (keyevent->keyval == GDK_Escape && priv->escape_timeout == 0)
      {
        /* Call hildon_app_escape_timeout every 1500ms until it returns FALSE 
         * and store the relative GSource id. Since hildon_app_escape_timeout
         * can only return FALSE, the call will occurr only once.
         */
        priv->escape_timeout = g_timeout_add(1500, hildon_app_escape_timeout, app);
      }

    /* FIXME: Handling +/- keys here is not usefull. Applications
              can equally easily handle the keypress themselves. */
    else if (HILDON_KEYEVENT_IS_INCREASE_KEY (keyevent))
      {
        _hildon_appview_increase_button_state_changed (appview,
	                                               keyevent->type);
      }
    else if (HILDON_KEYEVENT_IS_DECREASE_KEY (keyevent))
      {
        _hildon_appview_decrease_button_state_changed (appview,
                                                       keyevent->type);
      }

    /* Activate default bindings and let the widget with focus to handle key */
    return GTK_WIDGET_CLASS (parent_class)->key_press_event (widget, keyevent);
}

/*
 * Handles the key release event for the Escape, Toolbar and Fullscreen keys.
 */
static gboolean
hildon_app_key_release (GtkWidget *widget, GdkEventKey *keyevent)
{
  HildonApp *app = HILDON_APP (widget);
  HildonAppView *appview;
  HildonAppPrivate *priv = HILDON_APP_GET_PRIVATE(app);

  if (HILDON_IS_APPVIEW(GTK_BIN (app)->child))
    {
      appview = HILDON_APPVIEW (GTK_BIN (app)->child);
    }
  else
    {
      return FALSE;
    }

    if (keyevent->keyval == GDK_Escape)
      {
        /*
         * This will prevent the hildon_app_escape_timeout from being called.
         * See hildon_app_escape_timeout and hildon_app_remove_timeout for more.
         */
        hildon_app_remove_timeout(priv);
      }
    else if (HILDON_KEYEVENT_IS_TOOLBAR_KEY (keyevent))
      {
        g_signal_emit_by_name(G_OBJECT(appview),
                              "toolbar-toggle-request");
      }
     else if (HILDON_KEYEVENT_IS_FULLSCREEN_KEY (keyevent))
       {
         /* Emit the fullscreen_state_change directly, it'll save one step */
         if (hildon_appview_get_fullscreen_key_allowed (appview))
           {
              gboolean fullscreen;
              
              fullscreen = hildon_appview_get_fullscreen(appview);
              g_signal_emit_by_name(G_OBJECT(appview),
                          "fullscreen_state_change",
                          !fullscreen);
           }
       }

    /* FIXME: Should the event be marked as handled if any of the three
              above cases took an action */

    /* Activate default bindings and let the widget with focus to handle key */
    return GTK_WIDGET_CLASS (parent_class)->key_release_event (widget, keyevent);
}

/*
 * Handles the MENU key presses.
 */
static gboolean
hildon_app_key_snooper (GtkWidget *widget, GdkEventKey *keyevent, HildonApp *app)
{
    /* FIXME: Using normal keypress handler would be better choise. All
              keyevents come to window anyway, so we would get the same
              keys in that way as well, but we wouldn't need to struggle
              with grabs (modal dialogs etc). */

    /* Menu key handling is done here */
    if ( HILDON_KEYEVENT_IS_MENU_KEY (keyevent) ) {
	    HildonAppView *appview;
	    HildonAppPrivate *priv;
            GtkWidget *toplevel;
            GtkWidget *focus = NULL;

      /* Don't act on modal dialogs */
      toplevel = gtk_widget_get_toplevel (widget);
      focus = gtk_window_get_focus(GTK_WINDOW(app));

      /* Don't act when comboboxes are active, if a togglebutton
         (combobox) has the focus and it is active, we deactivate the
         combobox and do nothing */ 
      if (GTK_IS_TOGGLE_BUTTON (focus))
        {
          GtkWidget *parent = gtk_widget_get_parent (focus);
          
          if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (focus)) && 
              GTK_IS_COMBO_BOX (parent)) 
            {
              gtk_combo_box_popdown (GTK_COMBO_BOX (parent));
              return TRUE;
            }
        }

      /* Don't act when a GtkWindow of a combobox is selected, this
         can happen in some applications that change the properties of
         the widget focus attribute, WARNING: we are using the name of
         the hildon combobox widget to identify the window
         (gtkcombobox.c, HILDON_COMBO_BOX_POPUP), if it changes we
         must change this name */
      if (GTK_IS_WINDOW (widget) && 
          !g_ascii_strcasecmp("hildon-combobox-window", gtk_widget_get_name (widget)))
        {
          return TRUE;
        }

      if (GTK_IS_DIALOG (toplevel)
          && gtk_window_get_modal (GTK_WINDOW (toplevel)))
        {
          return TRUE;
        }
      		    
	    appview = HILDON_APPVIEW (GTK_BIN(app)->child);
	    priv = HILDON_APP_GET_PRIVATE(app);

	    if ( keyevent->type == GDK_KEY_PRESS ) {
            /* Toggle menu on press, avoid key repeat */
		    if ( priv->lastmenuclick == 0 ){
			    priv->lastmenuclick = 1;
			    if (_hildon_appview_toggle_menu(appview,
					    gtk_get_current_event_time()))
                {
                    return TRUE;
                }
		    }
	    } else if ( keyevent->type == GDK_KEY_RELEASE ) {
            /* We got release, so next press is really a new press, 
               not a repeat */
		    if ( priv->lastmenuclick == 1 ) {
			    priv->lastmenuclick = 0;
		    }
		    
	    } else {
		    /* Unknown key event */
		    return FALSE;
	    }

            /* don't stop the key event so that it reaches GTK where it
               closes all existing menus that might be open */
	    return FALSE;
    }

    return FALSE;
}

/*
 * Returns the message_type of the Atom registered with a certain name.
 */
static int
xclient_message_type_check(XClientMessageEvent *cm, const gchar *name)
{
  return cm->message_type == XInternAtom(GDK_DISPLAY(), name, FALSE);
}

/*
 * Returns the GtkWidget associated to a certain Window.
 */
static GtkWidget *
hildon_app_xwindow_lookup_widget(Window xwindow)
{
  GdkWindow *window;
  gpointer widget;

  window = gdk_xid_table_lookup(xwindow);
  if (window == NULL)
    return NULL;

  gdk_window_get_user_data(window, &widget);
  return widget;
}

/*
 * Let's search a actual main window using tranciency hints. 
 * Note that there can be several levels of menus/dialogs above
 * the actual main window.
 */
static Window get_active_main_window(Window window)
{
  Window parent_window;
  gint limit = 0;

  gdk_error_trap_push ();

  while (XGetTransientForHint(GDK_DISPLAY(), window, &parent_window))
  {
        /* The limit > TRANSIENCY_MAXITER ensures that we can't be stuck
           here forever if we have circular transiencies for some reason.
           Use of _MB_CURRENT_APP_WINDOW might be more elegant... */

    if (!parent_window || parent_window == GDK_ROOT_WINDOW() ||
	parent_window == window || limit > TRANSIENCY_MAXITER)
      {
	break;
      }

    limit++;
    window = parent_window;
  }
  
  gdk_flush ();

  if (gdk_error_trap_pop ())
    return 0;

  return window;
}

/*
 * Filters every GDK event first.
 */
static GdkFilterReturn
hildon_app_event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
    gint x,y;
    HildonApp *app = data;
    HildonAppPrivate *priv;
    HildonAppView *appview = NULL;

    XAnyEvent *eventti = xevent;

    if (HILDON_IS_APPVIEW (GTK_BIN (app)->child))
      {
        appview = HILDON_APPVIEW (GTK_BIN (app)->child);
      }

    g_return_val_if_fail (app, GDK_FILTER_CONTINUE);
    g_return_val_if_fail (HILDON_IS_APP(app), GDK_FILTER_CONTINUE);

    priv = HILDON_APP_GET_PRIVATE(app);
    if (eventti->type == ClientMessage)
      {
        XClientMessageEvent *cm = xevent;

        /* Check if a message indicating a click on titlebar has been
           received. Don't open it if mouse is grabbed (eg. modal dialog
           was just opened).
           _MB_GRAB_TRANSFER is emitted by MatchBox, and signals that a button
           has just been released. */
	if (xclient_message_type_check(cm, "_MB_GRAB_TRANSFER") &&
            HILDON_IS_APPVIEW(appview) &&
            gtk_grab_get_current() == NULL &&
	    !_hildon_appview_menu_visible(appview))
        {
          _hildon_appview_toggle_menu(appview, cm->data.l[0]);
          return GDK_FILTER_REMOVE;
        }
        /* IM_CLOSE is input method specific hack that is really questionable */
        else if (xclient_message_type_check(cm, "_HILDON_IM_CLOSE"))
        {
          g_signal_emit_by_name(app, "im_close", NULL);
	  return GDK_FILTER_REMOVE;
        }
        /* Task user changed the view through task navigator? */
        else if (xclient_message_type_check(cm, "_NET_ACTIVE_WINDOW"))
	  {
	    unsigned long view_id = cm->window;
            gpointer view_ptr = find_view(app, view_id);
						
            /* When getting a _NET_ACTIVE_WINDOW signal from the WM we need
             * to bring the application to the front */
            if (!priv->is_topmost)
              g_signal_emit_by_name (G_OBJECT(app), "topmost_status_acquire");

            if (HILDON_IS_APPVIEW(view_ptr))
              /* Sets the current view to the "window" that the _NET_ACTIVE_WINDOW
               * specified */
              hildon_app_set_appview(app, (HILDON_APPVIEW(view_ptr)));
            else
              /* there was no view, so we have to switch to an actual application */
              g_signal_emit_by_name (G_OBJECT(app), "switch_to", view_ptr);

            /* FIXME: This is a hack. This was once just gtk_window_present, but
                      was changed into this at some day!! */
            if (GTK_WIDGET(app)->window)
            {
              mb_util_window_activate(GDK_DISPLAY(),
                                      GDK_WINDOW_XID(GTK_WIDGET(app)->window));
            }
	  }
        /* FIXME: IM hack */
        else if (xclient_message_type_check(cm, "_HILDON_IM_CLIPBOARD_COPY"))
        {
	  Window xwindow = cm->data.l[0];
	  GtkWidget *widget = hildon_app_xwindow_lookup_widget(xwindow);

	  g_signal_emit_by_name (G_OBJECT(app), "clipboard_copy", widget);
	}
        /* FIXME: IM hack */
        else if (xclient_message_type_check(cm, "_HILDON_IM_CLIPBOARD_CUT"))
        {
	  Window xwindow = cm->data.l[0];
	  GtkWidget *widget = hildon_app_xwindow_lookup_widget(xwindow);

	  g_signal_emit_by_name (G_OBJECT(app), "clipboard_cut", widget);
        }
        /* FIXME: IM hack */
        else if (xclient_message_type_check(cm, "_HILDON_IM_CLIPBOARD_PASTE"))
        {
	  Window xwindow = cm->data.l[0];
	  GtkWidget *widget = hildon_app_xwindow_lookup_widget(xwindow);

	  g_signal_emit_by_name (G_OBJECT(app), "clipboard_paste", widget);
        }
      }
    
     if (eventti->type == ButtonPress)
       {

     /* FIXME: This is mysterious bugfix related to problems to open the
        application menu (bugzilla N#3204) */
	 XButtonEvent *bev = (XButtonEvent *)xevent;

	 if (HILDON_IS_APPVIEW(appview) &&
	     _hildon_appview_menu_visible(appview) &&
             !hildon_appview_get_fullscreen(appview))
          {
	    x = bev->x_root;
	    y = bev->y_root;
	    if ( (x >= MENUAREA_LEFT_LIMIT) && (x <= MENUAREA_RIGHT_LIMIT) &&
		 (y >= MENUAREA_TOP_LIMIT) && (y <= MENUAREA_BOTTOM_LIMIT))
	      {
                _hildon_appview_toggle_menu(appview, bev->time);
		return GDK_FILTER_CONTINUE;
	      }
	  }
       }
    
    /* FIXME: as above */
    if (eventti->type == ButtonRelease)
      {
        if (HILDON_IS_APPVIEW(appview) &&
            _hildon_appview_menu_visible(appview) &&
            !hildon_appview_get_fullscreen(appview))
          {
	    XButtonEvent *bev = (XButtonEvent *)xevent;
	    x = bev->x_root;
	    y = bev->y_root;
	    if ( (x >= MENUAREA_LEFT_LIMIT) && (x < MENUAREA_RIGHT_LIMIT) &&
		 (y >= MENUAREA_TOP_LIMIT) && (y <= MENUAREA_BOTTOM_LIMIT))
	      {
		return GDK_FILTER_REMOVE;
	      }
	  }
	return GDK_FILTER_CONTINUE;
      }

    /* Application stacking order changed */
    if (eventti->type == PropertyNotify)
      {
        Atom active_app_atom =
            XInternAtom (GDK_DISPLAY(), "_MB_CURRENT_APP_WINDOW", False);
        XPropertyEvent *prop = xevent;

        if ((prop->atom == active_app_atom)
            && (prop->window == GDK_ROOT_WINDOW()))
          {
            Atom realtype;
            int format;
            int status;
            unsigned long n;
            unsigned long extra;
            Window my_window;
            union
            {
                Window *win;
                unsigned char *char_pointer;
            } win;

            win.win = NULL;

            status = XGetWindowProperty(GDK_DISPLAY(), GDK_ROOT_WINDOW(),
                                        active_app_atom, 0L, 16L,
                                        0, XA_WINDOW, &realtype, &format,
                                        &n, &extra, &win.char_pointer);
            if (!(status == Success && realtype == XA_WINDOW && format == 32
                 && n == 1 && win.win != NULL))
              {
                if (win.win != NULL)
                    XFree(win.char_pointer);
                return GDK_FILTER_CONTINUE;
              }

            my_window = GDK_WINDOW_XID(GTK_WIDGET(app)->window);

            /* Are we the topmost one? */
            if (win.win[0] == my_window || 
                get_active_main_window(win.win[0]) == my_window)
              {
                if (!priv->is_topmost)
                    g_signal_emit_by_name (G_OBJECT(app),
                                          "topmost_status_acquire");
              }
            else if (priv->is_topmost)
            {
              GtkWidget *focus = gtk_window_get_focus(GTK_WINDOW(app));

              /* FIXME: IM hack, IM-module should do this in response to
                        topmost_status_lose (emission hook?) */
              if (GTK_IS_ENTRY(focus))
                gtk_im_context_focus_out(GTK_ENTRY(focus)->im_context);
              if (GTK_IS_TEXT_VIEW(focus))
                gtk_im_context_focus_out(GTK_TEXT_VIEW(focus)->im_context);

              g_signal_emit_by_name (app, "topmost_status_lose");
            }

            if (win.win != NULL)
                XFree(win.char_pointer);
          }
      }

    return GDK_FILTER_CONTINUE;
      }

/*
 * Sets the GTK Window title to the application's title, or 
 * combined appview/app title, if two part title is asked.
 */
static void
hildon_app_construct_title (HildonApp *self)
{
  g_return_if_fail (HILDON_IS_APP (self));

  if (GTK_WIDGET_REALIZED(self))
  {
    HildonAppPrivate *priv;
    GdkAtom subname;
    gchar *concatenated_title = NULL;
    HildonAppView *appview;

    priv = HILDON_APP_GET_PRIVATE (self);
    appview = hildon_app_get_appview(self);
    
    /* FIXME: The subname property is legacy stuff no longer supported by
       Matchbox. However, it is still set for the convenience of
       the Task Navigator. */
    subname = gdk_atom_intern("_MB_WIN_SUB_NAME", FALSE);

    if (!appview || !hildon_app_get_two_part_title(self) ||
        g_utf8_strlen(hildon_appview_get_title(appview), -1) < 1 )
      {
        /* Set an invisible dummy value if there is no appview title */
        gdk_property_change (GTK_WIDGET(self)->window, subname,
                             gdk_atom_intern ("UTF8_STRING", FALSE),
                             8, GDK_PROP_MODE_REPLACE, (guchar *) " \0", 1);
        gtk_window_set_title (GTK_WINDOW(self), priv->title);
      }
    else
      {
        gdk_property_change (GTK_WIDGET(self)->window, subname,
                            gdk_atom_intern ("UTF8_STRING", FALSE),
                            8, GDK_PROP_MODE_REPLACE,
                            (guchar *)hildon_appview_get_title(appview),
                            strlen(hildon_appview_get_title (appview)));
        concatenated_title = g_strjoin(TITLE_DELIMITER, priv->title,
            hildon_appview_get_title(appview), NULL);
        /* priv->title should always be non-null, but check anyway */
      	if (concatenated_title != NULL)
        {
          gtk_window_set_title (GTK_WINDOW(self), concatenated_title);
          g_free(concatenated_title);
        }
      }
  }    
}

/*
 * Callback function to the topmost_status_acquire signal emitted by
 * hildon_app_event_filter function. See it for more details.
 */
void
hildon_app_real_topmost_status_acquire (HildonApp *self)
{
  HildonAppPrivate *priv;
  g_return_if_fail (HILDON_IS_APP (self));
  priv = HILDON_APP_GET_PRIVATE (self);

  /* FIXME: What is the logic not to update topmost status now? */
  if (!GTK_BIN (self)->child)
    return;

  priv->is_topmost = TRUE;
}

/*
 * Callback function to the topmost_status_lose signal emitted by
 * hildon_app_event_filter function. See it for more details.
 */
void
hildon_app_real_topmost_status_lose (HildonApp *self)
{
  HildonAppPrivate *priv;
  g_return_if_fail (HILDON_IS_APP (self));
  priv = HILDON_APP_GET_PRIVATE (self);

  /* FIXME: What is the logic not to update topmost status now? */
  if (!GTK_BIN (self)->child)
    return;

  priv->is_topmost = FALSE;
}

void
hildon_app_real_switch_to (HildonApp *self)
{
  g_return_if_fail (HILDON_IS_APP (self));
  /* Do we have to do anything here? */
}


/**
 * hildon_app_set_autoregistration
 * @self : a #HildonApp
 * @auto_reg : whether the (app)view autoregistration should be active
 *
 * Controls the autoregistration/unregistration of (app)views.
 */

void hildon_app_set_autoregistration(HildonApp *self, gboolean auto_reg)
{
  HildonAppPrivate *priv;
  g_return_if_fail (HILDON_IS_APP (self));

  priv = HILDON_APP_GET_PRIVATE (self);
  priv->autoregistration = auto_reg;
}


/**
 * hildon_app_register_view:
 * @self : a #HildonApp
 * @view_ptr : pointer to the view instance to be registered
 *
 * Registers a new view. For appviews, this can be done automatically
 * if autoregistration is set.
 */

void hildon_app_register_view(HildonApp *self, gpointer view_ptr)
{
  HildonAppPrivate *priv;
  view_item *view_item_inst;

  g_return_if_fail (HILDON_IS_APP (self) || view_ptr != NULL);

  priv = HILDON_APP_GET_PRIVATE (self);  

  if (hildon_app_find_view_id(self, view_ptr) == 0)
  {
    /* The pointer to the view was unique, so add it to the list */
    view_item_inst = g_malloc(sizeof(view_item));
    view_item_inst->view_id = priv->view_id_counter;
    view_item_inst->view_ptr = view_ptr;

    priv->view_id_counter++;

    priv->view_ids = 
      g_slist_append(priv->view_ids, view_item_inst);

    /* Update the list of views */
    if (GTK_WIDGET_REALIZED(self))
      hildon_app_apply_client_list(self);
  }
}


/**
 * hildon_app_register_view_with_id:
 * @self : a #HildonApp
 * @view_ptr : pointer to the view instance to be registered
 * @view_id : the ID of the view
 * 
 * Registers a new view. Allows the application to specify any ID.
 * 
 * Returns: TRUE if the view registration succeeded, FALSE otherwise.
 *          The probable cause of failure is that view with that ID
 *          already existed.
 */

gboolean hildon_app_register_view_with_id(HildonApp *self,
					  gpointer view_ptr,
					  unsigned long view_id)
{
  view_item *view_item_inst;  
  HildonAppPrivate *priv;
  GSList *list_ptr = NULL;

  g_return_val_if_fail (HILDON_IS_APP (self), FALSE);
  g_return_val_if_fail (view_ptr, FALSE);

  priv = HILDON_APP_GET_PRIVATE (self);
  
  list_ptr = priv->view_ids;

  /* Check that the view is not already registered */
  while (list_ptr)
    {
      if ( (gpointer)((view_item *)list_ptr->data)->view_ptr == view_ptr
	&& (unsigned long)((view_item *)list_ptr->data)->view_id == view_id)
	{
	  return FALSE;
	}
      list_ptr = list_ptr->next;
    }

   /* The pointer to the view was unique, so add it to the list */
  view_item_inst = g_malloc(sizeof(view_item));
  view_item_inst->view_id = view_id;
  view_item_inst->view_ptr = view_ptr;

  priv->view_ids = 
    g_slist_append(priv->view_ids, view_item_inst);

  priv->view_id_counter++;

  /* Finally, update the _NET_CLIENT_LIST property */
  if (GTK_WIDGET_REALIZED(self))
    hildon_app_apply_client_list(self);

  return TRUE;
}

/**
 * hildon_app_unregister_view:
 * @self : a #HildonApp
 * @view_ptr : pointer to the view instance to be unregistered
 *
 * Unregisters a view from HildonApp. Done usually when a view is
 * destroyed. For appviews, this is can be automatically
 * if autoregistration is set.
 */
void hildon_app_unregister_view(HildonApp *self, gpointer view_ptr)
{
  HildonAppPrivate *priv = NULL;
  GSList *list_ptr = NULL;

  g_return_if_fail (HILDON_IS_APP (self));
  g_return_if_fail (view_ptr != NULL);
  
  priv = HILDON_APP_GET_PRIVATE (self);
  
  /* Search the view from the list */
  list_ptr = priv->view_ids;
  
  while (list_ptr)
    { 
      if ( (gpointer)((view_item *)list_ptr->data)->view_ptr == view_ptr)
	      {
            /* Found the view, kick it off */
            g_free (list_ptr->data);
	          priv->view_ids = g_slist_delete_link(priv->view_ids, list_ptr);
	          break;
	       }
      list_ptr = list_ptr->next;
    }
  
  if (GTK_WIDGET_REALIZED(self))
    hildon_app_apply_client_list(self);
}


/**
 * hildon_app_unregister_view_with_id:
 * @self: a #HildonApp
 * @view_id: the ID of the view that should be unregistered
 * 
 * Unregisters a view with specified ID, if it exists.
 */
void hildon_app_unregister_view_with_id(HildonApp *self,
					unsigned long view_id)
{
  HildonAppPrivate *priv;
  GSList *list_ptr = NULL;
  
  g_return_if_fail (HILDON_IS_APP (self));
  
  priv = HILDON_APP_GET_PRIVATE (self);
  
  /* Search the view from the list */
  list_ptr = priv->view_ids;
  
  while (list_ptr)
    { 
      if ( (unsigned long)((view_item *)list_ptr->data)->view_id == view_id)
	{
      /* Found view with given id, kick it off */
	  g_free (list_ptr->data);
	  priv->view_ids = g_slist_delete_link(priv->view_ids, list_ptr);
	  break;
	}
      list_ptr = list_ptr->next;
    }

  /* Update client list to reflect new situation. If we are not
     realized, then nobody knows about us anyway... */
  if (GTK_WIDGET_REALIZED(self))
    hildon_app_apply_client_list(self);  
}


/**
 * hildon_app_notify_view_changed:
 * @self : a #HildonApp
 * @view_ptr : pointer to the view that is switched to
 * 
 * Updates the X property that contains the currently active view
 */
void hildon_app_notify_view_changed(HildonApp *self, gpointer view_ptr)
{
  g_return_if_fail (HILDON_IS_APP (self));
  g_return_if_fail (view_ptr != NULL);

  /* We need GdkWindow before we can send X messages */ 
  if (GTK_WIDGET_REALIZED(self))
  {
    gulong id = hildon_app_find_view_id(self, view_ptr);
    Atom active_view = XInternAtom (GDK_DISPLAY(),
				 "_NET_ACTIVE_WINDOW", False);

    if (id) {
      /* Set _NET_ACTIVE_WINDOW for our own toplevel to contain view id */
      XChangeProperty(GDK_DISPLAY(), GDK_WINDOW_XID(GTK_WIDGET(self)->window),
		      active_view, XA_WINDOW, 32, PropModeReplace,
		      (unsigned char *)&id, 1);
    XFlush(GDK_DISPLAY());
    }
  }
}


/**
 * hildon_app_find_view_id:
 * @self : a #HildonApp
 * @view_ptr : pointer to the view whose ID we want to acquire
 * 
 * Returns: the ID of the view, or 0 if not found
 *
 * Allows mapping of view pointer to its view ID. If NULL is passed
 * as the view pointer, returns the ID of the current view.
 */
unsigned long hildon_app_find_view_id(HildonApp *self, gpointer view_ptr)
{
  HildonAppPrivate *priv;
  GSList *iter;

  priv = HILDON_APP_GET_PRIVATE (self);

  /* If no view is given, find the ID for the currently visible view */
  if (!view_ptr)
    view_ptr = GTK_BIN (self)->child;
  if (!view_ptr)
    return 0;

  /* Iterate through list and search for given view pointer */
  for (iter = priv->view_ids; iter; iter = iter->next)
  {
    if ( (gpointer)((view_item *)iter->data)->view_ptr == view_ptr)
  	  return (unsigned long)((view_item *)iter->data)->view_id;
  }

  return 0;
}

/**
 * hildon_app_set_killable:
 * @self : a #HildonApp
 * @killability : truth value indicating whether the app can be killed
 *                       
 * Updates information about whether the application can be killed or not by
 * Task Navigator (i.e. whether its statesave is up to date)
 */
void hildon_app_set_killable(HildonApp *self, gboolean killability)
{
  HildonAppPrivate *priv = HILDON_APP_GET_PRIVATE (self);
  g_return_if_fail (HILDON_IS_APP (self) );

  if (killability != priv->killable)
  {
    priv->killable = killability;

    /* If we have a window, then we can actually set this 
       property. Otherwise we wait until we are realized */
    if (GTK_WIDGET_REALIZED(self))
      hildon_app_apply_killable(self);
  }
}


/**
 * hildon_app_set_ui_manager:
 * @self : #HildonApp
 * @uim : #GtkUIManager to be set
 * 
 * Sets the #GtkUIManager assigned to the #HildonApp.
 * If @uim is NULL, unsets the current ui manager.
 * The @HildonApp holds a reference to the ui manager until
 * the @HildonApp is destroyed or unset.
 */
void hildon_app_set_ui_manager(HildonApp *self, GtkUIManager *uim)
{
    HildonAppPrivate *priv;

    g_return_if_fail(self && HILDON_IS_APP(self));
    
    priv = HILDON_APP_GET_PRIVATE (self);

    /* Release old ui-manager object if such exists */    
    if (priv->uim != NULL)
      {
        g_object_unref (G_OBJECT (priv->uim));
      }
    
    priv->uim = uim;

    /* If we got new ui-manager (it's perfectly valid not 
       to give one), acquire reference to it */
    if (priv->uim != NULL)
      {
        g_object_ref (G_OBJECT (uim));
      }

    g_object_notify (G_OBJECT(self), "ui-manager");
}

/**
 * hildon_app_get_ui_manager:
 * @self : #HildonApp
 * 
 * Gets the #GtkUIManager assigned to the #HildonApp.
 *
 * Returns: the #GtkUIManager assigned to this application
 * or null if no manager is assigned
 */
GtkUIManager *hildon_app_get_ui_manager(HildonApp *self)
{
    HildonAppPrivate *priv;
    
    g_return_val_if_fail(self && HILDON_IS_APP(self), NULL);

    priv = HILDON_APP_GET_PRIVATE (self);

    return (priv->uim);
}

/*
 * Search for a view with the given id within HildonApp.
 * Returns a pointer to the found view, or NULL if not found.
 */
static gpointer find_view(HildonApp *self, unsigned long view_id)
{
  HildonAppPrivate *priv;
  GSList *iter;
  
  priv = HILDON_APP_GET_PRIVATE (self);

  /* Iterate through the list of view ids and search given id */
  for (iter = priv->view_ids; iter; iter = iter->next)
  {
    if ( (unsigned long)((view_item *)iter->data)->view_id == view_id)
		  return (gpointer)((view_item *)iter->data)->view_ptr;
  }

  return NULL;
}
