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
#include <gtk/gtkhbox.h>
#include "test_suites.h"
#include "check_utils.h"

#include "hildon-date-editor.h"

/* Taken from the values of the properties of HildonDateEditor */
#define MAX_YEAR 2037
#define MAX_MONTH 12
#define MAX_DAY 31
#define MIN_YEAR 1970
#define MIN_MONTH 1
#define MIN_DAY 1

/* -------------------- Fixtures -------------------- */

static HildonDateEditor *date_editor = NULL;
static GtkWidget *showed_window = NULL;

static void 
fx_setup_default_date_editor ()
{
  int argc = 0;

  gtk_init(&argc, NULL);
  
  showed_window =  create_test_window ();

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());
  /* Check that the date editor object has been created properly */
  fail_if(!HILDON_IS_DATE_EDITOR(date_editor),
          "hildon-date-editor: Creation failed.");
  
  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (date_editor));
  
  /* Displays the widget and the window */
  show_all_test_window (showed_window);
}

static void 
fx_teardown_default_date_editor ()
{

  /* Destroy the widget and the window */
  gtk_widget_destroy (GTK_WIDGET (showed_window));

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_date -----*/

/**
 * Purpose: test setting regular values for hildon_date_editor_set_date
 * Cases considered:
 *    - Set and get the date 30/03/1981
 */
START_TEST (test_set_date_regular)
{
  guint year, month, day;
  guint ret_year, ret_month, ret_day;

  year = 1981;
  month = 3;
  day = 30;

  /* Test 1: Try date 30/3/1981 */
  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);
}
END_TEST

static void
test_set_date_limits_check (guint year, guint month, guint day)
{
  guint ret_year, ret_month, ret_day;

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);
}

/**
 * Purpose: test limit date values for hildon_date_editor_set_date
 * Cases considered:
 *    - test a year value equal to the year limits (1970, 2037)
 *    - test a month value equal to the month limits (1, 12)
 *    - test a day value equal to the day limits for March (1, 31)
 *    - test a day value equal to the day limits June (1, 30)
 *    - test a day value equal to the day limit for a common February (28-2-1981)
 *    - test a day value equal to the day limit for a February of a leap year (29-2-1980)
 */
START_TEST (test_set_date_limits)
{
  guint year, month, day;

  year = MIN_YEAR;
  month = 3;
  day = 30;

  /* Test 1: Test year limits */
  test_set_date_limits_check (year, month, day);

  year = MAX_YEAR;
  test_set_date_limits_check (year, month, day);

  /* Test 2: Test month limits */
  year = 1981;
  month = MIN_MONTH;
  day = 30;
  test_set_date_limits_check (year, month, day);

  month = MAX_MONTH;
  test_set_date_limits_check (year, month, day);

  /* Test 3: Test day limits */
  year = 1981;
  month = 3;
  day = 31;
  test_set_date_limits_check (year, month, day);

  /* Test 4: Test day limits */
  year = 1981;
  month = 6;
  day = 30;
  test_set_date_limits_check (year, month, day);

  /* Test 5: Test february limits */
  year = 1981;
  month = 2;
  day = 28;
  test_set_date_limits_check (year, month, day);

  /* Test 6: Test february limits for a leap year */
  year = 1980;
  month = 2;
  day = 29;
  test_set_date_limits_check (year, month, day);
}
END_TEST

/**
 * Purpose: test invalid parameter values for hildon_date_editor_set_date
 * Cases considered:
 *    - test NULL widget
 *    - test passing GtkHBox instead a HildonDateEditor
 *    - test leap year
 *    - test negative values
 *    - test invalid month days
 *    - test a year value lower and higher than the year limits (1970, 2037)
 *    - test a month value lower and higher than the year limits (1, 12)
 *    - test a day value lower and higher than the year limits (1, 31)
 */
START_TEST (test_set_date_invalid)
{
  guint year, month, day;
  guint ret_year, ret_month, ret_day;
  GtkWidget *aux_object = NULL;

  year = 1981;
  month = 3;
  day = 30;

  /* Set init date */
  hildon_date_editor_set_date (date_editor, year, month, day);

  /* Test 1: Test NULL */
  hildon_date_editor_set_date (NULL, year, month, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);

  /* Test 2: Test another object */
  aux_object = gtk_hbox_new (TRUE, 0);
  hildon_date_editor_set_date ((HildonDateEditor *) (aux_object), year, month, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);
  gtk_widget_destroy (GTK_WIDGET(aux_object));

  /* Test 3: Test leap year */
  hildon_date_editor_set_date (date_editor, year, 2, 29);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);

  /* Restore the original value */
  hildon_date_editor_set_date (date_editor, year, month, day);

  /* Test 4: Test negative values */
  hildon_date_editor_set_date (date_editor, -year, -month, -day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);
  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);

  /* Test 5: Test invalid month days */
  hildon_date_editor_set_date (date_editor, year, 11, 31);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);
  fail_if ((ret_year != year) || (ret_month != month) || (ret_day != day),
	   "hildon-date-editor: The returned date is %u/%u/%u and should be %u/%u/%u", 
	   ret_year, ret_month, ret_day, year, month, day);

  /* Test 6: Test year invalid values, the year value could be set
     under/over the value of the property because the date is not
     validated if the value was not set through the user interface */
  hildon_date_editor_set_date (date_editor, MIN_YEAR - 1, month, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if (ret_year != (MIN_YEAR - 1),
	   "hildon-date-editor: The returned year is %u and should be %u",
	   ret_year, MIN_YEAR - 1);

  hildon_date_editor_set_date (date_editor, MAX_YEAR + 1, month, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if (ret_year != MAX_YEAR + 1,
	   "hildon-date-editor: The returned year is %u and should be %u",
	   ret_year, MAX_YEAR + 1);

  /* Test 7: Test month invalid values, we do not have the same
     problem with the years because both month 0 and 13 are not valid
     for g_date */
  hildon_date_editor_set_date (date_editor, year, MIN_MONTH - 1, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if (ret_month != month,
	   "hildon-date-editor: The returned month is %u and should be %u",
	   ret_month, month);

  hildon_date_editor_set_date (date_editor, year, MAX_MONTH + 1, day);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if (ret_month != month,
	   "hildon-date-editor: The returned month is %u and should be %u",
	   ret_month, month);

  /* Test 8: Test day invalid values */
  hildon_date_editor_set_date (date_editor, year, month, MIN_DAY - 1);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if (ret_day != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   ret_day, day);

  hildon_date_editor_set_date (date_editor, year, month, MAX_DAY + 1);
  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  fail_if (ret_day != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   ret_day, day);
}
END_TEST

/* ----- Test case for get_date -----*/

/* We do not include tests for limit values because we think they're
   tested enought with the set_data tests */

/**
 * Purpose: test getting regular values for hildon_date_editor_get_date
 * Cases considered:
 *    - Set and get date 30/03/1981
 */
START_TEST (test_get_date_regular)
{
  guint year, month, day;
  guint ret_year, ret_month, ret_day;
  GValue value = { 0, };

  year = 1981;
  month = 3;
  day = 30;

  /* Test 1: Test regular values */
  hildon_date_editor_set_date (NULL, year, month, day);

  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, &ret_day);

  g_value_init (&value, G_TYPE_UINT);
  g_object_get_property (G_OBJECT (date_editor), "year", &value);
  fail_if (g_value_get_uint (&value) != ret_year,
	   "hildon-date-editor: get_date failed. The returned year is %u and should be %u",
	   g_value_get_uint (&value),
	   ret_year);

  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  g_object_get_property (G_OBJECT (date_editor), "month", &value);
  fail_if (g_value_get_uint (&value) != ret_month,
	   "hildon-date-editor: get_date failed. The returned month is %u and should be %u",
	   g_value_get_uint (&value),
	   ret_month);

  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  g_object_get_property (G_OBJECT (date_editor), "day", &value);
  fail_if (g_value_get_uint (&value) != ret_day,
	   "hildon-date-editor: get_date failed. The returned day is %u and should be %u",
	   g_value_get_uint (&value),
	   ret_day);
}
END_TEST

/**
 * Purpose: test getting regular values passing invalid arguments for
 * hildon_date_editor_get_date 
 * Cases considered: 
 *    - HildonDateEditor NULL
 *    - year is NULL
 *    - month is NULL
 *    - day is NULL
 */
START_TEST (test_get_date_invalid)
{
  guint year, month, day;
  guint ret_year, ret_month, ret_day;

  year = 1981;
  month = 3;
  day = 30;

  hildon_date_editor_set_date (date_editor, year, month, day);

  /* Check that does not fail */
  hildon_date_editor_get_date (NULL, &ret_year, &ret_month, &ret_day);

  /* Check NULL arguments */
  hildon_date_editor_get_date (date_editor, NULL, &ret_month, &ret_day);
  fail_if (hildon_date_editor_get_year (date_editor) != year,
	   "hildon-date-editor: get_date failed. The returned year is %u and should be %u", 
	   ret_year, year);

  hildon_date_editor_get_date (date_editor, &ret_year, NULL, &ret_day);
  fail_if (hildon_date_editor_get_month (date_editor) != month,
	   "hildon-date-editor: get_date failed. The returned month is %u and should be %u", 
	   ret_month, month);

  hildon_date_editor_get_date (date_editor, &ret_year, &ret_month, NULL);
  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: get_date failed. The returned day is %u and should be %u", 
	   ret_day, day);
}
END_TEST

/* ----- Test case for get_year -----*/

/**
 * Purpose: test getting regular values of the year for hildon_date_editor_get_year
 * Cases considered:
 *    - get a year set with set_date 30/03/1981
 *    - get a year set with set_year 1980
 *    - get a year set with set_property 2004
 */
START_TEST (test_get_year_regular)
{
  guint year, month, day;
  GValue value = {0, };

  year = 1981;
  month = 3;
  day = 30;

  /* Test 1: Set year with set_date */
  hildon_date_editor_set_date (date_editor, year, month, day);

  fail_if (hildon_date_editor_get_year (date_editor) != year,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), year);

  /* Test 2: set year with set_year */
  year = 1980;
  hildon_date_editor_set_year (date_editor, year);

  fail_if (hildon_date_editor_get_year (date_editor) != year,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), year);

  /* Test 3: set year with set_property */
  year = 2004;
  g_value_init (&value, G_TYPE_UINT);
  g_value_set_uint (&value, year);
  g_object_set_property (G_OBJECT (date_editor), "year", &value);

  fail_if (hildon_date_editor_get_year (date_editor) != year,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), year);
}
END_TEST

/**
 * Purpose: test getting year when a value over the limits was set for
 * hildon_date_editor_get_year
 * Cases considered:
 *    - test year 2037
 *    - test year 1970
 */
START_TEST (test_get_year_limits)
{
  guint year;

  year = 1981;

  /* Set init year */
  hildon_date_editor_set_year (date_editor, year);

  /* Test 1: upper limit */
  hildon_date_editor_set_year (date_editor, MAX_YEAR);

  fail_if (hildon_date_editor_get_year (date_editor) != MAX_YEAR,
	   "hildon-date-editor: The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), MAX_YEAR);

  /* Test 2: lower limit */
  hildon_date_editor_set_year (date_editor, MIN_YEAR);

  fail_if (hildon_date_editor_get_year (date_editor) != MIN_YEAR,
	   "hildon-date-editor: The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), MIN_YEAR);
}
END_TEST

/**
 * Purpose: test getting a year for invalid attributes for
 * hildon_date_editor_get_year
 * Cases considered: 
 *    - HildonDateEditor is NULL
 *    - Pass a GtkHBox instead a HildonDateEditor
 *    - test year 2038
 *    - test year 1969
 */
START_TEST (test_get_year_invalid)
{
  guint ret_year;
  GtkWidget *aux_object = NULL;

  /* Test 1: Test NULL */
  ret_year = hildon_date_editor_get_year (NULL);
  fail_if (ret_year != 0,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   ret_year, 0);

  /* Test 2: another object */
  aux_object = gtk_hbox_new (TRUE, 0);
  ret_year = hildon_date_editor_get_year ((HildonDateEditor *) (aux_object));
  fail_if (ret_year != 0,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   ret_year, 0);
  gtk_widget_destroy (GTK_WIDGET(aux_object));

  /* Test 3: upper limit, the test is OK but it shouldn't. The reason
     is that the value of the date is not validated by Hildon since it
     was not set using the UI */
  hildon_date_editor_set_year (date_editor, MAX_YEAR + 1);

  fail_if (hildon_date_editor_get_year (date_editor) != MAX_YEAR + 1,
	   "hildon-date-editor: The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), MAX_YEAR + 1);

  /* Test 4: lower limit, see the above comment */
  hildon_date_editor_set_year (date_editor, MIN_YEAR - 1);

  fail_if (hildon_date_editor_get_year (date_editor) != MIN_YEAR - 1,
	   "hildon-date-editor: The returned year is %u and should be %u", 
	   hildon_date_editor_get_year (date_editor), MIN_YEAR - 1);
}
END_TEST

/* ----- Test case for set_year -----*/

/**
 * Purpose: test setting a regular value for a year for
 * hildon_date_editor_set_year
 * Cases considered:
 *    - Set year 1981
 */
START_TEST (test_set_year_regular)
{
  guint year;
  guint ret_year;

  year = 1981;

  /* Test 1: Try year 1981 */
  hildon_date_editor_set_year (date_editor, year);
  ret_year = hildon_date_editor_get_year (date_editor);

  fail_if (ret_year != year,
	   "hildon-date-editor: set_year failed. The returned year is %u and should be %u", 
	   ret_year, year);
}
END_TEST

/**
 * Purpose: test setting values of the year over the limits for
 * hildon_date_editor_set_year
 * Cases considered:
 *    - Set year 2037
 *    - Set year 1970
 */
START_TEST (test_set_year_limits)
{
  guint year;
  GValue value = { 0, };

  year = 1981;

  /* Set init date */
  hildon_date_editor_set_year (date_editor, year);

  /* Test 1: Test upper limit */
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_year (date_editor, MAX_YEAR);
  g_object_get_property (G_OBJECT (date_editor), "year", &value);
  fail_if (g_value_get_uint (&value) != MAX_YEAR,
	   "hildon-date-editor: The returned year is %u and should be %u",
	   g_value_get_uint (&value), year);

  /* Test 2: Test lower limit */
  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_year (date_editor, MIN_YEAR);
  g_object_get_property (G_OBJECT (date_editor), "year", &value);
  fail_if (g_value_get_uint (&value) != MIN_YEAR,
	   "hildon-date-editor: The returned year is %u and should be %u",
	   g_value_get_uint (&value), MIN_YEAR);
}
END_TEST

/* ----- Test case for get_month -----*/

/**
 * Purpose: test getting a year for regular values for
 * hildon_date_editor_get_month
 * Cases considered:
 *    - set month with set_date 30/03/1981
 *    - set month with set_month 1
 *    - set month with set_property 7
 */
START_TEST (test_get_month_regular)
{
  guint year, month, day;
  GValue value = {0, };

  year = 1981;
  month = 3;
  day = 30;

  /* Test 1: Set year with set_date */
  hildon_date_editor_set_date (date_editor, year, month, day);

  fail_if (hildon_date_editor_get_month (date_editor) != month,
	   "hildon-date-editor: The returned month is %u and should be %u", 
	   hildon_date_editor_get_month (date_editor), month);

  /* Test 2: set month with set_month */
  month = 1;
  hildon_date_editor_set_month (date_editor, month);

  fail_if (hildon_date_editor_get_month (date_editor) != month,
	   "hildon-date-editor: The returned month is %u and should be %u", 
	   hildon_date_editor_get_month (date_editor), month);

  /* Test 3: set month with set_property */
  month = 7;
  g_value_init (&value, G_TYPE_UINT);
  g_value_set_uint (&value, month);
  g_object_set_property (G_OBJECT (date_editor), "month", &value);

  fail_if (hildon_date_editor_get_month (date_editor) != month,
	   "hildon-date-editor: The returned month is %u and should be %u", 
	   hildon_date_editor_get_month (date_editor), month);
}
END_TEST

/**
 * Purpose: test getting values of the month over the limits for
 * hildon_date_editor_get_month
 * Cases considered:
 *    - Get month 12
 *    - Get month 1
 */
START_TEST (test_get_month_limits)
{

  /* Test 1: Upper limit */
  hildon_date_editor_set_month (date_editor, MAX_MONTH);

  fail_if (hildon_date_editor_get_month (date_editor) != MAX_MONTH,
	   "hildon-date-editor: get_month failed. The returned month is %u and should be %u", 
	   hildon_date_editor_get_month (date_editor), MAX_MONTH);

  /* Test 2: Lower limit */
  hildon_date_editor_set_month (date_editor, MIN_MONTH);

  fail_if (hildon_date_editor_get_month (date_editor) != MIN_MONTH,
	   "hildon-date-editor: get_month failed. The returned month is %u and should be %u", 
	   hildon_date_editor_get_month (date_editor), MIN_MONTH);
}
END_TEST

/**
 * Purpose:  test getting a month for invalid attributes for
 * hildon_date_editor_get_month
 * Cases considered:
 *    - HildonDateEditor is NULL
 *    - HildonDateEditor is really a GtkHBox
 */
START_TEST (test_get_month_invalid)
{
  guint ret_month;
  GtkWidget *aux_object = NULL;

  /* Test 1: Test NULL */
  ret_month = hildon_date_editor_get_month (NULL);
  fail_if (ret_month != 0,
	   "hildon-date-editor: get_month failed. The returned month is %u and should be %u", 
	   ret_month, 0);

  /* Test 2: another object */
  aux_object = gtk_hbox_new (TRUE, 0);
  ret_month = hildon_date_editor_get_month ((HildonDateEditor *) (aux_object));
  fail_if (ret_month != 0,
	   "hildon-date-editor: get_month failed. The returned month is %u and should be %u", 
	   ret_month, 0);
  gtk_widget_destroy (GTK_WIDGET(aux_object));
}
END_TEST

/* ----- Test case for set_month -----*/

/**
 * Purpose: test setting regular values for month for
 * hildon_date_editor_set_month
 * Cases considered:
 *    - Set month 3
 */
START_TEST (test_set_month_regular)
{
  guint month;
  guint ret_month;

  month = 3;

  /* Test 1: Try month March (3) */
  hildon_date_editor_set_month (date_editor, month);
  ret_month = hildon_date_editor_get_month (date_editor);

  fail_if (ret_month != month,
	   "hildon-date-editor: set_month failed. The returned month is %u and should be %u", 
	   ret_month, month);
}
END_TEST

/**
 * Purpose: test setting values for month over the limits for
 * hildon_date_editor_get_month
 * Cases considered:
 *    - Set month 12
 *    - Set month 1
 */
START_TEST (test_set_month_limits)
{
  GValue value = { 0, };

  /* Test 1: Test upper limit */
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_month (date_editor, MAX_MONTH);
  g_object_get_property (G_OBJECT (date_editor), "month", &value);
  fail_if (g_value_get_uint (&value) != MAX_MONTH,
	   "hildon-date-editor: The returned month is %u and should be %u",
	   g_value_get_uint (&value), MAX_MONTH);

  /* Test 2: Test lower limit */
  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_month (date_editor, MIN_MONTH);
  g_object_get_property (G_OBJECT (date_editor), "month", &value);
  fail_if (g_value_get_uint (&value) != MIN_MONTH,
	   "hildon-date-editor: The returned month is %u and should be %u",
	   g_value_get_uint (&value), MIN_MONTH);
}
END_TEST

/* ----- Test case for get_day -----*/

/**
 * Purpose: test getting regular values for day for
 * hildon_date_editor_get_day
 * Cases considered:
 *    - Get a day set with set_date 30/03/1981
 *    - Get a day set with set_day 6
 *    - Get a day set with set_property 10
 */
START_TEST (test_get_day_regular)
{
  guint year, month, day;
  GValue value = {0, };

  year = 1981;
  month = 3;
  day = 30;

  /* Test 1: Set day with set_date */
  hildon_date_editor_set_date (date_editor, year, month, day);

  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);

  /* Test 2: set day with set_day */
  day = 6;
  hildon_date_editor_set_day (date_editor, day);

  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);


  /* Test 3: set day with set_property */
  day = 10;
  g_value_init (&value, G_TYPE_UINT);
  g_value_set_uint (&value, day);
  g_object_set_property (G_OBJECT (date_editor), "day", &value);

  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);

}
END_TEST

/**
 * Purpose: test getting a day set over the limits for
 * hildon_date_editor_get_day
 * Cases considered:
 *    - Get day 31 for March
 *    - Get day 30 for June
 *    - Get day 29 for February for a leap year
 *    - Get day 28 for February for a common year
 *    - Get day 1
 */
START_TEST (test_get_day_limits)
{
  guint day, month, year;

  year = 1981;
  month = 3;
  day = 31;

  /* Test 1: 31 of February */
  hildon_date_editor_set_date (date_editor, year, month, day);
  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: get_day failed. The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);

  /* Test 2: 30 of February */
  month = 6;
  day = 30;
  hildon_date_editor_set_date (date_editor, year, month, day);
  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: get_day failed. The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);

  /* Test 3: 29 of February */
  year = 1980;
  month = 2;
  day = 29;
  hildon_date_editor_set_date (date_editor, year, month, day);
  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: get_day failed. The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);

  /* Test 3: 28 of February */
  year = 1981;
  month = 2;
  day = 28;
  hildon_date_editor_set_date (date_editor, year, month, day);
  fail_if (hildon_date_editor_get_day (date_editor) != day,
	   "hildon-date-editor: get_day failed. The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), day);

  /* Test 5: day 1 */
  hildon_date_editor_set_day (date_editor, 1);

  fail_if (hildon_date_editor_get_day (date_editor) != 1,
	   "hildon-date-editor: get_day failed. The returned day is %u and should be %u", 
	   hildon_date_editor_get_day (date_editor), 1);
}
END_TEST

/**
 * Purpose: test getting a day with invalid attributes for
 * hildon_date_editor_get_day
 * Cases considered:
 *    - HildonDateEditor is NULL
 *    - HildonDateEditor is really a GtkHBox
 */
START_TEST (test_get_day_invalid)
{
  guint ret_year;
  GtkWidget *aux_object = NULL;

  /* Test 1: Test NULL */
  ret_year = hildon_date_editor_get_year (NULL);
  fail_if (ret_year != 0,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   ret_year, 0);

  /* Test 2: another object */
  aux_object = gtk_hbox_new (TRUE, 0);
  ret_year = hildon_date_editor_get_year ((HildonDateEditor *) aux_object);
  fail_if (ret_year != 0,
	   "hildon-date-editor: get_year failed. The returned year is %u and should be %u", 
	   ret_year, 0);
  gtk_widget_destroy (GTK_WIDGET(aux_object));
}
END_TEST

/* ----- Test case for set_day -----*/

/**
 * Purpose: test setting a regular value for day for
 * hildon_date_editor_get_day
 * Cases considered:
 *    - Set day 30
 */
START_TEST (test_set_day_regular)
{
  guint day;
  guint ret_day;

  day = 25;

  /* Test 1: Try day 30 */
  hildon_date_editor_set_day (date_editor, day);
  ret_day = hildon_date_editor_get_day (date_editor);

  fail_if (ret_day != day,
	   "hildon-date-editor: set_day failed. The returned day is %u and should be %u", 
	   ret_day, day);
}
END_TEST

/**
 * Purpose: test seeting a day over the limits for
 * hildon_date_editor_get_day
 * Cases considered:
 *    - Set day 31
 *    - Set day 30
 *    - Set day 29
 *    - Set day 28
 *    - Set day 1
 */
START_TEST (test_set_day_limits)
{
  guint day, year, month;
  GValue value = { 0, };

  year = 1981;
  month = 3;
  day = 31;

  /* Set init date */
  hildon_date_editor_set_date (date_editor, year, month, MIN_DAY);

  /* Test 1: Test 31/03 */
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_day (date_editor, day);
  g_object_get_property (G_OBJECT (date_editor), "day", &value);
  fail_if (g_value_get_uint (&value) != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   g_value_get_uint (&value), day);

  /* Test 2: Test 30/06 */
  month = 6;
  day = 30;
  hildon_date_editor_set_date (date_editor, year, month, 1);
  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_day (date_editor, day);
  g_object_get_property (G_OBJECT (date_editor), "day", &value);
  fail_if (g_value_get_uint (&value) != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   g_value_get_uint (&value), day);

  /* Test 3: Test 29/02/1980 */
  year = 1980;
  month = 2;
  day = 29;
  hildon_date_editor_set_date (date_editor, year, month, 1);
  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_day (date_editor, day);
  g_object_get_property (G_OBJECT (date_editor), "day", &value);
  fail_if (g_value_get_uint (&value) != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   g_value_get_uint (&value), day);

  /* Test 4: Test 28/02/1981 */
  year = 1981;
  month = 2;
  day = 28;
  hildon_date_editor_set_date (date_editor, year, month, 1);
  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_day (date_editor, day);
  g_object_get_property (G_OBJECT (date_editor), "day", &value);
  fail_if (g_value_get_uint (&value) != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   g_value_get_uint (&value), day);

  /* Test 5: Test 1/02/1980 */
  year = 1980;
  month = 2;
  day = 1;
  hildon_date_editor_set_date (date_editor, year, month, 10);
  g_value_unset (&value);
  g_value_init (&value, G_TYPE_UINT);
  hildon_date_editor_set_day (date_editor, day);
  g_object_get_property (G_OBJECT (date_editor), "day", &value);
  fail_if (g_value_get_uint (&value) != day,
	   "hildon-date-editor: The returned day is %u and should be %u",
	   g_value_get_uint (&value), day);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_date_editor_suite(void)
{
  /* Create the suite */
  Suite *s = suite_create("HildonDateEditor");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_date");
  TCase *tc2 = tcase_create("get_date");
  TCase *tc3 = tcase_create("get_year");
  TCase *tc4 = tcase_create("set_year");
  TCase *tc5 = tcase_create("get_month");
  TCase *tc6 = tcase_create("set_month");
  TCase *tc7 = tcase_create("get_day");
  TCase *tc8 = tcase_create("set_day");

  /* Create test case for set_date and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc1, test_set_date_regular);
  tcase_add_test(tc1, test_set_date_limits);
  tcase_add_test(tc1, test_set_date_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for get_date and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc2, test_get_date_regular);
  tcase_add_test(tc2, test_get_date_invalid);
  suite_add_tcase (s, tc2);

  /* Create test case for get_year and add it to the suite */
  tcase_add_checked_fixture(tc3, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc3, test_get_year_regular);
  tcase_add_test(tc3, test_get_year_limits);
  tcase_add_test(tc3, test_get_year_invalid);
  suite_add_tcase (s, tc3);

  /* Create test case for set_year and add it to the suite */
  tcase_add_checked_fixture(tc4, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc4, test_set_year_regular);
  tcase_add_test(tc4, test_set_year_limits);
  suite_add_tcase (s, tc4);

  /* Create test case for get_month and add it to the suite */
  tcase_add_checked_fixture(tc5, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc5, test_get_month_regular);
  tcase_add_test(tc5, test_get_month_limits);
  tcase_add_test(tc5, test_get_month_invalid);
  suite_add_tcase (s, tc5);

  /* Create test case for set_month and add it to the suite */
  tcase_add_checked_fixture(tc6, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc6, test_set_month_regular);
  tcase_add_test(tc6, test_set_month_limits);
  suite_add_tcase (s, tc6);

  /* Create test case for get_day and add it to the suite */
  tcase_add_checked_fixture(tc7, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc7, test_get_day_regular);
  tcase_add_test(tc7, test_get_day_limits);
  tcase_add_test(tc7, test_get_day_invalid);
  suite_add_tcase (s, tc7);

  /* Create test case for set_day and add it to the suite */
  tcase_add_checked_fixture(tc8, fx_setup_default_date_editor, fx_teardown_default_date_editor);
  tcase_add_test(tc8, test_set_day_regular);
  tcase_add_test(tc8, test_set_day_limits);
  suite_add_tcase (s, tc8);

  /* Return created suite */
  return s;
}
