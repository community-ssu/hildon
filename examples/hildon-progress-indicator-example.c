/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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

HildonTextView *textview;
GtkTextBuffer *buffer;

static void
show_progress_button_clicked                    (GtkWindow   *window,
                                                 HildonEntry *entry)
{
    g_debug ("Showing progress indicator...");
    hildon_gtk_window_set_progress_indicator (window, 1);
}

static void
hide_progress_button_clicked                    (GtkWindow   *window,
                                                 HildonEntry *entry)
{
    g_debug ("Hiding progress indicator...");
    hildon_gtk_window_set_progress_indicator (window, 0);
}

int
main                                            (int    argc,
                                                 char **argv)
{
    GtkWidget *win;
    GtkWidget *showprogressbutton, *hideprogressbutton;
    GtkBox *vbox;

    hildon_gtk_init (&argc, &argv);

    /* Window and vbox to pack everything */
    win = hildon_stackable_window_new ();
    vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));

    /* Buttons */
    showprogressbutton = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (showprogressbutton), "Show progress indicator");

    hideprogressbutton = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (hideprogressbutton), "Hide progress indicator");

    /* Pack all widgets */
    gtk_box_pack_start (GTK_BOX (vbox), showprogressbutton, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hideprogressbutton, TRUE, TRUE, 0);

    gtk_container_set_border_width (GTK_CONTAINER (win), 20);
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (vbox));

    /* Connect signals */
    g_signal_connect (win, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (showprogressbutton, "clicked",
                      G_CALLBACK (show_progress_button_clicked), win);
    g_signal_connect (hideprogressbutton, "clicked",
                      G_CALLBACK (hide_progress_button_clicked), win);

    /* Run example */
    gtk_widget_show_all (win);
    gtk_main ();

    return 0;
}
