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
 * SECTION:hildon-edit-toolbar
 * @short_description: Widget representing a toolbar for editing.
 *
 * The #HildonEditToolbar is a toolbar which contains a label and two
 * buttons, one of them being an arrow pointing backwards.
 *
 * The label is a description of the action that the user is supposed
 * to do. The button is to be pressed when the user completes the
 * action. The arrow is used to go back to the previous view
 * discarding any changes.
 *
 * Note that those widgets don't do anything themselves by default. To
 * actually peform actions the developer must provide callbacks for
 * them.
 *
 * <example>
 * <title>HildonEditToolbar example</title>
 * <programlisting>
 * GtkWidget *window;
 * GtkWidget *vbox;
 * GtkWidget *toolbar;
 * // Declare more widgets here ...
 * <!-- -->
 * window = hildon_stackable_window_new ();
 * vbox = gtk_vbox_new (FALSE, 10);
 * toolbar = hildon_edit_toolbar_new_with_text ("Choose items to delete", "Delete");
 * // Create more widgets here ...
 * <!-- -->
 * gtk_container_add (GTK_CONTAINER (window), vbox);
 * gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);
 * // Pack more widgets here ...
 * <!-- -->
 * g_signal_connect (toolbar, "button-clicked", G_CALLBACK (delete_button_clicked), someparameter);
 * g_signal_connect_swapped (toolbar, "arrow-clicked", G_CALLBACK (gtk_widget_destroy), window);
 * <!-- -->
 * gtk_widget_show_all (window);
 * gtk_window_fullscreen (GTK_WINDOW (window));
 * </programlisting>
 * </example>
 */

#include                                        "hildon-edit-toolbar.h"
#include                                        "hildon-gtk.h"

G_DEFINE_TYPE                                   (HildonEditToolbar, hildon_edit_toolbar, GTK_TYPE_HBOX);

#define                                         HILDON_EDIT_TOOLBAR_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_EDIT_TOOLBAR, HildonEditToolbarPrivate));

typedef struct                                  _HildonEditToolbarPrivate HildonEditToolbarPrivate;

struct                                          _HildonEditToolbarPrivate
{
    GtkLabel *label;
    GtkButton *button;
    GtkButton *arrow;
};

enum {
    BUTTON_CLICKED,
    ARROW_CLICKED,
    N_SIGNALS
};

static guint                                    toolbar_signals [N_SIGNALS] = { 0 };


static void
hildon_edit_toolbar_class_init                  (HildonEditToolbarClass *klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;

    g_type_class_add_private (klass, sizeof (HildonEditToolbarPrivate));

    /**
     * HildonEditToolbar::button-clicked:
     * @widget: the object which received the signal.
     *
     * Emitted when the toolbar button has been activated (pressed and released).
     *
     */
    toolbar_signals[BUTTON_CLICKED] =
        g_signal_new ("button_clicked",
                      G_OBJECT_CLASS_TYPE (gobject_class),
                      G_SIGNAL_RUN_FIRST,
                      0, NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);

    /**
     * HildonEditToolbar::arrow-clicked:
     * @widget: the object which received the signal.
     *
     * Emitted when the toolbar back button (arrow) has been activated
     * (pressed and released).
     *
     */
    toolbar_signals[ARROW_CLICKED] =
        g_signal_new ("arrow_clicked",
                      G_OBJECT_CLASS_TYPE (gobject_class),
                      G_SIGNAL_RUN_FIRST,
                      0, NULL, NULL,
                      g_cclosure_marshal_VOID__VOID,
                      G_TYPE_NONE, 0);
}

static void
button_clicked_cb                               (GtkButton *self,
                                                 HildonEditToolbar *toolbar)
{
    g_signal_emit (toolbar, toolbar_signals[BUTTON_CLICKED], 0);
}

static void
arrow_clicked_cb                                (GtkButton *self,
                                                 HildonEditToolbar *toolbar)
{
    g_signal_emit (toolbar, toolbar_signals[ARROW_CLICKED], 0);
}

static void
hildon_edit_toolbar_init                        (HildonEditToolbar *self)
{
    HildonEditToolbarPrivate *priv = HILDON_EDIT_TOOLBAR_GET_PRIVATE (self);
    GtkBox *hbox = GTK_BOX (self);

    priv->label = GTK_LABEL (gtk_label_new (NULL));
    priv->button = GTK_BUTTON (hildon_gtk_button_new (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_FINGER_HEIGHT));
    priv->arrow = GTK_BUTTON (gtk_button_new ());

    gtk_button_set_image (priv->arrow, gtk_image_new_from_stock (GTK_STOCK_GO_BACK, GTK_ICON_SIZE_LARGE_TOOLBAR));
    gtk_button_set_relief (priv->arrow, GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click (priv->arrow, FALSE);

    g_signal_connect (priv->button, "clicked", G_CALLBACK (button_clicked_cb), self);
    g_signal_connect (priv->arrow, "clicked", G_CALLBACK (arrow_clicked_cb), self);

    /* Temporary values, should be replaced by properties or fixed values from the specs */
    gtk_box_set_spacing (hbox, 10);
    gtk_widget_set_size_request (GTK_WIDGET (priv->arrow), 50, -1);

    gtk_box_pack_start (hbox, GTK_WIDGET (priv->label), TRUE, TRUE, 0);
    gtk_box_pack_start (hbox, GTK_WIDGET (priv->button), FALSE, FALSE, 0);
    gtk_box_pack_start (hbox, GTK_WIDGET (priv->arrow), FALSE, FALSE, 0);

    gtk_misc_set_alignment (GTK_MISC (priv->label), 0, 0.5);

    gtk_widget_show (GTK_WIDGET (priv->label));
    gtk_widget_show (GTK_WIDGET (priv->button));
    gtk_widget_show (GTK_WIDGET (priv->arrow));
}

/**
 * hildon_edit_toolbar_set_label:
 * @toolbar: a #HildonEditToolbar
 * @label: a new text for the toolbar label
 *
 * Sets the label of @toolbar to @label. This will clear any
 * previously set value.
 */
void
hildon_edit_toolbar_set_label                   (HildonEditToolbar *toolbar,
                                                 const gchar       *label)
{
    HildonEditToolbarPrivate *priv;
    g_return_if_fail (HILDON_IS_EDIT_TOOLBAR (toolbar));
    priv = HILDON_EDIT_TOOLBAR_GET_PRIVATE (toolbar);
    gtk_label_set_text (priv->label, label);
}

/**
 * hildon_edit_toolbar_set_button_label:
 * @toolbar: a #HildonEditToolbar
 * @label: a new text for the label of the toolbar button
 *
 * Sets the label of the toolbar button to @label. This will clear any
 * previously set value.
 */
void
hildon_edit_toolbar_set_button_label            (HildonEditToolbar *toolbar,
                                                 const gchar       *label)
{
    HildonEditToolbarPrivate *priv;
    g_return_if_fail (HILDON_IS_EDIT_TOOLBAR (toolbar));
    priv = HILDON_EDIT_TOOLBAR_GET_PRIVATE (toolbar);
    gtk_button_set_label (priv->button, label);
}

/**
 * hildon_edit_toolbar_new:
 *
 * Creates a new #HildonEditToolbar.
 *
 * Returns: a new #HildonEditToolbar
 */
GtkWidget *
hildon_edit_toolbar_new                         (void)
{
    return g_object_new (HILDON_TYPE_EDIT_TOOLBAR, NULL);
}

/**
 * hildon_edit_toolbar_new_with_text:
 *
 * Creates a new #HildonEditToolbar, with the toolbar label set to
 * @label and the button label set to @button.
 *
 * Returns: a new #HildonEditToolbar
 */
GtkWidget *
hildon_edit_toolbar_new_with_text               (const gchar *label,
                                                 const gchar *button)
{
    GtkWidget *toolbar = g_object_new (HILDON_TYPE_EDIT_TOOLBAR, NULL);

    hildon_edit_toolbar_set_label (HILDON_EDIT_TOOLBAR (toolbar), label);
    hildon_edit_toolbar_set_button_label (HILDON_EDIT_TOOLBAR (toolbar), button);

    return toolbar;
}
