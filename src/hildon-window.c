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


#include <memory.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include "hildon-app.h"
#include <hildon-window.h>
#include "hildon-program.h"
#include "hildon-window-private.h"
#include <hildon-find-toolbar.h>

#include <gtk/gtkmenu.h>
#include <gtk/gtkimcontext.h>
#include <gtk/gtkmenuitem.h>
#include <gtk/gtkcheckmenuitem.h>
#include <gtk/gtkmenushell.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtktextview.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkmain.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>

#include<gtk/gtkprivate.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


#include <libintl.h>
#define _(String) gettext(String)

/*The size of screen*/
#define WINDOW_HEIGHT           480
#define WINDOW_WIDTH            800

#define NAVIGATOR_HEIGHT        WINDOW_HEIGHT

#define APPVIEW_HEIGHT          396
#define APPVIEW_WIDTH           672

#define TOOLBAR_HEIGHT          40
#define TOOLBAR_MIDDLE		10
#define TOOLBAR_WIDTH           APPVIEW_WIDTH

/*FIXME*/
#define CAN_HIBERNATE "CANKILL"
#define CAN_HIBERNATE_LENGTH 7

#define CAN_HIBERNATE_PROPERTY "_HILDON_ABLE_TO_HIBERNATE"

#define TITLE_SEPARATOR " - "


#define HILDON_WINDOW_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
     HILDON_TYPE_WINDOW, HildonWindowPrivate))

static GtkWindowClass *parent_class;

static void
hildon_window_init (HildonWindow * self);

static void
hildon_window_class_init (HildonWindowClass * window_class);

static void
hildon_window_menupopupfunc (GtkMenu *menu, gint *x, gint *y,
                                         gboolean *push_in,
                                         GtkWidget *widget);
static void
hildon_window_menupopupfuncfull (GtkMenu *menu, gint *x, gint *y,
                                             gboolean *push_in,
                                             GtkWidget *widget);
static gboolean
hildon_window_expose (GtkWidget * widget, GdkEventExpose * event);
static void 
hildon_window_forall (GtkContainer * container,
                      gboolean include_internals,
                      GtkCallback callback,
                      gpointer callback_data);
static void
hildon_window_show_all (GtkWidget *widget);

static void
hildon_window_size_allocate (GtkWidget * widget,
                             GtkAllocation * allocation);
static void
hildon_window_size_request (GtkWidget * widget,
                            GtkRequisition * requisition);
static void
hildon_window_finalize (GObject * obj_self);
static void
hildon_window_set_property (GObject * object, guint property_id,
                            const GValue * value, GParamSpec * pspec);
static void
hildon_window_get_property (GObject * object, guint property_id,
                            GValue * value, GParamSpec * pspec);
static void
hildon_window_destroy (GtkObject *obj);
static void
hildon_window_realize (GtkWidget *widget);
static void
hildon_window_unrealize (GtkWidget *widget);
static gboolean
hildon_window_key_press_event (GtkWidget         *widget,
                               GdkEventKey       *event);
static gboolean
hildon_window_key_release_event (GtkWidget       *widget, 
                                 GdkEventKey     *event);
static gboolean
hildon_window_window_state_event (GtkWidget *widget, 
                                  GdkEventWindowState *event);


static void
hildon_window_notify (GObject *gobject, GParamSpec *param);

static void
hildon_window_is_topmost_notify (HildonWindow *window);

static gboolean
hildon_window_toggle_menu (HildonWindow * self);

static gboolean
hildon_window_escape_timeout (gpointer data);

static GdkFilterReturn
hildon_window_event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer data );

static GdkFilterReturn
hildon_window_root_window_event_filter (GdkXEvent *xevent, 
                                        GdkEvent *event, 
                                        gpointer data );

static void
hildon_window_get_borders (HildonWindow *window);

static void
visible_toolbar (gpointer data, gpointer user_data);
static void
paint_toolbar (GtkWidget *widget, GtkBox *box, 
		       GdkEventExpose * event, 
			   gboolean fullscreen);

typedef void (*HildonWindowSignal) (HildonWindow *, gint, gpointer);



enum
{
    PROP_0,
    PROP_IS_TOPMOST
};

enum
{
    WIN_TYPE = 0,
    WIN_TYPE_MESSAGE,
    MAX_WIN_MESSAGES
};

struct _HildonWindowPrivate
{
    GtkWidget *menu;
    GtkWidget *vbox;

    GtkBorder *borders;
    GtkBorder *toolbar_borders;

    GtkAllocation allocation;

    guint fullscreen;
    guint is_topmost;
    guint escape_timeout;
    gint visible_toolbars;
    gint previous_vbox_y;

    HildonProgram *program;
};

GType 
hildon_window_get_type (void)
{
    static GType window_type = 0;

    if (!window_type) {
        static const GTypeInfo window_info = {
            sizeof(HildonWindowClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_window_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonWindow),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_window_init,
        };
        window_type = g_type_register_static(GTK_TYPE_WINDOW,
                                              "HildonWindow",
                                              &window_info, 0);
    }
    return window_type;
}

/* Virtual methods */

static void 
hildon_window_class_init (HildonWindowClass * window_class)
{
    /* Get convenience variables */
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (window_class);
    GObjectClass *object_class = G_OBJECT_CLASS (window_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS (window_class);

    /* Set the global parent_class here */
    parent_class = g_type_class_peek_parent (window_class);

    object_class->set_property = hildon_window_set_property;
    object_class->get_property = hildon_window_get_property;
    object_class->notify = hildon_window_notify;

    /* Set the widgets virtual functions */
    widget_class->size_allocate = hildon_window_size_allocate;
    widget_class->size_request = hildon_window_size_request;
    widget_class->expose_event = hildon_window_expose;
    widget_class->show_all = hildon_window_show_all;
    widget_class->realize = hildon_window_realize;
    widget_class->unrealize = hildon_window_unrealize;
    widget_class->key_press_event = hildon_window_key_press_event;
    widget_class->key_release_event = hildon_window_key_release_event;
    widget_class->window_state_event = hildon_window_window_state_event;
    
    /* now the object stuff */
    object_class->finalize = hildon_window_finalize;

    /* To the container */
    container_class->forall = hildon_window_forall;
    
    /* gtkobject stuff*/
    GTK_OBJECT_CLASS (window_class)->destroy = hildon_window_destroy; 

    g_type_class_add_private (window_class,
                              sizeof (struct _HildonWindowPrivate));
                     
    /* Install properties */
    g_object_class_install_property (object_class, PROP_IS_TOPMOST,
                g_param_spec_boolean ("is-topmost",
                "Is top-most",
                "Whether the window is currently activated by the window "
                "manager",
                FALSE,
                G_PARAM_READABLE));

    gtk_widget_class_install_style_property (widget_class,
                     g_param_spec_boxed ("borders",
                                         "Graphical borders",
                                         "Size of graphical window borders",
                                          GTK_TYPE_BORDER,
                                          G_PARAM_READABLE));
    
    gtk_widget_class_install_style_property (widget_class,
                     g_param_spec_boxed ("toolbar-borders",
                                         "Graphical toolbar borders",
                                         "Size of graphical toolbar borders",
                                          GTK_TYPE_BORDER,
                                          G_PARAM_READABLE));
    
    /* opera hack, install clip operation signal */
    g_signal_new ("clipboard_operation",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_FIRST,
                  G_STRUCT_OFFSET (HildonWindowClass, clipboard_operation),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1,
                  GTK_TYPE_INT);
}

static void
hildon_window_init (HildonWindow * self)
{
    HildonWindowPrivate *priv = self->priv = HILDON_WINDOW_GET_PRIVATE(self);

    self->priv->vbox = gtk_vbox_new (TRUE, TOOLBAR_MIDDLE);
    gtk_widget_set_parent (self->priv->vbox, GTK_WIDGET(self));
    priv->menu = NULL;
    priv->visible_toolbars = 0;
    priv->is_topmost = FALSE;
    priv->borders = NULL;
    priv->toolbar_borders = NULL;
    priv->escape_timeout = 0;

    priv->fullscreen = FALSE;
   
    priv->program = NULL;
    
    /* We need to track the root window _MB_CURRENT_APP_WINDOW property */
    gdk_window_set_events (gdk_get_default_root_window (),
                          gdk_window_get_events (gdk_get_default_root_window ())                          | GDK_PROPERTY_CHANGE_MASK);

    gdk_window_add_filter (gdk_get_default_root_window (), 
            hildon_window_root_window_event_filter, self);
}

static void
hildon_window_finalize (GObject * obj_self)
{
    HildonWindow *self;
    g_return_if_fail (HILDON_WINDOW (obj_self));
    self = HILDON_WINDOW (obj_self);

    g_free (self->priv->borders);
    g_free (self->priv->toolbar_borders);

    if (G_OBJECT_CLASS (parent_class)->finalize)
        G_OBJECT_CLASS (parent_class)->finalize (obj_self);

}

static void
hildon_window_realize (GtkWidget *widget)
{
    Atom *old_atoms, *new_atoms;
    Display *disp;
    Window window;
    gint atom_count;
    Window active_window;

    GTK_WIDGET_CLASS (parent_class)->realize (widget);
    
    gtk_widget_realize (GTK_WIDGET (HILDON_WINDOW (widget)->priv->vbox));

    
    /* catch the custom button signal from mb to display the menu */
    gdk_window_add_filter (widget->window, hildon_window_event_filter, widget );
    
    window = GDK_WINDOW_XID ( widget->window );
    disp = GDK_WINDOW_XDISPLAY ( widget->window );
    
    /* Enable custom button that is used for menu */
    XGetWMProtocols (disp, window, &old_atoms, &atom_count);
    new_atoms = g_new (Atom, atom_count + 1);

    memcpy (new_atoms, old_atoms, sizeof(Atom) * atom_count);

    new_atoms[atom_count++] =
        XInternAtom (disp, "_NET_WM_CONTEXT_CUSTOM", False);

    XSetWMProtocols (disp, window, new_atoms, atom_count);

    XFree(old_atoms);
    g_free(new_atoms);
       
   /* rely on GDK to set the window group to its default */
   gdk_window_set_group (widget->window, NULL);

   if (HILDON_WINDOW (widget)->priv->program)
   {
       gboolean can_hibernate = hildon_program_get_can_hibernate (
             HILDON_WINDOW (widget)->priv->program);

       hildon_window_set_can_hibernate_property (HILDON_WINDOW (widget),
                 &can_hibernate);
   }

   /* Update the topmost status */
   active_window = hildon_window_get_active_window();
   hildon_window_update_topmost (HILDON_WINDOW (widget), active_window);

   /* Update the window title */
   hildon_window_update_title(HILDON_WINDOW (widget));

}

static void
hildon_window_unrealize (GtkWidget *widget)
{

    gdk_window_remove_filter (widget->window, hildon_window_event_filter,
                              widget);
    
    gtk_widget_unrealize (GTK_WIDGET (HILDON_WINDOW (widget)->priv->vbox));
    GTK_WIDGET_CLASS(parent_class)->unrealize(widget);
}

static void
hildon_window_set_property (GObject * object, guint property_id,
                            const GValue * value, GParamSpec * pspec)
{
    /*HildonWindow *window = HILDON_WINDOW (object);*/

    switch (property_id) {

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
        break;
    }
}

static void
hildon_window_get_property (GObject * object, guint property_id,
                            GValue * value, GParamSpec * pspec)
{
    HildonWindowPrivate *priv = HILDON_WINDOW_GET_PRIVATE (object);

    switch (property_id) {

    case PROP_IS_TOPMOST:
	    g_value_set_boolean (value, priv->is_topmost);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

/*
 * Retrieve the graphical borders size used by the themes
 */
static void
hildon_window_get_borders (HildonWindow *window)
{

    g_free (window->priv->borders);
    g_free (window->priv->toolbar_borders);

    gtk_widget_style_get (GTK_WIDGET (window), "borders",&window->priv->borders,
                          "toolbar-borders", &window->priv->toolbar_borders,
                          NULL);
    
    if (!window->priv->borders)
    {
        window->priv->borders = (GtkBorder *)g_malloc0 (sizeof (GtkBorder));
    }
    
    if (!window->priv->toolbar_borders)
    {
        window->priv->toolbar_borders = 
            (GtkBorder *)g_malloc0 (sizeof (GtkBorder));
    }
}

static void
visible_toolbars (gpointer data, gpointer user_data)
{
    if (GTK_WIDGET_VISIBLE (GTK_WIDGET (((GtkBoxChild *)data)->widget)))
        (*((gint *)user_data)) ++;
}

static gboolean
hildon_window_expose (GtkWidget * widget, GdkEventExpose * event)
{
    HildonWindowPrivate *priv = HILDON_WINDOW (widget)->priv;
    GtkWidget *bx = HILDON_WINDOW(widget)->priv->vbox;
    GtkBox *box = GTK_BOX(bx);
    GtkBorder *b = HILDON_WINDOW(widget)->priv->borders;
    GtkBorder *tb = HILDON_WINDOW(widget)->priv->toolbar_borders;
    gint tb_height = 0;
    gint currently_visible_toolbars = 0;

    if (!priv->borders)
    {
        hildon_window_get_borders (HILDON_WINDOW (widget));
        b = HILDON_WINDOW(widget)->priv->borders;
        tb = HILDON_WINDOW(widget)->priv->toolbar_borders;
    }

    tb_height = bx->allocation.height + tb->top + tb->bottom;

    g_list_foreach (box->children, visible_toolbars, 
            &currently_visible_toolbars);

    paint_toolbar (widget, box,
                   event, priv->fullscreen);

    if (!HILDON_WINDOW (widget)->priv->fullscreen)
    {

        /* Draw the left and right window border */
        gint side_borders_height = widget->allocation.height - b->top;

        if (currently_visible_toolbars)
            side_borders_height -= tb_height;
        else
            side_borders_height -= b->bottom;
        
        if (b->left > 0)
        {
            gtk_paint_box (widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, "left-border",
                    widget->allocation.x, widget->allocation.y +
                    b->top, b->left, side_borders_height);
        } 

        if (b->right > 0)
        {
            gtk_paint_box (widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, "right-border",
                    widget->allocation.x + widget->allocation.width -
                    b->right, widget->allocation.y + b->top,
                    b->right, side_borders_height);
        }

        /* If no toolbar, draw the bottom window border */
        if (!currently_visible_toolbars && b->bottom > 0)
        {
            gtk_paint_box (widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, "bottom-border",
                    widget->allocation.x, widget->allocation.y +
                    (widget->allocation.height - b->bottom),
                    widget->allocation.width, b->bottom);
        }

        /* Draw the top border */
        if (b->top > 0)
        {
            gtk_paint_box (widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, "top-border",
                    widget->allocation.x, widget->allocation.y,
                    widget->allocation.width, b->top);
        } 


    }

    /* don't draw the window stuff as it overwrites our borders with a blank
     * rectangle. Instead start with the drawing of the GtkBin */
    GTK_WIDGET_CLASS (g_type_class_peek_parent (parent_class))->
        expose_event (widget, event);
    /*GTK_WIDGET_CLASS (parent_class))->
        expose_event (widget, event);*/

    return FALSE;

}

static void
hildon_window_size_request (GtkWidget * widget, GtkRequisition * requisition)
{
    HildonWindowPrivate *priv = HILDON_WINDOW (widget)->priv;
    GtkWidget *child = GTK_BIN (widget)->child;
    GtkRequisition req2;
    gint border_width = GTK_CONTAINER(widget)->border_width;
   
    if (!priv->borders)
    {
        hildon_window_get_borders (HILDON_WINDOW (widget));
    }
    
    if (child)
        gtk_widget_size_request (child, requisition);

    if (HILDON_WINDOW (widget)->priv->vbox != NULL)
        gtk_widget_size_request (HILDON_WINDOW (widget)->priv->vbox,
                &req2);

    requisition->height += req2.height;
    requisition->width = (requisition->width < req2.width) ? 
        req2.width : requisition->width;


    requisition->width  += 2 * border_width;
    requisition->height += 2 * border_width;
    
    if (!priv->fullscreen)
    {
        requisition->height += priv->borders->top;
        if (req2.height == 0)
            requisition->height += priv->borders->bottom;
        requisition->width += priv->borders->left + priv->borders->right;
    }
}

static void
hildon_window_size_allocate (GtkWidget * widget, GtkAllocation * allocation)
{
    HildonWindowPrivate *priv = HILDON_WINDOW (widget)->priv;
    GtkAllocation box_alloc;
    GtkAllocation alloc = *allocation;
    GtkRequisition req;
    gint border_width = GTK_CONTAINER(widget)->border_width;

    GtkWidget *box = HILDON_WINDOW(widget)->priv->vbox;
    GtkBin *bin = GTK_BIN(widget);
    GtkBorder *b = HILDON_WINDOW (widget)->priv->borders;
    GtkBorder *tb = HILDON_WINDOW (widget)->priv->toolbar_borders;
    
    if (!priv->borders)
    {
        hildon_window_get_borders (HILDON_WINDOW (widget));
        b = HILDON_WINDOW (widget)->priv->borders;
        tb = HILDON_WINDOW (widget)->priv->toolbar_borders;
    }
    
    widget->allocation = *allocation;

    gtk_widget_get_child_requisition (box, &req);

    box_alloc.width = allocation->width - tb->left - tb->right;
    box_alloc.height = ( (req.height < allocation->height) ?
        req.height : allocation->height );
    box_alloc.x = allocation->x + tb->left;
    box_alloc.y = allocation->y + allocation->height - box_alloc.height - tb->bottom;

    if (bin->child != NULL && GTK_IS_WIDGET (bin->child)
                           && GTK_WIDGET_VISIBLE (bin->child))
    {
        alloc.x += border_width;
        alloc.y += border_width;
        alloc.width -= (border_width * 2);
        alloc.height -= (border_width * 2) + box_alloc.height;

        if (!(HILDON_WINDOW (widget)->priv->fullscreen))
        {
            alloc.x += b->left;
            alloc.width -= (b->left + b->right);
            alloc.y += b->top;

            alloc.height -= b->top;

            if (box_alloc.height <= 0)
                alloc.height -= b->bottom;
            else
                alloc.height -= (tb->top + tb->bottom);            
        }
        else
        {
            if (!(box_alloc.height <= 0))
                alloc.height -= (tb->top + tb->bottom);              
        }

        gtk_widget_size_allocate (bin->child, &alloc);
    }


    gtk_widget_size_allocate (box, &box_alloc);

    if (priv->previous_vbox_y != box_alloc.y)
    {
        /* The size of the VBox has changed, we need to redraw part
         * of the window borders */
        gint draw_from_y = priv->previous_vbox_y < box_alloc.y?
            priv->previous_vbox_y - tb->top:
            box_alloc.y - tb->top;
        
        gtk_widget_queue_draw_area (widget, 0, draw_from_y, 
                widget->allocation.width,
                widget->allocation.height - draw_from_y);
        
        priv->previous_vbox_y = box_alloc.y;
    }

}

static void
hildon_window_forall (GtkContainer * container, gboolean include_internals,
                      GtkCallback callback,     gpointer callback_data)
{
    HildonWindow *self = HILDON_WINDOW (container);

    g_return_if_fail (callback != NULL);

    GTK_CONTAINER_CLASS (parent_class)->forall (container, include_internals,
                                                callback, callback_data);
    if (include_internals && self->priv->vbox != NULL)
            (* callback)(GTK_WIDGET (self->priv->vbox), callback_data);
}

static void
hildon_window_show_all (GtkWidget *widget)
{
    HildonWindow *self = HILDON_WINDOW (widget);
    
    GTK_WIDGET_CLASS (parent_class)->show_all (widget);
    gtk_widget_show_all (self->priv->vbox);

}

static void
hildon_window_destroy (GtkObject *obj)
{
    HildonWindow *self = HILDON_WINDOW (obj);
    GList *menu_list;

    if (self->priv->vbox != NULL)
    {
        if (self->priv->program)
        {
            GtkWidget * common_toolbar = GTK_WIDGET (
                    hildon_program_get_common_toolbar (self->priv->program));
            if (common_toolbar && common_toolbar->parent == self->priv->vbox)
            {
                gtk_container_remove (GTK_CONTAINER (self->priv->vbox),
                        common_toolbar);
            }
        }
        
        gtk_widget_unparent (self->priv->vbox);
        self->priv->vbox = NULL;	
        
    }

    menu_list = g_list_copy (gtk_menu_get_for_attach_widget (GTK_WIDGET (obj)));

    while (menu_list)
    {
        if (GTK_IS_MENU(menu_list->data))
        {
            if (GTK_WIDGET_VISIBLE (GTK_WIDGET (menu_list->data)))
            {
                gtk_menu_popdown (GTK_MENU (menu_list->data));
                gtk_menu_shell_deactivate (GTK_MENU_SHELL (menu_list->data));
            }
            gtk_menu_detach (GTK_MENU (menu_list->data));
        }
        menu_list = menu_list->next;
    }

    g_list_free (menu_list);
    
    if (self->priv->program)
    {
        hildon_program_remove_window (self->priv->program, self);
    }
    
    gdk_window_remove_filter (gdk_get_default_root_window(), 
                              hildon_window_root_window_event_filter,
                              obj);
    
    gtk_widget_set_events (GTK_WIDGET(obj), 0);

    GTK_OBJECT_CLASS (parent_class)->destroy (obj);
}


static void
hildon_window_notify (GObject *gobject, GParamSpec *param)
{
    HildonWindow *window = HILDON_WINDOW (gobject);
    
    if (strcmp (param->name, "title") == 0)
    {

        hildon_window_update_title (window);
    }
    else if (strcmp (param->name, "is-topmost"))
    {
        hildon_window_is_topmost_notify (window);
    }

    if (G_OBJECT_CLASS(parent_class)->notify)
        G_OBJECT_CLASS(parent_class)->notify (gobject, param);
}

/* Utilities */

static void
visible_toolbar (gpointer data, gpointer user_data)
{
    if (GTK_WIDGET_VISIBLE (((GtkBoxChild *)data)->widget))
        (*((gint *)user_data))++;
}

static void 
find_findtoolbar_index (gpointer data, gpointer user_data)
{
    gint *pass_bundle = (gint *)user_data;

    if(((GtkBoxChild *)data)->widget->allocation.y < pass_bundle[0]
            && GTK_WIDGET_VISIBLE (((GtkBoxChild *)data)->widget))
        pass_bundle[1]++;
}

static void
find_findtoolbar (gpointer data, gpointer user_data)
{
    if(HILDON_IS_FIND_TOOLBAR (((GtkBoxChild *)data)->widget)
            && GTK_WIDGET_VISIBLE (((GtkBoxChild *)data)->widget))
        (*((GtkWidget **)user_data)) = ((GtkBoxChild *)data)->widget;
}

static void
paint_toolbar (GtkWidget *widget, GtkBox *box, 
		       GdkEventExpose * event, 
			   gboolean fullscreen)
{
    gint toolbar_num = 0; 
    gint ftb_index = 0;
    gint count;
    GtkWidget *findtoolbar = NULL;
    gchar toolbar_mode[40];
    GtkBorder *tb = HILDON_WINDOW (widget)->priv->toolbar_borders;

    /* collect info to help on painting the boxes */
    g_list_foreach (box->children, visible_toolbar, 
		            (gpointer) &toolbar_num);
    
    if(toolbar_num <= 0)
        return;
    
    g_list_foreach (box->children, find_findtoolbar, (gpointer) &findtoolbar);
    
    if (findtoolbar != NULL)
    {
        gint pass_bundle[2];/* an array for convient data passing
                               the first member contains the y allocation
                               of the find toolbar, and the second allocation
                               contains the index(how many toolbars are above
                               find toolbar) */
        pass_bundle[0] = findtoolbar->allocation.y;
        pass_bundle[1] = ftb_index;
        g_list_foreach(box->children, find_findtoolbar_index,
                (gpointer) pass_bundle);
        ftb_index = pass_bundle[1];
    }
    
    /*upper border*/
    sprintf (toolbar_mode, "toolbar%sframe-top", 
             fullscreen ? "-fullscreen-" : "-");
    gtk_paint_box (widget->style, widget->window,
		    GTK_WIDGET_STATE (widget), GTK_SHADOW_OUT,
		    &event->area, widget, toolbar_mode,
		    widget->allocation.x,
		    GTK_WIDGET (box)->allocation.y - tb->top,
		    widget->allocation.width, tb->top);
    
    /*top most toolbar painting*/
    if (findtoolbar != NULL && ftb_index == 0 )
    {
        sprintf (toolbar_mode, "findtoolbar%s", 
                 fullscreen ? "-fullscreen" : "");

        gtk_paint_box (widget->style, widget->window,
                GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                &event->area, widget, toolbar_mode,
                widget->allocation.x,
                GTK_WIDGET(box)->allocation.y,
                widget->allocation.width,
                TOOLBAR_HEIGHT);
    }
    else
    {
        sprintf (toolbar_mode, "toolbar%s", 
                fullscreen ? "-fullscreen" : "");
            
        gtk_paint_box (widget->style, widget->window,
                GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                &event->area, widget, toolbar_mode,
                widget->allocation.x,
                GTK_WIDGET(box)->allocation.y,
                widget->allocation.width,
                TOOLBAR_HEIGHT);
    }
    /*multi toolbar painting*/
    for (count = 0; count < toolbar_num - 1; count++)
    {
        sprintf (toolbar_mode, "toolbar%sframe-middle", 
                 fullscreen ? "-fullscreen-" : "-");

        gtk_paint_box (widget->style, widget->window,
                GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                &event->area, widget, toolbar_mode,
                widget->allocation.x,
                GTK_WIDGET(box)->allocation.y + 
                (1 + count) * TOOLBAR_HEIGHT + 
                count * TOOLBAR_MIDDLE,
                widget->allocation.width,
                TOOLBAR_MIDDLE);

        if (findtoolbar != NULL && count + 1 == ftb_index)
        {
            
            sprintf (toolbar_mode, "findtoolbar%s", 
                     fullscreen ? "-fullscreen" : "");

            gtk_paint_box (widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, toolbar_mode,
                    widget->allocation.x,
                    GTK_WIDGET(box)->allocation.y + 
                    (1 + count) * (TOOLBAR_HEIGHT + TOOLBAR_MIDDLE),
                    widget->allocation.width,
                    TOOLBAR_HEIGHT);
        }
        else
        {
            sprintf (toolbar_mode, "toolbar%s", 
                     fullscreen ? "-fullscreen" : "");

            gtk_paint_box (widget->style, widget->window,
                    GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                    &event->area, widget, toolbar_mode,
                    widget->allocation.x,
                    GTK_WIDGET(box)->allocation.y + 
                    (1 + count) * (TOOLBAR_HEIGHT + TOOLBAR_MIDDLE),
                    widget->allocation.width,
                    TOOLBAR_HEIGHT);
        }
    }
    sprintf (toolbar_mode, "toolbar%sframe-bottom", 
            fullscreen ? "-fullscreen-" : "-");

    gtk_paint_box (widget->style, widget->window,
            GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
            &event->area, widget, toolbar_mode,
            widget->allocation.x,
            GTK_WIDGET(box)->allocation.y + 
            GTK_WIDGET(box)->allocation.height,
            widget->allocation.width, tb->bottom);

}

/*
 * Checks the root window to know which is the topped window
 */
Window
hildon_window_get_active_window (void)
{
    Atom realtype;
    int format;
    int status;
    Window ret;
    unsigned long n;
    unsigned long extra;
    union
    {
        Window *win;
        unsigned char *char_pointer;
    } win;
    Atom active_app_atom = 
            XInternAtom (GDK_DISPLAY (), "_MB_CURRENT_APP_WINDOW", False);

    win.win = NULL;

    status = XGetWindowProperty (GDK_DISPLAY(), GDK_ROOT_WINDOW(),
            active_app_atom, 0L, 16L,
            0, XA_WINDOW, &realtype, &format,
            &n, &extra, &win.char_pointer);
    if (!(status == Success && realtype == XA_WINDOW && format == 32
                && n == 1 && win.win != NULL))
    {
        if (win.win != NULL)
            XFree (win.char_pointer);
        return None;
    }

    ret = win.win[0];

    if (win.win != NULL)
        XFree(win.char_pointer);

    return ret;
}

static int
xclient_message_type_check (XClientMessageEvent *cm, const gchar *name)
{
    return cm->message_type == XInternAtom(GDK_DISPLAY(), name, FALSE);
}

/*****************/
/* Event filters */
/*****************/

/*
 * Handle the window border custom button, which toggles the menu,
 * and the Hildon input method copy paste messages
 */
static GdkFilterReturn
hildon_window_event_filter (GdkXEvent *xevent, GdkEvent *event, gpointer data)
{
    XAnyEvent *eventti = xevent;

    if (eventti->type == ClientMessage)
    {
        XClientMessageEvent *cm = xevent;

        if (xclient_message_type_check (cm, "_MB_GRAB_TRANSFER"))
        {
            hildon_window_toggle_menu (HILDON_WINDOW ( data ));
            return GDK_FILTER_REMOVE;
        }
        /* opera hack clipboard client message */
        else if (xclient_message_type_check (cm, "_HILDON_IM_CLIPBOARD_COPY"))
        {
            g_signal_emit_by_name(G_OBJECT(data), "clipboard_operation",
                                  HILDON_WINDOW_CO_COPY);
            return GDK_FILTER_REMOVE;
        }
        else if (xclient_message_type_check(cm, "_HILDON_IM_CLIPBOARD_CUT"))
        {
            g_signal_emit_by_name(G_OBJECT(data), "clipboard_operation",
                                  HILDON_WINDOW_CO_CUT);
            return GDK_FILTER_REMOVE;
        }
        else if (xclient_message_type_check(cm, "_HILDON_IM_CLIPBOARD_PASTE"))
        {
            g_signal_emit_by_name(G_OBJECT(data), "clipboard_operation",
                                  HILDON_WINDOW_CO_PASTE);
            return GDK_FILTER_REMOVE;
        }
    }

    return GDK_FILTER_CONTINUE;
}

/*
 * Here we keep track of changes in the _MB_CURRENT_APP_WINDOW,
 * to know when we acquire/lose topmost status
 */
static GdkFilterReturn
hildon_window_root_window_event_filter (GdkXEvent *xevent, 
                                        GdkEvent *event, 
                                        gpointer data)
{
    XAnyEvent *eventti = xevent;
    HildonWindow *hwindow = HILDON_WINDOW (data);


    if (eventti->type == PropertyNotify)
    {
        XPropertyEvent *pevent = xevent;
        Atom active_app_atom = 
            XInternAtom (GDK_DISPLAY (), "_MB_CURRENT_APP_WINDOW", False);

        if (pevent->atom == active_app_atom)
        {
            Window active_window = hildon_window_get_active_window();

            hildon_window_update_topmost (hwindow, active_window);
        }
    }

    return GDK_FILTER_CONTINUE;
}

/***************************/
/*   Signal handlers       */
/***************************/

/*
 * Handle the menu hardware key here
 */
static gboolean
hildon_window_key_press_event (GtkWidget *widget, GdkEventKey *event)
{
    HildonWindowPrivate *priv;

    g_return_val_if_fail (HILDON_IS_WINDOW (widget),FALSE);

    priv = HILDON_WINDOW (widget)->priv;

    switch (event->keyval)
    {
        case HILDON_HARDKEY_MENU:
            if (hildon_window_toggle_menu (HILDON_WINDOW (widget)))
                return TRUE;
            break;
        case HILDON_HARDKEY_ESC:
            if (!priv->escape_timeout)
            {
                priv->escape_timeout = g_timeout_add 
                    (HILDON_WINDOW_LONG_PRESS_TIME,
                     hildon_window_escape_timeout, widget);
            }
            break;
    }

    return GTK_WIDGET_CLASS (parent_class)->key_press_event (widget, event);

}

static gboolean
hildon_window_key_release_event (GtkWidget *widget, GdkEventKey *event)
{
    HildonWindowPrivate *priv;

    g_return_val_if_fail (HILDON_IS_WINDOW (widget),FALSE);

    priv = HILDON_WINDOW (widget)->priv;

    switch (event->keyval)
    {
        case HILDON_HARDKEY_ESC:
            if (priv->escape_timeout)
            {
                g_source_remove (priv->escape_timeout);
                priv->escape_timeout = 0;
            }
            break;
    }

    return GTK_WIDGET_CLASS (parent_class)->key_release_event (widget, event);

}

/*
 * We keep track of the window state changes, because the drawing
 * (borders) differs whether we are in fullscreen mode or not
 */
static gboolean
hildon_window_window_state_event (GtkWidget *widget, 
                                  GdkEventWindowState *event)
{
    if (event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
    {
        HILDON_WINDOW (widget)->priv->fullscreen = 
            event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN;
    }

    if (GTK_WIDGET_CLASS (parent_class)->window_state_event)
    {
        return GTK_WIDGET_CLASS (parent_class)->window_state_event (
                widget,
                event);
    }
    else
    {
        return FALSE;
    }
}

static void 
hildon_window_title_notify (GObject *gobject,
                            GParamSpec *arg1,
                            gpointer user_data)
{
    HildonWindow *window = HILDON_WINDOW (gobject);

    hildon_window_update_title (window);

}

/*******************/
/*     General     */
/*******************/

/*The menu popuping needs a menu popup-function*/
static void
hildon_window_menupopupfunc (GtkMenu *menu, gint *x, gint *y,
                             gboolean *push_in, GtkWidget *widget)
{
    gint window_x = 0;
    gint window_y = 0;
    GdkWindow *window = GTK_WIDGET(widget)->window;
    
    if (window)
    {
        gdk_window_get_origin (window, &window_x, &window_y);
    }

    gtk_widget_style_get (GTK_WIDGET (menu), "horizontal-offset", x,
            "vertical-offset", y, NULL);

    *x += window_x;
    *y += window_y;
  
}

static void
hildon_window_menupopupfuncfull ( GtkMenu *menu, gint *x, gint *y,
                                              gboolean *push_in, 
                                              GtkWidget *widget )
{
    gtk_widget_style_get (GTK_WIDGET (menu), "horizontal-offset", x,
            "vertical-offset", y, NULL);

    *x = MAX (0, *x);
    *y = MAX (0, *y);
}


/********************/
/* Private methods  */
/********************/


/*
 * Takes the common toolbar when we acquire the top-most status
 */
static void
hildon_window_is_topmost_notify (HildonWindow *window)
{
    if (window->priv->is_topmost)
    {
        hildon_window_take_common_toolbar (window);
    }

    else
    {
        /* If the window lost focus while the user started to press
         * the ESC key, we won't get the release event. We need to
         * stop the timeout*/
        if (window->priv->escape_timeout)
        {
            g_source_remove (window->priv->escape_timeout);
            window->priv->escape_timeout = 0;
        }
    }
}

/*
 * Sets the program to which the window belongs. This should only be called
 * by hildon_program_add_window
 */
void
hildon_window_set_program (HildonWindow *self, GObject *program)
{
    if (self->priv->program)
    {
        g_object_unref (self->priv->program);
    }

    /* Now that we are bound to a program, we can rely on it to track the
     * root window */
    gdk_window_remove_filter (gdk_get_default_root_window(), 
                              hildon_window_root_window_event_filter,
                              self);

    self->priv->program = HILDON_PROGRAM (program);
    g_object_ref (program);
}

/*
 * Unsets the program to which the window belongs. This should only be called
 * by hildon_program_add_window
 */
void
hildon_window_unset_program (HildonWindow *self)
{
    g_return_if_fail(self && HILDON_IS_WINDOW (self));

    if (self->priv->program)
    {
        g_object_unref (self->priv->program);
        self->priv->program = NULL;

        /* We need to start tacking the root window again */
        gdk_window_set_events (gdk_get_default_root_window (),
                gdk_window_get_events (gdk_get_default_root_window ())
                | GDK_PROPERTY_CHANGE_MASK);

        gdk_window_add_filter (gdk_get_default_root_window (),
                hildon_window_root_window_event_filter, self );
    }

    self->priv->program = NULL;
}

/*
 * Sets whether or not the program to which this window belongs is
 * killable. This is used by the HildonProgram to signify to the
 * Task Navigator whether or not it can hibernate in memory-low situations
 **/    
void
hildon_window_set_can_hibernate_property (HildonWindow *self, 
                                          gpointer _can_hibernate)
{
    GdkAtom killable_atom;
    gboolean can_hibernate;

    g_return_if_fail(self && HILDON_IS_WINDOW (self));
        
    if (!GTK_WIDGET_REALIZED ((GTK_WIDGET (self))))
    {
        return;
    }

    can_hibernate = * ((gboolean *)_can_hibernate);

    killable_atom = gdk_atom_intern (CAN_HIBERNATE_PROPERTY, FALSE);

    if (can_hibernate)
    {
        gdk_property_change (GTK_WIDGET (self)->window, killable_atom,
                (GdkAtom)31/* XA_STRING */, 8,
                GDK_PROP_MODE_REPLACE, (const guchar *)CAN_HIBERNATE,
                CAN_HIBERNATE_LENGTH);
    }
    else
    {
        gdk_property_delete (GTK_WIDGET (self)->window, killable_atom);
    }
        
}

/*
 * If a common toolbar was set to the program, reparent it to
 * us
 */
void
hildon_window_take_common_toolbar (HildonWindow *self)
{
    g_return_if_fail(self && HILDON_IS_WINDOW (self));

    if (self->priv->program)
    {
        GtkWidget *common_toolbar =  
           GTK_WIDGET (hildon_program_get_common_toolbar (self->priv->program));

        if (common_toolbar && common_toolbar->parent != self->priv->vbox)
        {
            g_object_ref (common_toolbar);
            if (common_toolbar->parent)
            {
                gtk_container_remove (GTK_CONTAINER (common_toolbar->parent),
                        common_toolbar);
            }

            gtk_box_pack_end (GTK_BOX(self->priv->vbox), common_toolbar,
                    TRUE, TRUE, 0);
            g_object_unref (common_toolbar);

            gtk_widget_set_size_request (common_toolbar, -1, TOOLBAR_HEIGHT);

            gtk_widget_show  (self->priv->vbox);

        }
    }
}

/*
 * Compare the window that was last topped, and act consequently
 */
void
hildon_window_update_topmost (HildonWindow *self, Window window_id)
{
    Window my_window;
    
    my_window = GDK_WINDOW_XID (GTK_WIDGET (self)->window);

    if (window_id == my_window)
    {
        if (!self->priv->is_topmost)
        {
            self->priv->is_topmost = TRUE;
            hildon_window_is_topmost_notify (self);
            g_object_notify (G_OBJECT (self), "is-topmost");
        }
    }
    else if (self->priv->is_topmost)
    {
        /* Should this go in the signal handler? */
        GtkWidget *focus = gtk_window_get_focus (GTK_WINDOW (self));

        if (GTK_IS_ENTRY (focus))
            gtk_im_context_focus_out (GTK_ENTRY (focus)->im_context);
        if (GTK_IS_TEXT_VIEW (focus))
            gtk_im_context_focus_out (GTK_TEXT_VIEW (focus)->im_context);
            
        self->priv->is_topmost = FALSE;
        hildon_window_is_topmost_notify (self);
        g_object_notify (G_OBJECT (self), "is-topmost");

    }
}
    
/*
 * If the application
 * was given a name (with g_set_application_name(), set 
 * "ProgramName - WindowTitle" as the displayed
 * title
 */
void
hildon_window_update_title (HildonWindow *window)
{
    const gchar * application_name;
    g_return_if_fail (window && HILDON_IS_WINDOW (window));

    if (!GTK_WIDGET_REALIZED (window))
    {
        return;
    }

    application_name = g_get_application_name ();

    if (application_name && application_name[0])
    {
        const gchar *old_title = gtk_window_get_title (GTK_WINDOW (window));

        if (old_title && old_title[0])
        {
            gchar *title = NULL;
            
            title = g_strjoin (TITLE_SEPARATOR, application_name,
                    old_title, NULL);
           
            gdk_window_set_title (GTK_WIDGET (window)->window, title);
            
            g_free (title);
        }

    }
}

static void
detach_menu_func (GtkWidget *attach_widget, GtkMenu *menu)
{
}
/*
 * Toggles the display of the HildonWindow menu.
 * Returns whether or not something was done (whether or not we had a menu
 * to toggle)
 */
static gboolean
hildon_window_toggle_menu (HildonWindow * self)
{
    GtkMenu *menu_to_use = NULL;
    GList *menu_children = NULL;
    
    g_return_val_if_fail (self && HILDON_IS_WINDOW (self), FALSE);

    /* Select which menu to use, Window specific has highest priority,
     * then program specific */
    if (self->priv->menu)
    {
        menu_to_use = GTK_MENU (self->priv->menu);
    }
    else if (self->priv->program)
    {
        menu_to_use = hildon_program_get_common_menu (self->priv->program);
        if (menu_to_use && gtk_menu_get_attach_widget (menu_to_use) != 
                GTK_WIDGET (self))
        {
            g_object_ref (menu_to_use);
            if (gtk_menu_get_attach_widget (menu_to_use))
            {
                gtk_menu_detach (menu_to_use);
            }

            gtk_menu_attach_to_widget (menu_to_use, GTK_WIDGET (self), 
                    &detach_menu_func);
            g_object_unref (menu_to_use);
        }
    }

    if (!menu_to_use)
    {
        return FALSE;
    }
    

    if (GTK_WIDGET_MAPPED (GTK_WIDGET (menu_to_use)))
    {
        gtk_menu_popdown (menu_to_use);
        gtk_menu_shell_deactivate (GTK_MENU_SHELL (menu_to_use));
        return TRUE;
    }

    /* Check if the menu has items */
    menu_children = gtk_container_get_children (GTK_CONTAINER (menu_to_use));

    if (menu_children)
    {
        g_list_free (menu_children);

        /* Apply right theming */
        gtk_widget_set_name (GTK_WIDGET (menu_to_use),
                "menu_force_with_corners");
        
        if (self->priv->fullscreen) 
        {
            gtk_menu_popup (menu_to_use, NULL, NULL,
                           (GtkMenuPositionFunc)
                           hildon_window_menupopupfuncfull,
                           self, 0, 
                           gtk_get_current_event_time ());
        }
        else
        {
            gtk_menu_popup (menu_to_use, NULL, NULL,
                           (GtkMenuPositionFunc)
                           hildon_window_menupopupfunc,
                           self, 0, 
                           gtk_get_current_event_time ());
        }
        gtk_menu_shell_select_first (GTK_MENU_SHELL (menu_to_use), TRUE);
        return TRUE;
    }

    return FALSE;

}

/*
 * If the ESC key was not released when the timeout expires,
 * close the window
 */
static gboolean
hildon_window_escape_timeout (gpointer data)
{
    HildonWindowPrivate *priv;
    GdkEvent *event;

    GDK_THREADS_ENTER ();
    
    priv = HILDON_WINDOW(data)->priv;

    /* Send fake event, simulation a situation that user
       pressed 'x' from the corner */
    event = gdk_event_new(GDK_DELETE);
    ((GdkEventAny *)event)->window = GDK_WINDOW (g_object_ref (GTK_WIDGET(data)->window));
    gtk_main_do_event(event);

    /* That unrefs the window, so we're reffing it above */
    gdk_event_free(event);

    priv->escape_timeout = 0;

    GDK_THREADS_LEAVE ();
    
    return FALSE;
}


/******************/
/* public methods */
/******************/


/**
 * hildon_window_new: 
 * 
 * Use this function to create a new HildonWindow.
 * 
 * Return value: A @HildonWindow.
 **/
GtkWidget *
hildon_window_new (void)
{
    HildonWindow *newwindow = g_object_new (HILDON_TYPE_WINDOW, NULL);

    return GTK_WIDGET (newwindow);
}

/**
 * hildon_window_add_with_scrollbar
 * @self : A @HildonWindow
 * @child : A @GtkWidget
 *
 * Adds the @child to the HildonWindow and creates a scrollbar
 * to it. Similar as adding first a @GtkScrolledWindow and then the
 * @child to it.
 */
void
hildon_window_add_with_scrollbar (HildonWindow * self,
                                  GtkWidget * child)
{
    GtkScrolledWindow *scrolledw;

    g_return_if_fail (HILDON_IS_WINDOW (self));
    g_return_if_fail (GTK_IS_WIDGET (child));
    g_return_if_fail (child->parent == NULL);

    scrolledw = GTK_SCROLLED_WINDOW (gtk_scrolled_window_new (NULL, NULL));
    gtk_scrolled_window_set_policy (scrolledw, GTK_POLICY_NEVER,
                                    GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (scrolledw, GTK_SHADOW_NONE);

    if (GTK_IS_VIEWPORT (child))
        gtk_container_add (GTK_CONTAINER (scrolledw), child);
    else
    {
        if (GTK_IS_CONTAINER (child) )
            gtk_container_set_focus_vadjustment (GTK_CONTAINER(child),
                    gtk_scrolled_window_get_vadjustment (scrolledw) );
        gtk_scrolled_window_add_with_viewport (scrolledw, child);
    }

    gtk_container_add (GTK_CONTAINER (self), GTK_WIDGET (scrolledw));
}

/**
 * hildon_window_add_toolbar:
 * @self: A @HildonWindow
 * @toolbar: A #GtkToolbar to add to the HildonWindow
 *
 * Adds a toolbar to the window.
 **/
void 
hildon_window_add_toolbar (HildonWindow *self, GtkToolbar *toolbar)
{
    GtkBox *vbox;
    
    g_return_if_fail (self && HILDON_IS_WINDOW (self));
    g_return_if_fail (toolbar && GTK_IS_TOOLBAR (toolbar));

    vbox = GTK_BOX (self->priv->vbox);

    gtk_box_pack_start (vbox, GTK_WIDGET(toolbar), TRUE, TRUE, 0);
    gtk_box_reorder_child (vbox, GTK_WIDGET(toolbar), 0);
    gtk_widget_set_size_request (GTK_WIDGET (toolbar), -1, TOOLBAR_HEIGHT);

    gtk_widget_queue_resize (GTK_WIDGET(self));
}

/**
 * hildon_window_remove_toolbar:
 * @self: A @HildonWindow
 * @toolbar: A #GtkToolbar to remove from the HildonWindow
 *
 * Removes a toolbar from the window.
 **/
void
hildon_window_remove_toolbar (HildonWindow *self, GtkToolbar *toolbar)
{
    GtkContainer *vbox = GTK_CONTAINER (self->priv->vbox);
    
    g_return_if_fail (self && HILDON_IS_WINDOW (self));

    gtk_container_remove (vbox, GTK_WIDGET(toolbar));
}

/**
 * hildon_window_get_menu:
 * @self : #HildonWindow
 * 
 * Gets the #GtMenu assigned to the #HildonAppview.
 * 
 * Return value: The #GtkMenu assigned to this application view.
 **/
GtkMenu *
hildon_window_get_menu (HildonWindow * self)
{
    g_return_val_if_fail (self && HILDON_IS_WINDOW (self), NULL);

    return GTK_MENU (self->priv->menu);
}


/**
 * hildon_window_set_menu:
 * @self: A #HildonWindow
 * @menu: The #GtkMenu to be used for this #HildonWindow
 * 
 * Sets the menu to be used for this window. This menu overrides
 * a program-wide menu that may have been set with
 * hildon_program_set_common_menu. Pass NULL to remove the current
 * menu.
 **/ 
void
hildon_window_set_menu (HildonWindow *self, GtkMenu *menu)
{
    g_return_if_fail (HILDON_IS_WINDOW (self));

    if (self->priv->menu != NULL) {
        gtk_menu_detach (GTK_MENU (self->priv->menu));
        g_object_unref (self->priv->menu);
    }

    self->priv->menu = (menu != NULL) ? GTK_WIDGET (menu) : NULL;
    if (self->priv->menu != NULL) {
        gtk_widget_set_name (self->priv->menu, "menu_force_with_corners");
        gtk_menu_attach_to_widget (GTK_MENU (self->priv->menu), GTK_WIDGET (self), &detach_menu_func);
        g_object_ref (GTK_MENU (self->priv->menu));
        gtk_widget_show_all (GTK_WIDGET (self->priv->menu));
    }
}

/**
 * hildon_window_get_is_topmost:
 * @self: A #HildonWindow
 * 
 * Return value: Whether or not the #HildonWindow is currenltly activated
 * by the window manager.
 **/
gboolean
hildon_window_get_is_topmost(HildonWindow *self){
    g_return_val_if_fail (HILDON_IS_WINDOW (self), FALSE);
    
    return self->priv->is_topmost;
}

