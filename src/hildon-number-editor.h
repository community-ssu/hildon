/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

#ifndef                                         __HILDON_NUMBER_EDITOR_H__
#define                                         __HILDON_NUMBER_EDITOR_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_NUMBER_EDITOR \
                                                (hildon_number_editor_get_type())

#define                                         HILDON_NUMBER_EDITOR(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_NUMBER_EDITOR, HildonNumberEditor))

#define                                         HILDON_NUMBER_EDITOR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_NUMBER_EDITOR, \
                                                HildonNumberEditorClass))

#define                                         HILDON_IS_NUMBER_EDITOR(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_NUMBER_EDITOR))

#define                                         HILDON_IS_NUMBER_EDITOR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_NUMBER_EDITOR))

#define                                         HILDON_NUMBER_EDITOR_GET_CLASS(obj) \
                                                ((HildonNumberEditorClass *) G_OBJECT_GET_CLASS(obj))

typedef struct                                  _HildonNumberEditor HildonNumberEditor;

typedef struct                                  _HildonNumberEditorClass HildonNumberEditorClass;

struct                                          _HildonNumberEditor 
{
    GtkContainer parent;
};

typedef enum
{
    HILDON_NUMBER_EDITOR_ERROR_MAXIMUM_VALUE_EXCEED,
    HILDON_NUMBER_EDITOR_ERROR_MINIMUM_VALUE_EXCEED,
    HILDON_NUMBER_EDITOR_ERROR_ERRONEOUS_VALUE
}                                               HildonNumberEditorErrorType;

struct                                          _HildonNumberEditorClass 
{
    GtkContainerClass parent_class;
  
    gboolean  (*range_error)  (HildonNumberEditor *editor, HildonNumberEditorErrorType type); 
};

GType G_GNUC_CONST
hildon_number_editor_get_type                   (void);

GtkWidget*  
hildon_number_editor_new                        (gint min, gint max);

void
hildon_number_editor_set_range                  (HildonNumberEditor *editor, 
                                                 gint min,
                                                 gint max);

gint
hildon_number_editor_get_value                  (HildonNumberEditor *editor);

void
hildon_number_editor_set_value                  (HildonNumberEditor *editor, 
                                                 gint value);

G_END_DECLS

#endif                                          /* __HILDON_NUMBER_EDITOR_H__ */
