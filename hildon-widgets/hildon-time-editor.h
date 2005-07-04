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

#ifndef __HILDON_TIME_EDITOR_H__
#define __HILDON_TIME_EDITOR_H__

#include <gtk/gtkcontainer.h>

G_BEGIN_DECLS
#define HILDON_TYPE_TIME_EDITOR ( hildon_time_editor_get_type() )
#define HILDON_TIME_EDITOR(obj) (GTK_CHECK_CAST (obj,\
        HILDON_TYPE_TIME_EDITOR, HildonTimeEditor))
#define HILDON_TIME_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_TIME_EDITOR, HildonTimeEditorClass))
#define HILDON_IS_TIME_EDITOR(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_TYPE_TIME_EDITOR))
#define HILDON_IS_TIME_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass),\
        HILDON_TYPE_TIME_EDITOR))
typedef struct _HildonTimeEditor HildonTimeEditor;
typedef struct _HildonTimeEditorClass HildonTimeEditorClass;

struct _HildonTimeEditor {
    GtkContainer parent;
};

struct _HildonTimeEditorClass {
    GtkContainerClass parent_class;
};

/**
 * HildonTimeEditorField:
 * @HILDON_TIME_EDITOR_HOUR_FIELD: select and highlight hour field.
 * @HILDON_TIME_EDITOR_MINUTE_FIELD: select and highlight minute field.
 * @HILDON_TIME_EDITOR_SECOND_FIELD: select and highlight second field.
 *
 * These values are used to indicate which editor field should be highlighted.
 */
typedef enum {
    HILDON_TIME_EDITOR_HOUR_FIELD,
    HILDON_TIME_EDITOR_MINUTE_FIELD,
    HILDON_TIME_EDITOR_SECOND_FIELD
} HildonTimeEditorField;

GType hildon_time_editor_get_type(void);
GtkWidget *hildon_time_editor_new(void);

void hildon_time_editor_set_time(HildonTimeEditor * editor, guint hours,
                                 guint minutes, guint seconds);
void hildon_time_editor_get_time(HildonTimeEditor * editor, guint * hours,
                                 guint * minutes, guint * seconds);

void hildon_time_editor_show_seconds(HildonTimeEditor * editor,
                                     gboolean enable);
void hildon_time_editor_enable_duration_mode(HildonTimeEditor * editor,
                                             gboolean enable);

void hildon_time_editor_set_duration_range(HildonTimeEditor * editor,
                                           guint min_seconds,
                                           guint max_seconds);
void hildon_time_editor_get_duration_range(HildonTimeEditor * editor,
                                           guint * min_seconds,
                                           guint * max_seconds);


G_END_DECLS
#endif
