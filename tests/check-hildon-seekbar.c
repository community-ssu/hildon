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
#include <glib/gprintf.h>
#include "test_suites.h"
#include "check_utils.h"

#include <hildon/hildon-seekbar.h>

/* -------------------- Fixtures -------------------- */

static GtkWidget *showed_window = NULL;
static HildonSeekbar *seekbar = NULL;

static void 
fx_setup_default_seekbar ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  seekbar = HILDON_SEEKBAR(hildon_seekbar_new());

  showed_window =  create_test_window ();

  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (seekbar));

  /* Displays the widget and the window */
  show_all_test_window (showed_window);

  /* Check that the seekbar object has been created properly */
  fail_if(!HILDON_SEEKBAR(seekbar),
          "hildon-seekbar: Creation failed.");
}

static void 
fx_teardown_default_seekbar ()
{

  /* Destroy the window */
  gtk_widget_destroy (showed_window);
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_time -----*/

/**
 * Purpose: test setting regular time values for hildon_seekbar_set_time
 * Cases considered:
 *    - Set and get 1000 seconds without setting new position and fraction.
 *    - Set and get 500 seconds setting without setting new position but new fraction should be set.
 *    - Set and get 500 seconds setting without setting new position but new fraction should be set.
 */
START_TEST (test_set_time_regular)
{
  gint ret_seconds;
  gint seconds;
  gint position;
  gint fraction;

  /* Test 1: Set and get 1000 seconds without setting new position and fraction */
  seconds = 1000;
  hildon_seekbar_set_total_time(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_total_time(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar total time to %d but get_total_time returns %d",
          seconds,ret_seconds);


  /* Test 2: Set and get 500 seconds with a fraction set to 750 and position set to 750 (in order to test the correct update of 
     position and fraction.    */
  seconds = 500;
  position = 750;
  fraction = 750;

  hildon_seekbar_set_fraction(seekbar,fraction);
  fail_if(fraction != hildon_seekbar_get_fraction(seekbar), 
          "hildon-seekbar: set total time to %d but get_fraction returns %d ",
          fraction,hildon_seekbar_get_fraction(seekbar));

  hildon_seekbar_set_position(seekbar,position);
  fail_if(position != hildon_seekbar_get_position(seekbar), 
          "hildon-seekbar: set total time to %d but get_position returns %d",
          position,hildon_seekbar_get_position(seekbar));

  hildon_seekbar_set_total_time(seekbar,seconds);
  fail_if(seconds != hildon_seekbar_get_fraction(seekbar), 
          "hildon-seekbar: set total time to %d but get_fraction returns %d being total time %d",
          seconds,hildon_seekbar_get_fraction(seekbar),hildon_seekbar_get_total_time(seekbar));

  fail_if(seconds != hildon_seekbar_get_position(seekbar), 
          "hildon-seekbar: set total time to %d but get_fraction returns %d being total time %d",
          seconds,hildon_seekbar_get_fraction(seekbar),hildon_seekbar_get_total_time(seekbar));

  ret_seconds=hildon_seekbar_get_total_time(seekbar);
  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set total time to %d but get_total_time returns %d ",
          seconds,ret_seconds);   

  /* Test 3: Set and get 500 seconds with a fraction set to 750 and position set to 250 (in order to test the correct update of 
     position and fraction.
  */
  seconds = 1000;
  hildon_seekbar_set_total_time(seekbar,seconds);

  seconds = 500;
  position = 250;
  fraction = 750;

  hildon_seekbar_set_fraction(seekbar,fraction);
  fail_if(fraction != hildon_seekbar_get_fraction(seekbar), 
          "hildon-seekbar: set seekbar fraction to %d but get_fraction returns %d ",
          fraction,hildon_seekbar_get_fraction(seekbar));

  hildon_seekbar_set_position(seekbar,position);
  fail_if(position != hildon_seekbar_get_position(seekbar), 
          "hildon-seekbar: set seekbar position to %d but get_position returns %d",
          position,hildon_seekbar_get_position(seekbar));

  hildon_seekbar_set_total_time(seekbar,seconds);
  fail_if(fraction != hildon_seekbar_get_fraction(seekbar), 
          "hildon-seekbar: set seekbar fraction to %d but get_fraction returns %d being total time %d",
          seconds,hildon_seekbar_get_fraction(seekbar),hildon_seekbar_get_total_time(seekbar));

  ret_seconds=hildon_seekbar_get_total_time(seekbar);
  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set total time to %d but get_total_time returns %d ",
          seconds,ret_seconds);
}
END_TEST

/**
 * Purpose: test setting limit time values for hildon_seekbar_set_time
 * Cases considered:
 *    - Set and get 0 seconds setting new position and fraction (previously set to 750).
 *    - Set and get G_MAXINT seconds without setting new position and fraction.
 */
START_TEST (test_set_time_limits)
{
  gint ret_seconds;
  gint seconds;
  gint position;
  gint fraction;

  seconds = 1000;
  hildon_seekbar_set_total_time(seekbar,seconds);
    
  position = 750;
  fraction = 750;

  hildon_seekbar_set_fraction(seekbar,fraction);
  hildon_seekbar_set_position(seekbar,position);

  /* Test 1: Set and get 1 seconds without setting new position and fraction (time was previously set to 1000)*/
  seconds = 1;
  hildon_seekbar_set_total_time(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_total_time(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar total time to %d should return %d but get_total_time returns %d",
          seconds,ret_seconds);

  /* Check with seconds because hildon_seekbar_set_total_time must update fraction and position to total time set*/
  fail_if(seconds != hildon_seekbar_get_fraction(seekbar), 
          "hildon-seekbar: set time to %d but get_fraction returns %d ",
          seconds,hildon_seekbar_get_fraction(seekbar));

  /* Check with seconds because hildon_seekbar_set_total_time must update fraction and position to total time set*/
  fail_if(seconds != hildon_seekbar_get_position(seekbar), 
          "hildon-seekbar: set time to %d but get_position returns %d",
          seconds,hildon_seekbar_get_position(seekbar));

  /* Test 2: Set and get G_MAXINT seconds without setting new position and fraction */
  seconds = G_MAXINT;
  hildon_seekbar_set_total_time(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_total_time(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar total time to %d but get_total_time returns %d",
          seconds,ret_seconds);
    
    
}
END_TEST


/**
 * Purpose: test setting invalid time values for hildon_seekbar_set_time
 * Cases considered:
 *    - Set and get seconds to a NULL object.
 *    - Set and get 0 seconds without setting new position and fraction.
 *    - Set and get -1 seconds without setting new position and fraction.
 */
START_TEST (test_set_time_invalid)
{
  gint init_seconds;
  gint ret_seconds;
  gint seconds;

  init_seconds = 1000;
  /* Test 1: Set/get seconds on NULL object */
  hildon_seekbar_set_total_time(NULL,init_seconds);
  ret_seconds=hildon_seekbar_get_total_time(NULL);

  /* Init seekbar to 1000 seconds*/
  hildon_seekbar_set_total_time(seekbar,init_seconds);
    

  /* Test 2: Set and get 0 seconds */
  seconds = 0;
  hildon_seekbar_set_total_time(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_total_time(seekbar);

  fail_if(ret_seconds != init_seconds, 
          "hildon-seekbar: set seekbar total time to %d, should set %d but get_total_time returns %d",
          seconds,init_seconds,ret_seconds);

  /* Test 3: Set and get -1 seconds */
  seconds = -1;
  hildon_seekbar_set_total_time(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_total_time(seekbar);

  fail_if(ret_seconds != init_seconds, 
          "hildon-seekbar: set seekbar total time to %d, should return %d but get_total_time returns %d",
          seconds,init_seconds,ret_seconds);
    
}
END_TEST

/* ----- Test case for set_fraction -----*/

/**
 * Purpose: test setting regular fraction values for hildon_seekbar_set_fraction
 * Cases considered:
 *    - Set and get fraction to 500 with total time set to 1000.
 *    - Set and get fraction to 490 with total time set to 1000, fraction and position previously set to 500.
 */
START_TEST (test_set_fraction_regular)
{
  gint ret_seconds;
  gint init_seconds;
  gint seconds;

  /* Init seekbar to 1000 */
  init_seconds = 1000;
  hildon_seekbar_set_total_time(seekbar,init_seconds);

  /* Test 1: Set and get fraction to 500 with total time set to 1000 */
  seconds = 500;
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar fraction returns %d",
          seconds,ret_seconds);

  /* Test 2: Set and get fraction to 490 with total time set to 1000, fraction and position previously set to 500 */
  seconds = 500;

  hildon_seekbar_set_fraction(seekbar,seconds);
  hildon_seekbar_set_position(seekbar,seconds);

  hildon_seekbar_set_fraction(seekbar,seconds-10);

  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != seconds-10, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar fraction returns %d",
          seconds,ret_seconds);

  fail_if(hildon_seekbar_get_position(seekbar) != seconds-10, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar position returns %d",
          seconds-10,hildon_seekbar_get_position(seekbar));
    
}
END_TEST

/**
 * Purpose: test setting limit fraction values for hildon_seekbar_set_fraction
 * Cases considered:
 *    - Set and get fraction to 0 with total time set to G_MAXINT.
 *    - Set and get fraction to 1 with total time set to G_MAXINT.
 *    - Set and get fraction to G_MAXINT-1 with total time set to G_MAXINT.
 *    - Set and get fraction to G_MAXINT with total time set to G_MAXINT.
 */
START_TEST (test_set_fraction_limits)
{
  gint ret_seconds;
  gint seconds;
  gint init_seconds;

  /* Init seekbar to G_MAXINT total time */
  init_seconds = G_MAXINT;
  hildon_seekbar_set_total_time(seekbar,init_seconds);

  /* Test 1: Set and get fraction to 0 with total time set to G_MAXINT */
  seconds = 0;
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar fraction returns %d",
          seconds,ret_seconds);

  /* Test 2: Set and get fraction to 1 with total time set to G_MAXINT */
  seconds = 1;
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar fraction returns %d",
          seconds,ret_seconds);

  /* Test 3: Set and get fraction to G_MAXINT-1 with total time set to G_MAXINT */
  seconds = G_MAXINT-1;
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar fraction returns %d",
          seconds,ret_seconds);

  /* Test 4: Set and get fraction to G_MAXINT with total time set to G_MAXINT */
  seconds = G_MAXINT;
    
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);
    
  fail_if(ret_seconds != seconds, 
          "hildon-seekbar: set seekbar fraction to %d but get seekbar fraction returns %d",
          seconds,ret_seconds);

    
}
END_TEST

/**
 * Purpose: test setting invalid fraction values for hildon_seekbar_set_fraction
 * Cases considered:
 *    - Set and get fraction to NULL object.
 *    - Set and get fraction to -1 with total time set to G_MAXINT.
 *    - Set and get fraction to 2000 with total time set to 1000.
 */
START_TEST (test_set_fraction_invalid)
{
  gint ret_seconds;
  gint seconds;
  gint init_seconds;

  /* Init seekbar to G_MAXINT total time */
  init_seconds = G_MAXINT;
  hildon_seekbar_set_total_time(seekbar,init_seconds);

  /* Test 1: Set and get fraction to NULL object */
  seconds = 1000;
  hildon_seekbar_set_fraction(NULL,seconds);
  ret_seconds=hildon_seekbar_get_fraction(NULL);

  /* Test 2: Set and get fraction to -1 with total time set to G_MAXINT */
  seconds = -1;
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != 0, 
          "hildon-seekbar: set seekbar fraction to %d should set 0 but get seekbar fraction returns %d",
          seconds,ret_seconds);

  /* Init seekbar to 1000 total time */
  init_seconds = 1000;
  hildon_seekbar_set_total_time(seekbar,init_seconds);
  hildon_seekbar_set_fraction(seekbar,init_seconds-500);

  /* Test 3: Set and get fraction to 2000 with total time set to 1000 */
  seconds = 2000;
  hildon_seekbar_set_fraction(seekbar,seconds);
  ret_seconds=hildon_seekbar_get_fraction(seekbar);

  fail_if(ret_seconds != init_seconds-500, 
          "hildon-seekbar: set seekbar fraction to %d should set %d but get seekbar fraction returns %d",
          seconds,init_seconds,ret_seconds);
    
}
END_TEST



/* ---------- Suite creation ---------- */

Suite *create_hildon_seekbar_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonSeekbar");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_time");
  TCase *tc2 = tcase_create("set_fraction");

  /* Create test case for hildon_seekbar_set_time and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_seekbar, fx_teardown_default_seekbar);
  tcase_add_test(tc1, test_set_time_regular);
  tcase_add_test(tc1, test_set_time_limits);
  tcase_add_test(tc1, test_set_time_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for hildon_seekbar_set_fraction  and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_seekbar, fx_teardown_default_seekbar);
  tcase_add_test(tc2, test_set_fraction_regular);
  tcase_add_test(tc2, test_set_fraction_limits);
  tcase_add_test(tc2, test_set_fraction_invalid);
  suite_add_tcase (s, tc2);

  /* Return created suite */
  return s;             
}

