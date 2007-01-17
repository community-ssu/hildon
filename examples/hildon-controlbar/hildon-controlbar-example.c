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
 * the License.
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

int
main                                            (int argc, 
                                                 char **args)
{
    gtk_init (&argc, &args);
    
    HildonProgram *program = hildon_program_get_instance ();

    GtkWidget *window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));    

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);
    
    HildonControlbar *bar = HILDON_CONTROLBAR (hildon_controlbar_new ());
    hildon_controlbar_set_range (bar, 20, 120);
    hildon_controlbar_set_value (bar, 100);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (bar));
    gtk_widget_show_all (GTK_WIDGET (window));
    
    gtk_main ();
    return 0;
}


