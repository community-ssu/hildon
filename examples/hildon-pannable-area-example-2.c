/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Based in hildon-pannable-area-example.c
 * by Karl Lattimer <karl.lattimer@nokia.com>
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
#include                                        <string.h>
#include                                        "hildon.h"

enum { PIXBUF_COLUMN, TEXT_COLUMN, N_COLUMNS };

typedef struct {
  GtkWidget *scroll_entry;
  GtkWidget *jump_entry;
  GtkWidget *panarea;
  GtkWidget *treeview;
} SearchContext;

SearchContext *ctx;

static void
search_button_clicked (GtkButton *button,
		       gpointer user_data)
{
  GtkTreeModel *model;
  const gchar *s1;
  gchar *s2;
  gboolean found;
  GtkTreeIter iter;
  GtkTreePath *path;
  GdkRectangle rect;
  gint y;
  gboolean jump_or_scroll;

  jump_or_scroll = *((gboolean *) user_data);

  if (jump_or_scroll)
    {
      s1 = gtk_entry_get_text (GTK_ENTRY (ctx->scroll_entry));
    }
  else
    {
      s1 = gtk_entry_get_text (GTK_ENTRY (ctx->jump_entry));
    }

  model = gtk_tree_view_get_model (GTK_TREE_VIEW (ctx->treeview));

  gtk_tree_model_get_iter_first (model, &iter);

  do {
    gtk_tree_model_get (model, &iter, TEXT_COLUMN, &s2, -1);
    found = (strcmp (s1, s2) == 0);
    g_free (s2);
  } while (found != TRUE && gtk_tree_model_iter_next (model, &iter));

  if (found) {
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW (ctx->treeview));

    path = gtk_tree_model_get_path (model, &iter);

    gtk_tree_view_get_background_area (GTK_TREE_VIEW (ctx->treeview),
                                       path, NULL, &rect);
    gtk_tree_view_convert_bin_window_to_tree_coords (GTK_TREE_VIEW (ctx->treeview),
                                                     0, rect.y, NULL, &y);
    g_print ("text found in (0, %d)\n", y);

    gtk_tree_selection_select_path (selection, path);
    if (jump_or_scroll)
      {
        hildon_pannable_area_scroll_to (HILDON_PANNABLE_AREA (ctx->panarea),
                                        -1, y);
      }
    else
      {
        hildon_pannable_area_jump_to (HILDON_PANNABLE_AREA (ctx->panarea),
                                      -1, y);
      }

    gtk_tree_path_free (path);
  }
}

int
main (int argc, char **args)
{
  int i;
  HildonProgram *program;
  GtkWidget *window, *tv, *panarea, *button;
  GtkTreeViewColumn *col;
  GtkCellRenderer *renderer;
  GtkListStore *store;
  GtkWidget *hbox, *vbox;
  GtkWidget *entry;
  gboolean scroll = TRUE;
  gboolean jump = FALSE;
  GSList *stocks, *item;

  gtk_init (&argc, &args);

  program = hildon_program_get_instance ();

  /* Create the main window */
  window = hildon_window_new ();
  hildon_program_add_window (program, HILDON_WINDOW (window));
  gtk_container_set_border_width (GTK_CONTAINER (window), 5);

  /* Create a treeview */
  tv = gtk_tree_view_new ();

  col = gtk_tree_view_column_new ();
  gtk_tree_view_column_set_title (col, "Title");

  renderer = gtk_cell_renderer_pixbuf_new ();
  gtk_cell_renderer_set_fixed_size (renderer, 48, 48);
  gtk_tree_view_column_pack_start (col, renderer, FALSE);
  gtk_tree_view_column_set_attributes (col, renderer, "stock-id", PIXBUF_COLUMN, NULL);

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_column_pack_start (col, renderer, FALSE);
  gtk_tree_view_column_set_attributes (col, renderer, "text", TEXT_COLUMN, NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW(tv), col);

  /* Add some rows to the treeview */
  stocks = gtk_stock_list_ids ();
  item = stocks;
  store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
  for (i = 0; i < 100; i++) {
    GtkTreeIter iter;
    GtkStockItem stock_item;
    gchar *stock_id;

    stock_id = (gchar *)item->data;
    gtk_stock_lookup (stock_id, &stock_item);
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter, PIXBUF_COLUMN, stock_id, TEXT_COLUMN, stock_item.label, -1);

    item = item->next? item->next : stocks;
  }
  gtk_tree_view_set_model (GTK_TREE_VIEW (tv), GTK_TREE_MODEL (store));
  g_object_unref (store);

  g_slist_foreach (stocks, (GFunc)g_free, NULL);
  g_slist_free (stocks);

  /* Put everything in a pannable area */
  panarea = hildon_pannable_area_new ();
  gtk_container_add (GTK_CONTAINER (panarea), GTK_WIDGET (tv));

  ctx = g_new0 (SearchContext, 1);

  vbox = gtk_vbox_new (FALSE, 5);
  hbox = gtk_hbox_new (FALSE, 5);
  button = gtk_button_new_from_stock (GTK_STOCK_FIND);

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "Enter some text to scroll");

  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (search_button_clicked), &scroll);

  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 10);

  ctx->scroll_entry = entry;

  hbox = gtk_hbox_new (FALSE, 5);
  button = gtk_button_new_from_stock (GTK_STOCK_FIND);

  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "Enter some text to jump");

  g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (search_button_clicked), &jump);

  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), entry, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 10);

  ctx->jump_entry = entry;
  ctx->treeview = tv;
  ctx->panarea = panarea;

  g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

  gtk_box_pack_start (GTK_BOX (vbox), panarea, TRUE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (window), vbox);

  gtk_widget_show_all (GTK_WIDGET (window));

  gtk_main ();

  return 0;
}
