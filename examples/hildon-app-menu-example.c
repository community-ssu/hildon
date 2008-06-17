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
#include                                        <hildon-app-menu.h>

static void
menu_button_clicked                             (GtkButton *button,
                                                 GtkLabel *label)
{
    const char *buttontext = gtk_button_get_label (button);
    char *text = g_strdup_printf("Last button clicked:\n%s", buttontext);
    gtk_label_set_text (label, text);
    g_free (text);
}

static HildonAppMenu *
create_menu                                     (GtkWidget *label)
{
    GtkWidget *button;
    GtkWidget *group;
    HildonAppMenu *menu = HILDON_APP_MENU (hildon_app_menu_new ());

    /* Options */
    button = gtk_button_new_with_label ("Menu command one");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_append (menu, GTK_BUTTON (button));

    button = gtk_button_new_with_label ("Menu command two");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_append (menu, GTK_BUTTON (button));

    button = gtk_button_new_with_label ("Menu command three");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_append (menu, GTK_BUTTON (button));

    button = gtk_button_new_with_label ("Menu command four");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_append (menu, GTK_BUTTON (button));

    button = gtk_button_new_with_label ("Menu command five");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_append (menu, GTK_BUTTON (button));

    /* Filters */
    button = gtk_radio_button_new_with_label (NULL, "filter one");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_add_filter (menu, GTK_BUTTON (button));
    gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (button), FALSE);

    button = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (button), "filter two");
    g_signal_connect (button, "clicked", G_CALLBACK (menu_button_clicked), label);
    hildon_app_menu_add_filter (menu, GTK_BUTTON (button));
    gtk_toggle_button_set_mode (GTK_TOGGLE_BUTTON (button), FALSE);

    return menu;
}

static void
button_clicked                                  (GtkButton *button,
                                                 HildonAppMenu *menu)
{
    gtk_widget_show (GTK_WIDGET (menu));
}

static void
close_app                                       (GtkWidget *widget,
                                                 GdkEvent  *event,
                                                 GtkWidget *menu)
{
    gtk_widget_destroy (GTK_WIDGET (menu));
    gtk_main_quit ();
}

int
main                                            (int argc,
                                                 char **argv)
{
    GtkWidget *win;
    GtkWidget *button;
    GtkWidget *label;
    GtkWidget *label2;
    GtkBox *vbox;
    HildonAppMenu *menu;

    gtk_init (&argc, &argv);

    gtk_rc_parse_string ("style \"default\" {\n"
                         "bg[NORMAL] = \"#505050\""
                         "}\n"
                         "class \"HildonAppMenu\" style \"default\"\n");

    button = gtk_button_new_with_label ("Press me");
    label = gtk_label_new ("This is an example of the\nHildonAppMenu widget");
    label2 = gtk_label_new ("No button has been clicked");
    vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));
    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    menu = create_menu (label2);

    gtk_box_pack_start (vbox, label, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, button, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, label2, TRUE, TRUE, 0);

    gtk_container_set_border_width (GTK_CONTAINER (win), 20);
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (vbox));

    g_signal_connect (button, "clicked", G_CALLBACK(button_clicked), menu);
    g_signal_connect (win, "delete_event", G_CALLBACK(close_app), menu);

    gtk_widget_show_all (win);

    gtk_main ();

    return 0;
}
