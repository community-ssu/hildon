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

#ifndef                                         __HILDON_DATE_BUTTON__
#define                                         __HILDON_DATE_BUTTON__

#include                                        <gtk/gtk.h>

#include                                        "hildon-picker-button.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_DATE_BUTTON \
                                                hildon_date_button_get_type()

#define                                         HILDON_DATE_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj),\
                                                 HILDON_TYPE_DATE_BUTTON, HildonDateButton))

#define                                         HILDON_DATE_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_DATE_BUTTON, HildonDateButtonClass))

#define                                         HILDON_IS_DATE_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_DATE_BUTTON))

#define                                         HILDON_IS_DATE_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_DATE_BUTTON))

#define                                         HILDON_DATE_BUTTON_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_DATE_BUTTON, HildonDateButtonClass))


typedef struct                                  _HildonDateButton HildonDateButton;
typedef struct                                  _HildonDateButtonClass HildonDateButtonClass;

struct                                          _HildonDateButton
{
  HildonPickerButton parent;
};

struct                                          _HildonDateButtonClass
{
  HildonPickerButtonClass parent_class;
};

GType
hildon_date_button_get_type                     (void);

GtkWidget*
hildon_date_button_new                          (HildonSizeType          size,
                                                 HildonButtonArrangement arrangement);

GtkWidget *
hildon_date_button_new_with_year_range         (HildonSizeType size,
                                                HildonButtonArrangement arrangement,
                                                gint min_year,
                                                gint max_year);

void
hildon_date_button_get_date                     (HildonDateButton *button,
                                                 guint *year,
                                                 guint *month,
                                                 guint *day);
void
hildon_date_button_set_date                     (HildonDateButton * button,
                                                 guint year,
                                                 guint month,
                                                 guint day);

G_END_DECLS

#endif /* __HILDON_DATE_BUTTON__ */
