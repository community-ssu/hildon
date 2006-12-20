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

#ifndef                                         __HILDON_TIME_PICKER_PRIVATE_H__
#define                                         __HILDON_TIME_PICKER_PRIVATE_H__

G_BEGIN_DECLS

typedef struct                                  _HildonTimePickerPrivate HildonTimePickerPrivate;

#define                                         HILDON_TIME_PICKER_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_TIME_PICKER, HildonTimePickerPrivate))

enum
{
    WIDGET_GROUP_HOURS,
    WIDGET_GROUP_10_MINUTES,
    WIDGET_GROUP_1_MINUTES,
    WIDGET_GROUP_AMPM,

    WIDGET_GROUP_COUNT
};

enum
{
    BUTTON_UP,
    BUTTON_DOWN,

    BUTTON_COUNT
};

typedef struct
{
    GtkWidget *frame;
    GtkWidget *eventbox;
    GtkLabel *label;

    /* buttons are used for hours and minutes, but not for am/pm */
    GtkWidget *buttons[BUTTON_COUNT];

}                                               HildonTimePickerWidgetGroup;

struct                                          _HildonTimePickerPrivate
{
    HildonTimePickerWidgetGroup widgets[WIDGET_GROUP_COUNT];

    gchar *am_symbol;
    gchar *pm_symbol;

    guint key_repeat;
    guint minutes; /* time in minutes since midnight */
    gint mul; /* for key repeat handling */
    guint timer_id;

    guint show_ampm        : 1; /* 12 hour clock, show AM/PM */
    guint ampm_left        : 1;
    guint button_press     : 1;
    guint start_key_repeat : 1;
};

G_END_DECLS

#endif                                          /* __HILDON_TIME_PICKER_PRIVATE_H__ */
