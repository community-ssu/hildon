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

#ifndef __HILDON_DATE_EDITOR_H__
#define __HILDON_DATE_EDITOR_H__

#include <gtk/gtkcontainer.h>
#include "hildon-time-editor.h"

G_BEGIN_DECLS
/**
 * HILDON_TYPE_DATE_EDITOR:
 *
 * Macro for getting type of date editor.
 * Since: 0.12.10
 */
#define HILDON_TYPE_DATE_EDITOR ( hildon_date_editor_get_type() )

#define HILDON_DATE_EDITOR(obj) (GTK_CHECK_CAST (obj,\
  HILDON_TYPE_DATE_EDITOR, HildonDateEditor))
#define HILDON_DATE_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
  HILDON_TYPE_DATE_EDITOR, HildonDateEditorClass))
#define HILDON_IS_DATE_EDITOR(obj) (GTK_CHECK_TYPE (obj,\
  HILDON_TYPE_DATE_EDITOR))
#define HILDON_IS_DATE_EDITOR_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass),\
  HILDON_TYPE_DATE_EDITOR))

/**
 * HildonDateEditor:
 *
 * Internal struct for date editor.
 */
typedef struct _HildonDateEditor HildonDateEditor;
typedef struct _HildonDateEditorClass HildonDateEditorClass;

struct _HildonDateEditor {
    GtkContainer par;
};

struct _HildonDateEditorClass {
    GtkContainerClass parent_class;

    gboolean (*date_error) (HildonDateEditor *editor, 
			    HildonDateTimeEditorError type);
};

GType hildon_date_editor_get_type(void) G_GNUC_CONST;

GtkWidget *hildon_date_editor_new(void);

void hildon_date_editor_set_date(HildonDateEditor * date,
                                 guint year, guint month, guint day);

void hildon_date_editor_get_date(HildonDateEditor * date,
                                 guint * year, guint * month, guint * day);

gboolean hildon_date_editor_set_year(HildonDateEditor *editor, guint year);
gboolean hildon_date_editor_set_month(HildonDateEditor *editor, guint month);
gboolean hildon_date_editor_set_day(HildonDateEditor *editor, guint day);

guint hildon_date_editor_get_year(HildonDateEditor *editor);
guint hildon_date_editor_get_month(HildonDateEditor *editor);
guint hildon_date_editor_get_day(HildonDateEditor *editor);

G_END_DECLS
#endif /* __HILDON_DATE_EDITOR_H__ */
