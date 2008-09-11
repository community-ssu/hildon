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

#ifndef                                         __HILDON_CHECK_BUTTON_H__
#define                                         __HILDON_CHECK_BUTTON_H__

#include                                        "hildon-gtk.h"

G_BEGIN_DECLS

GtkWidget *
hildon_check_button_new                         (HildonSizeType size);

void
hildon_check_button_set_active                  (GtkButton *button,
                                                 gboolean   is_active);

gboolean
hildon_check_button_get_active                  (GtkButton *button);

G_END_DECLS

#endif /* __HILDON_CHECK_BUTTON_H__ */
