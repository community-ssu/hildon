/* -*- mode: C; c-file-style: "stroustrup"; indent-tabs-mode: nil; -*- */
/* Copyright (c) 2003 Nokia Oyj*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <gtk/gtk.h>

#include <hildon-widgets/hildon-date-editor.h>
#include <hildon-widgets/hildon-time-editor.h>
#include <hildon-widgets/hildon-calendar-popup.h>

#include <outo.h>

/* Prototypes */
void init_test (void);
testcase* get_tests (void);

void init_test (void)
{
    int plop = 0;
    gtk_init (&plop, NULL);
}

/* time editor: new time: ok parameters */
static int test01a()
{
  HildonTimeEditor *time_editor;

  
  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());
  assert(time_editor);
  return 1;
}

/* time editor: get time: ok parameters */
static int test02a()
{
  HildonTimeEditor *time_editor;
  guint hours = 99, minutes = 99, seconds = 99;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());
  hildon_time_editor_get_time( time_editor, &hours, &minutes, &seconds );

  assert (hours   >= 0 && hours   < 24);
  assert (minutes >= 0 && minutes < 60);
  assert (seconds >= 0 && seconds < 60);

  return 1;
}

/* time editor: get time: null time editor */
static int test02b()
{
  HildonTimeEditor *time_editor = NULL;
  guint hours = 99, minutes = 99, seconds = 99;

  

  hildon_time_editor_get_time( time_editor, &hours, &minutes, &seconds );

  assert (hours   >= 0 && hours   < 24);
  assert (minutes >= 0 && minutes < 60);
  assert (seconds >= 0 && seconds < 60);

  return 1;
}

/* time editor: get time: null minutes */
static int test02c()
{
  HildonTimeEditor *time_editor = NULL;
  guint hours = 99, seconds = 99;

  

  hildon_time_editor_get_time (time_editor, &hours, NULL, &seconds);

  return 1;
}

/* time editor: get time: null hours */
static int test02d()
{
  HildonTimeEditor *time_editor = NULL;
  guint minutes = 99, seconds = 99;

  

  hildon_time_editor_get_time (time_editor, NULL, &minutes, &seconds);

  return 1;
}

/* time editor: get time: null hours, null minutes, null seconds */
static int test02e()
{
  HildonTimeEditor *time_editor = NULL;

  

  hildon_time_editor_get_time (time_editor, NULL, NULL, NULL);

  return 1;
}

/* time editor: get time: null seconds */
static int test02f()
{
  HildonTimeEditor *time_editor = NULL;
  guint hours = 99, minutes = 99;;

  init_test();

  hildon_time_editor_get_time (time_editor, &hours, &minutes, NULL);

  return 1;
}

/* time editor: set time: hours, minutes, seconds are the minimum */
static int test03a()
{
  HildonTimeEditor *time_editor;
  guint hours = 0, minutes = 0, seconds = 0;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 0);
  assert (minutes == 0);
  assert (seconds == 0);

  return 1;
}

/* time editor: set time: ok parameters */
static int test03b()
{
  HildonTimeEditor *time_editor;
  guint hours = 12, minutes = 30, seconds = 30;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 12);
  assert (minutes == 30);
  assert (seconds == 30);

  return 1;
}

/* time editor: set time: hours on limit */
static int test03c()
{
  HildonTimeEditor *time_editor;
  guint hours = 24, minutes = 30, seconds = 0;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 24);
  assert (minutes == 30);
  assert (seconds == 0);


  return 1;
}

/* time editor: set time: minutes on limit */
static int test03d()
{
  HildonTimeEditor *time_editor;
  guint hours = 12, minutes = 60, seconds = 0;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 12);
  assert (minutes == 60);
  assert (seconds == 0);


  return 1;
}

/* time editor: set time: seconds on limit */
static int test03da()
{
  HildonTimeEditor *time_editor;
  guint hours = 12, minutes = 30, seconds = 60;

  init_test();

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 12);
  assert (minutes == 30);
  assert (seconds == 60);

  return 1;
}


/* time editor: set time: hours, minutes on limit */
static int test03e()
{
  HildonTimeEditor *time_editor;
  guint hours = 24, minutes = 60, seconds = 0;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 24);
  assert (minutes == 60);
  assert (seconds == 0);

  return 1;
}

/* time editor: set time: hours, minutes, second on limit */
static int test03ea()
{
  HildonTimeEditor *time_editor;
  guint hours = 24, minutes = 60, seconds = 60;

  init_test();

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert( hours   == 24 );
  assert( minutes == 60 );
  assert( seconds == 60 );

  return 1;
}

/* time editor: set time: hours, minutes over max */
static int test03f()
{
  HildonTimeEditor *time_editor;
  guint hours = 25, minutes = 61, seconds = 0;

  

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 25);
  assert (minutes == 61);
  assert (seconds == 0);

  return 1;
}

/* time editor: set time: hours, minutes, seconds over max */
static int test03fa()
{
  HildonTimeEditor *time_editor;
  guint hours = 25, minutes = 61, seconds = 61;

  init_test();

  time_editor = HILDON_TIME_EDITOR(hildon_time_editor_new());

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);
  hildon_time_editor_get_time (time_editor, &hours, &minutes, &seconds);

  assert (hours   == 25);
  assert (minutes == 61);
  assert (seconds == 61);

  return 1;
}

/* time editor: set time: null time editor*/
static int test03g()
{
  HildonTimeEditor *time_editor = NULL;
  guint hours = 23, minutes = 59, seconds = 0;

  init_test();

  hildon_time_editor_set_time (time_editor, hours, minutes, seconds);

  return 1;
}

/* time editor: show seconds: ok parameters */
static int test03h()
{
  HildonTimeEditor *time_editor = NULL;

  init_test();

  time_editor = HILDON_TIME_EDITOR( hildon_time_editor_new () );
  hildon_time_editor_show_seconds( time_editor, TRUE );

  return 1;
}

/* time editor: show seconds: null time editor*/
static int test03i()
{
  HildonTimeEditor *time_editor = NULL;

  

  hildon_time_editor_show_seconds( time_editor, TRUE );

  return 1;
}

/* time editor: get type: */
static int test04a()
{
  GType t;

  
  
  t = hildon_time_editor_get_type();

  assert (t);

  return 1;
}

/* date editor: new: ok parameters */
static int test05a()
{
  HildonDateEditor *date_editor;

  
  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());
  assert (date_editor);
  return 1;
}

/* date editor: get date: ok parameters */
static int test06a()
{
  HildonDateEditor *date_editor;
  guint year = 99999, month = 99, day = 999;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  > 0);
  assert (month > 0 && month < 13);
  assert (day   > 0 && day   < 32);

  return 1;
}

/* date editor: get date: null date editor */
static int test06b()
{
  HildonDateEditor *date_editor = NULL;
  guint year = 99999, month = 99, day = 999;

  

  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  > 0);
  assert (month > 0 && month < 13);
  assert (day   > 0 && day   < 32);

  return 1;
}

/* date editor: get date: null year */
static int test06c()
{
  HildonDateEditor *date_editor;
  guint month = 99, day = 999;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_get_date (date_editor, NULL, &month, &day);

  return 1;
}

/* date editor: get date: null month */
static int test06d()
{
  HildonDateEditor *date_editor;
  guint  month = 99, day = 999;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_get_date (date_editor, &month, NULL, &day);

  return 1;
}

/* date editor: get date: null day */
static int test06e()
{
  HildonDateEditor *date_editor;
  guint year = 99999, month = 99;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_get_date (date_editor, &year, &month, NULL);

  return 1;
}

/* date editor: get date: null year, month, day */
static int test06f()
{
  HildonDateEditor *date_editor;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_get_date (date_editor, NULL, NULL, NULL);

  return 1;
}

/* date editor: set date: year, month, day are zero */
static int test07a()
{
  HildonDateEditor *date_editor;
  guint year = 0, month = 0, day = 0;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 0);
  assert (month == 0);
  assert (day   == 0);

  return 1;
}

/* date editor: set date: year, month, day are the minumum */
static int test07ai()
{
  HildonDateEditor *date_editor;
  guint year = 1, month = 1, day = 1;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 1);
  assert (month == 1);
  assert (day   == 1);

  return 1;
}

/* date editor: set date: month less than minimum */
static int test07b()
{
  HildonDateEditor *date_editor;
  guint year = 2003, month = 0, day = 2;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 2003);
  assert (month == 0);
  assert (day   == 2);

  return 1;
}

/* date editor: set date: day less than minimum */
static int test07c()
{
  HildonDateEditor *date_editor;
  guint year = 2003, month = 12, day = 0;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 2003);
  assert (month == 12);
  assert (day   == 0);

  return 1;
}

/* date editor: set date: ok parameters */
static int test07d()
{
  HildonDateEditor *date_editor;
  guint year = 2003, month = 12, day = 2;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 2003);
  assert (month == 12);
  assert (day   == 2);

  return 1;
}

/* date editor: set date: big year, should be ok */
static int test07e()
{
  HildonDateEditor *date_editor;
  /* hmm.. year seems to fail between 2020 and 2030 */
  guint year = 2500, month = 12, day = 2;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 2500);
  assert (month == 12);
  assert (day   == 2);

  return 1;
}

/* date editor: set date: month over max */
static int test07f()
{
  HildonDateEditor *date_editor;
  guint year = 2003, month = 13, day = 2;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 2003);
  assert (month == 13);
  assert (day   == 2);

  return 1;
}

/* date editor: set date: day over max */
static int test07g()
{
  HildonDateEditor *date_editor;
  guint year = 2003, month = 11, day = 31;

  

  date_editor = HILDON_DATE_EDITOR(hildon_date_editor_new());

  hildon_date_editor_set_date (date_editor, year, month, day);
  hildon_date_editor_get_date (date_editor, &year, &month, &day);

  assert (year  == 2003);
  assert (month == 11);
  assert (day   == 31);

  return 1;
}

/* date editor: set date: null date editor */
static int test07h()
{
  

  hildon_date_editor_set_date (NULL, 0, 0, 0);

  return 1;
}


testcase tcases[] =
{
    {*test01a, " 01a_time_editor_new",                EXPECT_OK},
    {*test02a, " 02a_time_editor_get_time",           EXPECT_OK},
    {*test02b, " 02b_time_editor_get_time_null",      EXPECT_ASSERT},
    {*test02c, " 02c_time_editor_get_time_null2",     EXPECT_ASSERT},
    {*test02d, " 02d_time_editor_get_time_null3",     EXPECT_ASSERT},
    {*test02e, " 02e_time_editor_get_time_null4",     EXPECT_ASSERT},
    {*test02f, " 02e_time_editor_get_time_null5",     EXPECT_ASSERT},
    {*test03a, " 03a_time_editor_set_time_both_0",    EXPECT_OK},
    {*test03b, " 03b_..._both_between",               EXPECT_OK},
    {*test03c, " 03c_..._hours_limit",                EXPECT_ASSERT},
    {*test03d, " 03d_..._minutes_limit",              EXPECT_ASSERT},
    {*test03da," 03d_..._seconds_limit",              EXPECT_ASSERT},
    {*test03e, " 03e_..._both_limit",                 EXPECT_ASSERT},
    {*test03ea," 03e_..._all_limit",                  EXPECT_ASSERT},
    {*test03f, " 03f_..._both_over",                  EXPECT_ASSERT},
    {*test03fa," 03f_..._all_over",                   EXPECT_ASSERT},
    {*test03g, " 03g_..._null",                       EXPECT_ASSERT},
    {*test03h, " 03h_show_seconds_ok",                EXPECT_OK},
    {*test03i, " 03i_show_seconds_null",              EXPECT_ASSERT},
    {*test04a, " 04a_time_editor_get_type",           EXPECT_OK},
    {*test05a, " 05a_date_editor_new",                EXPECT_OK},
    {*test06a, " 06a_date_editor_get_date",           EXPECT_OK},
    {*test06b, " 06b_date_editor_get_date_null",      EXPECT_ASSERT},
    {*test06c, " 06c_date_editor_get_date_null2",     EXPECT_ASSERT},
    {*test06d, " 06d_date_editor_get_date_null3",     EXPECT_ASSERT},
    {*test06e, " 06e_date_editor_get_date_null4",     EXPECT_ASSERT},
    {*test06f, " 06f_date_editor_get_date_null5",     EXPECT_ASSERT},
    {*test07a, " 07a_date_editor_set_date_valid1",    EXPECT_ASSERT},
    {*test07ai," 07ai_date_editor_set_date_valid1a",  EXPECT_OK},
    {*test07b, " 07b_date_editor_set_date_valid2",    EXPECT_ASSERT},
    {*test07c, " 07c_date_editor_set_date_valid3",    EXPECT_ASSERT},
    {*test07d, " 07d_date_editor_set_date_valid4",    EXPECT_OK},
    {*test07e, " 07e_date_editor_set_date_valid5",    EXPECT_OK},
    {*test07f, " 07f_date_editor_set_date_valid6",    EXPECT_ASSERT},
    {*test07g, " 07g_date_editor_set_date_valid7",    EXPECT_ASSERT},
    {*test07h, " 07h_date_editor_set_date_null",      EXPECT_ASSERT},
    
    {0} /*REMEMBER THE TERMINATING NULL*/
};

/*use EXPECT_ASSERT for the tests that are _ment_ to throw assert so they are 
  consider passed when they throw assert and failed when tehy do not*/

testcase* get_tests (void)
{
    g_log_set_always_fatal (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);
    return tcases;
}
