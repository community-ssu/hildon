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
#include <gtk/gtkcontainer.h>
#include "test_suites.h"
#include "check_utils.h"

#include <hildon/hildon-volumebar-range.h>

/* Copied from hildon-volumebar-range.h because
   there is no way to retrieve this values */
#define VOLUMEBAR_RANGE_INITIAL_VALUE 50.0
#define VOLUMEBAR_RANGE_MINIMUM_VALUE 0.0
#define VOLUMEBAR_RANGE_MAXIMUM_VALUE 100.0
#define CHANGE_THRESHOLD 0.001

/* -------------------- Fixtures -------------------- */

static GtkWidget *showed_window = NULL;
static HildonVolumebarRange *volumebar_range = NULL;

static void
fx_setup_default_volumebar_range ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  volumebar_range = HILDON_VOLUMEBAR_RANGE(hildon_volumebar_range_new(GTK_ORIENTATION_VERTICAL));

  showed_window =  create_test_window ();

  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (volumebar_range));

  /* Displays the widget and the window */
  show_all_test_window (showed_window);

  /* Check volumebar range object has been created properly */
  fail_if(!HILDON_VOLUMEBAR_RANGE(volumebar_range),
          "hildon-volumebar-range: Creation failed.");
}

static void
fx_teardown_default_volumebar_range ()
{

  /* Destroy the window */
  gtk_widget_destroy (showed_window);
}


/* -------------------- Test cases -------------------- */

/* ----- Test case for set/get_level -----*/

/**
 * Purpose: Check level usage
 * Cases considered:
 *    - Set level over the threshold limit 
 *    - Set level under the threshold limit.
 */
START_TEST (test_set_get_level_regular)
{
  gdouble old_level, level, ret_level;

  /* Test1: set level so change is over the threshold limit */
  level = VOLUMEBAR_RANGE_INITIAL_VALUE + 1.0 + CHANGE_THRESHOLD;
  hildon_volumebar_range_set_level(volumebar_range, level);
  ret_level = hildon_volumebar_range_get_level(volumebar_range);
  fail_if(ret_level != level,
          "hildon-volumebar-range: Set level to %.5f, but get level returned %.5f",
          level, ret_level);

  /* Test2: set level so change is under the threshold limit */
  old_level = ret_level;
  level += (gdouble) (CHANGE_THRESHOLD/2.0);
  hildon_volumebar_range_set_level(volumebar_range, level);
  ret_level = hildon_volumebar_range_get_level(volumebar_range);
  fail_if(ret_level == level,
          "hildon-volumebar-range: Set level to %.5f when current level is %.5f, but get level returned %.5f",
          level, old_level, ret_level);
}
END_TEST

/**
 * Purpose: Test handling of limit values
 * Cases considered:
 *    - Set level to VOLUMEBAR_RANGE_MINIMUM_VALUE
 *    - Set level to VOLUMEBAR_RANGE_MAXIMUM_VALUE
 */
START_TEST (test_set_get_level_limits)
{
  gdouble level, ret_level;

  /* Test1: Set level to minimum allowed  (first set it the a different value) */
  level = (VOLUMEBAR_RANGE_MINIMUM_VALUE - VOLUMEBAR_RANGE_MINIMUM_VALUE) / 2.0;
  hildon_volumebar_range_set_level(volumebar_range, level);
  level = VOLUMEBAR_RANGE_MINIMUM_VALUE;
  hildon_volumebar_range_set_level(volumebar_range, level);
  ret_level = hildon_volumebar_range_get_level(volumebar_range);
  fail_if(ret_level != level,
          "hildon-volumebar-range: Set level to %.5f, but get level returned %.5f",
          level, ret_level);  

  /* Test2: Set level to maximum allowed */
  level = VOLUMEBAR_RANGE_MAXIMUM_VALUE;
  hildon_volumebar_range_set_level(volumebar_range, level);
  ret_level = hildon_volumebar_range_get_level(volumebar_range);
  fail_if(ret_level != level,
          "hildon-volumebar-range: Set level to %.5f, but get level returned %.5f",
          level, ret_level); 
}
END_TEST

/**
 * Purpose: Test handling of invalid values 
 * Cases considered:
 *    - Set level under the minimum value
 *    - Set level over the maximum value
 *    - Set level with NULL object
 *    - Get level with NULL object
 */
START_TEST (test_set_get_level_invalid)
{
  gdouble level, ret_level;

  /* Test1: Set level under minimum value */
  level = VOLUMEBAR_RANGE_MINIMUM_VALUE - 1.0 - CHANGE_THRESHOLD;
  hildon_volumebar_range_set_level(volumebar_range, level);
  ret_level = hildon_volumebar_range_get_level(volumebar_range);
  fail_if(ret_level != VOLUMEBAR_RANGE_MINIMUM_VALUE,
          "hildon-volumebar-range: Set level to %.5f (under the minimum allowed), and get level returned %.5f",
          level, ret_level);

  /* Test2: Set level over maximum value */
  level = VOLUMEBAR_RANGE_MAXIMUM_VALUE + 1.0 + CHANGE_THRESHOLD;
  hildon_volumebar_range_set_level(volumebar_range, level);
  ret_level = hildon_volumebar_range_get_level(volumebar_range);
  fail_if(ret_level != VOLUMEBAR_RANGE_MAXIMUM_VALUE,
          "hildon-volumebar-range: Set level to %.5f (greater than maximum allowed), and get level returned %.5f",
          level, ret_level);

  /* Test3: Set level with NULL object */
  hildon_volumebar_range_set_level(NULL, level);

  /* Test4: Get level with NULL object */
  hildon_volumebar_range_get_level(NULL);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_volumebar_range_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonVolumebarRange");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_get_level");

  /* Create test case for set/get_mute and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_volumebar_range, fx_teardown_default_volumebar_range);
  tcase_add_test(tc1, test_set_get_level_regular);
  tcase_add_test(tc1, test_set_get_level_limits);
  tcase_add_test(tc1, test_set_get_level_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;
}


