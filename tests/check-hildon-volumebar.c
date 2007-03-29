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

#include "hildon-window.h"
#include "hildon-volumebar.h"
#include "hildon-hvolumebar.h"
#include "hildon-vvolumebar.h"

/* -------------------- Fixtures -------------------- */

static HildonVolumebar *volumebar = NULL;
static HildonWindow *window = NULL;

static void 
fx_setup_hvolumebar ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  volumebar = HILDON_VOLUMEBAR(hildon_hvolumebar_new());

  /* Check volumebar object has been created properly */
  fail_if(!HILDON_VOLUMEBAR(volumebar),
          "hildon-volumebar: Creation failed.");
  
  /* Add volumebar to a window to avoid gtk warnings when
     trying to set focus to parent window */
  window = HILDON_WINDOW(hildon_window_new());

 /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(window),
          "hildon-window: Creation failed.");

  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(volumebar));

  /* Displays the widget and the window */
  show_all_test_window (GTK_WIDGET (window));
}

static void
fx_setup_vvolumebar ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  volumebar = HILDON_VOLUMEBAR(hildon_vvolumebar_new());

  /* Check volumebar object has been created properly */
  fail_if(!HILDON_VOLUMEBAR(volumebar),
          "hildon-volumebar: Creation failed.");

  /* Add volumebar to a window to avoid gtk warnings when
     trying to set focus to parent window */
  window = HILDON_WINDOW(hildon_window_new());

 /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(window),
          "hildon-window: Creation failed.");

  gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(volumebar));

  /* Displays the widget and the window */
  show_all_test_window (GTK_WIDGET (window));
}

static void
fx_teardown_volumebar()
{
   gtk_widget_destroy (GTK_WIDGET (window));
}


/* -------------------- Test cases -------------------- */

/* ----- Test case for set/get_mute -----*/

/**
 * Purpose: Check mute usage
 * Cases considered:
 *    - Set mute ON when volumebar is focusable
 *    - Set mute OFF when volumebar is focusable
 *    - Set mute ON when volumebar is not focusable
 *    - Set mute OFF when volumebar is not focusable
 */
START_TEST (test_set_get_mute_regular)
{
  gboolean ret_mute;
  GValue value = {0,};

  /* Test1: Set mute in focusable state */
  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, TRUE);
  g_object_set_property(G_OBJECT(volumebar), "can-focus", &value);
  hildon_volumebar_set_mute(HILDON_VOLUMEBAR(volumebar), TRUE);
  ret_mute = hildon_volumebar_get_mute(HILDON_VOLUMEBAR(volumebar));
  fail_if(ret_mute != TRUE,
          "hildon-volumebar: Set mute to TRUE (volumebar is focusable), but get mute returned FALSE");     

  /* Test2: Unset mute in focusable state */
  hildon_volumebar_set_mute(HILDON_VOLUMEBAR(volumebar), FALSE);
  ret_mute = hildon_volumebar_get_mute(HILDON_VOLUMEBAR(volumebar));
  fail_if(ret_mute != FALSE,
          "hildon-volumebar: Set mute to FALSE (volumebar is focusable), but get mute returned TRUE");

  /* Test3: Set mute in not focusable state */
  g_value_set_boolean(&value, FALSE);
  g_object_set_property(G_OBJECT(volumebar), "can-focus", &value);
  hildon_volumebar_set_mute(HILDON_VOLUMEBAR(volumebar), TRUE);
  ret_mute = hildon_volumebar_get_mute(HILDON_VOLUMEBAR(volumebar));
  fail_if(ret_mute != TRUE,
          "hildon-volumebar: Set mute to TRUE (volumebar is not focusable), but get mute returned FALSE");    
  
  /* Test4: Unset mute in not focusable state */
  hildon_volumebar_set_mute(HILDON_VOLUMEBAR(volumebar), FALSE);
  ret_mute = hildon_volumebar_get_mute(HILDON_VOLUMEBAR(volumebar));
  fail_if(ret_mute != FALSE,
          "hildon-volumebar: Set mute to FALSE (volumebar is not focusable), but get mute returned TRUE");
  fail_if(gtk_window_get_focus(GTK_WINDOW(window)) != NULL,
          "hildon-volumebar: Set mute to FALSE (volumebar is not focusable), but parent window has focused child");
}
END_TEST

/**
 * Purpose: Test handling of invalid values for get/set_mute interface
 * Cases considered:
 *    - Set mute with NULL volumebar object
 *    - Get mute with NULL volumebar object
 */
START_TEST (test_set_get_mute_invalid)
{
  /* Check volumebar object has been created properly */
  fail_if(!HILDON_VOLUMEBAR(volumebar),
          "hildon-volumebar: Creation failed.");

  /* Test1: Check set mute with NULL volumebar object */
  hildon_volumebar_set_mute(NULL, FALSE);

  /* Test2: Check get mute with NULL volumebar object */
  hildon_volumebar_get_mute(NULL);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_volumebar_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonVolumebar");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_get_mute_hvolumebar");
  TCase *tc2 = tcase_create("set_get_mute_vvolumebar");

  /* Create test case for set/get_mute (hvolumebar) and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_hvolumebar, fx_teardown_volumebar);
  tcase_add_test(tc1, test_set_get_mute_regular);
  tcase_add_test(tc1, test_set_get_mute_invalid);
  suite_add_tcase (s, tc1);

  /* Create test case for set/get_mute (vvolumebar) and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_vvolumebar, fx_teardown_volumebar);
  tcase_add_test(tc2, test_set_get_mute_regular);
  tcase_add_test(tc2, test_set_get_mute_invalid);
  suite_add_tcase (s, tc2);

  /* Return created suite */
  return s;
}


