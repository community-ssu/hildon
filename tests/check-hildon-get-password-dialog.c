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
#include <string.h>
#include <check.h>
#include <gtk/gtkmain.h>
#include "test_suites.h"
#include "check_utils.h"
#include <hildon/hildon-get-password-dialog.h>

/* -------------------- Fixtures -------------------- */

static HildonGetPasswordDialog *get_password_dialog = NULL;
static GtkWindow * gpd_window = NULL;

static void 
fx_setup_default_get_password_dialog ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  gpd_window = GTK_WINDOW(create_test_window());

  /* Check the auxiliary window is being created properly */
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");

  get_password_dialog = HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new(gpd_window, TRUE));
  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");

  show_test_window(GTK_WIDGET(gpd_window));

  show_test_window(GTK_WIDGET(get_password_dialog));

}

static void 
fx_teardown_default_get_password_dialog ()
{
  gtk_widget_destroy(GTK_WIDGET(get_password_dialog));

  gtk_widget_destroy(GTK_WIDGET(gpd_window));
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for hildon_get_password_dialog_new_with_default -----*/

/**
 * Purpose: Check that regular password values are set and get properly.
 * Cases considered:
 *    - Create new get password dialog with "test_password" password by default.
 *    - Create new get password dialog with "" password by default.
 *    - Create new get password dialog with "1234567890" password by default.
 *    - Create new get password dialog with TEST_STRING password by default
 */
START_TEST (test_new_with_default_regular)
{
  const gchar * default_password=NULL;
  const gchar * ret_password;
  HildonGetPasswordDialog * default_get_password_dialog;

  /* Test 1: Create new dialog with password "test_password" by default */
  default_password="test_password";
  default_get_password_dialog = 
    HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new_with_default(gpd_window,default_password,FALSE));
  fail_if (!HILDON_IS_GET_PASSWORD_DIALOG(default_get_password_dialog),
	  "hildon-get-password-dialog: Creation failed with hildon_get_password_dialog_new_with_default");

  ret_password = hildon_get_password_dialog_get_password(default_get_password_dialog);  
  fail_if (strcmp (default_password,ret_password) != 0,
	  "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);
  gtk_widget_destroy(GTK_WIDGET(default_get_password_dialog));

  /* Test 2: Create new dialog with password "" by default */
  default_password="";
  default_get_password_dialog = 
    HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new_with_default(gpd_window,default_password,FALSE));
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG(default_get_password_dialog),
	  "hildon-get-password-dialog: Creation failed with hildon_get_password_dialog_new_with_default");

  ret_password = hildon_get_password_dialog_get_password(default_get_password_dialog);   
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy(GTK_WIDGET(default_get_password_dialog));

  /* Test 3: Create new dialog with password "1234567890" by default */
  default_password="1234567890";
    
  default_get_password_dialog = 
    HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new_with_default(gpd_window,default_password,FALSE));
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG(default_get_password_dialog),
	  "hildon-get-password-dialog: Creation failed with hildon_get_password_dialog_new_with_default");

  ret_password = hildon_get_password_dialog_get_password(default_get_password_dialog);   
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy(GTK_WIDGET(default_get_password_dialog));

  /* Test 4: Create new dialog with password TEST_STRING by default */
  default_password=TEST_STRING;
    
  default_get_password_dialog = 
    HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new_with_default(gpd_window,default_password,FALSE));
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG(default_get_password_dialog),
	  "hildon-get-password-dialog: Creation failed with hildon_get_password_dialog_new_with_default");

  ret_password = hildon_get_password_dialog_get_password(default_get_password_dialog);  
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy(GTK_WIDGET(default_get_password_dialog));
}
END_TEST

/**
 * Purpose: Check that invalid password values are set and get properly.
 * Cases considered:
 *    - Create new NULL get password dialog.
 *    - Create new dialog with NULL window by default
 */
START_TEST (test_new_with_default_invalid)
{
  const gchar * default_password="";
  const gchar * ret_password;
  HildonGetPasswordDialog * default_get_password_dialog;

  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");

  /* Test 1: Create new dialog with NULL password by default */
  default_password=NULL;
  default_get_password_dialog = 
    HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new_with_default(gpd_window,default_password,FALSE));
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG(default_get_password_dialog),
	  "hildon-get-password-dialog: Creation failed with hildon_get_password_dialog_new_with_default");

  ret_password = hildon_get_password_dialog_get_password(default_get_password_dialog);  
  fail_if(strcmp ("",ret_password) != 0,
	  "hildon-get-password-dialog: Error setting null password. Returned password is \"%s\"",ret_password);
  
  gtk_widget_destroy(GTK_WIDGET(default_get_password_dialog));

  /* Test 2: Create new dialog with NULL window by default */
  default_get_password_dialog = 
    HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new_with_default(NULL,default_password,FALSE));
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG(default_get_password_dialog),
	  "hildon-get-password-dialog: Creation failed with hildon_get_password_dialog_new_with_default");

  ret_password = hildon_get_password_dialog_get_password(default_get_password_dialog);
  fail_if (ret_password == NULL,
	      "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy(GTK_WIDGET(default_get_password_dialog));
}
END_TEST

/**
 * Purpose: Check that regular "max characters" values are set and get properly.
 * Cases considered:
 *    - Set max characters to 10.
 *    - Set max characters to G_MAXUINT16+1.
 *
 */
START_TEST (test_set_max_characters_regular)
{
  gint max_chars;
  gint ret_max_chars;
  GValue value={0, };

  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");    

  g_value_init (&value, G_TYPE_INT);

  /* Test 1: Set max characters to 10 */
  max_chars = 10;
  hildon_get_password_dialog_set_max_characters(get_password_dialog,max_chars);
  g_object_get_property(G_OBJECT (get_password_dialog),"max-characters",&value);
  ret_max_chars = g_value_get_int (&value);

  fail_if(ret_max_chars!=max_chars,
	  "hildon-get-password-dialog: Set %i max chars incorrect. Return value is %i",max_chars,ret_max_chars);

  g_value_unset (&value);

  /* Test 2: Set max characters to G_MAXUINT16+1 */
  g_value_init (&value, G_TYPE_INT);
  max_chars = G_MAXUINT16+1;
  hildon_get_password_dialog_set_max_characters(get_password_dialog,max_chars);
  g_object_get_property(G_OBJECT (get_password_dialog),"max-characters",&value);
  ret_max_chars = g_value_get_int (&value);

  fail_if(ret_max_chars!=G_MAXUINT16,
	  "hildon-get-password-dialog: Set %i max chars incorrect. Return value is %i",max_chars,ret_max_chars);

  g_value_unset (&value);
}
END_TEST

/**
 * Purpose: Check that limits "max characters" values are set and get properly.
 * Cases considered:
 *    - Set max characters to 1.
 *    - Set max characters to G_MAXINT.
 */
START_TEST (test_set_max_characters_limits)
{
  gint max_chars;
  gint ret_max_chars;
  GValue value={0, };

  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");
    
  g_value_init (&value, G_TYPE_INT);

  /* Test 1: Set max characters to 1 */
  max_chars = 1;
  hildon_get_password_dialog_set_max_characters(get_password_dialog,max_chars);
  g_object_get_property(G_OBJECT (get_password_dialog),"max-characters",&value);
  ret_max_chars = g_value_get_int (&value);

  fail_if(ret_max_chars!=max_chars,
	  "hildon-get-password-dialog: Set %i max chars incorrect. Return value is %i",max_chars,ret_max_chars);

  g_value_unset (&value);

  /* Test 2: Set max characters to G_MAXINT */
  g_value_init (&value, G_TYPE_INT);
  max_chars = G_MAXINT;
  hildon_get_password_dialog_set_max_characters(get_password_dialog,max_chars);
  g_object_get_property(G_OBJECT (get_password_dialog),"max-characters",&value);
  ret_max_chars = g_value_get_int (&value);

  fail_if(ret_max_chars!=G_MAXUINT16,
	  "hildon-get-password-dialog: Set %i max chars incorrect. Return value is %u",max_chars,ret_max_chars);

  g_value_unset (&value);
}
END_TEST

/**
 * Purpose: Check that limits "max characters" values are set and get properly.
 * Cases considered:
 *    - Set max characters to 0.
 *    - Set max characters to -1.
 *    - Set max characters on NULL object.
 */
START_TEST (test_set_max_characters_invalid)
{
  gint max_chars;
  gint ret_max_chars;
  GValue value={0, };

  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");
    
  g_value_init (&value, G_TYPE_INT);

  /* Test 1: Set max characters to 0 */
  max_chars = 0;
  hildon_get_password_dialog_set_max_characters(get_password_dialog,max_chars);
  g_object_get_property(G_OBJECT (get_password_dialog),"max-characters",&value);
  ret_max_chars = g_value_get_int (&value);

  fail_if(ret_max_chars!=max_chars,
	  "hildon-get-password-dialog: Set %i max chars incorrect. Return value is %i",max_chars,ret_max_chars);

  g_value_unset (&value);

  /* Test 2: Set max characters to -1 */
  g_value_init (&value, G_TYPE_INT);
  max_chars = -1;
  hildon_get_password_dialog_set_max_characters(get_password_dialog,max_chars);
  g_object_get_property(G_OBJECT (get_password_dialog),"max-characters",&value);
  ret_max_chars = g_value_get_int (&value);

  fail_if(ret_max_chars!=0,
	  "hildon-get-password-dialog: Set %i max chars incorrect. Return value is %i and expected is 0",max_chars,ret_max_chars);
  g_value_unset (&value);
    
  /* Test 2: Set max characters on NULL object */
  max_chars = 1;
  hildon_get_password_dialog_set_max_characters(NULL,max_chars);
}
END_TEST

/**
 * Purpose: Check that regular password values are get properly.
 * Cases considered:
 *    - Get password "test_password" set by g_object_set_property.
 *    - Get password "01234567890" set by g_object_set_property.
 *    - Get password TEST_STRING set by g_object_set_property.
 *    - Get password "" set by set by g_object_set_property.
 */
START_TEST (test_get_password_regular)
{
  const gchar * default_password="test_password";  
  const gchar * ret_password;
  GValue value={0, };

  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");
    
  /* Test 1: Get password "test_password" set by g_object_set_property. */
  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_password);
  g_object_set_property(G_OBJECT (get_password_dialog),"password",&value);
  g_value_unset (&value);
    
  ret_password = hildon_get_password_dialog_get_password(get_password_dialog);
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  /* Test 2: Get password "01234567890" set by g_object_set_property. */
  default_password = "01234567890";

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_password);
  g_object_set_property(G_OBJECT (get_password_dialog),"password",&value);
  g_value_unset (&value);
    
  ret_password = hildon_get_password_dialog_get_password(get_password_dialog);
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  /* Test 3: Get password TEST_STRING set by g_object_set_property. */
  default_password = TEST_STRING;

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_password);
  g_object_set_property(G_OBJECT (get_password_dialog),"password",&value);
  g_value_unset (&value);
    
  ret_password = hildon_get_password_dialog_get_password(get_password_dialog);
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  /* Test 4: Get password "" set by g_object_set_property. */
  default_password = "";

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_password);
  g_object_set_property(G_OBJECT (get_password_dialog),"password",&value);
  g_value_unset (&value);
    
  ret_password = hildon_get_password_dialog_get_password(get_password_dialog);
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-get-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);
}
END_TEST

/**
 * Purpose: Check that limit password values are get properly.
 * Cases considered:
 *    - Get empty password set by g_object_set_property with NULL value.
 *    - Get password from NULL object.
 *
 */
START_TEST (test_get_password_invalid)
{
  const gchar * default_password=NULL;  
  const gchar * ret_password;
  GValue value={0, };

  /* Check get password dialog object has been created properly */
  fail_if(!HILDON_IS_GET_PASSWORD_DIALOG (get_password_dialog), 
          "hildon-get-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (gpd_window),
          "hildon-get-password-dialog: Window creation failed.");
    
  /* Test 1: Get password NULL set by g_object_set_property. */
  default_password = NULL;
  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_password);
  g_object_set_property(G_OBJECT (get_password_dialog),"password",&value);
  g_value_unset (&value);
    
  ret_password = hildon_get_password_dialog_get_password(get_password_dialog);
  fail_if(strcmp(ret_password,"")!=0,
	  "hildon-get-password-dialog: Returned password is not empty (%s)",ret_password);

  /* Test 2: Get password from NULL object. */
  ret_password = hildon_get_password_dialog_get_password(NULL);
}
END_TEST


/* ---------- Suite creation ---------- */
Suite *create_hildon_get_password_dialog_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonGetPasswordDialog");

  /* Create test cases */
  TCase *tc1 = tcase_create("new_with_default");
  TCase *tc2 = tcase_create("set_max_characters");
  TCase *tc3 = tcase_create("get_password");

  /* Create test case for hildon_get_password_dialog_new_with_default and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_get_password_dialog, fx_teardown_default_get_password_dialog);
  tcase_add_test(tc1, test_new_with_default_regular);
  tcase_add_test(tc1, test_new_with_default_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for hildon_get_password_dialog_set_max_characters and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_get_password_dialog, fx_teardown_default_get_password_dialog);
  tcase_add_test(tc2, test_set_max_characters_regular);
  tcase_add_test(tc2, test_set_max_characters_limits);
  tcase_add_test(tc2, test_set_max_characters_invalid);
  suite_add_tcase (s, tc2);

  /* Create test case for hildon_get_password_dialog_get_password and add it to the suite */
  tcase_add_checked_fixture(tc3, fx_setup_default_get_password_dialog, fx_teardown_default_get_password_dialog);
  tcase_add_test(tc3, test_get_password_regular);
  tcase_add_test(tc3, test_get_password_invalid);
  suite_add_tcase (s, tc3);


  /* Return created suite */
  return s;             
}
