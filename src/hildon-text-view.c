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
 * SECTION:hildon-text-view
 * @short_description: Widget representing a text view in the Hildon framework.
 *
 * The #HildonTextView is a GTK widget which represents a text
 * view. It is derived from the #GtkTextView widget and provides
 * additional commodities specific to the Hildon framework.
 *
 * Besides all the features inherited from #GtkTextView, a
 * #HildonTextView can also have a placeholder text. This text will be
 * shown if the text view is empty and doesn't have the input focus,
 * but it's otherwise ignored. Thus, calls to
 * hildon_text_view_get_buffer() will never return the placeholder
 * text, not even when it's being displayed.
 *
 * Although #HildonTextView is derived from #GtkTextView,
 * gtk_text_view_get_buffer() and gtk_text_view_set_buffer() must
 * never be used to get/set the buffer in this
 * widget. hildon_text_view_get_buffer() and
 * hildon_text_view_set_buffer() must be used instead.
 *
 * <example>
 * <title>Creating a HildonTextView with a placeholder</title>
 * <programlisting>
 * GtkWidget *
 * create_text_view (void)
 * {
 *     GtkWidget *text_view;
 * <!-- -->
 *     text_view = hildon_text_view_new ();
 *     hildon_text_view_set_placeholder (HILDON_TEXT_VIEW (text_view),
 *                                       "Type some text here");
 * <!-- -->
 *     return text_view;
 * }
 * </programlisting>
 * </example>
 */

#include                                        "hildon-text-view.h"

G_DEFINE_TYPE                                   (HildonTextView, hildon_text_view, GTK_TYPE_TEXT_VIEW);

#define                                         HILDON_TEXT_VIEW_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_TEXT_VIEW, HildonTextViewPrivate));

struct                                          _HildonTextViewPrivate
{
    GtkTextBuffer *main_buffer;                   /* Used to show the "real" contents */
    GtkTextBuffer *placeholder_buffer;   /* Internal, used to display the placeholder */
    gulong changed_id;               /* ID of the main_buffer::changed signal handler */
};

static const gchar *placeholder_widget_name     = "hildon-text-view-placeholder";

/* Function used to decide whether to show the placeholder or not */
static void
hildon_text_view_refresh_contents               (GtkWidget *text_view)
{
    HildonTextViewPrivate *priv = HILDON_TEXT_VIEW (text_view)->priv;
    gint bufsize = gtk_text_buffer_get_char_count (priv->main_buffer);

    if ((bufsize > 0) || GTK_WIDGET_HAS_FOCUS (text_view)) {
        /* Display the main buffer if it contains text or the widget is focused */
        gtk_widget_set_name (text_view, NULL);
        gtk_text_view_set_buffer (GTK_TEXT_VIEW (text_view), priv->main_buffer);
    } else {
        /* Otherwise, display the placeholder */
        gtk_widget_set_name (text_view, placeholder_widget_name);
        gtk_text_view_set_buffer (GTK_TEXT_VIEW (text_view), priv->placeholder_buffer);
    }
}

/**
 * hildon_text_view_set_buffer:
 * @text_view: a #HildonTextView
 * @buffer: a #GtkTextBuffer
 *
 * Sets @buffer as the buffer being displayed by @text_view. The
 * previous buffer displayed by the text view is unreferenced, and a
 * reference is added to @buffer. If you owned a reference to @buffer
 * before passing it to this function, you must remove that reference
 * yourself
 *
 * Note that you must never use gtk_text_view_set_buffer() to set the
 * buffer of a #HildonTextView.
 */
void
hildon_text_view_set_buffer                     (HildonTextView *text_view,
                                                 GtkTextBuffer  *buffer)
{
    HildonTextViewPrivate *priv;

    g_return_if_fail (HILDON_IS_TEXT_VIEW (text_view));
    g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));

    priv = text_view->priv;

    /* If this is the same buffer, don't do anything */
    if (buffer == priv->main_buffer)
        return;

    /* Disconnect the signal handler from the old buffer */
    g_signal_handler_disconnect (priv->main_buffer, priv->changed_id);

    /* Replace the old buffer with the new one */
    g_object_unref (priv->main_buffer);
    priv->main_buffer = g_object_ref (buffer);

    /* Attach a callback to the new text buffer */
    priv->changed_id =
        g_signal_connect_swapped (priv->main_buffer, "changed",
                                  G_CALLBACK (hildon_text_view_refresh_contents), text_view);

    /* Refresh textview contents */
    hildon_text_view_refresh_contents (GTK_WIDGET (text_view));
}

/**
 * hildon_text_view_get_buffer:
 * @text_view: a #HildonTextView
 *
 * Returns the text buffer in @text_view. The reference count is not
 * incremented; the caller of this function won't own a new reference.
 *
 * Note that you must never use gtk_text_view_get_buffer() to get the
 * buffer from a #HildonTextView.
 *
 * Also note that placeholder text (set using
 * hildon_text_view_set_placeholder()) is never contained in this
 * buffer.
 *
 * Returns: a #GtkTextBuffer
 */
GtkTextBuffer *
hildon_text_view_get_buffer                     (HildonTextView *text_view)
{
    g_return_val_if_fail (HILDON_IS_TEXT_VIEW (text_view), NULL);

    /* Always return priv->main_buffer even if the placeholder is
     * being displayed */
    return text_view->priv->main_buffer;
}

/**
 * hildon_text_view_set_placeholder:
 * @text_view: a #HildonTextView
 * @text: the new text
 *
 * Sets the placeholder text in @text_view to @text.
 */
void
hildon_text_view_set_placeholder                (HildonTextView *text_view,
                                                 const gchar    *text)
{
    g_return_if_fail (HILDON_IS_TEXT_VIEW (text_view) && text != NULL);

    gtk_text_buffer_set_text (text_view->priv->placeholder_buffer, text, -1);
}

/**
 * hildon_text_view_new:
 *
 * Creates a new text view.
 *
 * Returns: a new #HildonTextView
 */
GtkWidget *
hildon_text_view_new                            (void)
{
    GtkWidget *entry = g_object_new (HILDON_TYPE_TEXT_VIEW, NULL);

    return entry;
}

static gboolean
hildon_text_view_focus_in_event                 (GtkWidget     *widget,
                                                 GdkEventFocus *event)
{
    hildon_text_view_refresh_contents (widget);

    if (GTK_WIDGET_CLASS (hildon_text_view_parent_class)->focus_in_event) {
        return GTK_WIDGET_CLASS (hildon_text_view_parent_class)->focus_in_event (widget, event);
    } else {
        return FALSE;
    }
}

static gboolean
hildon_text_view_focus_out_event                (GtkWidget     *widget,
                                                 GdkEventFocus *event)
{
    hildon_text_view_refresh_contents (widget);

    if (GTK_WIDGET_CLASS (hildon_text_view_parent_class)->focus_out_event) {
        return GTK_WIDGET_CLASS (hildon_text_view_parent_class)->focus_out_event (widget, event);
    } else {
        return FALSE;
    }
}

static void
hildon_text_view_finalize                       (GObject *object)
{
    HildonTextViewPrivate *priv = HILDON_TEXT_VIEW (object)->priv;

    g_signal_handler_disconnect (priv->main_buffer, priv->changed_id);
    g_object_unref (priv->main_buffer);
    g_object_unref (priv->placeholder_buffer);

    if (G_OBJECT_CLASS (hildon_text_view_parent_class)->finalize)
        G_OBJECT_CLASS (hildon_text_view_parent_class)->finalize (object);
}

static void
hildon_text_view_class_init                     (HildonTextViewClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *)klass;
    GtkWidgetClass *widget_class = (GtkWidgetClass *)klass;

    gobject_class->finalize = hildon_text_view_finalize;
    widget_class->focus_in_event = hildon_text_view_focus_in_event;
    widget_class->focus_out_event = hildon_text_view_focus_out_event;

    g_type_class_add_private (klass, sizeof (HildonTextViewPrivate));
}

static void
hildon_text_view_init                           (HildonTextView *self)
{
    HildonTextViewPrivate *priv = HILDON_TEXT_VIEW_GET_PRIVATE (self);

    self->priv = priv;

    priv->main_buffer = gtk_text_buffer_new (NULL);
    priv->placeholder_buffer = gtk_text_buffer_new (NULL);

    hildon_text_view_refresh_contents (GTK_WIDGET (self));

    priv->changed_id =
        g_signal_connect_swapped (priv->main_buffer, "changed",
                                  G_CALLBACK (hildon_text_view_refresh_contents), self);
}
