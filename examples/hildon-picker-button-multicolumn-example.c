/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation; version 2 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 */

#include <glib.h>
#include <gtk/gtk.h>

#include "hildon-program.h"
#include "hildon-stackable-window.h"

#include "hildon-picker-button.h"

GtkWidget *
create_touch_selector (void)
{
  GtkWidget *selector;
  GtkListStore *model;
  GtkTreeIter iter;

  selector = hildon_touch_selector_new ();

  model = gtk_list_store_new (1, G_TYPE_STRING);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "IMAP", -1);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "IMAP4", -1);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "POP3", -1);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "Mailbox", -1);

  hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                            GTK_TREE_MODEL (model));

  model = gtk_list_store_new (1, G_TYPE_STRING);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "SMTP", -1);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "SMTPS", -1);

  gtk_list_store_append (model, &iter);
  gtk_list_store_set (model, &iter, 0, "Smoke signals", -1);

  hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                            GTK_TREE_MODEL (model));
  return selector;
}

static void
on_picker_value_changed (HildonPickerButton * button, gpointer data)
{
  g_print ("Newly selected value: %s\n",
           hildon_button_get_value (HILDON_BUTTON (button)));
}

int
main (int argc, char **args)
{
  HildonProgram *program = NULL;
  GtkWidget *window = NULL;
  GtkWidget *button;
  GtkWidget *selector;

  gtk_init (&argc, &args);

  program = hildon_program_get_instance ();
  g_set_application_name ("hildon-picker-button-example");

  window = hildon_stackable_window_new ();
  gtk_window_set_default_size (GTK_WINDOW (window), 300, 200);
  hildon_program_add_window (program, HILDON_WINDOW (window));
  selector = create_touch_selector ();

  button = hildon_picker_button_new (HILDON_BUTTON_WITH_VERTICAL_VALUE);
  hildon_button_set_title (HILDON_BUTTON (button), "Protocol");
  hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (button),
                                     HILDON_TOUCH_SELECTOR (selector));
  g_signal_connect (G_OBJECT (button), "value-changed",
                    G_CALLBACK (on_picker_value_changed), NULL);

  gtk_container_add (GTK_CONTAINER (window), button);

  gtk_widget_show_all (window);

  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (GTK_WIDGET (window));

  gtk_main ();

  return 0;
}
