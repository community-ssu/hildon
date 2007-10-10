/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
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

/**
 * SECTION:hildon-helper
 * @short_description: A collection of usefull utilities and functions.
 *
 * Hildon provides some helper functions that can be used for commonly 
 * performed tasks and functionality blocks. This includes operations 
 * on widget styles and probing functions for touch events.
 *
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        <gtk/gtk.h>
#include                                        "hildon-helper.h"
#include                                        "hildon-banner.h"

#define                                         HILDON_FINGER_PRESSURE_THRESHOLD 0.4

#define                                         HILDON_FINGER_BUTTON 8

#define                                         HILDON_FINGER_ALT_BUTTON 1

#define                                         HILDON_FINGER_ALT_MASK GDK_MOD4_MASK

#define                                         HILDON_FINGER_SIMULATE_BUTTON 2

struct                                          _HildonLogicalElement
{
    gboolean is_color;                          /* If FALSE, it's a logical font def */
    GtkRcFlags rc_flags;
    GtkStateType state;
    gchar *logical_color_name;
    gchar *logical_font_name;
} typedef                                       HildonLogicalElement;

static void
hildon_logical_element_list_free                (GSList *list)
{
    GSList *iterator = list;

    while (iterator) {
        HildonLogicalElement *element = (HildonLogicalElement *) iterator->data;

        g_free (element->logical_color_name);
        g_free (element->logical_font_name);
        g_slice_free (HildonLogicalElement, element);

        iterator = iterator->next;
    }

    /* Free the list itself */
    g_slist_free (list);
}

static GQuark
hildon_helper_logical_data_quark                (void)
{
    static GQuark quark = 0;

    if (G_UNLIKELY (quark == 0))
        quark = g_quark_from_static_string ("hildon-logical-data");

    return quark;
}

static HildonLogicalElement*
attach_blank_element                            (GtkWidget *widget, 
                                                 GSList **style_list)
{
    gboolean first = (*style_list == NULL) ? TRUE : FALSE;

    HildonLogicalElement *element = g_slice_new (HildonLogicalElement);
    
    element->is_color = FALSE;
    element->rc_flags = 0;
    element->state = 0;
    element->logical_color_name = NULL;
    element->logical_font_name = NULL;

    *style_list = g_slist_append (*style_list, element);

    if (first) 
        g_object_set_qdata_full (G_OBJECT (widget), hildon_helper_logical_data_quark (), *style_list, (GDestroyNotify) hildon_logical_element_list_free);

    return element;
}

static GSList*
attach_new_font_element                         (GtkWidget *widget, 
                                                 const gchar *font_name)
{
    GSList *style_list = g_object_get_qdata (G_OBJECT (widget), hildon_helper_logical_data_quark ());
    HildonLogicalElement *element = NULL;
   
    /* Try to find an element that already sets a font */
    GSList *iterator = style_list;
    while (iterator) {
        element = (HildonLogicalElement *) iterator->data;

        if (element->is_color == FALSE) {
            /* Reusing ... */
            g_free (element->logical_font_name);
            element->logical_font_name = g_strdup (font_name);
            return style_list;
        }

        iterator = iterator->next;
    }

    /* It was not found so we need to create a new one and attach it */
    element = attach_blank_element (widget, &style_list);
    element->is_color = FALSE;
    element->logical_font_name = g_strdup (font_name);
    return style_list;
}

static GSList*
attach_new_color_element                        (GtkWidget *widget, 
                                                 GtkRcFlags flags, 
                                                 GtkStateType state, 
                                                 const gchar *color_name)
{
    GSList *style_list = g_object_get_qdata (G_OBJECT (widget), hildon_helper_logical_data_quark ());
    HildonLogicalElement *element = NULL;
   
    /* Try to find an element that has same flags and state */
    GSList *iterator = style_list;
    while (iterator) {
        element = (HildonLogicalElement *) iterator->data;

        if (element->rc_flags == flags &&
            element->state == state &&
            element->is_color == TRUE) {
            /* Reusing ... */
            element->logical_color_name = g_strdup (color_name);
            return style_list;
        }

        iterator = iterator->next;
    }

    /* It was not found so we need to create a new one and attach it */
    element = attach_blank_element (widget, &style_list);
    element->is_color = TRUE;
    element->state = state;
    element->rc_flags = flags;
    element->logical_color_name = g_strdup (color_name);
    return style_list;
}

static void 
hildon_change_style_recursive_from_list         (GtkWidget *widget, 
                                                 GtkStyle *prev_style, 
                                                 GSList *list)
{
    g_assert (GTK_IS_WIDGET (widget));

    /* Change the style for child widgets */
    if (GTK_IS_CONTAINER (widget)) {
        GList *iterator, *children;
        children = gtk_container_get_children (GTK_CONTAINER (widget));
        for (iterator = children; iterator != NULL; iterator = g_list_next (iterator))
            hildon_change_style_recursive_from_list (GTK_WIDGET (iterator->data), prev_style, list);
        g_list_free (children);
    }

    /* gtk_widget_modify_*() emit "style_set" signals, so if we got here from
       "style_set" signal, we need to block this function from being called
       again or we get into inifinite loop.

    FIXME: Compiling with gcc > 3.3 and -pedantic won't allow
    conversion between function and object pointers. GLib API however
    requires an object pointer for a function, so we have to work
    around this.
    See http://bugzilla.gnome.org/show_bug.cgi?id=310175
    */

    G_GNUC_EXTENSION
        g_signal_handlers_block_matched (G_OBJECT (widget), G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC,
                g_signal_lookup ("style_set", G_TYPE_FROM_INSTANCE (widget)),
                0, NULL,
                (gpointer) hildon_change_style_recursive_from_list,
                NULL);

    /* We iterate over all list elements and apply each style
     * specification. */

    GSList *iterator = list;
    while (iterator) {
    
        HildonLogicalElement *element = (HildonLogicalElement *) iterator->data;

        if (element->is_color == TRUE) {

            /* Changing logical color */
            GdkColor color;
            gtk_widget_ensure_style (widget);
            if (gtk_style_lookup_color (widget->style, element->logical_color_name, &color) == TRUE) {
               
                switch (element->rc_flags)
                {
                    case GTK_RC_FG:
                        gtk_widget_modify_fg (widget, element->state, &color);
                        break;

                    case GTK_RC_BG:
                        gtk_widget_modify_bg (widget, element->state, &color);
                        break;

                    case GTK_RC_TEXT:
                        gtk_widget_modify_text (widget, element->state, &color);
                        break;

                    case GTK_RC_BASE:
                        gtk_widget_modify_base (widget, element->state, &color);
                        break;
                }
            }
        } else {
            
            /* Changing logical font */
            GtkStyle *font_style = gtk_rc_get_style_by_paths (gtk_settings_get_default (), element->logical_font_name, NULL, G_TYPE_NONE);
            if (font_style != NULL) {
                PangoFontDescription *font_desc = font_style->font_desc;

                if (font_desc != NULL)
                    gtk_widget_modify_font (widget, font_desc);
            }
        }

        iterator = iterator->next;
    } 

    /* FIXME: Compilation workaround for gcc > 3.3 + -pedantic again */

    G_GNUC_EXTENSION
        g_signal_handlers_unblock_matched (G_OBJECT (widget), G_SIGNAL_MATCH_ID | G_SIGNAL_MATCH_FUNC,
                g_signal_lookup ("style_set", G_TYPE_FROM_INSTANCE (widget)),
                0, NULL,
                (gpointer) hildon_change_style_recursive_from_list,
                NULL);
}

/**
 * hildon_helper_event_button_is_finger:
 * @event: A @gtkeventbutton to check
 *
 * Checks if the given button event is a finger event.
 * 
 * return value : TRUE if the event is a finger event.
 **/
gboolean 
hildon_helper_event_button_is_finger            (GdkEventButton *event)
{
    gdouble pressure;

    if (gdk_event_get_axis ((GdkEvent*) event, GDK_AXIS_PRESSURE, &pressure) &&
        pressure > HILDON_FINGER_PRESSURE_THRESHOLD)
        return TRUE;

    if (event->button == HILDON_FINGER_BUTTON)
        return TRUE;

    if (event->button == HILDON_FINGER_ALT_BUTTON &&
        event->state & HILDON_FINGER_ALT_MASK)
        return TRUE;

    if (event->button == HILDON_FINGER_SIMULATE_BUTTON)
        return TRUE;

    return FALSE;
}

/**
 * hildon_helper_set_logical_font:
 * @widget : a @gtkwidget to assign this logical font for.
 * @logicalfontname : a gchar* with the logical font name to assign to the widget.
 * 
 * This function assigns a defined logical font to the @widget and all its child widgets.
 * it also connects to the "style_set" signal which will retrieve & assign the new font 
 * for the given logical name each time the theme is changed.
 * The returned signal id can be used to disconnect the signal. 
 * When calling multiple times the previous signal (obtained by calling this function) is disconnected 
 * automatically and should not be used. 
 * 
 * return value : the signal id that is triggered every time theme is changed. 0 if font set failed.
 **/
gulong
hildon_helper_set_logical_font                  (GtkWidget *widget, 
                                                 const gchar *logicalfontname)
{
    gulong signum = 0;
    GSList *list;

    g_return_val_if_fail (GTK_IS_WIDGET (widget), 0);
    g_return_val_if_fail (logicalfontname != NULL, 0);

    list = attach_new_font_element (widget, logicalfontname);

    /* Disconnects the previously connected signals. That calls the closure notify
     * and effectively disposes the allocated data (hildon_logical_data_free) */
    g_signal_handlers_disconnect_matched (G_OBJECT (widget), G_SIGNAL_MATCH_FUNC, 
                                          0, 0, NULL, 
                                          G_CALLBACK (hildon_change_style_recursive_from_list), NULL);

    /* Change the font now */
    hildon_change_style_recursive_from_list (widget, NULL, list);

    /* Connect to "style_set" so that the font gets changed whenever theme changes. */
    signum = g_signal_connect_data (G_OBJECT (widget), "style_set",
                                    G_CALLBACK (hildon_change_style_recursive_from_list),
                                    list, NULL, 0);

    return signum;
}

static GQuark
hildon_helper_insensitive_message_quark         (void)
{
    static GQuark quark = 0;

    if (G_UNLIKELY (quark == 0))
        quark = g_quark_from_static_string ("hildon-insensitive-message");

    return quark;
}

static void
show_insensitive_message                        (GtkWidget *widget, 
                                                 gpointer user_data)
{
    gchar *message = NULL;

    g_assert (GTK_IS_WIDGET (widget));

    message = (gchar*) g_object_get_qdata (G_OBJECT (widget),
            hildon_helper_insensitive_message_quark ());

    if (message)
        hildon_banner_show_information (widget, NULL, message);
}


/**
 * hildon_helper_set_insensitive_message
 * @widget : A @GtkWidget to assign a banner to
 * @message: A message to display to the user
 *
 * This function assigns an insensitive message to a @widget. When the @widget is 
 * in an insensitive state and the user activates it, the @message will be displayed
 * using a standard @HildonBanner. 
 *
 **/
void
hildon_helper_set_insensitive_message           (GtkWidget *widget,
                                                 const gchar *message)
{
    g_return_if_fail (GTK_IS_WIDGET (widget));

    /* Clean up any previous instance of the insensitive message */
    g_signal_handlers_disconnect_matched (G_OBJECT (widget), G_SIGNAL_MATCH_FUNC,
					  0, 0, NULL,
					  G_CALLBACK (show_insensitive_message), NULL);
    
    /* We need to dup the string because the pointer might not be valid when the
     insensitive-press signal callback is executed */
    g_object_set_qdata_full (G_OBJECT (widget), hildon_helper_insensitive_message_quark (), 
			     (gpointer)g_strdup (message),
			     g_free);

    if (message != NULL) {
      g_signal_connect (G_OBJECT (widget), "insensitive-press",
			G_CALLBACK (show_insensitive_message), NULL);
    }
}

/**
 * hildon_helper_set_insensitive_messagef
 * @widget : A @GtkWidget to assign a banner to
 * @format : a printf-like format string
 * @varargs : arguments for the format string
 *
 * A version of hildon_helper_set_insensitive_message with string formatting.
 *
 **/
void
hildon_helper_set_insensitive_messagef          (GtkWidget *widget,
                                                 const gchar *format,
                                                 ...)
{
    g_return_if_fail (GTK_IS_WIDGET (widget));

    gchar *message;
    va_list args;

    va_start (args, format);
    message = g_strdup_vprintf (format, args);
    va_end (args);

    hildon_helper_set_insensitive_message (widget, message);

    g_free (message);
}

/**
 * hildon_helper_set_logical_color:
 * @widget : A @GtkWidget to assign this logical font for.
 * @rcflags : @GtkRcFlags enumeration defining whether to assign to FG, BG, TEXT or BASE style.
 * @state : @GtkStateType indicating to which state to assign the logical color
 * @logicalcolorname : A gchar* with the logical font name to assign to the widget.
 * 
 * This function assigns a defined logical color to the @widget and all it's child widgets.
 * It also connects to the "style_set" signal which will retrieve & assign the new color 
 * for the given logical name each time the theme is changed.
 * The returned signal id can be used to disconnect the signal.
 * When calling multiple times the previous signal (obtained by calling this function) is disconnected 
 * automatically and should not be used. 
 * 
 * Example : If the style you want to modify is bg[NORMAL] then set rcflags to GTK_RC_BG and state to GTK_STATE_NORMAL.
 * 
 * Return value : The signal id that is triggered every time theme is changed. 0 if color set failed.
 **/
gulong 
hildon_helper_set_logical_color                 (GtkWidget *widget, 
                                                 GtkRcFlags rcflags,
                                                 GtkStateType state, 
                                                 const gchar *logicalcolorname)
{
    gulong signum = 0;
    GSList *list = NULL;

    g_return_val_if_fail (GTK_IS_WIDGET (widget), 0);
    g_return_val_if_fail (logicalcolorname != NULL, 0);
    
    list = attach_new_color_element (widget, rcflags, state, logicalcolorname);

    /* Disconnects the previously connected signals. */
    g_signal_handlers_disconnect_matched (G_OBJECT (widget), G_SIGNAL_MATCH_FUNC, 
                                          0, 0, NULL, 
                                          G_CALLBACK (hildon_change_style_recursive_from_list), NULL);

    /* Change the colors now */
    hildon_change_style_recursive_from_list (widget, NULL, list);

    /* Connect to "style_set" so that the colors gets changed whenever theme */
    signum = g_signal_connect_data (G_OBJECT (widget), "style_set",
                                    G_CALLBACK (hildon_change_style_recursive_from_list),
                                    list, NULL, 0);

    return signum;
}


/**
 *
 * hildon_helper_set_thumb_scrollbar
 * @win: A @GtkScrolledWindow to use as target
 * @thumb: TRUE to enable the thumb scrollbar, FALSE to disable
 *
 * This function enables a thumb scrollbar on a given scrolled window. It'll convert the
 * existing normal scrollbar into a larger, finger-usable scrollbar that works without a stylus. 
 * As fingerable list rows are fairly high, consider using the whole available vertical space 
 * of your application for the content in order to have as many rows as possible 
 * visible on the screen at once. 
 *
 * Finger-Sized scrollbar should always be used together with finger-sized content.
 **/
void
hildon_helper_set_thumb_scrollbar               (GtkScrolledWindow *win, 
                                                 gboolean thumb)
{
    g_return_if_fail (GTK_IS_SCROLLED_WINDOW (win));

    if (win->vscrollbar) 
        gtk_widget_set_name (win->vscrollbar, (thumb) ? "hildon-thumb-scrollbar" : NULL);
}




