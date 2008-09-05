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
 * is derived from the GtkEntry widget and provides additional
 * commodities specific to the Hildon framework.
 */

#include                                        "hildon-entry.h"

G_DEFINE_TYPE                                   (HildonEntry, hildon_entry, GTK_TYPE_ENTRY);

static const gchar *placeholder_widget_name     = "hildon-entry-placeholder";

void
hildon_entry_set_text                           (HildonEntry *entry,
                                                 const gchar *text)
{
    g_return_if_fail (HILDON_IS_ENTRY (entry));

    gtk_entry_set_text (GTK_ENTRY (entry), text);

    gtk_widget_set_name (GTK_WIDGET (entry), NULL);
}

const gchar *
hildon_entry_get_text                           (HildonEntry *entry)
{
    g_return_val_if_fail (HILDON_IS_ENTRY (entry), NULL);

    if (g_str_equal (gtk_widget_get_name (GTK_WIDGET (entry)), placeholder_widget_name)) {
        return "";
    }

    return gtk_entry_get_text (GTK_ENTRY (entry));
}

void
hildon_entry_set_placeholder                    (HildonEntry *entry,
                                                 const gchar *text)
{
    g_return_if_fail (HILDON_IS_ENTRY (entry));

    gtk_widget_set_name (GTK_WIDGET (entry), placeholder_widget_name);

    gtk_entry_set_text (GTK_ENTRY (entry), text);
}

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
