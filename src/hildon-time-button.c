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

#include "hildon-time-selector.h"
#include "hildon-touch-selector.h"
#include "hildon-picker-button.h"
#include "hildon-time-button.h"

G_DEFINE_TYPE (HildonTimeButton, hildon_time_button, HILDON_TYPE_PICKER_BUTTON)

#if 0
#define GET_PRIVATE(o)                                                  \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_TIME_BUTTON, HildonTimeButtonPrivate))
typedef struct _HildonTimeButtonPrivate HildonTimeButtonPrivate;

struct _HildonTimeButtonPrivate
{
};
#endif

#if 0
static void
hildon_time_button_get_property (GObject * object, guint property_id,
                                 GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_time_button_set_property (GObject * object, guint property_id,
                                 const GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}
#endif

static void
hildon_time_button_class_init (HildonTimeButtonClass * klass)
{
#if 0
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (HildonTimeButtonPrivate));

  object_class->get_property = hildon_time_button_get_property;
  object_class->set_property = hildon_time_button_set_property;
#endif
}

static void
hildon_time_button_init (HildonTimeButton * self)
{
  GtkWidget *time_selector;

  time_selector = hildon_time_selector_new ();

  hildon_picker_button_set_selector (HILDON_PICKER_BUTTON (self),
                                     HILDON_TOUCH_SELECTOR (time_selector));
}

GtkWidget *
hildon_time_button_new (HildonSizeType          size,
                        HildonButtonArrangement arrangement)
{
  return g_object_new (HILDON_TYPE_TIME_BUTTON,
                       "title", "Time", "arrangement", arrangement, "size-flags", size, NULL);
}

void
hildon_time_button_get_time (HildonTimeButton * button,
                             guint * hours, guint * minutes)
{
  HildonTouchSelector *selector;

  g_return_if_fail (HILDON_IS_TIME_BUTTON (button));

  selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (button));

  hildon_time_selector_get_time (HILDON_TIME_SELECTOR (selector), hours, minutes);
}

void
hildon_time_button_set_time (HildonTimeButton * button,
                             guint hours, guint minutes)
{
  HildonTouchSelector *selector;
  gchar *time;

  g_return_if_fail (HILDON_IS_TIME_BUTTON (button));

  selector = hildon_picker_button_get_selector (HILDON_PICKER_BUTTON (button));

  hildon_time_selector_set_time (HILDON_TIME_SELECTOR (selector), hours, minutes);
  time = hildon_touch_selector_get_current_text (HILDON_TOUCH_SELECTOR (selector));

  hildon_button_set_value (HILDON_BUTTON (button), time);
}
