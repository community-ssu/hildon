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
#include                                        <hildon/hildon.h>

static gboolean
on_fire_clicked                                 (GtkWidget *widget)
{
    GtkWidget* banner = hildon_banner_show_information (widget, NULL, "Batterie chargee. Pour economiser l'energie, debrancher le chargeur du bloc d'alimentation."); 
    hildon_banner_set_timeout (HILDON_BANNER (banner), 9000);
    return TRUE;
}

int
main                                            (int argc, 
                                                 char **argv)
{
    hildon_gtk_init (&argc, &argv);

    HildonProgram *program = hildon_program_get_instance ();

    GtkWidget *window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));    

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);

    GtkButton *button = GTK_BUTTON (gtk_button_new_with_label ("Fire"));
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_fire_clicked), NULL);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (button));

    gtk_widget_show_all (GTK_WIDGET (window));
    
    gtk_main ();
    return 0;
}


