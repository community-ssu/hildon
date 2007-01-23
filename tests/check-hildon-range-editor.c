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
#include "test_suites.h"
#include "check_utils.h"

#include "hildon-range-editor.h"
#include <unistd.h>

/* -------------------- Fixtures -------------------- */

static GtkWidget *showed_window = NULL;
static HildonRangeEditor *range_editor = NULL;

static void
fx_setup_default_range_editor ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new());

  showed_window =  create_test_window ();

  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (range_editor));

  /* Displays the widget and the window */
  show_all_test_window (showed_window);

  /* Check range_editor object has been created properly */
  fail_if(!HILDON_IS_RANGE_EDITOR(range_editor),
          "hildon-range-editor: Creation failed.");


  while (gtk_events_pending ())
    {
      gtk_main_iteration ();
    }

}

static void 
fx_teardown_default_range_editor ()
{

  /* Destroy the window */
  gtk_widget_destroy (showed_window);
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_limit, get_min, get_max -----*/

/**
 * Purpose: Check that regular range limit values are set and get properly
 * Cases considered:
 *    - Set limits to (-10,10) and get min/max using the range editor object
 *    - Set limits to (0,10) and get min/max using the range editor object
 *    - Set limits to (-10,0) and get min/max using the range editor object
 *    - Set limits to (1,10) and get min/max using the range editor object
 *    - Set limits to (-10,-10) and get min/max using the range editor object
 */
START_TEST (test_set_limits_get_min_get_max_regular)
{
  gint range_start, range_end;

  /* Test1: Set limits to (-10,10) */
  hildon_range_editor_set_limits(range_editor, -10, 10);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != -10, 
          "hildon-range-editor: set limits to (-10,10) but get_min didn't return -10");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 10, 
          "hildon-range-editor: set limits to (-10,10) but get_max didn't return 10");

  /* Test2: Set limits to (0,10) */
  hildon_range_editor_set_limits(range_editor, 0, 10);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != 0, 
          "hildon-range-editor: set limits to (0,10) but get_min didn't return 0");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 10, 
          "hildon-range-editor: set limits to (0,10) but get_max didn't return 10");

  /* Test3: Set limits to (-10,0) */
  hildon_range_editor_set_limits(range_editor, -10, 0);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != -10, 
          "hildon-range-editor: set limits to (-10,0) but get_min didn't return -10");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 0, 
          "hildon-range-editor: set limits to (-10,0) but get_max didn't return 0");

  /* Test4: Set limits to (1,10) */
  hildon_range_editor_set_limits(range_editor, 1, 10);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != 1, 
          "hildon-range-editor: set limits to (1,10) but get_min didn't return 1");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 10, 
          "hildon-range-editor: set limits to (1,10) but get_max didn't return 10");

  /* Test5: Set limits to (-10,-1) */
  hildon_range_editor_set_limits(range_editor, -10, -1);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != -10, 
          "hildon-range-editor: set limits to (-10,-1) but get_min didn't return -10");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != -1, 
          "hildon-range-editor: set limits to (-10,-1) but get_max didn't return -1");
}
END_TEST

/**
 * Purpose: Check that range limit values are set and get properly
 * Cases considered:
 *    - Set limits to (G_MININT,G_MAXINT) and get min/max using the range editor object
 */
START_TEST (test_set_limits_get_min_get_max_limits)
{
  gint range_start, range_end;

  /* Test1: Set limits to (G_MININT,G_MAXINT) */
  hildon_range_editor_set_limits(range_editor, G_MININT, G_MAXINT);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != G_MININT, 
          "hildon-range-editor: set limits to (G_MININT,G_MAXINT) but get_min didn't return G_MININT");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != G_MAXINT, 
          "hildon-range-editor: set limits to (G_MININT,G_MAXINT) but get_max didn't return G_MAXINT");
}
END_TEST

/**
 * Purpose: Check that invalid values are handled properly
 * Cases considered:
 *    - Set inverted limits (10,-10)
 *    - Set range editor object to NULL for set_limits
 *    - Set range editor object to NULL for get_min and get_max
 */
START_TEST (test_set_limits_get_min_get_max_invalid)
{
  gint range_start, range_end;

  /* Test1: Set limits to (10,-10) */
  hildon_range_editor_set_limits(range_editor, 10, -10);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != -10, 
          "hildon-range-editor: set inverted limits to (10,-10) expecting to set limits to (-10,-10) but get_min didn't return -10");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != -10, 
          "hildon-range-editor: set inverted limits to (10,-10) expecting to set limits to (-10,-10) but get_max didn't return -1");

  /* Test2: set range editor to NULL for set_limits. */
  hildon_range_editor_set_limits(NULL, 100, -100);

  /* Test3: set range editor to NULL for get_min and get_max */
  range_start = hildon_range_editor_get_min(NULL);
  fail_if(range_start != 0, 
          "hildon-range-editor: setting range editor to NULL produced get_min to return a value != 0");
  range_end = hildon_range_editor_get_max(NULL);
  fail_if(range_end != 0, 
          "hildon-range-editor: setting range editor to NULL produced get_max to return a value != 0");
}
END_TEST


/* ----- Test case for set_min -----*/

/**
 * Purpose: Check regular minimum values for limits are set properly
 * Cases considered:
 *    - Set min limit to -100
 *    - Set min limit to 0
 *    - Set min limit to 100
 */
START_TEST (test_set_min_regular)
{
  gint range_start;

  /* Test1: Set min limits -100 */
  hildon_range_editor_set_min(range_editor, -100);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != -100, 
          "hildon-range-editor: set min limit to -100 but get_min didn't return -100");

  /* Test2: Set min limits 0 */
  hildon_range_editor_set_min(range_editor, 0);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != 0, 
          "hildon-range-editor: set min limit to 0 but get_min didn't return 0");

  /* Test3: Set min limits 100 */
  hildon_range_editor_set_min(range_editor, 100);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != 100, 
          "hildon-range-editor: set min limit to 100 but get_min didn't return 100");
}
END_TEST

/**
 * Purpose: Check limit minimum values for limits are set properly
 * Cases considered:
 *    - Set min limit to G_MININT
 */
START_TEST (test_set_min_limits)
{
  gint range_start;

  /* Test1: Set min limit to G_MININT */
  hildon_range_editor_set_min(range_editor, G_MININT);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != G_MININT, 
          "hildon-range-editor: set min limit to G_MININT but get_min didn't return G_MININT");
}
END_TEST

/**
 * Purpose: Check that invalid values are handled properly
 * Cases considered:
 *    - Set minimum limit greater than maximum limit
 *    - Set range editor object to NULL for set_min
 */
START_TEST (test_set_min_invalid)
{
  gint range_start, range_end;

  /* Test1: Set minimum limit greater than maximum limit */
  hildon_range_editor_set_limits(range_editor, -10, 10);
  hildon_range_editor_set_min(range_editor, 15);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != 15, 
          "hildon-range-editor: set min limit to 15 when max limit is 10 expecting to set limits to (15,15) but get_min didn't return 15");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 15, 
          "hildon-range-editor: set min limit to 15 when max limit is 10 expecting to set limits to (15,15) but get_max didn't return 15");

  /* Test2: set range editor to NULL */
  hildon_range_editor_set_min(NULL, 15);
}
END_TEST


/* ----- Test case for set_max -----*/

/**
 * Purpose: Check regular maximum values for limits are set properly
 * Cases considered:
 *    - Set max limit to -100
 *    - Set max limit to 0
 *    - Set max limit to 100
 */
START_TEST (test_set_max_regular)
{
  gint range_end;

  /* Test1: Set max limits -100 */
  hildon_range_editor_set_max(range_editor, -100);
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != -100, 
          "hildon-range-editor: set max limit to -100 but get_max didn't return -100");

  /* Test2: Set max limits 0 */
  hildon_range_editor_set_max(range_editor, 0);
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 0, 
          "hildon-range-editor: set max limit to 0 but get_max didn't return 0");

  /* Test3: Set max limits 100 */
  hildon_range_editor_set_max(range_editor, 100);
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != 100, 
          "hildon-range-editor: set max limit to 100 but get_max didn't return 100");
}
END_TEST

/**
 * Purpose: Check limit maximum values for limits are set properly
 * Cases considered:
 *    - Set min limit to G_MAXINT
 */
START_TEST (test_set_max_limits)
{
  gint range_end;

  /* Test1: Set max limit to G_MAXINT */
  hildon_range_editor_set_max(range_editor, G_MAXINT);
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != G_MAXINT, 
          "hildon-range-editor: set min limit to G_MAXINT but get_max didn't return G_MAXINT");
}
END_TEST

/**
 * Purpose: Check that invalid values are handled properly
 * Cases considered:
 *    - Set maximum limit lower than minimum limit
 *    - Set range editor object to NULL for set_max
 */
START_TEST (test_set_max_invalid)
{
  gint range_start, range_end;

  /* Test1: Set maximum limit lower than minimum limit */
  hildon_range_editor_set_limits(range_editor, -10, 10);
  hildon_range_editor_set_max(range_editor, -15);
  range_start = hildon_range_editor_get_min(range_editor);
  fail_if(range_start != -15, 
          "hildon-range-editor: set max limit to -15 when min limit is -10 expecting to set limits to (-15,-15) but get_min didn't return -15");
  range_end = hildon_range_editor_get_max(range_editor);
  fail_if(range_end != -15, 
          "hildon-range-editor: set min limit to -15 when min limit is -10 expecting to set limits to (-15,-15) but get_max didn't return -15");

  /* Test2: set range editor to NULL */
  hildon_range_editor_set_max(NULL, 15);
}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_range_editor_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonRangeEditor");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_limits_get_min_get_max");
  TCase *tc2 = tcase_create("set_min");
  TCase *tc3 = tcase_create("set_max");

  /* Create test case for set_limits, get_min and get_max and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_range_editor, fx_teardown_default_range_editor);
  tcase_add_test(tc1, test_set_limits_get_min_get_max_regular);
  tcase_add_test(tc1, test_set_limits_get_min_get_max_limits);
  tcase_add_test(tc1, test_set_limits_get_min_get_max_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for set_min */
  tcase_add_checked_fixture(tc2, fx_setup_default_range_editor, fx_teardown_default_range_editor);
  tcase_add_test(tc2, test_set_min_regular);
  tcase_add_test(tc2, test_set_min_limits);
  tcase_add_test(tc2, test_set_min_invalid);
  suite_add_tcase (s, tc2);

  /* Create test case for set_max */
  tcase_add_checked_fixture(tc3, fx_setup_default_range_editor, fx_teardown_default_range_editor);
  tcase_add_test(tc3, test_set_max_regular);
  tcase_add_test(tc3, test_set_max_limits);
  tcase_add_test(tc3, test_set_max_invalid);
  suite_add_tcase (s, tc3);

  /* Return created suite */
  return s;
}


