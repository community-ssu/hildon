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

GtkWidget *
hildon_picker_button_new (HildonSizeType          size,
                          HildonButtonArrangement arrangement)
{
  GtkWidget *button;

  button = g_object_new (HILDON_TYPE_PICKER_BUTTON,
                         "arrangement", arrangement, "size", size, NULL);

  return button;
}

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

HildonTouchSelector *
hildon_picker_button_get_selector (HildonPickerButton * button)
{
  HildonPickerButtonPrivate *priv;

  g_return_val_if_fail (HILDON_IS_PICKER_BUTTON (button), NULL);

  priv = GET_PRIVATE (button);

  return HILDON_TOUCH_SELECTOR (priv->selector);
}
