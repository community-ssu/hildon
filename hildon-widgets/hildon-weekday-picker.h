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
 * the License or any later version.
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

#ifndef __HILDON_WEEKDAY_PICKER_H__
#define __HILDON_WEEKDAY_PICKER_H__

#include <gtk/gtkcontainer.h>

G_BEGIN_DECLS
/**
 * HILDON_TYPE_WEEKDAY_PICKER:
 *
 * Macro for getting type of weekday picker.
 * Since: 0.12.10
 */
#define HILDON_TYPE_WEEKDAY_PICKER ( hildon_weekday_picker_get_type() )

/**
 * HILDON_WEEKDAY_PICKER_TYPE:
 *
 * Deprecated: use #HILDON_TYPE_WEEKDAY_PICKER instead.
 */
#define HILDON_WEEKDAY_PICKER_TYPE HILDON_TYPE_WEEKDAY_PICKER

#define HILDON_WEEKDAY_PICKER(obj) \
        (GTK_CHECK_CAST (obj, HILDON_TYPE_WEEKDAY_PICKER, \
         HildonWeekdayPicker))
#define HILDON_WEEKDAY_PICKER_CLASS(klass) \
        (GTK_CHECK_CLASS_CAST ((klass), \
         HILDON_TYPE_WEEKDAY_PICKER, HildonWeekdayPickerClass))
#define HILDON_IS_WEEKDAY_PICKER(obj) \
        (GTK_CHECK_TYPE (obj, HILDON_TYPE_WEEKDAY_PICKER))
#define HILDON_IS_WEEKDAY_PICKER_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_WEEKDAY_PICKER))
/**
 * HildonWeekdayPicker:
 *
 * Internal struct for weekday picker.
 */
typedef struct _HildonWeekdayPicker HildonWeekdayPicker;
typedef struct _HildonWeekdayPickerClass HildonWeekdayPickerClass;

struct _HildonWeekdayPicker {
    GtkContainer parent;
};

struct _HildonWeekdayPickerClass {
    GtkContainerClass parent_class;

    void (*selection_changed) (HildonWeekdayPicker * self);
};
  
GType hildon_weekday_picker_get_type(void) G_GNUC_CONST;

/**
 * hildon_weekday_picker_new:
 *
 * Creates a new #HildonWeekdayPicker.
 * 
 * Return value: New #HildonWeekdayPicker.
 **/
GtkWidget *hildon_weekday_picker_new(void);

/**
 * hildon_weekday_picker_set_day:
 * @picker: #HildonWeekdayPicker.
 * @day: #GDateWeekday.
 *
 * Select specified weekday.
 *
 **/
void hildon_weekday_picker_set_day(HildonWeekdayPicker * picker,
                                   GDateWeekday day);

/**
 * hildon_weekday_picker_unset_day:
 * @picker: #HildonWeekdayPicker.
 * @day: #GDateWeekday.
 *
 * Unselect specified weekday.
 *
 **/
void hildon_weekday_picker_unset_day(HildonWeekdayPicker * picker,
                                     GDateWeekday day);

/**
 * hildon_weekday_picker_toggle_day:
 * @picker: #HildonWeekdayPicker.
 * @day: #GDateWeekday.
 *
 * Toggle current status of the specified weekday.
 *
 **/
void hildon_weekday_picker_toggle_day(HildonWeekdayPicker * picker,
                                      GDateWeekday day);

/**
 * hildon_weekday_picker_set_all:
 * @picker: #HildonWeekdayPicker.
 *
 * Select all weekdays. 
 *
 **/
void hildon_weekday_picker_set_all(HildonWeekdayPicker * picker);

/**
 * hildon_weekday_picker_unset_all:
 * @picker: #HildonWeekdayPicker.
 *
 * Unselect all weekdays. 
 *
 **/
void hildon_weekday_picker_unset_all(HildonWeekdayPicker * picker);

/**
 * hildon_weekday_picker_isset_day:
 * @picker: #HildonWeekdayPicker.
 * @day: #GDateWeekday.
 *
 * Check if the specified weekday is set. 
 *
 * Return value: Set/not set.
 **/
gboolean hildon_weekday_picker_isset_day(HildonWeekdayPicker * picker,
                                         GDateWeekday day);

G_END_DECLS
#endif /* __HILDON_WEEKDAY_PICKER_H__ */
