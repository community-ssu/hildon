/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version. or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __HILDON_TOUCH_PICKER_H__
#define __HILDON_TOUCH_PICKER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define HILDON_TYPE_TOUCH_PICKER             (hildon_touch_picker_get_type ())
#define HILDON_TOUCH_PICKER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_TOUCH_PICKER, HildonTouchPicker))
#define HILDON_TOUCH_PICKER_CLASS(vtable)    (G_TYPE_CHECK_CLASS_CAST ((vtable), HILDON_TYPE_TOUCH_PICKER, HildonTouchPickerClass))
#define HILDON_IS_TOUCH_PICKER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_TOUCH_PICKER))
#define HILDON_IS_TOUCH_PICKER_CLASS(vtable) (G_TYPE_CHECK_CLASS_TYPE ((vtable), HILDON_TYPE_TOUCH_PICKER))
#define HILDON_TOUCH_PICKER_GET_CLASS(inst)  (G_TYPE_INSTANCE_GET_CLASS ((inst), HILDON_TYPE_TOUCH_PICKER, HildonTouchPickerClass))

typedef struct _HildonTouchPicker HildonTouchPicker;
typedef struct _HildonTouchPickerClass HildonTouchPickerClass;
typedef struct _HildonTouchPickerPrivate HildonTouchPickerPrivate;

typedef gchar *(*HildonTouchPickerPrintFunc) (HildonTouchPicker * picker);

struct _HildonTouchPicker
{
  GtkHBox parent_instance;

  /*< private > */
  HildonTouchPickerPrivate *priv;
};

struct _HildonTouchPickerClass
{
  GtkHBoxClass parent_class;

  /* signals */
  void (*changed) (HildonTouchPicker * picker, gint column);
};

typedef enum
{
  HILDON_TOUCH_PICKER_SELECTION_MODE_SINGLE,
  HILDON_TOUCH_PICKER_SELECTION_MODE_MULTIPLE
} HildonTouchPickerSelectionMode;

/* construction */
GType      hildon_touch_picker_get_type (void) G_GNUC_CONST;
GtkWidget *hildon_touch_picker_new      (void);

/* column related  */
gboolean hildon_touch_picker_append_text_column     (HildonTouchPicker * picker,
                                                     GtkTreeModel * model);

gboolean hildon_touch_picker_append_column          (HildonTouchPicker * picker,
                                                     GtkTreeModel * model,
                                                     GtkCellRenderer * cell_renderer,
                                                     ...);

void hildon_touch_picker_set_column_attributes      (HildonTouchPicker * picker,
                                                     gint column,
                                                     GtkCellRenderer * cell_renderer,
                                                     ...);

gboolean hildon_touch_picker_remove_column          (HildonTouchPicker * picker,
                                                     gint column);

gint hildon_touch_picker_get_num_columns            (HildonTouchPicker * picker);

void hildon_touch_picker_set_column_selection_mode  (HildonTouchPicker * picker,
                                                     HildonTouchPickerSelectionMode mode);

HildonTouchPickerSelectionMode
hildon_touch_picker_get_column_selection_mode       (HildonTouchPicker * picker);

/* get/set active item */
gboolean hildon_touch_picker_get_active_iter  (HildonTouchPicker * picker,
                                               gint column,
                                               GtkTreeIter * iter);
void hildon_touch_picker_set_active_iter      (HildonTouchPicker * picker,
                                               gint column,
                                               GtkTreeIter * iter,
                                               gboolean scroll_to);
GList *hildon_touch_picker_get_selected_rows  (HildonTouchPicker * picker,
                                               gint column);
/* model  */
void hildon_touch_picker_set_model            (HildonTouchPicker * picker,
                                               gint column, GtkTreeModel * model);
GtkTreeModel *hildon_touch_picker_get_model   (HildonTouchPicker * picker,
                                               gint column);

/* show the current element selected */
gchar *hildon_touch_picker_get_current_text (HildonTouchPicker * picker);

void hildon_touch_picker_set_print_func     (HildonTouchPicker * picker,
                                             HildonTouchPickerPrintFunc func);

HildonTouchPickerPrintFunc
hildon_touch_picker_get_print_func          (HildonTouchPicker * picker);

G_END_DECLS

#endif /* __HILDON_TOUCH_PICKER_H__ */
