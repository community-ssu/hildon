/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Author: Karl Lattimer <karl.lattimer@nokia.com>
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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon.h"

enum { TEXT_COLUMN, OPTIONAL_COLUMN, N_COLUMNS };

typedef struct {
  GtkWidget *treeview;
} HiddenColContext;

HiddenColContext *ctx;

static void
horizontal_movement (HildonPannableArea *area,
                     HildonPannableAreaMovDirection direction,
		     GtkWidget *widget, gdouble x, gdouble y,
                     gpointer user_data)
{
  GtkTreePath *path;
  GdkRectangle rect;
  gint col_x;
  GtkTreeViewColumn *col = GTK_TREE_VIEW_COLUMN (user_data);

  g_print ("widget %p treeview %p\n", widget, ctx->treeview);

  if (direction == HILDON_PANNABLE_AREA_MOV_LEFT) {

    path = gtk_tree_path_new_first ();

    gtk_tree_view_get_background_area (GTK_TREE_VIEW (ctx->treeview),
                                       path, col, &rect);

    gtk_tree_view_convert_bin_window_to_tree_coords (GTK_TREE_VIEW (ctx->treeview),
                                                     rect.x, 0, &col_x, NULL);

    gtk_tree_path_free (path);

    hildon_pannable_area_scroll_to (area, col_x, -1);
  }
  else {
    hildon_pannable_area_scroll_to (area, 0, -1);
  }

  g_print ("horizontal_movement %lf, %lf\n", x, y);
}

static void
vertical_movement (HildonPannableArea *area,
                   HildonPannableAreaMovDirection direction,
		   gdouble x, gdouble y,
                   gpointer user_data)
{
  g_print ("vertical_movement: %lf, %lf\n", x, y);
}

static void
get_sawtooth_label (gchar **label, guint num)
{
  static gchar *sawtooth = NULL;
  gchar *label_aux, *sawtooth_aux;

  if (num % 5 != 0) {
    sawtooth_aux = g_strconcat (" ", sawtooth, NULL);
    g_free (sawtooth);

    sawtooth = sawtooth_aux;
  } else {
    sawtooth = g_strdup (" ");
  }

  label_aux = g_strconcat (sawtooth, *label, NULL);
  g_free (*label);

  *label = label_aux;
}

int
main (int argc, char **args)
{
    int i;
    HildonProgram *program;
    GtkWidget *window, *tv, *panarea;
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkListStore *store;
    GtkVBox *vbox;

    gtk_init (&argc, &args);

    program = hildon_program_get_instance ();

    ctx = g_new0 (HiddenColContext, 1);

    /* Create the main window */
    window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));

    gtk_container_set_border_width (GTK_CONTAINER (window), 5);

    /* Create a VBox and pack some buttons */
    vbox = GTK_VBOX (gtk_vbox_new (FALSE, 1));

    /* Create a treeview */
    tv = gtk_tree_view_new ();
    ctx->treeview = tv;

    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Title", renderer, "text", TEXT_COLUMN, NULL);
    gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_FIXED);

    gtk_tree_view_column_set_fixed_width (col, 700);

    gtk_tree_view_append_column (GTK_TREE_VIEW(tv), col);

    col = gtk_tree_view_column_new_with_attributes ("Title", renderer, "text", OPTIONAL_COLUMN, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(tv), col);

    /* Add some rows to the treeview */
    store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
    for (i = 0; i < 100; i++) {
            GtkTreeIter iter;
            gchar *label = g_strdup_printf ("Row number %d", i);
            gchar *label_optional = g_strdup_printf ("< >");

            get_sawtooth_label (&label, i);

            gtk_list_store_append (store, &iter);
            gtk_list_store_set (store, &iter, TEXT_COLUMN, label, -1);
            gtk_list_store_set (store, &iter, OPTIONAL_COLUMN, label_optional, -1);
            g_free (label);
            g_free (label_optional);
    }
    gtk_tree_view_set_model (GTK_TREE_VIEW (tv), GTK_TREE_MODEL (store));
    g_object_unref (store);

    /* Pack the treeview in the VBox */
    gtk_box_pack_start (GTK_BOX (vbox), tv, TRUE, TRUE, 0);

    /* Put everything in a pannable area */
    panarea = hildon_pannable_area_new ();
    g_object_set (panarea, "mov_mode", HILDON_PANNABLE_AREA_MOV_MODE_VERT,
                  "hovershoot_max", 0,
                  "hindicator_mode", HILDON_PANNABLE_AREA_INDICATOR_MODE_HIDE, NULL);

    hildon_pannable_area_add_with_viewport (HILDON_PANNABLE_AREA (panarea), GTK_WIDGET (vbox));
    gtk_container_add (GTK_CONTAINER (window), panarea);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    g_signal_connect (G_OBJECT (panarea), "horizontal_movement", G_CALLBACK (horizontal_movement), col);
    g_signal_connect (G_OBJECT (panarea), "vertical_movement", G_CALLBACK (vertical_movement), NULL);

    gtk_widget_show_all (GTK_WIDGET (window));

    gtk_main ();

    return 0;
}
