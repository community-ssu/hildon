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
#ifndef __HILDON_TIME_PICKER_H__
#define __HILDON_TIME_PICKER_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS


#define HILDON_TYPE_TIME_PICKER (hildon_time_picker_get_type())
#define HILDON_TIME_PICKER(obj) (GTK_CHECK_CAST (obj, \
                                 HILDON_TYPE_TIME_PICKER, \
                                 HildonTimePicker))
#define HILDON_TIME_PICKER_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass), \
                                         HILDON_TYPE_TIME_PICKER,\
                                         HildonTimePickerClass))
#define HILDON_IS_TIME_PICKER(obj) (GTK_CHECK_TYPE (obj, \
                                    HILDON_TYPE_TIME_PICKER))
#define HILDON_IS_TIME_PICKER_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass),\
                                            HILDON_TYPE_TIME_PICKER))


typedef struct _HildonTimePicker HildonTimePicker;
typedef struct _HildonTimePickerClass HildonTimePickerClass;
typedef struct _HildonTimePickerPrivate HildonTimePickerPrivate;


struct _HildonTimePicker
{
  GtkDialog parent;
  HildonTimePickerPrivate *priv;
};

struct _HildonTimePickerClass
{
  GtkDialogClass parent_class;
};


GType hildon_time_picker_get_type( void ) G_GNUC_CONST;

GtkWidget *hildon_time_picker_new( GtkWindow *parent );

void hildon_time_picker_set_time( HildonTimePicker *picker,
                                  guint hours, guint minutes );

void hildon_time_picker_get_time( HildonTimePicker *picker,
                                  guint *hours, guint *minutes );


G_END_DECLS
#endif /* __HILDON_TIME_PICKER_H__ */
