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
 * SECTION:hildon-picker-button
 * @short_description: A button that launches a #HildonPickerDialog and displays the
 * selected item
 * @see_also: #HildonTouchSelector, #HildonPickerDialog
 *
 * #HildonPickerButton is a widget that lets the user select a particular item from
 * a list. Visually, it's a button with title and value labels that brings up a
 * #HildonPickerDialog. The user can then use this dialog to choose an item, which
 * will be displayed in the value label of the button.
 *
 * You should create your own #HildonTouchSelector at convenience and set it
 * to the #HildonPickerButton with hildon_picker_button_set_selector(). For
 * the common use cases of buttons to select date and time, you can use #HildonDateButton
 * and #HildonTimeButton.
 *
 */

#include "hildon-picker-button.h"
#include "hildon-picker-dialog.h"

G_DEFINE_TYPE (HildonPickerButton, hildon_picker_button, HILDON_TYPE_BUTTON)

#define GET_PRIVATE(o)                                                  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_PICKER_BUTTON, HildonPickerButtonPrivate))

typedef struct _HildonPickerButtonPrivate HildonPickerButtonPrivate;

struct _HildonPickerButtonPrivate
{
  GtkWidget *selector;
  GtkWidget *dialog;
};

/* Signals */
enum
{
  VALUE_CHANGED,
  LAST_SIGNAL
};

enum
{
  PROP_SELECTOR = 1,
};

static guint picker_button_signals[LAST_SIGNAL] = { 0 };

static void
hildon_picker_button_get_property (GObject * object, guint property_id,
                                   GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
  case PROP_SELECTOR:
    g_value_set_object (value,
                        hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (object)));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_picker_button_set_property (GObject * object, guint property_id,
                                   const GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
  case PROP_SELECTOR:
    hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (object),
                                       g_value_get_object (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_picker_button_clicked (GtkButton * button)
{
  GtkWidget *parent;
  HildonPickerButtonPrivate *priv;
  gint response;

  priv = GET_PRIVATE (HILDON_PICKER_BUTTON (button));

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (priv->selector));

  /* Create the dialog if it doesn't exist already.  */
  if (!priv->dialog) {
    parent = gtk_widget_get_toplevel (GTK_WIDGET (button));
    if (GTK_WIDGET_TOPLEVEL (parent)) {
      priv->dialog = hildon_picker_dialog_new (GTK_WINDOW (parent));
    } else {
      priv->dialog = hildon_picker_dialog_new (NULL);
    }

    hildon_picker_dialog_set_selector (HILDON_PICKER_DIALOG (priv->dialog),
                                       HILDON_TOUCH_SELECTOR (priv->selector));

    gtk_window_set_modal (GTK_WINDOW (priv->dialog),
                          gtk_window_get_modal (GTK_WINDOW (parent)));
    gtk_window_set_title (GTK_WINDOW (priv->dialog),
                          hildon_button_get_title (HILDON_BUTTON (button)));
  }

  response = gtk_dialog_run (GTK_DIALOG (priv->dialog));
  switch (response) {
  case GTK_RESPONSE_OK:
    hildon_button_set_value (HILDON_BUTTON (button),
                             hildon_touch_selector_get_current_text
                             (HILDON_TOUCH_SELECTOR (priv->selector)));
    g_signal_emit (HILDON_PICKER_BUTTON (button),
                   picker_button_signals[VALUE_CHANGED], 0);
    break;
  }
  gtk_widget_hide (priv->dialog);
}

static void
hildon_picker_button_class_init (HildonPickerButtonClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkButtonClass *button_class = GTK_BUTTON_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HildonPickerButtonPrivate));

  object_class->get_property = hildon_picker_button_get_property;
  object_class->set_property = hildon_picker_button_set_property;

  button_class->clicked = hildon_picker_button_clicked;

  g_object_class_install_property (object_class,
                                   PROP_SELECTOR,
                                   g_param_spec_object ("touch-selector",
                                                        "HildonTouchSelector widget",
                                                        "HildonTouchSelector widget to be launched on button clicked",
                                                        HILDON_TYPE_TOUCH_SELECTOR,
                                                        G_PARAM_READWRITE));

  /**
   * HildonPickerButton::value-changed:
   * @widget: the widget that received the signal
   *
   * The ::value-changed signal is emitted each time the user chooses a different
   * item from the #HildonTouchSelector related, and the value label gets updated.
   */
  picker_button_signals[VALUE_CHANGED] =
    g_signal_new ("value-changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                  0,
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0, NULL);
}

static void
hildon_picker_button_init (HildonPickerButton * self)
{
  HildonPickerButtonPrivate *priv;

  priv = GET_PRIVATE (self);

  priv->dialog = NULL;
  priv->selector = NULL;
}

/**
 * hildon_picker_button_new:
 * @size: One of #HildonSizeType, specifying the size of the new button.
 * @arrangement: one of #HildonButtonArrangement, specifying the placement of the
 * labels.
 *
 * Creates a new #HildonPickerButton. See hildon_button_new() for details on the
 * parameters.
 *
 * Returns: a newly created #HildonPickerButton
 **/
GtkWidget *
hildon_picker_button_new (HildonSizeType          size,
                          HildonButtonArrangement arrangement)
{
  GtkWidget *button;

  button = g_object_new (HILDON_TYPE_PICKER_BUTTON,
                         "arrangement", arrangement, "size", size, NULL);

  return button;
}

/**
 * hildon_picker_button_set_selector:
 * @button: a #HildonPickerButton
 * @selector: a #HildonTouchSelector
 *
 * Sets @selector as the #HildonTouchSelector to be shown in the
 * #HildonPickerDialog that @button brings up.
 **/
void
hildon_picker_button_set_selector (HildonPickerButton * button,
                                   HildonTouchSelector * selector)
{
  HildonPickerButtonPrivate *priv;

  g_return_if_fail (HILDON_IS_PICKER_BUTTON (button));
  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));

  priv = GET_PRIVATE (button);

  priv->selector = g_object_ref (selector);
}

/**
 * hildon_picker_button_get_selector:
 * @button: a #HildonPickerButton
 *
 * Retrieves the #HildonTouchSelector associated to @button.
 *
 * Returns: a #HildonTouchSelector
 **/
HildonTouchSelector *
hildon_picker_button_get_selector (HildonPickerButton * button)
{
  HildonPickerButtonPrivate *priv;

  g_return_val_if_fail (HILDON_IS_PICKER_BUTTON (button), NULL);

  priv = GET_PRIVATE (button);

  return HILDON_TOUCH_SELECTOR (priv->selector);
}

/**
 * hildon_picker_button_get_active:
 * @button: a #HildonPickerButton
 *
 * Returns the index of the currently active item, or -1 if there's no
 * active item. If the selector has several columns, only the first
 * one is used.
 *
 * Returns: an integer which is the index of the currently active item, or -1 if there's no active item.
 **/
gint
hildon_picker_button_get_active                 (HildonPickerButton * button)
{
  HildonTouchSelector *sel;
  g_return_val_if_fail (HILDON_IS_PICKER_BUTTON (button), -1);

  sel = hildon_picker_button_get_selector (button);

  return hildon_touch_selector_get_active (sel, 0);
}

/**
 * hildon_picker_button_set_active:
 * @button: a #HildonPickerButton
 * @index: the index of the item to select, or -1 to have no active item
 *
 * Sets the active item of the #HildonTouchSelector associated to
 * @button to @index. If the selector has several columns, only the
 * first one is used.
 **/
void
hildon_picker_button_set_active                 (HildonPickerButton * button,
                                                 gint index)
{
  HildonTouchSelector *sel;
  g_return_if_fail (HILDON_IS_PICKER_BUTTON (button));

  sel = hildon_picker_button_get_selector (button);

  hildon_touch_selector_set_active (sel, 0, index);

  hildon_button_set_value (HILDON_BUTTON (button),
                           hildon_touch_selector_get_current_text (sel));
}
