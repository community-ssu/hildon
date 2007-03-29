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

static gboolean
on_animation_idle                               (GtkWidget *banner)
{
    gtk_widget_destroy (banner);
    return FALSE;
}

static gboolean
on_progress_idle                                (GtkWidget *banner)
{
    gtk_widget_destroy (banner);
    return FALSE;
}

static gboolean
on_information_clicked                          (GtkWidget *widget)
{
    GtkWidget* banner = hildon_banner_show_information (widget, NULL, "Information banner"); 
    hildon_banner_set_timeout (HILDON_BANNER (banner), 9000);
    return TRUE;
}

static gboolean
on_animation_clicked                            (GtkWidget *widget)
{
    GtkWidget *banner = hildon_banner_show_animation (widget, NULL, "Animation banner"); 
    g_timeout_add (2000, (gpointer) on_animation_idle, banner);
    return TRUE;
}

static gboolean
on_progress_clicked                             (GtkWidget *widget)
{
    GtkWidget *banner = hildon_banner_show_progress (widget, NULL, "Progress banner"); 
    g_timeout_add (2000, (gpointer) on_progress_idle, banner);
    return TRUE;
}

int
main                                            (int argc, 
                                                 char **args)
{
    gtk_init (&argc, &args);
    
    HildonProgram *program = hildon_program_get_instance ();

    GtkWidget *window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));    

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);

    GtkVBox *vbox = GTK_VBOX (gtk_vbox_new (6, FALSE));
    GtkButton *button1 = GTK_BUTTON (gtk_button_new_with_label ("Information"));
    g_signal_connect (G_OBJECT (button1), "clicked", G_CALLBACK (on_information_clicked), NULL);

    GtkButton *button2 = GTK_BUTTON (gtk_button_new_with_label ("Animation"));
    g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (on_animation_clicked), NULL);

    GtkButton *button3 = GTK_BUTTON (gtk_button_new_with_label ("Progress"));
    g_signal_connect (G_OBJECT (button3), "clicked", G_CALLBACK (on_progress_clicked), NULL);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (button1), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (button2), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (button3), TRUE, TRUE, 0);
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (vbox));

    gtk_widget_show_all (GTK_WIDGET (window));
    
    gtk_main ();
    return 0;
}


