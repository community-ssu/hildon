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

#ifndef                                         __HILDON_WEEKDAY_PICKER_H__
#define                                         __HILDON_WEEKDAY_PICKER_H__

#include                                        <gtk/gtkcontainer.h>

G_BEGIN_DECLS

typedef struct                                  _HildonWeekdayPicker HildonWeekdayPicker;

typedef struct                                  _HildonWeekdayPickerClass HildonWeekdayPickerClass;

#define                                         HILDON_TYPE_WEEKDAY_PICKER \
                                                (hildon_weekday_picker_get_type())

#define                                         HILDON_WEEKDAY_PICKER(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_WEEKDAY_PICKER, \
                                                HildonWeekdayPicker))

#define                                         HILDON_WEEKDAY_PICKER_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_WEEKDAY_PICKER, HildonWeekdayPickerClass))

#define                                         HILDON_IS_WEEKDAY_PICKER(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_WEEKDAY_PICKER))

#define                                         HILDON_IS_WEEKDAY_PICKER_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_WEEKDAY_PICKER))

#define                                         HILDON_WEEKDAY_PICKER_GET_CLASS(obj) \
                                                ((HildonWeekdayPickerClass *) G_OBJECT_GET_CLASS(obj))

struct _HildonWeekdayPicker 
{
    GtkContainer parent;
};

struct _HildonWeekdayPickerClass 
{
    GtkContainerClass parent_class;

    void (*selection_changed) (HildonWeekdayPicker *self);
};
  
GType G_GNUC_CONST
hildon_weekday_picker_get_type                  (void);

GtkWidget*
hildon_weekday_picker_new                       (void);

void 
hildon_weekday_picker_set_day                   (HildonWeekdayPicker *picker,
                                                 GDateWeekday day);

void 
hildon_weekday_picker_unset_day                 (HildonWeekdayPicker *picker,
                                                 GDateWeekday day);

void
hildon_weekday_picker_toggle_day                (HildonWeekdayPicker *picker,
                                                 GDateWeekday day);

void
hildon_weekday_picker_set_all                   (HildonWeekdayPicker *picker);

void 
hildon_weekday_picker_unset_all                 (HildonWeekdayPicker *picker);

gboolean
hildon_weekday_picker_isset_day                 (HildonWeekdayPicker * picker,
                                                 GDateWeekday day);

G_END_DECLS

#endif                                          /* __HILDON_WEEKDAY_PICKER_H__ */
