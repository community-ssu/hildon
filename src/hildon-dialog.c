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
 * The HildonDialog is a GTK widget which represent a popup window in the
 * Hildon framework. It is derived from the GtkDialog and provides additional
 * commodities specific to the Hildon framework.
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

#include                                        "hildon-dialog.h"

G_DEFINE_TYPE (HildonDialog, hildon_dialog, GTK_TYPE_DIALOG);

static void
hildon_dialog_class_init                        (HildonDialogClass * dialog_class)
{
}

static void
hildon_dialog_init (HildonDialog *self)
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
