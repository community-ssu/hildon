/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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
 * SECTION:hildon-vvolumebar
 * @short_description: A widget that displays a vertical volume bar.
 * @see_also: #HildonVolumebar, #HildonHVolumebar
 *
 * #HildonVVolumebar is a subclass of #HildonVolumebar.  It displays a
 * vertical volume bar that allows increasing or decreasing volume
 * within a predefined range, and muting when users click the mute icon.
 * 
 * Here is an example that creates a vertical volume bar and connects
 * both its signals.
 * <example>
 * <title>HildonVVolumebar example</title>
 * <programlisting>
 * GtkWidget *volbar = hildon_vvolumebar_new ();
 * g_signal_connect (G_OBJECT (volbar), "mute_toggled", G_CALLBACK (mute_toggle), NULL);
 * g_signal_connect (G_OBJECT (volbar), "level_changed", G_CALLBACK (level_change), NULL);
 * </programlisting>
 * </example>
 */

#undef                                          HILDON_DISABLE_DEPRECATED

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-vvolumebar.h"
#include                                        "hildon-volumebar-range.h"
#include                                        "hildon-volumebar-private.h"

/* Volume bar */
#define                                         DEFAULT_BAR_WIDTH 60

#define                                         MINIMUM_BAR_HEIGHT 165

/* Toggle button */

#define                                         DEFAULT_VERTICAL_TBUTTON_WIDTH 60

#define                                         DEFAULT_VERTICAL_TBUTTON_HEIGHT 60

#define                                         DEFAULT_ENDING_SIZE 0

/* Gap to leave for mute button */

#define                                         HORIZONTAL_MUTE_GAP 0

#define                                         VERTICAL_MUTE_GAP 0

static HildonVolumebarClass*                    parent_class;

static void 
hildon_vvolumebar_class_init                    (HildonVVolumebarClass * klass);

static void 
hildon_vvolumebar_init                          (HildonVVolumebar * vvolumebar);

static gboolean
hildon_vvolumebar_expose                        (GtkWidget * widget,
                                                 GdkEventExpose * event);

static void 
hildon_vvolumebar_size_request                  (GtkWidget * widget,
                                                 GtkRequisition * requisition);

static void
hildon_vvolumebar_size_allocate                 (GtkWidget * widget,
                                                 GtkAllocation * allocation);

/**
 * hildon_vvolumebar_get_type:
 *
 * Initializes and returns the type of a hildon vvolumebar.
 *
 * Returns: GType of #HildonVVolumebar
 */
GType G_GNUC_CONST
hildon_vvolumebar_get_type                      (void)
{
    static GType type = 0;

    if (!type) {
        static const GTypeInfo info = {
            sizeof (HildonVVolumebarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_vvolumebar_class_init,     /* class_init */
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonVVolumebar),
            0,
            (GInstanceInitFunc) hildon_vvolumebar_init,
        };
        type =
            g_type_register_static (HILDON_TYPE_VOLUMEBAR,
                    "HildonVVolumebar", &info, 0);
    }
    return type;
}

static void 
hildon_vvolumebar_class_init                    (HildonVVolumebarClass *klass)
{
    GtkWidgetClass *volumebar_class = GTK_WIDGET_CLASS(klass);

    parent_class = g_type_class_peek_parent(klass);

    volumebar_class->size_request   = hildon_vvolumebar_size_request;
    volumebar_class->size_allocate  = hildon_vvolumebar_size_allocate;
    volumebar_class->expose_event   = hildon_vvolumebar_expose;
}

static void 
hildon_vvolumebar_init                          (HildonVVolumebar *vvolumebar)
{
    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE (vvolumebar);
    g_assert (priv);

    priv->volumebar = HILDON_VOLUMEBAR_RANGE (hildon_volumebar_range_new
            (GTK_ORIENTATION_VERTICAL));

    gtk_widget_set_parent (GTK_WIDGET (priv->tbutton), GTK_WIDGET (vvolumebar));
    gtk_widget_set_parent (GTK_WIDGET (priv->volumebar), GTK_WIDGET (vvolumebar));

    gtk_scale_set_draw_value (GTK_SCALE (priv->volumebar), FALSE);

    /* Signals */
    g_signal_connect_swapped(G_OBJECT(priv->volumebar), "value-changed",
            G_CALLBACK(hildon_volumebar_level_change),
            vvolumebar);

    g_signal_connect_swapped(priv->tbutton, "toggled",
            G_CALLBACK(hildon_volumebar_mute_toggled), vvolumebar);

    /* FIXME Not sure why this is here */
    gtk_widget_show (GTK_WIDGET (priv->volumebar));
}

/**
 * hildon_vvolumebar_new:
 *
 * Creates a new #HildonVVolumebar widget.
 *
 * Returns: a new #HildonVVolumebar
 */
GtkWidget*
hildon_vvolumebar_new                           (void)
{
    return GTK_WIDGET (g_object_new(HILDON_TYPE_VVOLUMEBAR, NULL));
}

static gboolean 
hildon_vvolumebar_expose                        (GtkWidget *widget,
                                                 GdkEventExpose *event)
{

    HildonVolumebarPrivate *priv;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(HILDON_VOLUMEBAR(widget));
    g_assert (priv);

    if (GTK_WIDGET_DRAWABLE (widget)) {
        /* Paint background */
        gtk_paint_box (widget->style, widget->window,
                GTK_WIDGET_STATE (priv->volumebar), GTK_SHADOW_OUT,
                NULL, widget, "background",
                widget->allocation.x,
                widget->allocation.y,
                widget->allocation.width,
                widget->allocation.height);

        /* The contents of the widget can paint themselves */
        (*GTK_WIDGET_CLASS (parent_class)->expose_event) (widget, event);
    }

    return FALSE;
}

static void
hildon_vvolumebar_size_request                  (GtkWidget *widget,
                                                 GtkRequisition *requisition)
{
    requisition->height = MINIMUM_BAR_HEIGHT;
    requisition->width = DEFAULT_BAR_WIDTH;
}

static void
hildon_vvolumebar_size_allocate                 (GtkWidget *widget,
                                                 GtkAllocation *allocation)
{
    HildonVolumebarPrivate *priv;

    GtkAllocation range_allocation, button_allocation;

    priv = HILDON_VOLUMEBAR_GET_PRIVATE(widget);
    g_assert (priv);

    /* Center the widget horizontally */
    if (allocation->width > DEFAULT_BAR_WIDTH) {
        allocation->x +=
            (allocation->width - DEFAULT_BAR_WIDTH) / 2;
        allocation->width = DEFAULT_BAR_WIDTH;
    }

    GTK_WIDGET_CLASS (parent_class)->size_allocate (widget, allocation);

    if (priv->volumebar && GTK_WIDGET_VISIBLE (priv->volumebar)) {
        /* Allocate space for the slider */
        range_allocation.x = allocation->x;
        range_allocation.y = allocation->y + DEFAULT_ENDING_SIZE;

        range_allocation.width = DEFAULT_BAR_WIDTH;

        if (priv->tbutton && GTK_WIDGET_VISIBLE (priv->tbutton))
        {
            /* Leave room for the mute button */
            range_allocation.height = MAX (0,
                    allocation->height
                    - 2 * DEFAULT_ENDING_SIZE
                    - DEFAULT_VERTICAL_TBUTTON_HEIGHT
                    - VERTICAL_MUTE_GAP);
        }

        else
        {
            range_allocation.height = MAX (0,
                    allocation->height
                    - 2 * DEFAULT_ENDING_SIZE);
        }

        gtk_widget_size_allocate (GTK_WIDGET (priv->volumebar),
                &range_allocation);
    }

    if (priv->tbutton && GTK_WIDGET_VISIBLE (priv->tbutton)) {
        /* Allocate space for the mute button */
        button_allocation.x = allocation->x + HORIZONTAL_MUTE_GAP;
        button_allocation.y = allocation->y + allocation->height -
            DEFAULT_VERTICAL_TBUTTON_HEIGHT -
            VERTICAL_MUTE_GAP - 2 * DEFAULT_ENDING_SIZE;
        button_allocation.width = DEFAULT_VERTICAL_TBUTTON_WIDTH;
        button_allocation.height = DEFAULT_VERTICAL_TBUTTON_HEIGHT;
        gtk_widget_size_allocate (GTK_WIDGET (priv->tbutton),
                &button_allocation);
    }
}
