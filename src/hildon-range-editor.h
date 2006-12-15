/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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

#ifndef                                         __HILDON_RANGE_EDITOR_H__
#define                                         __HILDON_RANGE_EDITOR_H__

#include                                        <gtk/gtkcontainer.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_RANGE_EDITOR \
                                                (hildon_range_editor_get_type())

#define                                         HILDON_RANGE_EDITOR(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_RANGE_EDITOR, HildonRangeEditor))

#define                                         HILDON_RANGE_EDITOR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_RANGE_EDITOR, HildonRangeEditorClass))

#define                                         HILDON_IS_RANGE_EDITOR(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_RANGE_EDITOR))

#define                                         HILDON_IS_RANGE_EDITOR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_RANGE_EDITOR))

#define                                         HILDON_RANGE_EDITOR_GET_CLASS(obj) \
                                                ((HildonRangeEditorClass *) G_OBJECT_GET_CLASS(obj))

typedef struct                                  _HildonRangeEditor HildonRangeEditor;

typedef struct                                  _HildonRangeEditorClass HildonRangeEditorClass;

struct _HildonRangeEditor 
{
    GtkContainer parent;
};

struct _HildonRangeEditorClass 
{
    GtkContainerClass parent_class;
};

GType G_GNUC_CONST
hildon_range_editor_get_type                    (void);

GtkWidget*
hildon_range_editor_new_with_separator          (const gchar * separator);

GtkWidget*
hildon_range_editor_new                         (void);

void
hildon_range_editor_set_range                   (HildonRangeEditor *editor,
                                                 gint start, 
                                                 gint end);

void 
hildon_range_editor_get_range                   (HildonRangeEditor *editor,
                                                 gint * start, 
                                                 gint * end);

void
hildon_range_editor_set_limits                  (HildonRangeEditor *editor,
                                                 gint start, 
                                                 gint end);

void 
hildon_range_editor_set_lower                   (HildonRangeEditor *editor, 
                                                 gint value);

void 
hildon_range_editor_set_higher                  (HildonRangeEditor *editor, 
                                                 gint value);

gint 
hildon_range_editor_get_lower                   (HildonRangeEditor *editor);

gint
hildon_range_editor_get_higher                  (HildonRangeEditor *editor);

void
hildon_range_editor_set_min                     (HildonRangeEditor *editor, 
                                                 gint value);

void
hildon_range_editor_set_max                     (HildonRangeEditor *editor, 
                                                 gint value);

gint
hildon_range_editor_get_min                     (HildonRangeEditor *editor);

gint
hildon_range_editor_get_max                     (HildonRangeEditor *editor);

void 
hildon_range_editor_set_separator               (HildonRangeEditor *editor,
                                                 const gchar *separator);

const gchar*
hildon_range_editor_get_separator               (HildonRangeEditor *editor);

G_END_DECLS

#endif                                          /* __HILDON_RANGE_EDITOR_H__ */
