/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef HILDON_RANGE_EDITOR_H
#define HILDON_RANGE_EDITOR_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtkcontainer.h>
#include <gtk/gtkhbox.h>


G_BEGIN_DECLS
/**
 * HILDON_RANGE_EDITOR_TYPE
 *
 * Macro for getting type of range editor.
 */
#define HILDON_RANGE_EDITOR_TYPE (hildon_range_editor_get_type())
#define HILDON_RANGE_EDITOR(obj) \
        (GTK_CHECK_CAST (obj, HILDON_RANGE_EDITOR_TYPE, HildonRangeEditor))
#define HILDON_RANGE_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
         HILDON_RANGE_EDITOR_TYPE, HildonRangeEditorClass))
#define HILDON_IS_RANGE_EDITOR(obj) \
        (GTK_CHECK_TYPE (obj, HILDON_RANGE_EDITOR_TYPE))
#define HILDON_IS_RANGE_EDITOR_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_RANGE_EDITOR_TYPE))
/**
 * HildonRangeEditor:
 *
 * Internal struct for range editor.
 */
typedef struct _HildonRangeEditor HildonRangeEditor;
typedef struct _HildonRangeEditorClass HildonRangeEditorClass;


struct _HildonRangeEditor {
    GtkContainer parent;
};

struct _HildonRangeEditorClass {
    GtkContainerClass parent_class;
};

GType hildon_range_editor_get_type(void) G_GNUC_CONST;

GtkWidget *hildon_range_editor_new_with_separator(gchar * separator);
GtkWidget *hildon_range_editor_new(void);

void hildon_range_editor_set_range(HildonRangeEditor * editor,
                                   gint start, gint end);
void hildon_range_editor_get_range(HildonRangeEditor * editor,
                                   gint * start, gint * end);
void hildon_range_editor_set_limits(HildonRangeEditor * editor,
                                    gint start, gint end);

G_END_DECLS
#endif
