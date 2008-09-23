/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version. or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <glib.h>
#include <gtk/gtk.h>

#include "hildon.h"

static const gchar* artists [] = {
  "AC/DC",
  "Aerosmith",
  "Alice in Chains",
  "Black Sabbath",
  "Carcass",
  "Danzig",
  "Deep Purple",
  "Dream Theater",
  "Eric Clapton",
  "Focus",
  "Frank Gambale",
  "Guns n' Roses",
  "Gus Gus",
  "Hearth",
  "Helloween",
  "Hole",
  "Jerry Cantrell",
  "Jethro Tull",
  "Jimi Hendrix",
  "Jim Morrison",
  "Joe Satriani",
  "John Lennon",
  "King Diamond",
  "Lacuna Coil",
  "Led Zeppelin",
  "Lynyrd Skynyrd",
  "Metallica",
  "Megadeth",
  "Nirvana",
  "Nine Inch Nails",
  "No Doubt",
  NULL
};

int
main (int argc, char **args)
{
  HildonProgram *program = NULL;
  GtkWidget *window = NULL;
  GtkWidget *button;
  GtkWidget *selector;
  gint i;

  gtk_init (&argc, &args);

  program = hildon_program_get_instance ();
  g_set_application_name
    ("hildon-touch-picker cell renderer example program");

  window = hildon_stackable_window_new ();
  hildon_program_add_window (program, HILDON_WINDOW (window));

  gtk_container_set_border_width (GTK_CONTAINER (window), 6);

  button = hildon_picker_button_new (HILDON_SIZE_AUTO, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  hildon_button_set_title (HILDON_BUTTON (button), "Pick a band!");
  selector = hildon_touch_selector_entry_new_text ();
  for (i = 0; artists [i] != NULL; i++) {
    hildon_touch_selector_append_text (HILDON_TOUCH_SELECTOR (selector),
                                       artists[i]);
  }
  hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (button),
                                     HILDON_TOUCH_SELECTOR (selector));

  gtk_container_add (GTK_CONTAINER (window), button);
  g_signal_connect (G_OBJECT (window), "destroy",
                    G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (GTK_WIDGET (window));

  gtk_main ();

  return 0;
}
