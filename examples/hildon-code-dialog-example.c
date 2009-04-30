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
on_input                                        (void);

gboolean
on_input                                        (void)
{
    g_debug ("Input in the code dialog!");
    return TRUE;
}

int
main                                            (int argc, 
                                                 char **argv)
{
    hildon_gtk_init (&argc, &argv);

    GtkDialog *dialog = GTK_DIALOG (hildon_code_dialog_new ());
    g_signal_connect (G_OBJECT (dialog), "input", G_CALLBACK (on_input), NULL);

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);
    
    return 0;
}


