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
 * @file hildon-hvolumebar.c
 *
 * This file contains API implementation for Hildon horizontal volumebar.
 */


/* Horizontal volumebar subclass */

#include <gtk/gtktoolbar.h>
#include "hildon-hvolumebar.h"
#include "hildon-volumebar.h"
#include "hildon-volumebar-range.h"
#include "hildon-volumebar-private.h"

/* Defines for normal version of HVolumebar */
#define DEFAULT_TBUTTON_WIDTH  26
#define DEFAULT_TBUTTON_HEIGHT 26
#define MINIMUM_BAR_WIDTH     147
#define DEFAULT_BAR_HEIGHT     58
#define DEFAULT_ENDING_SIZE    20

#define VERTICAL_MUTE_GAP      16
#define HORIZONTAL_MUTE_GAP     6

#define TOOL_DEFAULT_TBUTTON_WIDTH  26
#define TOOL_DEFAULT_TBUTTON_HEIGHT 26
#define TOOL_MINIMUM_BAR_WIDTH     121
#define TOOL_DEFAULT_BAR_HEIGHT     40
#define TOOL_DEFAULT_ENDING_SIZE     0
#define TOOL_VERTICAL_MUTE_GAP ((TOOL_DEFAULT_BAR_HEIGHT - TOOL_DEFAULT_TBUTTON_HEIGHT) / 2)

static HildonVolumebarClass *parent_class;
static void hvolumebar_class_init(HildonHVolumebarClass * klass);
static void hvolumebar_init(HildonHVolumebar * hvolumebar);
static void
hildon_hvolumebar_mute(GtkWidget * widget, HildonHVolumebar * self);

/* new */
static gboolean hildon_hvolumebar_expose(GtkWidget * widget,
                                         GdkEventExpose * event);
static void hildon_hvolumebar_size_request(GtkWidget * widget,
                                           GtkRequisition * requisition);
static void hildon_hvolumebar_size_allocate(GtkWidget * widget,
                                            GtkAllocation * allocation);
static void hildon_hvolumebar_map(GtkWidget * widget);


GType hildon_hvolumebar_get_type(void)
{
    static GType type = 0;

    if (!type) {
        static const GTypeInfo info = {
            sizeof(HildonHVolumebarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hvolumebar_class_init,     /* class_init */
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonHVolumebar),
            0,
            (GInstanceInitFunc) hvolumebar_init,
        };
        type = g_type_register_static(HILDON_TYPE_VOLUMEBAR,
                                      "HildonHVolumebar", &info, 0);
    }
    return type;
}


static void hvolumebar_class_init(HildonHVolumebarClass * klass)
{
    GtkWidgetClass *volumebar_class = GTK_WIDGET_CLASS(klass);

    parent_class = g_type_class_peek_parent(klass);

    volumebar_class->size_request = hildon_hvolumebar_size_request;
    volumebar_class->size_allocate = hildon_hvolumebar_size_allocate;
    volumebar_class->map = hildon_hvolumebar_map;
    volumebar_class->expose_event = hildon_hvolumebar_expose;
}


static void hvolumebar_init(HildonHVolumebar * hvolumebar)
{
    HildonVolumebar *v_ptr = HILDON_VOLUMEBAR(hvolumebar);
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(hvolumebar);

    priv->ownorientation = GTK_ORIENTATION_HORIZONTAL;
    priv->orientation = GTK_ORIENTATION_HORIZONTAL;

    priv->volumebar =
        HILDON_VOLUMEBAR_RANGE(hildon_volumebar_range_new
                               (GTK_ORIENTATION_HORIZONTAL));

    GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(hvolumebar), GTK_CAN_FOCUS);

    gtk_widget_set_parent(GTK_WIDGET(priv->tbutton), GTK_WIDGET(v_ptr));
    gtk_widget_set_parent(GTK_WIDGET(priv->volumebar), GTK_WIDGET(v_ptr));

    gtk_scale_set_draw_value(GTK_SCALE(priv->volumebar), FALSE);

    /* Signals */
    g_signal_connect_swapped(G_OBJECT(priv->volumebar), "value-changed",
                             G_CALLBACK(hildon_volumebar_level_change),
                             hvolumebar);
    g_signal_connect(G_OBJECT(priv->tbutton), "toggled",
                     G_CALLBACK(hildon_hvolumebar_mute), hvolumebar);

    /*gtk_widget_show(GTK_WIDGET(priv->tbutton));*/
    gtk_widget_show(GTK_WIDGET(priv->volumebar));
}

/**
 * hildon_hvolumebar_new:
 *
 * Creates a new #HildonHVolumebar widget.
 *
 * Returns: a new #HildonHVolumebar.
 */
GtkWidget *hildon_hvolumebar_new()
{
    return GTK_WIDGET(g_object_new(HILDON_TYPE_HVOLUMEBAR, NULL));
}

static void hildon_hvolumebar_map(GtkWidget * widget)
{
    HildonVolumebar *vbar;
    HildonVolumebarPrivate *priv;
    GtkWidget *parent;

    vbar = HILDON_VOLUMEBAR(widget);
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(vbar);

    parent = gtk_widget_get_ancestor(GTK_WIDGET(widget), GTK_TYPE_TOOLBAR);

    if (parent)
        priv->is_toolbar = TRUE;

    GTK_WIDGET_CLASS(parent_class)->map(widget);
}

static gboolean hildon_hvolumebar_expose(GtkWidget * widget,
                                         GdkEventExpose * event)
{
    if (GTK_WIDGET_DRAWABLE(widget)) {
        gtk_paint_box(widget->style, widget->window,
                      GTK_WIDGET_STATE(widget), GTK_SHADOW_OUT,
                      NULL, widget, "background",
                      widget->allocation.x,
                      widget->allocation.y,
                      widget->allocation.width,
                      widget->allocation.height);

        (*GTK_WIDGET_CLASS(parent_class)->expose_event) (widget, event);
    }

    return FALSE;
}

static void
hildon_hvolumebar_size_request(GtkWidget * widget,
                               GtkRequisition * requisition)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(HILDON_VOLUMEBAR(widget));

    requisition->width = (priv->is_toolbar
                          ? TOOL_MINIMUM_BAR_WIDTH
                          : MINIMUM_BAR_WIDTH);
    requisition->height = (priv->is_toolbar
                           ? TOOL_DEFAULT_BAR_HEIGHT
                           : DEFAULT_BAR_HEIGHT);
}

static void
hildon_hvolumebar_size_allocate(GtkWidget * widget,
                                GtkAllocation * allocation)
{
    HildonVolumebar *vbar;
    HildonVolumebarPrivate *priv;
    GtkAllocation button_allocation, range_allocation;

    vbar = HILDON_VOLUMEBAR(widget);
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(vbar);
    
    button_allocation.x = 0;
    button_allocation.width = 0;
    
    if (priv->is_toolbar && allocation->height > TOOL_DEFAULT_BAR_HEIGHT) {
        allocation->y += (allocation->height - TOOL_DEFAULT_BAR_HEIGHT) / 2;
        allocation->height = TOOL_DEFAULT_BAR_HEIGHT;
    }

    if (!priv->is_toolbar && allocation->height > DEFAULT_BAR_HEIGHT) {
        allocation->y += (allocation->height - DEFAULT_BAR_HEIGHT) / 2;
        allocation->height = DEFAULT_BAR_HEIGHT;
    }

    widget->allocation = *allocation;

    if (priv->tbutton && GTK_WIDGET_VISIBLE(priv->tbutton)) {

        if (priv->is_toolbar) {
            button_allocation.x = allocation->x;
            button_allocation.y = allocation->y + TOOL_VERTICAL_MUTE_GAP;
            button_allocation.width = TOOL_DEFAULT_TBUTTON_WIDTH;
            button_allocation.height = TOOL_DEFAULT_TBUTTON_HEIGHT;
        } else {
            button_allocation.x = allocation->x + DEFAULT_ENDING_SIZE;
            button_allocation.y = allocation->y + VERTICAL_MUTE_GAP;
            button_allocation.width = DEFAULT_TBUTTON_WIDTH;
            button_allocation.height = DEFAULT_TBUTTON_HEIGHT;
        }
        gtk_widget_size_allocate(GTK_WIDGET(priv->tbutton),
                                 &button_allocation);
    }
    if (priv->volumebar && GTK_WIDGET_VISIBLE(priv->volumebar)) {

        range_allocation.y = allocation->y;
        
        if (priv->tbutton && GTK_WIDGET_VISIBLE(priv->tbutton))
        {
            range_allocation.x = button_allocation.x
                                 + button_allocation.width
                                 + HORIZONTAL_MUTE_GAP;
        
            if (priv->is_toolbar) 
            {
                range_allocation.width = MAX(0,
                                           allocation->width
                                           - 2 * TOOL_DEFAULT_ENDING_SIZE
                                           - TOOL_DEFAULT_TBUTTON_WIDTH
                                           - HORIZONTAL_MUTE_GAP);
                                         
                range_allocation.height = TOOL_DEFAULT_BAR_HEIGHT;
            
            } 
            
            else 
            {
                range_allocation.width = MAX(0,
                                             allocation->width
                                             - 2 * DEFAULT_ENDING_SIZE
                                             - DEFAULT_TBUTTON_WIDTH
                                             - HORIZONTAL_MUTE_GAP);

                range_allocation.height = DEFAULT_BAR_HEIGHT;
            }
        
        }
        
        else
        {
            if (priv->is_toolbar) 
            {
                range_allocation.x = allocation->x;
                
                range_allocation.width = MAX(0,
                                        allocation->width
                                        - 2 * TOOL_DEFAULT_ENDING_SIZE );
                                         
                range_allocation.height = TOOL_DEFAULT_BAR_HEIGHT;
            
            } 
            
            else 
            {
                
                range_allocation.x = allocation->x + DEFAULT_ENDING_SIZE;
                
                range_allocation.width = MAX(0,
                                             allocation->width
                                             - 2 * DEFAULT_ENDING_SIZE );

                range_allocation.height = DEFAULT_BAR_HEIGHT;
            }
        }

        gtk_widget_size_allocate(GTK_WIDGET(priv->volumebar),
                                 &range_allocation);
    }
}

static void
hildon_hvolumebar_mute(GtkWidget * widget, HildonHVolumebar * self)
{
    g_signal_emit_by_name(GTK_WIDGET(self), "mute_toggled");
}
