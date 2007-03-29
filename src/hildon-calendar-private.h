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

#ifndef                                         __HILDON_CALENDAR_PRIVATE_H__
#define                                         __HILDON_CALENDAR_PRIVATE_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_CALENDAR_GET_PRIVATE(widget)  \
                                                (((HildonCalendarPrivate*)(HILDON_CALENDAR (widget)->private_data)))

typedef struct                                  _HildonCalendarPrivate HildonCalendarPrivate;

struct                                          _HildonCalendarPrivate
{
    GdkWindow *header_win;
    GdkWindow *footer_win;
    GdkWindow *day_name_win;
    GdkWindow *main_win;
    GdkWindow *week_win;
    GdkWindow *arrow_win[4];

    gint year_before;
    guint header_h;
    guint day_name_h;
    guint main_h;

    guint      arrow_state[4];
    /* guint           arrow_width;  This is now defined constant. Even normal Gtk don't allow to change this */ 
    guint      arrow_width;
    guint      max_month_width;
    guint      max_year_width;

    guint day_width;
    guint week_width;

    guint min_day_width;
    guint max_day_char_width;
    guint max_day_char_ascent;
    guint max_day_char_descent;
    guint max_label_char_ascent;
    guint max_label_char_descent;
    guint max_week_char_width;

    guint freeze_count;

    /* flags */
    guint dirty_header : 1;
    guint dirty_day_names : 1;
    guint dirty_main : 1;
    guint dirty_week : 1;

    /*  guint year_before : 1;*/

    guint need_timer  : 1;

    guint in_drag : 1;
    guint drag_highlight : 1;

    guint32 timer;
    gint click_child;

    /* Following variables are for current date */
    guint current_day;
    guint current_month;
    guint current_year;

    /* Keep track of day and month
     * where mouse button was pressed
     */
    guint pressed_day;
    guint pressed_month;

    /* Boolean value to indicate if
     * out of bound day was selected
     */
    gboolean is_bad_day;

    /* Must check if we are sliding stylus */
    gboolean slide_stylus;
    gint prev_row;
    gint prev_col;

    gint week_start;

    gint drag_start_x;
    gint drag_start_y;

    gint min_year;
    gint max_year;

    char *abbreviated_dayname[7];
    char *monthname[12];
};

G_END_DECLS

#endif                                          /* __HILDON_CALENDAR_PRIVATE_H__ */
