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

#ifndef                                         __HILDON_CHECK_BUTTON_H__
#define                                         __HILDON_CHECK_BUTTON_H__

#include                                        "hildon-gtk.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_CHECK_BUTTON \
                                                (hildon_check_button_get_type())

#define                                         HILDON_CHECK_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_CHECK_BUTTON, HildonCheckButton))

#define                                         HILDON_CHECK_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_CHECK_BUTTON, HildonCheckButtonClass))

#define                                         HILDON_IS_CHECK_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_CHECK_BUTTON))

#define                                         HILDON_IS_CHECK_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_CHECK_BUTTON))

#define                                         HILDON_CHECK_BUTTON_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_CHECK_BUTTON, HildonCheckButtonClass))

typedef struct                                  _HildonCheckButton HildonCheckButton;

typedef struct                                  _HildonCheckButtonClass HildonCheckButtonClass;

typedef struct                                  _HildonCheckButtonPrivate HildonCheckButtonPrivate;

struct                                          _HildonCheckButtonClass
{
    GtkButtonClass parent_class;

    /* Signal handlers */
    void (* toggled)                            (HildonCheckButton *button);
};

struct                                          _HildonCheckButton
{
    GtkButton parent;

    /* private */
    HildonCheckButtonPrivate *priv;
};

GType
hildon_check_button_get_type                    (void) G_GNUC_CONST;

GtkWidget *
hildon_check_button_new                         (HildonSizeType size);

void
hildon_check_button_set_active                  (HildonCheckButton *button,
                                                 gboolean           is_active);

gboolean
hildon_check_button_get_active                  (HildonCheckButton *button);

void
hildon_check_button_toggled                     (HildonCheckButton *button);

G_END_DECLS

#endif /* __HILDON_CHECK_BUTTON_H__ */
