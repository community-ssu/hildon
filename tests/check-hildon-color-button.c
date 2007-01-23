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
#include <check.h>
#include <gtk/gtkmain.h>
#include <gtk/gtkhbox.h>
#include <glib/gprintf.h>
#include "test_suites.h"
#include "check_utils.h"
#include "hildon-color-button.h"

/* -------------------- Fixtures -------------------- */
static HildonColorButton *color_button = NULL;
static GtkWidget *showed_window = NULL;

static void 
fx_setup_default_color_button()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  color_button = HILDON_COLOR_BUTTON(hildon_color_button_new());
      /* Check that the color button object has been created properly */

  showed_window =  create_test_window ();
 
  /* This packs the widget into the window (a gtk container). */
  gtk_container_add (GTK_CONTAINER (showed_window), GTK_WIDGET (color_button));
  
  /* Displays the widget and the window */
  show_all_test_window (showed_window);


  fail_if(!HILDON_IS_COLOR_BUTTON(color_button),
          "hildon-color-button: Creation failed.");

}

static void 
fx_teardown_default_color_button()
{
  
  /* Destroy the widget and the window */
  gtk_widget_destroy(GTK_WIDGET(showed_window)); 

}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set_color -----*/
/**
 * Purpose: Check that a regular color can be set safely
 * Cases considered:
 *    - Set color defined by (255, 255, 255) and pixel = 255
 *    - Set color defined by (10, 20, 30) and pixel = 10
 *    - Set color defined by (20, 10, 30) and pixel = 10
 *    - Set color defined by (30, 10, 20) and pixel = 20
 */

START_TEST (test_set_color_regular)
{
  GdkColor color;
  GdkColor * b_color;
  GdkColor * ret_color = g_new (GdkColor, 1);
  guint red;
  guint green;
  guint blue;
  guint pixel;

  b_color = gdk_color_copy(&color);

  /* Test 1: Set color defined by (255,255,255)*/
  red = 255;
  green = 255;
  blue = 255;
  pixel = 255;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;
    
  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 2: Set color defined by (10,20,30)*/
  red = 10;
  green = 20;
  blue = 30;
  pixel = 10;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;
    
  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 3: Set color defined by (20,10,30)*/
  red = 20;
  green = 10;
  blue = 30;
  pixel = 10;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;
    
  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 4: Set color defined by (30,10,20)*/
  red = 30;
  green = 10;
  blue = 20;
  pixel = 20;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;
    
  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);
    
  if (b_color)
          gdk_color_free(b_color);

  if (ret_color)
          gdk_color_free(ret_color);
}
END_TEST

/**
 * Purpose: Check that a limits values on color components can be set safely.
 * Cases considered:
 *    - Set color defined by (0, 0, 0) and pixel = 0
 *    - Set color defined by (G_MAXUINT16, G_MAXUINT16, G_MAXUINT16) and pixel = G_MAXUINT32
 *    - Set color defined by (G_MAXUINT16, 0, 0) and pixel = G_MAXUINT32
 *    - Set color defined by (0, G_MAXUINT16, 0) and pixel = 0
 *    - Set color defined by (0, 0, G_MAXUINT16) and pixel = G_MAXUINT32
 */

START_TEST (test_set_color_limits)
{
  GdkColor color;
  GdkColor * b_color;
  GdkColor * ret_color = g_new (GdkColor, 1);
  guint red;
  guint green;
  guint blue;
  guint pixel;

  b_color = gdk_color_copy(&color);

  /* Test 1: Set color defined by (0,0,0)*/
  red = 0;
  green = 0;
  blue = 0;
  pixel = 0;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;

  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 2: Set color defined by (G_MAXUINT16,G_MAXUINT16,G_MAXUINT16)*/
  red = G_MAXUINT16;
  green = G_MAXUINT16;
  blue = G_MAXUINT16;
  pixel = G_MAXUINT32;

  b_color->pixel=pixel;
  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
    
  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 3: Set color defined by (G_MAXUINT16,0,0)*/
  red = G_MAXUINT16;
  green = 0;
  blue = 0;
  pixel = G_MAXUINT32;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;

  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 4: Set color defined by (0,G_MAXUINT16,0)*/
  red = 0;
  green = G_MAXUINT16;
  blue = 0;
  pixel = 0;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;
        
  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);

  /* Test 5: Set color defined by (0,0,G_MAXUINT16)*/
  red = 0;
  green = 0;
  blue = G_MAXUINT16;
  pixel = G_MAXUINT32;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=pixel;

  hildon_color_button_set_color(color_button,b_color);
  hildon_color_button_get_color(color_button, ret_color);

  fail_if ((red != ret_color->red) || (green != ret_color->green) || 
           (blue != ret_color->blue) || (pixel != ret_color->pixel),
           "hildon-color-button: The returned color for the pixel %d (returned %d) in RGB is %d/%d/%d and should be %d/%d/%d",
           pixel, ret_color->red, ret_color->green, 
           ret_color->blue, ret_color->pixel);
    
  if (b_color) 
          gdk_color_free(b_color);

  if (ret_color)
          gdk_color_free(ret_color);
}
END_TEST

/**
 * Purpose: Check that a limits values on color components can be set safely.
 * Cases considered:
 *    - Set color defined by (0, 0, 0) on NULL object.
 *    - Get color from NULL object.
 *    - Set color defined by (0, 0, 0) on GtkHBox object.
 *    - Get a color from GtkHBox object.
 */

START_TEST (test_set_color_invalid)
{
  GdkColor color;
  GdkColor * b_color;
  GdkColor * ret_color = g_new (GdkColor, 1);
  GtkWidget *aux_object = NULL;
  guint red;
  guint green;
  guint blue;
    
  b_color = gdk_color_copy(&color);

  red = 0;
  green = 0;
  blue = 0;

  b_color->red=red;
  b_color->green=green;
  b_color->blue=blue;
  b_color->pixel=0;

  /* Test 1: Set color defined by (0,0,0) on NULL object*/
  hildon_color_button_set_color(NULL,b_color);

  /* Test 2: Get color from NULL object*/
  hildon_color_button_get_color(NULL, ret_color);    
   
  /* Test 3: Set color defined by (0, 0, 0) on GtkHBox object. */
  aux_object = gtk_hbox_new (TRUE, 0);
  hildon_color_button_set_color((HildonColorButton *) (aux_object), b_color);
  gdk_color_free(b_color);

  /* Test 4: Get color from GtkHBox object. */
  ret_color->red = 99;
  ret_color->green = 99;
  ret_color->blue = 99;
  hildon_color_button_get_color((HildonColorButton *) (aux_object), ret_color);
    
  if (ret_color->red != 99      || 
      ret_color->green != 99    ||
      ret_color->blue != 99)
    {
      gtk_widget_destroy(aux_object);
      fail ("hildon-color-button: get_color must not modify the color when launched on invalid widget");
    }
  
  gtk_widget_destroy(aux_object);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_color_button_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonColorButton");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_color");

  /* Create test case for hildon_color_button_set_color and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_color_button, fx_teardown_default_color_button);
  tcase_add_test(tc1, test_set_color_regular);
  tcase_add_test(tc1, test_set_color_limits);
  tcase_add_test(tc1, test_set_color_invalid);
  suite_add_tcase(s, tc1);

  /* Return created suite */
  return s;             
}
