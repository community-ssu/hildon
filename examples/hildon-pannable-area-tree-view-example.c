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

#include                                        <stdio.h>
#include                                        <stdlib.h>
#include                                        <glib.h>
#include                                        <gtk/gtk.h>
#include                                        <hildon/hildon.h>

enum { TEXT_COLUMN, N_COLUMNS };

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
    if (sawtooth)
      g_free (sawtooth);

    sawtooth = g_strdup (" ");
  }
  
  label_aux = g_strconcat (sawtooth, *label, NULL);
  g_free (*label);
  
  *label = label_aux;
}

int
main (int argc, char **argv)
{
    int i;
    HildonProgram *program;
    GtkWidget *window, *tv, *panarea;
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkListStore *store;

    hildon_gtk_init (&argc, &argv);

    program = hildon_program_get_instance ();

    /* Create the main window */
    window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));

    gtk_container_set_border_width (GTK_CONTAINER (window), 5);

    /* Create a treeview */
    tv = gtk_tree_view_new ();
    renderer = gtk_cell_renderer_text_new ();
    col = gtk_tree_view_column_new_with_attributes ("Title", renderer, "text", TEXT_COLUMN, NULL);
    gtk_tree_view_append_column (GTK_TREE_VIEW(tv), col);

    /* Add some rows to the treeview */
    store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING);
    for (i = 0; i < 100; i++) {
            GtkTreeIter iter;
            gchar *label = g_strdup_printf ("Row number %d", i);
            
            get_sawtooth_label (&label, i);

            gtk_list_store_append (store, &iter);
            gtk_list_store_set (store, &iter, TEXT_COLUMN, label, -1);
            g_free (label);
    }
    gtk_tree_view_set_model (GTK_TREE_VIEW (tv), GTK_TREE_MODEL (store));
    g_object_unref (store);

    /* Put everything in a pannable area */
    panarea = hildon_pannable_area_new ();
    gtk_container_add (GTK_CONTAINER (panarea), GTK_WIDGET (tv));
    gtk_container_add (GTK_CONTAINER (window), panarea);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (GTK_WIDGET (window));

    gtk_main ();

    return 0;
}
