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
#include <gtk/gtkcontainer.h>
#include "test_suites.h"
#include "check_utils.h"

#include <unistd.h>

#include <hildon/hildon-time-editor.h>

#define MIN_DURATION 0
#define MAX_DURATION 359999

/* -------------------- Fixtures -------------------- */

static GtkWidget *showed_window = NULL;
/* static HildonTimeEditor *time_editor = NULL;  */

static void
fx_setup_default_time_editor ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  showed_window =  create_test_window ();
}

static void
fx_teardown_default_time_editor ()
{
  /* Destroy the window */
  gtk_widget_destroy (showed_window);
}

static HildonTimeEditor *create_and_show_time_editor(GtkWidget *window, gboolean seconds,gboolean duration)
{
  HildonTimeEditor *time_editor = NULL;

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_show_seconds(time_editor,seconds);
  hildon_time_editor_set_duration_mode(time_editor,duration);

  /* Add some widgets to the window and displays the window */
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET(time_editor));
  show_all_test_window (window);

  hildon_time_editor_set_duration_mode(time_editor,duration);
  
  fail_if( !HILDON_IS_TIME_EDITOR(time_editor),"hildon-time-editor: Creation failed.");
  return time_editor;
}

static void remove_and_destroy_time_editor(GtkWidget *window,HildonTimeEditor ** time_editor)
{
  
  gtk_container_remove (GTK_CONTAINER (window), GTK_WIDGET(*time_editor));
  gtk_widget_hide_all (window);

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_time -----*/

/**
 * Purpose: Check that regular time values are set and get properly       
 * Cases considered:
 *    - Set and get time to 5h 30m using time editor object.
 *    - Set and get time to 18h 2m using time editor object.
 *    - Set and get time to 5h 30m 45s using time editor object.
 *    - Set and get time to 18h 2m 5s using time editor object.
 *    - Set and get time to 5:30:45 using time editor object with duration mode.
 *    - Set and get time to 50:40:30 using time editor object with duration mode.
 */
START_TEST (test_set_time_regular)
{
  guint initial_hours;
  guint initial_minutes;
  guint initial_seconds;
  guint hours;
  guint minutes;
  guint seconds;
  HildonTimeEditor *time_editor = NULL;

  /* Test 1: Set time value to 5h 30m 0s to test time (am) without seconds */
  initial_hours=5;
  initial_minutes=30;
  initial_seconds=0;
  
  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);
  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm). Result values are: (%dh %dm)",
          initial_hours,initial_minutes,hours,minutes);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 2: Set time value to 18h 2m 0s to test time (pm) without seconds*/
  initial_hours=18;
  initial_minutes=2;
  initial_seconds=0;

  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);
  
  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm). Result values are: (%dh %dm)",
          initial_hours,initial_minutes,hours,minutes);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 3: Set time value to 5h 30m 45s to test time (am) with seconds*/
  initial_hours=5;
  initial_minutes=30;
  initial_seconds=45;

  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm %ds). Result values are: (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);
  
  /* Test 4: Set time value to 18h 2m 5s to test time (pm) with seconds */
  initial_hours=18;
  initial_minutes=2;
  initial_seconds=5;


  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm %ds). Result values are: (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);
   

  /* Test 5: Set time value to 5:30:45 to test duration*/
  initial_hours=5;
  initial_minutes=30;
  initial_seconds=45;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);


  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%d:%d:%d). Result values are: (%d:%d:%d)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 6: Set time value to 50:40:30 to test duration */
  initial_hours=50;
  initial_minutes=40;
  initial_seconds=30;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);
  
  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);
  
  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%d:%d:%d). Result values are: (%d:%d:%d)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);


}
END_TEST

/**
 * Purpose: Check that limit time values are set and get properly
 * Cases considered:
 *    - Set and get time to 0h 0m using time editor object.
 *    - Set and get time to 23h 59m using time editor object.
 *    - Set and get time to 23h 59m 59s using time editor object.
 *    - Set and get time to 0h 59m 59s using time editor object.
 *    - Set and get time to 99:59:59 using time editor object on duration mode.
 *    - Set and get time to 0:0:0 using time editor object on duration mode.
 */
START_TEST (test_set_time_limits)
{
  guint initial_hours;
  guint initial_minutes;
  guint initial_seconds;
  guint hours;
  guint minutes;
  guint seconds;
  HildonTimeEditor *time_editor = NULL;
    
  /* Test 1: Set time value to 00h 00m 00s to test time (am) without seconds*/
  initial_hours=0;
  initial_minutes=0;
  initial_seconds=0;

  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm). Result values are: (%dh %dm)",
          initial_hours,initial_minutes,hours,minutes);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 2: Set time value to 23h 59m 0s to test time (pm) without seconds*/
  initial_hours=23;
  initial_minutes=59;
  initial_seconds=0;

  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm). Result values are: (%dh %dm)",
          initial_hours,initial_minutes,hours,minutes);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 3: Set time value to 23h 59m 59s to test time (am) with seconds*/
  initial_hours=23;
  initial_minutes=59;
  initial_seconds=59;

  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm %ds). Result values are: (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 4: Set time value to 0h 59m 59s to test time (am) with seconds */
  initial_hours=0;
  initial_minutes=59;
  initial_seconds=59;
  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm %ds). Result values are: (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);
    
  /* Test 5: Set time value to 99:59:59 to test with duration mode */
  initial_hours=99;
  initial_minutes=59;
  initial_seconds=59;
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm %ds). Result values are: (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 6: Set time value to 0:0:0 to test with duration mode */
  initial_hours=0;
  initial_minutes=0;
  initial_seconds=0;

  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((hours != initial_hours) || (minutes != initial_minutes) || (seconds != initial_seconds),
          "hildon-time-editor: set_time or get_time failed with values (%dh %dm %ds). Result values are: (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds);
    
  remove_and_destroy_time_editor(showed_window,&time_editor);

    
}
END_TEST

/**
 * Purpose: Check that invalid time values are set and get properly
 * Cases considered:
 *    - Set NULL the editor time object.
 *    - Test NULL (time object) on get_time
 *    - Set and get time to 0h 60m using time editor object.
 *    - Set and get time to 0h 0m 60s using time editor object.
 *    - Set and get time to 24h 60m 60s using time editor object.
 *    - Set and get time to 50h 100m 100s using time editor object.
 *    - Set and get time to 0:80:80 using time editor object.
 *    - Set and get time to 110:80:80 using time editor object.
 */
START_TEST (test_set_time_invalid)
{
  guint initial_hours;
  guint initial_minutes;
  guint initial_seconds;
  guint expected_hours;
  guint expected_minutes;
  guint expected_seconds;
  guint hours;
  guint minutes;
  guint seconds;
  HildonTimeEditor *time_editor = NULL;

  /* Initialization to test 1, test 2 and test 3*/
  initial_hours=0;
  initial_minutes=60;
  initial_seconds=0;
  expected_hours=1;
  expected_minutes=0;
  expected_seconds=0;

  /* Test 1: Set NULL (set_time) */
  hildon_time_editor_set_time (NULL,initial_hours,initial_minutes,initial_seconds);

  /* Test 2: Set NULL (get_time) */
  hildon_time_editor_get_time(NULL,&hours,&minutes,&seconds);
 
  /* Test 3: Set time value to 00h 60m 00s to test time (am) without seconds and expect the correct value 01h 00m 00s */

  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);
  
  remove_and_destroy_time_editor(showed_window,&time_editor);

  fail_if((expected_hours != hours) || (expected_minutes != minutes) || (expected_seconds != seconds),
          "hildon-time-editor: set_time/get_time failed with (%dh %dm). Result values are (%dh %dm) and expected values are (%dh %dm)",
          initial_hours,initial_minutes,hours,minutes,expected_hours,expected_minutes);
    
  /* Test 4: Set time value to 0h 0m 60s to test time (pm) without seconds and expect the correct value 00h 01m 00s*/
  initial_hours=0;
  initial_minutes=0;
  initial_seconds=60;
  expected_hours=0;
  expected_minutes=1;
  expected_seconds=0;

  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((expected_hours != hours) || (expected_minutes != minutes) || (expected_seconds != seconds),
          "hildon-time-editor: set_time/get_time failed with (%dh %dm %ds). Result values are (%dh %dm %ds) and expected values are (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds,expected_hours,expected_minutes,expected_seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 5: Set time value to 24h 60m 60s to test time (am) with seconds*/
  initial_hours=24;
  initial_minutes=60;
  initial_seconds=60;
  expected_hours=23;
  expected_minutes=1;
  expected_seconds=0;

  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((expected_hours != hours) || (expected_minutes != minutes) || (expected_seconds != seconds),
          "hildon-time-editor: set_time/get_time failed with values (%dh %dm %ds). Result values are (%dh %dm %ds) and expected values are (%dh %dm %ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds,expected_hours,expected_minutes,expected_seconds);
  
  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 6: Set time value to 50h 100m 100s to test time (am) with seconds */
  initial_hours=50;
  initial_minutes=100;
  initial_seconds=100;
  expected_hours=23;
  expected_minutes=41;
  expected_seconds=40;

  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((expected_hours != hours) || (expected_minutes != minutes) || (expected_seconds != seconds),
          "hildon-time-editor: set_time/get_time failed with values (%d:%dm:%ds). Result values are (%d:%dm:%ds) and expected values are (%d:%dm:%ds)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds,expected_hours,expected_minutes,expected_seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 7: Set time value to 0:80:80 to test with duration mode */
  initial_hours=0;
  initial_minutes=80;
  initial_seconds=80;
  expected_hours=1;
  expected_minutes=21;
  expected_seconds=20;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  fail_if((expected_hours != hours) || (expected_minutes != minutes) || (expected_seconds != seconds),
          "hildon-time-editor: set_time/get_time failed with values (%d:%d:%d). Result values are (%d:%d:%d) and expected values are (%d:%d:%d)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds,expected_hours,expected_minutes,expected_seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 8: Set time value to 110:80:80 to test with duration mode */
  initial_hours=110;
  initial_minutes=80;
  initial_seconds=80;
  expected_hours=99;
  expected_minutes=59;
  expected_seconds=59;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_time(time_editor,initial_hours,initial_minutes,initial_seconds);

  hildon_time_editor_get_time(time_editor,&hours,&minutes,&seconds);

  
  fail_if((expected_hours != hours) || (expected_minutes != minutes) || (expected_seconds != seconds),
          "hildon-time-editor: set_time/get_time failed with values (%d:%d:%d). Result values are (%d:%d:%d) and expected values are (%d:%d:%d)",
          initial_hours,initial_minutes,initial_seconds,hours,minutes,seconds,expected_hours,expected_minutes,expected_seconds);

  remove_and_destroy_time_editor(showed_window,&time_editor);

}
END_TEST


/* ----- Test case for set_ticks -----*/

/**
 * Purpose: Check that regular ticks values are set and get properly
 * Cases considered:
 *    - Set and get ticks to 45660s using time editor object with time mode.
 *    - Set and get ticks to 124860s using time editor object with duration mode.
 */
START_TEST (test_set_ticks_regular)
{
  guint initial_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;
  
  /* Test 1: Set tick to 45660 seconds to test set_ticks */
  
  initial_ticks=45660;
  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);
  
  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d",initial_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);
    

  /* Test 2: Set tick to 124860 seconds to test set_ticks */

  initial_ticks=124860;

  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d",initial_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);
  
}
END_TEST

/**
 * Purpose: Check that limit ticks values are set and get properly
 * Cases considered:
 *    - Set and get ticks to 0s using time editor object with time mode.
 *    - Set and get ticks to 86399s  using time editor object with time mode. 86399 seconds are 23h 59m 59s.
 *    - Set and get ticks to 0s using time editor object with duration mode.
 *    - Set and get ticks to 359999s using time editor object with duration mode. 359999 seconds are 99:59:59.
 */
START_TEST (test_set_ticks_limits)
{
  guint initial_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;
  
  /* Test 1: Set ticks to 0 seconds to test set_ticks with */
  
  initial_ticks=0;
  time_editor = create_and_show_time_editor(showed_window,FALSE,FALSE);
  
  hildon_time_editor_set_ticks(time_editor,initial_ticks);
  
  ticks = hildon_time_editor_get_ticks(time_editor);
  
  fail_if(( initial_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d",initial_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);
   
  /* Test 2: Set ticks to 86399 seconds to test set_ticks */
  initial_ticks=86399;
  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);
  
  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d",initial_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 3: Set ticks to 0 seconds to test set_ticks */

  initial_ticks=0;
  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);
    
  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d",initial_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 4: Set ticks to 359999 seconds to test set_ticks */

  initial_ticks=359999;
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d",initial_ticks,ticks);
  remove_and_destroy_time_editor(showed_window,&time_editor);

}
END_TEST

/**
 * Purpose: Check that invalid ticks values are set and get properly. In this tests we'll test several invalid
 *          values with the same case because the "mod" operator will define the time set.
 * Cases considered:
 *    - Set and get ticks to 86400s using time editor object with time mode. 86399 seconds are 23h 59m 59s.
 *    - Set and get ticks to 360000s using time editor object with time mode.
 *    - Set and get ticks to 90060s using time editor object with time.
 *    - Set and get ticks to 360000s using time editor object with duration mode. 359999 seconds are 99:59:59.
 *    - Set and get ticks to 654333s using time editor object with duration mode.
 *    - Set ticks on NULL object.
 */
START_TEST (test_set_ticks_invalid)
{
  guint initial_ticks;
  guint expected_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;

  initial_ticks=86400;
  expected_ticks=82800;

  /* Test 1: Set ticks to 86400 seconds to test set_ticks */
  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( expected_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d and result is %d",
          initial_ticks,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 2: Set ticks to 360000 seconds to test set_ticks */
  initial_ticks=360001;
  expected_ticks=82801;

  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( expected_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d and result is %d",
          initial_ticks,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 3: Set ticks to 90060 seconds to test set_ticks */
  initial_ticks=90060;
  expected_ticks=82860;

  time_editor = create_and_show_time_editor(showed_window,TRUE,FALSE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( expected_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d and result is %d",
          initial_ticks,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 4: Set ticks to 360000 seconds to test set_ticks */

  initial_ticks=360000;
  expected_ticks=359999;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( expected_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d and result is %d",
          initial_ticks,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 5: Set ticks to 654333 seconds to test set_ticks */

  initial_ticks=654333;
  expected_ticks=359999;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,initial_ticks);

  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( expected_ticks!=ticks ),
          "hildon-time-editor: set_ticks or get_ticks failed with %d. Expected result is %d and result is %d",
          initial_ticks,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 6: Set initial_ticks on NULL */
  hildon_time_editor_set_ticks(NULL,initial_ticks);
}
END_TEST


/* ----- Test case for set_duration_min_set_duration_max -----*/

/**
 * Purpose: Check that regular minimum/maximum values are set/get properly and check that set_duration_mode works well,
 *          because min and max can not be set without duration mode set.
 * Cases considered:
 *    - Set and get minimum duration to 100000 having ticks under 100000 using time editor object with duration mode.
 *    - Set and get minimum duration to 100000 having ticks over 100000 using time editor object with duration mode.
 *    - Set and get maximum duration to 200000 having ticks under 200000 using time editor object with duration mode.
 *    - Set and get maximum duration to 200000 having ticks over 200000 using time editor object with duration mode.
 */
START_TEST (test_set_duration_min_set_duration_max_regular)
{
  guint initial_duration_min;
  guint duration_min;
  guint initial_duration_max;
  guint duration_max;
  guint expected_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;

  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);
  /* Initialize values in order to avoid side effects from previous tests */
  hildon_time_editor_set_duration_min(time_editor,MIN_DURATION);
  hildon_time_editor_set_duration_max(time_editor,MAX_DURATION);

  /* set_min tests*/
    
  /* Test 1: Set and get minimum duration to 100000 having ticks under 100000 using time editor object with duration mode. */
  initial_duration_min=100000;
  expected_ticks=initial_duration_min;

  hildon_time_editor_set_ticks(time_editor,initial_duration_min-1000);
  hildon_time_editor_set_duration_min(time_editor,initial_duration_min);
  duration_min = hildon_time_editor_get_duration_min(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_min!=duration_min) | (ticks!=expected_ticks),
          "hildon-time-editor: set_duration_min or get_duration_min failed with %d. Expected result is %d. Set ticks to %d, expected ticks are %d and result is %d",
          initial_duration_min,duration_min,initial_duration_min-1000,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 2: Set and get minimum duration to 100000 having ticks over 100000 using time editor object with duration mode.*/
  initial_duration_min=100000;
  expected_ticks=initial_duration_min+980;

  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_min(time_editor,initial_duration_min);
  duration_min = hildon_time_editor_get_duration_min(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_min!=duration_min) | (ticks!=expected_ticks),
          "hildon-time-editor: set_duration_min or get_duration_min failed with %d. Expected result is %d. Set ticks to %d and result is %d",
          initial_duration_min,duration_min,ticks,expected_ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);



  /* Test 3: Set and get maximum duration to 200000 having ticks under 200000 using time editor object with duration mode. */
  initial_duration_max=200000;
  expected_ticks=initial_duration_max-1040;
  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_max(time_editor,initial_duration_max);
  duration_max = hildon_time_editor_get_duration_max(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_max!=duration_max) | (ticks!=expected_ticks),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d. Set ticks to %d and result is %d",
          initial_duration_max,duration_max,ticks,expected_ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 4: Set and get maximum duration to 200000 having ticks over 200000 using time editor object with duration mode.*/
  initial_duration_max=200000;
  expected_ticks=initial_duration_max+1000;
  time_editor = create_and_show_time_editor(showed_window,FALSE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_max(time_editor,initial_duration_max);
  duration_max = hildon_time_editor_get_duration_max(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_max!=duration_max) | (ticks!=initial_duration_max),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d. Set ticks to %d, expected result is %d and result is %d",
          initial_duration_max,duration_max,expected_ticks,initial_duration_max,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);
 
}
END_TEST


/**
 * Purpose: Check that limits duration minimum/maximum values are set and get properly.
 * Cases considered:
 *    - Set and get minimum duration to MIN_DURATION having ticks over MIN_DURATION using time editor object with duration mode.
 *    - Set and get maximum duration to MAX_DURATION having ticks under MAX_DURATION.
 *      using time editor object with duration mode. 359940 seconds are 99:59:00.
 *    - Set and get maximum duration to MIN_DURATION having ticks over MIN_DURATION using time editor object with duration mode.
 *    - Set and get minimum duration to MAX_DURATION having ticks under MAX_DURATION.
 *      using time editor object with duration mode. 359999 seconds are 99:59:59.
 */
START_TEST (test_set_duration_min_set_duration_max_limits)
{
  guint initial_duration_min;
  guint duration_min;
  guint initial_duration_max;
  guint duration_max;
  guint expected_ticks;
  guint ticks;
     HildonTimeEditor *time_editor = NULL;


  /* Test 1: Set minimum duration to MAX_DURATION seconds having ticks under MAX_DURATION. */
  expected_ticks=MIN_DURATION+1;
  initial_duration_min=MIN_DURATION;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_min(time_editor,initial_duration_min);
  duration_min = hildon_time_editor_get_duration_min(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_min!=duration_min) | (ticks!=expected_ticks),
          "hildon-time-editor: set_duration_min or get_duration_min failed with %d. Expected result is %d. Set ticks to %d and result is %d",
          initial_duration_min,duration_min,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 2: Set maximum duration to MAX_DURATION seconds having ticks under MAX_DURATION. */
  expected_ticks=MAX_DURATION-59;
  initial_duration_max=MAX_DURATION;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_max(time_editor,initial_duration_max);
  duration_max = hildon_time_editor_get_duration_max(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_max!=duration_max) | (ticks!=expected_ticks),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d. Set ticks to %d and result is %d",
          initial_duration_max,duration_max,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 3:  Set and get maximum duration to MIN_DURATION having ticks over MIN_DURATION
     using time editor object with duration mode. */
  expected_ticks=MIN_DURATION+1;
  initial_duration_max=MIN_DURATION;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_max(time_editor,initial_duration_max);
  duration_max = hildon_time_editor_get_duration_max(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_max!=duration_max) | (ticks!=MIN_DURATION),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d. Set ticks to %d and result is %d",
          initial_duration_max,duration_max,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 4: Set and get minimum duration to MIN_DURATION having ticks over MIN_DURATION
     using time editor object with duration mode. */
  expected_ticks=MAX_DURATION-1;
  initial_duration_min=MAX_DURATION;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_min(time_editor,initial_duration_min);
  duration_min = hildon_time_editor_get_duration_min(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( initial_duration_min!=duration_min) | (ticks!=MAX_DURATION),
          "hildon-time-editor: set_duration_min or get_duration_min failed with %d. Expected result is %d. Set ticks to %d and result is %d",
          initial_duration_min,duration_min,ticks,MAX_DURATION);

  remove_and_destroy_time_editor(showed_window,&time_editor);
  
}
END_TEST

/**
 * Purpose: Check that invalid minimum/maximum duration values are set and get properly.
 * Cases considered:
 *    - Set duration_min on NULL object.
 *    - Set and get minimum duration to MAX_DURATION+1 using time editor object with duration mode
 *      setting ticks previously with a value under MAX_DURATION.
 *    - Set and get minimum duration to MAX_DURATION+1 using time editor object with duration mode
 *      setting ticks previously with a value over MAX_DURATION.
 *    - Set duration_max on NULL object.
 *    - Set and get maximum duration to MAX_DURATION+1 using time editor object with duration mode
 *      setting ticks previously with a value under MAX_DURATION.
 *    - Set and get maximum duration to MAX_DURATION+1 using time editor object with duration mode
 *      setting ticks previously with a value over MAX_DURATION.
 */
START_TEST (test_set_duration_min_set_duration_max_invalid)
{
  guint initial_duration_min;
  guint expected_duration_min;
  guint duration_min;
  guint initial_duration_max;
  guint expected_duration_max;
  guint duration_max;
  guint expected_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;


  /* Test 1: Set minimum duration on NULL object */
  hildon_time_editor_set_duration_min(NULL,MIN_DURATION);

  /* Test 2: Set minimum duration to MAX_DURATION+1 seconds to test set_duration_min */
  initial_duration_min=MAX_DURATION+1;
  expected_duration_min=MAX_DURATION+1;
  expected_ticks=MAX_DURATION-1;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_min(time_editor,initial_duration_min);
  duration_min = hildon_time_editor_get_duration_min(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);
    
  fail_if(( expected_duration_min!=duration_min) | (ticks!=MAX_DURATION),
          "hildon-time-editor: set_duration_min or get_duration_min failed with %d. Expected result is %d and result is %d. Set ticks to %d, expected result is %d and result is %d",
          initial_duration_min,duration_min,expected_duration_min,expected_ticks,MAX_DURATION,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 3: Set minimum duration to MAX_DURATION+1 seconds to test set_duration_min */
  initial_duration_min=MAX_DURATION+1;
  expected_duration_min=MAX_DURATION+1;
  expected_ticks=MAX_DURATION+2;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_min(time_editor,initial_duration_min);
  duration_min = hildon_time_editor_get_duration_min(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);
    
  fail_if(( expected_duration_min!=duration_min) | (ticks!=MAX_DURATION),
          "hildon-time-editor: set_duration_min or get_duration_min failed with %d. Expected result is %d and result is %d. Set ticks to %d, expected result is %d and result is %d",
          initial_duration_min,duration_min,expected_duration_min,expected_ticks,MAX_DURATION,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);
 
  /* Test 4: Set maximum duration on NULL object */
  hildon_time_editor_set_duration_max(NULL,MAX_DURATION);

  /* Test 5: Set maximum duration to MAX_DURATION+1 seconds to test set_duration_max settings ticks under MAX_DURATION*/
  initial_duration_max=MAX_DURATION+1;
  expected_duration_max=MAX_DURATION;
  expected_ticks=MAX_DURATION-59;
  
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
  
  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_max(time_editor,initial_duration_max);
  duration_max = hildon_time_editor_get_duration_max(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if(( expected_duration_max!=duration_max) | (ticks!=expected_ticks),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d and result is %d. Set ticks to %d, expected result is %d and result is %d",
          initial_duration_max,duration_max,expected_duration_max,expected_ticks,expected_ticks,ticks);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Initialize values in order to avoid side effects from previous tests */
  initial_duration_max=MAX_DURATION+1;
  expected_duration_max=MAX_DURATION;
  expected_ticks=MAX_DURATION+1;

  /* Test 6: Set maximum duration to MAX_DURATION+1 seconds to test set_duration_max setting ticks over MAX_DURATION */
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_max(time_editor,initial_duration_max);
  duration_max = hildon_time_editor_get_duration_max(time_editor);
  ticks = hildon_time_editor_get_ticks(time_editor);
  fail_if(( expected_duration_max!=duration_max),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d and result is %d. Set ticks to %d, expected result is %d and result is %d",
          initial_duration_max,duration_max,expected_duration_max,expected_ticks,MAX_DURATION,ticks);

  /*  fail_if(( expected_duration_max!=duration_max) | (ticks!=MAX_DURATION),
          "hildon-time-editor: set_duration_max or get_duration_max failed with %d. Expected result is %d and result is %d. Set ticks to %d, expected result is %d and result is %d",
          initial_duration_max,duration_max,expected_duration_max,expected_ticks,MAX_DURATION,ticks);
  */
  remove_and_destroy_time_editor(showed_window,&time_editor);


}
END_TEST

/* ----- Test case for set_duration_range  -----*/

/**
 * Purpose: Check that regular range values are set and get properly
 * Cases considered:
 *    - Set and get duration range (100000,200000) having ticks under 100000 using time editor object with duration mode.
 *    - Set and get duration range (100000,200000) having ticks between 100000 and 200000 using time editor object with duration mode.
 *    - Set and get duration range (100000,200000) having ticks over 200000 using time editor object with duration mode.
 *    - Set and get duration range (100000,100000) having ticks under 100000 using time editor object with duration mode.
 *    - Set and get duration range (100000,100000) having ticks equal to 100000 using time editor object with duration mode.
 *    - Set and get duration range (100000,100000) having ticks over 100000 using time editor object with duration mode.
 *
 */
START_TEST (test_set_duration_range_regular)
{
  guint initial_duration_min;
  guint initial_duration_max;
  guint duration_min;
  guint duration_max;
  guint expected_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;

  /* Test 1: Set duration range to (100000,200000) seconds to test set_duration_range having ticks under 100000 */
  initial_duration_min=100000;
  initial_duration_max=200000;
  expected_ticks=90000;
    
  /* Initialize values in order to avoid side effects from previous tests */
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 2: Set duration range to (100000,200000) seconds to test set_duration_range having ticks between 100000 and 200000 */
  initial_duration_min=100000;
  initial_duration_max=200000;
  expected_ticks=150000;
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
    
  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 3: Set duration range to (100000,200000) seconds to test set_duration_range having ticks over 200000*/
  initial_duration_min=100000;
  initial_duration_max=200000;
  expected_ticks=220000;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
   
  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 4: Set duration range to (100000,100000) seconds to test set_duration_range having ticks under 100000 */
  initial_duration_min=100000;
  initial_duration_max=100000;
  expected_ticks=90000;
    
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 5: Set duration range to (100000,100000) seconds to test set_duration_range having ticks equal to 100000 */
  initial_duration_min=100000;
  initial_duration_max=100000;
  expected_ticks=100000;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 6: Set duration range to (100000,100000) seconds to test set_duration_range having ticks over 100000 */
  initial_duration_min=100000;
  initial_duration_max=100000;
  expected_ticks=120000;
    
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);

    
}
END_TEST

/**
 * Purpose: Check that limits range values are set and get properly
 * Cases considered:
 *    - Set and get duration range to (MIN_DURATION,MAX_DURATION) having ticks between MIN_DURATION and MAX_DURATION
 *      using time editor object with duration mode.
 *    - Set and get duration range to (MIN_DURATION,MIN_DURATION) having ticks over MIN_DURATION
 *      using time editor object with duration mode.
 *    - Set and get duration range to (MAX_DURATION,MAX_DURATION) having ticks under MAX_DURATION
 *      using time editor object with duration mode.
 *
 */
START_TEST (test_set_duration_range_limits)
{
  guint initial_duration_min;
  guint initial_duration_max;
  guint duration_min;
  guint duration_max;
  guint expected_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;

    
  /* Test 1: Set duration range to (MIN_DURATION,MAX_DURATION) seconds to test set_duration_range having ticks under MIN_DURATION */
  initial_duration_min=MIN_DURATION;
  initial_duration_max=MAX_DURATION;
  expected_ticks=MIN_DURATION+1;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
  
  /* Initialize values in order to avoid side effects from previous tests */
  hildon_time_editor_set_duration_min(time_editor,MIN_DURATION);
  hildon_time_editor_set_duration_max(time_editor,MAX_DURATION);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);
  

  /* Test 2: Set duration range to (MIN_DURATION,MIN_DURATION) seconds to test set_duration_range having ticks over MIN_DURATION */
  initial_duration_min=MIN_DURATION;
  initial_duration_max=MIN_DURATION;
  expected_ticks=MIN_DURATION+1;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);
  
  /* Test 3: Set duration range to (MAX_DURATION,MAX_DURATION) seconds to test set_duration_range having ticks under MAX_DURATION */
  initial_duration_min=MAX_DURATION;
  initial_duration_max=MAX_DURATION;
  expected_ticks=MAX_DURATION-1;
    
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_min!=duration_min) | (initial_duration_max!=duration_max) | ( ticks!=initial_duration_min )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,initial_duration_max,duration_min,duration_max,ticks,initial_duration_min);

  remove_and_destroy_time_editor(showed_window,&time_editor);
  
    
}
END_TEST

/**
 * Purpose: Check that invalid range values are set and get properly
 * Cases considered:
 *    - Set duration_range on NULL object.
 *    - Set and get duration range to (MAX_DURATION,MIN_DURATION) using time editor object with duration mode.
 *    - Set and get duration range to (MIN_DURATION,MAX_DURATION+1) having ticks between MIN_DURATION and MAX_DURATION
 *      using time editor object with duration mode.
 *    - Set and get duration range to (MAX_DURATION+1,MIN_DURATION) using time editor object with duration mode.
 *    - Set and get duration range to (MAX_DURATION+1,MAX_DURATION+3)
 *      using time editor object with duration mode.
 *    - Set and get duration range to (MAX_DURATION+3,MAX_DURATION+1)
 *      using time editor object with duration mode.
 *
 */
START_TEST (test_set_duration_range_invalid)
{
  guint initial_duration_min;
  guint initial_duration_max;
  guint duration_min;
  guint duration_max;
  guint expected_ticks;
  guint ticks;
  HildonTimeEditor *time_editor = NULL;

   
  /* Test 1: Set duration range on NULL */
  hildon_time_editor_set_duration_range(NULL,MIN_DURATION,MAX_DURATION);

  /* Test 2: Set duration range to (MAX_DURATION,MIN_DURATION) seconds to test set_duration_range */
  initial_duration_min=MAX_DURATION;
  initial_duration_max=MIN_DURATION;
  expected_ticks=MIN_DURATION+1;
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
  
  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (MIN_DURATION!=duration_min) | (MAX_DURATION!=duration_max) | ( ticks!=MIN_DURATION )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           MIN_DURATION,MAX_DURATION,duration_min,duration_max,ticks,MIN_DURATION);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 3: Set duration range to (MIN_DURATION,MAX_DURATION+1) seconds to test set_duration_range */
  initial_duration_min=MIN_DURATION;
  initial_duration_max=MAX_DURATION+1;
  expected_ticks=MAX_DURATION-59;
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);
    
  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (MIN_DURATION!=duration_min) | (MAX_DURATION!=duration_max) | ( ticks!=MIN_DURATION )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           MIN_DURATION,MAX_DURATION,duration_min,duration_max,ticks,MIN_DURATION);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 4: Set duration range to (MAX_DURATION+1,MIN_DURATION) seconds to test set_duration_range */
  initial_duration_min=MAX_DURATION+1;
  initial_duration_max=MIN_DURATION;
  expected_ticks=MIN_DURATION-1;
  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);

  fail_if( ( (MIN_DURATION!=duration_min) | (MAX_DURATION!=duration_max) ),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). ",
           MIN_DURATION,MAX_DURATION,duration_min,duration_max);

  remove_and_destroy_time_editor(showed_window,&time_editor);


  /* Test 5: Set duration range to (MAX_DURATION+1,MIN_DURATION+3) seconds to test set_duration_range */
  initial_duration_min=MAX_DURATION+1;
  initial_duration_max=MAX_DURATION+3;
  expected_ticks=MAX_DURATION+1;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

/*   fail_if( ( (initial_duration_min!=duration_min) | (MAX_DURATION!=duration_max) | ( ticks!=MAX_DURATION )), */
/*            "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ", */
/*            initial_duration_min,MAX_DURATION,duration_min,duration_max,ticks,MAX_DURATION); */

  fail_if( ( (initial_duration_min!=duration_min) | (MAX_DURATION!=duration_max) ),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_min,MAX_DURATION,duration_min,duration_max,ticks,MAX_DURATION);

  remove_and_destroy_time_editor(showed_window,&time_editor);

  /* Test 6: Set duration range to (MAX_DURATION+3,MIN_DURATION+1) seconds to test set_duration_range */
  initial_duration_min=MAX_DURATION+3;
  initial_duration_max=MAX_DURATION+1;
  expected_ticks=MAX_DURATION+1;

  time_editor = create_and_show_time_editor(showed_window,TRUE,TRUE);

  hildon_time_editor_set_ticks(time_editor,expected_ticks);
  hildon_time_editor_set_duration_range(time_editor,initial_duration_min,initial_duration_max);
  hildon_time_editor_get_duration_range(time_editor,&duration_min,&duration_max);
  ticks = hildon_time_editor_get_ticks(time_editor);

  fail_if( ( (initial_duration_max!=duration_min) | (MAX_DURATION!=duration_max) | ( ticks!=MAX_DURATION )),
           "hildon-time-editor: set_duration_range or get_duration_range failed with (minimum,maximum) as (%d,%d). Expected range is (%d,%d). Set ticks to %d and expected ticks are %d ",
           initial_duration_max,MAX_DURATION,duration_min,duration_max,ticks,MAX_DURATION);

  remove_and_destroy_time_editor(showed_window,&time_editor);

    
}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_time_editor_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonTimeEditor");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_time");
  TCase *tc2 = tcase_create("set_ticks");
  TCase *tc3 = tcase_create("set_duration_min_set_duration_max");
  TCase *tc4 = tcase_create("set_duration_range");

  /* Create test case for hildon_time_editor_set_time and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_time_editor, fx_teardown_default_time_editor);
  tcase_add_test(tc1, test_set_time_regular);
  tcase_add_test(tc1, test_set_time_limits);
  tcase_add_test(tc1, test_set_time_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for hildon_time_editor_set_ticks and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_time_editor, fx_teardown_default_time_editor);
  tcase_add_test(tc2, test_set_ticks_regular);
  tcase_add_test(tc2, test_set_ticks_limits);
  tcase_add_test(tc2, test_set_ticks_invalid);
  suite_add_tcase (s, tc2);

  /* Create test case for hildon_time_editor_set_duration_max and add it to the suite */
  tcase_add_checked_fixture(tc3, fx_setup_default_time_editor, fx_teardown_default_time_editor);
  tcase_add_test(tc3, test_set_duration_min_set_duration_max_regular);
  tcase_add_test(tc3, test_set_duration_min_set_duration_max_limits);
  tcase_add_test(tc3, test_set_duration_min_set_duration_max_invalid);
  suite_add_tcase (s, tc3);

  /* Create test case for hildon_time_editor_set_duration_range and add it to the suite */
  tcase_add_checked_fixture(tc4, fx_setup_default_time_editor, fx_teardown_default_time_editor);
  tcase_add_test(tc4, test_set_duration_range_regular);
  tcase_add_test(tc4, test_set_duration_range_limits);
  tcase_add_test(tc4, test_set_duration_range_invalid);
  suite_add_tcase (s, tc4);

  /* Return created suite */
  return s;             
}
