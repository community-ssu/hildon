/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Author: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
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

#include                                        <stdio.h>
#include                                        <stdlib.h>
#include                                        <glib.h>
#include                                        <gtk/gtk.h>
#include                                        "hildon.h"

void
on_mute_clicked                                 (GtkWidget *clicked, 
                                                 GObject *bar);

void
on_mute_clicked                                 (GtkWidget *clicked, 
                                                 GObject *bar)
{
    gboolean focus;
    g_object_get (bar, "mute", &focus, NULL);
    g_object_set (bar, "mute", !focus, NULL);
}

int
main                                            (int argc,
                                                 char **argv)
{
    hildon_gtk_init (&argc, &argv);


    GtkDialog *dialog = GTK_DIALOG (gtk_dialog_new ());
    GtkButton *button = GTK_BUTTON (gtk_button_new_with_label ("mute"));

    HildonHVolumebar *bar = HILDON_HVOLUMEBAR (hildon_hvolumebar_new ());
    gtk_widget_set_size_request (GTK_WIDGET (bar), 400, -1);

    gtk_box_pack_start (GTK_BOX (dialog->vbox), GTK_WIDGET (bar), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (dialog->vbox), GTK_WIDGET (button), FALSE, FALSE, 0);
    gtk_dialog_add_button (dialog, "Close", GTK_RESPONSE_CLOSE);

    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_mute_clicked), bar);

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);

    return 0;
}


