/*
 * Copyright (C) 2006 Nokia Corporation.
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

#include <stdlib.h>
#include <check.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkwindow.h>
#include <gtk/gtkentry.h>
#include "test_suites.h"
#include "check_utils.h"
#include <unistd.h>

#include "hildon-composite-widget.h"
#include "hildon-time-editor.h"

/* -------------------- Fixtures ---------------------- */

static void 
fx_setup_gtk ()
{
  int argc = 0;
  gtk_init(&argc, NULL);
}

static void 
fx_teardown_gtk ()
{
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for focus -----*/

/**
 * Purpose: Test regular usage
 * Cases considered:
 *    - Check with a composite widget (hildon_time_editor)
 *    - Check with a composite widget (hildon_time_editor) inside a window
 *    - check with a composite widget (hildon_time_editor) inside a window and a focus already given to the window
 */
START_TEST (test_focus_regular)
{
  GtkWidget *time_editor;
  GtkWindow *window;

  /* Test1: check with a composite widget */
  time_editor = hildon_time_editor_new();

  hildon_composite_widget_focus(time_editor, GTK_DIR_RIGHT);

  gtk_widget_destroy(GTK_WIDGET(time_editor));

  /* Test2: check with a composite widget (inside a window) */
  window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  time_editor = hildon_time_editor_new();
  gtk_container_add(GTK_CONTAINER(window), time_editor);

  /* show widget */
  show_all_test_window(GTK_WIDGET(window));

  hildon_composite_widget_focus(time_editor, GTK_DIR_RIGHT);

  gtk_widget_destroy(GTK_WIDGET(window));

  /* Test3: check with a composite widget (hildon_time_editor) inside a window and a focus already given to the window */
  window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  time_editor = hildon_time_editor_new();
  gtk_container_add(GTK_CONTAINER(window), time_editor);

  /* show widget */
  show_all_test_window(GTK_WIDGET(window));
  gtk_widget_grab_focus(GTK_WIDGET(window));
  hildon_composite_widget_focus(time_editor, GTK_DIR_RIGHT);

  gtk_widget_destroy(GTK_WIDGET(window));  
}
END_TEST

/**
 * Purpose: Test invalid usage
 * Cases considered:
 *    - Check with non composite widget (gtkentry)
 *    - Check with NULL object
 *    - Check with invalid direction
 *    - Check with non composite widget (gtkentry) (inside a window)
 *    - Check with invalid direction (inside a window)
 */
START_TEST (test_focus_invalid)
{
  GtkWidget *time_editor, *entry;
  GtkWindow *window;
  
  /* Test1: check with a composite widget */
  entry = gtk_entry_new();
  hildon_composite_widget_focus(entry, GTK_DIR_RIGHT);
  
  gtk_widget_destroy(GTK_WIDGET(entry));
  
  /* Test2: with NULL widget */
  hildon_composite_widget_focus(NULL, GTK_DIR_RIGHT);
  
  /* Test3: with invalid direction */
  time_editor = hildon_time_editor_new();
  hildon_composite_widget_focus(time_editor,GTK_DIR_RIGHT+1);

  gtk_widget_destroy(GTK_WIDGET(time_editor));
  
  /* Test4: Check with non composite widget (gtkentry) (inside a window) */
  window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  entry = gtk_entry_new();
  gtk_container_add(GTK_CONTAINER(window), entry);

  show_all_test_window(GTK_WIDGET(window));
  hildon_composite_widget_focus(entry, GTK_DIR_RIGHT);

  gtk_widget_destroy(GTK_WIDGET(window));
  
  /* Test5: Check with invalid direction (inside a window) */
  window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  time_editor = hildon_time_editor_new();
  gtk_container_add(GTK_CONTAINER(window), time_editor);

  show_all_test_window(GTK_WIDGET(window));
  hildon_composite_widget_focus(time_editor, GTK_DIR_RIGHT+1);

  gtk_widget_destroy(GTK_WIDGET(window));
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_composite_widget_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonCompositeWidget");

  /* Create test cases */
  TCase *tc1 = tcase_create("focus");

  /* Create test case for focus and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_gtk, fx_teardown_gtk);
  tcase_add_test(tc1, test_focus_regular);
  tcase_add_test(tc1, test_focus_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;             
}


