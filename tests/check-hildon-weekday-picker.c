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

#include "hildon-weekday-picker.h"
#include "hildon-window.h"
#include "hildon-caption.h"

/* -------------------- Fixtures -------------------- */

static HildonWeekdayPicker *weekday_picker = NULL;
static GtkWidget *showed_window = NULL;

static void 
fx_setup_default_weekday_picker ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  showed_window =  create_test_window ();
 
  weekday_picker = HILDON_WEEKDAY_PICKER(hildon_weekday_picker_new());
  /* Check weekday_picker object has been created properly */
  fail_if(!HILDON_IS_WEEKDAY_PICKER(weekday_picker), 
          "hildon-weekday-picker: Creation failed.");

  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (weekday_picker)); 

  /* Displays the widget and the window */
  show_all_test_window (showed_window);


}

static void 
fx_teardown_default_weekday_picker ()
{
  gtk_widget_destroy (GTK_WIDGET (showed_window));
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_day / isset_day -----*/

/**
 * Purpose: Check that regular day values are set and get properly
 * Cases considered:
 *    - Set and isset the day "WEDNESDAY" on weekday picker.
 *    - Set and isset two days (WEDNESDAY and SATURDAY) on weekday picker and check that only the two selected days are set.
 */
START_TEST (test_set_day_regular)
{
        
  /* Test 1: Set WEDNESDAY on weekday picker. */
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_WEDNESDAY);
    
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),  "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY didn't set WEDNESDAY.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set MONDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),     "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),    "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),    "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set SATURDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set SUNDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY), "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY set G_DATE_BAD_WEEKDAY wrong.");

  hildon_weekday_picker_set_day(weekday_picker,G_DATE_WEDNESDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_SATURDAY);
    
  /* Test 1: Set WEDNESDAY and SATURDAY on weekday picker. */
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),     "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY didn't set WEDNESDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY didn't set SATURDAY.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY set MONDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY set SUNDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY),    "hildon-weekday-picker: set_day/isset_day to G_DATE_WEDNESDAY and G_DATE_SATURDAY set G_DATE_BAD_WEEKDAY wrong.");
    
}
END_TEST

/**
 * Purpose: Check that limit day values are set and get properly
 * Cases considered:
 *    - Set and isset the day "MONDAY".
 *    - Set and isset the day "SUNDAY".
 *    - Set SUNDAY and MONDAY on weekday picker and check that the two selected days are set, but the others doesn't be set.
 *    - Set all days on weekday picker and check that all days are set.
 */
START_TEST (test_set_day_limits)
{
    
  hildon_weekday_picker_unset_all(weekday_picker);
    
  /* Test 1: Set MONDAY on weekday picker. */
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_MONDAY);
    
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY didn't set MONDAY.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set SUNDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set WEDNESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set SATURDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY),    "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set G_DATE_BAD_WEEKDAY wrong.");

  hildon_weekday_picker_unset_all(weekday_picker);
    
  /* Test 2: Set SUNDAY on weekday picker. */
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_SUNDAY);
    
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_SUNDAY didn't set SUNDAY.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set MONDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set WEDNESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set SATURDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY),    "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY set G_DATE_BAD_WEEKDAY wrong.");

  hildon_weekday_picker_unset_all(weekday_picker);

  hildon_weekday_picker_set_day(weekday_picker,G_DATE_SUNDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_MONDAY);
    
  /* Test 3: Set MONDAY and SUNDAY on weekday picker. */
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY didn't set MONDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY didn't set SUNDAY.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY set WEDNESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_MONDAY and G_DATE_SUNDAY set SATURDAY wrong.");    

  hildon_weekday_picker_unset_all(weekday_picker);

  /* Test 4: Set all days on weekday picker. */
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_MONDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_TUESDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_WEDNESDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_THURSDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_FRIDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_SATURDAY);
  hildon_weekday_picker_set_day(weekday_picker,G_DATE_SUNDAY);
    
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),        "hildon-weekday-picker: set_day/isset_day all days did not set MONDAY.");
 fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day/isset_day all days did not set SUNDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),       "hildon-weekday-picker: set_day/isset_day all days did not set TUESDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),     "hildon-weekday-picker: set_day/isset_day all days did not set WEDNESDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),      "hildon-weekday-picker: set_day/isset_day all days did not set THURSDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),        "hildon-weekday-picker: set_day/isset_day all days did not set FRIDAY.");
  fail_if(!hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),      "hildon-weekday-picker: set_day/isset_day all days did not set SATURDAY.");    

}
END_TEST

/**
 * Purpose: Check that invalid day values are set and get properly
 * Cases considered:
 *    - Set MONDAY on NULL weekday picker.
 *    - Set G_DATE_BAD_WEEKDAY on weekday picker.
 *    - Set 8 on weekday picker.
 *    - Set -2 on weekday picker.
 *    - isset_day from NULL weekday picker
 */
START_TEST (test_set_day_invalid)
{
    
  hildon_weekday_picker_unset_all(weekday_picker);
    
  /* Test 1: Set MONDAY on NULL weekday picker. */
  hildon_weekday_picker_set_day(NULL,G_DATE_MONDAY);
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),         "hildon-weekday-picker: set_day on NULL object set MONDAY wrong");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day on NULL object set SUNDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day on NULL object set WEDNESDAY wrong.");

  /* Test 2: Set G_DATE_BAD_WEEKDAY on weekday picker. */

  hildon_weekday_picker_set_day(weekday_picker,G_DATE_BAD_WEEKDAY);
    
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY),    "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set G_DATE_BAD_WEEKDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set MONDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set WEDNESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),       "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set SATURDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day/isset_day to G_DATE_BAD_WEEKDAY set SUNDAY wrong.");
    
  /* Test 3: Set 8 on weekday picker. */
  hildon_weekday_picker_set_day(weekday_picker,8);
    
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY),    "hildon-weekday-picker: set_day/isset_day to 8 set G_DATE_BAD_WEEKDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),         "hildon-weekday-picker: set_day/isset_day to 8 set MONDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to 8 set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day/isset_day to 8 set WEDNESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to 8 set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to 8 set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),       "hildon-weekday-picker: set_day/isset_day to 8 set SATURDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day/isset_day to 8 set SUNDAY wrong.");

  /* Test 4: Set -2 on weekday picker. */
  hildon_weekday_picker_set_day(weekday_picker,-2);
    
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_BAD_WEEKDAY),    "hildon-weekday-picker: set_day/isset_day to -2 set G_DATE_BAD_WEEKDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_MONDAY),         "hildon-weekday-picker: set_day/isset_day to -2 set MONDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_TUESDAY),        "hildon-weekday-picker: set_day/isset_day to -2 set TUESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_WEDNESDAY),      "hildon-weekday-picker: set_day/isset_day to -2 set WEDNESDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_THURSDAY),       "hildon-weekday-picker: set_day/isset_day to -2 set THURSDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_FRIDAY),         "hildon-weekday-picker: set_day/isset_day to -2 set FRIDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SATURDAY),       "hildon-weekday-picker: set_day/isset_day to -2 set SATURDAY wrong.");
  fail_if(hildon_weekday_picker_isset_day(weekday_picker,G_DATE_SUNDAY),         "hildon-weekday-picker: set_day/isset_day to -2 set SUNDAY wrong.");

  /* Test 4: isset_day from NULL weekday picker. */
  hildon_weekday_picker_isset_day(NULL,G_DATE_BAD_WEEKDAY);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_weekday_picker_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonWeekdayPicker");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_day");
    
  /* Create test case for hildon_weekday_picker_set_day and hildon_weekday_picker_isset_day and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_weekday_picker, fx_teardown_default_weekday_picker);
  tcase_add_test(tc1, test_set_day_regular);
  tcase_add_test(tc1, test_set_day_limits);
  tcase_add_test(tc1, test_set_day_invalid);
  suite_add_tcase (s, tc1);
  
  /* Return created suite */
  return s;             
}
