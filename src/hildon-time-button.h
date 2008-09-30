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

#ifndef                                         __HILDON_TIME_BUTTON__
#define                                         __HILDON_TIME_BUTTON__

#include                                        <glib-object.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_TIME_BUTTON \
                                                hildon_time_button_get_type()

#define                                         HILDON_TIME_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
                                                HILDON_TYPE_TIME_BUTTON, HildonTimeButton))

#define                                         HILDON_TIME_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_TIME_BUTTON, HildonTimeButtonClass))

#define                                         HILDON_IS_TIME_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_TIME_BUTTON))

#define                                         HILDON_IS_TIME_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_TIME_BUTTON))

#define                                         HILDON_TIME_BUTTON_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_TIME_BUTTON, HildonTimeButtonClass))

typedef struct                                  _HildonTimeButton HildonTimeButton;
typedef struct                                  _HildonTimeButtonClass HildonTimeButtonClass;

struct                                          _HildonTimeButton
{
  HildonPickerButton parent;
};

struct                                          _HildonTimeButtonClass
{
  HildonPickerButtonClass parent_class;
};

GType
hildon_time_button_get_type                     (void);

GtkWidget*
hildon_time_button_new                          (HildonSizeType         size,
                                                HildonButtonArrangement arrangement);

void
hildon_time_button_get_time                     (HildonTimeButton *button,
                                                 guint            *hours,
                                                 guint            *minutes);

void
hildon_time_button_set_time                     (HildonTimeButton *button,
                                                 guint             hours,
                                                 guint             minutes);

G_END_DECLS

#endif /* _HILDON_TIME_BUTTON */
