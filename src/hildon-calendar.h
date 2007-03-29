/*
 * This file is a part of hildon
 *
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * GTK Calendar Widget
 * Copyright (C) 1998 Cesar Miquel and Shawn T. Amundson

 * HldonCalendar modifications
 * Copyright (C) 2005, 2006 Nokia Corporation. 
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
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

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef                                         __HILDON_CALENDAR_H__
#define                                         __HILDON_CALENDAR_H__

#include                                        <gdk/gdk.h>
#include                                        <gtk/gtkwidget.h>

/* Not needed, retained for compatibility -Yosh */
#include                                        <gtk/gtksignal.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_CALENDAR \
                                                (hildon_calendar_get_type ())

#define                                         HILDON_CALENDAR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_CALENDAR, HildonCalendar))

#define                                         HILDON_CALENDAR_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_CALENDAR, HildonCalendarClass))

#define                                         HILDON_IS_CALENDAR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_CALENDAR))

#define                                         HILDON_IS_CALENDAR_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_CALENDAR))

#define                                         HILDON_CALENDAR_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), HILDON_TYPE_CALENDAR, HildonCalendarClass))

typedef struct                                  _HildonCalendar HildonCalendar;

typedef struct                                  _HildonCalendarClass HildonCalendarClass;

typedef enum
{
    HILDON_CALENDAR_SHOW_HEADING                = 1 << 0,
    HILDON_CALENDAR_SHOW_DAY_NAMES              = 1 << 1,
    HILDON_CALENDAR_NO_MONTH_CHANGE             = 1 << 2,
    HILDON_CALENDAR_SHOW_WEEK_NUMBERS           = 1 << 3,
    HILDON_CALENDAR_WEEK_START_MONDAY           = 1 << 4
} HildonCalendarDisplayOptions;

struct                                          _HildonCalendar
{
    GtkWidget widget;

    GtkStyle  *header_style;
    GtkStyle  *label_style;

    gint month;
    gint year;
    gint selected_day;

    gint day_month[6][7];
    gint day[6][7];

    gint num_marked_dates;
    gint marked_date[31];
    HildonCalendarDisplayOptions  display_flags;
    GdkColor marked_date_color[31];

    GdkGC *gc;
    GdkGC *xor_gc;

    gint focus_row;
    gint focus_col;

    gint highlight_row;
    gint highlight_col;

    gpointer private_data;
    gchar grow_space [32];

    /* Padding for future expansion */
    void (*_gtk_reserved1) (void);
    void (*_gtk_reserved2) (void);
    void (*_gtk_reserved3) (void);
    void (*_gtk_reserved4) (void);
};

struct                                          _HildonCalendarClass
{
    GtkWidgetClass parent_class;

    /* Signal handlers */
    void (* month_changed)                      (HildonCalendar *calendar);
    void (* day_selected)                       (HildonCalendar *calendar);
    void (* day_selected_double_click)          (HildonCalendar *calendar);
    void (* prev_month)                         (HildonCalendar *calendar);
    void (* next_month)                         (HildonCalendar *calendar);
    void (* prev_year)                          (HildonCalendar *calendar);
    void (* next_year)                          (HildonCalendar *calendar);
};


GType G_GNUC_CONST
hildon_calendar_get_type                        (void);

GtkWidget* 
hildon_calendar_new                             (void);

gboolean   
hildon_calendar_select_month                    (HildonCalendar *calendar, 
                                                 guint month,
                                                 guint year);

void       
hildon_calendar_select_day                      (HildonCalendar *calendar,
                                                 guint day);

gboolean   
hildon_calendar_mark_day                        (HildonCalendar *calendar,
                                                 guint day);

gboolean   
hildon_calendar_unmark_day                      (HildonCalendar *calendar,
                                                 guint day);

void    
hildon_calendar_clear_marks                     (HildonCalendar *calendar);

void       
hildon_calendar_set_display_options             (HildonCalendar *calendar,
                                                 HildonCalendarDisplayOptions flags);

HildonCalendarDisplayOptions
hildon_calendar_get_display_options             (HildonCalendar *calendar);

void       
hildon_calendar_get_date                        (HildonCalendar *calendar, 
                                                 guint *year,
                                                 guint *month,
                                                 guint *day);

void      
hildon_calendar_freeze                          (HildonCalendar *calendar);

void    
hildon_calendar_thaw                            (HildonCalendar *calendar);

G_END_DECLS

#endif                                          /* __HILDON_CALENDAR_H__ */
