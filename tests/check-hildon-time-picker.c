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

#include "hildon-window.h"
#include "hildon-time-picker.h"

/* -------------------- Fixtures -------------------- */

static HildonTimePicker *time_picker = NULL;
static HildonWindow *tp_window = NULL;

static void
fx_setup_default_time_picker ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  tp_window = HILDON_WINDOW(hildon_window_new());
 /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(tp_window),
          "hildon-time-picker: Window creation failed.");

  time_picker = HILDON_TIME_PICKER(hildon_time_picker_new(GTK_WINDOW(tp_window)));

  /* Check time_picker object has been created properly */
  fail_if(!HILDON_IS_TIME_PICKER(time_picker),
          "hildon-time-picker: Creation failed.");

  /* Displays the widget and the window */
  show_all_test_window (GTK_WIDGET (tp_window));
  show_all_test_window (GTK_WIDGET (time_picker));

}

static void
fx_teardown_default_time_picker ()
{
  gtk_widget_destroy (GTK_WIDGET (time_picker));
  gtk_widget_destroy (GTK_WIDGET (tp_window));
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_time -----*/

/**
 * Purpose: Check that regular time values are set and get properly
 * Cases considered:
 *    - Set and get time to 5h 30m using time picker object.
 *    - Set and get time to 18h 2m using time picker object.
 */
START_TEST (test_set_time_regular)
{
  guint hours;
  guint minutes;
  guint ret_hours;
  guint ret_minutes;
    
  /* Check time_picker object has been created properly */
  fail_if(!HILDON_IS_TIME_PICKER(time_picker), 
          "hildon-time-picker: Creation failed.");

  /* Test 1: Set and get time to 5h 30m using time picker object. */
  hours = 5;
  minutes = 30;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);
    
  fail_if (hours != ret_hours || minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, ret_hours, ret_minutes);

  /* Test 2: Set and get time to 18h 2m using time picker object. */
  hours = 18;
  minutes = 2;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (hours != ret_hours || minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, ret_hours, ret_minutes);
}
END_TEST

/**
 * Purpose: Check that limit time values are set and get properly       
 * Cases considered:
 *    - Set and get time to 0h 0m using time picker object.
 *    - Set and get time to 0h 59m using time picker object.
 *    - Set and get time to 12h 59m using time picker object.
 *    - Set and get time to 23h 59m using time picker object.
 */
START_TEST (test_set_time_limits)
{
  guint hours;
  guint minutes;
  guint ret_hours;
  guint ret_minutes;
    
  /* Check time_picker object has been created properly */
  fail_if(!HILDON_IS_TIME_PICKER(time_picker), 
          "hildon-time-picker: Creation failed.");
    
  /* Test 1: Set and get time to 0h 0m using time picker object. */
  hours = 0;
  minutes = 0;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (hours != ret_hours || minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, ret_hours, ret_minutes);    

  /* Test 2: Set and get time to 0h 59m using time picker object. */
  hours = 0;
  minutes = 59;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (hours != ret_hours || minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, ret_hours, ret_minutes);

  /* Test 3: Set and get time to 12h 59m using time picker object. */
  hours = 12;
  minutes = 59;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (hours != ret_hours || minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, ret_hours, ret_minutes);

  /* Test 4: Set and get time to 23h 59m using time picker object. */
  hours = 23;
  minutes = 59;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (hours != ret_hours || minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, ret_hours, ret_minutes);   
}
END_TEST

/**
 * Purpose: Check that limit time values are set and get properly       
 * Cases considered:
 *    - Set and get time using NULL time picker.
 *    - Set and get time to 0h 60m using time picker object.
 *    - Set and get time to 24h 0m using time picker object.
 *    - Set and get time to 24h 60m using time picker object.
 *    - Set and get time to 16000h 15533m using time picker object.
 */
START_TEST (test_set_time_invalid)
{
  guint hours;
  guint minutes;
  guint ret_hours;
  guint ret_minutes;
  guint expected_hours;
  guint expected_minutes;
    
  /* Check time_picker object has been created properly */
  fail_if(!HILDON_IS_TIME_PICKER(time_picker), 
          "hildon-time-picker: Creation failed.");

  hours = 0;
  minutes = 60;
  expected_hours=1;
  expected_minutes=0;

  /* Test 1: Set and get time using NULL time picker */
  hildon_time_picker_set_time(NULL,hours,minutes);
  hildon_time_picker_get_time(NULL,&ret_hours,&ret_minutes);

  /* Test 2: Set and get time to 0h 60m using time picker object. */   
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (expected_hours != ret_hours || expected_minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um), expected time is (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, expected_hours, expected_minutes, ret_hours, ret_minutes);   

  /* Test 3: Set and get time to 24h 0m using time picker object. */   
  hours = 24;
  minutes = 0;
  expected_hours=0;
  expected_minutes=0;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (expected_hours != ret_hours || expected_minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um), expected time is (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, expected_hours, expected_minutes, ret_hours, ret_minutes);   

  /* Test 4: Set and get time to 24h 60m using time picker object. */
  hours = 24;
  minutes = 60;
  expected_hours=1;
  expected_minutes=0;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (expected_hours != ret_hours || expected_minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um), expected time is (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, expected_hours, expected_minutes, ret_hours, ret_minutes);   

  /* Test 5: Set and get time to 16000h 15533m using time picker object.*/
  hours = 16000;
  minutes = 15533;
  expected_hours=10;
  expected_minutes=53;
    
  hildon_time_picker_set_time(time_picker,hours,minutes);
  hildon_time_picker_get_time(time_picker,&ret_hours,&ret_minutes);

  fail_if (expected_hours != ret_hours || expected_minutes != ret_minutes,
	   "hildon-time-picker: Set time (%uh:%um), expected time is (%uh:%um) but returned time is (%uh:%um)",
	   hours, minutes, expected_hours, expected_minutes, ret_hours, ret_minutes);    
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_time_picker_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonTimePicker");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_time");

  /* Create test case for hildon_time_picker_set_time and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_time_picker, fx_teardown_default_time_picker);
  tcase_add_test(tc1, test_set_time_regular);
  tcase_add_test(tc1, test_set_time_limits);
  tcase_add_test(tc1, test_set_time_invalid);
  suite_add_tcase(s, tc1);

  /* Return created suite */
  return s;             
}
