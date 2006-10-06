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
 * SECTION:hildon-appview
 * @short_description: A widget which present one view of an application
 * @see_also: #HildonApp
 *
 * #HildonAppView is a widget which presents one view of an application.
 * Application can have many different views and the appview helps to organize.
 * It has automatic fullscreen and menu handling. It also helps to handle 
 * components like a toolbar.
 */

#include <memory.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include "hildon-app.h"
#include <hildon-appview.h>
#include <hildon-find-toolbar.h>

#include <gtk/gtkadjustment.h>
#include <gtk/gtkmenu.h>
#include <gtk/gtkimcontext.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkmenushell.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkprogressbar.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkiconfactory.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


#include <libintl.h>
#define _(String) gettext(String)

enum {
  PROP_0,
  PROP_CONNECTED_ADJUSTMENT,
  PROP_FULLSCREEN_KEY_ALLOWED,
  PROP_FULLSCREEN,
  PROP_TITLE,
  PROP_MENU_UI
};

/*The size of screen*/
#define WINDOW_HEIGHT           480
#define WINDOW_WIDTH            800

#define NAVIGATOR_HEIGHT        WINDOW_HEIGHT

#define APPVIEW_HEIGHT          396
#define APPVIEW_WIDTH           672

#define TOOLBAR_HEIGHT          40
#define TOOLBAR_UP		9
#define TOOLBAR_DOWN		9
#define TOOLBAR_MIDDLE		10
#define TOOLBAR_RIGHT		24
#define TOOLBAR_LEFT		24
#define TOOLBAR_WIDTH           APPVIEW_WIDTH

#define WORKAREA_ATOM "_NET_WORKAREA"

/* Non atom defines */
#define _NET_WM_STATE_REMOVE    0       /* remove/unset property */
#define _NET_WM_STATE_ADD       1       /* add/set property */

/*Margins
 * These margins are set to be 5pixels smaller than in the specs
 * Inner things are allocation that extra space
 * */
/*
#define MARGIN_TOOLBAR_TOP 2
#define MARGIN_TOOLBAR_BOTTOM 6
#define MARGIN_TOOLBAR_LEFT 22
#define MARGIN_TOOLBAR_RIGHT 23
*/
#define MARGIN_APPVIEW_TOP 0
#define MARGIN_APPVIEW_BOTTOM 24
#define MARGIN_APPVIEW_LEFT 24
#define MARGIN_APPVIEW_RIGHT 24


#define HILDON_APPVIEW_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
     HILDON_TYPE_APPVIEW, HildonAppViewPrivate))

/*Progressbar*/
#define DEFAULT_WIDTH 20
#define DEFAULT_HEIGHT 28
#define BANNER_WIDTH DEFAULT_WIDTH
#define BANNER_HEIGHT DEFAULT_HEIGHT

static GtkBinClass *parent_class;

static void hildon_appview_init(HildonAppView * self);
static void hildon_appview_class_init(HildonAppViewClass * appview_class);

static void hildon_appview_menupopupfunc(GtkMenu *menu, gint *x, gint *y,
                                         gboolean *push_in,
                                         GtkWidget *widget);
static void hildon_appview_menupopupfuncfull(GtkMenu *menu, gint *x, gint *y,
                                             gboolean *push_in,
                                             GtkWidget *widget);
static gboolean hildon_appview_expose(GtkWidget * widget,
                                      GdkEventExpose * event);
static void hildon_appview_forall(GtkContainer * container,
                                  gboolean include_internals,
                                  GtkCallback callback,
                                  gpointer callback_data);
static void hildon_appview_show_all(GtkWidget *widget);

static void hildon_appview_size_allocate(GtkWidget * widget,
                                         GtkAllocation * allocation);
static void hildon_appview_size_request(GtkWidget * widget,
                                        GtkRequisition * requisition);
static void hildon_appview_finalize(GObject * obj_self);
static void hildon_appview_set_property(GObject * object, guint property_id,
                                    const GValue * value, GParamSpec * pspec);
static void hildon_appview_get_property(GObject * object, guint property_id,
                                    GValue * value, GParamSpec * pspec);
static void hildon_appview_destroy(GtkObject *obj);
static void hildon_appview_real_fullscreen_state_change(HildonAppView *
                                                        self,
                                                        gboolean
                                                        fullscreen);
static void hildon_appview_switched_to(HildonAppView * self);
static void get_client_area(GtkWidget * widget,
                            GtkAllocation * allocation);

typedef void (*HildonAppViewSignal) (HildonAppView *, gint, gpointer);

/* signals */
enum {
    TOOLBAR_CHANGED,
    TOOLBAR_TOGGLE_REQUEST,
    FULLSCREEN_STATE_CHANGE,
    TITLE_CHANGE,
    SWITCHED_TO,
    SWITCHED_FROM,
    INCREASE_BUTTON_EVENT,
    DECREASE_BUTTON_EVENT,
    HILDON_APPVIEW_LAST_SIGNAL
};

static guint appview_signals[HILDON_APPVIEW_LAST_SIGNAL] = { 0 };

enum {
    WIN_TYPE = 0,
    WIN_TYPE_MESSAGE,
    MAX_WIN_MESSAGES
};

struct _HildonAppViewPrivate {
    GtkWidget *menu;
    gchar *title;

    GtkAllocation allocation;

    guint fullscreen : 1;
    guint fullscreenshortcutallowed : 1;

    /* For future expansion.
     * We might use the below variables for disabling keyrepeat if we need it someday. */
    guint increase_button_pressed_down : 1;
    guint decrease_button_pressed_down : 1;
    gint visible_toolbars;
    GtkAdjustment * connected_adjustment;

    gchar *menu_ui;
};

/* FIXME: Extremely old Legacy code. I wonder why we need 
          a custom marshaller in the first place. */
static void hildon_appview_signal_marshal(GClosure * closure,
                                          GValue * return_value,
                                          guint n_param_values,
                                          const GValue * param_values,
                                          gpointer invocation_hint,
                                          gpointer marshal_data)
{
    register HildonAppViewSignal callback;
    register GCClosure *cc = (GCClosure *) closure;
    register gpointer data1, data2;

    g_assert(n_param_values == 2);

    if (G_CCLOSURE_SWAP_DATA(closure)) {
        data1 = closure->data;
        data2 = g_value_peek_pointer(param_values + 0);
    } else {
        data1 = g_value_peek_pointer(param_values + 0);
        data2 = closure->data;
    }

    callback =
  /* FIXME: This is a compilation workaround for gcc > 3.3 since glib is buggy */ 
  /* see http://bugzilla.gnome.org/show_bug.cgi?id=310175 */

#ifdef __GNUC__
  __extension__
#endif
        (HildonAppViewSignal) (marshal_data !=
                               NULL ? marshal_data : cc->callback);

    callback((HildonAppView *) data1,
             (gint) g_value_get_int(param_values + 1), data2);
}

GType hildon_appview_get_type(void)
{
    static GType appview_type = 0;

    if (!appview_type) {
        static const GTypeInfo appview_info = {
            sizeof(HildonAppViewClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_appview_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonAppView),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_appview_init,
        };
        appview_type = g_type_register_static(GTK_TYPE_BIN,
                                              "HildonAppView",
                                              &appview_info, 0);
    }
    return appview_type;
}

/*
 * Class initialisation.
 */
static void hildon_appview_class_init(HildonAppViewClass * appview_class)
{
    /* Get convenience variables */
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(appview_class);
    GObjectClass *object_class = G_OBJECT_CLASS(appview_class);
    GtkContainerClass *container_class =
        GTK_CONTAINER_CLASS(appview_class);

    /* Set the global parent_class here */
    parent_class = g_type_class_peek_parent(appview_class);

    object_class->set_property = hildon_appview_set_property;
    object_class->get_property = hildon_appview_get_property;

    /* Set the widgets virtual functions */
    widget_class->size_allocate = hildon_appview_size_allocate;
    widget_class->size_request = hildon_appview_size_request;
    widget_class->expose_event = hildon_appview_expose;
    widget_class->show_all = hildon_appview_show_all;
    /* widget_class->realize = hildon_appview_realize; */
    
    /* now the object stuff */
    object_class->finalize = hildon_appview_finalize;

    /* To the container */
    container_class->forall = hildon_appview_forall;
    
    /* gtkobject stuff*/
    GTK_OBJECT_CLASS(appview_class)->destroy = hildon_appview_destroy; 
    
    /* And own virtual functions */
    appview_class->fullscreen_state_change =
        hildon_appview_real_fullscreen_state_change;
    appview_class->switched_to = hildon_appview_switched_to;

    g_type_class_add_private(appview_class,
                             sizeof(struct _HildonAppViewPrivate));

    /* New signals */
    appview_signals[TOOLBAR_CHANGED] =
        g_signal_new("toolbar-changed",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass, toolbar_changed),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    appview_signals[TOOLBAR_TOGGLE_REQUEST] =
        g_signal_new("toolbar-toggle-request",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass,
                                     toolbar_toggle_request), NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    appview_signals[FULLSCREEN_STATE_CHANGE] =
        g_signal_new("fullscreen_state_change",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass,
                                     fullscreen_state_change), NULL, NULL,
                     hildon_appview_signal_marshal, G_TYPE_NONE, 1,
                     G_TYPE_INT);

    appview_signals[TITLE_CHANGE] =
        g_signal_new("title_change",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass, title_change),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    appview_signals[SWITCHED_TO] =
        g_signal_new("switched_to",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass, switched_to),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    appview_signals[SWITCHED_FROM] =
        g_signal_new("switched_from",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass, switched_from),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

    appview_signals[INCREASE_BUTTON_EVENT] =
        g_signal_new("increase_button_event",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass, increase_button_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1,
		     G_TYPE_UINT);

    appview_signals[DECREASE_BUTTON_EVENT] =
        g_signal_new("decrease_button_event",
                     G_OBJECT_CLASS_TYPE(object_class),
                     G_SIGNAL_RUN_FIRST,
                     G_STRUCT_OFFSET(HildonAppViewClass, decrease_button_event),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__UINT, G_TYPE_NONE, 1,
		     G_TYPE_UINT);

    /* New properties */                     
    g_object_class_install_property(object_class, PROP_CONNECTED_ADJUSTMENT,
        g_param_spec_object("connected-adjustment",
                            "Connected GtkAdjustment",
                            "The GtkAdjustment. The increase and decrease hardware buttons are mapped to this.",
			    GTK_TYPE_ADJUSTMENT,
			    G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_FULLSCREEN_KEY_ALLOWED,
				    g_param_spec_boolean("fullscreen-key-allowed",
							 "Fullscreen key allowed",
							 "Whether the fullscreen key is allowed or not",
							 FALSE,
							 G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_FULLSCREEN,
				    g_param_spec_boolean("fullscreen",
							 "Fullscreen",
							 "Whether the appview should be fullscreen or not",
							 FALSE,
							 G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_TITLE,
				    g_param_spec_string("title",
							"Title",
							"Appview title",
							NULL,
							G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_MENU_UI,
				    g_param_spec_string("menu-ui",
							"Menu UI string",
							"UI string for application view menu",
							NULL,
							G_PARAM_READWRITE));
   widget_class = (GtkWidgetClass*) appview_class;
}

/*
 * Performs the initialisation of the widget.
 */
static void hildon_appview_init(HildonAppView * self)
{
    HildonAppViewPrivate *priv = self->priv =
        HILDON_APPVIEW_GET_PRIVATE(self);

    /* the vbox is used to handle both the view's main body and how many
     * toolbars as the user wants */

    self->vbox = gtk_vbox_new(TRUE, TOOLBAR_MIDDLE);
    /* TOOLBAR_MIDDLE is here properly used, as originally meant. In order to
     * be free to use whatever distance between toolbars, it's crucial to mind
     * that the relevant gtkrc file must contain the following border property
     * for the "toolbar-frame-middle" property:  border = {24, 24, 5, 4}
     */

    gtk_widget_set_parent(self->vbox, GTK_WIDGET(self));
    priv->menu = NULL;
    priv->visible_toolbars = 0;

    priv->title = g_strdup("");

    priv->fullscreen = FALSE;
    priv->fullscreenshortcutallowed = FALSE;
    priv->increase_button_pressed_down = FALSE;
    priv->decrease_button_pressed_down = FALSE;
   
    priv->connected_adjustment = NULL;
}

/*
 * Performs the standard gtk finalize function, freeing allocated
 * memory and propagating the finalization to the parent.
 */
static void hildon_appview_finalize(GObject * obj_self)
{
    HildonAppView *self;
    g_assert(HILDON_APPVIEW(obj_self));
    self = HILDON_APPVIEW(obj_self);

    if (self->priv->menu_ui)
      g_free (self->priv->menu_ui);

    if (self->priv->connected_adjustment != NULL)
      g_object_remove_weak_pointer (G_OBJECT (self->priv->connected_adjustment),
				    (gpointer) &self->priv->connected_adjustment);

    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);

    g_free(self->priv->title);
}

/*
 * An accessor to set private properties of HildonAppView.
 */
static void hildon_appview_set_property(GObject * object, guint property_id,
                                    const GValue * value, GParamSpec * pspec)
{
    HildonAppView *appview = HILDON_APPVIEW (object);

    switch (property_id) {
    case PROP_CONNECTED_ADJUSTMENT:
        hildon_appview_set_connected_adjustment (appview, g_value_get_object (value));
	break;

    case PROP_FULLSCREEN_KEY_ALLOWED:
	hildon_appview_set_fullscreen_key_allowed (appview, g_value_get_boolean (value));
	break;

    case PROP_FULLSCREEN:
	hildon_appview_set_fullscreen (appview, g_value_get_boolean (value));
	break;

    case PROP_TITLE:
	hildon_appview_set_title (appview, g_value_get_string (value));
	break;

    case PROP_MENU_UI:
    	hildon_appview_set_menu_ui (appview, g_value_get_string (value));
    	break;
    
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

/*
 * An accessor to get private properties of HildonAppView.
 */
static void hildon_appview_get_property(GObject * object, guint property_id,
                                    GValue * value, GParamSpec * pspec)
{
    HildonAppViewPrivate *priv = HILDON_APPVIEW_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_CONNECTED_ADJUSTMENT:
        g_value_set_object (value, priv->connected_adjustment);
	break;

    case PROP_FULLSCREEN_KEY_ALLOWED:
	g_value_set_boolean (value, priv->fullscreenshortcutallowed);
	break;

    case PROP_FULLSCREEN:
	g_value_set_boolean (value, priv->fullscreen);
	break;

    case PROP_TITLE:
	g_value_set_string (value, priv->title);
	break;

    case PROP_MENU_UI:
    	g_value_set_string (value, priv->menu_ui);
    	break;
	
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

/*
 * Used when the HildonAppView is exposed, this function gets a GtkBoxChild
 * as first argument, and a pointer to a gint as second argument. If such
 * GtkBoxChild is visible, the function increments the gint. It is used
 * in a loop, to compute the number of visible toolbars.
 */
static void visible_toolbar(gpointer child, gpointer number_of_visible_toolbars) 
{
    if(GTK_WIDGET_VISIBLE(((GtkBoxChild *)child)->widget))
      (*((gint *)number_of_visible_toolbars))++;
}

/*
 * Used in the paint_toolbar function to discover how many toolbars are
 * above the find toolbar. It's called in a loop that iterates through
 * all the children of the GtkVBox of the HildonAppView.
 */
static void find_findtoolbar_index(gpointer child, gpointer number_of_visible_toolbars)
{
    gint *pass_bundle = (gint *)number_of_visible_toolbars;
    
    if(((GtkBoxChild *)child)->widget->allocation.y < pass_bundle[0]
       && GTK_WIDGET_VISIBLE(((GtkBoxChild *)child)->widget))
        pass_bundle[1]++;
}

/*
 * Used in the paint_toolbar function, it's get a GtkBoxChild as first argument
 * and a pointer to a GtkWidget as the second one, which will be addressed to
 * the find toolbar widget, if it is contained in the given GtkBoxChild.
 */
static void find_findtoolbar(gpointer child, gpointer widget)
{
    if(HILDON_IS_FIND_TOOLBAR(((GtkBoxChild *)child)->widget)
       && GTK_WIDGET_VISIBLE(((GtkBoxChild *)child)->widget))
        (*((GtkWidget **)widget)) = ((GtkBoxChild *)child)->widget;
}

/*
 * Paints all the toolbar children of the GtkVBox of the HildonAppView.
 */
static void paint_toolbar(GtkWidget *widget, GtkBox *box, 
		          GdkEventExpose * event, 
			  gboolean fullscreen)
{
    gint toolbar_num = 0; 
    gint ftb_index = 0;
    gint count;
    GtkWidget *findtoolbar = NULL;
    gchar toolbar_mode[40];

    /* Iterate through all the children of the vbox of the HildonAppView.
     * The visible_toolbar function increments toolbar_num if the toolbar
     * is visible. After this loop, toobar_num will contain the number
     * of the visible toolbars. */
    g_list_foreach(box->children, visible_toolbar, 
		   (gpointer) &toolbar_num);
    if(toolbar_num <= 0)
      return;

    /* Loop through all the children of the GtkVBox of the HildonAppView.
     * The find_findtoolbar function will assign a pointer to the find toolbar
     * to "findtoolbar" argument. If the findtoolbar is not found, i.e. it
     * isn't in the GtkVBox, then the "findtoolbar" argument will stay NULL */
    g_list_foreach(box->children, find_findtoolbar, 
		   (gpointer) &findtoolbar);
    if(findtoolbar != NULL){
        gint pass_bundle[2];
        
        /* an array for convient data passing
         * the first member contains the y allocation
         * of the find toolbar, and the second allocation
         * contains the index(how many toolbars are above
         * find toolbar) */
        pass_bundle[0] = findtoolbar->allocation.y;
        pass_bundle[1] = ftb_index;

        /* computes how many toolbars are above the find toolbar, and the
         * value is stored in pass_bundle[1] */
        g_list_foreach(box->children, find_findtoolbar_index,
		       (gpointer) pass_bundle);
        ftb_index = pass_bundle[1];
    }
    /*upper border*/
    sprintf(toolbar_mode, "toolbar%sframe-top", 
	    fullscreen ? "-fullscreen-" : "-");
    gtk_paint_box(widget->style, widget->window,
		  GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		  &event->area, widget, toolbar_mode,
		  widget->allocation.x,
		  GTK_WIDGET(box)->allocation.y -TOOLBAR_UP,
		  widget->allocation.width, TOOLBAR_UP);
    
    /*top most toolbar painting*/
    if(findtoolbar != NULL && ftb_index == 0 )
    {
        sprintf(toolbar_mode, "findtoolbar%s", 
	        fullscreen ? "-fullscreen" : "");
        
        gtk_paint_box(widget->style, widget->window,
	    	      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		      &event->area, widget, toolbar_mode,
		      widget->allocation.x,
		      GTK_WIDGET(box)->allocation.y,
		      widget->allocation.width,
		      TOOLBAR_HEIGHT);
    }else{
        sprintf(toolbar_mode, "toolbar%s", 
	        fullscreen ? "-fullscreen" : "");
	gtk_paint_box(widget->style, widget->window,
	    	      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		      &event->area, widget, toolbar_mode,
		      widget->allocation.x,
		      GTK_WIDGET(box)->allocation.y,
		      widget->allocation.width,
		      TOOLBAR_HEIGHT);
    }
    /*multi toolbar painting*/
    for(count = 0; count < toolbar_num - 1; count++)
    {
        sprintf(toolbar_mode, "toolbar%sframe-middle", 
	        fullscreen ? "-fullscreen-" : "-");
        
        gtk_paint_box(widget->style, widget->window,
		  GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		  &event->area, widget, toolbar_mode,
		  widget->allocation.x,
		  GTK_WIDGET(box)->allocation.y + 
		  (1 + count) * TOOLBAR_HEIGHT + 
		  count * TOOLBAR_MIDDLE,
		  widget->allocation.width,
		  TOOLBAR_MIDDLE);

	if(findtoolbar != NULL && count + 1 == ftb_index){
            sprintf(toolbar_mode, "findtoolbar%s", 
	            fullscreen ? "-fullscreen" : "");
	    
	    gtk_paint_box(widget->style, widget->window,
		      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		      &event->area, widget, toolbar_mode,
		      widget->allocation.x,
		      GTK_WIDGET(box)->allocation.y + 
		      (1 + count) * (TOOLBAR_HEIGHT + TOOLBAR_MIDDLE),
		      widget->allocation.width,
		      TOOLBAR_HEIGHT);
	}else{
            sprintf(toolbar_mode, "toolbar%s", 
	            fullscreen ? "-fullscreen" : "");
	    
	    gtk_paint_box(widget->style, widget->window,
		      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		      &event->area, widget, toolbar_mode,
		      widget->allocation.x,
		      GTK_WIDGET(box)->allocation.y + 
		      (1 + count) * (TOOLBAR_HEIGHT + TOOLBAR_MIDDLE),
		      widget->allocation.width,
		      TOOLBAR_HEIGHT);
	}
    }
    sprintf(toolbar_mode, "toolbar%sframe-bottom", 
	    fullscreen ? "-fullscreen-" : "-");
    
    gtk_paint_box(widget->style, widget->window,
		  GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
		  &event->area, widget, toolbar_mode,
		  widget->allocation.x,
		  GTK_WIDGET(box)->allocation.y + 
		  GTK_WIDGET(box)->allocation.height,
		  widget->allocation.width, TOOLBAR_DOWN);
}

/*
 * Callback function to an expose event.
 */
static gboolean hildon_appview_expose(GtkWidget * widget,
                                      GdkEventExpose * event)
{
    gint toolbar_num = 0;
    GtkBox *box = GTK_BOX(HILDON_APPVIEW(widget)->vbox);

    if(GTK_WIDGET_VISIBLE(box) && box->children != NULL)
    {
        HildonAppViewPrivate *priv = HILDON_APPVIEW_GET_PRIVATE(widget);

        /* Iterate through all the children of the vbox of the HildonAppView.
         * The visible_toolbar function increments toolbar_num if the toolbar
         * is visible. After this loop, toobar_num will contain the number
         * of the visible toolbars. */
        g_list_foreach(box->children, visible_toolbar, 
                (gpointer) &toolbar_num);

        if( priv->visible_toolbars != toolbar_num)
        {
            /* If the code reaches this block, it means that a toolbar as 
             * been added or removed since last time the view was drawn.
             * Let's then compute the new height of the toolbars areas */
            gint y_pos = 0;
            /* the height difference */
            gint change = (priv->visible_toolbars - toolbar_num) *
                (TOOLBAR_HEIGHT+TOOLBAR_MIDDLE+TOOLBAR_UP);
            if( change < 0 )
                change = TOOLBAR_MIDDLE + TOOLBAR_UP;
            /* the new y-coordinate for the toolbars area */
            y_pos = HILDON_APPVIEW(widget)->vbox->allocation.y - change;

            gtk_widget_queue_draw_area(widget, 0, y_pos, widget->allocation.width,
                    change + HILDON_APPVIEW(widget)->vbox->allocation.height +
                    TOOLBAR_DOWN);
            priv->visible_toolbars = toolbar_num;
        }
    }


    if (HILDON_APPVIEW(widget)->priv->fullscreen)
    {
        if (toolbar_num > 0)
            paint_toolbar(widget, box, event, TRUE);
    }
    else
    {
        gint appview_height_decrement = 0;
        if (toolbar_num > 0)
        {
            appview_height_decrement = toolbar_num * TOOLBAR_HEIGHT +
                (toolbar_num - 1) * TOOLBAR_MIDDLE 
                + TOOLBAR_UP + TOOLBAR_DOWN;

            paint_toolbar(widget, box, event, FALSE);
        }
        else
        {
            appview_height_decrement = MARGIN_APPVIEW_BOTTOM;

            gtk_paint_box(widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, "bottom-border",
                    widget->allocation.x,
                    widget->allocation.y +
                    (widget->allocation.height - MARGIN_APPVIEW_BOTTOM),
                    widget->allocation.width, MARGIN_APPVIEW_BOTTOM);
        }
        gtk_paint_box( widget->style, widget->window,
                GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT, 
                &event->area,
                widget, "left-border", widget->allocation.x,
                widget->allocation.y, MARGIN_APPVIEW_LEFT,
                widget->allocation.height - appview_height_decrement );
        gtk_paint_box( widget->style, widget->window,
                GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT, 
                &event->area,
                widget, "right-border",
                (widget->allocation.x +
                 widget->allocation.width) -
                MARGIN_APPVIEW_RIGHT, widget->allocation.y,
                MARGIN_APPVIEW_RIGHT,
                widget->allocation.height - appview_height_decrement );
    }

    GTK_WIDGET_CLASS(parent_class)->expose_event(widget, event);

    return FALSE;
}

/*
 * Responds to the usual size_request signal.
 */
static void hildon_appview_size_request(GtkWidget * widget,
                                        GtkRequisition * requisition)
{
    HildonAppViewPrivate *priv = HILDON_APPVIEW(widget)->priv;
    GtkWidget *child = GTK_BIN(widget)->child;

    /* forward the size_request to the eventual child of the main container */
    if (child)
        gtk_widget_size_request(child, requisition);

    /* forward the size_request to the eventual vbox (which may contain
     * toolbars) */
    if (HILDON_APPVIEW(widget)->vbox != NULL)
        gtk_widget_size_request(HILDON_APPVIEW(widget)->vbox, requisition);

    /* express the size_request for the view */
    if (priv->fullscreen) {
        requisition->height = WINDOW_HEIGHT;
        requisition->width = WINDOW_WIDTH;
    } else {
        requisition->height = APPVIEW_HEIGHT;
        requisition->width = APPVIEW_WIDTH;
    }
}

/*
 * Computes size and position for the children of the view.
 */
static void hildon_appview_size_allocate(GtkWidget * widget,
                                         GtkAllocation * allocation)
{
    GtkAllocation box_allocation;
    GtkAllocation alloc = *allocation;
    gint border_width = GTK_CONTAINER(widget)->border_width;
    GtkBin *bin = GTK_BIN(widget);
    GtkBox *box = GTK_BOX(HILDON_APPVIEW(widget)->vbox);
    gboolean at_least_one_visible_toolbar = FALSE;

    if(!GTK_IS_WIDGET(bin->child)) return;

    widget->allocation = *allocation;
    
    if (bin->child != NULL && GTK_IS_WIDGET(bin->child)) {
        if (HILDON_APPVIEW(widget)->priv->fullscreen) {
            alloc.x += border_width;
            alloc.y += border_width;
            alloc.width -= (border_width * 2);
            alloc.height -= (border_width * 2);
        } else {
            alloc.x += border_width + MARGIN_APPVIEW_LEFT;
            alloc.y += border_width + MARGIN_APPVIEW_TOP;
            alloc.width -= (border_width * 2) + (MARGIN_APPVIEW_LEFT +
                                      MARGIN_APPVIEW_RIGHT);
            alloc.height -= (border_width * 2) + MARGIN_APPVIEW_TOP;
        }
    }

    if (box->children != NULL) {
        gint length = 0;
        gint box_height = 0;
        /* Iterate through all the children of the vbox of the HildonAppView.
         * The visible_toolbar function increments toolbar_num if the toolbar
         * is visible. After this loop, toobar_num will contain the number
         * of the visible toolbars. */
        g_list_foreach(box->children, visible_toolbar, 
		     (gpointer) &length);
        if(length > 0){
            box_height = length * TOOLBAR_HEIGHT + 
	      (length - 1) * TOOLBAR_MIDDLE;
       
            if(bin->child != NULL) {
                alloc.height = alloc.height - box_height - TOOLBAR_UP
	          - TOOLBAR_DOWN;
	        at_least_one_visible_toolbar = TRUE;
	    }
	
            box_allocation.y = allocation->height - box_height - TOOLBAR_DOWN;
            box_allocation.height = box_height;
            box_allocation.x = allocation->x + TOOLBAR_LEFT;
	    box_allocation.width = allocation->width - TOOLBAR_LEFT - 
	      TOOLBAR_RIGHT;
	    gtk_widget_size_allocate(GTK_WIDGET(box), &box_allocation);
        }
    }
    
    /* The bottom skin graphics is visible only when there are no toolbars */
    if ((HILDON_APPVIEW(widget)->priv->fullscreen == FALSE) &&
	(at_least_one_visible_toolbar == FALSE))
        alloc.height -= MARGIN_APPVIEW_BOTTOM;
    
    gtk_widget_size_allocate(GTK_WIDGET(bin->child), &alloc);
}

/*
 * Overrides gtk_container_forall, calling the callback function for each of
 * the children of HildonAppPrivate.
 */
static void hildon_appview_forall(GtkContainer * container,
                                  gboolean include_internals,
                                  GtkCallback callback,
                                  gpointer callback_data)
{
    HildonAppView *self = HILDON_APPVIEW(container);

    g_assert(callback != NULL);

    GTK_CONTAINER_CLASS(parent_class)->forall(container, include_internals,
                                              callback, callback_data);
    if(include_internals && self->vbox != NULL)
        (* callback)(GTK_WIDGET(self->vbox), callback_data);
}

/**
 * Shows all the widgets in the container.
 */
static void hildon_appview_show_all(GtkWidget *widget)
{
    HildonAppView *self = HILDON_APPVIEW(widget);
    
    /* Toolbar items */
    gtk_widget_show_all(self->vbox);

    /* Parent handless stuff inside appview */
    GTK_WIDGET_CLASS(parent_class)->show_all(widget);
}

/*
 * Frees all the resources and propagates the destroy call to the parent.
 */
static void hildon_appview_destroy(GtkObject *obj)
{
    HildonAppView *self = HILDON_APPVIEW(obj);
    
    if(self->vbox != NULL){
        gtk_widget_unparent(self->vbox);
        self->vbox = NULL;	
    }

    GTK_OBJECT_CLASS(parent_class)->destroy(obj);
}

/*******************/
/*   Signals       */
/*******************/

/*Signal - When is changed to this appview, this is called*/
static void hildon_appview_switched_to(HildonAppView * self)
{
    GtkWidget *parent;

    g_assert(self && HILDON_IS_APPVIEW(self));

    parent = gtk_widget_get_parent(GTK_WIDGET(self));
    hildon_appview_set_fullscreen( self, self->priv->fullscreen );
}

/*Signal - When the fullscreen state is changed, this is called*/
static void hildon_appview_real_fullscreen_state_change(HildonAppView *
                                                        self,
                                                        gboolean
                                                        fullscreen)
{
    HildonAppViewPrivate *priv;
    g_assert(self && HILDON_IS_APPVIEW(self));
    priv = self->priv;

    /* Ensure that state is really changed */
    if( priv->fullscreen == fullscreen )
      return;

    if( fullscreen )
      gtk_window_fullscreen( GTK_WINDOW(
                             gtk_widget_get_parent(GTK_WIDGET(self))) );
    else
      gtk_window_unfullscreen( GTK_WINDOW(
                             gtk_widget_get_parent(GTK_WIDGET(self))) );

    priv->fullscreen = fullscreen;
}

/*******************/
/*     General     */
/*******************/


/*
 * queries a window for the root window coordinates and size of its
 * client area (i.e. minus the title borders etc.)
 */
static void get_client_area(GtkWidget * widget, GtkAllocation * allocation)
{
    GdkWindow *window = widget->window;
    
    if (window)
      gdk_window_get_origin(window, &allocation->x, &allocation->y);
    else
      memset( allocation, 0, sizeof(GtkAllocation) );
}

/*The menu popuping needs a menu popup-function*/
static void hildon_appview_menupopupfunc( GtkMenu *menu, gint *x, gint *y,
                                          gboolean *push_in, GtkWidget *widget )
{
  GtkAllocation client_area = { 0, 0, 0, 0 };

  get_client_area( GTK_WIDGET(widget), &client_area );

  gtk_widget_style_get (GTK_WIDGET (menu), "horizontal-offset", x,
			"vertical-offset", y, NULL);
   
  *x += client_area.x;
  *y += client_area.y;
  
}

/* Similar to above, but used in fullscreen mode */
static void hildon_appview_menupopupfuncfull( GtkMenu *menu, gint *x, gint *y,
                                              gboolean *push_in, 
                                              GtkWidget *widget )
{
  gtk_widget_style_get (GTK_WIDGET (menu), "horizontal-offset", x,
			"vertical-offset", y, NULL);

  *x = MAX (0, *x);
  *y = MAX (0, *y);
}

/*******************/
/*public functions*/
/*******************/


/**
 * hildon_appview_new: 
 * @title:the application view title of the new @HildonAppView
 * 
 * Use this function to create a new application view. The title will 
 * be set only if two-part-title is enabled on the @HildonApp.
 * 
 * Returns: A #HildonAppView.
 **/
GtkWidget *hildon_appview_new(const gchar * title)
{
    HildonAppView *newappview = g_object_new(HILDON_TYPE_APPVIEW, NULL);

    hildon_appview_set_title(newappview, title);
    return GTK_WIDGET(newappview);
}

/**
 * hildon_appview_add_with_scrollbar
 * @self : a @HildonAppView
 * @child : a @GtkWidget
 *
 * Adds the @child to the @self(HildonAppView) and creates a vertical 
 * scrollbar to it. Similar as adding first a #GtkScrolledWindow 
 * and then the @child to it.
 */
void hildon_appview_add_with_scrollbar(HildonAppView * self,
                                       GtkWidget * child)
{
    GtkScrolledWindow *scrolledw;

    g_return_if_fail(HILDON_IS_APPVIEW(self));
    g_return_if_fail(GTK_IS_WIDGET(child));
    g_return_if_fail(child->parent == NULL);

    scrolledw = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_policy(scrolledw, GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type(scrolledw, GTK_SHADOW_NONE);

    /* Check whether child widget supports adjustments */
    if (GTK_WIDGET_GET_CLASS (child)->set_scroll_adjustments_signal)
        gtk_container_add(GTK_CONTAINER(scrolledw), child);
    else
    {
      if( GTK_IS_CONTAINER(child) )
        gtk_container_set_focus_vadjustment( GTK_CONTAINER(child),
                              gtk_scrolled_window_get_vadjustment(scrolledw) );
        gtk_scrolled_window_add_with_viewport(scrolledw, child);
    }

    gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(scrolledw));
}

/**
 * hildon_appview_get_title:
 * @self : a #HildonAppView
 *
 * Gets the title of given #HildonAppView.
 *
 * Returns: the title of the application view
 */
const gchar *hildon_appview_get_title(HildonAppView * self)
{
    g_return_val_if_fail(self && HILDON_IS_APPVIEW(self), "");
    return self->priv->title;
}

/**
 * hildon_appview_set_title:
 * @self : a #HildonAppView
 * @newname : the new title of the application view.
 * 
 * Sets an title of an application view. The title is visible only if
 * two-part-title is enabled on the #HildonApp.
 * 
 */
void hildon_appview_set_title(HildonAppView * self, const gchar * newname)
{
    gchar *oldtitle;

    g_return_if_fail(self && HILDON_IS_APPVIEW(self));
    oldtitle = self->priv->title;

    if (newname != NULL)
        self->priv->title = g_strdup(newname);
    else
        self->priv->title = g_strdup("");

    g_free(oldtitle);
    g_signal_emit_by_name(G_OBJECT(self), "title_change");
}

/**
 * hildon_appview_set_toolbar:
 * @self: a #HildonAppView
 * @toolbar: a #GtkToolbar
 *
 * Sets the #GtkToolbar to given #HildonAppView. This is, however, not a
 * recommended way to set your toolbars. When you have multi toolbars, calling
 * this function more than once will just replace the bottom most toolbar.
 * There is a #GtkVBox in #HildonAppView's public structure, the programmer
 * is responsible to pack his toolbars in the #GtkVBox, and #HildonAppView will
 * take care of putting them at the right place.
 * 
 */
#ifndef HILDON_DISABLE_DEPRECATED
void hildon_appview_set_toolbar(HildonAppView * self, GtkToolbar * toolbar)
{
    GtkBox *box = GTK_BOX(HILDON_APPVIEW(self)->vbox);
    g_return_if_fail(self && HILDON_IS_APPVIEW(self));
    if(toolbar != NULL)/*for failure checking*/
        g_return_if_fail(GTK_IS_TOOLBAR(toolbar));
    
    /*if it is NULL, it unsets the last one, 
     * if it is not null, it unsets the last one anyway*/
    if(box->children != NULL){
        GtkWidget *last_widget;
	
        last_widget = ((GtkBoxChild *)g_list_last
		       (box->children)->data)->widget;
        gtk_container_remove(GTK_CONTAINER(box), 
			     last_widget);
    }
    
    gtk_box_pack_end(box, GTK_WIDGET(toolbar), TRUE, TRUE, 0);
    gtk_widget_queue_resize(GTK_WIDGET(self));
    /*deprecated signal*/
    g_signal_emit_by_name(G_OBJECT(self), "toolbar-changed");
}
#endif
/**
 * hildon_appview_get_toolbar:
 * @self: a #HildonAppView
 *
 * This function will only 
 * return the last widget that has been packed into the #GtkVBox in the public
 * structure. Note this does not, however, mean that it is the bottom most
 * toolbar.
 * 
 * Return value: the #GtkToolbar assigned to this application view. 
 **/
#ifndef HILDON_DISABLE_DEPRECATED
GtkToolbar *hildon_appview_get_toolbar(HildonAppView * self)
{
    GtkBox *box = GTK_BOX(HILDON_APPVIEW(self)->vbox);
    g_return_val_if_fail(self != NULL && HILDON_IS_APPVIEW(self), FALSE);
    if(box != NULL && box->children != NULL)
      return GTK_TOOLBAR(((GtkBoxChild*)
			  g_list_last(box->children)->data)->widget);
    else
      return NULL;
}
#endif
/**
 * hildon_appview_set_fullscreen:
 * @self: a #HildonAppView
 * @fullscreen: the new state of fullscreen mode. TRUE means fullscreen
 * will be set. FALSE the opposite.
 * 
 * Set the fullscreen state of given #HildonAppView class.
 */
void hildon_appview_set_fullscreen(HildonAppView * self,
                                   gboolean fullscreen)
{
    g_return_if_fail(self && HILDON_IS_APPVIEW(self));
    g_signal_emit_by_name(G_OBJECT(self), "fullscreen_state_change",
                          fullscreen);
}

/**
 * hildon_appview_get_fullscreen:
 * @self: a #HildonAppView
 *
 * Gets the current state of fullscreen mode.
 * 
 * Returns: the current state of fullscreen mode
 */
gboolean hildon_appview_get_fullscreen(HildonAppView * self)
{
    g_return_val_if_fail(self && HILDON_IS_APPVIEW(self), FALSE);
    return self->priv->fullscreen;
}

/**
 * hildon_appview_get_fullscreen_key_allowed:
 * @self: a #HildonAppView
 *
 * Check if fullscreening with a shortcut is allowed for given
 * #HildonAppView.
 * 
 * Returns: wheter it's possible to swith fullscreen on/off with
 *               a shortcut key
 */
gboolean hildon_appview_get_fullscreen_key_allowed(HildonAppView * self)
{
    g_return_val_if_fail(self && HILDON_IS_APPVIEW(self), FALSE);
    return self->priv->fullscreenshortcutallowed;
}

/**
 * hildon_appview_set_fullscreen_key_allowed:
 * @self: a #HildonAppView
 * @allow: wheter it's possible to swith fullscreen on/off with
 *               a shortcut key
 *
 * Sets given #HildonAppView whether to allow toggling fullscreen mode
 * with a shortcut key.
 */
void hildon_appview_set_fullscreen_key_allowed(HildonAppView * self,
                                               gboolean allow)
{
    g_return_if_fail(self && HILDON_IS_APPVIEW(self));
    self->priv->fullscreenshortcutallowed = allow;
}

/**
 * hildon_appview_get_menu:
 * @self : #HildonAppView
 * 
 * Gets the #GtMenu assigned to the #HildonAppview.
 *
 * Returns: the #GtkMenu assigned to this application view
 */
GtkMenu *hildon_appview_get_menu(HildonAppView * self)
{
    g_return_val_if_fail(self && HILDON_IS_APPVIEW(self), NULL);

    if (self->priv->menu == NULL) {
        /* Create hildonlike menu */
        
        GtkUIManager *uim;
        GtkWidget *parent = gtk_widget_get_parent (GTK_WIDGET (self));

        /* Try to get appview menu from ui manager */        
        if (parent && HILDON_IS_APP (parent))
          {
            uim = hildon_app_get_ui_manager (HILDON_APP (parent));
            if (uim)
              {
                self->priv->menu =
                  gtk_ui_manager_get_widget (uim, "/HildonApp");
              }
          }
        
        
        if (self->priv->menu == NULL)
          {
            /* Fall back to oldskool menus */
            self->priv->menu = GTK_WIDGET (g_object_new (GTK_TYPE_MENU, NULL));
          }  
          
        gtk_widget_set_name(GTK_WIDGET(self->priv->menu),
                            "menu_force_with_corners");
        gtk_widget_show_all (self->priv->menu);
    }

    return GTK_MENU(self->priv->menu);
}

/**
 * _hildon_appview_toggle_menu:
 * @self : a #HildonAppView
 * @button_event_time :
 *
 * This function should be only called from @HildonApp.
 * Should be renamed to popup menu. Just the first parameter is used.
 * 
 * Returns: Whether or not something was done (whether or not we had
 * a menu)
 */
gboolean _hildon_appview_toggle_menu(HildonAppView * self,
                                 Time button_event_time)
{
    GList *children;

    g_return_val_if_fail(self && HILDON_IS_APPVIEW(self), FALSE);

    if (!self->priv->menu)
        return FALSE;

    if (GTK_WIDGET_VISIBLE(self->priv->menu)) {
        gtk_menu_popdown(GTK_MENU(self->priv->menu));
        gtk_menu_shell_deactivate(GTK_MENU_SHELL(self->priv->menu));
        return TRUE;
    }

    /* Avoid opening an empty menu */
    children = gtk_container_get_children(
                        GTK_CONTAINER(hildon_appview_get_menu(self)));
    if (children != NULL) {
        GtkWidget *menu;

        g_list_free(children);
        menu = GTK_WIDGET(hildon_appview_get_menu(self));
        if (self->priv->fullscreen) {
            gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
                           (GtkMenuPositionFunc)
                           hildon_appview_menupopupfuncfull,
                           self, 0, button_event_time);
        } else {
            gtk_menu_popup(GTK_MENU(menu), NULL, NULL,
                           (GtkMenuPositionFunc)
                           hildon_appview_menupopupfunc,
                           self, 0, button_event_time);
        }
        gtk_menu_shell_select_first(GTK_MENU_SHELL(menu), TRUE);
        return TRUE;
    }
    return FALSE;

}

/**
 * _hildon_appview_menu_visible
 * @self:  a #HildonAppView
 * 
 * Checks whether the titlebar menu is currently visible
 * Returns: TRUE if the menu is visible, FALSE if not
 */

gboolean _hildon_appview_menu_visible(HildonAppView * self)
{
    g_return_val_if_fail (HILDON_IS_APPVIEW (self), FALSE);
    
    return GTK_WIDGET_VISIBLE(GTK_WIDGET(hildon_appview_get_menu(self)));
}

/**
 * hildon_appview_set_connected_adjustment
 * @self :  #HildonAppView
 * @adjustment : a new #GtkAdjustment set to reach to increase
 *   / decrease hardware keys or NULL to unset
 * 
 * Sets a #GtkAdjustment which will change when increase/decrease buttons
 * are pressed.
 */
void hildon_appview_set_connected_adjustment (HildonAppView * self,
					      GtkAdjustment * adjustment)
{
    g_return_if_fail (HILDON_IS_APPVIEW (self));

    /* Disconnect old adjustment */
    if (self->priv->connected_adjustment != NULL)
      g_object_remove_weak_pointer (G_OBJECT (self->priv->connected_adjustment),
				    (gpointer) &self->priv->connected_adjustment);

    /* Start using the new one */
    self->priv->connected_adjustment = adjustment;
    if (self->priv->connected_adjustment != NULL)
        g_object_add_weak_pointer (G_OBJECT (self->priv->connected_adjustment),
				   (gpointer) &self->priv->connected_adjustment);
}

/**
 * hildon_appview_get_connected_adjustment
 * @self : a #HildonAppView
 * 
 * Retrieves the #GtkAdjustment which is connected to this application view
 * and is changed with increase / decrease hardware buttons.
 *
 * Returns: currently connectd #GtkAdjustment assigned to this
 *               application view or NULL if it's not set
 */
GtkAdjustment * hildon_appview_get_connected_adjustment (HildonAppView * self)
{
    g_return_val_if_fail (HILDON_IS_APPVIEW (self), NULL);
   
    return self->priv->connected_adjustment;
}


/**
 * hildon_appview_set_menu_ui
 * @self : a #HildonAppView
 * @ui_string : a #GtkUIManager ui description string
 *
 * Sets the ui description (xml) from which the UIManager creates menus
 * (see @GtkUIManager for details on how to use it)
 */
void hildon_appview_set_menu_ui(HildonAppView *self, const gchar *ui_string)
{
  g_return_if_fail (HILDON_IS_APPVIEW (self));
  
  if (ui_string)
    {
      if (self->priv->menu_ui)
        g_free (self->priv->menu_ui);
      
      self->priv->menu_ui = g_strdup (ui_string);

      /* FIXME: We should update the menu here, preferrably by a
       * hildon_app_ensure_menu_update() which re-installs the menu ui
       * and calls gtk_ui_manager_ensure_update()
       */
    }
  else
    {
      /* Reset the UI */
      if (self->priv->menu_ui)
        {
          g_free (self->priv->menu_ui);
          self->priv->menu_ui = NULL;
        }
    }

    g_object_notify (G_OBJECT(self), "menu-ui");
}

/**
 * hildon_appview_get_menu_ui
 * @self : a #HildonAppView
 *
 * Sets the ui description (xml) from which the UIManager creates menus
 * (see #GtkUIManager for details on how to use it)
 *
 * Returns: currently set ui description
 */
const gchar *hildon_appview_get_menu_ui(HildonAppView *self)
{
  g_return_val_if_fail (HILDON_IS_APPVIEW (self), NULL);

  return (self->priv->menu_ui);

}

/* Called when '+' hardkey is pressed/released */
void _hildon_appview_increase_button_state_changed (HildonAppView * self,
						    guint newkeytype)
{
  self->priv->increase_button_pressed_down = newkeytype;

  /* Transform '+' press into adjustment update (usually scrollbar move) */
  if ((self->priv->connected_adjustment != NULL) && (newkeytype == GDK_KEY_PRESS))
    {
      gfloat clampedvalue = CLAMP (gtk_adjustment_get_value (self->priv->connected_adjustment) + self->priv->connected_adjustment->step_increment,
				   self->priv->connected_adjustment->lower,
				   self->priv->connected_adjustment->upper - self->priv->connected_adjustment->page_size);
      gtk_adjustment_set_value (self->priv->connected_adjustment, clampedvalue);				
    }   

  g_signal_emit (G_OBJECT (self), appview_signals[INCREASE_BUTTON_EVENT], 0, newkeytype);
}

/* Called when '-' hardkey is pressed/released */
void _hildon_appview_decrease_button_state_changed (HildonAppView * self,
						    guint newkeytype)
{
  self->priv->decrease_button_pressed_down = newkeytype;

  /* Transform '-' press into adjustment update (usually scrollbar move) */
  if ((self->priv->connected_adjustment != NULL) && (newkeytype == GDK_KEY_PRESS))
    {
      gfloat clampedvalue = CLAMP (gtk_adjustment_get_value (self->priv->connected_adjustment) - self->priv->connected_adjustment->step_increment,
				   self->priv->connected_adjustment->lower,
				   self->priv->connected_adjustment->upper - self->priv->connected_adjustment->page_size);
      gtk_adjustment_set_value (self->priv->connected_adjustment, clampedvalue);				
    }

  g_signal_emit (G_OBJECT (self), appview_signals[DECREASE_BUTTON_EVENT], 0, newkeytype);
}
