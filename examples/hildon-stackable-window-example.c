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

#include                                        <stdio.h>
#include                                        <stdlib.h>
#include                                        <glib.h>
#include                                        <gtk/gtk.h>
#include                                        "hildon.h"

#include                                        <X11/X.h>
#include                                        <X11/Xlib.h>
#include                                        <X11/Xatom.h>

static void
add_window                                      (GtkWidget* w);

static GtkWidget*
new_window                                      (gboolean ismain)
{
    GtkWidget *window, *hbbox, *add;
    static int count = 0;
    gchar* title;

    window = hildon_stackable_window_new ();

    if (count == 0)
        title = g_strdup ("main window");
    else
        title = g_strdup_printf ("win%d", count);

    gtk_window_set_title (GTK_WINDOW (window), title);
    g_free (title);

    count++;

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);

    hbbox = gtk_hbutton_box_new ();
    gtk_container_add (GTK_CONTAINER (window), hbbox);

    add = gtk_button_new_with_label ("Add a window");
    gtk_box_pack_start (GTK_BOX (hbbox), add, FALSE, FALSE, 0);

    g_signal_connect (G_OBJECT (add), "clicked", G_CALLBACK (add_window), NULL);

    if (!ismain)
    {
        GtkWidget *detach, *back;
        detach = GTK_WIDGET (gtk_button_new_with_label ("Destroy"));
        gtk_box_pack_end (GTK_BOX (hbbox), detach, FALSE, FALSE, 0);

        g_signal_connect_swapped (G_OBJECT (detach), "clicked",
                                  G_CALLBACK (gtk_widget_destroy),
                                  HILDON_STACKABLE_WINDOW (window));

        back = GTK_WIDGET (gtk_button_new_with_label ("Back to root"));
        gtk_box_pack_end (GTK_BOX (hbbox), back, FALSE, FALSE, 0);

        g_signal_connect_swapped (G_OBJECT (back), "clicked",
                                  G_CALLBACK (hildon_program_go_to_root_window),
                                  hildon_program_get_instance ());
    }

    return window;
}

static void
add_window                                      (GtkWidget *w)
{
    GtkWidget *window = new_window (FALSE);
    gtk_widget_show_all (window);

    return;
}

int
main                                            (int argc,
                                                 char **args)
{
    GtkWidget *window;

    gtk_init (&argc, &args);

    g_set_application_name ("stack");

    window = new_window (TRUE);

    g_signal_connect (G_OBJECT (window), "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show_all (GTK_WIDGET (window));

    gtk_main ();

    return 0;
}
