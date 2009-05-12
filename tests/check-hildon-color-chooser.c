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
#include <glib/gprintf.h>
#include "test_suites.h"
#include "check_utils.h"

#include <hildon/hildon-color-chooser.h>
#include <hildon/hildon-window.h>

#define MAX_COLOR 65535

/* -------------------- Fixtures -------------------- */
static HildonColorChooser *color_chooser = NULL;
static GtkWindow * cs_window;

static void 
fx_setup_default_color_chooser ()
{
  int argc = 0;
  gtk_init(&argc, NULL);
  
  cs_window = GTK_WINDOW(create_test_window());

  /* Check window object has been created properly */
  fail_if(!HILDON_IS_WINDOW(cs_window),
         "hildon-color-chooser: Window creation failed.");  

  color_chooser = HILDON_COLOR_CHOOSER(hildon_color_chooser_new());
  /* Check that the color chooser object has been created properly */
  fail_if(!HILDON_IS_COLOR_CHOOSER(color_chooser),
          "hildon-color-chooser: Creation failed.");  

  gtk_container_add (GTK_CONTAINER(cs_window), GTK_WIDGET (color_chooser));

  show_all_test_window(GTK_WIDGET(cs_window));
  show_all_test_window(GTK_WIDGET(color_chooser));

}

static void 
fx_teardown_default_color_chooser ()
{    
  gtk_widget_destroy (GTK_WIDGET (color_chooser)); 
  gtk_widget_destroy (GTK_WIDGET (cs_window));
 
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_color -----*/

/**
 * Purpose: Check that regular base colors are set and get properly
 * Cases considered:
 *    - Set the color #33CC33.
 */
START_TEST (test_set_color_regular)
{
  GdkColor color;
  GdkColor * b_color;
  GdkColor * ret_color = g_new (GdkColor, 1);    
   
  /* Test 1: Set the color #33CC33*/
  gdk_color_parse( "#33CC33", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser, ret_color);    

  fail_if ((b_color->red != ret_color->red) || (b_color->green != ret_color->green) || 
           (b_color->blue != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           b_color->red, b_color->green, b_color->blue);

  if (b_color)
    gdk_color_free(b_color);

  if (ret_color)
    gdk_color_free(ret_color);
}
END_TEST

/**
 * Purpose: Check that limits base colors are set and get properly
 * Cases considered:
 *    - Set and get base color defined by #000000.
 *    - Set and get base color defined by #FFFFFF.
 *    - Set and get base color defined by #FF0000.
 *    - Set and get base color defined by #0000FF.
 */
START_TEST (test_set_color_limits)
{
  GdkColor color;
  GdkColor * b_color;
  GdkColor * ret_color = g_new (GdkColor, 1);

  /* Check that the color chooser object has been created properly */
  fail_if(!HILDON_IS_COLOR_CHOOSER(color_chooser),
          "hildon-color-chooser: Creation failed.");   
    
  /* Test 1: Set the color #000000 */
  gdk_color_parse( "#000000", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser, ret_color);    

  fail_if ((b_color->red != ret_color->red) || (b_color->green != ret_color->green) || 
           (b_color->blue != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           b_color->red, b_color->green, b_color->blue);

  gdk_color_free(b_color);


  /* Test 2: Set the color #FFFFFF */
  gdk_color_parse( "#000000", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser, ret_color);    

  fail_if ((b_color->red != ret_color->red) || (b_color->green != ret_color->green) || 
           (b_color->blue != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           b_color->red, b_color->green, b_color->blue);

  gdk_color_free(b_color);


  /* Test 3: Set the color #FF0000 */
  gdk_color_parse( "#FF0000", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser, ret_color);    

  fail_if ((b_color->red != ret_color->red) || (b_color->green != ret_color->green) || 
           (b_color->blue != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           b_color->red, b_color->green, b_color->blue);

  gdk_color_free(b_color);

  /* Test 4: Set the color to #0000FF*/
  gdk_color_parse( "#0000FF", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser, ret_color);    

  fail_if ((b_color->red != ret_color->red) || (b_color->green != ret_color->green) || 
           (b_color->blue != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           b_color->red, b_color->green, b_color->blue);

  if (b_color)
    gdk_color_free(b_color);

  if (ret_color)
    gdk_color_free(ret_color);
}
END_TEST
/**
 * Purpose: Check that invalid base colors are set and get properly
 * Cases considered:
 *    - Set and get base color defined by #00FFFF and pixel 0.
 *    - Set and get base color defined by #0ABCDE and pixel 0.
 *    - Set and get base color defined by #FF00FF and pixel 0.
 *    - Set the color (65536,65536,65536).
 *    - Set and get base color defined by NULL color.
 *    - Set base color on NULL object.
 *    - Get base color from NULL object.
 */
START_TEST (test_set_color_invalid)
{
  GdkColor color;
  GdkColor * b_color;
  GdkColor * ret_color = g_new (GdkColor, 1);
  guint red;
  guint green;
  guint blue;

  /* Check that the color chooser object has been created properly */
  fail_if(!HILDON_IS_COLOR_CHOOSER(color_chooser),
          "hildon-color-chooser: Creation failed.");
   
  /* Initialize color to check that invalid values doesn't set colors*/
  gdk_color_parse( "#000000", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  gdk_color_free(b_color);

  /* Test 1: Set the color #00FFFF*/
  gdk_color_parse( "#00FFFF", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser,ret_color);    
    
  fail_if ((0 != ret_color->red) || (0 != ret_color->green) || (0 != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           0, 0, 0);

  gdk_color_free(b_color);

  /* Test 2: Set the color #FFFFFF */
  gdk_color_parse( "#0ABCDE", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser,ret_color);    

  fail_if ((0 != ret_color->red) || (0 != ret_color->green) || (0 != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           0, 0, 0);

  gdk_color_free(b_color);

  /* Test 2: Set the color #FF00FF */
  gdk_color_parse( "#FF00FF", &color);
  b_color = gdk_color_copy(&color);

  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser,ret_color);    

  fail_if ((0 != ret_color->red) || (0 != ret_color->green) || (0 != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           0, 0, 0);

  gdk_color_free(b_color);

  /* Test 3: Set the color (65536,65536,65536)*/
  gdk_color_parse( "#000000", &color);
  b_color = gdk_color_copy(&color);
  red = MAX_COLOR+1;
  green = MAX_COLOR+1;
  blue = MAX_COLOR+1;
    
  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
    
  hildon_color_chooser_set_color(color_chooser,b_color);
  hildon_color_chooser_get_color(color_chooser, ret_color);    

  fail_if ((0 != ret_color->red) || (0 != ret_color->green) || (0 != ret_color->blue),
           "hildon-color-chooser: The returned RGB color is %i/%i/%i and should be %i/%i/%i",
           ret_color->red, ret_color->green, ret_color->blue,
           0, 0, 0);
    
  /* Test 5: Set the color NULL on color chooser*/
  hildon_color_chooser_set_color(color_chooser,NULL);

  /* Test 6: Set the color on NULL object*/
  hildon_color_chooser_set_color(NULL,b_color);

  /* Test 7: Get color from NULL object*/
  hildon_color_chooser_get_color(NULL, ret_color);

  if (b_color)
    gdk_color_free(b_color);

  if (ret_color)
    gdk_color_free(ret_color);
}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_color_chooser_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonColorChooser");

  /* Create test case for hildon_color_chooser_set_color and add it to the suite */
  TCase *tc1 = tcase_create("set_color");
  tcase_add_checked_fixture(tc1, fx_setup_default_color_chooser, fx_teardown_default_color_chooser);
  tcase_add_test(tc1, test_set_color_regular);
  tcase_add_test(tc1, test_set_color_limits);
  tcase_add_test(tc1, test_set_color_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;             
}
