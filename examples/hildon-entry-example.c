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

#include                                        <gtk/gtk.h>
#include                                        <hildon.h>

static void
reset_entry                                     (HildonEntry *entry)
{
    hildon_entry_set_placeholder (entry, "Placeholder text");
}

static void
text_changed                                    (HildonEntry *entry,
                                                 GParamSpec  *arg1,
                                                 GtkLabel    *label)
{
    /* Do *NOT* use gtk_entry_get_text () */
    const gchar *text = hildon_entry_get_text (entry);

    if (text != NULL && *text != '\0') {
        gtk_label_set_text (label, text);
    } else {
        gtk_label_set_text (label, "(empty)");
    }
}

int
main                                            (int    argc,
                                                 char **argv)
{
    GtkWidget *win;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *button;
    GtkWidget *label3;
    GtkWidget *label2;
    GtkBox *vbox;

    gtk_init (&argc, &argv);

    gtk_rc_parse_string ("style \"placeholder\" {\n"
                         "text[NORMAL] = \"#C03030\""
                         "}\n"
                         "widget \"*.hildon-entry-placeholder\" style \"placeholder\"\n");

    win = hildon_stackable_window_new ();
    vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));

    label = gtk_label_new ("HildonEntry example");
    entry = hildon_entry_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    button = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    label2 = gtk_label_new ("Contents of the entry:");
    label3 = gtk_label_new (NULL);

    gtk_button_set_label (GTK_BUTTON (button), "Reset entry");
    reset_entry (HILDON_ENTRY (entry));
    text_changed (HILDON_ENTRY (entry), NULL, GTK_LABEL (label3));

    gtk_box_pack_start (vbox, label, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, button, FALSE, FALSE, 0);
    gtk_box_pack_start (vbox, entry, FALSE, FALSE, 0);
    gtk_box_pack_start (vbox, label2, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, label3, TRUE, TRUE, 0);

    gtk_container_set_border_width (GTK_CONTAINER (win), 20);
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (vbox));

    g_signal_connect (win, "destroy", G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (entry, "notify::text", G_CALLBACK (text_changed), label3);
    g_signal_connect_swapped (button, "clicked", G_CALLBACK (reset_entry), entry);

    gtk_widget_show_all (win);

    gtk_main ();

    return 0;
}
