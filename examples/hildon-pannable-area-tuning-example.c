/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2009 Nokia Corporation, all rights reserved.
 * Copyright (C) 2006 The Free Software Foundation
 *
 * Author: Claudio Saavedra <csaavedra@alumnos.utalca.cl>
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
 */

/* This example is based on other example written by Claudio for
   EOG */

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "hildon.h"

typedef struct {
    GtkWidget *pannable;
    GtkWidget *treeview;
} ApplicationContext;

enum {
    PIXBUF_COLUMN,
    TEXT_COLUMN,
    N_COLUMNS};

enum {
    COLUMN_PROP_NAME = 0,
    COLUMN_PROP_VALUE,
    COLUMN_PROP_TYPE,
    N_COLS
};

static GtkListStore *
create_list_store (void)
{
    GtkListStore *store;

    store = gtk_list_store_new (N_COLS,
                                G_TYPE_STRING,
                                G_TYPE_STRING,
                                G_TYPE_GTYPE);

    return store;
}

static void
list_store_fill (GtkListStore *store,
                 GObject *object)
{
    guint n_properties, i;
    GParamSpec **param_specs;
    GtkTreeIter iter;
    GObjectClass *objectclass;
    gdouble f_value;
    gint i_value;
    gchar *s_value = NULL;

    objectclass = G_OBJECT_GET_CLASS (object);
    param_specs = g_object_class_list_properties (objectclass,
                                                  &n_properties);

    for (i = 0; i < n_properties; i++) {
        if (param_specs [i]->owner_type == G_TYPE_FROM_CLASS (objectclass)) {
            switch (param_specs[i]->value_type) {
            case G_TYPE_DOUBLE:
                g_object_get (object,
                              param_specs[i]->name, &f_value, NULL);
                s_value = g_strdup_printf ("%f", f_value);
                break;
            case G_TYPE_OBJECT:
                break;
           case G_TYPE_BOOLEAN:
           case G_TYPE_ENUM:
           case G_TYPE_UINT:
           case G_TYPE_INT:
                g_object_get (object,
                              param_specs[i]->name, &i_value, NULL);
                s_value = g_strdup_printf ("%d", i_value);
                break;
            }

            if (s_value) {
                gtk_list_store_append (store, &iter);
                gtk_list_store_set (store, &iter,
                                    COLUMN_PROP_NAME, param_specs[i]->name,
                                    COLUMN_PROP_VALUE, s_value,
                                    COLUMN_PROP_TYPE, param_specs[i]->value_type,
                                    -1);

                g_free (s_value);
                s_value = NULL;
            }
        }
    }

    g_free (param_specs);
}

static void
cell_edited_cb (GtkCellRendererText *renderer,
                gchar               *path_string,
                gchar               *s_value,
                gpointer             user_data)
{
    ApplicationContext *app_ctx;
    GObjectClass *objectclass;
    GParamSpec **param_specs;
    gint column;
    GtkTreePath *path;
    GtkTreeModel *model;
    gdouble f_value;
    gint i_value;
    gchar *p_name;
    GtkTreeIter iter;
    guint n_properties, i = 0;

    app_ctx = (ApplicationContext *)user_data;

    objectclass = G_OBJECT_GET_CLASS (app_ctx->pannable);

    model = gtk_tree_view_get_model (GTK_TREE_VIEW (app_ctx->treeview));
    path = gtk_tree_path_new_from_string (path_string);
    gtk_tree_model_get_iter (model, &iter, path);
    gtk_tree_path_free (path);

    gtk_tree_model_get (model, &iter,
                        COLUMN_PROP_NAME, &p_name,
                        -1);

    param_specs = g_object_class_list_properties (objectclass,
                                                  &n_properties);

    column = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (renderer),
                                                 "column"));

    while (strcmp (param_specs[i]->name, p_name)) i++;

    switch (param_specs[i]->value_type) {
    case G_TYPE_DOUBLE:
        f_value = g_ascii_strtod (s_value, NULL);
        g_object_set (G_OBJECT (app_ctx->pannable),
                      p_name, f_value, NULL);
        break;
    case G_TYPE_BOOLEAN:
    case G_TYPE_ENUM:
    case G_TYPE_UINT:
    case G_TYPE_INT:
        i_value = atoi (s_value);
        g_object_set (G_OBJECT (app_ctx->pannable),
                      p_name, i_value, NULL);
        break;
    }

    gtk_list_store_set (GTK_LIST_STORE (model), &iter,
                        column, s_value,
                        -1);

    g_free (param_specs);

    g_free (p_name);
}

static GtkWidget *
create_attributes_treeview (ApplicationContext *app_ctx)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeView *treeview;

    treeview = GTK_TREE_VIEW (gtk_tree_view_new ());

    renderer = gtk_cell_renderer_text_new ();
    column = gtk_tree_view_column_new_with_attributes ("Property",
                                                       renderer,
                                                       "text",
                                                       COLUMN_PROP_NAME,
                                                       NULL);
    gtk_tree_view_column_set_sort_column_id (column, COLUMN_PROP_NAME);
    gtk_tree_view_append_column (treeview, column);

    renderer = gtk_cell_renderer_text_new ();
    g_object_set (renderer, "editable", TRUE, NULL);
    column = gtk_tree_view_column_new_with_attributes ("Value",
                                                       renderer,
                                                       "text", COLUMN_PROP_VALUE,
                                                       NULL);
    gtk_tree_view_column_set_sort_column_id (column, COLUMN_PROP_VALUE);
    gtk_tree_view_append_column (treeview, column);
    g_signal_connect (renderer, "edited",
                      G_CALLBACK (cell_edited_cb), app_ctx);
    g_object_set_data (G_OBJECT (renderer), "column",
                       GINT_TO_POINTER (COLUMN_PROP_VALUE));

    return GTK_WIDGET (treeview);
}

static void
on_notify_cb (GObject *object,
              GParamSpec *param_spec,
              gpointer user_data)
{
    GtkTreeModel *store;
    GtkTreeIter iter;
    gchar *par_name;
    gfloat f_value;
    gchar s_value[128];

    store = GTK_TREE_MODEL (user_data);

    gtk_tree_model_get_iter_first (store, &iter);

    do {

      gtk_tree_model_get (store, &iter,
                          COLUMN_PROP_NAME, &par_name,
                          -1);
      if (strcmp (par_name, param_spec->name) == 0 &&
          param_spec->value_type == G_TYPE_FLOAT) {
        g_object_get (object,
                      param_spec->name, &f_value,
                      NULL);
        g_sprintf (s_value, "%f", f_value);
        gtk_list_store_set (GTK_LIST_STORE (store), &iter,
                            COLUMN_PROP_VALUE, s_value,
                            -1);
      }

      g_free (par_name);

    } while (gtk_tree_model_iter_next (store, &iter));
}

static GtkWidget*
create_pannable_treeview (void)
{
    GtkWidget *tv;
    GtkTreeViewColumn *col;
    GtkCellRenderer *renderer;
    GtkListStore *store;
    GSList *stocks, *item;
    gint i;

    /* Create a treeview */
    tv = gtk_tree_view_new ();

    g_object_set (tv, "fixed-height-mode", TRUE, NULL);

    col = gtk_tree_view_column_new ();
    gtk_tree_view_column_set_sizing (col, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_title (col, "Title");

    g_object_set (tv, "fixed-height-mode", TRUE, NULL);
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

    return tv;
}
gint
main (gint argc, gchar **argv)
{
    HildonProgram *program;
    GtkWidget *window;
    GtkWidget *pannable;
    GtkWidget *hbox, *vbox;
    GtkWidget *treeview, *pannable_treeview;
    GtkWidget *sw;
    GtkListStore *store;
    ApplicationContext *app_ctx = NULL;

    hildon_gtk_init (&argc, &argv);

    program = hildon_program_get_instance ();

    /* Create the main window */
    window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));

    gtk_container_set_border_width (GTK_CONTAINER (window), 5);

    g_signal_connect (G_OBJECT (window), "delete-event",
                      G_CALLBACK (gtk_main_quit), NULL);

    pannable = hildon_pannable_area_new ();

    app_ctx = g_new0 (ApplicationContext, 1);
    app_ctx->pannable = pannable;

    pannable_treeview = create_pannable_treeview();

    gtk_container_add (GTK_CONTAINER (pannable),
                       pannable_treeview);

    treeview = create_attributes_treeview (app_ctx);

    app_ctx->treeview = treeview;
    store = create_list_store ();

    gtk_tree_view_set_model (GTK_TREE_VIEW (treeview),
                             GTK_TREE_MODEL (store));

    list_store_fill (store, G_OBJECT (pannable));

    sw = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                         GTK_SHADOW_ETCHED_IN);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                    GTK_POLICY_NEVER,
                                    GTK_POLICY_AUTOMATIC);

    gtk_container_add (GTK_CONTAINER (sw), treeview);

    hbox = gtk_hbox_new (FALSE, 10);

    vbox = gtk_vbox_new (FALSE, 10);
    gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 10);

    gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, FALSE, 10);

/*     vbox = gtk_vbox_new (FALSE, 10); */
/*     gtk_box_pack_start (GTK_BOX (vbox), pannable, TRUE, TRUE, 10); */
    gtk_box_pack_start (GTK_BOX (hbox), pannable, TRUE, TRUE, 10);

    g_signal_connect_after (pannable, "notify",
                            G_CALLBACK (on_notify_cb), store);

    gtk_window_set_default_size (GTK_WINDOW (window), 600, 400);
    gtk_container_add (GTK_CONTAINER (window), hbox);

    gtk_widget_show_all (window);

    gtk_main ();

    g_free (app_ctx);

    return 0;
}
