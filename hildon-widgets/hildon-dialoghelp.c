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

/*
 *  @file hildon-dialoghelp.c
 *
 *  This file provides API for the help dialog with
 *  the optional icon.
 *
 */

#include <gdk/gdkx.h>
#include "hildon-dialoghelp.h"

static guint help_signal = 0;

static GdkFilterReturn
handle_xevent(GdkXEvent * xevent, GdkEvent * event, gpointer data)
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
            g_signal_emit(G_OBJECT(data), help_signal, 0);
        }

        return GDK_FILTER_REMOVE;       /* Event handled, don't process
                                           further */
    }

    return GDK_FILTER_CONTINUE; /* Event not handled */
}

/**
 * gtk_dialog_help_enable:
 * @dialog: The dialog of which help is to be enabled.
 *
 * Enables context help button for given dialog. The signal "help" can be
 * connected to handler by normal gtk methods. Note that this function
 * has to be called before the dialog is shown.
 *
 * The "help" signal itself has no other parameters than the dialog where
 * it is connected to, ie.:
 * void user_function(GtkDialog *, gpointer user_data);
 **/
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

    XGetWMProtocols(GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window),
		    &list, &amount);
    
    protocols = (Atom *) malloc ((amount+1) * sizeof (Atom));
    helpatom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");

    for (i=0; i<amount; i++)
    {
	    protocols[n++] = list[i];
	    if (list[i] == helpatom)
	    {
		    help_enabled = 1;
	    }
    }
    XFree (list);

    if (!help_enabled)
    {
	    protocols[n++] = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");
    }
    
    XSetWMProtocols (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window), protocols, n);
    free (protocols);
    
    gdk_window_add_filter(window, handle_xevent, dialog);
}


/*
 * gtk_dialog_help_disable:
 * @dialog: The dialog of which help is to be disabled.
 * 
 */
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

    XGetWMProtocols(GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window),
		    &list, &amount);
    
    helpatom = gdk_x11_get_xatom_by_name_for_display (display, "_NET_WM_CONTEXT_HELP");
    protocols = (Atom *) malloc (amount * sizeof (Atom));

    for (i=0; i<amount; i++)
    {
	    if (list[i] != helpatom)
	    {
		    protocols[n++] = list[i];
	    }
    }
    XFree (list);
    
    XSetWMProtocols (GDK_DISPLAY_XDISPLAY (display), GDK_WINDOW_XID (window), protocols, n);
    free (protocols);

    gdk_window_remove_filter(window, handle_xevent, dialog);
}




