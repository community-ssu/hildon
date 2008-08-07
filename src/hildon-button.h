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

#ifndef                                         __HILDON_BUTTON_H__
#define                                         __HILDON_BUTTON_H__

#include                                        <gtk/gtk.h>
#include                                        "hildon-helper.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_BUTTON \
                                                (hildon_button_get_type())

#define                                         HILDON_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_BUTTON, HildonButton))

#define                                         HILDON_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_BUTTON, HildonButtonClass))

#define                                         HILDON_IS_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_BUTTON))

#define                                         HILDON_IS_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_BUTTON))

#define                                         HILDON_BUTTON_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_BUTTON, HildonButtonClass))

typedef struct                                  _HildonButton HildonButton;

typedef struct                                  _HildonButtonClass HildonButtonClass;

struct                                          _HildonButtonClass
{
    GtkButtonClass parent_class;
};

struct                                          _HildonButton
{
    GtkButton parent;
};

typedef enum {
   HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
   HILDON_BUTTON_ARRANGEMENT_VERTICAL
}                                               HildonButtonArrangement;

GType
hildon_button_get_type                          (void) G_GNUC_CONST;

GtkWidget *
hildon_button_new                               (HildonSizeType          size,
                                                 HildonButtonArrangement arrangement);

GtkWidget *
hildon_button_new_with_text                     (HildonSizeType           size,
                                                 HildonButtonArrangement  arrangement,
                                                 const gchar             *title,
                                                 const gchar             *value);

GtkWidget *
hildon_button_new_full                          (HildonSizeType           size,
                                                 HildonButtonArrangement  arrangement,
                                                 const gchar             *title,
                                                 const gchar             *value,
                                                 GtkSizeGroup            *title_size_group,
                                                 GtkSizeGroup            *value_size_group);

void
hildon_button_set_title                         (HildonButton *button,
                                                 const gchar  *title);

void
hildon_button_set_value                         (HildonButton *button,
                                                 const gchar  *value);

const gchar *
hildon_button_get_title                         (HildonButton *button);

const gchar *
hildon_button_get_value                         (HildonButton *button);

void
hildon_button_set_text                          (HildonButton *button,
                                                 const gchar  *title,
                                                 const gchar  *value);

void
hildon_button_set_size_groups                   (HildonButton *button,
                                                 GtkSizeGroup *title_size_group,
                                                 GtkSizeGroup *value_size_group);

G_END_DECLS

#endif /* __HILDON_BUTTON_H__ */
