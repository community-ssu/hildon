/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * Contact: Karl Lattimer <karl.lattimer@nokia.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation; version 2 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 */

/**
 * SECTION:hildon-entry
 * @short_description: Widget representing a text entry in the Hildon framework.
 *
 * The #HildonEntry is a GTK widget which represents a text entry. It
 * is derived from the #GtkEntry widget and provides additional
 * commodities specific to the Hildon framework.
 *
 * Besides all the features inherited from #GtkEntry, a #HildonEntry
 * can also have a placeholder text. This text will be shown if the
 * entry is empty and doesn't have the input focus, but it's otherwise
 * ignored. Thus, calls to hildon_entry_get_text() will never return
 * the placeholder text, not even when it's being displayed.
 *
 * Although #HildonEntry is derived from #GtkEntry,
 * gtk_entry_get_text() and gtk_entry_set_text() must never be used to
 * get/set the text in this widget. hildon_entry_get_text() and
 * hildon_entry_set_text() must be used instead.
 */

#include                                        "hildon-entry.h"

G_DEFINE_TYPE                                   (HildonEntry, hildon_entry, GTK_TYPE_ENTRY);

static const gchar *placeholder_widget_name     = "hildon-entry-placeholder";

/**
 * hildon_entry_set_text:
 * @entry: a #HildonEntry
 * @text: the new text
 *
 * Sets the text in @entry to @text, replacing its current contents.
 *
 * Note that you must never use gtk_entry_set_text() to set the text
 * of a #HildonEntry.
 */
void
hildon_entry_set_text                           (HildonEntry *entry,
                                                 const gchar *text)
{
    g_return_if_fail (HILDON_IS_ENTRY (entry));

    gtk_entry_set_text (GTK_ENTRY (entry), text);

    gtk_widget_set_name (GTK_WIDGET (entry), NULL);
}

/**
 * hildon_entry_get_text:
 * @entry: a #HildonEntry
 *
 * Gets the current text in @entry.
 *
 * Note that you must never use gtk_entry_get_text() to get the text
 * from a #HildonEntry.
 *
 * Also note that placeholder text (set using
 * hildon_entry_set_placeholder()) is never returned. Only text set by
 * hildon_entry_set_text() or typed by the user is considered.
 *
 * Returns: the text in @entry. This text must not be modified or
 * freed.
 */
const gchar *
hildon_entry_get_text                           (HildonEntry *entry)
{
    g_return_val_if_fail (HILDON_IS_ENTRY (entry), NULL);

    if (g_str_equal (gtk_widget_get_name (GTK_WIDGET (entry)), placeholder_widget_name)) {
        return "";
    }

    return gtk_entry_get_text (GTK_ENTRY (entry));
}

/**
 * hildon_entry_set_placeholder:
 * @entry: a #HildonEntry
 * @text: the new text
 *
 * Sets the placeholder text in @entry to @text.
 */
void
hildon_entry_set_placeholder                    (HildonEntry *entry,
                                                 const gchar *text)
{
    g_return_if_fail (HILDON_IS_ENTRY (entry));

    gtk_widget_set_name (GTK_WIDGET (entry), placeholder_widget_name);

    gtk_entry_set_text (GTK_ENTRY (entry), text);
}

/**
 * hildon_entry_new:
 * @size: The size of the entry
 *
 * Creates a new entry.
 *
 * Returns: a new #HildonEntry
 */
GtkWidget *
hildon_entry_new                                (HildonSizeType size)
{
    GtkWidget *entry = g_object_new (HILDON_TYPE_ENTRY, NULL);

    hildon_gtk_widget_set_theme_size (entry, size);

    return entry;
}

static gboolean
hildon_entry_focus_in_event                     (GtkWidget     *widget,
                                                 GdkEventFocus *event)
{
    if (g_str_equal (gtk_widget_get_name (widget), placeholder_widget_name)) {
        hildon_entry_set_text (HILDON_ENTRY (widget), "");
    }
    if (GTK_WIDGET_CLASS (hildon_entry_parent_class)->focus_in_event) {
        return GTK_WIDGET_CLASS (hildon_entry_parent_class)->focus_in_event (widget, event);
    } else {
        return FALSE;
    }
}

static void
hildon_entry_class_init                         (HildonEntryClass *klass)
{
    GtkWidgetClass *widget_class = (GtkWidgetClass *)klass;

    widget_class->focus_in_event = hildon_entry_focus_in_event;
}

static void
hildon_entry_init                               (HildonEntry *self)
{
}
