/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
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

/*
 * @file hildon-grid.c
 *
 * This file contains the implementation of HildonGrid. HildonGrid is used
 * in views like Home and Control Panel which have single-tap activated
 * items.
 */

/*
 * TODO
 * - there must be a predefined place for the "no items" -label...
 * - performance :-)
 * - dimmed items & scrolling by scrollbar
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkrange.h>
#include <gtk/gtkvscrollbar.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkenums.h>
#include <gdk/gdkkeysyms.h>

#include "hildon-grid-item-private.h"
#include <hildon-widgets/hildon-grid.h>
#include <hildon-widgets/hildon-grid-item.h>

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

#define HILDON_GRID_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_GRID, \
                                      HildonGridPrivate))


#define DEFAULT_STYLE   "largeicons-home"

#define DEFAULT_WIDTH         128
#define DEFAULT_HEIGHT         96
#define DEFAULT_HSPACING        8
#define DEFAULT_VSPACING        8
#define DEFAULT_LABEL_HSPACING  4
#define DEFAULT_LABEL_VSPACING  0

#define DEFAULT_EMBLEM_SIZE    25
#define DEFAULT_ICON_SIZE      64
#define DEFAULT_N_COLUMNS       3
#define DEFAULT_LABEL_POS       1
#define GRID_LABEL_POS_PAD     16

#define DRAG_SENSITIVITY        6


enum {
    ACTIVATE_CHILD,
    POPUP_CONTEXT,
    LAST_SIGNAL
};

enum {
    PROP_0,
    PROP_EMPTY_LABEL
};


typedef struct _HildonGridChild HildonGridChild;
typedef struct _HildonGridPrivate HildonGridPrivate;


struct _HildonGridChild {
    GtkWidget *widget;
};


struct _HildonGridPrivate {
    GList *children;
    GtkWidget *scrollbar;
    gint old_sb_pos;
    GdkWindow *event_window;

    gchar *style;
    /* We don't want to fetch style properties every single time. */
    guint icon_size;
    guint emblem_size;
    guint num_columns;
    HildonGridPositionType label_pos;
    gint label_hspacing;
    gint label_vspacing;
    GtkWidget *empty_label;

    guint widget_width;
    guint widget_height;
    guint widget_hspacing;
    guint widget_vspacing;

    guint saved_width;
    guint saved_height;
    guint saved_hspacing;
    guint saved_vspacing;


    gint focus_index;
    guint click_x;
    guint click_y;

    /* Handy variables outsize _allocate. */
    gint area_height;
    gint area_rows;
    gint scrollbar_width;
    gint visible_cols;

    gint first_index;
    GdkEventType last_button_event;
};



/* Prototypes. */
static void hildon_grid_class_init(HildonGridClass * klass);
static void hildon_grid_init(HildonGrid * grid);
static void hildon_grid_realize(GtkWidget * widget);
static void hildon_grid_unrealize(GtkWidget * widget);
static void hildon_grid_map(GtkWidget * widget);
static void hildon_grid_unmap(GtkWidget * widget);
static gboolean hildon_grid_expose(GtkWidget * widget,
                                   GdkEventExpose * event);
static void hildon_grid_size_request(GtkWidget * widget,
                                     GtkRequisition * requisition);
static void hildon_grid_size_allocate(GtkWidget * widget,
                                      GtkAllocation * allocation);
static void hildon_grid_add(GtkContainer * container, GtkWidget * widget);
static void hildon_grid_remove(GtkContainer * container,
                               GtkWidget * widget);
static void hildon_grid_set_focus_child(GtkContainer * container,
                                        GtkWidget * widget);
static void hildon_grid_forall(GtkContainer * container,
                               gboolean include_internals,
                               GtkCallback callback,
                               gpointer callback_data);
static void hildon_grid_tap_and_hold_setup(GtkWidget * widget,
                                           GtkWidget * menu,
                                           GtkCallback func,
                                           GtkWidgetTapAndHoldFlags flags);

static GType hildon_grid_child_type(GtkContainer * container);


static void hildon_grid_set_property(GObject * object,
                                     guint prop_id,
                                     const GValue * value,
                                     GParamSpec * pspec);
static void hildon_grid_get_property(GObject * object,
                                     guint prop_id,
                                     GValue * value, GParamSpec * pspec);

static void _hildon_grid_set_empty_label(HildonGrid * grid,
                                         const gchar * empty_label);
static const gchar *_hildon_grid_get_empty_label(HildonGrid * grid);
static void _hildon_grid_set_n_columns(HildonGrid * grid, gint num_cols);
static void _hildon_grid_set_label_pos(HildonGrid * grid,
                                       HildonGridPositionType label_pos);
static void _hildon_grid_set_label_hspacing(HildonGrid * grid,
                                            gint hspacing);
static void _hildon_grid_set_label_vspacing(HildonGrid * grid,
                                            gint vspacing);
static void _hildon_grid_set_icon_size(HildonGrid * grid, gint icon_size);
static void _hildon_grid_set_emblem_size(HildonGrid * grid,
                                         gint emblem_size);
static void hildon_grid_destroy(GtkObject * self);
static void hildon_grid_finalize(GObject * object);

/* Signal handlers. */
static gboolean hildon_grid_button_pressed(GtkWidget * widget,
                                           GdkEventButton * event,
                                           gpointer data);
static gboolean hildon_grid_button_released(GtkWidget * widget,
                                            GdkEventButton * event,
                                            gpointer data);
static gboolean hildon_grid_key_pressed(GtkWidget * widget,
                                        GdkEventKey * event,
                                        gpointer data);
static gboolean hildon_grid_scrollbar_moved(GtkWidget * widget,
                                            gpointer data);
static gboolean hildon_grid_state_changed(GtkWidget * widget,
                                          GtkStateType state,
                                          gpointer data);

/* Other internal functions. */
static void get_style_properties(HildonGrid * grid);
static gint get_child_index(HildonGridPrivate * priv, GtkWidget * child);
static gint get_child_index_by_coord(HildonGridPrivate * priv,
                                     gint x, gint y);
static GtkWidget *get_child_by_index(HildonGridPrivate * priv, gint index);

static gboolean jump_scrollbar_to_focused(HildonGrid * grid);
static gboolean adjust_scrollbar_height(HildonGrid * grid);
static gboolean update_contents(HildonGrid * grid);
static void set_focus(HildonGrid * grid,
                      GtkWidget * widget, gboolean refresh_view);

/* Our fancy little marshaler. */
static void marshal_BOOLEAN__INT_INT_INT(GClosure * closure,
                                         GValue * return_value,
                                         guint n_param_values,
                                         const GValue * param_values,
                                         gpointer invocation_hint,
                                         gpointer marshal_data);


static GtkContainerClass *parent_class = NULL;
static guint grid_signals[LAST_SIGNAL] = { 0 };


GType hildon_grid_get_type(void)
{
    static GType grid_type = 0;

    if (!grid_type) {
        static const GTypeInfo grid_info = {
            sizeof(HildonGridClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_grid_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonGrid),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_grid_init,
        };
        grid_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                           "HildonGrid", &grid_info, 0);
    }

    return grid_type;
}



static void hildon_grid_class_init(HildonGridClass * klass)
{
    GObjectClass *gobject_class;
    GtkWidgetClass *widget_class;
    GtkContainerClass *container_class;

    widget_class = GTK_WIDGET_CLASS(klass);
    container_class = GTK_CONTAINER_CLASS(klass);
    gobject_class = G_OBJECT_CLASS(klass);

    parent_class = g_type_class_peek_parent(klass);

    g_type_class_add_private(klass, sizeof(HildonGridPrivate));

    GTK_OBJECT_CLASS(klass)->destroy = hildon_grid_destroy;
    gobject_class->finalize = hildon_grid_finalize;
    gobject_class->set_property = hildon_grid_set_property;
    gobject_class->get_property = hildon_grid_get_property;

    widget_class->realize = hildon_grid_realize;
    widget_class->unrealize = hildon_grid_unrealize;
    widget_class->map = hildon_grid_map;
    widget_class->unmap = hildon_grid_unmap;
    widget_class->expose_event = hildon_grid_expose;
    widget_class->size_request = hildon_grid_size_request;
    widget_class->size_allocate = hildon_grid_size_allocate;
    widget_class->tap_and_hold_setup = hildon_grid_tap_and_hold_setup;

    container_class->add = hildon_grid_add;
    container_class->remove = hildon_grid_remove;
    container_class->forall = hildon_grid_forall;
    container_class->child_type = hildon_grid_child_type;
    container_class->set_focus_child = hildon_grid_set_focus_child;

    g_object_class_install_property(gobject_class, PROP_EMPTY_LABEL,
        g_param_spec_string("empty_label",
                            "Empty label",
                            "Label to show when grid has no items",
                            _("No icons"),
                            G_PARAM_READWRITE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("item_width",
                          "Item width",
                          "Total width of an item",
                          1, G_MAXINT,
                          DEFAULT_WIDTH,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("item_height",
                          "Item height",
                          "Total height of an item",
                          1, G_MAXINT, DEFAULT_HEIGHT,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("item_hspacing",
                          "Item horizontal spacing",
                          "Horizontal space between items",
                          0, G_MAXINT, DEFAULT_HSPACING,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("item_vspacing",
                          "Item vertical spacing",
                          "Vertical space between items",
                          0, G_MAXINT, DEFAULT_VSPACING,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("label_hspacing",
                          "Horizontal label spacing",
                          "Horizontal margin between item and label",
                          0, G_MAXINT,
                          DEFAULT_LABEL_HSPACING,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("label_vspacing",
                          "Vertical label spacing",
                          "Vertical margin between item and label",
                          0, G_MAXINT,
                          DEFAULT_LABEL_VSPACING,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("n_columns",
                          "Columns",
                          "Number of columns",
                          0, G_MAXINT,
                          DEFAULT_N_COLUMNS,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("label_pos",
                          "Label position",
                          "Position of label related to the icon",
                          1, 2,
                          DEFAULT_LABEL_POS,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("icon_size",
                          "Icon size",
                          "Size of the icon in pixels",
                          1, G_MAXINT,
                          DEFAULT_ICON_SIZE,
                          G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_uint("emblem_size",
                          "Emblem size",
                          "Size of the emblem in pixels",
                          1, G_MAXINT,
                          DEFAULT_EMBLEM_SIZE,
                          G_PARAM_READABLE));

    /**
     * HildonGrid::activate-child:
     *
     * Emitted when a child (@HildonGridItem) is activated either by
     * tapping on it or by pressing enter.
     */
    grid_signals[ACTIVATE_CHILD] =
        g_signal_new("activate-child",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                     G_STRUCT_OFFSET(HildonGridClass, activate_child),
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1, HILDON_TYPE_GRID_ITEM);

    /**
     * HildonGrid::popup-context-menu:
     *
     * Emitted when popup-menu is supposed to open. Used for tap-and-hold.
     */
    grid_signals[POPUP_CONTEXT] =
        g_signal_new("popup-context-menu",
                     G_OBJECT_CLASS_TYPE(gobject_class),
                     G_SIGNAL_RUN_LAST,
                     G_STRUCT_OFFSET(HildonGridClass, popup_context_menu),
                     g_signal_accumulator_true_handled, NULL,
                     marshal_BOOLEAN__INT_INT_INT,
                     G_TYPE_BOOLEAN, 3,
                     G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
}



/*
 * _hildon_grid_set_empty_label:
 * @grid:           #HildonGrid
 * @empty_label:    New label
 *
 * Sets empty label.
 */
static void
_hildon_grid_set_empty_label(HildonGrid * grid, const gchar * empty_label)
{
    /* No need to worry about update -- label receives a signal for it. */
    gtk_label_set_label(GTK_LABEL(HILDON_GRID_GET_PRIVATE
                                  (grid)->empty_label),
                        empty_label == NULL ? "" : empty_label);
}

/*
 * _hildon_grid_get_empty_label:
 * @grid:   #HildonGrid
 *
 * Returns empty label. Label must not be modified nor freed.
 *
 * Return value: Label
 */
static const gchar *_hildon_grid_get_empty_label(HildonGrid * grid)
{
    return gtk_label_get_label(GTK_LABEL(HILDON_GRID_GET_PRIVATE
                                         (grid)->empty_label));
}

/*
 * _hildon_grid_set_n_columns:
 * @grid:       #HildonGrid
 * @columsn:    Number of columns
 *
 * Sets number of columns.
 */
static void _hildon_grid_set_n_columns(HildonGrid * grid, gint columns)
{
    GtkWidget *focus;
    HildonGridPrivate *priv;

    g_return_if_fail(HILDON_IS_GRID(grid));

    priv = HILDON_GRID_GET_PRIVATE(grid);

    if (priv->num_columns == columns) {
        return;
    }

    priv->num_columns = columns;

    /* If number of columns is defined (non-zero), use it immediately. */
    if (columns != 0) {
        priv->visible_cols = columns;
    }
    focus = GTK_CONTAINER(grid)->focus_child;
    gtk_widget_queue_resize(GTK_WIDGET(grid));
}

/*
 * _hildon_grid_set_label_pos:
 * @grid:       #HildonGrid
 * @label_pos:  Label position
 *
 * Sets icon label position.
 */
static void
_hildon_grid_set_label_pos(HildonGrid * grid,
                           HildonGridPositionType label_pos)
{
    GtkRequisition req;
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;

    priv = HILDON_GRID_GET_PRIVATE(grid);

    if (label_pos == priv->label_pos)
        return;

    /* gtknotebook doesn't check if we use valid values -- why should
       we?-) */

    priv->label_pos = label_pos;

    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        _hildon_grid_item_set_label_pos(HILDON_GRID_ITEM(child),
                                        label_pos);
        gtk_widget_size_request(child, &req);
        gtk_widget_queue_resize(child);
    }
}

/*
 * _hildon_grid_set_label_hspacing:
 * @grid:       #HildonGrid
 * @hspacing:   Horizontal spacing
 *
 * Sets horizontal spacing for label.
 */
static void
_hildon_grid_set_label_hspacing(HildonGrid * grid, gint hspacing)
{
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;

    priv = HILDON_GRID_GET_PRIVATE(grid);
    if (hspacing == priv->label_hspacing)
        return;

    priv->label_hspacing = hspacing;

    /* Update children. */
    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        _hildon_grid_item_set_label_spacing(HILDON_GRID_ITEM(child),
                                            priv->label_hspacing,
                                            priv->label_vspacing);
        if (priv->label_pos == HILDON_GRID_ITEM_LABEL_POS_RIGHT) {
            gtk_widget_queue_resize(child);
        }
    }
}


/*
 * _hildon_grid_set_label_vspacing:
 * @grid:       #HildonGrid
 * @hspacing:   Vertical spacing
 *
 * Sets vertical spacing for label.
 */
static void
_hildon_grid_set_label_vspacing(HildonGrid * grid, gint vspacing)
{
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;

    priv = HILDON_GRID_GET_PRIVATE(grid);
    if (vspacing == priv->label_vspacing)
        return;

    priv->label_vspacing = vspacing;

    /* Update children. */
    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        _hildon_grid_item_set_label_spacing(HILDON_GRID_ITEM(child),
                                            priv->label_hspacing,
                                            priv->label_vspacing);
        if (priv->label_pos == HILDON_GRID_ITEM_LABEL_POS_BOTTOM) {
            gtk_widget_queue_resize(child);
        }
    }
}

/*
 * _hildon_grid_set_icon_size:
 * @grid:       #HildonGrid
 * @icon_size:  Icon size
 *
 * Sets icon size (in pixels).
 */
static void _hildon_grid_set_icon_size(HildonGrid * grid, gint icon_size)
{
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;

    priv = HILDON_GRID_GET_PRIVATE(grid);

    if (icon_size == priv->icon_size)
        return;

    priv->icon_size = icon_size;

    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        _hildon_grid_item_set_icon_size(HILDON_GRID_ITEM(child),
                                        icon_size);
        gtk_widget_queue_resize(child);
    }
}


/*
 * _hildon_grid_set_emblem_size:
 * @grid:           #HildonGrid
 * @emblem_size:    Emblem size
 *
 * Sets emblem size (in pixels).
 */
static void
_hildon_grid_set_emblem_size(HildonGrid * grid, gint emblem_size)
{
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;

    priv = HILDON_GRID_GET_PRIVATE(grid);

    if (emblem_size == priv->emblem_size)
        return;

    priv->emblem_size = emblem_size;

    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        _hildon_grid_item_set_emblem_size(HILDON_GRID_ITEM(child),
                                          emblem_size);
        gtk_widget_queue_draw(child);
    }
}

static GType hildon_grid_child_type(GtkContainer * container)
{
    return GTK_TYPE_WIDGET;
}

static void hildon_grid_init(HildonGrid * grid)
{
    HildonGridPrivate *priv;

    priv = HILDON_GRID_GET_PRIVATE(grid);

    GTK_CONTAINER(grid)->focus_child = NULL;
    priv->focus_index = -1;

    priv->scrollbar = gtk_vscrollbar_new(NULL);
    priv->empty_label = gtk_label_new(_("No items"));
    priv->style = NULL;

    priv->area_height = 1;
    priv->area_rows = 1;
    priv->children = NULL;

    priv->first_index = 0;
    priv->click_x = 0;
    priv->click_y = 0;

    priv->visible_cols = DEFAULT_N_COLUMNS;
    priv->widget_width = 1;
    priv->widget_hspacing = 0;
    priv->widget_height = 1;
    priv->widget_vspacing = 0;

    priv->old_sb_pos = -1;

    gtk_widget_set_parent(priv->scrollbar, GTK_WIDGET(grid));
    gtk_widget_set_parent(priv->empty_label, GTK_WIDGET(grid));

    priv->last_button_event = GDK_NOTHING;

    GTK_WIDGET_SET_FLAGS(grid, GTK_NO_WINDOW);

    /* Signal for scrollbar. */
    g_signal_connect(G_OBJECT(priv->scrollbar), "value-changed",
                     G_CALLBACK(hildon_grid_scrollbar_moved), grid);

    /* Signal for key press. */
    GTK_WIDGET_SET_FLAGS(GTK_WIDGET(grid), GTK_CAN_FOCUS);
    gtk_widget_set_events(GTK_WIDGET(grid), GDK_KEY_PRESS_MASK);
    g_signal_connect(G_OBJECT(grid), "key-press-event",
                     G_CALLBACK(hildon_grid_key_pressed), grid);

    GTK_WIDGET_UNSET_FLAGS(priv->scrollbar, GTK_CAN_FOCUS);
    hildon_grid_set_style(grid, DEFAULT_STYLE);
}

/**
 * hildon_grid_new:
 *
 * Creates a new #HildonGrid.
 *
 * Return value: A new #HildonGrid
 */
GtkWidget *hildon_grid_new(void)
{

    HildonGrid *grid;

    grid = g_object_new(HILDON_TYPE_GRID, NULL);

    return GTK_WIDGET(grid);
}


static void hildon_grid_realize(GtkWidget * widget)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GdkWindowAttr attr;
    gint attr_mask;


    GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

    grid = HILDON_GRID(widget);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    /* Create GdkWindow for catching events. */
    attr.x = widget->allocation.x;
    attr.y = widget->allocation.y;
    attr.width = widget->allocation.width - priv->scrollbar_width;
    attr.height = widget->allocation.height;
    attr.window_type = GDK_WINDOW_CHILD;
    attr.event_mask = gtk_widget_get_events(widget)
        | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK;

    widget->window = gtk_widget_get_parent_window(widget);
    g_object_ref(widget->window);

    attr.wclass = GDK_INPUT_ONLY;
    attr_mask = GDK_WA_X | GDK_WA_Y;

    priv->event_window = gdk_window_new(widget->window, &attr, attr_mask);
    gdk_window_set_user_data(priv->event_window, widget);

    widget->style = gtk_style_attach(widget->style, widget->window);

    gtk_style_set_background(widget->style,
                             widget->window, GTK_STATE_NORMAL);

    g_signal_connect(G_OBJECT(widget),
                     "button-press-event",
                     G_CALLBACK(hildon_grid_button_pressed), grid);
    g_signal_connect(G_OBJECT(widget),
                     "button-release-event",
                     G_CALLBACK(hildon_grid_button_released), grid);
}


static void hildon_grid_unrealize(GtkWidget * widget)
{
    HildonGridPrivate *priv;

    priv = HILDON_GRID_GET_PRIVATE(HILDON_GRID(widget));

    if (priv->event_window != NULL) {
        gdk_window_set_user_data(priv->event_window, NULL);
        gdk_window_destroy(priv->event_window);
        priv->event_window = NULL;
    }

    if (GTK_WIDGET_CLASS(parent_class)->unrealize) {
        (*GTK_WIDGET_CLASS(parent_class)->unrealize) (widget);
    }
}



static void hildon_grid_map(GtkWidget * widget)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;

    g_return_if_fail(HILDON_IS_GRID(widget));

    if (!GTK_WIDGET_VISIBLE(widget))
        return;

    grid = HILDON_GRID(widget);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    (*GTK_WIDGET_CLASS(parent_class)->map) (widget);

    /* We don't really need the following... They never do anything I
       think. */
    if (priv->scrollbar != NULL && GTK_WIDGET_VISIBLE(priv->scrollbar)) {
        if (!GTK_WIDGET_MAPPED(priv->scrollbar)) {
            gtk_widget_map(priv->scrollbar);
        }
    }

    if (priv->empty_label != NULL &&
        GTK_WIDGET_VISIBLE(priv->empty_label)) {
        if (!GTK_WIDGET_MAPPED(priv->empty_label)) {
            gtk_widget_map(priv->empty_label);
        }
    }

    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        if (GTK_WIDGET_VISIBLE(child)) {
            if (!GTK_WIDGET_MAPPED(child)) {
                gtk_widget_map(child);
            }
        }
    }
    /* END OF don't really need */

    /* Also make event window visible. */
    gdk_window_show(priv->event_window);
}



static void hildon_grid_unmap(GtkWidget * widget)
{
    HildonGridPrivate *priv;

    priv = HILDON_GRID_GET_PRIVATE(HILDON_GRID(widget));

    if (priv->event_window != NULL) {
        gdk_window_hide(priv->event_window);
    }

    (*GTK_WIDGET_CLASS(parent_class)->unmap) (widget);
}



static gboolean
hildon_grid_expose(GtkWidget * widget, GdkEventExpose * event)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GtkContainer *container;
    GList *list;
    gint child_no;

    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(HILDON_IS_GRID(widget), FALSE);
    g_return_val_if_fail(event, FALSE);

    grid = HILDON_GRID(widget);
    priv = HILDON_GRID_GET_PRIVATE(grid);
    container = GTK_CONTAINER(grid);

    if (priv->children == NULL || g_list_length(priv->children) == 0) {
        if (priv->empty_label != NULL) {
            gtk_container_propagate_expose(container,
                                           priv->empty_label, event);
        }
        return FALSE;
    }

    /* Only expose visible children. */

    /* Jump over invisible. */
    for (list = priv->children, child_no = 0;
         list != NULL && child_no < priv->first_index;
         list = list->next, child_no++) {
        ;       /* Nothing here. */
    }

    for (; list != NULL && child_no < priv->first_index
         + priv->visible_cols * priv->area_rows; list = list->next) {
        gtk_container_propagate_expose(container,
                                       ((HildonGridChild *) list->data)
                                       ->widget, event);
    }

    /* Kludge to keep focused item focused. Enjoy. */
    if (container->focus_child != NULL
        && !GTK_WIDGET_HAS_FOCUS(container->focus_child)) {
        set_focus(grid, container->focus_child, FALSE);
    }
    if (priv->scrollbar_width > 0 && priv->scrollbar != NULL) {
        gtk_container_propagate_expose(container, priv->scrollbar, event);
    }

    return FALSE;
}


static void
hildon_grid_size_request(GtkWidget * widget, GtkRequisition * requisition)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;
    GtkRequisition req;

    g_return_if_fail(widget);
    g_return_if_fail(requisition);

    grid = HILDON_GRID(widget);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    /* Want as big as possible. */
    requisition->width = 0x7fff;        /* Largest possible gint16 */
    requisition->height = 0x7fff;       /* Largest possible gint16 */

    if (priv->children == NULL) {
        if (priv->empty_label != NULL &&
            GTK_WIDGET_VISIBLE(priv->empty_label)) {
            gtk_widget_size_request(priv->empty_label, &req);
        }
    }

    if (priv->scrollbar != NULL && GTK_WIDGET_VISIBLE(priv->scrollbar)) {
        gtk_widget_size_request(priv->scrollbar, &req);
    }

    for (list = priv->children; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        gtk_widget_size_request(child, &req);
    }
}

/*
 * hildon_grid_size_allocate:
 *
 * Supposedly called when size of grid changes and after view have moved so
 * that items need to be relocated.
 */
static void
hildon_grid_size_allocate(GtkWidget * widget, GtkAllocation * allocation)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *child;
    gint child_no;
    gint y_offset;

    GtkAllocation alloc;
    GtkRequisition req;
    GtkRequisition c_req;
    gint item_width, item_height;

    g_return_if_fail(widget);
    g_return_if_fail(allocation);

    grid = HILDON_GRID(widget);
    priv = HILDON_GRID_GET_PRIVATE(grid);
    widget->allocation = *allocation;


    get_style_properties(grid);

    /* First of all, make sure GdkWindow is over our widget. */
    if (priv->event_window != NULL) {
        gdk_window_move_resize(priv->event_window,
                               widget->allocation.x,
                               widget->allocation.y,
                               widget->allocation.width
                               - priv->scrollbar_width,
                               widget->allocation.height);
    }
    /* Show the label if there are no items. */
    if (priv->children == NULL) {
        /* 
         * We probably don't need this as scrollbar should be hidden when
         * removing items, but one can never be too sure...
         */
        if (priv->scrollbar != NULL &&
            GTK_WIDGET_VISIBLE(priv->scrollbar)) {
            priv->scrollbar_width = 0;
            gtk_widget_hide(priv->scrollbar);
        }

        /* Show label if creating one actually worked. */
        if (priv->empty_label != NULL) {
            gtk_widget_get_child_requisition(priv->empty_label, &req);

            /* ...for sure we must have a position for the label here... */
            alloc.x = allocation->x + GRID_LABEL_POS_PAD;
            alloc.y = allocation->y + GRID_LABEL_POS_PAD;
            alloc.width = MIN(req.width, allocation->width -
                              GRID_LABEL_POS_PAD);
            alloc.height = MIN(req.height, allocation->height -
                               GRID_LABEL_POS_PAD);

            /* Make sure we don't use negative values. */
            if (alloc.width < 0) {
                alloc.width = 0;
            }
            if (alloc.height < 0) {
                alloc.height = 0;
            }

            gtk_widget_size_allocate(priv->empty_label, &alloc);

            if (!GTK_WIDGET_VISIBLE(priv->empty_label)) {
                gtk_widget_show(priv->empty_label);
            }
        }

        return;
    }

    /* As we have some items, hide label if it was visible. */
    if (priv->empty_label != NULL &&
        GTK_WIDGET_VISIBLE(priv->empty_label)) {
        gtk_widget_hide(priv->empty_label);
    }

    /* Get item size. */
    item_width = priv->widget_width + priv->widget_hspacing;
    item_height = priv->widget_height + priv->widget_vspacing;

    priv->area_height = allocation->height;
    priv->area_rows = allocation->height / item_height;


    /* Adjust/show/hide scrollbar. */
    adjust_scrollbar_height(grid);


    /* Only one column? Lets use all the space we have. */
    if (priv->visible_cols == 1) {
        if (priv->scrollbar != NULL &&
            GTK_WIDGET_VISIBLE(priv->scrollbar)) {
            gtk_widget_get_child_requisition(priv->scrollbar, &req);
        } else {
            req.width = 0;
        }

        item_width = allocation->width - priv->scrollbar_width
            - priv->widget_hspacing - req.width;
        priv->widget_width = item_width;
    } else {
        priv->widget_width = priv->saved_width;
        priv->widget_hspacing = priv->saved_hspacing;
        item_width = priv->widget_width + priv->widget_hspacing;
    }

    priv->first_index =
        (int) gtk_range_get_value(GTK_RANGE(priv->scrollbar))
        / item_height * priv->visible_cols;

    /* Hide items before visible ones. */
    for (list = priv->children, child_no = 0;
         list != NULL && child_no < priv->first_index;
         list = list->next, child_no++) {
        child = ((HildonGridChild *) list->data)->widget;

        if (GTK_WIDGET_VISIBLE(child)) {
            gtk_widget_hide(child);
        }
    }

    /* Allocate visible items. */
    for (y_offset = priv->first_index / priv->visible_cols * item_height;
         list != NULL && child_no < priv->first_index
         + priv->area_rows * priv->visible_cols;
         list = list->next, child_no++) {
        child = ((HildonGridChild *) list->data)->widget;

        if (!GTK_WIDGET_VISIBLE(child)) {
            gtk_widget_show(child);
        }
        gtk_widget_get_child_requisition(child, &c_req);

        /* Don't update icons which are not visible... */
        alloc.y = (child_no / priv->visible_cols) * item_height +
            allocation->y - y_offset + priv->widget_vspacing;
        alloc.x = (child_no % priv->visible_cols) * item_width
            + allocation->x + priv->widget_hspacing;
        alloc.width = priv->widget_width;
        alloc.height = priv->widget_height;

        gtk_widget_size_allocate(child, &alloc);
    }

    /* Hide items after visible items. */
    for (; list != NULL; list = list->next) {
        child = ((HildonGridChild *) list->data)->widget;

        if (GTK_WIDGET_VISIBLE(child)) {
            gtk_widget_hide(child);
        }
    }
}



/**
 * hildon_grid_add:
 * @container:  Container (#HildonGrid) to add HildonGridItem into.
 * @widget:     #GtkWidget (#HildonGridItem) to add.
 *
 * Adds a new HildonGridItem into HildonGrid.
 */
static void hildon_grid_add(GtkContainer * container, GtkWidget * widget)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    HildonGridChild *child;


    g_return_if_fail(HILDON_IS_GRID(container));
    g_return_if_fail(HILDON_IS_GRID_ITEM(widget));

    grid = HILDON_GRID(container);
    priv = HILDON_GRID_GET_PRIVATE(HILDON_GRID(grid));
    GTK_WIDGET_SET_FLAGS(widget, GTK_NO_WINDOW);

    child = g_new(HildonGridChild, 1);
    if (child == NULL) {
        g_critical("no memory for child - not adding");
        return;
    }
    child->widget = widget;

    _hildon_grid_item_set_label_pos(HILDON_GRID_ITEM(widget),
                                    priv->label_pos);
    _hildon_grid_item_set_label_spacing(HILDON_GRID_ITEM(widget),
                                        priv->label_hspacing,
                                        priv->label_vspacing);
    _hildon_grid_item_set_icon_size(HILDON_GRID_ITEM(widget),
                                    priv->icon_size);
    _hildon_grid_item_set_emblem_size(HILDON_GRID_ITEM(widget),
                                      priv->emblem_size);
    priv->children = g_list_append(priv->children, child);
    gtk_widget_set_parent(widget, GTK_WIDGET(grid));

    /* Property changes (child's set_sensitive) */
    g_signal_connect_after(G_OBJECT(widget), "state-changed",
                           G_CALLBACK(hildon_grid_state_changed), grid);

    /* Matches both empty grid and all-dimmed grid. */
    if (GTK_CONTAINER(grid)->focus_child == NULL)
        set_focus(grid, widget, TRUE);

    /* 
     * If item was added in visible area, relocate items. Otherwise update
     * scrollbar and see if items need relocating.
     */
    if (g_list_length(priv->children) < priv->first_index
        + priv->area_rows * priv->visible_cols) {
        gtk_widget_queue_resize(GTK_WIDGET(grid));
    } else {
        gboolean updated;

        updated = adjust_scrollbar_height(grid);
        /* Basically this other test is useless -- shouldn't need to jump. 
         */
        updated |= jump_scrollbar_to_focused(grid);

        if (updated) {
            gtk_widget_queue_resize(GTK_WIDGET(grid));
        }
    }
}

/**
 * hildon_grid_remove:
 * @container:  Container (#HildonGrid) to remove #HildonGridItem from.
 * @widget:     Widget (#HildonGridItem) to be removed.
 *
 * Removes HildonGridItem from HildonGrid.
 */
static void
hildon_grid_remove(GtkContainer * container, GtkWidget * widget)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    HildonGridChild *child;
    GtkWidget *child_widget;
    GList *list;
    gint index, old_index;
    gboolean deleted;
    gboolean updated;

    g_return_if_fail(HILDON_IS_GRID(container));
    g_return_if_fail(HILDON_IS_GRID_ITEM(widget));

    grid = HILDON_GRID(container);
    priv = HILDON_GRID_GET_PRIVATE(container);

    old_index = priv->focus_index;
    updated = GTK_WIDGET_VISIBLE(widget);

    for (list = priv->children, index = 0, deleted = FALSE;
         list != NULL; list = list->next, index++) {
        child = (HildonGridChild *) list->data;
        child_widget = child->widget;
        if (child_widget == widget) {
            gtk_widget_unparent(child_widget);
            priv->children = g_list_remove_link(priv->children, list);
            g_list_free(list);
            g_free(child);

            deleted = TRUE;

            break;
        }
    }

    if (!deleted) {
        g_warning("tried to remove unexisting item");
        return;
    }

    /* Move focus somewhere. */
    if (old_index == index) {
        if (old_index == g_list_length(priv->children)) {
            if (index == 0) {
                set_focus(grid, NULL, TRUE);
            } else {
                set_focus(grid,
                          get_child_by_index(priv, old_index - 1), TRUE);
            }
        } else {
            set_focus(grid, get_child_by_index(priv, old_index), TRUE);
        }
    } else {
        set_focus(grid, GTK_CONTAINER(grid)->focus_child, TRUE);
    }

    updated |= adjust_scrollbar_height(grid);
    updated |= jump_scrollbar_to_focused(grid);

    if (updated) {
        gtk_widget_queue_resize(GTK_WIDGET(grid));
    }
}

/**
 * hildon_grid_set_focus_child:
 * @container:  HildonGrid
 * @widget:     HildonGridItem
 *
 * Sets focus.
 */
static void
hildon_grid_set_focus_child(GtkContainer * container, GtkWidget * widget)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;

    g_return_if_fail(HILDON_IS_GRID(container));
    g_return_if_fail(HILDON_IS_GRID_ITEM(widget) || widget == NULL);

    grid = HILDON_GRID(container);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    if (GTK_CONTAINER(grid)->focus_child == widget || widget == NULL)
        return;

    set_focus(grid, widget, TRUE);
}



static void
set_focus(HildonGrid * grid, GtkWidget * widget, gboolean refresh_view)
{
    HildonGridPrivate *priv;
    GtkContainer *container;
    gboolean view_updated;


    priv = HILDON_GRID_GET_PRIVATE(grid);
    container = GTK_CONTAINER(grid);

    if (widget == NULL && container->focus_child != NULL)
        GTK_WIDGET_UNSET_FLAGS(container->focus_child, GTK_HAS_FOCUS);

    GTK_CONTAINER(grid)->focus_child = widget;
    if (widget == NULL) {
        priv->focus_index = -1;
        return;
    }

    priv->focus_index = get_child_index(priv, widget);

    gtk_widget_grab_focus(widget);

    if (refresh_view) {
        view_updated = jump_scrollbar_to_focused(grid);
    } else {
        view_updated = FALSE;
    }

    if (view_updated) {
        /* Don't just queue it, let's do it now! */
        hildon_grid_size_allocate(GTK_WIDGET(grid),
                                  &GTK_WIDGET(grid)->allocation);
    }
}

static void
hildon_grid_forall(GtkContainer * container,
                   gboolean include_internals,
                   GtkCallback callback, gpointer callback_data)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GList *list;

    g_return_if_fail(container);
    g_return_if_fail(callback);

    grid = HILDON_GRID(container);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    if (include_internals) {
        if (priv->scrollbar != NULL) {
            (*callback) (priv->scrollbar, callback_data);
        }
        if (priv->empty_label != NULL) {
            (*callback) (priv->empty_label, callback_data);
        }
    }

    for (list = priv->children; list != NULL; list = list->next) {
        (*callback) (((HildonGridChild *) list->data)->widget,
                     callback_data);
    }
}

static void hildon_grid_destroy(GtkObject * self)
{
    HildonGridPrivate *priv;

    g_return_if_fail(self != NULL);
    g_return_if_fail(HILDON_IS_GRID(self));

    priv = HILDON_GRID_GET_PRIVATE(self);

    if (GTK_WIDGET(self)->window != NULL) {
        g_object_unref(G_OBJECT(GTK_WIDGET(self)->window));
    }

    gtk_container_forall(GTK_CONTAINER(self),
                         (GtkCallback) gtk_object_ref, NULL);
    gtk_container_forall(GTK_CONTAINER(self),
                         (GtkCallback) gtk_widget_unparent, NULL);

    GTK_OBJECT_CLASS(parent_class)->destroy(self);
}

static void hildon_grid_finalize(GObject * object)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;

    grid = HILDON_GRID(object);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    gtk_container_forall(GTK_CONTAINER(object),
                         (GtkCallback) gtk_object_unref, NULL);

    if (priv->style != NULL) {
        g_free(priv->style);
    }
    if (G_OBJECT_CLASS(parent_class)->finalize) {
        G_OBJECT_CLASS(parent_class)->finalize(object);
    }
}

/*
 * hildon_grid_key_pressed:
 * @widget: Widget where we get the signal from
 * @event:  EventKey
 * @data:   #HildonGrid
 *
 * Handle user key press (keyboard navigation).
 *
 * And here's how it works if some items are dimmed (moving to right):
 * . . . .      . . # .     . 2 # .     . # . .
 * . 1 # 2      . 1 # #     1 # # #     1 # # #
 * . . .        . . 2       . . .       . 2 .
 *
 * '.' = item, '#' = dimmed item, 
 * '1' = starting position, '2' = final position
 *
 * ...although only the first example is implemented right now.
 *
 * Return value: Signal handled
 */
static gboolean
hildon_grid_key_pressed(GtkWidget * widget,
                        GdkEventKey * event, gpointer data)
{
    GtkAdjustment *adjustment;
    GtkContainer *container;
    GtkWidget *new_focus;
    HildonGrid *grid;
    HildonGridPrivate *priv;
    gboolean shift;
    gint keyval;
    gint x, y;
    gint focus_index;
    gint child_count, child_rows;
    gint t;
    gint addition, max_add;

    g_return_val_if_fail(widget, FALSE);

    grid = HILDON_GRID(data);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    /* 
     * If focus was never lost (which is supposedly a bug), we could just
     * see if an item is focused - if not, there's nothing else to focus...
     */

    /* No items? */
    if (priv->children == NULL || g_list_length(priv->children) == 0)
        return FALSE;

    /* Focused item is dimmed? */
    /* If we have no focus, allow non-existing focus to move... */
    container = GTK_CONTAINER(grid);
    if (container->focus_child != NULL
        && !GTK_WIDGET_IS_SENSITIVE(container->focus_child)) {
        return FALSE;
    }
    /* At the moment we don't want to do anything here if alt or control
       or MODX is pressed, so return now. Shift + TAB are accepted (from
       hildon-table-grid) And right now modifiers do not make any
       difference... */

    /* Said somewhere that "foo = a == b" is not desirable. */
    if (event->state & GDK_SHIFT_MASK) {
        shift = TRUE;
    } else {
        shift = FALSE;
    }

    keyval = event->keyval;
    if (gtk_widget_get_default_direction() == GTK_TEXT_DIR_RTL) {
        switch (event->keyval) {
        case GDK_Left:
            keyval = GDK_Right;
            break;
        case GDK_KP_Left:
            keyval = GDK_KP_Right;
            break;
        case GDK_Right:
            keyval = GDK_Left;
            break;
        case GDK_KP_Right:
            keyval = GDK_KP_Left;
            break;
        }
    }

    child_count = g_list_length(priv->children);
    child_rows = (child_count - 1) / priv->visible_cols + 1;

    if (priv->focus_index != -1) {
        x = priv->focus_index % priv->visible_cols;
        y = priv->focus_index / priv->visible_cols;
    } else {
        x = y = 0;
    }

    switch (keyval) {
    case GDK_KP_Page_Up:
    case GDK_Page_Up:
        if (priv->first_index == 0) {
            if (priv->focus_index == 0) {
                return TRUE;
            }
            set_focus(grid, get_child_by_index(priv, 0), TRUE);
            return TRUE;
        }

        t = MAX(priv->first_index / priv->visible_cols
                - priv->area_rows, 0);
        adjustment = gtk_range_get_adjustment(GTK_RANGE(priv->scrollbar));
        adjustment->value = (gdouble) (t * (priv->widget_height
                                            + priv->widget_vspacing));
        gtk_range_set_adjustment(GTK_RANGE(priv->scrollbar), adjustment);
        gtk_widget_queue_draw(priv->scrollbar);
        update_contents(grid);

        /* Want to update now. */
        hildon_grid_size_allocate(GTK_WIDGET(grid),
                                  &GTK_WIDGET(grid)->allocation);

        return TRUE;
        break;

    case GDK_KP_Page_Down:
    case GDK_Page_Down:
        if (priv->first_index / priv->visible_cols
            == child_rows - priv->area_rows) {
            if (priv->focus_index == child_count - 1) {
                return TRUE;
            }
            set_focus(grid, get_child_by_index(priv, child_count - 1),
                      TRUE);
            return TRUE;
        }

        t = MIN(priv->first_index / priv->visible_cols
                + priv->area_rows, child_rows - priv->area_rows);
        adjustment = gtk_range_get_adjustment(GTK_RANGE(priv->scrollbar));
        adjustment->value = (gdouble) (t * (priv->widget_height
                                            + priv->widget_vspacing));
        gtk_range_set_adjustment(GTK_RANGE(priv->scrollbar), adjustment);
        gtk_widget_queue_draw(priv->scrollbar);
        update_contents(grid);

        /* Want to update now. */
        hildon_grid_size_allocate(GTK_WIDGET(grid),
                                  &GTK_WIDGET(grid)->allocation);

        return TRUE;
        break;

    case GDK_KP_Up:
    case GDK_Up:
        if (y <= 0) {
            return TRUE;
        }
        addition = -priv->visible_cols;
        max_add = y;
        y--;
        break;

    case GDK_KP_Down:
    case GDK_Down:
        if (y >= (child_count - 1) / priv->visible_cols) {
            return TRUE;
        }
        t = child_count % priv->visible_cols;
        if (t == 0) {
            t = priv->visible_cols;
        }
        if (y == (child_count - 1) / priv->visible_cols - 1 && x >= t) {
            x = t - 1;
        }
        y++;
        addition = priv->visible_cols;
        max_add = child_rows - y;
        break;

    case GDK_KP_Left:
    case GDK_Left:
        if (x <= 0) {
            return TRUE;
        }
        addition = -1;
        max_add = x;
        x--;
        break;

    case GDK_KP_Right:
    case GDK_Right:
        if (x >= priv->visible_cols - 1) {
            return TRUE;
        }
        if (y == 0 && x >= child_count - 1) {
            return TRUE;
        }
        x++;
        addition = 1;
        max_add = priv->visible_cols - x;
        if (y * priv->visible_cols + x == child_count) {
            y--;
        }
        break;
    case GDK_KP_Enter:
    case GDK_Return:
        hildon_grid_activate_child(grid,
                                   HILDON_GRID_ITEM
                                   (GTK_CONTAINER(grid)->focus_child));
        return TRUE;
        break;
    default:
        return FALSE;
        break;
    }

    focus_index = y * priv->visible_cols + x;
    new_focus = get_child_by_index(priv, focus_index);

    while (new_focus != NULL &&
           focus_index < child_count && !GTK_WIDGET_SENSITIVE(new_focus)) {
        max_add--;

        if (max_add == 0) {
            return TRUE;
        }
        focus_index += addition;
        new_focus = get_child_by_index(priv, focus_index);
    }

    if (new_focus != NULL) {
        set_focus(grid, new_focus, TRUE);
    }
    return TRUE;
}


/*
 * hildon_grid_button_pressed:
 * @widget: Widget where signal is coming from
 * @event:  #EventButton
 * @data:   #HildonGrid
 *
 * Handle mouse button press.
 *
 * Return value: Signal handled
 */
static gboolean
hildon_grid_button_pressed(GtkWidget * widget,
                           GdkEventButton * event, gpointer data)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GtkWidget *child;
    int child_no;

/* Watch out for double/triple click press events */

    if (event->type == GDK_2BUTTON_PRESS ||
        event->type == GDK_3BUTTON_PRESS) {
        grid = HILDON_GRID(data);
        priv = HILDON_GRID_GET_PRIVATE(grid);
        priv->last_button_event = event->type;
        return FALSE;
    }

    grid = HILDON_GRID(data);
    priv = HILDON_GRID_GET_PRIVATE(grid);

    priv->last_button_event = event->type;

    if (event->type != GDK_BUTTON_PRESS)
        return FALSE;


    child_no = get_child_index_by_coord(priv, event->x, event->y);

    if (child_no == -1 || child_no >= g_list_length(priv->children))
        return FALSE;

    child = get_child_by_index(priv, child_no);
    if (!GTK_WIDGET_IS_SENSITIVE(child))
        return FALSE;

    set_focus(grid, child, TRUE);

    priv->click_x = event->x;
    priv->click_y = event->y;

    return FALSE;
}

/*
 * hildon_grid_button_released:
 * @widget: Widget the signal is coming from
 * @event:  #EventButton
 * @data:   #HildonGrid
 *
 * Handle mouse button release.
 *
 * Return value: Signal handled
 */
static gboolean
hildon_grid_button_released(GtkWidget * widget,
                            GdkEventButton * event, gpointer data)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GtkWidget *child;
    int child_no;

    grid = HILDON_GRID(data);
    priv = HILDON_GRID_GET_PRIVATE(grid);

/* In case of double/triple click, silently ignore the release event */

    if (priv->last_button_event == GDK_2BUTTON_PRESS ||
        priv->last_button_event == GDK_3BUTTON_PRESS) {
        priv->last_button_event = event->type;
        return FALSE;
    }

    child_no = get_child_index_by_coord(priv, event->x, event->y);

    if (child_no == -1 || child_no >= g_list_length(priv->children)) {
        return FALSE;
    }
    child = get_child_by_index(priv, child_no);
    if (!GTK_WIDGET_IS_SENSITIVE(child)) {
        return FALSE;
    }
    if (abs(priv->click_x - event->x) >= DRAG_SENSITIVITY
        && abs(priv->click_y - event->y) >= DRAG_SENSITIVITY) {
        return FALSE;
    }
    set_focus(grid, child, TRUE);
    priv->last_button_event = event->type;
    hildon_grid_activate_child(grid, HILDON_GRID_ITEM(child));

    return FALSE;
}

/*
 * hildon_grid_scrollbar_moved:
 * @widget: Widget which sent the signal
 * @data:   #HildonGrid
 *
 * Update HildonGrid contents when scrollbar is moved.
 *
 * Return value: Signal handeld
 */
static gboolean
hildon_grid_scrollbar_moved(GtkWidget * widget, gpointer data)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    gboolean updated = FALSE;

    grid = HILDON_GRID(data);
    priv = HILDON_GRID_GET_PRIVATE(grid);
    updated = update_contents(grid);

    /* 
     * Kludge-time!
     *
     * If grid changes focus while dragging scrollbar and pointer leaves
     * scrollbar, focus is moved to prev_focus... This prevents that.
     */
    gtk_window_set_prev_focus_widget(GTK_WINDOW
                                     (gtk_widget_get_toplevel(widget)),
                                     GTK_CONTAINER(grid)->focus_child);

    if (updated)
        /* Don't just queue it, let's do it now! */
        hildon_grid_size_allocate(GTK_WIDGET(grid),
                                  &GTK_WIDGET(grid)->allocation);

    return TRUE;
}


/*
 * update_contents:
 * @grid:   #HildonGrid
 *
 * Update the view if scrollbar has moved so that first visible row
 * should've changed. Returns true if location actually changed.
 *
 * Return value: Content changed
 */
static gboolean update_contents(HildonGrid * grid)
{
    HildonGridPrivate *priv;
    gint new_row;

    priv = HILDON_GRID_GET_PRIVATE(grid);
    new_row = (int) gtk_range_get_value(GTK_RANGE(priv->scrollbar))
        / (priv->widget_height + priv->widget_vspacing);

    if (new_row != priv->old_sb_pos) {
        priv->old_sb_pos = new_row;
        priv->first_index = new_row * priv->visible_cols;

        return TRUE;
    }
    return FALSE;
}

/*
 * jump_scrollbar_to_focused:
 * @grid:   #HildonGrid
 *
 * Moves scrollbar position so that focused item will be shown 
 * in visible area.
 * Returns TRUE if visible position of widgets have changed.
 *
 * Return value: Content changed
 */
static gboolean jump_scrollbar_to_focused(HildonGrid * grid)
{
    HildonGridPrivate *priv;
    GtkAdjustment *adjustment;
    gint child_count;
    gint empty_grids;
    gint new_row;

    priv = HILDON_GRID_GET_PRIVATE(grid);
    /* If we don't have scrollbar, let the focus be broken, too. */
    g_return_val_if_fail(priv->scrollbar != NULL, FALSE);

    /* Make sure "first widget" is something sensible. */
    priv->first_index = priv->first_index / priv->visible_cols
        * priv->visible_cols;

    child_count = g_list_length(priv->children);
    empty_grids = priv->visible_cols * priv->area_rows - child_count
        + priv->first_index;

    if (priv->focus_index < priv->first_index) {
        new_row = priv->focus_index / priv->visible_cols;
    } else if (priv->focus_index >= priv->first_index
               + priv->area_rows * priv->visible_cols) {
        new_row = priv->focus_index / priv->visible_cols -
            priv->area_rows + 1;
    } else if (empty_grids >= priv->visible_cols) {
        new_row = ((child_count - 1) / priv->visible_cols + 1)
            - priv->area_rows;
        if (new_row < 0) {
            new_row = 0;
        }
    } else {
        return FALSE;
    }

    /* Move scrollbar accordingly. */
    adjustment = gtk_range_get_adjustment(GTK_RANGE(priv->scrollbar));
    adjustment->value = (gdouble) (new_row * (priv->widget_height
                                              + priv->widget_vspacing));
    gtk_range_set_adjustment(GTK_RANGE(priv->scrollbar), adjustment);
    priv->first_index = new_row * priv->visible_cols;
    priv->old_sb_pos = new_row;

    gtk_widget_queue_draw(priv->scrollbar);

    return TRUE;
}


/*
 * adjust_scrollbar_height:
 * @grid:   HildonGridPrivate
 *
 * Return value: View should change
 *
 * Adjust scrollbar according the #HildonGrid contents. 
 * Show/hide scrollbar if
 * appropriate. Also sets priv->first_index.
 */
static gboolean adjust_scrollbar_height(HildonGrid * grid)
{
    HildonGridPrivate *priv;
    GtkRequisition req;
    GtkAdjustment *adj;
    GtkAllocation alloc;
    GtkAllocation *gridalloc;
    gint old_upper;
    gint need_rows;
    gint need_pixels;
    gboolean updated;

    priv = HILDON_GRID_GET_PRIVATE(grid);
    g_return_val_if_fail(priv->scrollbar != NULL, FALSE);

    updated = FALSE;
    gridalloc = &GTK_WIDGET(grid)->allocation;

    /* See if we need scrollbar at all. */
    if (priv->num_columns == 0) {
        priv->visible_cols = MAX(1,
                                 gridalloc->width /
                                 (priv->widget_width +
                                  priv->widget_hspacing));
    } else {
        priv->visible_cols = MAX(1, priv->num_columns);
    }

    if (g_list_length(priv->children) != 0) {
        need_rows = (g_list_length(priv->children) - 1)
            / priv->visible_cols + 1;
    } else {
        need_rows = 0;
    }

    if (need_rows <= priv->area_rows) {
        updated = priv->first_index != 0;

        priv->first_index = 0;
        if (GTK_WIDGET_VISIBLE(priv->scrollbar)) {
            gtk_widget_hide(priv->scrollbar);
            updated = TRUE;
        }

        return updated;
    }

    /* All right then, we need scrollbar. Place scrollbar on the screen. */
    gtk_widget_get_child_requisition(priv->scrollbar, &req);
    priv->scrollbar_width = req.width;

    alloc.width = req.width;
    alloc.height = gridalloc->height;
    alloc.x = gridalloc->width - req.width + gridalloc->x;
    alloc.y = gridalloc->y;
    gtk_widget_size_allocate(priv->scrollbar, &alloc);

    if (!GTK_WIDGET_VISIBLE(priv->scrollbar)) {
        gtk_widget_show(priv->scrollbar);
        updated = TRUE;
    }


    /* 
     * If we're fitting as many columns as possible, 
     * recount number of visible
     * columns and recalculate number of needed rows accordingly.
     */
    if (priv->num_columns == 0) {
        priv->visible_cols = MAX(1,
                                 (gridalloc->width -
                                  priv->scrollbar_width) /
                                 (priv->widget_width +
                                  priv->widget_hspacing));
        need_rows = (g_list_length(priv->children) - 1) /
            priv->visible_cols + 1;
    }

    need_pixels =
        need_rows * (priv->widget_height + priv->widget_vspacing);

    /* Once we know how much space we need, update the scrollbar. */
    adj = gtk_range_get_adjustment(GTK_RANGE(priv->scrollbar));
    old_upper = (int) adj->upper;
    adj->lower = 0.0;
    adj->upper = (gdouble) need_pixels;
    adj->step_increment = (gdouble) (priv->widget_height
                                     + priv->widget_vspacing);
    adj->page_increment =
        (gdouble) (priv->area_rows *
                   (priv->widget_height + priv->widget_vspacing));
    adj->page_size =
        (gdouble) (priv->area_height /
                   (priv->widget_height +
                    priv->widget_vspacing) * (priv->widget_height +
                                              priv->widget_vspacing));

    /* Also update position if needed to show focused item. */

    gtk_range_set_adjustment(GTK_RANGE(priv->scrollbar), adj);

    /* Then set first_index. */
    priv->first_index = (int) adj->value / (priv->widget_height +
                                            priv->widget_vspacing) *
        priv->visible_cols;

    /* Finally, ask Gtk to redraw the scrollbar. */
    if (old_upper != (int) adj->upper) {
        gtk_widget_queue_draw(priv->scrollbar);
    }
    return updated;
}

/*
 * get_child_index_by_coord:
 * @priv:   HildonGridPrivate
 * @x:      X-coordinate
 * @y:      Y-coordinate
 *
 * Returns index of child at given coordinates, -1 if no child.
 *
 * Return value: Index
 */
static gint
get_child_index_by_coord(HildonGridPrivate * priv, gint x, gint y)
{
    int t;

    if (x >
        priv->visible_cols * (priv->widget_width + priv->widget_hspacing)
        || x % (priv->widget_width + priv->widget_hspacing)
        < priv->widget_hspacing
        || y % (priv->widget_height + priv->widget_vspacing)
        < priv->widget_vspacing) {
        return -1;
    }

    t = y / (priv->widget_height + priv->widget_vspacing) *
        priv->visible_cols
        + x / (priv->widget_width + priv->widget_hspacing)
        + priv->first_index;

    if (t >= priv->first_index + priv->area_rows * priv->visible_cols
        || t >= g_list_length(priv->children) || t < 0) {
        return -1;
    }
    return t;
}

/*
 * get_child_by_index:
 * @priv:   HildonGridPrivate
 * @index:  Index of child
 *
 * Returns child that is #th in HildonGrid or NULL if child was not found
 * among the children.
 *
 * Return value: GtkWidget
 */
static GtkWidget *get_child_by_index(HildonGridPrivate * priv, gint index)
{
    GList *list;
    int i = 0;

    if (index >= g_list_length(priv->children) || index < 0) {
        return NULL;
    }
    for (list = priv->children, i = 0; list != NULL;
         list = list->next, i++) {
        if (index == i) {
            return ((HildonGridChild *) list->data)->widget;
        }
    }

    g_warning("no such child");
    return NULL;
}

/*
 * get_child_index:
 * @priv:   HildonGridPrivate
 * @child:  #GtkWidget to look for
 *
 * Returns index of a child or -1 if child was not found among the
 * children.
 *
 * Return value: Index
 */
static gint get_child_index(HildonGridPrivate * priv, GtkWidget * child)
{
    GList *list;
    gint index;

    if (child == NULL)
        return -1;

    for (list = priv->children, index = 0;
         list != NULL; list = list->next, index++) {
        if (((HildonGridChild *) list->data)->widget == child) {
            return index;
        }
    }

    g_warning("no such child");
    return -1;
}


/**
 * hildon_grid_activate_child:
 * @grid:   #HildonGrid
 * @item:   #HildonGridItem
 *
 * Emits a signal to tell HildonGridItem was actiavated.
 */
void hildon_grid_activate_child(HildonGrid * grid, HildonGridItem * item)
{
    g_return_if_fail(HILDON_IS_GRID(grid));

    g_signal_emit(grid, grid_signals[ACTIVATE_CHILD], 0, item);
}



/**
 * hildon_grid_set_style:
 * @grid:       #HildonGrid
 * @style_name: Style name
 *
 * Sets style. Setting style sets widget size, spacing, label position,
 * number of columns, and icon size.
 */
void hildon_grid_set_style(HildonGrid * grid, const gchar * style_name)
{
    HildonGridPrivate *priv;

    g_return_if_fail(HILDON_IS_GRID(grid));


    priv = HILDON_GRID_GET_PRIVATE(grid);
    if (priv->style != NULL) {
        g_free((gpointer) priv->style);
    }
    if (style_name != NULL) {
        priv->style = g_strdup(style_name);
    } else {
        priv->style = NULL;
    }

    gtk_widget_set_name(GTK_WIDGET(grid), style_name);
    get_style_properties(grid);

    gtk_widget_queue_resize(GTK_WIDGET(grid));
}

/**
 * hildon_grid_get_style:
 * @grid:   #HildonGrid
 *
 * Returns the name of style currently used in HildonGrid.
 *
 * Return value: Style name
 */
const gchar *hildon_grid_get_style(HildonGrid * grid)
{
    g_return_val_if_fail(HILDON_IS_GRID(grid), NULL);

    return gtk_widget_get_name(GTK_WIDGET(grid));
}

/*
 * For registering signal handler...
 */
static void
marshal_BOOLEAN__INT_INT_INT(GClosure * closure,
                             GValue * return_value,
                             guint n_param_values,
                             const GValue * param_values,
                             gpointer invocation_hint,
                             gpointer marshal_data)
{
    typedef gboolean(*marshal_func_BOOLEAN__INT_INT_INT)
     (gpointer data1, gint arg_1, gint arg_2, gint arg_3, gpointer data2);
    register marshal_func_BOOLEAN__INT_INT_INT callback;
    register GCClosure *cc = (GCClosure *) closure;
    register gpointer data1, data2;
    gboolean v_return;

    g_return_if_fail(return_value != NULL);
    g_return_if_fail(n_param_values == 4);

    if (G_CCLOSURE_SWAP_DATA(closure)) {
        data1 = closure->data;
        data2 = g_value_peek_pointer(param_values + 0);
    } else {
        data1 = g_value_peek_pointer(param_values + 0);
        data2 = closure->data;
    }
    callback = (marshal_func_BOOLEAN__INT_INT_INT)
        (marshal_data ? marshal_data : cc->callback);

    /* Doing this is somewhat ugly but it works. */
    v_return = callback(data1,
                        (int) (param_values + 1)->data[0].v_pointer,
                        (int) (param_values + 2)->data[0].v_pointer,
                        (int) (param_values + 3)->data[0].v_pointer,
                        data2);
    /* 
     * And here's the original code from gtkmarshal.c:
     * 
     v_return = callback (data1,
     g_marshal_value_peek_int (param_values + 1),
     g_marshal_value_peek_int (param_values + 2),
     g_marshal_value_peek_int (param_values + 3),
     data2);
     */

    g_value_set_boolean(return_value, v_return);
}

/*
 * get_style_properties:
 * @grid:   #HildonGrid
 *
 * Gets widget size and other stuff from gtkrc. If some stuff changed, let
 * children know this, too.
 */
static void get_style_properties(HildonGrid * grid)
{
    gint icon_size;
    gint num_columns;
    HildonGridPositionType label_pos;
    gint label_hspacing;
    gint label_vspacing;
    gint emblem_size;

    HildonGridPrivate *priv;

    g_return_if_fail(HILDON_IS_GRID(grid));

    priv = HILDON_GRID_GET_PRIVATE(grid);
    gtk_widget_style_get(GTK_WIDGET(grid),
                         "item_width", &priv->widget_width,
                         "item_hspacing", &priv->widget_hspacing,
                         "item_height", &priv->widget_height,
                         "item_vspacing", &priv->widget_vspacing,
                         "icon_size", &icon_size,
                         "n_columns", &num_columns,
                         "label_pos", &label_pos,
                         "label_hspacing", &label_hspacing,
                         "label_vspacing", &label_vspacing,
                         "emblem_size", &emblem_size, NULL);

    priv->saved_width = priv->widget_width;
    priv->saved_height = priv->widget_height;
    priv->saved_hspacing = priv->widget_hspacing;
    priv->saved_vspacing = priv->widget_vspacing;

    _hildon_grid_set_icon_size(grid, icon_size);
    _hildon_grid_set_n_columns(grid, num_columns);
    _hildon_grid_set_label_pos(grid, label_pos);
    _hildon_grid_set_label_hspacing(grid, label_hspacing);
    _hildon_grid_set_label_vspacing(grid, label_vspacing);
    _hildon_grid_set_emblem_size(grid, emblem_size);
}



/**
 * hildon_grid_set_scrollbar_pos:
 * @grid:           #HildonGrid
 * @scrollbar_pos:  new position (in pixels)
 *
 * Sets view (scrollbar) to specified position.
 */
void hildon_grid_set_scrollbar_pos(HildonGrid * grid, gint scrollbar_pos)
{
    HildonGridPrivate *priv;
    GtkAdjustment *adjustment;

    g_return_if_fail(HILDON_IS_GRID(grid));

    priv = HILDON_GRID_GET_PRIVATE(grid);
    adjustment = gtk_range_get_adjustment(GTK_RANGE(priv->scrollbar));
    adjustment->value = (gdouble) scrollbar_pos;

    gtk_range_set_adjustment(GTK_RANGE(priv->scrollbar), adjustment);

    /* If grid isn't drawable, updating anything could mess up focus. */
    if (!GTK_WIDGET_DRAWABLE(GTK_WIDGET(grid)))
        return;

    update_contents(grid);
}

/**
 * hildon_grid_get_scrollbar_pos:
 * @grid:   #HildonGrid
 *
 * Returns position of scrollbar (in pixels).
 *
 * Return value: Scrollbar position
 */
gint hildon_grid_get_scrollbar_pos(HildonGrid * grid)
{
    GtkAdjustment *adjustment;

    g_return_val_if_fail(HILDON_IS_GRID(grid), -1);

    adjustment = gtk_range_get_adjustment(GTK_RANGE
                                          (HILDON_GRID_GET_PRIVATE
                                           (grid)->scrollbar));
    return (int) adjustment->value;
}

static void
hildon_grid_set_property(GObject * object,
                         guint prop_id,
                         const GValue * value, GParamSpec * pspec)
{
    HildonGrid *grid;

    grid = HILDON_GRID(object);

    switch (prop_id) {
    case PROP_EMPTY_LABEL:
        _hildon_grid_set_empty_label(grid, g_value_get_string(value));
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_grid_get_property(GObject * object,
                         guint prop_id, GValue * value, GParamSpec * pspec)
{
    switch (prop_id) {
    case PROP_EMPTY_LABEL:
        g_value_set_string(value,
                           _hildon_grid_get_empty_label(HILDON_GRID
                                                        (object)));
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean
hildon_grid_state_changed(GtkWidget * widget,
                          GtkStateType state, gpointer data)
{
    HildonGrid *grid;
    HildonGridPrivate *priv;
    GList *list;
    GtkWidget *current;
    GtkWidget *prev_focusable, *next_focusable;
    gboolean found_old;

    g_return_val_if_fail(HILDON_IS_GRID(data), FALSE);
    g_return_val_if_fail(HILDON_IS_GRID_ITEM(widget), FALSE);

    grid = HILDON_GRID(data);
    priv = HILDON_GRID_GET_PRIVATE(grid);


    if (GTK_WIDGET_IS_SENSITIVE(widget))
        return FALSE;

    prev_focusable = next_focusable = NULL;
    found_old = FALSE;

    for (list = priv->children; list != NULL; list = list->next) {
        current = ((HildonGridChild *) list->data)->widget;

        if (GTK_WIDGET_IS_SENSITIVE(current)) {
            if (found_old) {
                next_focusable = current;
                break;
            } else {
                prev_focusable = current;
            }
        } else if (current == widget) {
            found_old = TRUE;
        }
    }

    if (next_focusable == NULL) {
        next_focusable = prev_focusable;
    }

    gtk_container_set_focus_child(GTK_CONTAINER(grid), next_focusable);

    return FALSE;
}



static void
hildon_grid_tap_and_hold_setup(GtkWidget * widget,
                               GtkWidget * menu,
                               GtkCallback func,
                               GtkWidgetTapAndHoldFlags flags)
{
    g_return_if_fail(HILDON_IS_GRID(widget) && GTK_IS_MENU(menu));

    parent_class->parent_class.tap_and_hold_setup
        (widget, menu, func, flags | GTK_TAP_AND_HOLD_NO_INTERNALS);
}
