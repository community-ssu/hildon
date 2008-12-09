/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
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
 * SECTION:hildon-dialog
 * @short_description: Widget representing a popup window in the Hildon framework.
 * @see_also: #HildonCodeDialog, #HildonColorChooserDialog, #HildonFontSelectionDialog, #HildonGetPasswordDialog, #HildonLoginDialog, #HildonSetPasswordDialog, #HildonSortDialog, #HildonWizardDialog
 *
 * #HildonDialog is a GTK widget which represent a popup window in the
 * Hildon framework. It is derived from #GtkDialog and provides additional
 * commodities specific to the Hildon framework.
 *
 * As of hildon 2.2, #HildonDialog has been deprecated in favor of #GtkDialog.
 *
 * <example>
 * <title>Simple <structname>HildonDialog</structname> usage</title>
 * <programlisting>
 * void quick_message (gchar *message)
 * {
 * <!-- -->
 *     GtkWidget *dialog, *label;
 * <!-- -->
 *     dialog = hildon_dialog_new ();
 *     label = gtk_label_new (message);
 * <!-- -->
 *     g_signal_connect_swapped (dialog,
 *                               "response",
 *                               G_CALLBACK (gtk_widget_destroy),
 *                               dialog);
 * <!-- -->
 *     gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
 *                        label);
 *     gtk_widget_show_all (dialog);
 * <!-- -->
 * }
 * </programlisting>
 * </example>
 */

#undef                                          HILDON_DISABLE_DEPRECATED

#include                                        "hildon-dialog.h"
#include                                        "hildon-gtk.h"

G_DEFINE_TYPE (HildonDialog, hildon_dialog, GTK_TYPE_DIALOG);

static void
hildon_dialog_class_init                        (HildonDialogClass *dialog_class)
{
}

static void
hildon_dialog_init                              (HildonDialog *self)
{
}

/**
 * hildon_dialog_new:
 *
 * Creates a new #HildonDialog widget
 *
 * Returns: the newly created #HildonDialog
 */
GtkWidget*
hildon_dialog_new                               (void)
{
    GtkWidget *self = g_object_new (HILDON_TYPE_DIALOG, NULL);

    return self;
}

/**
 * hildon_dialog_new_with_buttons:
 * @title: Title of the dialog, or %NULL
 * @parent: Transient parent of the dialog, or %NULL
 * @flags: from #GtkDialogFlags
 * @first_button_text: stock ID or text to go in first button, or %NULL
 * @Varargs: response ID for first button, then additional buttons, ending with %NULL
 *
 * Creates a new #HildonDialog. See gtk_dialog_new_with_buttons() for
 * more information.
 *
 * Return value: a new #HildonDialog
 */
GtkWidget*
hildon_dialog_new_with_buttons                  (const gchar *title,
                                                 GtkWindow *parent,
                                                 GtkDialogFlags flags,
                                                 const gchar *first_button_text,
                                                 ...)
{
    GtkWidget *dialog;

    dialog = g_object_new (HILDON_TYPE_DIALOG, NULL);

    /* This code is copied from gtk_dialog_new_empty(), as it's a
     * private function that we cannot use here */
    if (title)
        gtk_window_set_title (GTK_WINDOW (dialog), title);

    if (parent)
        gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

    if (flags & GTK_DIALOG_MODAL)
        gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);

    if (flags & GTK_DIALOG_DESTROY_WITH_PARENT)
        gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);

    if (flags & GTK_DIALOG_NO_SEPARATOR)
        gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);

    /* This is almost copied from gtk_dialog_add_buttons_valist() */
    if (first_button_text != NULL) {
        va_list args;
        const gchar *text;
        gint response_id;

        va_start (args, first_button_text);
        text = first_button_text;
        response_id = va_arg (args, gint);

        while (text != NULL) {
            hildon_dialog_add_button (HILDON_DIALOG (dialog), text, response_id);

            text = va_arg (args, gchar*);
            if (text == NULL)
                break;
            response_id = va_arg (args, int);
        }
        va_end (args);
    }

    return dialog;
}

/**
 * hildon_dialog_add_button:
 * @dialog: a #HildonDialog
 * @button_text: text of the button, or stock ID
 * @response_id: response ID for the button.
 *
 * Adds a button to the dialog. Works exactly like
 * gtk_dialog_add_button(), the only difference being that the button
 * has finger size.
 *
 * Returns: the button widget that was added
 */
GtkWidget *
hildon_dialog_add_button                        (HildonDialog *dialog,
                                                 const gchar  *button_text,
                                                 gint          response_id)
{
    GtkWidget *button;
    button = gtk_dialog_add_button (GTK_DIALOG (dialog), button_text, response_id);
    return button;
}

/**
 * hildon_dialog_add_buttons:
 * @dialog: a #HildonDialog
 * @first_button_text: text of the button, or stock ID
 * @Varargs: response ID for first button, then more text-response_id pairs
 *
 * Adds several buttons to the dialog. Works exactly like
 * gtk_dialog_add_buttons(), the only difference being that the
 * buttons have finger size.
 */
void
hildon_dialog_add_buttons                       (HildonDialog *dialog,
                                                 const gchar  *first_button_text,
                                                 ...)
{
    va_list args;
    const gchar *text;
    gint response_id;

    va_start (args, first_button_text);
    text = first_button_text;
    response_id = va_arg (args, gint);

    while (text != NULL) {
        hildon_dialog_add_button (HILDON_DIALOG (dialog), text, response_id);

        text = va_arg (args, gchar*);
        if (text == NULL)
            break;
        response_id = va_arg (args, int);
    }

    va_end (args);
}

