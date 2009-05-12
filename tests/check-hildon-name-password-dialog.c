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
#include <hildon/hildon-name-password-dialog.h>
#include <hildon/hildon-window.h>

/* -------------------- Fixtures -------------------- */

static HildonNamePasswordDialog *name_password_dialog = NULL;
static GtkWindow * npd_window = NULL; 

static void 
fx_setup_default_name_password_dialog ()
{
  int argc = 0;
    
  gtk_init(&argc, NULL);
    
  npd_window = GTK_WINDOW(create_test_window());   

  /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(npd_window),
          "hildon-name-password-dialog: Window creation failed.");

  name_password_dialog = HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new(npd_window));
  /* Check that the name password dialog object has been created properly */
  fail_if (!HILDON_IS_NAME_PASSWORD_DIALOG (name_password_dialog),
           "hildon-name-password-dialog: Creation failed.");
  
  show_test_window(GTK_WIDGET(npd_window));

  show_test_window(GTK_WIDGET(name_password_dialog));

}

static void 
fx_teardown_default_name_password_dialog ()
{
  gtk_widget_destroy (GTK_WIDGET(name_password_dialog));

  gtk_widget_destroy (GTK_WIDGET(npd_window));
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for new_with_default -----*/

/**
 * Purpose: Check that regular name and password values are correctly added by default on creation.
 * Cases considered: 
 *    - Create new name and password dialog with "test_name" and "test_password" by default.
 *    - Create new name and password dialog with "0123456789" and "123412341234" by default.
 *    - Create new name and password dialog with TEST_STRING and TEST_STRING by default.
 *    - Create new name and password dialog with "" and "" by default.
 */
START_TEST (test_new_with_default_regular)
{
  const gchar * default_name="test_name";
  const gchar * default_password="test_password";
  const gchar * ret_name;
  const gchar * ret_password;
  HildonNamePasswordDialog * default_name_password_dialog=NULL;

  /* Check that the name password dialog object has been created properly */
  fail_if (!HILDON_IS_NAME_PASSWORD_DIALOG (name_password_dialog),
           "hildon-name-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (npd_window),
	   "hildon-name-password-dialog: Window creation failed.");

  /* Test 1: Create new dialog with username and password by default */
  default_name_password_dialog = 
    HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new_with_default(npd_window, default_name, default_password));       
  fail_if(!HILDON_IS_NAME_PASSWORD_DIALOG(default_name_password_dialog),
	  "hildon-name-password-dialog: Creation failed with hildon_name_password_dialog_new_with_default");

  ret_name = hildon_name_password_dialog_get_name(default_name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)",default_name,ret_name);

  ret_password = hildon_name_password_dialog_get_password(default_name_password_dialog);
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-name-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy (GTK_WIDGET(default_name_password_dialog));
  default_name_password_dialog=NULL;

  /* Test 2: Create new dialog with username and password by default */
  default_name = "0123456789";
  default_password = "123412341234";

  default_name_password_dialog = 
    HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new_with_default(npd_window, default_name, default_password));       
  fail_if(!HILDON_IS_NAME_PASSWORD_DIALOG(default_name_password_dialog),
	  "hildon-name-password-dialog: Creation failed with hildon_name_password_dialog_new_with_default");

  ret_name = hildon_name_password_dialog_get_name(default_name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)", default_name, ret_name);

  ret_password = hildon_name_password_dialog_get_password(default_name_password_dialog);
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-name-password-dialog: Default password and returned password are diferent (%s,%s)", default_password, ret_password);

  gtk_widget_destroy (GTK_WIDGET(default_name_password_dialog));

  /* Test 3: Create new dialog with invalid username and password by default. */
  default_name = TEST_STRING;
  default_password = TEST_STRING;

  default_name_password_dialog = 
    HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new_with_default(npd_window,default_name,default_password));

  fail_if(!HILDON_IS_NAME_PASSWORD_DIALOG(default_name_password_dialog),
	  "hildon-name-password-dialog: Creation failed with hildon_name_password_dialog_new_with_default");

  ret_name = hildon_name_password_dialog_get_name(default_name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)",default_name,ret_name);

  ret_password = hildon_name_password_dialog_get_password(default_name_password_dialog);       
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-name-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy (GTK_WIDGET(default_name_password_dialog)); 

  /* Test 4: Create new dialog with empty username and password by default. */
  default_name = "";
  default_password = "";

  default_name_password_dialog = 
    HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new_with_default(npd_window,default_name,default_password));

  fail_if(!HILDON_IS_NAME_PASSWORD_DIALOG(default_name_password_dialog),
	  "hildon-name-password-dialog: Creation failed with hildon_name_password_dialog_new_with_default");

  ret_name = hildon_name_password_dialog_get_name(default_name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)",default_name,ret_name);

  ret_password = hildon_name_password_dialog_get_password(default_name_password_dialog);  
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-name-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);


  gtk_widget_destroy (GTK_WIDGET(default_name_password_dialog)); 
}
END_TEST


/**
 * Purpose: Check that regular name and password values are correctly added by default on creation.
 * Cases considered: 
 *    - Create a name and password dialog with NULL name.
 *    - Create a name and password dialog with NULL password.
 */
START_TEST (test_new_with_default_invalid)
{
  const gchar * default_name="test_name";
  const gchar * default_password="test_password";
  const gchar * ret_name;
  const gchar * ret_password;
  HildonNamePasswordDialog * default_name_password_dialog;

  /* Check that the name password dialog object has been created properly */
  fail_if (!HILDON_IS_NAME_PASSWORD_DIALOG (name_password_dialog),
           "hildon-name-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (npd_window),
	   "hildon-name-password-dialog: Window creation failed.");
    
  /* Test 1: Create a name and password dialog with NULL name. */
  default_name_password_dialog = 
    HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new_with_default(npd_window,NULL,default_password));
   
  fail_if(!HILDON_IS_NAME_PASSWORD_DIALOG(default_name_password_dialog),
	  "hildon-name-password-dialog: Creation failed with hildon_name_password_dialog_new_with_default");

  ret_name = hildon_name_password_dialog_get_name(default_name_password_dialog);
  fail_if (strcmp ("",ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (,%s)",ret_name);

  ret_password = hildon_name_password_dialog_get_password(default_name_password_dialog);
  fail_if (strcmp (default_password,ret_password) != 0,
	   "hildon-name-password-dialog: Default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy (GTK_WIDGET(default_name_password_dialog)); 

  /* Test 2: Create a name and password dialog with NULL name. */
  default_name_password_dialog = 
    HILDON_NAME_PASSWORD_DIALOG(hildon_name_password_dialog_new_with_default(npd_window,default_name,NULL));

  fail_if(!HILDON_IS_NAME_PASSWORD_DIALOG(default_name_password_dialog),
	  "hildon-name-password-dialog: Creation failed with hildon_name_password_dialog_new_with_default");

  ret_name = hildon_name_password_dialog_get_name(default_name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)",default_name,ret_name);

  ret_password = hildon_name_password_dialog_get_password(default_name_password_dialog);   
  fail_if (strcmp ("",ret_password) != 0,
	   "hildon-name-password-dialog: Default password and returned password are diferent (,%s)",ret_password);
        
  gtk_widget_destroy (GTK_WIDGET(default_name_password_dialog)); 

}
END_TEST

/* ----- Test case for get_name -----*/

/**
 * Purpose: Check that regular name and password values are correctly added by default on creation.
 * Cases considered: 
 *    - Get name "test_name" set by g_object_set_property.
 *    - Get name "0123456789" set by g_object_set_property.
 *    - Get name TEST_STRING set by g_object_set_property.
 */
START_TEST (test_get_name_regular)
{
  const gchar * default_name="test_name";  
  const gchar * ret_name;
  GValue value={0, };

  /* Check that the name password dialog object has been created properly */
  fail_if (!HILDON_IS_NAME_PASSWORD_DIALOG (name_password_dialog),
           "hildon-name-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (npd_window),
	   "hildon-name-password-dialog: Window creation failed.");

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value, default_name);
  g_object_set_property(G_OBJECT(name_password_dialog), "name", &value);
  g_value_unset (&value);
    
  /* Test 1: Get name "test_name" set by g_object_set_property. */
  ret_name = hildon_name_password_dialog_get_name(name_password_dialog);
    
  fail_if (strcmp(default_name, ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)", default_name, ret_name);
                
  /* Test 2: Get name "0123456789" set by g_object_set_property. */
  default_name = "0123456789";

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_name);
  g_object_set_property(G_OBJECT(name_password_dialog), "name", &value);
  g_value_unset (&value);

  ret_name = hildon_name_password_dialog_get_name(name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)", default_name, ret_name);

  /* Test 3: Get name TEST_STRING set by g_object_set_property. */
  default_name=TEST_STRING;

  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_name);
  g_object_set_property(G_OBJECT (name_password_dialog),"name",&value);
  g_value_unset (&value);

  ret_name = hildon_name_password_dialog_get_name(name_password_dialog);
  fail_if (strcmp (default_name,ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (%s,%s)",default_name,ret_name);
}
END_TEST


/**
 * Purpose: Check that regular name and password values are correctly added by default on creation.
 * Cases considered: 
 *    - Get name on NULL object.
 *    - Get empty password set by g_object_set_property with NULL value.
 */

START_TEST (test_get_name_invalid)
{
  const gchar * default_name;
  const gchar * ret_name;
  GValue value={0, };

  /* Check that the name password dialog object has been created properly */
  fail_if (!HILDON_IS_NAME_PASSWORD_DIALOG (name_password_dialog),
           "hildon-name-password-dialog: Creation failed.");
  fail_if (!GTK_IS_WINDOW (npd_window),
	   "hildon-name-password-dialog: Window creation failed.");

  /* Test 1: Get name on NULL object. */
  ret_name = hildon_name_password_dialog_get_name(NULL);


  /* Test 2: Get name NULL set by g_object_set_property. */
  default_name = NULL;
  g_value_init (&value, G_TYPE_STRING);
  g_value_set_string(&value,default_name);
  g_object_set_property(G_OBJECT (name_password_dialog),"name",&value);
  g_value_unset (&value);

  ret_name = hildon_name_password_dialog_get_name(name_password_dialog);
    
  fail_if (strcmp ("",ret_name) != 0,
	   "hildon-name-password-dialog: Default name and returned name are diferent (,%s)",ret_name);          
}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_name_password_dialog_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonNamePasswordDialog");

  /* Create test cases */
  TCase *tc1 = tcase_create("new_with_default");
  TCase *tc2 = tcase_create("get_name");

  /* Create unit tests for hildon_name_password_dialog_new_with_default and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_name_password_dialog, fx_teardown_default_name_password_dialog);
  tcase_add_test(tc1, test_new_with_default_regular);
  tcase_add_test(tc1, test_new_with_default_invalid);
  suite_add_tcase (s, tc1);

  /* Create unit tests for hildon_name_password_dialog_get_name and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_name_password_dialog, fx_teardown_default_name_password_dialog);
  tcase_add_test(tc2, test_get_name_regular);
  tcase_add_test(tc2, test_get_name_invalid);
  suite_add_tcase (s, tc2);

  /* Return created suite */
  return s;             
}
