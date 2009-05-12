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

#include <hildon/hildon-set-password-dialog.h>
#include <hildon/hildon-window.h>

/* -------------------- Fixtures -------------------- */

static HildonSetPasswordDialog *set_password_dialog = NULL;
static GtkWindow *spd_window=NULL;

static void 
fx_setup_default_set_password_dialog ()
{
  int argc = 0;
    
  gtk_init(&argc, NULL);
  spd_window = GTK_WINDOW(create_test_window());

  /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(spd_window),
          "hildon-set_password_dialog: Window creation failed.");

  set_password_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(spd_window, TRUE));    

  /* Check that the set password dialog object has been created properly */
  fail_if(!HILDON_SET_PASSWORD_DIALOG(set_password_dialog),
          "hildon-set_password_dialog: Creation failed.");

  show_test_window(GTK_WIDGET(spd_window));

  show_test_window(GTK_WIDGET(set_password_dialog));

}

static void 
fx_teardown_default_set_password_dialog ()
{

  gtk_widget_destroy (GTK_WIDGET (set_password_dialog));

  gtk_widget_destroy (GTK_WIDGET (spd_window));

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for get_protected -----*/

/**
 * Purpose: Check that regular values are get properly.
 * Cases considered:
 *    - Get TRUE from modify_protection property.
 *    - Get FALSE from modify_protection property.
 *
 */
START_TEST (test_get_protected_regular)
{
  gboolean ret_protected;
  HildonSetPasswordDialog *protected_test_password_dialog = NULL;

  /* I must create one dialog for each value of protected boolean because is a G_PARAM_CONSTRUCT_ONLY property */
  protected_test_password_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(spd_window, TRUE));
    
  /* Test 1: Get TRUE from modify_protection property. */
  ret_protected = hildon_set_password_dialog_get_protected(set_password_dialog);
  fail_if(ret_protected!=TRUE,
	  "hildon-set_password_dialog: modify_protection must be TRUE, but hildon_set_password_dialog_get_protected returns FALSE");

  gtk_widget_destroy (GTK_WIDGET (protected_test_password_dialog));

  /* Test 2: Get FALSE from modify_protection property */
  /* I must create one dialog for each value of protected boolean because is a G_PARAM_CONSTRUCT_ONLY property */
  protected_test_password_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(spd_window, FALSE));

  ret_protected = hildon_set_password_dialog_get_protected(protected_test_password_dialog);
  fail_if(ret_protected!=FALSE,
	  "hildon-set_password_dialog: modify_protection must be FALSE, but hildon_set_password_dialog_get_protected returns TRUE");
    
  gtk_widget_destroy (GTK_WIDGET (protected_test_password_dialog));
}
END_TEST

/**
 * Purpose: Check that invalid values are get properly.
 * Cases considered:
 *    - Get modify_protection from NULL object.
 *
 */
START_TEST (test_get_protected_invalid)
{
  gboolean ret_protected;

  ret_protected = hildon_set_password_dialog_get_protected (NULL);
  fail_if (ret_protected != FALSE,
	   "hildon-set_password_dialog: modify_protection must be FALSE, but hildon_set_password_dialog_get_protected return TRUE");
}
END_TEST

/* ----- Test case for get_password -----*/

/**
 * Purpose: Check that regular values are get properly.
 * Cases considered:
 *    - Get password "test_password" from HildonSetPasswordDialog created with "test_password" by default.
 *    - Get password "" from HildonSetPasswordDialog created with "" by default.
 *    - Get password TEST_STRING from HildonSetPasswordDialog created with TEST_STRING by default.
 */
START_TEST (test_get_password_regular)
{
  const gchar * default_password=NULL;
  const gchar * ret_password;
  HildonSetPasswordDialog * default_set_password_dialog;
    

  /* Test 1: Get password "test_password" from HildonSetPasswordDialog created with "test_password" by default. */
  default_password = "test_password";
  default_set_password_dialog = 
    HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new_with_default(spd_window,default_password,FALSE));
  fail_if(!HILDON_IS_SET_PASSWORD_DIALOG(default_set_password_dialog),
          "hildon-set_password_dialog: hildon-set-password-dialog: Creation failed with hildon_set_password_dialog_new_with_default");

  ret_password = hildon_set_password_dialog_get_password(default_set_password_dialog);   
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-set_password_dialog: default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy (GTK_WIDGET (default_set_password_dialog));    

  /* Test 2: Create new dialog with password "" by default */
  default_password = "";
  default_set_password_dialog = 
    HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new_with_default(spd_window,default_password,FALSE));
  fail_if(!HILDON_IS_SET_PASSWORD_DIALOG(default_set_password_dialog),
          "hildon-set-password-dialog: Creation failed with hildon_set_password_dialog_new_with_default");

  ret_password = hildon_set_password_dialog_get_password(default_set_password_dialog);   
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-set_password_dialog: default password and returned password are diferent (%s,%s)",default_password,ret_password);

  gtk_widget_destroy (GTK_WIDGET (default_set_password_dialog));        

  /* Test 3: Create new dialog with password TEST_STRING by default */
  default_password = TEST_STRING;
  default_set_password_dialog = 
    HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new_with_default(spd_window,default_password,FALSE));
  fail_if(!HILDON_IS_SET_PASSWORD_DIALOG(default_set_password_dialog),
          "hildon-set-password-dialog: Creation failed with hildon_set_password_dialog_new_with_default");

  ret_password = hildon_set_password_dialog_get_password(default_set_password_dialog);
  fail_if(strcmp (default_password,ret_password)!=0,
	  "hildon-set_password_dialog: default password and returned password are diferent (%s,%s)",default_password,ret_password);
    
  gtk_widget_destroy (GTK_WIDGET (default_set_password_dialog));
    
}
END_TEST

/**
 * Purpose: Check that regular values are get properly.
 * Cases considered:
 *    - Get empty password from HildonSetPasswordDialog created with NULL password by default.
 *    - Get password from NULL object.
 */
START_TEST (test_get_password_invalid)
{
  const gchar * ret_password;
  HildonSetPasswordDialog * default_set_password_dialog;
    

  /* Test 1: Get empty password from HildonSetPasswordDialog created with NULL password by default */
  default_set_password_dialog = 
    HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new_with_default(spd_window,NULL,FALSE));
  fail_if(!HILDON_IS_SET_PASSWORD_DIALOG(default_set_password_dialog),
          "hildon-set-password-dialog: Creation failed with hildon_set_password_dialog_new_with_default");

  ret_password = hildon_set_password_dialog_get_password(default_set_password_dialog);
  fail_if(strcmp(ret_password,"")!=0,
	  "hildon-set_password_dialog: returned password is not empty");
    
  gtk_widget_destroy (GTK_WIDGET (default_set_password_dialog));

  /* Test 2: Get password from NULL object */
  ret_password = hildon_set_password_dialog_get_password(NULL);

  fail_if(ret_password!=NULL,
	  "hildon-set_password_dialog: returned password is not null");
    
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_set_password_dialog_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonSetPasswordDialog");

  /* Create test cases */
  TCase *tc1 = tcase_create("get_protected");
  TCase *tc2 = tcase_create("get_password");

  /* Create test case for hildon_set_password_dialog_get_protected and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_set_password_dialog, fx_teardown_default_set_password_dialog);
  tcase_add_test(tc1, test_get_protected_regular);
  tcase_add_test(tc1, test_get_protected_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for hildon_set_password_dialog_get_password and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_set_password_dialog, fx_teardown_default_set_password_dialog);
  tcase_add_test(tc2, test_get_password_regular);
  tcase_add_test(tc2, test_get_password_invalid);
  suite_add_tcase (s, tc2);

  /* Return created suite */
  return s;             
}
