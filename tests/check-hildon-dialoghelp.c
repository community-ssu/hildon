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
#include <gtk/gtksignal.h>
#include <glib/gprintf.h>
#include "test_suites.h"
#include "check_utils.h"

#include <gdk/gdkx.h>
#include "hildon-dialoghelp.h"

/* -------------------- Fixtures -------------------- */
static GtkWidget * dialoghelp = NULL;

static void
change_help_status (GtkDialog *dialog, gpointer func_data)
{
  gboolean * data = func_data;
  *data = TRUE;
}

static void 
fx_setup_default_dialoghelp ()
{
  int argc = 0;
  gtk_init(&argc, NULL);
    
  dialoghelp = gtk_dialog_new();
  /* Check that the dialog help object has been created properly */
  fail_if(!GTK_IS_DIALOG (dialoghelp), "hildon-dialoghelp: Creation failed");

  /* Displays the widget */
  show_all_test_window (dialoghelp);

}

static void 
fx_teardown_default_dialoghelp ()
{
  gtk_widget_destroy(GTK_WIDGET(dialoghelp)); 
}

/* -------------------- Test cases -------------------- */

/* ----- Test case for enable_dialoghelp -----*/
/**
 * Purpose: test gtk_dialog_help_enable/gtk_dialog_help_disable connecting help signal 
 * Cases considered:
 *    - Test if gtk_dialog_help_enable actually enables help dialog status
 *    - Test if gtk_dialog_help_enable actually adds help atom to atoms' list
 *    - Test if gtk_dialog_help_disable actually removes help atom to atoms' list
 */
START_TEST (test_enable_dialoghelp_regular)
{
  gboolean help_enabled = FALSE;
  GdkWindow *window=NULL;
  GdkDisplay *display;
  Atom *list;
  Atom helpatom;
  int amount = 0;
  int i = 0;
  gboolean helpAtom = FALSE;
    
  gtk_dialog_help_enable(GTK_DIALOG(dialoghelp));    
  gtk_signal_connect(GTK_OBJECT(dialoghelp),"help",GTK_SIGNAL_FUNC(change_help_status),&help_enabled);   
  gtk_signal_emit_by_name(GTK_OBJECT(dialoghelp),"help");
    
  /* Test 1: Test if gtk_dialog_help_enable actually enables help dialog status */
  fail_if(help_enabled!=TRUE,"hildon-dialoghelp: Enable help failed");
        
  /* In order to obtain dialog->window. */
  gtk_widget_realize(GTK_WIDGET(dialoghelp));
  window = GTK_WIDGET(dialoghelp)->window;
  display = gdk_drawable_get_display (window);

  XGetWMProtocols(GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window),
                  &list, &amount);

  helpatom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");
    
  /* search if the help_atom is in the atoms' list */
  for (i=0; i<amount; i++) 
    {
      if (list[i] == helpatom) 
        {
          helpAtom = TRUE;
          break;
        }
    }
  XFree (list);

  /* Test 2: Test if gtk_dialog_help_enable actually adds help atom to atoms' list */
  fail_if(helpAtom!=TRUE,"hildon-dialoghelp: Help atom is not in the atoms' list");
    
    
  helpAtom = FALSE;
  gtk_dialog_help_disable(GTK_DIALOG(dialoghelp)); 
  XGetWMProtocols(GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window),
                  &list, &amount);
  helpatom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");
    
  /* search if the help_atom is in the atoms' list */
  for (i=0; i<amount; i++)
    {
      if (list[i] == helpatom)
        {
          helpAtom = TRUE;
          break;
        }
    }
  XFree (list);
  /* Test 3: Test if gtk_dialog_help_disable actually removes help atom to atoms' list */
  fail_if(helpAtom==TRUE,"hildon-dialoghelp: Help atom is in the atoms' list");  
}
END_TEST

/**
 * Purpose: test gtk_dialog_help_enable connecting help signal 
 * Cases considered:
 *    - Enable help dialog on NULL object.
 *    - Disable help dialog on NULL object.
 */
START_TEST (test_enable_dialoghelp_invalid)
{
  gtk_dialog_help_enable(NULL);
  gtk_dialog_help_disable(NULL);
}
END_TEST


/* ---------- Suite creation ---------- */

Suite *create_hildon_dialoghelp_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonDialogHelp");

  /* Create test cases */
  TCase *tc1 = tcase_create("dialog_help_enable");

  /* Create test case for gtk_dialog_help_enable and add it to the suite */
  tcase_add_checked_fixture(tc1, fx_setup_default_dialoghelp, fx_teardown_default_dialoghelp);
  tcase_add_test(tc1, test_enable_dialoghelp_regular);
  tcase_add_test(tc1, test_enable_dialoghelp_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;             
}
