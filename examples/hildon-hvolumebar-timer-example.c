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

gboolean
on_idle                                         (void);

GtkDialog*                                      dialog = NULL;

HildonHVolumebar*                               bar = NULL;

int                                             cycle = 0;

gboolean
on_idle                                         (void)
{

    if (dialog == NULL) {

        g_debug ("Creating the dialog!");
        
        dialog = GTK_DIALOG (gtk_dialog_new ());

        bar = HILDON_HVOLUMEBAR (hildon_hvolumebar_new ());
        gtk_widget_set_size_request (GTK_WIDGET (bar), 400, -1);
        hildon_helper_set_insensitive_message ((GtkWidget *) bar, "Insensitive");
        hildon_volumebar_set_range_insensitive_message (HILDON_VOLUMEBAR (bar), "Insensitive range");

        gtk_box_pack_start (GTK_BOX (dialog->vbox), GTK_WIDGET (bar), FALSE, FALSE, 0);
        gtk_dialog_add_button (dialog, "Close", GTK_RESPONSE_CLOSE);

        gtk_widget_show_all (GTK_WIDGET (dialog));
        gtk_dialog_run (dialog);
        gtk_widget_hide (GTK_WIDGET (dialog));
 
    } else {
        
        if (cycle == 0) {
                g_debug ("Making insensitive...");
                gtk_widget_set_sensitive (GTK_WIDGET (bar), FALSE);
        } else if (cycle == 1) {
                g_debug ("Making sensitive...");
                gtk_widget_set_sensitive (GTK_WIDGET (bar), TRUE);
        } else if (cycle == 2) { 
                g_debug ("Showing back...");
                gtk_widget_show (GTK_WIDGET (dialog));
                gtk_dialog_run (dialog);
                gtk_widget_hide (GTK_WIDGET (dialog));
        }
         
        cycle = (cycle + 1) % 3;
    }

    g_timeout_add (2000, (GSourceFunc) on_idle, NULL);
    return FALSE;
}

int
main                                            (int argc, 
                                                 char **argv)
{
    hildon_gtk_init (&argc, &argv);

    g_timeout_add (2000, (GSourceFunc) on_idle, NULL);

    gtk_main ();
    
    return 0;
}


