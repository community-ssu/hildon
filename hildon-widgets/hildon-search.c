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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hildon-search.h"

#include <gtk/gtkimage.h>
#include <gtk/gtkiconfactory.h>
#include <gtk/gtkstock.h>
#include <hildon-widgets/hildon-defines.h>

#include <libintl.h>

#define _(String) dgettext(PACKAGE, String)

#define SEARCH_FIELD_HEIGHT 54
#define SEARCH_CHILD_HEIGHT 30

#define SEARCH_ICON_WIDTH  30   /* search stock icon size */
#define SEARCH_ICON_HEIGHT 30
#define SEARCH_ICON_RB 12
#define SEARCH_ICON_TB 0

#define SEARCH_TOP_BORDER    12
#define SEARCH_BOTTOM_BORDER 12
#define SEARCH_LEFT_BORDER   20
#define SEARCH_RIGHT_BORDER  20

#define HILDON_SEARCH_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     HILDON_TYPE_SEARCH, HildonSearchPrivate));

typedef struct _HildonSearchPrivate HildonSearchPrivate;

static void hildon_search_class_init(HildonSearchClass * class);
static void hildon_search_init(HildonSearch * search);
static void hildon_search_forall(GtkContainer * container,
                                 gboolean include_internals,
                                 GtkCallback callback,
                                 gpointer callback_data);
static gboolean hildon_search_expose(GtkWidget * widget,
                                     GdkEventExpose * event);
static void hildon_search_size_request(GtkWidget * widget,
                                       GtkRequisition * requisition);
static void hildon_search_size_allocate(GtkWidget * widget,
                                        GtkAllocation * allocation);
static void hildon_search_finalize(GObject * object);
static void hildon_search_destroy(GtkObject * object);
static void
hildon_search_set_property(GObject * object, guint property_id,
                           const GValue * value, GParamSpec * pspec);
static void
hildon_search_get_property(GObject * object, guint property_id,
                           GValue * value, GParamSpec * pspec);

static GtkBinClass *parent_class = NULL;

enum {
    PROP_ICON_NAME = 1
};

struct _HildonSearchPrivate {
    GtkWidget *image;
    gchar *icon_name;
};

GType hildon_search_get_type(void)
{
    static GType hildon_search_type = 0;

    if (!hildon_search_type) {
        static const GTypeInfo hildon_search_info = {
            sizeof(HildonSearchClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_search_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonSearch),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_search_init,
            NULL,       /* value_table */
        };

        hildon_search_type = g_type_register_static(GTK_TYPE_BIN,
                                                    "HildonSearch",
                                                    &hildon_search_info,
                                                    0);
    }

    return hildon_search_type;
}

/* private functions */
static void hildon_search_class_init(HildonSearchClass * class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(class);
    GObjectClass *object_class = G_OBJECT_CLASS(class);

    g_type_class_add_private(class, sizeof(HildonSearchPrivate));

    parent_class = g_type_class_peek_parent(class);

    object_class->finalize = hildon_search_finalize;
    object_class->set_property = hildon_search_set_property;
    object_class->get_property = hildon_search_get_property;
    widget_class->expose_event = hildon_search_expose;
    widget_class->size_request = hildon_search_size_request;
    widget_class->size_allocate = hildon_search_size_allocate;
    GTK_OBJECT_CLASS(class)->destroy = hildon_search_destroy;
    container_class->forall = hildon_search_forall;

    g_object_class_install_property(object_class, PROP_ICON_NAME,
        g_param_spec_string("stock-icon-name",
                            "Stock icon Name",
                            "Set the stock icon name",
                            NULL,
                            G_PARAM_READWRITE));
}

static void
hildon_search_set_property(GObject * object, guint property_id,
                           const GValue * value, GParamSpec * pspec)
{
    HildonSearchPrivate *priv = HILDON_SEARCH_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_ICON_NAME:
    {
        const gchar *image_text;

        g_free(priv->icon_name);

        /* This can be NULL in the first time */
        if (priv->image) {        
          gtk_widget_unparent(priv->image);
        }

        image_text = g_value_get_string(value);
        if (!image_text)
          image_text = GTK_STOCK_FIND;

        priv->icon_name = g_strdup(image_text);
        priv->image = gtk_image_new_from_stock(priv->icon_name,
                                               HILDON_ICON_SIZE_WIDG);
        gtk_widget_set_parent(priv->image, GTK_WIDGET(object));
        gtk_widget_show(priv->image);
    }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}


static void
hildon_search_get_property(GObject * object, guint property_id,
                           GValue * value, GParamSpec * pspec)
{
    HildonSearchPrivate *priv = HILDON_SEARCH_GET_PRIVATE(object);

    switch (property_id) {
    case PROP_ICON_NAME:
        g_value_set_string(value, priv->icon_name);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void hildon_search_init(HildonSearch * search)
{
    HildonSearchPrivate *priv = HILDON_SEARCH_GET_PRIVATE(search);

    priv->image = NULL;
    priv->icon_name = NULL;
}

static void hildon_search_finalize(GObject * object)
{
    HildonSearchPrivate *priv = HILDON_SEARCH_GET_PRIVATE(object);

    if (priv->icon_name) {
        g_free(priv->icon_name);
    }
    if (G_OBJECT_CLASS(parent_class)->finalize) {
        G_OBJECT_CLASS(parent_class)->finalize(object);
    }
}

static void hildon_search_destroy(GtkObject * object)
{
    HildonSearchPrivate *priv = HILDON_SEARCH_GET_PRIVATE(object);

    if (priv->image) {
        gtk_widget_unparent(priv->image);
        priv->image = NULL;
    }
}

/**
 * hildon_search_new:
 * @image : Name of the stock icon or NULL to be used 
 * as the optional icon in the Search dialog.
 * 
 * Creates a new Search widget without or with the optional image 
 * left of the search widget.
 * 
 * Return value: #GtkWidget pointer of #HildonSearch widget.
 */
GtkWidget *hildon_search_new(const gchar * stockicon)
{
    HildonSearch *search = g_object_new(HILDON_TYPE_SEARCH,
                                        "stock-icon-name", stockicon,
                                        NULL);

    return GTK_WIDGET(search);
}

static void hildon_search_forall(GtkContainer * container,
                                 gboolean include_internals,
                                 GtkCallback callback,
                                 gpointer callback_data)
{
    GtkBin *bin = GTK_BIN(container);
    HildonSearch *search = HILDON_SEARCH(container);
    HildonSearchPrivate *priv = HILDON_SEARCH_GET_PRIVATE(search);

    if (bin->child) {
        (*callback) (bin->child, callback_data);
    }

    if (include_internals) {
      (*callback) (priv->image, callback_data);
    }
}


/**
 * hildon_search_set_image_stock:
 * @search: A pointer of #HildonSearch widget to work on
 * @stock_icon: Name of a stock icon to use as the optional image 
 * in the Search widget. This will change the previous one if it is set.
 *
 * Sets or changes the optional image left of the Search widget.
 */
void hildon_search_set_image_stock(HildonSearch * search,
                                   const gchar * stock_icon)
{
    g_return_if_fail(HILDON_IS_SEARCH(search));

    g_object_set(search, "stock-icon-name", stock_icon, NULL);
}

static gboolean hildon_search_expose(GtkWidget * widget,
                                     GdkEventExpose * event)
{
    if (GTK_WIDGET_DRAWABLE(widget)) {
        gint x, y, width, height;

        x = widget->allocation.x;
        y = widget->allocation.y;
        width = widget->allocation.width;
        height = widget->allocation.height;

        gtk_paint_box(widget->style, widget->window,
                      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                      NULL, widget, "search", x, y, width, height);

        (*GTK_WIDGET_CLASS(parent_class)->expose_event) (widget, event);
    }
    return FALSE;
}

static void hildon_search_size_request(GtkWidget * widget,
                                       GtkRequisition * requisition)
{
    HildonSearch *search;
    HildonSearchPrivate *priv;
    GtkBin *bin;
    GtkRequisition child_requisition;

    search = HILDON_SEARCH(widget);
    bin = GTK_BIN(search);
    priv = HILDON_SEARCH_GET_PRIVATE(search);

    requisition->width = GTK_CONTAINER(search)->border_width * 2;
    requisition->height = GTK_CONTAINER(search)->border_width * 2;

    gtk_widget_size_request(priv->image, &child_requisition);

    if (bin->child && GTK_WIDGET_VISIBLE(bin->child)) {
        gtk_widget_size_request(bin->child, &child_requisition);
        requisition->width += child_requisition.width;
    }

    requisition->width += SEARCH_LEFT_BORDER + SEARCH_RIGHT_BORDER +
        SEARCH_ICON_WIDTH + SEARCH_ICON_RB;
    requisition->height += SEARCH_CHILD_HEIGHT +
        SEARCH_TOP_BORDER + SEARCH_BOTTOM_BORDER;
}

static void hildon_search_size_allocate(GtkWidget * widget,
                                        GtkAllocation * allocation)
{
    HildonSearch *search;
    HildonSearchPrivate *priv;
    GtkBin *bin;
    GtkAllocation child_allocation;

    widget->allocation = *allocation;

    if (widget->allocation.height > SEARCH_FIELD_HEIGHT)
    {
      widget->allocation.y += (widget->allocation.height - SEARCH_FIELD_HEIGHT) / 2;
      widget->allocation.height = SEARCH_FIELD_HEIGHT;
    }

    search = HILDON_SEARCH(widget);
    bin = GTK_BIN(search);
    priv = HILDON_SEARCH_GET_PRIVATE(search);

    child_allocation.x = widget->allocation.x + SEARCH_LEFT_BORDER;
    child_allocation.y = widget->allocation.y + SEARCH_TOP_BORDER;
    child_allocation.width = SEARCH_ICON_WIDTH;
    child_allocation.height = SEARCH_ICON_HEIGHT;

    gtk_widget_size_allocate(priv->image, &child_allocation);

    if (bin->child && GTK_WIDGET_VISIBLE(bin->child)) {
        child_allocation.x = widget->allocation.x + SEARCH_LEFT_BORDER +
            SEARCH_ICON_WIDTH + SEARCH_ICON_RB;
        child_allocation.y = widget->allocation.y + SEARCH_TOP_BORDER;
        child_allocation.width = MAX(1, (gint) widget->allocation.width -
                                     (SEARCH_LEFT_BORDER +
                                      SEARCH_RIGHT_BORDER +
                                      SEARCH_ICON_WIDTH + SEARCH_ICON_RB));
        child_allocation.height =
            MAX(1,
                (gint) widget->allocation.height - (SEARCH_TOP_BORDER +
                                             SEARCH_BOTTOM_BORDER));

        gtk_widget_size_allocate(bin->child, &child_allocation);
    }
}
