/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
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

#include <glib.h>
#include <gtk/gtk.h>

#include "hildon-program.h"
#include "hildon-stackable-window.h"

#include "hildon-touch-selector.h"
#include "hildon-picker-dialog.h"
#include "hildon-picker-button.h"

static GtkWidget *create_selector ();
static GtkWidget *get_visible_content (GtkWidget * window);

static GtkWindow *parent_window = NULL;

static GtkWidget *label = NULL;

static void
value_changed (HildonPickerButton * button,
               gpointer user_data)
{
  HildonTouchSelector *selector;
  gchar *aux_string = NULL;

  selector = hildon_picker_button_get_selector (button);
  aux_string = hildon_touch_selector_get_current_text (selector);
  gtk_label_set_text (GTK_LABEL (label), aux_string);
  g_free (aux_string);
}

static GtkWidget *
create_selector ()
{
  GtkWidget *selector = NULL;
  GSList *icon_list = NULL;
  GtkListStore *store_icons = NULL;
  GSList *item = NULL;
  GtkCellRenderer *renderer = NULL;
  GValue val = { 0, };
  GValue val2 = { 0, };

  selector = hildon_touch_selector_new ();

  icon_list = gtk_stock_list_ids ();

  store_icons = gtk_list_store_new (1, G_TYPE_STRING);
  for (item = icon_list; item; item = g_slist_next (item)) {
    GtkTreeIter iter;
    gchar *label = item->data;

    gtk_list_store_append (store_icons, &iter);
    gtk_list_store_set (store_icons, &iter, 0, label, -1);
    g_free (label);
  }
  g_slist_free (icon_list);

  renderer = gtk_cell_renderer_pixbuf_new ();
  g_value_init (&val, G_TYPE_INT);
  g_value_set_int (&val, 100);
  g_object_set_property (G_OBJECT (renderer), "height", &val);
  g_value_init (&val2, G_TYPE_STRING);
  g_value_set_string (&val2, "orange");
  g_object_set_property (G_OBJECT (renderer), "cell-background", &val2);

  hildon_touch_selector_append_column (HILDON_TOUCH_SELECTOR (selector),
                                       GTK_TREE_MODEL (store_icons),
                                       renderer, "stock-id", 0, NULL);

  hildon_touch_selector_set_column_selection_mode (HILDON_TOUCH_SELECTOR (selector),
                                                   HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE);

  return selector;
}

static GtkWidget *
get_visible_content (GtkWidget * window)
{
  GtkWidget *result = NULL;
  GtkWidget *button = NULL;
  GtkWidget *selector;

  label = gtk_label_new ("Here we are going to put the selection");

  button = hildon_picker_button_new (HILDON_BUTTON_WITH_VERTICAL_VALUE);
  hildon_button_set_title (HILDON_BUTTON (button), "Click me..");
  selector = create_selector ();
  hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (button),
                                     HILDON_TOUCH_SELECTOR (selector));

  g_signal_connect (G_OBJECT (button), "value-changed",
                    G_CALLBACK (value_changed), window);

  result = gtk_vbox_new (FALSE, 6);

  gtk_container_add (GTK_CONTAINER (result), button);
  gtk_container_add (GTK_CONTAINER (result), label);

  return result;
}

int
main (int argc, char **args)
{
  HildonProgram *program = NULL;
  GtkWidget *window = NULL;

  gtk_init (&argc, &args);

  program = hildon_program_get_instance ();
  g_set_application_name
    ("hildon-touch-selector cell renderer example program");

  window = hildon_stackable_window_new ();
  parent_window = GTK_WINDOW (window);
  hildon_program_add_window (program, HILDON_WINDOW (window));

  gtk_container_set_border_width (GTK_CONTAINER (window), 6);

  gtk_rc_parse_string ("style \"fremantle-widget\" {\n"
                       " GtkWidget::hildon-mode = 1 \n"
                       "} widget \"*.fremantle-widget\" style \"fremantle-widget\"\n");


  GtkWidget *vbox = get_visible_content (window);

  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (vbox));

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (GTK_WIDGET (window));

  gtk_main ();

  return 0;
}
