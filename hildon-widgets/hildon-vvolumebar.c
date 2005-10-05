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

/**
 * @file hildon-vvolumebar.c
 *
 * This file contains the implementation of HildonVVolumebar.
 * This widget is a subclass of HildonVolumebar and implements
 * horizontal version of Volumebar.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include "hildon-vvolumebar.h"
#include "hildon-volumebar.h"
#include "hildon-volumebar-range.h"
#include "hildon-volumebar-private.h"

#define DEFAULT_BAR_WIDTH               58
#define MINIMUM_BAR_HEIGHT             165
#define DEFAULT_VERTICAL_TBUTTON_WIDTH  26
#define DEFAULT_VERTICAL_TBUTTON_HEIGHT 26
#define DEFAULT_ENDING_SIZE             20
#define HORIZONTAL_MUTE_GAP             16
#define VERTICAL_MUTE_GAP                6

static HildonVolumebarClass *parent_class;
static void vvolumebar_class_init(HildonVVolumebarClass * klass);
static void vvolumebar_init(HildonVVolumebar * vvolumebar);
static void hildon_vvolumebar_mute(GtkWidget * widget,
                                   HildonVVolumebar * self);
static gboolean hildon_vvolumebar_expose(GtkWidget * widget,
                                         GdkEventExpose * event);
static void hildon_vvolumebar_size_request(GtkWidget * widget,
                                           GtkRequisition * requisition);
static void hildon_vvolumebar_size_allocate(GtkWidget * widget,
                                            GtkAllocation * allocation);

GType hildon_vvolumebar_get_type(void)
{
    static GType type = 0;

    if (!type) {
        static const GTypeInfo info = {
            sizeof(HildonVVolumebarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) vvolumebar_class_init,     /* class_init */
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonVVolumebar),
            0,
            (GInstanceInitFunc) vvolumebar_init,
        };
        type =
            g_type_register_static(HILDON_TYPE_VOLUMEBAR,
                                   "HildonVVolumebar", &info, 0);
    }
    return type;
}

static void vvolumebar_class_init(HildonVVolumebarClass * klass)
{
    GtkWidgetClass *volumebar_class = GTK_WIDGET_CLASS(klass);

    parent_class = g_type_class_peek_parent(klass);

    volumebar_class->size_request = hildon_vvolumebar_size_request;
    volumebar_class->size_allocate = hildon_vvolumebar_size_allocate;
    volumebar_class->expose_event = hildon_vvolumebar_expose;
}

static void vvolumebar_init(HildonVVolumebar * vvolumebar)
{
    HildonVolumebar *v_ptr = HILDON_VOLUMEBAR(vvolumebar);
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(vvolumebar);

    priv->ownorientation = GTK_ORIENTATION_VERTICAL;
    priv->orientation = GTK_ORIENTATION_VERTICAL;

    priv->volumebar =
        HILDON_VOLUMEBAR_RANGE(hildon_volumebar_range_new
                               (GTK_ORIENTATION_VERTICAL));

    GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(vvolumebar), GTK_CAN_FOCUS);

    gtk_widget_set_parent(GTK_WIDGET(priv->tbutton), GTK_WIDGET(v_ptr));
    gtk_widget_set_parent(GTK_WIDGET(priv->volumebar), GTK_WIDGET(v_ptr));

    gtk_scale_set_draw_value(GTK_SCALE(priv->volumebar), FALSE);

    /* Signals */
    g_signal_connect_swapped(G_OBJECT(priv->volumebar), "value-changed",
                             G_CALLBACK(hildon_volumebar_level_change),
                             vvolumebar);
    g_signal_connect(G_OBJECT(priv->tbutton), "toggled",
                     G_CALLBACK(hildon_vvolumebar_mute), vvolumebar);
    /* end here */

    gtk_widget_show(GTK_WIDGET(priv->volumebar));
}

/**
 * hildon_vvolumebar_new:
 *
 * Creates a new #HildonVVolumebar widget.
 *
 * Returns: a new #HildonVVolumebar.
 */
GtkWidget *hildon_vvolumebar_new()
{
    return GTK_WIDGET(g_object_new(HILDON_TYPE_VVOLUMEBAR, NULL));
}

static gboolean hildon_vvolumebar_expose(GtkWidget * widget,
                                         GdkEventExpose * event)
{
    HildonVolumebarPrivate *priv;
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(HILDON_VOLUMEBAR(widget));
    
    if (GTK_WIDGET_DRAWABLE(widget)) {
        gtk_paint_box(widget->style, widget->window,
                      GTK_WIDGET_STATE(priv->volumebar), GTK_SHADOW_OUT,
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
hildon_vvolumebar_size_request(GtkWidget * widget,
                               GtkRequisition * requisition)
{
    requisition->height = MINIMUM_BAR_HEIGHT;
    requisition->width = DEFAULT_BAR_WIDTH;
}

static void
hildon_vvolumebar_size_allocate(GtkWidget * widget,
                                GtkAllocation * allocation)
{
    HildonVolumebar *vbar;
    HildonVolumebarPrivate *priv;

    GtkAllocation range_allocation, button_allocation;

    vbar = HILDON_VOLUMEBAR(widget);
    priv = HILDON_VOLUMEBAR_GET_PRIVATE(vbar);

    if (allocation->width > DEFAULT_BAR_WIDTH) {
        allocation->x +=
	  (allocation->width - DEFAULT_BAR_WIDTH) / 2;
        allocation->width = DEFAULT_BAR_WIDTH;
    }

    widget->allocation = *allocation;

    if (priv->volumebar && GTK_WIDGET_VISIBLE(priv->volumebar)) {
        range_allocation.x = allocation->x;
        range_allocation.y = allocation->y + DEFAULT_ENDING_SIZE;

        range_allocation.width = DEFAULT_BAR_WIDTH;
        
        if (priv->tbutton && GTK_WIDGET_VISIBLE(priv->tbutton))
        {
            range_allocation.height = MAX(0,
                                          allocation->height
                                          - 2 * DEFAULT_ENDING_SIZE
                                          - DEFAULT_VERTICAL_TBUTTON_HEIGHT
                                          - VERTICAL_MUTE_GAP);
        }
        
        else
        {
            range_allocation.height = MAX(0,
                                          allocation->height
                                          - 2 * DEFAULT_ENDING_SIZE);
        }
        
        gtk_widget_size_allocate(GTK_WIDGET(priv->volumebar),
                                 &range_allocation);
    }
    
    if (priv->tbutton && GTK_WIDGET_VISIBLE(priv->tbutton)) {
        button_allocation.x = allocation->x + HORIZONTAL_MUTE_GAP;
        button_allocation.y = allocation->y + allocation->height -
                              VERTICAL_MUTE_GAP - 2 * DEFAULT_ENDING_SIZE;
        button_allocation.width = DEFAULT_VERTICAL_TBUTTON_WIDTH;
        button_allocation.height = DEFAULT_VERTICAL_TBUTTON_HEIGHT;
        gtk_widget_size_allocate(GTK_WIDGET(priv->tbutton),
                                 &button_allocation);
    }
}

static void
hildon_vvolumebar_mute(GtkWidget * widget, HildonVVolumebar * self)
{
    g_signal_emit_by_name(GTK_WIDGET(self), "mute_toggled");
}
