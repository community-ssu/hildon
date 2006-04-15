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

#ifndef __HILDON_NUMBER_EDITOR_H__
#define __HILDON_NUMBER_EDITOR_H__

#include <gtk/gtkcontainer.h>

G_BEGIN_DECLS
#define HILDON_TYPE_NUMBER_EDITOR \
  ( hildon_number_editor_get_type() )
#define HILDON_NUMBER_EDITOR(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_NUMBER_EDITOR, HildonNumberEditor))
#define HILDON_NUMBER_EDITOR_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_NUMBER_EDITOR, \
  HildonNumberEditorClass))
#define HILDON_IS_NUMBER_EDITOR(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_NUMBER_EDITOR))
#define HILDON_IS_NUMBER_EDITOR_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_NUMBER_EDITOR))
typedef struct _HildonNumberEditor HildonNumberEditor;
typedef struct _HildonNumberEditorClass HildonNumberEditorClass;

struct _HildonNumberEditor 
{
  GtkContainer parent;
};

typedef enum
{
  MAXIMUM_VALUE_EXCEED,
  MINIMUM_VALUE_EXCEED,
  ERRONEOUS_VALUE

}HildonNumberEditorErrorType;

struct _HildonNumberEditorClass 
{
  GtkContainerClass parent_class;
  
  gboolean	(*range_error)	(HildonNumberEditor *editor, 
					 HildonNumberEditorErrorType type); 
};

/* Public API */

GType 		hildon_number_editor_get_type	(void) G_GNUC_CONST;

GtkWidget*	hildon_number_editor_new	(gint min, gint max);

void 		hildon_number_editor_set_range	(HildonNumberEditor *editor, 
						 gint min, gint max);

gint 		hildon_number_editor_get_value	(HildonNumberEditor *editor);
void 		hildon_number_editor_set_value	(HildonNumberEditor *editor, 
						 gint value);

G_END_DECLS
#endif /* __HILDON_NUMBER_EDITOR_H__ */
