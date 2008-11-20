/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Based in hildon-pannable-area-example.c
 * by Karl Lattimer <karl.lattimer@nokia.com>
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

#include                                        "hildon.h"

GtkWidget *btn;

static void
on_button_clicked (GtkWidget *widget, gpointer data)
{
    g_debug ("Button %d clicked", GPOINTER_TO_INT (data));
    btn = widget;
}

static void
find_button_clicked (GtkButton *button,
		     gpointer user_data)
{
	HildonPannableArea *panarea;

	panarea = HILDON_PANNABLE_AREA (user_data);

	hildon_pannable_area_scroll_to_child (panarea, btn);
}

int
main (int argc, char **args)
{
    int i;
    HildonProgram *program;
    GtkWidget *window, *panarea, *button;
    GtkWidget *hbox, *vbox;

    gtk_init (&argc, &args);

    program = hildon_program_get_instance ();

    /* Create the main window */
    window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));
    gtk_container_set_border_width (GTK_CONTAINER (window), 5);

    /* Create a VBox and pack some buttons */
    vbox = gtk_vbox_new (FALSE, 1);
    for (i = 0; i < 80; i++) {
            gchar *label = g_strdup_printf ("Button number %d", i);

            button = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
            gtk_button_set_label (GTK_BUTTON (button), label);
            gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);
            g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_button_clicked), GINT_TO_POINTER (i));
            g_free (label);
    }

    /* Put everything in a pannable area */
    panarea = hildon_pannable_area_new ();
    hildon_pannable_area_add_with_viewport(HILDON_PANNABLE_AREA (panarea), GTK_WIDGET (vbox));

    vbox = gtk_vbox_new (FALSE, 10);
    hbox = gtk_hbox_new (FALSE, 10);

    button = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (button), "Find the latest clicked button");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (find_button_clicked), panarea);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 10);
    gtk_box_pack_start (GTK_BOX (vbox), panarea, TRUE, TRUE, 0);

    gtk_container_add (GTK_CONTAINER (window), vbox);

    gtk_widget_show_all (GTK_WIDGET (window));

    gtk_main ();

    return 0;
}
