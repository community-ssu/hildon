/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
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

/**
 * SECTION:hildon-dialoghelp
 * @short_description: A helper which contains a button in a normal dialog to 
 * open a help if required
 * 
 * #HildonDialoghelp is a helper that provides an API for enabling or
 * disabling a help button in the titlebar area of normal dialogs that
 * are derived from GtkDialog. 
 */

#include <stdlib.h>
#include <gdk/gdkx.h>
#include "hildon-dialoghelp.h"

static guint help_signal = 0;

static GdkFilterReturn
handle_xevent(GdkXEvent * xevent, GdkEvent * event, gpointer dialog)
{
    XAnyEvent *eventti = xevent;

    if (eventti->type == ClientMessage) {
        Atom help_atom, wm_atom;
        Display *disp;
        XClientMessageEvent *cm;

        disp = GDK_DISPLAY();
        cm = xevent;

        help_atom = XInternAtom(disp, "_NET_WM_CONTEXT_HELP", True);
        wm_atom = XInternAtom(disp, "WM_PROTOCOLS", True);

        if (cm->message_type == wm_atom && cm->data.l[0] == help_atom) {
            /* XClientMessageEvent *cm = xevent; */
            g_signal_emit(G_OBJECT(dialog), help_signal, 0);
        }

        return GDK_FILTER_REMOVE;       /* Event handled, don't process
                                           further */
    }

    return GDK_FILTER_CONTINUE; /* Event not handled */
}

/**
 * gtk_dialog_help_enable:
 * @dialog: The dialog for which help is to be enabled.
 *
 * Enables context help button for a given dialog. The signal "help" can be
 * connected to handler by normal GTK methods. Note that this function
 * has to be called before the dialog is shown.
 *
 * The "help" signal itself has no other parameters than the dialog where
 * it is connected to, ie.:
 * void user_function(GtkDialog *dialog, gpointer user_data);
 */
void gtk_dialog_help_enable(GtkDialog * dialog)
{
    GdkWindow *window;
    GdkDisplay *display;
    Atom *protocols;
    Atom *list;
    Atom helpatom;
    int amount = 0;
    int n = 0;
    int i = 0;
    int help_enabled = 0;
    
    /* Create help signal if it didn't exist */   
    if (help_signal == 0) {
        help_signal = g_signal_new("help", GTK_TYPE_DIALOG,
                                   G_SIGNAL_ACTION, (guint) - 1, NULL,
                                   NULL, g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
    }

    g_return_if_fail(GTK_IS_DIALOG(dialog));

    gtk_widget_realize(GTK_WIDGET(dialog));
    window = GTK_WIDGET(dialog)->window;
    display = gdk_drawable_get_display (window);

    /* Create a list of atoms stored in GdkWindow */
    XGetWMProtocols(GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window),
		    &list, &amount);
    
    protocols = (Atom *) malloc ((amount+1) * sizeof (Atom));
    helpatom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");

    /* Enable the dialoghelp if help_atom is in the atoms' list */
    for (i=0; i<amount; i++)
    {
	    protocols[n++] = list[i];
	    if (list[i] == helpatom)
	    {
		    help_enabled = 1;
	    }
    }
    XFree (list);

    /* Add the help_atom to the atoms' list if it was not in it */ 
    if (!help_enabled)
    {
	    protocols[n++] = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");
    }
    
    /* Replace the protocol property of the GdkWindow with the new atoms' list */
    XSetWMProtocols (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window), protocols, n);
    free (protocols);
    
    /* Add a callback function as event filter */ 
    gdk_window_add_filter(window, handle_xevent, dialog);
}


/**
 * gtk_dialog_help_disable:
 * @dialog: The dialog for which help is to be disabled.
 *
 * Disables context help button for the given dialog.
 **/
void gtk_dialog_help_disable(GtkDialog * dialog)
{
    GdkWindow *window=NULL;
    GdkDisplay *display;
    Atom *protocols;
    Atom *list;
    Atom helpatom;
    int amount = 0;
    int n = 0;
    int i = 0;
    
    g_return_if_fail(GTK_IS_DIALOG(dialog));

    gtk_widget_realize(GTK_WIDGET(dialog));
    window = GTK_WIDGET(dialog)->window;
    display = gdk_drawable_get_display (window);

    /* Create a list of atoms stored in GdkWindow */
    XGetWMProtocols(GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window),
		    &list, &amount);
    
    helpatom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");
    protocols = (Atom *) malloc (amount * sizeof (Atom));

    /* Remove the help_atom if it is in the atoms' list */
    for (i=0; i<amount; i++)
    {
	    if (list[i] != helpatom)
	    {
		    protocols[n++] = list[i];
	    }
    }
    XFree (list);
    
    /* Replace the protocol property of the GdkWindow with the new atoms' list */
    XSetWMProtocols (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window), protocols, n);
    free (protocols);

    /* Remove the event filter */
    gdk_window_remove_filter(window, handle_xevent, dialog);
}




