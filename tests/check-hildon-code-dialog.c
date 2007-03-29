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
#include <glib/gprintf.h>
#include <string.h>
#include "test_suites.h"
#include "check_utils.h"

#include "hildon-code-dialog.h"

/* -------------------- Fixtures -------------------- */

static HildonCodeDialog *code_dialog = NULL;

static void 
fx_setup_default_code_dialog()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  code_dialog = HILDON_CODE_DIALOG(hildon_code_dialog_new());
  /* Check code_dialog object has been created properly */
  fail_if(!HILDON_IS_CODE_DIALOG(code_dialog), 
          "hildon-code-dialog: Creation failed.");

  show_test_window(GTK_WIDGET(code_dialog));
  
}

static void 
fx_teardown_default_code_dialog()
{
  
  /* Destroy the dialog */
  gtk_widget_destroy (GTK_WIDGET (code_dialog));

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for get_code -----*/
/**
 * Purpose: Check that the regular code values are get without problems
 *
 * Cases considered:
 *    - Get code from new created dialog.
 *    
 */
START_TEST (test_get_code_regular)
{
  const gchar * code;
    
  /* Test 1: Get code from new created dialog. */
  /* Check that code is correctly get. */
  code  = hildon_code_dialog_get_code (code_dialog);
  fail_if (strcmp (code,"") != 0,
	   "hildon-code-dialog: init code isn't empty");

}
END_TEST

/**
 * Purpose: Check that the regular code values are get without problems
 *
 * Cases considered:
 *    - Get code from NULL object
 *    - Get code from object that isn't a code dialog.
 *    
 */
START_TEST (test_get_code_invalid)
{
  const gchar * code;
  GtkWidget *aux_object = NULL;
    
  /* Test 1: Get code from NULL object. */
  code  = hildon_code_dialog_get_code (NULL);

  /* Test 2: Get code from object that it isn't a code dialog. */
  aux_object = gtk_hbox_new (TRUE, 0);
  code  = hildon_code_dialog_get_code ((HildonCodeDialog *) aux_object);

  gtk_widget_destroy (aux_object);

}
END_TEST

/* ---------- Suite creation ---------- */
Suite *create_hildon_code_dialog_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonCodeDialog");
  
  /* Create test cases */
  TCase *tc1 = tcase_create("get_code");

  /* Create test case for hildon_code_dialog_get_code and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_code_dialog, fx_teardown_default_code_dialog);
  tcase_add_test(tc1, test_get_code_regular);
  tcase_add_test(tc1, test_get_code_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;             
}
