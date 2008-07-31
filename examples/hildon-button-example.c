/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Author: Karl Lattimer <karl.lattimer@nokia.com>
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

#include                                        <gtk/gtk.h>
#include                                        <hildon-button.h>

static void
button_clicked_cb                               (HildonButton *button,
                                                 gpointer      data)
{
    g_debug ("Pressed button: %s", hildon_button_get_title (button));
}

static void
vertical_buttons_window                         (GtkButton *b,
                                                 GtkToggleButton *horizontal)
{
    GtkWidget *win;
    GtkWidget *button;
    GtkBox *hbox;
    GtkBox *vbox1;
    GtkBox *vbox2;
    GtkBox *vbox3;
    int i;
    HildonButtonFlags layout;

    /* Create window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 20);

    layout = gtk_toggle_button_get_active (horizontal) ?
            HILDON_BUTTON_WITH_HORIZONTAL_VALUE :
            HILDON_BUTTON_WITH_VERTICAL_VALUE;

    /* Create and pack boxes */
    hbox = GTK_BOX (gtk_hbox_new (FALSE, 10));
    vbox1 = GTK_BOX (gtk_vbox_new (FALSE, 10));
    vbox2 = GTK_BOX (gtk_vbox_new (FALSE, 10));
    vbox3 = GTK_BOX (gtk_vbox_new (FALSE, 10));

    gtk_box_pack_start (hbox, GTK_WIDGET (vbox1), TRUE, TRUE, 0);
    gtk_box_pack_start (hbox, GTK_WIDGET (vbox2), TRUE, TRUE, 0);
    gtk_box_pack_start (hbox, GTK_WIDGET (vbox3), TRUE, TRUE, 0);

    /* Finger buttons */
    gtk_box_pack_start (vbox1, gtk_label_new ("Finger height"), FALSE, FALSE, 0);
    for (i = 0; i < 4; i++) {
        char *title = g_strdup_printf ("Title %d", i);
        button = hildon_button_new_with_text (layout |
                                              HILDON_BUTTON_FINGER_HEIGHT, title,
                                              i % 2 ? "Value" : NULL);
        g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);
        gtk_box_pack_start (vbox1, button, FALSE, FALSE, 0);
        g_free (title);
    }

    /* Thumb buttons */
    gtk_box_pack_start (vbox2, gtk_label_new ("Thumb height"), FALSE, FALSE, 0);
    for (i = 0; i < 3; i++) {
        char *title = g_strdup_printf ("Title %d", i);
        button = hildon_button_new_with_text (layout |
                                              HILDON_BUTTON_THUMB_HEIGHT, title,
                                              i % 2 ? "Value" : NULL);
        g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);
        gtk_box_pack_start (vbox2, button, FALSE, FALSE, 0);
        g_free (title);
    }

    /* Auto buttons */
    gtk_box_pack_start (vbox3, gtk_label_new ("Auto height"), FALSE, FALSE, 0);
    for (i = 0; i < 6; i++) {
        char *title = g_strdup_printf ("Title %d", i);
        button = hildon_button_new_with_text (layout, title,
                                              i % 2 ? "Value" : NULL);
        g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);
        gtk_box_pack_start (vbox3, button, FALSE, FALSE, 0);
        g_free (title);
    }

    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (hbox));

    g_signal_connect (win, "delete_event", G_CALLBACK (gtk_widget_destroy), NULL);

    gtk_widget_show_all (win);
}

static void
horizontal_buttons_window                       (GtkButton *b,
                                                 GtkToggleButton *horizontal)
{
    GtkWidget *win;
    GtkWidget *button;
    GtkBox *vbox;
    GtkBox *hbox1;
    GtkBox *hbox2;
    GtkBox *hbox3;
    GtkBox *hbox4;
    HildonButtonFlags layout;

    /* Create window */
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width (GTK_CONTAINER (win), 20);

    layout = gtk_toggle_button_get_active (horizontal) ?
            HILDON_BUTTON_WITH_HORIZONTAL_VALUE :
            HILDON_BUTTON_WITH_VERTICAL_VALUE;

    /* Create and pack boxes */
    vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));
    hbox1 = GTK_BOX (gtk_hbox_new (FALSE, 10));
    hbox2 = GTK_BOX (gtk_hbox_new (FALSE, 10));
    hbox3 = GTK_BOX (gtk_hbox_new (FALSE, 10));
    hbox4 = GTK_BOX (gtk_hbox_new (FALSE, 10));

    gtk_box_pack_start (vbox, gtk_label_new ("(all buttons have finger height)"), FALSE, FALSE, 0);
    gtk_box_pack_start (vbox, GTK_WIDGET (hbox1), FALSE, FALSE, 0);
    gtk_box_pack_start (vbox, GTK_WIDGET (hbox2), FALSE, FALSE, 0);
    gtk_box_pack_start (vbox, GTK_WIDGET (hbox3), FALSE, FALSE, 0);
    gtk_box_pack_start (vbox, GTK_WIDGET (hbox4), FALSE, FALSE, 0);

    /* Full screen width button */
    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_FULLSCREEN_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT, "Full width", "Value");
    gtk_box_pack_start (hbox1, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    /* Half screen width buttons */
    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_HALFSCREEN_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT, "Half width 1", "Value");
    gtk_box_pack_start (hbox2, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_HALFSCREEN_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT,
                                          "Half width 2 with long title",
                                          "Value");
    gtk_box_pack_start (hbox2, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    /* Half screen width buttons */
    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_HALFSCREEN_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT, "Half width 3", NULL);
    gtk_box_pack_start (hbox3, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_HALFSCREEN_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT,
                                          "Half width 4 with very long title (REALLY long)",
                                          "Value (title is truncated)");
    gtk_box_pack_start (hbox3, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    /* Auto width button */
    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_AUTO_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT,
                                          "Auto width 1", "Value");
    gtk_box_pack_start (hbox4, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    button = hildon_button_new_with_text (layout |
                                          HILDON_BUTTON_AUTO_WIDTH |
                                          HILDON_BUTTON_FINGER_HEIGHT,
                                          "Auto width 2 with longer text", NULL);
    gtk_box_pack_start (hbox4, button, TRUE, TRUE, 0);
    g_signal_connect (button, "clicked", G_CALLBACK (button_clicked_cb), NULL);

    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (vbox));

    g_signal_connect (win, "delete_event", G_CALLBACK (gtk_widget_destroy), NULL);

    gtk_widget_show_all (win);
}

int
main                                            (int    argc,
                                                 char **argv)
{
    GtkWidget *win;
    GtkWidget *but1;
    GtkWidget *but2;
    GtkWidget *label;
    GtkBox *vbox;
    GtkWidget *align;
    GtkWidget *horizontal_layout;


    gtk_init (&argc, &argv);

    vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));

    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    label = gtk_label_new ("HildonButton example");
    but1 = gtk_button_new_with_label ("Buttons with different heights");
    but2 = gtk_button_new_with_label ("Buttons with different widths");

    horizontal_layout = gtk_check_button_new_with_label ("Use horizontal layout");
    align = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (align), horizontal_layout);

    gtk_box_pack_start (vbox, label, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, but1, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, but2, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, align, FALSE, FALSE, 0);

    gtk_container_set_border_width (GTK_CONTAINER (win), 20);
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (vbox));

    g_signal_connect (but1, "clicked", G_CALLBACK (vertical_buttons_window), horizontal_layout);
    g_signal_connect (but2, "clicked", G_CALLBACK (horizontal_buttons_window), horizontal_layout);
    g_signal_connect (win, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (win);

    gtk_main ();

    return 0;
}
