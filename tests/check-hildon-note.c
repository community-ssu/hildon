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
#include <string.h>
#include <check.h>
#include <gtk/gtkmain.h>
#include "test_suites.h"
#include "hildon-window.h"
#include "hildon-note.h"

/* -------------------- Fixtures -------------------- */

static HildonNote *note = NULL;
static GtkWindow * n_window = NULL;

static void 
fx_setup_default_note ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  n_window = GTK_WINDOW(hildon_window_new());
 /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(n_window),
          "hildon-note: Window creation failed.");
}

static void 
fx_teardown_default_note ()
{
    gtk_widget_destroy (GTK_WIDGET (n_window));
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for new_confirmation -----*/
/**
 * Purpose: Check that note dialog is properly created with description regular values. 
 * Cases considered:
 *    - Create new confirmation note with description set to TEST_STRING
 *    - Create new confirmation note with description set to "".
 *
 */
START_TEST (test_new_confirmation_regular)
{
  const gchar * description = NULL;
  const gchar * ret_description = NULL;
  GValue value={0, };
  GValue enum_value={0, };
  HildonNoteType note_type;

  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new confirmation note with description set to TEST_STRING */
  description = TEST_STRING;
  note = HILDON_NOTE(hildon_note_new_confirmation(n_window,description));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_confirmation");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);
    
  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_if( note_type != HILDON_NOTE_CONFIRMATION_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_CONFIRMATION_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;

  /* Test 2: create new confirmation note with description set to "" */
  description = "";
  note = HILDON_NOTE(hildon_note_new_confirmation(n_window,description));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_confirmation");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);

  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_unless( note_type == HILDON_NOTE_CONFIRMATION_TYPE,
               "hildon-note: Type was not set property on creation (HILDON_NOTE_CONFIRMATION_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;


  g_value_unset(&value);
  g_value_unset(&enum_value);    
}
END_TEST

/**
 * Purpose: Check that note dialog is properly created with description invalid values. 
 * Cases considered:
 *    - Create new confirmation note with window set to NULL.
 *    - Create new confirmation note with description set to "NULL".
 *
 */
START_TEST (test_new_confirmation_invalid)
{
  const gchar * ret_description = NULL;
  GValue value={0, };
  GValue enum_value={0, };
  HildonNoteType note_type;
  HildonNote * invalid_note;
   
  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new confirmation note with window set to "NULL" */
  invalid_note = HILDON_NOTE(hildon_note_new_confirmation(NULL,""));
  fail_if(!HILDON_IS_NOTE(invalid_note),
          "hildon-note: Creation failed with hildon_note_new_confirmation");

  g_object_get_property(G_OBJECT (invalid_note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp ("",ret_description) != 0,
	   "hildon-note: Empty description was not set properly on creation. Returned description: %s",
	   ret_description);

  g_object_get_property(G_OBJECT (invalid_note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_if( note_type != HILDON_NOTE_CONFIRMATION_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_CONFIRMATION_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (invalid_note));
  invalid_note=NULL;

  /* Test 2: create new confirmation note with description set to "NULL" */
  invalid_note = HILDON_NOTE(hildon_note_new_confirmation(n_window,NULL));
  fail_if(HILDON_IS_NOTE(invalid_note),
          "hildon-note: Creation succeeded with hildon_note_new_confirmation with NULL description");

  g_value_unset(&value);
  g_value_unset(&enum_value);
}
END_TEST

/* ----- Test case for new_information -----*/
/**
 * Purpose: Check that note dialog is properly created with description regular values. 
 * Cases considered:
 *    - Create new information note with description set to TEST_STRING.
 *    - Create new information note with description set to "".
 *
 */
START_TEST (test_new_information_regular)
{
  const gchar * description = NULL;
  const gchar * ret_description = NULL;
  GValue value={0, };
  GValue enum_value={0, };
  HildonNoteType note_type;

  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new information note with description set to "Standard question?" */
  description = TEST_STRING;
  note = HILDON_NOTE(hildon_note_new_information(n_window,description));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_information");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);

  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);   
  fail_if( note_type != HILDON_NOTE_INFORMATION_THEME_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_INFORMATION_THEME_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;

  /* Test 2: create new information note with description set to "" */
  description = "";
  note = HILDON_NOTE(hildon_note_new_information(n_window,description));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_information");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);

  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_if( note_type != HILDON_NOTE_INFORMATION_THEME_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_INFORMATION_THEME_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;

  g_value_unset(&value);
  g_value_unset(&enum_value);       
}
END_TEST

  
/**
 * Purpose: Check that note dialog is properly created with description invalid values. 
 * Cases considered:
 *    - Create new information note with window set to NULL.
 *    - Create new information note with description set to "NULL".
 *
 */
START_TEST (test_new_information_invalid)
{
  const gchar * ret_description = NULL;
  GValue value={0, };
  GValue enum_value={0, };
  HildonNoteType note_type;
  HildonNote * invalid_note;
   
  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new information note with window set to "NULL" */
  invalid_note = HILDON_NOTE(hildon_note_new_information(NULL,""));
  fail_if(!HILDON_IS_NOTE(invalid_note),
          "hildon-note: Creation failed with hildon_note_new_information");

  g_object_get_property(G_OBJECT (invalid_note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp ("",ret_description) != 0,
	   "hildon-note: Empty description was not set properly on creation. Returned description: %s",
	   ret_description);

  g_object_get_property(G_OBJECT (invalid_note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_if( note_type != HILDON_NOTE_INFORMATION_THEME_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_INFORMATION_THEME_TYPE)",note_type);
    
  gtk_widget_destroy (GTK_WIDGET (invalid_note));
  invalid_note=NULL;

  /* Test 2: create new information note with description set to "NULL" */
  invalid_note = HILDON_NOTE(hildon_note_new_information(n_window,NULL));
  fail_if(HILDON_IS_NOTE(invalid_note),
          "hildon-note: Creation succeeded with hildon_note_new_information where msg == NULL");

  g_value_unset(&value);
  g_value_unset(&enum_value);
}
END_TEST

/* ----- Test case for new_confirmation_with_icon_name -----*/
/**
 * Purpose: Check that note dialog is properly created with description regular values. 
 * Cases considered:
 *    - Create new confirmation note with description set to TEST_STRING and icon name "qgn_list_cp_calibration".
 *    - Create new confirmation note with description set to "" and icon name NULL.
 *
 */
START_TEST (test_new_confirmation_with_icon_name_regular)
{
  const gchar * description = NULL;
  const gchar * ret_description = NULL;
  const gchar *icon_name = NULL;
  const gchar * ret_icon_name = NULL;
  GValue value={0, };
  GValue icon_name_value={0, };
  GValue enum_value={0, };
  HildonNoteType note_type;

  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&icon_name_value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new confirmation note with description set to TEST_STRING */
  description = TEST_STRING;
  icon_name="qgn_list_cp_calibration";
  note = HILDON_NOTE(hildon_note_new_confirmation_with_icon_name(n_window,description,icon_name));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_confirmation_with_icon_name");

    g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);
  
  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);  
  fail_if( note_type != HILDON_NOTE_CONFIRMATION_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_CONFIRMATION_TYPE)",note_type);
  
  g_object_get_property(G_OBJECT (note),"icon",&icon_name_value);
  ret_icon_name = g_value_get_string (&icon_name_value);
  fail_if( strcmp (icon_name,ret_icon_name) != 0,
	   "hildon-note: icon_name (%s) was not set properly on creation. Returned icon_name: %s",
	   icon_name,ret_icon_name);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;
  
  /* Test 2: create new confirmation note with description set to "" and icon name set to NULL */
  description = "";
  icon_name=NULL;
  note = HILDON_NOTE(hildon_note_new_confirmation_with_icon_name(n_window,description,icon_name));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_confirmation_with_icon_name");
  
  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);
  
  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);  
  fail_if( note_type != HILDON_NOTE_CONFIRMATION_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_CONFIRMATION_TYPE)",note_type);
  
  g_object_get_property(G_OBJECT (note),"icon",&icon_name_value);
  ret_icon_name = g_value_get_string (&icon_name_value);
  fail_if( ret_icon_name != NULL,
	   "hildon-note: icon_name (%s) was not set properly on creation. Returned icon_name: %s",
	   icon_name,ret_icon_name);
  
  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;
  
  g_value_unset(&value);
  g_value_unset(&icon_name_value);
  g_value_unset(&enum_value);
  
}
END_TEST

/**
 * Purpose: Check that note dialog is properly created with description invalid values. 
 * Cases considered:
 *    - Create new confirmation note with window set to NULL.
 *    - Create new confirmation note with description set to "NULL".
 *
 */
START_TEST (test_new_confirmation_with_icon_name_invalid)
{
  const gchar * ret_description = NULL;
  const gchar * ret_icon_name = NULL;
  GValue value={0, };
  GValue enum_value={0, };
  GValue icon_name_value={0, };
  HildonNoteType note_type;
  HildonNote * invalid_note;
   
  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&icon_name_value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new confirmation note with window set to "NULL" */
  invalid_note = HILDON_NOTE(hildon_note_new_confirmation_with_icon_name(NULL,"",""));
  fail_if(!HILDON_IS_NOTE(invalid_note),
          "hildon-note: Creation failed with hildon_note_new_confirmation_with_icon_name");

  g_object_get_property(G_OBJECT (invalid_note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp ("",ret_description) != 0,
	   "hildon-note: Description "" was not set properly on creation. Returned description: %s",
	   ret_description);

  g_object_get_property(G_OBJECT (invalid_note),"icon",&icon_name_value);
  ret_icon_name = g_value_get_string (&icon_name_value);
  fail_if( strcmp ("",ret_icon_name) != 0,
	   "hildon-note: Description "" was not set properly on creation. Returned description: %s",
	   ret_icon_name);

  g_object_get_property(G_OBJECT (invalid_note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_if( note_type != HILDON_NOTE_CONFIRMATION_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_CONFIRMATION_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (invalid_note));
  invalid_note=NULL;

  /* Test 2: create new confirmation note with description set to "NULL" */
  invalid_note = HILDON_NOTE(hildon_note_new_confirmation_with_icon_name(n_window,NULL,"qgn_list_cp_calibration"));
  fail_if(HILDON_IS_NOTE(invalid_note),
          "hildon-note: Creation succeeded with hildon_note_new_confirmation_with_icon_name with message == NULL");

  g_value_unset(&icon_name_value);
  g_value_unset(&value);
  g_value_unset(&enum_value);
}
END_TEST

/* ----- Test case for new_cancel_with_progress_bar -----*/

/**
 * Purpose: Check that note dialog is properly created with description regular values. 
 * Cases considered:
 *    - Create new confirmation note with description set to TEST_STRING and NULL GtkProgressBar.
 *    - Create new confirmation note with description set to "" and correct GtkProgressBar.
 *
 */
START_TEST (test_new_cancel_with_progress_bar_regular)
{
  const gchar * description = NULL;
  const gchar * ret_description = NULL;
  GValue value={0, };
  GValue enum_value={0, };
  GtkProgressBar * progress_bar=NULL;
   
  HildonNoteType note_type;

  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new confirmation note with description set to TEST_STRING and NULL GtkProgressBar */
  description = TEST_STRING;
  note = HILDON_NOTE(hildon_note_new_cancel_with_progress_bar(n_window,description,progress_bar));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_cancel_with_progress_bar");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);

  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);   
  fail_if( note_type != HILDON_NOTE_PROGRESSBAR_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_PROGRESSBAR_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;

  /* Test 2: create new confirmation note with description set to "" */
  description = "";
  progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
  fail_if(!GTK_IS_PROGRESS_BAR(progress_bar),
          "hildon-note: Progress bar creation failed in hildon_note_new_cancel_with_progress_bar");

  gtk_progress_bar_set_fraction(progress_bar,0.5);
    
  note = HILDON_NOTE(hildon_note_new_cancel_with_progress_bar(n_window,description,progress_bar));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_cancel_with_progress_bar");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp (description,ret_description) != 0,
	   "hildon-note: Description (%s) was not set properly on creation. Returned description: %s",
	   description,ret_description);

  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);   
  fail_if( note_type != HILDON_NOTE_PROGRESSBAR_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_PROGRESSBAR_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (progress_bar));
  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;

  g_value_unset(&value);
  g_value_unset(&enum_value);
}
END_TEST

/**
 * Purpose: Check that note dialog is properly created with description invalid values. 
 * Cases considered:
 *    - Create new confirmation note with description set to NULL.
 *    - Create new confirmation note with window set to NULL.
 *
 */
START_TEST (test_new_cancel_with_progress_bar_invalid)
{
  const gchar * description = NULL;
  const gchar * ret_description = NULL;
  GValue value={0, };
  GValue enum_value={0, };
   
  HildonNoteType note_type;

  g_value_init (&value, G_TYPE_STRING);
  g_value_init (&enum_value, G_TYPE_INT);

  /* Test 1: create new confirmation note with description set to NULL */
  description = NULL;
  note = HILDON_NOTE(hildon_note_new_cancel_with_progress_bar(n_window,description,NULL));
  fail_if(HILDON_IS_NOTE(note),
          "hildon-note: Creation succeeded with hildon_note_new_cancel_with_progress_bar where msg == NULL");

  /* Test 2: create new confirmation note with window set to NULL */
  description = "";
  note = HILDON_NOTE(hildon_note_new_cancel_with_progress_bar(NULL,description,NULL));
  fail_if(!HILDON_IS_NOTE(note),
          "hildon-note: Creation failed with hildon_note_new_cancel_with_progress_bar");

  g_object_get_property(G_OBJECT (note),"description",&value);
  ret_description = g_value_get_string (&value);
  fail_if( strcmp(description,ret_description) != 0,
	   "hildon-note: Empty description was not set properly on creation",
	   description,ret_description);

  g_object_get_property(G_OBJECT (note),"note_type",&enum_value);
  note_type = g_value_get_int(&enum_value);
  fail_if( note_type != HILDON_NOTE_PROGRESSBAR_TYPE,
	   "hildon-note: Type was not set property on creation (HILDON_NOTE_PROGRESSBAR_TYPE)",note_type);

  gtk_widget_destroy (GTK_WIDGET (note));
  note=NULL;

  g_value_unset(&value);
  g_value_unset(&enum_value);
}
END_TEST

  
  
/* ---------- Suite creation ---------- */
Suite *create_hildon_note_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonNote");

  /* Create test cases */
  TCase *tc1 = tcase_create("new_confirmation");
  TCase *tc2 = tcase_create("new_confirmation_with_icon_name");
  TCase *tc3 = tcase_create("new_information");
  TCase *tc4 = tcase_create("new_cancel_with_progress_bar");

  /* Create test case for hildon_note_new_confirmation and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_note, fx_teardown_default_note);
  tcase_add_test(tc1, test_new_confirmation_regular);
  tcase_add_test(tc1, test_new_confirmation_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for hildon_note_new_confirmation_with_icon_name and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_note, fx_teardown_default_note);
  tcase_add_test(tc2, test_new_confirmation_with_icon_name_regular);
  tcase_add_test(tc2, test_new_confirmation_with_icon_name_invalid);
  suite_add_tcase (s, tc2);

  /* Create test case for hildon_note_new_with_information and add it to the suite */
  tcase_add_checked_fixture(tc3, fx_setup_default_note, fx_teardown_default_note);
  tcase_add_test(tc3, test_new_information_regular);
  tcase_add_test(tc3, test_new_information_invalid);
  suite_add_tcase (s, tc3);

  /* Create test case for hildon_note_new_with_progress_bar and add it to the suite */
  tcase_add_checked_fixture(tc4, fx_setup_default_note, fx_teardown_default_note);
  tcase_add_test(tc4, test_new_cancel_with_progress_bar_regular);
  tcase_add_test(tc4, test_new_cancel_with_progress_bar_invalid);
  suite_add_tcase (s, tc4);

  /* Return created suite */
  return s;
}
