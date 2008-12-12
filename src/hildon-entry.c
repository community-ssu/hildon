/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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
 *
 * <example>
 * <title>Creating a HildonEntry with a placeholder</title>
 * <programlisting>
 * GtkWidget *
 * create_entry (void)
 * {
 *     GtkWidget *entry;
 * <!-- -->
 *     entry = hildon_entry_new (HILDON_SIZE_AUTO);
 *     hildon_entry_set_placeholder (HILDON_ENTRY (entry), "First name");
 * <!-- -->
 *     return entry;
 * }
 * </programlisting>
 * </example>
 */

#include                                        "hildon-entry.h"

G_DEFINE_TYPE                                   (HildonEntry, hildon_entry, GTK_TYPE_ENTRY);

#define                                         HILDON_ENTRY_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_ENTRY, HildonEntryPrivate));

struct                                          _HildonEntryPrivate
{
    gchar *placeholder;
};

static const gchar *placeholder_widget_name     = "hildon-entry-placeholder";

/* Function used to decide whether to show the placeholder or not */
static void
hildon_entry_refresh_contents                   (GtkWidget *entry)
{
    HildonEntryPrivate *priv = HILDON_ENTRY (entry)->priv;
    gboolean showing_placeholder, entry_has_focus;

    showing_placeholder = g_str_equal (gtk_widget_get_name (entry), placeholder_widget_name);
    entry_has_focus = GTK_WIDGET_HAS_FOCUS (entry);

    if (showing_placeholder) {
        if (entry_has_focus) {
            /* Remove the placeholder when the widget obtains focus */
            gtk_widget_set_name (entry, NULL);
            gtk_entry_set_text (GTK_ENTRY (entry), "");
        } else {
            /* Update the placeholder (it may have been changed) */
            gtk_entry_set_text (GTK_ENTRY (entry), priv->placeholder);
        }
    } else {
        /* Show the placeholder when the widget is empty and has no focus */
        const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
        if (text[0] == '\0' && !entry_has_focus) {
            if (priv->placeholder[0] != '\0') {
                gtk_widget_set_name (entry, placeholder_widget_name);
                gtk_entry_set_text (GTK_ENTRY (entry), priv->placeholder);
            }
        }
    }
}

/**
 * hildon_entry_set_text:
 * @entry: a #HildonEntry
 * @text: the new text
 *
 * Sets the text in @entry to @text, replacing its current contents.
 *
 * Note that you must never use gtk_entry_set_text() to set the text
 * of a #HildonEntry.
 *
 * Since: 2.2
 */
void
hildon_entry_set_text                           (HildonEntry *entry,
                                                 const gchar *text)
{
    g_return_if_fail (HILDON_IS_ENTRY (entry) && text != NULL);

    gtk_widget_set_name (GTK_WIDGET (entry), NULL);
    gtk_entry_set_text (GTK_ENTRY (entry), text);

    /* If the entry is cleared show the placeholder */
    if (text[0] == '\0')
        hildon_entry_refresh_contents (GTK_WIDGET (entry));
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
 *
 * Since: 2.2
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
 *
 * Since: 2.2
 */
void
hildon_entry_set_placeholder                    (HildonEntry *entry,
                                                 const gchar *text)
{
    g_return_if_fail (HILDON_IS_ENTRY (entry) && text != NULL);

    g_free (entry->priv->placeholder);
    entry->priv->placeholder = g_strdup (text);
    hildon_entry_refresh_contents (GTK_WIDGET (entry));
}

/**
 * hildon_entry_new:
 * @size: The size of the entry
 *
 * Creates a new entry.
 *
 * Returns: a new #HildonEntry
 *
 * Since: 2.2
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
    hildon_entry_refresh_contents (widget);

    if (GTK_WIDGET_CLASS (hildon_entry_parent_class)->focus_in_event) {
        return GTK_WIDGET_CLASS (hildon_entry_parent_class)->focus_in_event (widget, event);
    } else {
        return FALSE;
    }
}

static gboolean
hildon_entry_focus_out_event                    (GtkWidget     *widget,
                                                 GdkEventFocus *event)
{
    hildon_entry_refresh_contents (widget);

    if (GTK_WIDGET_CLASS (hildon_entry_parent_class)->focus_out_event) {
        return GTK_WIDGET_CLASS (hildon_entry_parent_class)->focus_out_event (widget, event);
    } else {
        return FALSE;
    }
}

static void
hildon_entry_finalize                           (GObject *object)
{
    HildonEntryPrivate *priv = HILDON_ENTRY (object)->priv;

    g_free (priv->placeholder);

    if (G_OBJECT_CLASS (hildon_entry_parent_class)->finalize)
        G_OBJECT_CLASS (hildon_entry_parent_class)->finalize (object);
}

static void
hildon_entry_class_init                         (HildonEntryClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class = (GtkWidgetClass *)klass;

    gobject_class->finalize = hildon_entry_finalize;
    widget_class->focus_in_event = hildon_entry_focus_in_event;
    widget_class->focus_out_event = hildon_entry_focus_out_event;

    g_type_class_add_private (klass, sizeof (HildonEntryPrivate));
}

static void
hildon_entry_init                               (HildonEntry *self)
{
    self->priv = HILDON_ENTRY_GET_PRIVATE (self);
    self->priv->placeholder = g_strdup ("");
}
