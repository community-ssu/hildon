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
#include                                        <unistd.h>

gpointer
new_thread_func                                 (gpointer data);

gpointer
new_thread_func                                 (gpointer data)
{
        HildonColorButton *button = (HildonColorButton *) data;

        sleep (3);


        if (hildon_color_button_get_is_popped (button)) {
                g_debug ("Dialog popped, trying to close it!");
                hildon_color_button_pop_up (button);
        } else {
                g_debug ("Dialog not popped, doing nothing");
        }

        return NULL;
}

int
main                                            (int argc, 
                                                 char **args)
{
    g_thread_init (NULL);
    gtk_init (&argc, &args);
   
    HildonProgram *program = hildon_program_get_instance ();

    GtkWidget *window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));    

    GtkWidget *button = hildon_color_button_new ();

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);
    
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_add (GTK_CONTAINER (window), button);
    gtk_widget_show_all (GTK_WIDGET (window));

    g_thread_create (new_thread_func, button, TRUE, NULL);
    
    gtk_main ();
    return 0;
}


