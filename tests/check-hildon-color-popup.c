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
#include <check.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkhbox.h>
#include "test_suites.h"
#include "check_utils.h"
#include "hildon-color-popup.h"
#include "hildon-window.h"

/* -------------------- Fixtures -------------------- */

static GtkWindow *window = NULL;
static GtkWidget *dialog_color_popup = NULL;
static HildonColorPopup color_popup;
static GdkColor initial_color;

static void
fx_setup_default_color_popup ()
{
  int argc = 0;

  gtk_init(&argc, NULL);

  gdk_color_parse ("#33CC33", &initial_color);

  window = GTK_WINDOW(create_test_window());
  
  /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(window),
         "hildon-color-popup: Window creation failed.");  

  dialog_color_popup = hildon_color_popup_new (window, &initial_color, &color_popup); 

  show_all_test_window(GTK_WIDGET(window));
  show_all_test_window(GTK_WIDGET(dialog_color_popup));

  /* Check that the color popup object has been created properly */
  fail_if (!GTK_IS_WIDGET(dialog_color_popup), 
           "hildon-color-popup: Creation failed.");

}

static void 
fx_teardown_default_color_popup ()
{
  gtk_widget_destroy (GTK_WIDGET (window));
}
/* -------------------- Test cases -------------------- */
/* ----- Test case for set_color_from_sliders -----*/

static guint16
round_color (guint16 color)
{
  return ((color >> 11) & 0x1F) << 11;
}

/**
 * Purpose: test getting the color currently set in the sliders
 * Cases considered:
 *    - Set the initial color
 */
START_TEST (test_set_color_from_sliders_regular)
{
  GdkColor color;

  hildon_color_popup_set_color_from_sliders (&color, &color_popup);

  fail_if ((color.red != round_color (initial_color.red)) || (color.green != round_color (initial_color.green)) || 
	   (color.blue != round_color (initial_color.blue)),
	   "hildon-color-popup: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
	   color.red, color.green, color.blue,
	   round_color (initial_color.red), round_color (initial_color.green), round_color (initial_color.blue));
}
END_TEST

/**
 * Purpose: test setting limit values for colors
 * Cases considered:
 *    - Set color (65535, 65535, 65535)
 *    - Set color (0, 0, 0)
 */
START_TEST (test_set_color_from_sliders_limits)
{
  GdkColor color, ret_color;

  /* Destroy the popup */
  gtk_widget_destroy (GTK_WIDGET (window)); 

  /* Test 1 */
  color.red = 65535;
  color.green = 65535;
  color.blue = 65535;

  window = GTK_WINDOW(hildon_window_new());
  dialog_color_popup = hildon_color_popup_new (window, &color, &color_popup); 

  hildon_color_popup_set_color_from_sliders (&ret_color, &color_popup);

  fail_if ((ret_color.red != round_color (color.red)) || (ret_color.green != round_color (color.green)) || 
	   (ret_color.blue != round_color (color.blue)),
	   "hildon-color-popup: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
	   ret_color.red, ret_color.green, ret_color.blue,
	   round_color (color.red), round_color (color.green), round_color (color.blue));

  /* Destroy the popup */
  gtk_widget_destroy (GTK_WIDGET (window));

  /* Test 2 */
  color.red = 0;
  color.green = 0;
  color.blue = 0;

  window = GTK_WINDOW(hildon_window_new());
  dialog_color_popup = hildon_color_popup_new (window, &color, &color_popup); 

  hildon_color_popup_set_color_from_sliders (&ret_color, &color_popup);

  fail_if ((ret_color.red != round_color (color.red)) || (ret_color.green != round_color (color.green)) || 
	   (ret_color.blue != round_color (color.blue)),
	   "hildon-color-popup: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
	   ret_color.red, ret_color.green, ret_color.blue,
	   round_color (color.red), round_color (color.green), round_color (color.blue));
}
END_TEST

/**
 * Purpose: test that invalid values do not cause problems
 * Cases considered:
 *    - Set color to a NULL popup
 *    - Set color to a NULL GdkColor
 *    - Set color to a GtkBox instead of a HildonColorPopup
 */
START_TEST (test_set_color_from_sliders_invalid)
{
  GdkColor color;
  GtkWidget *hbox = NULL;

  /* Test 1 */
  hildon_color_popup_set_color_from_sliders (&color, NULL);

  fail_if ((color.red != round_color (initial_color.red)) || (color.green != round_color (initial_color.green)) ||
	   (color.blue != round_color (initial_color.blue)),
	   "hildon-color-popup: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
	   color.red, color.green, color.blue,
	   round_color (initial_color.red), round_color (initial_color.green), round_color (initial_color.blue));

  /* Test 2 */
  hildon_color_popup_set_color_from_sliders (NULL, &color_popup);

  fail_if ((color.red != round_color (initial_color.red)) || (color.green != round_color (initial_color.green)) ||
	   (color.blue != round_color (initial_color.blue)),
	   "hildon-color-popup: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
	   color.red, color.green, color.blue,
	   round_color (initial_color.red), round_color (initial_color.green), round_color (initial_color.blue));

  /* Test 3 */
  hbox = gtk_hbox_new (FALSE, 0);

  color.red = 0;
  color.green = 0;
  color.blue = 0;

  hildon_color_popup_set_color_from_sliders (&color, (HildonColorPopup*) hbox);

  if ((color.red != 0) || (color.green != 0) || (color.blue != 0))
    {
      gtk_widget_destroy (hbox);
      fail ("hildon-color-popup: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
            color.red, color.green, color.blue, 0, 0, 0);
    }

  gtk_widget_destroy (hbox);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_color_popup_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonColorPopup");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_color_from_sliders");

  /* Create test cases and add them to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_color_popup, fx_teardown_default_color_popup);
  tcase_add_test(tc1, test_set_color_from_sliders_regular);
  tcase_add_test(tc1, test_set_color_from_sliders_limits);
  tcase_add_test(tc1, test_set_color_from_sliders_invalid);
  suite_add_tcase(s, tc1);

  /* Return created suite */
  return s;             
}
