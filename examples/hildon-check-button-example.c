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

#include                                        <hildon/hildon.h>

static void
button_toggled_cb                               (HildonCheckButton *button,
                                                 GtkLabel          *label)
{
    gboolean active = hildon_check_button_get_active (button);
    const gchar *labeltext = gtk_button_get_label (GTK_BUTTON (button));
    char *text = g_strconcat (labeltext, active ? " (checked)" : " (unchecked)", NULL);
    gtk_label_set_text (label, text);
    g_free (text);
}

int
main                                            (int    argc,
                                                 char **argv)
{
    GtkWidget *win;
    GtkBox *vbox;
    GtkWidget *label;
    GtkWidget *table;
    int i;

    hildon_gtk_init (&argc, &argv);

    win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    vbox = GTK_BOX (gtk_vbox_new (FALSE, 10));
    table = gtk_table_new (3, 2, TRUE);
    label = gtk_label_new ("none");

    gtk_table_set_row_spacings (GTK_TABLE (table), 10);
    gtk_table_set_col_spacings (GTK_TABLE (table), 10);

    gtk_box_pack_start (vbox, gtk_label_new ("Hildon check button example"), TRUE, TRUE, 0);

    for (i = 0; i < 6; i++) {
        char *text;
        GtkWidget *button = hildon_check_button_new (HILDON_SIZE_HALFSCREEN_WIDTH | HILDON_SIZE_FINGER_HEIGHT);
        text = g_strdup_printf ("Button %d", i+1);
        gtk_button_set_label (GTK_BUTTON (button), text);
        g_free (text);
        gtk_table_attach_defaults (GTK_TABLE (table), button, i/2, (i/2) + 1, i%2, (i%2) + 1);
        g_signal_connect (button, "toggled", G_CALLBACK (button_toggled_cb), label);
    }

    gtk_box_pack_start (vbox, table, TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, gtk_label_new ("Last toggled:"), TRUE, TRUE, 0);
    gtk_box_pack_start (vbox, label, TRUE, TRUE, 0);

    gtk_container_set_border_width (GTK_CONTAINER (win), 20);
    gtk_container_add (GTK_CONTAINER (win), GTK_WIDGET (vbox));

    g_signal_connect (win, "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (win);

    gtk_main ();

    return 0;
}
