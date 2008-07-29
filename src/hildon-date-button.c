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

#include "hildon-date-selector.h"
#include "hildon-touch-picker.h"
#include "hildon-picker-button.h"
#include "hildon-date-button.h"

G_DEFINE_TYPE (HildonDateButton, hildon_date_button, HILDON_TYPE_PICKER_BUTTON)

#if 0
#define GET_PRIVATE(o)                                                  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_DATE_BUTTON, HildonDateButtonPrivate))

typedef struct _HildonDateButtonPrivate HildonDateButtonPrivate;

struct _HildonDateButtonPrivate
{
};
#endif

#if 0
static void
hildon_date_button_get_property (GObject * object, guint property_id,
                                 GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_date_button_set_property (GObject * object, guint property_id,
                                 const GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}
#endif

static void
hildon_date_button_class_init (HildonDateButtonClass * klass)
{
#if 0
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HildonDateButtonPrivate));

  object_class->get_property = hildon_date_button_get_property;
  object_class->set_property = hildon_date_button_set_property;
#endif
}

static void
hildon_date_button_init (HildonDateButton * self)
{
  GtkWidget *date_selector;

  date_selector = hildon_date_selector_new ();

  hildon_picker_button_set_picker (HILDON_PICKER_BUTTON (self),
                                   HILDON_TOUCH_PICKER (date_selector));
}

GtkWidget *
hildon_date_button_new (HildonButtonFlags flags)
{
  return g_object_new (HILDON_TYPE_DATE_BUTTON,
                       "title", "Date",
                       "arrangement-flags", flags,
                       NULL);
}

void
hildon_date_button_get_date (HildonDateButton * button,
                             guint * year, guint * month, guint * day)
{
  HildonTouchPicker *picker;

  g_return_if_fail (HILDON_IS_DATE_BUTTON (button));

  picker = hildon_picker_button_get_picker (HILDON_PICKER_BUTTON (button));

  hildon_date_selector_get_date (HILDON_DATE_SELECTOR (picker), year, month, day);
}

void
hildon_date_button_set_date (HildonDateButton * button,
                             guint year, guint month, guint day)
{
  HildonTouchPicker *picker;
  gchar *date;

  g_return_if_fail (HILDON_IS_DATE_BUTTON (button));

  picker = hildon_picker_button_get_picker (HILDON_PICKER_BUTTON (button));

  hildon_date_selector_select_current_date (HILDON_DATE_SELECTOR (picker),
                                            year, month, day);
  date = hildon_touch_picker_get_current_text (HILDON_TOUCH_PICKER (picker));

  hildon_button_set_value (HILDON_BUTTON (button), date);
}
