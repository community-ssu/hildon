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

#ifndef                                         __HILDON_TOGGLE_BUTTON_H__
#define                                         __HILDON_TOGGLE_BUTTON_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_TOGGLE_BUTTON \
                                                (hildon_toggle_button_get_type())

#define                                         HILDON_TOGGLE_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_TOGGLE_BUTTON, HildonToggleButton))

#define                                         HILDON_TOGGLE_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_TOGGLE_BUTTON, HildonToggleButtonClass))

#define                                         HILDON_IS_TOGGLE_BUTTON(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_TOGGLE_BUTTON))

#define                                         HILDON_IS_TOGGLE_BUTTON_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_TOGGLE_BUTTON))

#define                                         HILDON_TOGGLE_BUTTON_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_TOGGLE_BUTTON, HildonToggleButtonClass))

typedef struct                                  _HildonToggleButton HildonToggleButton;

typedef struct                                  _HildonToggleButtonClass HildonToggleButtonClass;

struct                                          _HildonToggleButtonClass
{
    GtkToggleButtonClass parent_class;
};

struct                                          _HildonToggleButton
{
    GtkToggleButton parent;
};

typedef enum {
   HILDON_TOGGLE_BUTTON_AUTO_WIDTH              = 0,      /* leave width unset */
   HILDON_TOGGLE_BUTTON_HALFSCREEN_WIDTH        = 1,      /* set to 50% screen width */
   HILDON_TOGGLE_BUTTON_FULLSCREEN_WIDTH        = 2,      /* set to 100% screen width */
   HILDON_TOGGLE_BUTTON_AUTO_HEIGHT             = 0 << 2, /* leave height unset */
   HILDON_TOGGLE_BUTTON_FINGER_HEIGHT           = 1 << 2, /* set to finger height */
   HILDON_TOGGLE_BUTTON_THUMB_HEIGHT            = 2 << 2, /* set to thumb height */
}                                               HildonToggleButtonFlags;

GType
hildon_toggle_button_get_type                   (void) G_GNUC_CONST;

GtkWidget *
hildon_toggle_button_new                        (HildonToggleButtonFlags flags);

GtkWidget *
hildon_toggle_button_new_with_label             (HildonToggleButtonFlags  flags,
                                                 const char              *label);

G_END_DECLS

#endif /* __HILDON_TOGGLE_BUTTON_H__ */
