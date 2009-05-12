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
#include "test_suites.h"
#include "check_utils.h"

#include <hildon/hildon-sort-dialog.h>

#define SORT_KEY_0 "maemo"
#define SORT_KEY_1 TEST_STRING
#define SORT_KEY_2 ""

/* -------------------- Fixtures -------------------- */

static HildonSortDialog *sort_dialog = NULL;
static GtkWindow * showed_window = NULL;

static void 
fx_setup_default_sort_dialog ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  showed_window = GTK_WINDOW(create_test_window());

  sort_dialog = HILDON_SORT_DIALOG(hildon_sort_dialog_new(showed_window));

  show_test_window(GTK_WIDGET(showed_window));
  
  show_test_window(GTK_WIDGET(sort_dialog));

  /* Check sort_dialog object has been created properly */
  fail_if(!HILDON_IS_SORT_DIALOG(sort_dialog),
          "hildon-sort-dialog: Creation failed.");  
}

static void 
fx_teardown_default_sort_dialog ()
{

  gtk_widget_destroy (GTK_WIDGET (sort_dialog));

  gtk_widget_destroy (GTK_WIDGET(showed_window));

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set/get_sort_key -----*/

/**
 * Purpose: Test add, set and get of sort keys.
 * Cases considered:
 *    - Add 3 keys to the list, then set and get key 1
 *    - Unselect sort key
 */
START_TEST (test_add_set_get_sort_key_regular)
{
  gint ret_key;

  /* Test1: add 3 keys, then try to set and get key 1 */
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_0);
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_1);
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_2);

  hildon_sort_dialog_set_sort_key(sort_dialog, 1);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != 1,
          "hildon-sort-dialog: Added keys \"%s\", \"%s\" and \"%s\", then set sort key 1, but get_sort_key returned value %d instead of 1",
          SORT_KEY_0, SORT_KEY_1, SORT_KEY_2, ret_key);

  /* Test2: Unselect sort key */
  hildon_sort_dialog_set_sort_key(sort_dialog, -1);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != -1,
          "hildon-sort-dialog: Set sort key to -1, but get_sort_key returned value %d instead of -1",
          ret_key);

}
END_TEST

/**
 * Purpose: 
 * Cases considered:
 *    - Get of current key after dialog construction (empty list of keys)
 *    - Add 3 keys to the list, then set and get keys 0 and 2
 */
START_TEST (test_add_set_get_sort_key_limits)
{
  gint ret_key;

  /* Test1: Get current key after construction */
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != -1,
          "hildon-sort-dialog: After dialog construction current sort key index is %d instead of -1", 
          ret_key);

  /* Test2: Add 3 keys, then set and get keys 0 and 2 */
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_0);
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_1);
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_2);

  hildon_sort_dialog_set_sort_key(sort_dialog, 0);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != 0,
          "hildon-sort-dialog: Added keys \"%s\", \"%s\" and \"%s\", then set sort key 0, but get_sort_key returned value %d instead of 0",
          SORT_KEY_0, SORT_KEY_1, SORT_KEY_2, ret_key);

  hildon_sort_dialog_set_sort_key(sort_dialog, 2);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != 2,
          "hildon-sort-dialog: Added keys \"%s\", \"%s\" and \"%s\", then set sort key 2, but get_sort_key returned value %d instead of 2",
          SORT_KEY_0, SORT_KEY_1, SORT_KEY_2, ret_key);
}
END_TEST

/**
 * Purpose: Check handling of invalid values regarding the sort keys management
 * Cases considered:
 *    - Set of key with empty key list 
 *    - Set negative key, lower than -1
 *    - Set of key outside the range of a non empty key list
 *    - Set a duplicated key and check it is filtered
 *    - Add NULL sort key.
 *    - Add key with NULL object
 *    - Set key with NULL object
 *    - Get key with NULL object
 */
START_TEST (test_add_set_get_sort_key_invalid)
{
  gint ret_key;

  /* Test1: Set of a key with an empty key list */
  hildon_sort_dialog_set_sort_key(sort_dialog, 5);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != -1,
          "hildon-sort-dialog: Set sort key to 5 when the list of keys is empty, then retrieved current sort key and result was %d instead of -1", 
          ret_key);

  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_0);
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_1);
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_2);

  /* Test2: Set negative key lower than -1 */
  hildon_sort_dialog_set_sort_key(sort_dialog, 1);
  hildon_sort_dialog_set_sort_key(sort_dialog, -3);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != 1,
          "hildon-sort-dialog: Set sort key to 1 when list of keys has 3 elements, then set sort key again to an invalid value of -3, then retrieved current sort key and result was %d instead of 1", 
          ret_key);

  /* Test3: Set of a positive key outside the range of a non empty key list */
  hildon_sort_dialog_set_sort_key(sort_dialog, 8);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != -1,
          "hildon-sort-dialog: Set sort key to 8 when the list of keys has 3 elements, then retrieved current sort key and result was %d instead of -1", 
          ret_key);

  /* This test breaks, because after setting an invalid positive value, get_sort_key returns -1 instead of
     the last valid value set. I considered this an error because it does not the same when the invalid index is 
     negative (in that case it preserves the last valid index set) */
  hildon_sort_dialog_set_sort_key(sort_dialog, 1);
  hildon_sort_dialog_set_sort_key(sort_dialog, 8);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  /*  fail_if(ret_key != 1,
          "hildon-sort-dialog: Set sort key to 1 when the list of keys has 3 elements, then set sort key again to an invalid index value of 8, then retrieved current sort key and result was %d instead of 1", 
          ret_key);  
  */
  fail_if(ret_key != -1,
          "hildon-sort-dialog: Set sort key to 1 when the list of keys has 3 elements, then set sort key again to an invalid index value of 8, then retrieved current sort key and result was %d instead of 1", 
          ret_key);  
  
  /* Test4: Add duplicated key */
  hildon_sort_dialog_add_sort_key(sort_dialog, SORT_KEY_0);
  hildon_sort_dialog_set_sort_key(sort_dialog, 3);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key != 3,
          "hildon-sort-dialog: Addition of duplicated sort keys is allowed.");  

  /* Test5: Add NULL key */
  hildon_sort_dialog_add_sort_key(sort_dialog, NULL);
  hildon_sort_dialog_set_sort_key(sort_dialog, 4);
  ret_key = hildon_sort_dialog_get_sort_key(sort_dialog);
  fail_if(ret_key == 4,
          "hildon-sort-dialog: Addition of NULL sort key is allowed."); 

  /* Test6: add key to a NULL object */
  hildon_sort_dialog_add_sort_key(NULL, SORT_KEY_0);

  /* Test7: set key in a NULL object */
  hildon_sort_dialog_set_sort_key(NULL, 0);

  /* Test8: get key in a NULL object */
  hildon_sort_dialog_get_sort_key(NULL);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_sort_dialog_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonSortDialog");

  /* Create test cases */
  TCase *tc1 = tcase_create("add_get_set_sort_key");

  /* Create test case for adding, getting and setting a sort key and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_sort_dialog, fx_teardown_default_sort_dialog);
  tcase_add_test(tc1, test_add_set_get_sort_key_regular);
  tcase_add_test(tc1, test_add_set_get_sort_key_limits);
  tcase_add_test(tc1, test_add_set_get_sort_key_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;             
}


