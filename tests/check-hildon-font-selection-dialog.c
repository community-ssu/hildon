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
#include <string.h>
#include "test_suites.h"
#include "check_utils.h"

#include "hildon-font-selection-dialog.h"

#define PREVIEW_TEXT TEST_STRING
#define DEFAULT_FONT_FAMILY "Sans"

static const guint16 N_FONT_SIZES = 7;
static const guint16 FONT_SIZES[] = 
  {
    6, 8, 10, 12, 16, 24, 32
  };

/* -------------------- Fixtures -------------------- */

static HildonFontSelectionDialog *font_selection_dialog = NULL;
static GtkWidget *showed_window = NULL;

static void 
fx_setup_default_font_selection_dialog ()
{
  int argc = 0;
  gtk_init(&argc, NULL);

  showed_window = create_test_window();


  font_selection_dialog = HILDON_FONT_SELECTION_DIALOG(hildon_font_selection_dialog_new(GTK_WINDOW(showed_window), "Font setup"));
 
  /* Check font selection dialog object has been created properly */
  fail_if(!HILDON_IS_FONT_SELECTION_DIALOG(font_selection_dialog), 
          "hildon-font-selection-dialog: Creation failed.");

  show_test_window(GTK_WIDGET(showed_window));
  
  show_test_window(GTK_WIDGET(font_selection_dialog));
 
}

static void 
fx_teardown_default_font_selection_dialog ()
{
  
  gtk_widget_destroy(GTK_WIDGET(font_selection_dialog));
    
  gtk_widget_destroy(GTK_WIDGET(showed_window));

}

/* -------------------- Helpers -------------------- */

static int 
helper_cmp_families(const void *a, const void *b)
{
  const char *a_name =
    pango_font_family_get_name(*(PangoFontFamily **) a);
  const char *b_name =
    pango_font_family_get_name(*(PangoFontFamily **) b);
  
  return g_utf8_collate(a_name, b_name);
}

static void 
helper_get_available_fonts(HildonFontSelectionDialog *fsd,
                           PangoFontFamily ***families,
                           gint *n_families)

{
  pango_context_list_families(gtk_widget_get_pango_context(GTK_WIDGET(fsd)), 
                              families,
			      n_families);

  qsort(*families, *n_families, sizeof(PangoFontFamily *), helper_cmp_families);
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for set/get_preview_text ----- */

/**
 * Purpose: Check set and get of a valid preview text
 * Cases considered:
 *    - Set and get of a preview text
 */
START_TEST (test_set_get_preview_text_regular)
{
  gchar *ret_preview_text;
 
  /* Test1: set and get a preview text */
  hildon_font_selection_dialog_set_preview_text(font_selection_dialog, PREVIEW_TEXT);
  ret_preview_text = hildon_font_selection_dialog_get_preview_text(font_selection_dialog);
  fail_if(strcmp(PREVIEW_TEXT, ret_preview_text) != 0,
          "hildon-font-selection-dialog: Called set_previex_text with \"%s\", but get_preview_text returned \"%s\"",
          PREVIEW_TEXT, ret_preview_text);
  g_free(ret_preview_text);
}
END_TEST

/**
 * Purpose: Check handling of invalid values
 * Cases considered:
 *    - Set empty preview text
 *    - Set a NULL preview text
 *    - Set with NULL object
 *    - Set with NULL object
 */
START_TEST (test_set_get_preview_text_invalid)
{
  gchar *ret_preview_text;

  /* Test1: Set a NULL preview text  */
  hildon_font_selection_dialog_set_preview_text(font_selection_dialog, NULL);
  ret_preview_text = hildon_font_selection_dialog_get_preview_text(font_selection_dialog);
  fail_if(ret_preview_text == NULL,
          "hildon-font-selection-dialog: Call to set_preview_text with a NULL text is allowed");
  g_free(ret_preview_text);

  /* Test2: Set an empty preview text  */
  hildon_font_selection_dialog_set_preview_text(font_selection_dialog, "");
  ret_preview_text = hildon_font_selection_dialog_get_preview_text(font_selection_dialog);
  fail_if(strcmp("", ret_preview_text) != 0,
          "hildon-font-selection-dialog: Call to set_preview_text with an empty text is allowed");
  g_free(ret_preview_text);
  
  /* Test3: Set with NULL object */
  hildon_font_selection_dialog_set_preview_text(NULL, PREVIEW_TEXT);

  /* Test4: Get with NULL object */
  hildon_font_selection_dialog_get_preview_text(NULL);
}
END_TEST

/* ----- Test case for set/get property "family" ----- */

/**
 * Purpose: Check handling of regular values
 * Cases considered:
 *    - Set and get a font from the middle of the available fonts list
 */
START_TEST (test_set_get_property_family_regular)
{
  PangoFontFamily **families;
  gint n_families;
  GValue value = {0,};
  GValue ret_value = {0,};

  helper_get_available_fonts(font_selection_dialog, &families, &n_families);
  fail_if (n_families <= 0, "hildon-font-selection-dialog: No available fonts");

  /* Test1: set and get a font family */
  g_value_init(&value, G_TYPE_STRING);
  g_value_init(&ret_value, G_TYPE_STRING); 
  g_value_set_string(&value, pango_font_family_get_name(families[n_families/2]));
  g_object_set_property(G_OBJECT(font_selection_dialog), "family", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family", &ret_value);
  fail_if(strcmp(g_value_get_string(&value), g_value_get_string(&ret_value)) != 0,
	  "hildon-font-selection-dialog: set property \"family\" to value \"%s\", but get property returned value \"%s\"",
	  g_value_get_string(&value), g_value_get_string(&ret_value));
}
END_TEST

/**
 * Purpose: Check handling of limit values
 * Cases considered:
 *    - Set and get first font of the available fonts list
 *    - Set and get last font of the available fonts list
 */
START_TEST (test_set_get_property_family_limits)
{
  PangoFontFamily **families;
  gint n_families;
  GValue value = {0,};
  GValue ret_value = {0,};
 
  helper_get_available_fonts(font_selection_dialog, &families, &n_families);
  fail_if (n_families <= 0, "hildon-font-selection-dialog: No available fonts");

  /* Test1: Test first font in the list */
  g_value_init(&value, G_TYPE_STRING);
  g_value_init(&ret_value, G_TYPE_STRING); 
  g_value_set_string(&value, pango_font_family_get_name(families[0]));
  g_object_set_property(G_OBJECT(font_selection_dialog), "family", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family", &ret_value);
  fail_if(strcmp(g_value_get_string(&value), g_value_get_string(&ret_value)) != 0,
	  "hildon-font-selection-dialog: set property \"family\" to value \"%s\", but get property returned value \"%s\"",
	  g_value_get_string(&value), g_value_get_string(&ret_value));     
      
  /* Test2: Test last font in the list */
  g_value_set_string(&value, pango_font_family_get_name(families[n_families-1]));
  g_object_set_property(G_OBJECT(font_selection_dialog), "family", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family", &ret_value);
  fail_if(strcmp(g_value_get_string(&value), g_value_get_string(&ret_value)) != 0,
	  "hildon-font-selection-dialog: set property \"family\" to value \"%s\", but get property returned value \"%s\"",
	  g_value_get_string(&value), g_value_get_string(&ret_value));   
}
END_TEST

/**
 * Purpose: Check handling of invalid values
 * Cases considered:
 *    - Set a non existing font
 *    - Set a NULL font
 */
START_TEST (test_set_get_property_family_invalid)
{
  GValue value = {0,};
  GValue ret_value = {0,};

  /* Test1: Set non existing font */
  g_value_init(&value, G_TYPE_STRING);
  g_value_init(&ret_value, G_TYPE_STRING); 
  g_value_set_string(&value, "-- invalid font name --");
  g_object_set_property(G_OBJECT(font_selection_dialog), "family", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family", &ret_value);
  fail_if(strcmp(DEFAULT_FONT_FAMILY, g_value_get_string(&ret_value)) != 0,
          "hildon-font-selection-dialog: set property \"family\" to invalid font name \"%s\", but get property returned value \"%s\" instead of \"%s\"",
          g_value_get_string(&value), g_value_get_string(&ret_value), DEFAULT_FONT_FAMILY);   
  
  /* Test2: Set NULL font */
  g_value_reset (&ret_value);
  g_value_reset (&value);
  g_value_set_string(&value, NULL);
  g_object_set_property(G_OBJECT(font_selection_dialog), "family", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family", &ret_value);
  fail_if(strcmp(DEFAULT_FONT_FAMILY, g_value_get_string(&ret_value)) != 0,
          "hildon-font-selection-dialog: set property \"family\" to NULL, but get property returned value \"%s\" instead of \"%s\"",
          g_value_get_string(&ret_value), DEFAULT_FONT_FAMILY);
}
END_TEST

/* ----- Test case for set/get property "family-set" ----- */

/**
 * Purpose: Check set and get of valid values
 * Cases considered:
 *    - Get returns FALSE when no font is selected.
 *    - Get returns TRUE when a font has been selected.
 *    - Set and get of value TRUE once a font has been selected
 *    - Set and get of value FALSE 
 */
START_TEST (test_set_get_property_family_set_regular)
{
  PangoFontFamily **families;
  gint n_families;
  GValue value = {0,};
  GValue ret_value = {0,}; 
  GValue family_value = {0,};

  helper_get_available_fonts(font_selection_dialog, &families, &n_families);
  fail_if (n_families <= 0, "hildon-font-selection-dialog: No available fonts");

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_init(&ret_value, G_TYPE_BOOLEAN);
  g_value_init(&family_value, G_TYPE_STRING);

  /* Test1: Check get value returns FALSE when no font is selected */
  g_object_get_property(G_OBJECT(font_selection_dialog), "family-set", &ret_value);
  if (g_value_get_boolean(&ret_value) == FALSE)
    {
      g_object_get_property(G_OBJECT(font_selection_dialog), "family", &family_value);
      fail_if(strcmp(DEFAULT_FONT_FAMILY, g_value_get_string(&family_value)) != 0,
	      "hildon-font-selection-dialog: After dialog creation, get value of property \"family-set\" returns FALSE, but get value of property \"family\" is \"%s\" instead of \"%s\"",
	      g_value_get_string(&family_value), DEFAULT_FONT_FAMILY);      
    }

  /* Test2: Check get value returns TRUE when a font has been selected */
  g_value_set_string(&family_value, pango_font_family_get_name(families[0]));
  g_object_set_property(G_OBJECT(font_selection_dialog), "family", &family_value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family-set", &ret_value);
  fail_if(g_value_get_boolean(&ret_value) != TRUE,
	  "hildon-font-selection-dialog: Set a valid font for property \"family\", but get property \"family-set\" returned FALSE");
      
  /* Test3: Check set value to TRUE */
  g_value_set_boolean(&value, TRUE);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family-set", &ret_value);
  fail_if(g_value_get_boolean(&ret_value) != TRUE,
	  "hildon-font-selection-dialog: Set property \"family-set\" to TRUE once a valid font has been selected, but get_property returned FALSE");

  /* Test4: Check set value to FALSE */
  g_value_set_boolean(&value, FALSE);
  g_object_set_property(G_OBJECT(font_selection_dialog), "family-set", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "family-set", &ret_value);
  fail_if(g_value_get_boolean(&value) != g_value_get_boolean(&ret_value),
	  "hildon-font-selection-dialog: Set property \"family-set\" to FALSE, but get_property returned TRUE");
  g_object_get_property(G_OBJECT(font_selection_dialog), "family", &family_value);
  fail_if(strcmp(DEFAULT_FONT_FAMILY, g_value_get_string(&family_value)) != 0,
	  "hildon-font-selection-dialog: Set property  \"family-set\" to FALSE, but get of propery \"family\" returns \"%s\" instead of \"%s\"",
	  g_value_get_string(&family_value), DEFAULT_FONT_FAMILY);
}
END_TEST

/* ----- Test case for set/get property "size" ----- */

/**
 * Purpose: Check set and get of valid values
 * Cases considered:
 *    - Set a font size from the middle of the available sizes list 
 */
START_TEST (test_set_get_property_size_regular)
{
  gint font_size;
  GValue value = {0,};
  GValue ret_value = {0,};

  g_value_init(&value, G_TYPE_INT);
  g_value_init(&ret_value, G_TYPE_INT);

  /* Test1: set a valid font size */
  font_size = FONT_SIZES[N_FONT_SIZES/2];
  g_value_set_int(&value, font_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "size", &ret_value);
  fail_if(g_value_get_int(&ret_value) != font_size,
          "hildon-font-selection-dialog: Set property \"size\" to %d, but get property returned \"%d\"",
          font_size, g_value_get_int(&ret_value));
}
END_TEST

/**
 * Purpose: Check set and get of limit values
 * Cases considered:
 *    - Set first font size in the available sizes list
 *    - Set last font size in the available sizes list
 */
START_TEST (test_set_get_property_size_limits)
{
  gint font_size;
  GValue value = {0,};
  GValue ret_value = {0,};

  g_value_init(&value, G_TYPE_INT);
  g_value_init(&ret_value, G_TYPE_INT);

  /* Test1: set first font size */
  font_size = FONT_SIZES[0];
  g_value_set_int(&value, font_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "size", &ret_value);
  fail_if(g_value_get_int(&ret_value) != font_size,
          "hildon-font-selection-dialog: Set property \"size\" to %d, but get property returned \"%d\"",
          font_size, g_value_get_int(&ret_value));

  /* Test2: set last font size */
  font_size = FONT_SIZES[N_FONT_SIZES-1];
  g_value_set_int(&value, font_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "size", &ret_value);
  fail_if(g_value_get_int(&ret_value) != font_size,
          "hildon-font-selection-dialog: Set property \"size\" to %d, but get property returned \"%d\"",
          font_size, g_value_get_int(&ret_value));
}
END_TEST

/**
 * Purpose: Check set and get of invalid values
 * Cases considered:
 *    - Set negative size
 *    - Set a positive size that is not in the available sizes list
 *    - Set a positive size over the maximum allowed for the property
 */
START_TEST (test_set_get_property_size_invalid)
{
  gint font_size;
  gint default_size;
  GValue value = {0,};
  GValue ret_value = {0,};

  g_value_init(&value, G_TYPE_INT);
  g_value_init(&ret_value, G_TYPE_INT);

  default_size = 8;
  g_value_set_int(&value, default_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);

  /* Test1: Set negative value */
  font_size = -5;
  g_value_set_int(&value, font_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "size", &ret_value);
  fail_if(g_value_get_int(&ret_value) != default_size,
          "hildon-font-selection-dialog: Set property \"size\" to invalid %d, but get property returned \"%d\" instead of previous valid value \"%d\"",
          font_size, g_value_get_int(&ret_value), default_size);  

  /* Test2: Set a positive that is not in the available list */
  font_size = 17;
  g_value_set_int(&value, font_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "size", &ret_value);
  fail_if(g_value_get_int(&ret_value) != default_size,
          "hildon-font-selection-dialog: Set property \"size\" to invalid %d, but get property returned \"%d\" instead of previous valid value \"%d\"",
          font_size, g_value_get_int(&ret_value), default_size);

  /* Test3: Set a positive value over the maximum allowed */
  font_size = 112;
  g_value_set_int(&value, font_size);
  g_object_set_property(G_OBJECT(font_selection_dialog), "size", &value);
  g_object_get_property(G_OBJECT(font_selection_dialog), "size", &ret_value);
  fail_if(g_value_get_int(&ret_value) != default_size,
          "hildon-font-selection-dialog: Set property \"size\" to invalid %d, but get property returned \"%d\" instead of previous valid value \"%d\"",
          font_size, g_value_get_int(&ret_value), default_size);
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_font_selection_dialog_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonFontSelectionDialog");

  /* Create test cases */
  TCase *tc1 = tcase_create("set_get_preview_text");
  TCase *tc2 = tcase_create("set_get_property_family");
  TCase *tc3 = tcase_create("set_get_property_family_set");
  TCase *tc4 = tcase_create("set_get_property_size");

  /* Create test case for set/get_preview_text and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_font_selection_dialog, 
                            fx_teardown_default_font_selection_dialog);
  tcase_add_test(tc1, test_set_get_preview_text_regular);
  tcase_add_test(tc1, test_set_get_preview_text_invalid);
  suite_add_tcase(s, tc1);

  /* Create test case for set/get property "family" and add it to the suite */
  tcase_add_checked_fixture(tc2, fx_setup_default_font_selection_dialog,
                            fx_teardown_default_font_selection_dialog);
  tcase_add_test(tc2, test_set_get_property_family_regular);
  tcase_add_test(tc2, test_set_get_property_family_limits);
  tcase_add_test(tc2, test_set_get_property_family_invalid);
  suite_add_tcase(s, tc2);

  /* Create test case for set/get property "family-set" and add it to the suite */
  tcase_add_checked_fixture(tc3, fx_setup_default_font_selection_dialog,
                            fx_teardown_default_font_selection_dialog);
  tcase_add_test(tc3, test_set_get_property_family_set_regular);
  suite_add_tcase(s, tc3);

  /* Create test case for set/get property "size" and add it to the suite */
  tcase_add_checked_fixture(tc4, fx_setup_default_font_selection_dialog,
                            fx_teardown_default_font_selection_dialog);
  tcase_add_test(tc4, test_set_get_property_size_regular);
  tcase_add_test(tc4, test_set_get_property_size_limits);
  tcase_add_test(tc4, test_set_get_property_size_invalid);
  suite_add_tcase(s, tc4);

  /* Return created suite */
  return s;             
}
