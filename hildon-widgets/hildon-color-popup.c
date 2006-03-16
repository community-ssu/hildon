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
 * @file hildon-color-popup.c
 *
 * This file contains the implementation of Hildon Color Selector Popup
 * widget containing the custom color palette selector popup for selecting 
 * different colors based on RGB values.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtkbox.h>
#include <gtk/gtktable.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkdrawingarea.h>

#include "hildon-color-selector.h"
#include "hildon-color-popup.h"
#include "hildon-controlbar.h"

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

/* Pixel sizes */
#define HILDON_COLOR_PALETTE_SIZE        120
#define HILDON_COLOR_CONTROLBAR_MAX       31
#define HILDON_COLOR_CONTROLBAR_MIN        0 
#define HILDON_COLOR_LABELS_LEFT_PAD      35
#define HILDON_COLOR_PALETTE_POS_PAD      45
#define HILDON_COLOR_BAR_WIDTH           449
#define HILDON_COLOR_COL_SPACING          18

/* 
 * Private function prototype definitions 
 */

static gboolean 
hildon_popup_palette_expose (GtkWidget * widget,
                             GdkEventExpose * event,
                             gpointer data);
/**
 * hildon_color_popup_new:
 * @parent: the parent window of the dialog.
 * @initial_color: a #GdkColor with the initial values to be used.
 * @popup_data: a #HildonColorPopup.
 *
 * This function creates a new popup dialog with three controlbars
 * (red, green, blue) and a drawing area with the current color.
 *
 * Used as normal GtkDialog (run with gtk_dialog_run() and read 
 * stardard responses (GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL). 
 *
 * Returns: the newly created popup dialog. 
 */

GtkWidget *
hildon_color_popup_new(GtkWindow *parent, const GdkColor *initial_color, 
		HildonColorPopup *popup_data)
{
  GtkWidget *popup;
  GtkTable *layout;
  GtkWidget *area;
  GtkWidget *l_red, *l_green, *l_blue;
  GdkColor  *current_color;

  current_color = initial_color;

  /* Create control bars for HildonColorPopup */
  popup_data->ctrlbar_red   = hildon_controlbar_new (); 
  popup_data->ctrlbar_green = hildon_controlbar_new ();
  popup_data->ctrlbar_blue  = hildon_controlbar_new ();
  area = gtk_drawing_area_new();
  layout = GTK_TABLE(gtk_table_new(12, 2, FALSE));

  /* Set widgets' size */
  gtk_widget_set_size_request (area, 
                               HILDON_COLOR_PALETTE_SIZE, 
                               HILDON_COLOR_PALETTE_SIZE);
  gtk_widget_set_size_request(popup_data->ctrlbar_red,
                               HILDON_COLOR_BAR_WIDTH, -1); 
  gtk_widget_set_size_request(popup_data->ctrlbar_green,
                               HILDON_COLOR_BAR_WIDTH, -1); 
  gtk_widget_set_size_request(popup_data->ctrlbar_blue,
                               HILDON_COLOR_BAR_WIDTH, -1); 

  /* Create labels for three kinds of color */
  l_red = gtk_label_new( _("Ecdg_fi_5bit_colour_selector_red"));
  l_green = gtk_label_new(_("Ecdg_fi_5bit_colour_selector_green"));
  l_blue = gtk_label_new(_("Ecdg_fi_5bit_colour_selector_blue"));
  
  /* Position the labels to start about the same label as the controlbars */
  gtk_misc_set_alignment(GTK_MISC(l_red), 0.08f, 0.5f);
  gtk_misc_set_alignment(GTK_MISC(l_green), 0.08f, 0.5f);  
  gtk_misc_set_alignment(GTK_MISC(l_blue), 0.08f, 0.5f);

  /* Add labels and control bars to the layout table */
  gtk_table_set_col_spacing(layout, 0, HILDON_COLOR_COL_SPACING);
  gtk_table_attach_defaults(layout, l_red, 0, 1, 0, 2);
  gtk_table_attach_defaults(layout, popup_data->ctrlbar_red, 0, 1, 2, 4);
  gtk_table_attach_defaults(layout, l_green, 0, 1, 4, 6);
  gtk_table_attach_defaults(layout, popup_data->ctrlbar_green, 0, 1, 6, 8);
  gtk_table_attach_defaults(layout, l_blue, 0, 1, 8, 10);
  gtk_table_attach_defaults(layout, popup_data->ctrlbar_blue, 0, 1, 10, 12);
  gtk_table_attach(layout, area, 1, 2, 3, 11, GTK_SHRINK, GTK_SHRINK, 0, 0);

  /* Give the maximum and minimum limits for each control bar */
  hildon_controlbar_set_range (HILDON_CONTROLBAR(popup_data->ctrlbar_red),
                                 HILDON_COLOR_CONTROLBAR_MIN, 
                                 HILDON_COLOR_CONTROLBAR_MAX);
  hildon_controlbar_set_range (HILDON_CONTROLBAR(popup_data->ctrlbar_green), 
                                 HILDON_COLOR_CONTROLBAR_MIN, 
                                 HILDON_COLOR_CONTROLBAR_MAX);
  hildon_controlbar_set_range (HILDON_CONTROLBAR(popup_data->ctrlbar_blue), 
                                 HILDON_COLOR_CONTROLBAR_MIN, 
                                 HILDON_COLOR_CONTROLBAR_MAX);

  /* Give the initial values for each control bar */
  hildon_controlbar_set_value (HILDON_CONTROLBAR(popup_data->ctrlbar_red),   
                                 (current_color->red >> 11)&0x1F);
  hildon_controlbar_set_value (HILDON_CONTROLBAR(popup_data->ctrlbar_green),
                                 (current_color->green >> 11)&0x1F);
  hildon_controlbar_set_value (HILDON_CONTROLBAR(popup_data->ctrlbar_blue),
                                 (current_color->blue >> 11)&0x1F);

  /* Register controlbar callbacks */
  g_signal_connect_swapped(popup_data->ctrlbar_red, "value-changed",
                      G_CALLBACK(gtk_widget_queue_draw), area);
  g_signal_connect_swapped(popup_data->ctrlbar_green, "value-changed",
                      G_CALLBACK(gtk_widget_queue_draw), area);
  g_signal_connect_swapped(popup_data->ctrlbar_blue, "value-changed",
                      G_CALLBACK(gtk_widget_queue_draw), area);

  /* Attach expose_event callback function to drawing area */
  g_signal_connect (area,  "expose_event",
                      G_CALLBACK(hildon_popup_palette_expose), 
                      popup_data);
  
  /* Create popup dialog */
  popup = gtk_dialog_new_with_buttons (_("ecdg_ti_5bit_colour_selector"), 
                                       GTK_WINDOW(parent),
                                       GTK_DIALOG_DESTROY_WITH_PARENT |
                                       GTK_DIALOG_NO_SEPARATOR,
                                       _("ecdg_bd_5bit_colour_selector_ok"), GTK_RESPONSE_OK,
                                       _("Ecdg_bd_5bit_colour_selector_cancel"),
                                       GTK_RESPONSE_CANCEL,
                                       NULL);

  gtk_dialog_set_default_response(GTK_DIALOG(popup), GTK_RESPONSE_OK);

  /* Add layout table to the Vbox of the popup dialog */
  gtk_box_pack_start (GTK_BOX(GTK_DIALOG(popup)->vbox), 
                        GTK_WIDGET(layout), TRUE, TRUE, 0);

  /* Show thw Vbox of the popup dialog */
  gtk_widget_show_all(GTK_DIALOG(popup)->vbox);	     

  return popup;
}

/**
 * hildon_color_popup_set_color_from_sliders:
 * @color: a pointer to #GdkColor to which the new values will be put.
 * @popup_data: a #HildonColorPopup.
 *
 * This function sets the values of the given #GdkColor to the values
 * in the sliders of controlbars.
 * 
 * Returns: void. 
 */

void
hildon_color_popup_set_color_from_sliders(GdkColor *color,
  HildonColorPopup *popup_data)
{
  color->pixel = 0;
  color->red = hildon_controlbar_get_value (
        HILDON_CONTROLBAR(popup_data->ctrlbar_red)) <<  11;
  color->green = hildon_controlbar_get_value (
        HILDON_CONTROLBAR(popup_data->ctrlbar_green)) <<  11;
  color->blue = hildon_controlbar_get_value (
        HILDON_CONTROLBAR(popup_data->ctrlbar_blue)) <<  11;
}

/* expose_event callback function */
static gboolean 
hildon_popup_palette_expose (GtkWidget * widget,
                             GdkEventExpose *event, gpointer data)
{
  if (GTK_WIDGET_DRAWABLE(widget))
  {
    GdkColor color;
    GdkGC * gc = gdk_gc_new (widget->window); 

    /* Get the current color value */
    hildon_color_popup_set_color_from_sliders(&color, data);
    gdk_gc_set_rgb_fg_color(gc, &color);
                         
    /* draw the color area */
    gdk_draw_rectangle( widget->window, gc, TRUE /* filled */,  
                        1, 1, widget->allocation.width - 2, 
                        widget->allocation.height - 2);    

    color.pixel = color.red = color.green = color.blue = 0;
    gdk_gc_set_rgb_fg_color(gc, &color);

    /* Draw frames on color box */
    gdk_draw_rectangle( widget->window, gc, FALSE, 
                        0, 0, widget->allocation.width - 1, 
                        widget->allocation.height - 1); 

    /* Free memory used by the graphics contexts */
    g_object_unref(gc);
  }

  return TRUE;
}
