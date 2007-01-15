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
#include <time.h>
#include <check.h>
#include <gtk/gtkmain.h>
#include "test_suites.h"
#include "check_utils.h"

#include "hildon-calendar-popup.h"

/* -------------------- Fixtures -------------------- */

static HildonCalendarPopup *calendar = NULL;

static void 
fx_setup_default_calendar()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  calendar = HILDON_CALENDAR_POPUP(hildon_calendar_popup_new(NULL, 2006, 4, 10));
  
  show_test_window(GTK_WIDGET(calendar));

  /* Check calendar object has been created properly */
  fail_if(!HILDON_IS_CALENDAR_POPUP(calendar), 
          "hildon-calendar-popup: Creation failed.");
}

static void 
fx_teardown_default_calendar()
{

  gtk_widget_destroy(GTK_WIDGET(calendar)); 

}

/* -------------------- Test cases -------------------- */

/* ------ Test case for get/set_date ----- */

/**
 * Purpose: Check regular values for dates
 * Cases considered:
 *    - Set and get date 30/03/1981
 */
START_TEST (test_set_get_date_regular)
{
  guint year, month, day;
  guint ret_year, ret_month, ret_day;

  /* Test1: set a valid date like 30/03/1981 */
  year = 1981;
  month = 3;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(year != ret_year || month != ret_month || day != ret_day, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned date %u/%u/%u", 
          year, month, day, ret_year, ret_month, ret_day);

}
END_TEST

/**
 * Purpose: Check limit values for dates
 * Cases considered:
 *    - Set and get year to max-year property
 *    - Set and get year to min-year property
 *    - Set and get month to 12
 *    - Set and get month to 1
 *    - Set and get day to 31 (March)
 *    - Set and get day to 30 (April)
 *    - Set and get day to 29 (February 2004)
 *    - Set and get day to 28 (February 2006)
 *    - Set and get day to 1
 *    - Set and get maximum allowed date (according to max-year property).
 *    - Set and get minimum allowed date (according to min-year property).
 */
START_TEST (test_set_get_date_limits)
{
  GValue value = {0, };
  guint year, month, day;
  guint ret_year, ret_month, ret_day; 
  guint max_year, min_year;

  g_value_init(&value, G_TYPE_UINT);

  /* Test1: Test year maximum limit */
  max_year = 2050;
  g_value_set_uint (&value, max_year);
  g_object_set_property (G_OBJECT (calendar), "max-year", &value);
  year = max_year;
  month = 3;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(year != ret_year, 
          "hildon-calendar-popup: Set date to %u/%u/%u, with maximum year set to %u, but get date returned year %u", 
          year, month, day, max_year, ret_year);

  /* Test2: Test year minimum limit */
  min_year = 2000;
  g_value_set_uint (&value, min_year);
  g_object_set_property (G_OBJECT (calendar), "min-year", &value);
  year = min_year;
  month = 3;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(year != ret_year, 
          "hildon-calendar-popup: Set date to %u/%u/%u, with minimum year set to %u, but get date returned year %u", 
          year, month, day, min_year, ret_year);

  /* Test3: Test month minimum limit */
  year = 2006;
  month = 1;
  day = 10;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(month != ret_month, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned month %u", 
          year, month, day, ret_month);  

  /* Test4: Test month maximum limit */
  year = 2006;
  month = 12;
  day = 10;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(month != ret_month, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned month %u", 
          year, month, day, ret_month);    

  /* Test5: Test day maximum limit */

  /* 31 */
  year = 2006;
  month = 3;
  day = 31;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(day != ret_day, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned day %u", 
          year, month, day, ret_day);     

  /* 30 */
  year = 2006;
  month = 4;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(day != ret_day, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned day %u", 
          year, month, day, ret_day); 

  /* February 28th */
  year = 2006;
  month = 2;
  day = 28;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(day != ret_day, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned day %u", 
          year, month, day, ret_day); 

  /* February 29th */
  year = 2004;
  month = 2;
  day = 29;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(day != ret_day, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned day %u", 
          year, month, day, ret_day); 
 
  /* Test6: Test day minimum limit */
  year = 2006;
  month = 2;
  day = 1;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(day != ret_day, 
          "hildon-calendar-popup: Set date to %u/%u/%u, but get date returned day %u", 
          year, month, day, ret_day); 

  /* Test7: Test maximum allowed date */
  max_year = 2050;
  g_value_set_uint (&value, max_year);
  g_object_set_property (G_OBJECT (calendar), "max-year", &value);
  year = max_year;
  month = 12;
  day = 31;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(year != ret_year, 
          "hildon-calendar-popup: Set date to %u/%u/%u, with maximum year set to %u, but get date returned date %u/%u/%u", 
          year, month, day, max_year, ret_year, ret_month, ret_day);

  /* Test8: Test minimum allowed date */
  min_year = 2000;
  g_value_set_uint (&value, min_year);
  g_object_set_property (G_OBJECT (calendar), "min-year", &value);
  year = min_year;
  month = 1;
  day = 1;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(year != ret_year, 
          "hildon-calendar-popup: Set date to %u/%u/%u, with minimum year set to %u, but get date returned date %u/%u/%u", 
          year, month, day, min_year, ret_year, ret_month, ret_day);
}
END_TEST

/**
 * Purpose: Check invalid values for dates
 * Cases considered:
 *    - Set and get year above max-year property
 *    - Set and get year under min-year property
 *    - Set and get month to 13
 *    - Set and get month to 0
 *    - Set and get day to 32 (March)
 *    - Set and get day to 31 (April)
 *    - Set and get day to 30 (February 2004)
 *    - Set and get day to 29 (February 2006)
 *    - Set and get day to 0
 *    - Set and get above maximum allowed date (according to max-year property).
 *    - Set and get under minimum allowed date (according to min-year property).
 *    - Set and get date using NULL calendar object
 */
START_TEST (test_set_get_date_invalid)
{
  GValue value = {0, };
  guint year, month, day;
  guint valid_year, valid_month, valid_day;
  guint current_year, current_month, current_day;
  guint ret_year, ret_month, ret_day; 
  guint max_year, min_year;
  GDate date;

  g_value_init(&value, G_TYPE_UINT);

  /* Test 1: Test above maximum year limit */
  max_year = 2050;
  g_value_set_uint (&value, max_year);
  g_object_set_property (G_OBJECT (calendar), "max-year", &value);
  year = max_year + 1;
  month = 3;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(max_year != ret_year, 
          "hildon-calendar-popup: Set date to %u/%u/%u, with maximum year set to %u, but get date returned year = %u != %u", 
          year, month, day, max_year, ret_year, max_year);

  /* Test 2: Test under minimum year limit */
  min_year = 2000;
  g_value_set_uint (&value, min_year);
  g_object_set_property (G_OBJECT (calendar), "min-year", &value);
  year = min_year - 1;
  month = 3;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if(min_year != ret_year,
          "hildon-calendar-popup: Set date to %u/%u/%u, with minimum year set to %u, but get date returned year = %u != %u",
          year, month, day, min_year, ret_year, min_year);

  /* Test 3: Test above maximum month limit */
  valid_year = 2006;
  valid_month = 10;
  valid_day = 5;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2010;
  month = 13;
  day = 12;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);


  /* Test 4: Test under minimum month limit */
  valid_year = 2006;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2010;
  month = 0;
  day = 10;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);


  /* Test 5: Test above maximum day limit */

  /* 31 */
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2006;
  month = 3;
  day = 32;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);


  /* 30 */
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2006;
  month = 4;
  day = 31;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);

  /* February 28th */
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2006;
  month = 2;
  day = 29;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);


  /* February 29th */
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2004;
  month = 2;
  day = 30;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);

  /* Test6: Test day minimum limit */
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = 2006;
  month = 2;
  day = 0;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);

  /* Gets the curret date */
  g_date_clear(&date, 1);
  g_date_set_time(&date, time(NULL));

  current_day = g_date_get_day(&date);
  current_month = g_date_get_month(&date);
  current_year = g_date_get_year(&date);

  fail_if((current_year != ret_year) || (current_month != ret_month) || (current_day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, ret_year, ret_month, ret_day, current_year, current_month, current_day);

  /* Test7: Test above maximum allowed date */
  max_year = 2050;
  g_value_set_uint (&value, max_year);
  g_object_set_property (G_OBJECT (calendar), "max-year", &value);
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = max_year + 1;
  month = 1;
  day = 1;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if((max_year != ret_year) || (month != ret_month) || (day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, with maximum year set to %u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, max_year, ret_year, ret_month, ret_day, max_year, month, day);

  /* Test8: Test minimum allowed date */
  min_year = 2000;
  g_value_set_uint (&value, min_year);
  g_object_set_property (G_OBJECT (calendar), "min-year", &value);
  valid_year = 2005;
  valid_month = 2;
  valid_day = 15;
  hildon_calendar_popup_set_date(calendar, valid_year, valid_month, valid_day);
  year = min_year - 1;
  month = 12;
  day = 31;
  hildon_calendar_popup_set_date(calendar, year, month, day);
  hildon_calendar_popup_get_date(calendar, &ret_year, &ret_month, &ret_day);
  fail_if((min_year != ret_year) || (month != ret_month) || (day != ret_day),
          "hildon-calendar-popup: Set valid date to %u/%u/%u, then set an invalid date %u/%u/%u, with minimum year set to %u, but get date returned %u/%u/%u instead of %u/%u/%u",
          valid_year, valid_month, valid_day, year, month, day, min_year, ret_year, ret_month, ret_day, min_year, month, day);

  /* Test9: Set and get date with NULL calendar objects */
  hildon_calendar_popup_set_date(NULL, 2006, 10, 15);
  hildon_calendar_popup_get_date(NULL, &year, &month, &day);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_calendar_popup_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonCalendarPopup");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_get_date");

  /* Create test case for set_date and get_date and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_calendar, fx_teardown_default_calendar);
  tcase_add_test(tc1, test_set_get_date_regular);
  tcase_add_test(tc1, test_set_get_date_limits);
  tcase_add_test(tc1, test_set_get_date_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;
}
