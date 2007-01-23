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
#include <gtk/gtkcontainer.h>
#include "test_suites.h"
#include "check_utils.h"
#include "hildon-controlbar.h"

/* -------------------- Fixtures -------------------- */
static HildonControlbar *controlbar = NULL;
static GtkWidget *showed_window = NULL;

static void 
fx_setup_default_controlbar ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  controlbar = HILDON_CONTROLBAR(hildon_controlbar_new());
  /* Check controlbar object has been created properly */
  fail_if(!HILDON_IS_CONTROLBAR(controlbar), 
          "hildon-controlbar: Creation failed.");

  showed_window =  create_test_window ();
  
  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (controlbar));
  
  /* Displays the widget and the window */
  show_all_test_window (showed_window);
  
}

static void 
fx_teardown_default_controlbar ()
{

  /* Destroy the widget and the window */
  gtk_widget_destroy (GTK_WIDGET(showed_window));

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_range -----*/
/**
 * Purpose: Check that regular range values are set and get properly
 * Cases considered:
 *    - Set a range of [20,1000].
 *    - Set a range of [20,20].
 */
START_TEST (test_set_range_regular)
{
  gint init_min;
  gint init_max;
  gint min;
  gint max;
    
  /* Test 1: Set a range of [20,1000] */
  init_min = 20;
  init_max = 1000;
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  min = hildon_controlbar_get_min(controlbar);

  fail_if (min != init_min,
           "hildon-controlbar: The returned min is %d and should be %d", 
           min, init_min);

  max = hildon_controlbar_get_max(controlbar);

  fail_if (max != init_max,
           "hildon-controlbar: The returned max is %d and should be %d", 
           max, init_max);

  /* Test 2: Set a range of [20, 20] */
  init_min = 20;
  init_max = 20;
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  min = hildon_controlbar_get_min(controlbar);

  fail_if (min != init_min,
           "hildon-controlbar: The returned min is %d and should be %d", 
           min, init_min);

  max = hildon_controlbar_get_max(controlbar);

  fail_if (max != init_max,
           "hildon-controlbar: The returned max is %d and should be %d", 
           max, init_max);
    
}
END_TEST

/**
 * Purpose: Check that limits range values are set and get properly
 * Cases considered:
 *    - Set a range of [0,G_MAXINT-1].
 */
START_TEST (test_set_range_limits)
{
  gint init_min;
  gint init_max;
  gint min;
  gint max;
    
  /* Test 1: Set a range of [0,G_MAXINT-1] */
  init_min = 0;
  init_max = G_MAXINT-1;
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  min=hildon_controlbar_get_min(controlbar);

  fail_if (min != init_min,
           "hildon-controlbar: The returned min is %d and should be %d", 
           min, init_min);

  max=hildon_controlbar_get_max(controlbar);

  fail_if (max != init_max,
           "hildon-controlbar: The returned max is %d and should be %d", 
           max, init_max);
}
END_TEST

/**
 * Purpose: Check that invalid range values are set and get properly
 * Cases considered:
 *    - Set a range of [10,100] on NULL object.
 *    - Set a range of [-1,G_MAXINT].
 *    - Set a range of [G_MININT,-1].
 *    - Set a range of [G_MAXINT,-1].
 *    - Set a range of [1,G_MININT].
 */
START_TEST (test_set_range_invalid)
{
  gint init_min;
  gint init_max;
  gint min;
  gint max;

  init_min = 10;
  init_max = 100;
  /* Test 1: Set range on a NULL object */
  hildon_controlbar_set_range(NULL,init_min,init_max);

  init_min = -1;
  init_max = G_MAXINT;
  /* Test 2: Set a range of [-1,G_MAXINT] */
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  min=hildon_controlbar_get_min(controlbar);

  fail_if (min != 0,
           "hildon-controlbar: The returned min is %d and should be 0",
           min);

  max=hildon_controlbar_get_max(controlbar);

  fail_if (max != init_max,
           "hildon-controlbar: The returned max is %d and should be %d",
           max, init_max);

  /* Test 3: Set a range of [G_MININT,-1] */
  init_min = G_MININT;
  init_max = -1;
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  min=hildon_controlbar_get_min(controlbar);

  fail_if (min != 0,
           "hildon-controlbar: The returned min is %d and should be 0",
           min, 0);
  max=hildon_controlbar_get_max(controlbar);

  fail_if (max != 0,
           "hildon-controlbar: The returned max is %d and should be 0",
           max);

  /* Test 4: Set a range of [G_MAXINT,-1] */
  init_min = G_MAXINT;
  init_max = -1;
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  max=hildon_controlbar_get_max(controlbar);

  fail_if (max != 0,
           "hildon-controlbar: The returned max is %d and should be 0",
           max);

  min=hildon_controlbar_get_min(controlbar);

  fail_if (min != 0,
           "hildon-controlbar: The returned min is %d and should be 0",
           min);

  /* Test 5: Set a range of [1,G_MININT] */
  init_min = 1;
  init_max = G_MININT;
  hildon_controlbar_set_range(controlbar,init_min,init_max);

  max=hildon_controlbar_get_max(controlbar);

  fail_if (max != 0,
           "hildon-controlbar: The returned max is %d and should be 0",
           max);

  min=hildon_controlbar_get_min(controlbar);

  fail_if (min != 0,
           "hildon-controlbar: The returned min is %d and should be 0",
           min);
}
END_TEST

/* ----- Test case for set_value -----*/
/**
 * Purpose: Check that regular values are set and get properly
 * Cases considered:
 *    - Set a value of 500 in the range [0,1000].
 */
START_TEST (test_set_value_regular)
{
  gint init_value;
  gint value;

  init_value=500;
  /* Must set a range different than [0,0], if not set you can not set a value*/
  hildon_controlbar_set_range(controlbar,0,1000);

  /* Test 1: Set a value of 500 in the range [0,1000].*/
  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != init_value,
           "hildon-controlbar: The returned value is %d and should be %d", 
           value, init_value);
    
}
END_TEST

/**
 * Purpose: Check that limit values are set and get properly
 * Cases considered:
 *    - Set a value of 0 in the range [0,1000].
 *    - Set a value of 1000 in the range [0,1000].
 */
START_TEST (test_set_value_limits)
{
  gint init_value;
  gint value;

  /* Must set a range diferent than [0,0], if not set you can not set a value*/
  hildon_controlbar_set_range(controlbar,0,1000);

  /*  Test 1: Set a value of 0 in the range [0,1000] */
  init_value=0;

  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != init_value,
           "hildon-controlbar: The returned value is %d and should be %d", 
           value, init_value);

  /* Test 2: Set a value of 1000 in the range [0,1000] */
  init_value=1000;

  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != init_value,
           "hildon-controlbar: The returned value is %d and should be %d", 
           value, init_value);    
}
END_TEST


/**
 * Purpose: Check that invalid values are set and get properly
 * Cases considered:
 *    - Set a value of 1 in a NULL controlbar.
 *    - Set a value of -1 in the range [2,1000].
 *    - Set a value of 1 in the range [2,1000].
 *    - Set a value of G_MININT in the range [2,1000].
 *    - Set a value of 1001 in the range [2,1000].
 *    - Set a value of G_MAXINT in the range [2,1000].
 *    - Set a value of G_MAXINT in the range [2,G_MAXINT].
 */
START_TEST (test_set_value_invalid)
{
  gint init_value;
  gint initial_value=4;
  gint value;
  gint current_value;
  gint min_value;
  gint max_value;

  min_value = 2;
  max_value = 1000;
  /* Must set a range diferent than [0,0], if not set you can not set a value*/
  hildon_controlbar_set_range(controlbar,min_value,max_value);
  hildon_controlbar_set_value(controlbar,initial_value);

  /* Test 1: Set a value of 1 in a NULL controlbar. */
  init_value=10;
  hildon_controlbar_set_value(NULL,init_value);


  /* Test 2: Set a value of -1 in the range [2,1000] */
  init_value=-1;
  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != initial_value,
           "hildon-controlbar: The returned value is %d and should be %d",
           value, initial_value);

  /* Test 3: Set a value of 1 in the range [2,1000] */
  init_value=1;
  hildon_controlbar_set_value(controlbar,min_value);

  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != min_value,
           "hildon-controlbar: The returned value is %d and should be %d",
           value, min_value);


  /* Test 4: Set a value of G_MININT in the range [2,1000] */
  init_value=G_MININT;
  hildon_controlbar_set_value(controlbar,min_value+2);
  current_value = hildon_controlbar_get_value(controlbar);

  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != current_value,
           "hildon-controlbar: The returned value is %d and should be %d",
           value, current_value);

  /* Test 5: Set a value of 1001 in the range [2,1000] */
  init_value=1001;

  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != max_value,
           "hildon-controlbar: The returned value is %d and should be %d",
           value, max_value);


  /* Test 6: Set a value of G_MAXINT in the range [2,1000] */
  init_value=G_MAXINT;

  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != max_value,
           "hildon-controlbar: The returned value is %d and should be %d",
           value, max_value);

  /* Test 7: Set a value of G_MAXINT in the range [2,G_MAXINT] */
  init_value=G_MAXINT;
  hildon_controlbar_set_range(controlbar,2,G_MAXINT);
  hildon_controlbar_set_value(controlbar,init_value);
  value=hildon_controlbar_get_value(controlbar);
  fail_if (value != G_MAXINT-1,
           "hildon-controlbar: The returned value is %d and should be %d",
           value, G_MAXINT-1);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_controlbar_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonControlbar");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_range");
  TCase *tc2 = tcase_create("set_value");

  /* Create test case for hildon_controlbar_set_range and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_controlbar, fx_teardown_default_controlbar);
  tcase_add_test(tc1, test_set_range_regular);
  tcase_add_test(tc1, test_set_range_limits);
  tcase_add_test(tc1, test_set_range_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for hildon_controlbar_set_value and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_controlbar, fx_teardown_default_controlbar);
  tcase_add_test(tc2, test_set_value_regular);
  tcase_add_test(tc2, test_set_value_limits);
  tcase_add_test(tc2, test_set_value_invalid);
  suite_add_tcase (s, tc2);

  return s;
}
