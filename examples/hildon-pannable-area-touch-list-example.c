/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Author: Karl Lattimer <karl.lattimer@nokia.com>
 *
 * Based on testhildontreeview.c by Kristian Rietveld <kris@xxxxxxx.com>
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

#include <gtk/gtk.h>
#include <hildon.h>

enum
{
  MULTI_SELECT  = 1 << 0,
  NORMAL_MODE   = 1 << 1,
  EDIT_MODE     = 1 << 2
};


static GtkTreeModel *
create_model (void)
{
  int i;
  GtkListStore *store;

  store = gtk_list_store_new (1, G_TYPE_STRING);

  for (i = 0; i < 50; i++)
    {
      gchar *str;

      str = g_strdup_printf ("\nRow %d\n", i);
      gtk_list_store_insert_with_values (store, NULL, i, 0, str, -1);
      g_free (str);
    }

  return GTK_TREE_MODEL (store);
}

static void
row_activated_callback (GtkWidget         *tree_view,
                        GtkTreePath       *path,
                        GtkTreeViewColumn *column,
                        gpointer           user_data)
{
  g_print ("row-activated emitted.\n");
}

static void
row_insensitive_callback (GtkWidget         *tree_view,
                          GtkTreePath       *path,
                          gpointer           user_data)
{
  g_print ("row-insensitive emitted.\n");
}

static GtkWidget *
create_tree_view (HildonUIMode  mode,
                  const char   *name,
                  gboolean      multi_select)
{
  GtkWidget *tree_view;
  GtkCellRenderer *renderer;
  GtkTreeSelection *selection;
  GtkTreeModel *model;

  if (name && g_str_equal (name, "fremantle-widget"))
      tree_view = hildon_gtk_tree_view_new (mode);
  else
      tree_view = gtk_tree_view_new ();

  if (name)
    gtk_widget_set_name (tree_view, name);

  gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (tree_view), TRUE);

  model = create_model ();
  gtk_tree_view_set_model (GTK_TREE_VIEW (tree_view), model);
  g_object_unref (model);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tree_view));
  if (multi_select)
    gtk_tree_selection_set_mode (selection, GTK_SELECTION_MULTIPLE);
  else if (mode != HILDON_UI_MODE_NORMAL)
    gtk_tree_selection_set_mode (selection, GTK_SELECTION_SINGLE);

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer,
                "xalign", 0.5,
                "weight", PANGO_WEIGHT_BOLD,
                NULL);

  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (tree_view),
                                               0, "Column 0",
                                               renderer,
                                               "text", 0,
                                               NULL);

  return tree_view;
}

static void
create_tree_view_window (GtkWidget *button,
                         gpointer   user_data)
{
  const char *name;
  GtkWidget *window;
  GtkWidget *sw;
  GtkWidget *tree_view;
  HildonUIMode mode = 0;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete-event",
                    G_CALLBACK (gtk_widget_destroy), window);
  gtk_container_set_border_width (GTK_CONTAINER (window), 6);

  sw = hildon_pannable_area_new ();
  gtk_container_add (GTK_CONTAINER (window), sw);

  if ((GPOINTER_TO_INT (user_data) & NORMAL_MODE) == NORMAL_MODE)
    {
      mode = HILDON_UI_MODE_NORMAL;
      name = "fremantle-widget";
    }
  else if ((GPOINTER_TO_INT (user_data) & EDIT_MODE) == EDIT_MODE)
    {
      mode = HILDON_UI_MODE_EDIT;
      name = "fremantle-widget";
    }
  else
    name = NULL;

  tree_view = create_tree_view (mode, name,
                                (GPOINTER_TO_INT (user_data) & MULTI_SELECT) == MULTI_SELECT);

  /* Some signals doing printfs() to see if the behavior is correct. */
  g_signal_connect (tree_view, "row-activated",
                    G_CALLBACK (row_activated_callback), NULL);
  g_signal_connect (tree_view, "row-insensitive",
                    G_CALLBACK (row_insensitive_callback), NULL);

  gtk_widget_set_size_request (tree_view, 480, 800);
  gtk_container_add (GTK_CONTAINER (sw), tree_view);

  gtk_widget_show_all (window);
}

int
main (int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *mainbox;
  GtkWidget *label;
  GtkWidget *vbox;
  GtkWidget *padbox;
  GtkWidget *button;

  gtk_init (&argc, &argv);

  gtk_rc_parse_string ("style \"fremantle-widget\" {\n"
                       "  GtkWidget::hildon-mode = 1\n"
                       "} widget \"*.fremantle-widget\" style \"fremantle-widget\"");

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete-event",
                    G_CALLBACK (gtk_main_quit), NULL);
  gtk_container_set_border_width (GTK_CONTAINER (window), 6);

  mainbox = gtk_vbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (window), mainbox);

  /* old-style */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), "<b>Old-style behavior</b>");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (mainbox), label, FALSE, FALSE, 0);

  padbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (mainbox), padbox, FALSE, FALSE, 6);

  gtk_box_pack_start (GTK_BOX (padbox), gtk_label_new ("   "),
                      FALSE, FALSE, 6);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (padbox), vbox, TRUE, TRUE, 6);

  button = gtk_button_new_with_label ("Single selection");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (create_tree_view_window), 0x0);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  button = gtk_button_new_with_label ("Multiple selections");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (create_tree_view_window),
                    GINT_TO_POINTER (MULTI_SELECT));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  /* normal-mode */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), "<b>Fremantle Normal mode</b>");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (mainbox), label, FALSE, FALSE, 0);

  padbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (mainbox), padbox, FALSE, FALSE, 6);

  gtk_box_pack_start (GTK_BOX (padbox), gtk_label_new ("   "),
                      FALSE, FALSE, 6);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (padbox), vbox, TRUE, TRUE, 6);

  button = gtk_button_new_with_label ("Direct activation");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (create_tree_view_window),
                    GINT_TO_POINTER (NORMAL_MODE));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  /* edit-mode */
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), "<b>Fremantle Edit mode</b>");
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (mainbox), label, FALSE, FALSE, 0);

  padbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (mainbox), padbox, FALSE, FALSE, 6);

  gtk_box_pack_start (GTK_BOX (padbox), gtk_label_new ("   "),
                      FALSE, FALSE, 6);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (padbox), vbox, TRUE, TRUE, 6);

  button = gtk_button_new_with_label ("Single selection");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (create_tree_view_window),
                    GINT_TO_POINTER (EDIT_MODE));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);

  button = gtk_button_new_with_label ("Multiple selections");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (create_tree_view_window),
                    GINT_TO_POINTER (EDIT_MODE | MULTI_SELECT));
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 0);


  button = gtk_button_new_with_label ("Close");
  g_signal_connect (button, "clicked",
                    G_CALLBACK (gtk_main_quit), NULL);
  gtk_box_pack_end (GTK_BOX (mainbox), button, FALSE, FALSE, 0);

  gtk_box_pack_end (GTK_BOX (mainbox), gtk_hseparator_new (),
                    FALSE, FALSE, 6);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
