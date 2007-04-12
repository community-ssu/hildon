/*
 * This file is a part of hildon
 *
 * Copyright (C) 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 * Author: Xan Lopez <xan.lopez@nokia.com>
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
#include "hildon-bread-crumb-trail.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#if 1
#include <hildon/hildon.h>
#endif

GtkWidget *treeview;
gchar *current_root;

enum {
  ICON_COL,
  STRING_COL,
  IS_DIR_COL,
  N_COLS
};

static void
populate_store (GtkListStore *store,
		const gchar *path)
{
  GDir *dir;
  GError *error = NULL;
  gchar *item;
  GtkTreeIter iter;
  struct stat stat_info;
  GdkPixbuf *pixbuf = NULL;

  dir = g_dir_open (path, 0, &error);
  if (error)
    {
      g_debug ("Error populating store: %s", error->message);
      g_error_free (error);
      return;
    }

  while ((item = (gchar*)g_dir_read_name (dir)) != NULL)
    {
      gchar *file_path = g_strconcat (path, "/", item, NULL);

      if (stat (file_path, &stat_info) == -1)
	{
	  g_debug ("error retrieving stat info for %s", item);
	  continue;
	}
      g_free (file_path);

      gtk_list_store_append (store, &iter);

      if (S_ISDIR (stat_info.st_mode))
	pixbuf = gdk_pixbuf_new_from_file ("./gnome-fs-directory.png", NULL);
      else
	pixbuf = gdk_pixbuf_new_from_file ("./gnome-mime-text.png", NULL);

      gtk_list_store_set (store, &iter,
			  ICON_COL, pixbuf,
			  STRING_COL, item,
			  IS_DIR_COL, S_ISDIR (stat_info.st_mode) ? TRUE : FALSE,
			  -1);
      if (pixbuf)
	g_object_unref (pixbuf);
    }

  g_dir_close (dir);

  return;
}

static void
free_id (gpointer data)
{
  g_debug ("Freeing ID data");
  g_free (data);
}

static void
row_activated_cb (GtkTreeView *treeview,
		  GtkTreePath *path,
		  GtkTreeViewColumn *column,
		  HildonBreadCrumbTrail *bct)
{
  gchar *text = NULL, *new_root;
  GtkTreeIter iter;
  GtkTreeModel *model;
  gboolean is_dir;

  model = gtk_tree_view_get_model (treeview);
  gtk_tree_model_get_iter (model, &iter, path);
  gtk_tree_model_get (model, &iter,
		      STRING_COL, &text,
		      IS_DIR_COL, &is_dir,
		      -1);

  if (is_dir == FALSE) goto out;

  g_debug ("Clicked %s", text);

  new_root = g_strconcat (g_str_equal (current_root, "/")? "" : current_root, "/", text, NULL);
  gtk_list_store_clear (GTK_LIST_STORE (model));
  populate_store (GTK_LIST_STORE (model), new_root);

  hildon_bread_crumb_trail_push_text (bct, text, new_root, (GDestroyNotify)free_id);
  
  if (current_root)
    {
      g_free (current_root);
    }

  current_root = g_strdup (new_root);

 out:
  g_free (text);
}

static void
crumb_clicked_cb (HildonBreadCrumbTrail *bct, gpointer id)
{
  GtkTreeModel *model;
  gchar *text = (gchar*)id;

  g_debug ("item %s clicked", text);
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (treeview));
  gtk_list_store_clear (GTK_LIST_STORE (model));
  populate_store (GTK_LIST_STORE (model), text);
  if (current_root)
    g_free (current_root);
  current_root = g_strdup (text);
}

int main (int argc, char **argv)
{
#if 1
  HildonProgram *program;
#endif
  GtkListStore *store;
  GtkWidget *window, *scrolled_window, *vbox, *bct;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  gtk_init (&argc, &argv);

  /* Main window */
#if 1
  program = hildon_program_get_instance ();
  window = hildon_window_new ();
  hildon_program_add_window (program, HILDON_WINDOW (window));
#else
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
#endif
  gtk_container_set_border_width (GTK_CONTAINER (window), 2);
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 600);
  g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

  vbox = gtk_vbox_new (FALSE, 3);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  gtk_widget_show (vbox);

  current_root = g_strdup ("/");

  bct = hildon_bread_crumb_trail_new ();
  g_signal_connect (bct, "crumb-clicked", G_CALLBACK (crumb_clicked_cb), NULL);
  gtk_box_pack_start (GTK_BOX (vbox), bct, FALSE, FALSE, 0);
  gtk_widget_show (bct);

  hildon_bread_crumb_trail_push_text (HILDON_BREAD_CRUMB_TRAIL (bct), "/",
				      g_strdup ("/"), (GDestroyNotify)free_id);

  /* Treeview */
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
				  GTK_POLICY_AUTOMATIC,
				  GTK_POLICY_AUTOMATIC);
  store = gtk_list_store_new (N_COLS, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_BOOLEAN);
  populate_store (store, "/");
  treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
  g_signal_connect (treeview, "row-activated", G_CALLBACK (row_activated_cb), bct);

  renderer = gtk_cell_renderer_pixbuf_new ();
  column = gtk_tree_view_column_new_with_attributes ("Icon",
						     renderer,
						     "pixbuf", ICON_COL,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("Name",
						     renderer,
						     "text", STRING_COL,
						     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), column);

  gtk_container_add (GTK_CONTAINER (scrolled_window), treeview);
  gtk_widget_show (treeview);
  gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);
  gtk_widget_show (scrolled_window);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
