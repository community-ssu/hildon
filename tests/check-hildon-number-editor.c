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
#include <glib/gprintf.h>
#include "test_suites.h"
#include "check_utils.h"

#include "hildon-number-editor.h"

/* -------------------- Fixtures -------------------- */

static HildonNumberEditor *number_editor = NULL;
static GtkWidget *showed_window = NULL;

static void 
fx_setup_default_number_editor ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  number_editor = HILDON_NUMBER_EDITOR(hildon_number_editor_new(0, 100));
  /* Check number_editor object has been created properly */
  fail_if(!HILDON_IS_NUMBER_EDITOR(number_editor), 
          "hildon-number-editor: Creation failed.");

  showed_window =  create_test_window ();
  
  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (number_editor));
  
  /* Displays the widget and the window */
  show_all_test_window (showed_window);
}

static void 
fx_teardown_default_number_editor ()
{
  gtk_widget_destroy (GTK_WIDGET (number_editor));
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_range -----*/

/**
 * Purpose: Check that the ranges can be set safely
 * Cases considered:
 *    - Set a number limit of (0, 100) 
 *    - Set a number limit of (1, 100) 
 *    - Set a number limit of (-100, 100)
 *    - Set a number limit of (-100, 0)
 *    - Set a number limit of (-100, -1)
 */
START_TEST (test_set_range_regular)
{
  gint number;

  /* Test 1: Set limits to (0, 100) */
  hildon_number_editor_set_range(number_editor, 0, 100);

  hildon_number_editor_set_value(number_editor, 0);

  hildon_number_editor_set_value(number_editor, 101);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 0,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 and then we set the value 101 but we get %d instead of 0", 
	   number);

  /* Test 2: Set limits to (1, 100) */
  hildon_number_editor_set_range(number_editor, 1, 100);

  hildon_number_editor_set_value(number_editor, 1);

  hildon_number_editor_set_value(number_editor, 0);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 1,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 and then we set the value -1 but we get %d instead of 0",
	   number);

  /* Test 3: Set limits to (-100, 100) */
  hildon_number_editor_set_range(number_editor, -100, 100);

  hildon_number_editor_set_value(number_editor, 0);

  hildon_number_editor_set_value(number_editor, -101);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 0,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 and then we set the value -101 but we get %d instead of 0",
	   number);

  /* Test 4: Set limits to (-100, 0) */
  hildon_number_editor_set_range(number_editor, -100, 0);

  hildon_number_editor_set_value(number_editor, 0);

  hildon_number_editor_set_value(number_editor, 1);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 0,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 and then we set the value 1 but we get %d instead of 0", 
	   number);

  /* Test 5: Set limits to (-100, -1) */
  hildon_number_editor_set_range(number_editor, -100, -1);

  hildon_number_editor_set_value(number_editor, -1);

  hildon_number_editor_set_value(number_editor, 0);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != -1,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set -1 and then we set the value 0 but we get %d instead of -1", 
	   number);
}
END_TEST

/**
 * Purpose: Check that the ranges limits are controlled properly
 * Cases considered:
 *    - Set G_MAXINT and G_MININT
 *    - Set a number limit of (0, 100), and test that 0 and 100 work properly
 */
START_TEST (test_set_range_limits)
{
  gint number;

  /* Test 6: Set limits to (G_MININT, G_MAXINT) */
  hildon_number_editor_set_range(number_editor, G_MININT, G_MAXINT);

  /* Test 7: Set limits to (0,100) */
  hildon_number_editor_set_value(number_editor, 0);

  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 0,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 but we get %d instead of 0", 
	   number);

  hildon_number_editor_set_value(number_editor, 100);

  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 100,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 100 but we get %d instead of 100", 
	   number);
}
END_TEST

/**
 * Purpose: Check that the invalid values are controlled properly we
 * use set/get value to test if our first value is modified
 *
 * Cases considered:
 *    - Change the order of the minimum and maximum and check if it does not change (200, -2)
 *    - Set values where high value is less than low value (0, 100)
 *    - Set a range to a NULL widget
 */
START_TEST (test_set_range_invalid)
{
  gint number;
  
  /* Test 8: Set limits to (200, -2) */
  hildon_number_editor_set_range(number_editor, 200, -2);

  hildon_number_editor_set_value(number_editor, 0);

  /* we set 101 and it should not be set if method didn't reverse min and max, the value may continue being 0 */
  hildon_number_editor_set_value(number_editor, 101);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 101,
	   "hildon-number-editor: range_value failed, we set range: 200/-2, and it should not be set so 0/100 should continue being the range, we set 0 and then we set the value 101 but we get %d instead of 0", 
	   number);

  /* Test 9: Set limits to (100, 0) */
  hildon_number_editor_set_range(number_editor, 0, 100);

  hildon_number_editor_set_value(number_editor, 0);

  /* we set -5  and it should not be set, the value may continue being 0 */
  hildon_number_editor_set_value(number_editor, -5);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 0,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 and then we set the value -5 but we get %d instead of 0", 
	   number);

  /* we set 101  and it should not be set, the value may continue being 0 */
  hildon_number_editor_set_value(number_editor, 101);
  number = hildon_number_editor_get_value(number_editor);
  fail_if (number != 0,
	   "hildon-number-editor: range_value failed, we set range: 0/100, we set 0 and then we set the value 101 but we get %d instead of 0", 
	   number);

  /* Test 10: Set a range to a NULL widget */
  hildon_number_editor_set_range(NULL, 0, 100);

}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_number_editor_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonNumberEditor");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_number");

  /* Create test case for set_number and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_number_editor, fx_teardown_default_number_editor);
  tcase_add_test(tc1, test_set_range_regular);
  tcase_add_test(tc1, test_set_range_limits);
  tcase_add_test(tc1, test_set_range_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;             
}


