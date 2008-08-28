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

#ifndef                                         __HILDON_EDIT_TOOLBAR_H__
#define                                         __HILDON_EDIT_TOOLBAR_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_EDIT_TOOLBAR \
                                                (hildon_edit_toolbar_get_type())

#define                                         HILDON_EDIT_TOOLBAR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_EDIT_TOOLBAR, HildonEditToolbar))

#define                                         HILDON_EDIT_TOOLBAR_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_EDIT_TOOLBAR, HildonEditToolbarClass))

#define                                         HILDON_IS_EDIT_TOOLBAR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_EDIT_TOOLBAR))

#define                                         HILDON_IS_EDIT_TOOLBAR_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_EDIT_TOOLBAR))

#define                                         HILDON_EDIT_TOOLBAR_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_EDIT_TOOLBAR, HildonEditToolbarClass))

typedef struct                                  _HildonEditToolbar HildonEditToolbar;

typedef struct                                  _HildonEditToolbarClass HildonEditToolbarClass;

struct                                          _HildonEditToolbarClass
{
    GtkHBoxClass parent_class;
};

struct                                          _HildonEditToolbar
{
    GtkHBox parent;
};

GType
hildon_edit_toolbar_get_type                    (void) G_GNUC_CONST;

GtkWidget *
hildon_edit_toolbar_new                         (void);

GtkWidget *
hildon_edit_toolbar_new_with_text               (const gchar *label,
                                                 const gchar *button);

void
hildon_edit_toolbar_set_label                   (HildonEditToolbar *toolbar,
                                                 const gchar       *label);

void
hildon_edit_toolbar_set_button_label            (HildonEditToolbar *toolbar,
                                                 const gchar       *label);

G_END_DECLS

#endif /* __HILDON_EDIT_TOOLBAR_H__ */
