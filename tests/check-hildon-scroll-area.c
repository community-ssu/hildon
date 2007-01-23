/*
 * This file is a part of hildon tests
 *
 * Copyright (C) 2006, 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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

#include <stdlib.h>
#include <check.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtkcellrenderer.h>
#include <gtk/gtktreeviewcolumn.h>
#include <gtk/gtktreemodel.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtktreeview.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkcellrenderertext.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkfixed.h>
#include <gtk/gtklabel.h>
#include <glib/gprintf.h>
#include "test_suites.h"

#include "hildon-scroll-area.h"

enum
  {
    COL_NAME = 0,
    COL_AGE,
    NUM_COLS
  } ;

/* -------------------- Fixtures -------------------- */

static void 
fx_setup_default_scroll_area ()
{
  int argc = 0;

  gtk_init(&argc, NULL);
}

/* -------------------- Test cases -------------------- */

/* ------------ Test case for scroll-area --------------*/

/**
 * 
 * Helper function used to create the tests, it creates a treemodel
 * and inserts test data to it. This function it is used in the
 * create_view_and_model function.
 * 
 */
static GtkTreeModel *
create_and_fill_model (void)
{
  GtkListStore  *store;
  GtkTreeIter    iter;
  gint i = 0;  
  store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

  for(i=0; i< 20; i++)
    {
      /* Append a row and fill in some data */
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          COL_NAME, "Heinz El-Mann",
                          COL_AGE, 51,
                          -1);
  
      /* append another row and fill in some data */
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          COL_NAME, "Jane Doe",
                          COL_AGE, 23,
                          -1);
  
      /* ... and a third row */
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          COL_NAME, "Joe Bungop",
                          COL_AGE, 91,
                          -1);
    }  
  return GTK_TREE_MODEL (store);
}

/**
 * 
 * Helper function used to create the tests, it creates a treeview and
 * a treemodel, it returns the treeview with the treemodel associated.
 * 
 */
static GtkWidget *
create_view_and_model (void)
{
  GtkTreeViewColumn   *col;
  GtkCellRenderer     *renderer;
  GtkTreeModel        *model;
  GtkWidget           *view;

  view = gtk_tree_view_new ();

  /* --- Column #1 --- */

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Name",  
                                               renderer,
                                               "text", COL_NAME,
                                               NULL);

  /* --- Column #2 --- */

  col = gtk_tree_view_column_new();

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,      
                                               "Age",  
                                               renderer,
                                               "text", COL_AGE,
                                               NULL);

  model = create_and_fill_model ();

  gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

  g_object_unref (model); /* destroy model automatically with view */

  return view;
}

/**
 * Purpose: Check the construction of the scroll-area
 * Cases considered:
 *    - Create a regular case of construction
 *    - Create a scroll area with a label, it is not a proper case but it is 
 *      returning a proper value
 */
START_TEST (test_create_scroll_area_regular)
{
  GtkWidget *button[2];
  GtkWidget *box;
  GtkWidget *scroll_window;
  GtkWidget *treew;
  GtkWidget *fixed;
  GtkWidget *label;
  gint i;

  /* Test1: Create a regular case of construction*/
  box = gtk_vbox_new(FALSE,0);
  scroll_window = gtk_scrolled_window_new(NULL, NULL);

  treew = create_view_and_model();

  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  
  for(i=0; i<2; i++)
    button[i] = gtk_button_new_with_label("Junk button");

  gtk_box_pack_start_defaults(GTK_BOX(box), button[0]);

  fixed = hildon_scroll_area_new(scroll_window, treew);

  /* Check the return value is a proper gtk_fixed object */
  if (!GTK_FIXED(fixed))
    {
      gtk_widget_destroy (GTK_WIDGET (scroll_window));
      gtk_widget_destroy (GTK_WIDGET (box));
      gtk_widget_destroy (GTK_WIDGET (treew));
      fail("hildon-scroll-area: Creation failed.");
    }

  gtk_box_pack_start_defaults(GTK_BOX(box), fixed);
  gtk_box_pack_start_defaults(GTK_BOX(box), button[1]);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_window), box);

  gtk_widget_destroy (GTK_WIDGET (scroll_window));

  /* Test2: Create a scroll area with a label, it is not a proper case but it is 
     returning a proper value */
  scroll_window = gtk_scrolled_window_new(NULL, NULL);

  label = gtk_label_new("This is a label widget example");
  fixed = hildon_scroll_area_new(scroll_window, label);

  /* Check the return value is a fixed value when we try to create the scroll area with a label */
  if (!GTK_FIXED(fixed)) 
    {
      gtk_widget_destroy (GTK_WIDGET (label));
      gtk_widget_destroy (GTK_WIDGET (scroll_window));
      fail("hildon-scroll-area: Creation with a label did not return a proper fixed object");
    }
  gtk_widget_destroy (GTK_WIDGET (label));
  gtk_widget_destroy (GTK_WIDGET (scroll_window));
}
END_TEST

/**
 * Purpose: Check the construction of the scroll-area
 *          invalid values.
 * Cases considered:
 *    - Create with NULL widgets
 *    - Create with actual invalid widget instead of the scrolled-window
 *    - Create with actual invalid widget instead of the treeview
 */
START_TEST (test_create_scroll_area_invalid)
{
  GtkWidget *fixed;
  GtkWidget *label;
  GtkWidget *treew;
  GtkWidget *scroll_window;

  fixed = hildon_scroll_area_new(NULL, NULL);

  /* Test1: Create with NULL widgets */
  fail_if(fixed != NULL,
          "hildon-scroll-area: Invalid creation did not return a NULL value.");

  treew = create_view_and_model();
  label = gtk_label_new("This is an invalid example widget");
  fixed = hildon_scroll_area_new(label, treew);

  /* Test2: Create with actual invalid widget instead of the scrolled-window */
  if (GTK_FIXED(fixed)) 
    {    
      gtk_widget_destroy (GTK_WIDGET (label));
      gtk_widget_destroy (GTK_WIDGET (treew));
      fail ("hildon-scroll-area: Invalid creation did not return a NULL value when we set an invalid value in the first parameter.");
    }
  
  gtk_widget_destroy (GTK_WIDGET (label));
  gtk_widget_destroy (GTK_WIDGET (treew));

  scroll_window = gtk_scrolled_window_new(NULL, NULL);
  fixed = hildon_scroll_area_new(scroll_window, label);

  /* Test3: Create with actual invalid widget instead of the treeview */
  if (!GTK_FIXED(fixed))
    {      
      gtk_widget_destroy (GTK_WIDGET (scroll_window));
      fail ("hildon-scroll-area: Invalid creation returned a NULL value when we set an invalid value in the second parameter.");
    }
  
  gtk_widget_destroy (GTK_WIDGET (scroll_window));
  
}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_scroll_area_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonScrollArea");

  /* Create test cases */
  TCase *tc1 = tcase_create("create_scroll_area");

  /* Create test case for scroll-area and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_scroll_area, NULL);
  tcase_add_test(tc1, test_create_scroll_area_regular);
  tcase_add_test(tc1, test_create_scroll_area_invalid);
  suite_add_tcase(s, tc1);

  /* Return created suite */
  return s;
}
