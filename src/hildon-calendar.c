/*
 * This file is a part of hildon
 *
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * GTK Calendar Widget
 * Copyright (C) 1998 Cesar Miquel, Shawn T. Amundson and Mattias Groenlund
 * 
 * lib_date routines
 * Copyright (C) 1995, 1996, 1997, 1998 by Steffen Beyer
 * 
 * HldonCalendar modifications
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
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

/**
 * SECTION:hildon-calendar
 * @short_description: A calendar widget
 *
 * HildonCalendar is a slightly modified #GtkCalendar. It has an almost same API 
 * but a slightly different look and behaviour. Use this widget instead of 
 * standard #GtkCalendar or use #HildonDateEditor for more higher-level date setting
 * operations.
 *
 */    

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#define                                         _GNU_SOURCE /* needed for GNU nl_langinfo_l */

#ifdef                                          HAVE_SYS_TIME_H
#include                                        <sys/time.h>
#endif

#include                                        <string.h>
#include                                        <stdlib.h>
#include                                        <time.h>
#include                                        <langinfo.h>
#include                                        <locale.h>
#include                                        <glib/gprintf.h>

#include                                        "hildon-calendar.h"
#include                                        "hildon-marshalers.h"
#include                                        <gtk/gtkdnd.h>
#include                                        <gtk/gtkmain.h>
#include                                        <gdk/gdkkeysyms.h>
#include                                        <gtk/gtkprivate.h>
#include                                        "hildon-calendar-private.h"

/***************************************************************************/
/* The following date routines are taken from the lib_date package.  Keep
 * them separate in case we want to update them if a newer lib_date comes
 * out with fixes.  */

typedef unsigned int                            N_int;

typedef unsigned long                           N_long;

typedef signed long                             Z_long;

typedef enum                                    { false = FALSE , true = TRUE } boolean;

#define                                         and &&      /* logical (boolean) operators: lower case */

#define                                         or ||

static const                                    N_int month_length [2][13] =
{
    { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
    { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

static const N_int days_in_months[2][14] =
{
    { 0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
    { 0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
};

static                                          Z_long calc_days (N_int year, N_int mm, N_int dd);

static                                          N_int day_of_week (N_int year, N_int mm, N_int dd);

static                                          Z_long dates_difference (N_int year1, N_int mm1, N_int dd1, 
                                                                         N_int year2, N_int mm2, N_int dd2);

static N_int                                    weeks_in_year (N_int year);

static boolean 
leap                                            (N_int year)
{
    return ((((year % 4) == 0) and ((year % 100) != 0)) or ((year % 400) == 0));
}

static N_int 
day_of_week                                     (N_int year, 
                                                 N_int mm, 
                                                 N_int dd)
{
    Z_long days;

    days = calc_days (year, mm, dd);
    if (days > 0L)
    {
        days--;
        days %= 7L;
        days++;
    }
    return( (N_int) days );
}

static N_int 
weeks_in_year                                   (N_int year)
{
    return (52 + ((day_of_week(year,1,1)==4) or (day_of_week(year,12,31)==4)));
}

static boolean 
check_date                                      (N_int year, 
                                                 N_int mm, 
                                                 N_int dd)
{
    if (year < 1) return(false);
    if ((mm < 1) or (mm > 12)) return(false);
    if ((dd < 1) or (dd > month_length[leap(year)][mm])) return(false);
    return(true);
}

static N_int 
week_number                                     (N_int year, 
                                                 N_int mm, 
                                                 N_int dd)
{
    N_int first;

    first = day_of_week (year,1,1) - 1;
    return( (N_int) ( (dates_difference(year,1,1, year,mm,dd) + first) / 7L ) +
            (first < 4) );
}

static Z_long 
year_to_days                                    (N_int year)
{
    return ( year * 365L + (year / 4) - (year / 100) + (year / 400) );
}

static Z_long 
calc_days                                       (N_int year, 
                                                 N_int mm, 
                                                 N_int dd)
{
    boolean lp;

    if (year < 1) return(0L);
    if ((mm < 1) or (mm > 12)) return(0L);
    if ((dd < 1) or (dd > month_length[(lp = leap(year))][mm])) return(0L);
    return( year_to_days(--year) + days_in_months[lp][mm] + dd );
}

static boolean 
week_of_year                                    (N_int *week, 
                                                 N_int *year, 
                                                 N_int mm, 
                                                 N_int dd)
{
    if (check_date(*year,mm,dd))
    {
        *week = week_number(*year,mm,dd);
        if (*week == 0) 
            *week = weeks_in_year(--(*year));
        else if (*week > weeks_in_year(*year))
        {
            *week = 1;
            (*year)++;
        }
        return(true);
    }
    return(false);
}

static Z_long 
dates_difference                                (N_int year1, 
                                                 N_int mm1, 
                                                 N_int dd1,
                                                 N_int year2, 
                                                 N_int mm2, 
                                                 N_int dd2)
{
    return (calc_days (year2, mm2, dd2) - calc_days (year1, mm1, dd1));
}

/*** END OF lib_date routines ********************************************/

/* HILDON: Spacings modified */
#define                                         HILDON_ARROW_SEP 5 /* Space between arrows and data */

#define                                         HILDON_DAY_WIDTH 26

#define                                         HILDON_DAY_HEIGHT 25 

/* additional widths given to week number and day windows */

#define                                         HILDON_WEEKS_EXTRA_WIDTH 8

#define                                         HILDON_DAYS_EXTRA_WIDTH 8

/* Spacing around day/week headers and main area, inside those windows */

#define                                         CALENDAR_MARGIN 0

/* Spacing around day/week headers and main area, outside those windows */

#define                                         INNER_BORDER 0 /* 4 */

/* Separation between day headers and main area */

#define                                         CALENDAR_YSEP 3 /* 4 */

/* Separation between week headers and main area */

#define                                         CALENDAR_XSEP 6 /* 4 */

#define                                         DAY_XSEP 0 /* not really good for small calendar */

#define                                         DAY_YSEP 0 /* not really good for small calendar */

/* Color usage */
#define                                         HEADER_FG_COLOR(widget) \
                                                (& (widget)->style->fg[GTK_WIDGET_STATE (widget)])

#define                                         HEADER_BG_COLOR(widget) \
                                                (& (widget)->style->bg[GTK_WIDGET_STATE (widget)])

#define                                         SELECTED_BG_COLOR(widget) \
                                                (& (widget)->style->base[GTK_WIDGET_HAS_FOCUS (widget) ? GTK_STATE_SELECTED : GTK_STATE_ACTIVE])

#define                                         SELECTED_FG_COLOR(widget) \
                                                (& (widget)->style->text[GTK_WIDGET_HAS_FOCUS (widget) ? GTK_STATE_SELECTED : GTK_STATE_ACTIVE])

#define                                         NORMAL_DAY_COLOR(widget) \
                                                (& (widget)->style->fg[GTK_WIDGET_STATE (widget)])

#define                                         PREV_MONTH_COLOR(widget) \
                                                (& (widget)->style->mid[GTK_WIDGET_STATE (widget)])

#define                                         NEXT_MONTH_COLOR(widget) \
                                                (& (widget)->style->mid[GTK_WIDGET_STATE (widget)])

#define                                         MARKED_COLOR(widget) \
                                                (& (widget)->style->fg[GTK_WIDGET_STATE (widget)])

#define                                         BACKGROUND_COLOR(widget) \
                                                (& (widget)->style->base[GTK_WIDGET_STATE (widget)])

#define                                         HIGHLIGHT_BACK_COLOR(widget) \
                                                (& (widget)->style->mid[GTK_WIDGET_STATE (widget)])

#define                                         CALENDAR_INITIAL_TIMER_DELAY    200

#define                                         CALENDAR_TIMER_DELAY            20

enum {
    ARROW_YEAR_LEFT,
    ARROW_YEAR_RIGHT,
    ARROW_MONTH_LEFT,
    ARROW_MONTH_RIGHT
};

enum {
    MONTH_PREV,
    MONTH_CURRENT,
    MONTH_NEXT
};

enum {
    MONTH_CHANGED_SIGNAL,
    DAY_SELECTED_SIGNAL,
    DAY_SELECTED_DOUBLE_CLICK_SIGNAL,
    PREV_MONTH_SIGNAL,
    NEXT_MONTH_SIGNAL,
    PREV_YEAR_SIGNAL,
    NEXT_YEAR_SIGNAL,
    ERRONEOUS_DATE_SIGNAL,
    SELECTED_DATE_SIGNAL,
    LAST_SIGNAL
};

enum
{
    PROP_0,
    PROP_YEAR,
    PROP_MONTH,
    PROP_DAY,
    PROP_SHOW_HEADING,
    PROP_SHOW_DAY_NAMES,
    PROP_NO_MONTH_CHANGE,
    PROP_SHOW_WEEK_NUMBERS,
    PROP_WEEK_START,
    PROP_MIN_YEAR,
    PROP_MAX_YEAR,
    PROP_LAST
};

static gint                                     hildon_calendar_signals [LAST_SIGNAL] = { 0 };

static GtkWidgetClass*                          parent_class = NULL;

typedef void (*HildonCalendarSignalDate) (GtkObject *object, guint arg1, guint arg2, guint arg3, gpointer data);

static void
hildon_calendar_class_init                      (HildonCalendarClass *class);

static void 
hildon_calendar_init                            (HildonCalendar *calendar);

static void
hildon_calendar_finalize                        (GObject *calendar);

static void 
hildon_calendar_destroy                         (GtkObject *calendar);

static void 
hildon_calendar_set_property                    (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec);

static void
hildon_calendar_get_property                    (GObject *object,
                                                 guint prop_id,
                                                 GValue *value,
                                                 GParamSpec *pspec);

static void
hildon_calendar_realize                         (GtkWidget *widget);

static void
hildon_calendar_unrealize                       (GtkWidget *widget);

static void 
hildon_calendar_size_request                    (GtkWidget *widget,
                                                 GtkRequisition *requisition);

static void 
hildon_calendar_size_allocate                   (GtkWidget *widget,
                                                 GtkAllocation *allocation);

static gint 
hildon_calendar_expose                          (GtkWidget *widget,
                                                 GdkEventExpose *event);

static gint 
hildon_calendar_button_press                    (GtkWidget *widget,
                                                 GdkEventButton *event);

static gint 
hildon_calendar_button_release                  (GtkWidget *widget,
                                                 GdkEventButton *event);

static void
hildon_calendar_main_button                     (GtkWidget *widget,
                                                 GdkEventButton *event);

static gint 
hildon_calendar_motion_notify                   (GtkWidget *widget,
                                                 GdkEventMotion *event);

static gint
hildon_calendar_enter_notify                    (GtkWidget *widget,
                                                 GdkEventCrossing *event);

static gint 
hildon_calendar_leave_notify                    (GtkWidget *widget,
                                                 GdkEventCrossing *event);

static gint 
hildon_calendar_key_press                       (GtkWidget *widget,
                                                 GdkEventKey *event);

static gint 
hildon_calendar_scroll                          (GtkWidget *widget,
                                                 GdkEventScroll *event);

static void 
hildon_calendar_grab_notify                     (GtkWidget *widget,
                                                 gboolean was_grabbed);

static gboolean 
hildon_calendar_focus_out                       (GtkWidget *widget,
                                                 GdkEventFocus *event);

static void 
hildon_calendar_state_changed                   (GtkWidget *widget,
                                                 GtkStateType previous_state);

static void
hildon_calendar_style_set                       (GtkWidget *widget,
                                                 GtkStyle *previous_style);

static void
hildon_calendar_paint_header                    (GtkWidget *widget);

static void 
hildon_calendar_paint_footer                    (GtkWidget *widget);

static void
hildon_calendar_paint_day_names                 (GtkWidget *widget);

static void
hildon_calendar_paint_week_numbers              (GtkWidget *widget);

static void 
hildon_calendar_paint_main                      (GtkWidget *widget);

static void 
hildon_calendar_select_and_focus_day            (HildonCalendar *calendar,
                                                 guint day);

static void 
hildon_calendar_paint_arrow                     (GtkWidget *widget,
                                                 guint arrow);

static void 
hildon_calendar_paint_day_num                   (GtkWidget *widget,
                                                 gint day);

static void 
hildon_calendar_paint_day                       (GtkWidget *widget,
                                                 gint row,
                                                 gint col);

static void
hildon_calendar_compute_days                    (HildonCalendar *calendar);

static gint 
left_x_for_column                               (HildonCalendar  *calendar,
                                                 gint column);

static gint 
top_y_for_row                                   (HildonCalendar  *calendar,
                                                 gint row);

static void 
hildon_calendar_drag_data_get                   (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 GtkSelectionData *selection_data,
                                                 guint info,
                                                 guint time);

static void 
hildon_calendar_drag_data_received              (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 gint x,
                                                 gint y,
                                                 GtkSelectionData *selection_data,
                                                 guint info,
                                                 guint time);

static gboolean 
hildon_calendar_drag_motion                     (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 gint x,
                                                 gint y,
                                                 guint time);

static void 
hildon_calendar_drag_leave                      (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 guint time);

static gboolean
hildon_calendar_drag_drop                       (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 gint x,
                                                 gint y,
                                                 guint time);

/* This function was added because we need to mark current day according to
 * specifications
 */

static void
hildon_calendar_check_current_date              (HildonCalendar *calendar, 
                                                 gint x, 
                                                 gint y);

GType G_GNUC_CONST
hildon_calendar_get_type                        (void)
{
    static GType calendar_type = 0;

    if (!calendar_type)
    {
        static const GTypeInfo calendar_info =
        {
            sizeof (HildonCalendarClass),
            NULL,           /* base_init */
            NULL,           /* base_finalize */
            (GClassInitFunc) hildon_calendar_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof (HildonCalendar),
            0,              /* n_preallocs */
            (GInstanceInitFunc) hildon_calendar_init,
        };

        calendar_type = g_type_register_static (GTK_TYPE_WIDGET, "HildonCalendar",
                &calendar_info, 0);
    }

    return calendar_type;
}

static void
locales_init                                    (HildonCalendarPrivate *priv)
{
    /* Hildon: This is not exactly portable, see
     * http://bugzilla.gnome.org/show_bug.cgi?id=343415
     * The labels need to be instance variables as the startup wizard changes
     * locale on runtime.
     */
    locale_t l;

    l = newlocale (LC_TIME_MASK, setlocale (LC_MESSAGES, NULL), NULL);

    priv->abbreviated_dayname[0] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_1, l),
            -1, NULL, NULL, NULL);
    priv->abbreviated_dayname[1] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_2, l),
            -1, NULL, NULL, NULL);
    priv->abbreviated_dayname[2] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_3, l),
            -1, NULL, NULL, NULL) ;
    priv->abbreviated_dayname[3] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_4, l),
            -1, NULL, NULL, NULL);
    priv->abbreviated_dayname[4] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_5, l),
            -1, NULL, NULL, NULL);
    priv->abbreviated_dayname[5] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_6, l),
            -1, NULL, NULL, NULL);
    priv->abbreviated_dayname[6] = g_locale_to_utf8 (nl_langinfo_l(ABDAY_7, l),
            -1, NULL, NULL, NULL);
    priv->monthname[0] = g_locale_to_utf8 (nl_langinfo_l(MON_1, l),
            -1, NULL, NULL, NULL);
    priv->monthname[1] = g_locale_to_utf8 (nl_langinfo_l(MON_2, l),
            -1, NULL, NULL, NULL);
    priv->monthname[2] = g_locale_to_utf8 (nl_langinfo_l(MON_3, l),
            -1, NULL, NULL, NULL);
    priv->monthname[3] = g_locale_to_utf8 (nl_langinfo_l(MON_4, l),
            -1, NULL, NULL, NULL);
    priv->monthname[4] = g_locale_to_utf8 (nl_langinfo_l(MON_5, l),
            -1, NULL, NULL, NULL);
    priv->monthname[5] = g_locale_to_utf8 (nl_langinfo_l(MON_6, l),
            -1, NULL, NULL, NULL);
    priv->monthname[6] = g_locale_to_utf8 (nl_langinfo_l(MON_7, l),
            -1, NULL, NULL, NULL);
    priv->monthname[7] = g_locale_to_utf8 (nl_langinfo_l(MON_8, l),
            -1, NULL, NULL, NULL);
    priv->monthname[8] = g_locale_to_utf8 (nl_langinfo_l(MON_9, l),
            -1, NULL, NULL, NULL);
    priv->monthname[9] = g_locale_to_utf8 (nl_langinfo_l(MON_10, l),
            -1, NULL, NULL, NULL);
    priv->monthname[10] = g_locale_to_utf8 (nl_langinfo_l(MON_11, l),
            -1, NULL, NULL, NULL);
    priv->monthname[11] = g_locale_to_utf8 (nl_langinfo_l(MON_12, l),
            -1, NULL, NULL, NULL);

    freelocale (l);
}

static void
hildon_calendar_class_init                      (HildonCalendarClass *class)
{
    GObjectClass *gobject_class;
    GtkObjectClass *object_class;
    GtkWidgetClass *widget_class;

    gobject_class = (GObjectClass*) class;
    object_class = (GtkObjectClass*) class;
    widget_class = (GtkWidgetClass*) class;

    parent_class = g_type_class_peek_parent (class);

    gobject_class->set_property             = hildon_calendar_set_property;
    gobject_class->get_property             = hildon_calendar_get_property;
    gobject_class->finalize                 = hildon_calendar_finalize;

    object_class->destroy                   = hildon_calendar_destroy;

    widget_class->realize                   = hildon_calendar_realize;
    widget_class->unrealize                 = hildon_calendar_unrealize;
    widget_class->expose_event              = hildon_calendar_expose;
    widget_class->size_request              = hildon_calendar_size_request;
    widget_class->size_allocate             = hildon_calendar_size_allocate;
    widget_class->button_press_event        = hildon_calendar_button_press;
    widget_class->button_release_event      = hildon_calendar_button_release;
    widget_class->motion_notify_event       = hildon_calendar_motion_notify;
    widget_class->enter_notify_event        = hildon_calendar_enter_notify;
    widget_class->leave_notify_event        = hildon_calendar_leave_notify;
    widget_class->key_press_event           = hildon_calendar_key_press;
    widget_class->scroll_event              = hildon_calendar_scroll;
    widget_class->style_set                 = hildon_calendar_style_set;
    widget_class->state_changed             = hildon_calendar_state_changed;
    widget_class->grab_notify               = hildon_calendar_grab_notify;
    widget_class->focus_out_event           = hildon_calendar_focus_out;

    widget_class->drag_data_get             = hildon_calendar_drag_data_get;
    widget_class->drag_motion               = hildon_calendar_drag_motion;
    widget_class->drag_leave                = hildon_calendar_drag_leave;
    widget_class->drag_drop                 = hildon_calendar_drag_drop;
    widget_class->drag_data_received        = hildon_calendar_drag_data_received;

    class->month_changed = NULL;
    class->day_selected = NULL;
    class->day_selected_double_click = NULL;
    class->prev_month = NULL;
    class->next_month = NULL;
    class->prev_year = NULL;
    class->next_year = NULL;

    /**
     * HildonCalendar:year:
     *
     * The selected year.
     */
    g_object_class_install_property (gobject_class,
            PROP_YEAR,
            g_param_spec_int ("year",
                "Year",
                "The selected year",
                0, G_MAXINT, 0,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:month:
     *
     * The selected month as number between 0 and 11.
     */
    g_object_class_install_property (gobject_class,
            PROP_MONTH,
            g_param_spec_int ("month",
                "Month",
                "The selected month (as a number between 0 and 11)",
                0, 11, 0,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:day:
     *
     * The selected day as number between 1 and 31 or 0 to unselect the currently selected day.
     */
    g_object_class_install_property (gobject_class,
            PROP_DAY,
            g_param_spec_int ("day",
                "Day",
                "The selected day (as a number between 1 and 31, or 0 to unselect the currently selected day)",
                0, 31, 0,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:show-heading:
     *
     * Determines whether a heading is displayed.
     *
     */
    g_object_class_install_property (gobject_class,
            PROP_SHOW_HEADING,
            g_param_spec_boolean ("show-heading",
                "Show Heading",
                "If TRUE, a heading is displayed",
                TRUE,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:show-day-names:
     *
     * Determines whether day names are displayed.
     *
     */
    g_object_class_install_property (gobject_class,
            PROP_SHOW_DAY_NAMES,
            g_param_spec_boolean ("show-day-names",
                "Show Day Names",
                "If TRUE, day names are displayed",
                TRUE,
                GTK_PARAM_READWRITE));
    /**
     * HildonCalendar:no-month-change:
     *
     * Determines whether the selected month can be changed.
     *
     */
    g_object_class_install_property (gobject_class,
            PROP_NO_MONTH_CHANGE,
            g_param_spec_boolean ("no-month-change",
                "No Month Change",
                "If TRUE, the selected month cannot be changed",
                FALSE,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:show-week-numbers:
     *
     * Determines whether week numbers are displayed.
     *
     */
    g_object_class_install_property (gobject_class,
            PROP_SHOW_WEEK_NUMBERS,
            g_param_spec_boolean ("show-week-numbers",
                "Show Week Numbers",
                "If TRUE, week numbers are displayed",
                FALSE,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:week-start:
     *
     * Determines the start day of the week (0 for Sunday, 1 for Monday etc.)
     *
     */
    g_object_class_install_property (gobject_class,
            PROP_WEEK_START,
            g_param_spec_int ("week-start",
                "Week start day",
                "First day of the week; 0 for Sunday, 1 for Monday etc.",
                0, 6, 0,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:min-year:
     *
     * Minimum valid year (0 if no limit).
     * 
     */
    g_object_class_install_property (gobject_class,
            PROP_MIN_YEAR,
            g_param_spec_int ("min-year",
                "Minimum valid year",
                "Minimum valid year (0 if no limit)",
                0, 10000, 0,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:max-year:
     *
     * Maximum valid year (0 if no limit).
     * 
     */
    g_object_class_install_property (gobject_class,
            PROP_MAX_YEAR,
            g_param_spec_int ("max-year",
                "Maximum valid year",
                "Maximum valid year (0 if no limit)",
                0, 10000, 0,
                GTK_PARAM_READWRITE));

    /**
     * HildonCalendar:hildonlike:
     *
     * Changes the appearance and behaviour of HildonCalendar to be consistent with
     * Hildon library.
     * 
     */
    gtk_widget_class_install_style_property (widget_class,
            g_param_spec_boolean ("hildonlike",
                "Size request",
                "Size allocate",
                FALSE,
                GTK_PARAM_READABLE));

    hildon_calendar_signals[MONTH_CHANGED_SIGNAL] =
        g_signal_new ("month_changed",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, month_changed),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    hildon_calendar_signals[DAY_SELECTED_SIGNAL] =
        g_signal_new ("day_selected",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, day_selected),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    hildon_calendar_signals[DAY_SELECTED_DOUBLE_CLICK_SIGNAL] =
        g_signal_new ("day_selected_double_click",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, day_selected_double_click),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    hildon_calendar_signals[PREV_MONTH_SIGNAL] =
        g_signal_new ("prev_month",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, prev_month),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    hildon_calendar_signals[NEXT_MONTH_SIGNAL] =
        g_signal_new ("next_month",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, next_month),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    hildon_calendar_signals[PREV_YEAR_SIGNAL] =
        g_signal_new ("prev_year",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, prev_year),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    hildon_calendar_signals[NEXT_YEAR_SIGNAL] =
        g_signal_new ("next_year",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                G_STRUCT_OFFSET (HildonCalendarClass, next_year),
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    
    /**
     * HildonCalendar::erroneous-date:
     *
     * Emitted when the user tries to set a date which is outside the boundaries 
     * set by min-year and max-year properties.
     * 
     */
    hildon_calendar_signals[ERRONEOUS_DATE_SIGNAL] =
        g_signal_new ("erroneous_date",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_FIRST,
                0,
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
    /**
     * HildonCalendar::selected-date:
     *
     * Emitted on button-release when the user has selected a date.
     * 
     */
    hildon_calendar_signals[SELECTED_DATE_SIGNAL] =
        g_signal_new ("selected_date",
                G_OBJECT_CLASS_TYPE(gobject_class),
                G_SIGNAL_RUN_FIRST,
                0,
                NULL, NULL,
                _hildon_marshal_VOID__VOID,
                G_TYPE_NONE, 0);
}

static void
hildon_calendar_init                            (HildonCalendar *calendar)
{
    time_t secs;
    struct tm *tm;
    gint i;
    /*  char buffer[255];*/
    /*  time_t tmp_time;*/
    GtkWidget *widget;
    HildonCalendarPrivate *private_data;
    /*  gchar *year_before;*/
    /*  gint row;
        gint col; */
    gchar *langinfo;
    GDateWeekday week_1stday;
    gint first_weekday;
    guint week_origin;

    widget = GTK_WIDGET (calendar);
    GTK_WIDGET_SET_FLAGS (widget, GTK_CAN_FOCUS);

    calendar->private_data = g_malloc (sizeof (HildonCalendarPrivate));
    private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);

    /* Set defaults */
    secs = time (NULL);
    tm = localtime (&secs);
    calendar->month = tm->tm_mon;
    calendar->year  = 1900 + tm->tm_year;

    for (i=0;i<31;i++)
        calendar->marked_date[i] = FALSE;
    calendar->num_marked_dates = 0;
    calendar->selected_day = tm->tm_mday; 

    calendar->display_flags = ( HILDON_CALENDAR_SHOW_HEADING | 
            HILDON_CALENDAR_SHOW_DAY_NAMES );

    /* Hildon: we should mark current day  and we need to store current date */
    private_data->current_day  = tm->tm_mday;
    private_data->current_month = tm->tm_mon;
    private_data->current_year = tm->tm_year + 1900;

    /* Hildon: following lines are for stylus sliding */   
    private_data->slide_stylus = FALSE;
    private_data->prev_row = -1;
    private_data->prev_col = -1;

    /* Hildon: is_bad_day indicate if day was selected out of legal range */
    private_data->is_bad_day = FALSE;

    calendar->highlight_row = -1;
    calendar->highlight_col = -1; 

    calendar->focus_row = -1;
    calendar->focus_col = -1; 
    calendar->xor_gc = NULL;

    private_data->max_year_width = 0;
    private_data->max_month_width = 0;
    private_data->max_day_char_width = 0;
    private_data->max_week_char_width = 0;

    private_data->max_day_char_ascent = 0;
    private_data->max_day_char_descent = 0;
    private_data->max_label_char_ascent = 0;
    private_data->max_label_char_descent = 0;

    /*  private_data->arrow_width = 10;*/

    private_data->freeze_count = 0;

    private_data->dirty_header = 0;
    private_data->dirty_day_names = 0;
    private_data->dirty_week = 0;
    private_data->dirty_main = 0;

    private_data->need_timer = 0;
    private_data->timer = 0;
    private_data->click_child = -1;

    private_data->in_drag = 0;
    private_data->drag_highlight = 0;

    private_data->min_year = 0;
    private_data->max_year = 0;

    gtk_drag_dest_set (widget, 0, NULL, 0, GDK_ACTION_COPY);
    gtk_drag_dest_add_text_targets (widget);

#if 0
    private_data->year_before = 0;

    /* Translate to calendar:YM if you want years to be displayed
     * before months; otherwise translate to calendar:MY.
     * Do *not* translate it to anything else, if it
     * it isn't calendar:YM or calendar:MY it will not work.
     *
     * Note that this flipping is in top the text direction flipping,
     * so if you have a default text direction of RTL and YM, then
     * the year will appear on the right.
     */
    year_before = _("calendar:MY");
    if (strcmp (year_before, "calendar:YM") == 0)
        private_data->year_before = 1;
    else if (strcmp (year_before, "calendar:MY") != 0)
        g_warning ("Whoever translated calendar:MY did so wrongly.\n");
#endif
    langinfo = nl_langinfo (_NL_TIME_FIRST_WEEKDAY);
    first_weekday = langinfo[0];
    langinfo = nl_langinfo (_NL_TIME_WEEK_1STDAY);
    week_origin = GPOINTER_TO_UINT (langinfo);
    if (week_origin == 19971130)
        week_1stday = G_DATE_SUNDAY;
    else if (week_origin == 19971201)
        week_1stday = G_DATE_MONDAY;
    else if (g_date_valid_dmy ((week_origin % 100),
                (week_origin / 100) % 100,
                (week_origin / 10000)))
    {
        GDate *date;
        date = g_date_new_dmy ((week_origin % 100),
                (week_origin / 100) % 100,
                (week_origin / 10000));
        week_1stday = g_date_get_weekday (date);
        g_date_free (date);
    }
    else
    {
        g_warning ("Invalid value set for _NL_TIME_WEEK_1STDAY"); 
        week_1stday = G_DATE_SUNDAY;
    }

    private_data->week_start = (week_1stday + first_weekday - 1) % 7;

    locales_init (private_data);
}

GtkWidget*
hildon_calendar_new                             (void)
{
    return g_object_new (HILDON_TYPE_CALENDAR, NULL);
}

/* column_from_x: returns the column 0-6 that the
 * x pixel of the xwindow is in */
static gint
column_from_x                                   (HildonCalendar *calendar,
                                                 gint event_x)
{
    gint c, column;
    gint x_left, x_right;

    column = -1;

    for (c = 0; c < 7; c++)
    {
        x_left = left_x_for_column (calendar, c);
        x_right = x_left + HILDON_CALENDAR_GET_PRIVATE (calendar)->day_width;

        if (event_x >= x_left && event_x < x_right)
        {
            column = c;
            break;
        }
    }

    return column;
}
#if 0
    static gint
row_height (HildonCalendar *calendar)
{
    return (HILDON_CALENDAR_GET_PRIVATE (calendar)->main_h - CALENDAR_MARGIN
            - ((calendar->display_flags & HILDON_CALENDAR_SHOW_DAY_NAMES)
                ? CALENDAR_YSEP : CALENDAR_MARGIN)) / 6;
}
#endif

/* row_from_y: returns the row 0-5 that the
 * y pixel of the xwindow is in */
static gint
row_from_y                                      (HildonCalendar *calendar,
                                                 gint event_y)
{
    gint r, row;
    /*gint height;*/
    gint y_top, y_bottom;

    row = -1;

    for (r = 0; r < 6; r++)
    {
        y_top = top_y_for_row (calendar, r);
        y_bottom = y_top + HILDON_DAY_HEIGHT /*height*/;

        if (event_y >= y_top && event_y < y_bottom)
        {
            row = r;
            break;
        }
    }

    return row;
}

/* left_x_for_column: returns the x coordinate
  * for the left of the column */
static gint
left_x_for_column                               (HildonCalendar *calendar,
                                                 gint column)
{
    gint width;
    gint x_left;

    if (gtk_widget_get_direction (GTK_WIDGET (calendar)) == GTK_TEXT_DIR_RTL)
        column = 6 - column;

    width = HILDON_CALENDAR_GET_PRIVATE (calendar)->day_width;
    if (calendar->display_flags & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
        x_left = CALENDAR_XSEP + (width + DAY_XSEP) * column;
    else
        x_left = CALENDAR_MARGIN + (width + DAY_XSEP) * column;

    return x_left;
}

/* top_y_for_row: returns the y coordinate
 * for the top of the row */
static gint
top_y_for_row                                   (HildonCalendar *calendar,
                                                 gint row)
{
    return (HILDON_CALENDAR_GET_PRIVATE (calendar)->main_h 
            - (CALENDAR_MARGIN + (6 - row)
                * HILDON_DAY_HEIGHT));
}

static void
hildon_calendar_set_month_prev                  (HildonCalendar *calendar)
{
    HildonCalendarPrivate *priv = HILDON_CALENDAR_GET_PRIVATE (calendar);
    gint month_len;

    if (calendar->display_flags & HILDON_CALENDAR_NO_MONTH_CHANGE)
        return;

    if (calendar->month == 0)
    {
        if (!priv->min_year || calendar->year > priv->min_year)
        {
            calendar->month = 11;
            calendar->year--;
        }
    }
    else
        calendar->month--;

    month_len = month_length[leap (calendar->year)][calendar->month + 1];

    hildon_calendar_freeze (calendar);
    hildon_calendar_compute_days (calendar);

    g_signal_emit (calendar,
            hildon_calendar_signals[PREV_MONTH_SIGNAL],
            0);
    g_signal_emit (calendar,
            hildon_calendar_signals[MONTH_CHANGED_SIGNAL],
            0);

    if (month_len < calendar->selected_day)
    {
        calendar->selected_day = 0;
        hildon_calendar_select_day (calendar, month_len);
    }
    else
    {
        if (calendar->selected_day < 0)
            calendar->selected_day = calendar->selected_day + 1 + month_length[leap (calendar->year)][calendar->month + 1];
        hildon_calendar_select_day (calendar, calendar->selected_day);
    }

    gtk_widget_queue_draw (GTK_WIDGET (calendar));
    hildon_calendar_thaw (calendar);
}

static void
hildon_calendar_set_month_next                  (HildonCalendar *calendar)
{
    HildonCalendarPrivate *priv;
    gint month_len;

    g_return_if_fail (GTK_IS_WIDGET (calendar));

    priv = HILDON_CALENDAR_GET_PRIVATE (calendar);

    if (calendar->display_flags & HILDON_CALENDAR_NO_MONTH_CHANGE)
        return;

    if (calendar->month == 11)
    {
        if (!priv->max_year || calendar->year < priv->max_year)
        {
            calendar->month = 0;
            calendar->year++;
        }
    } 
    else 
        calendar->month++;

    hildon_calendar_freeze (calendar);
    hildon_calendar_compute_days (calendar);
    g_signal_emit (calendar,
            hildon_calendar_signals[NEXT_MONTH_SIGNAL],
            0);
    g_signal_emit (calendar,
            hildon_calendar_signals[MONTH_CHANGED_SIGNAL],
            0);

    month_len = month_length[leap (calendar->year)][calendar->month + 1];

    if (month_len < calendar->selected_day)
    {
        calendar->selected_day = 0;
        hildon_calendar_select_day (calendar, month_len);
    }
    else
        hildon_calendar_select_day (calendar, calendar->selected_day);

    gtk_widget_queue_draw (GTK_WIDGET (calendar));
    hildon_calendar_thaw (calendar);
}

static void
hildon_calendar_set_year_prev                   (HildonCalendar *calendar)
{
    HildonCalendarPrivate *priv;
    gint month_len;

    g_return_if_fail (GTK_IS_WIDGET (calendar));

    priv = HILDON_CALENDAR_GET_PRIVATE (calendar);

    if (!priv->min_year || priv->min_year < calendar->year)
        calendar->year--;

    hildon_calendar_freeze (calendar);
    hildon_calendar_compute_days (calendar);
    g_signal_emit (calendar,
            hildon_calendar_signals[PREV_YEAR_SIGNAL],
            0);
    g_signal_emit (calendar,
            hildon_calendar_signals[MONTH_CHANGED_SIGNAL],
            0);

    month_len = month_length[leap (calendar->year)][calendar->month + 1];

    if (month_len < calendar->selected_day)
    {
        calendar->selected_day = 0;
        hildon_calendar_select_day (calendar, month_len);
    }
    else
        hildon_calendar_select_day (calendar, calendar->selected_day);

    gtk_widget_queue_draw (GTK_WIDGET (calendar));
    hildon_calendar_thaw (calendar);
}

static void
hildon_calendar_set_year_next                   (HildonCalendar *calendar)
{
    HildonCalendarPrivate *priv;
    gint month_len;

    g_return_if_fail (GTK_IS_WIDGET (calendar));
    priv = HILDON_CALENDAR_GET_PRIVATE (calendar);

    hildon_calendar_freeze (calendar);

    if (!priv->max_year || priv->max_year > calendar->year)
        calendar->year++;

    hildon_calendar_compute_days (calendar);
    g_signal_emit (calendar,
            hildon_calendar_signals[NEXT_YEAR_SIGNAL],
            0);
    g_signal_emit (calendar,
            hildon_calendar_signals[MONTH_CHANGED_SIGNAL],
            0);

    month_len = month_length[leap (calendar->year)][calendar->month + 1];

    if (month_len < calendar->selected_day)
    {
        calendar->selected_day = 0;
        hildon_calendar_select_day (calendar, month_len);
    }
    else
        hildon_calendar_select_day (calendar, calendar->selected_day);
    gtk_widget_queue_draw (GTK_WIDGET (calendar));
    hildon_calendar_thaw (calendar);
}

static void
hildon_calendar_main_button                     (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint x, y;
    gint row, col;
    gint day_month;
    gint day;
    gboolean hildonlike;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    x = (gint) (event->x);
    y = (gint) (event->y);

    row = row_from_y (calendar, y);
    col = column_from_x (calendar, x);

    /* If row or column isn't found, just return. */
    if (row == -1 || col == -1)
        return;

    gtk_widget_style_get (GTK_WIDGET (calendar), "hildonlike", &hildonlike, NULL);
    day_month = calendar->day_month[row][col];

    if (hildonlike) 
    {
        if ((calendar->year == private_data->min_year &&
                    calendar->month == 0 && day_month == MONTH_PREV) ||
                (calendar->year == private_data->max_year &&
                 calendar->month == 11 && day_month == MONTH_NEXT)) 
        {
            private_data->is_bad_day = TRUE;
            g_signal_emit (calendar, hildon_calendar_signals[ERRONEOUS_DATE_SIGNAL], 0);
            return;
        }
    }

    if (event->type == (hildonlike ? GDK_BUTTON_RELEASE : GDK_BUTTON_PRESS))
    {
        day = calendar->day[row][col];

        if (day_month == MONTH_PREV)
        {  
            hildon_calendar_set_month_prev (calendar);
        }
        else if (day_month == MONTH_NEXT)
        {
            hildon_calendar_set_month_next (calendar);
        }

        if (!GTK_WIDGET_HAS_FOCUS (widget))
            gtk_widget_grab_focus (widget);

        if (event->button == 1) 
        {
            private_data->in_drag = 1;
            private_data->drag_start_x = x;
            private_data->drag_start_y = y;
        }

        hildon_calendar_select_and_focus_day (calendar, day);
    }
    else if (event->type == GDK_2BUTTON_PRESS)
    {
        private_data->in_drag = 0;
        private_data->slide_stylus = FALSE;
        if (day_month == MONTH_CURRENT)
            g_signal_emit (calendar,
                    hildon_calendar_signals[DAY_SELECTED_DOUBLE_CLICK_SIGNAL], 0);
    }
}

static void
hildon_calendar_realize_arrows                  (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    GdkWindowAttr attributes;
    gint attributes_mask;
    gint i;
    guint arrow_vlength, arrow_hlength;
    /*gboolean year_left;*/

    g_return_if_fail (HILDON_IS_CALENDAR (widget));

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    gtk_widget_style_get (widget,
            "scroll-arrow-hlength", &arrow_hlength,
            "scroll-arrow-vlength", &arrow_vlength,
            NULL);
    /*
       if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) 
       year_left = private_data->year_before;
       else
       year_left = !private_data->year_before;
       */    
    /* Arrow windows ------------------------------------- */
    if (! (calendar->display_flags & HILDON_CALENDAR_NO_MONTH_CHANGE)
            && (calendar->display_flags & HILDON_CALENDAR_SHOW_HEADING))
    {
        attributes.wclass = GDK_INPUT_OUTPUT;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.visual = gtk_widget_get_visual (widget);
        attributes.colormap = gtk_widget_get_colormap (widget);
        attributes.event_mask = (gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK
                | GDK_BUTTON_PRESS_MASK  | GDK_BUTTON_RELEASE_MASK
                | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
        attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
        attributes.y = 0;
        attributes.width = arrow_vlength;
        attributes.height = arrow_hlength;

        attributes.x = (widget->allocation.width - private_data->max_year_width) / 2 - arrow_vlength - HILDON_ARROW_SEP;    
        private_data->arrow_win[ARROW_YEAR_LEFT] = gdk_window_new (private_data->header_win,
                &attributes, attributes_mask);

        attributes.x = (widget->allocation.width + private_data->max_year_width) / 2 + HILDON_ARROW_SEP;
        private_data->arrow_win[ARROW_YEAR_RIGHT] = gdk_window_new (private_data->header_win,
                &attributes, attributes_mask);
        attributes.x = (widget->allocation.width - private_data->max_month_width) / 2 - arrow_vlength - HILDON_ARROW_SEP;
        private_data->arrow_win[ARROW_MONTH_LEFT] = gdk_window_new (private_data->footer_win,
                &attributes, attributes_mask);
        attributes.x = (widget->allocation.width + private_data->max_month_width) / 2 + HILDON_ARROW_SEP;
        private_data->arrow_win[ARROW_MONTH_RIGHT] = gdk_window_new (private_data->footer_win,
                &attributes, attributes_mask);

        /*
           for (i = 0; i < 4; i++)
           {
           switch (i)
           {
           case ARROW_MONTH_LEFT:
           if (year_left) 
           attributes.x = (widget->allocation.width - 2 * widget->style->xthickness
           - (3 + 2*private_data->arrow_width 
           + private_data->max_month_width));
           else
           attributes.x = 3;
           break;
           case ARROW_MONTH_RIGHT:
           if (year_left) 
           attributes.x = (widget->allocation.width - 2 * widget->style->xthickness 
           - 3 - private_data->arrow_width);
           else
           attributes.x = (private_data->arrow_width 
           + private_data->max_month_width);
           break;
           case ARROW_YEAR_LEFT:
           if (year_left) 
           attributes.x = 3;
           else
           attributes.x = (widget->allocation.width - 2 * widget->style->xthickness
           - (3 + 2*private_data->arrow_width 
           + private_data->max_year_width));
           break;
           case ARROW_YEAR_RIGHT:
           if (year_left) 
           attributes.x = (private_data->arrow_width 
           + private_data->max_year_width);
           else
           attributes.x = (widget->allocation.width - 2 * widget->style->xthickness 
           - 3 - private_data->arrow_width);
           break;
           }
           private_data->arrow_win[i] = gdk_window_new (private_data->header_win,
           &attributes, 
           attributes_mask);*/

        for (i = 0; i < 4; i++)
        {
            if (GTK_WIDGET_IS_SENSITIVE (widget))
                private_data->arrow_state[i] = GTK_STATE_NORMAL;
            else 
                private_data->arrow_state[i] = GTK_STATE_INSENSITIVE;
            gdk_window_set_background (private_data->arrow_win[i],
                    HEADER_BG_COLOR (GTK_WIDGET (calendar)));
            gdk_window_show (private_data->arrow_win[i]);
            gdk_window_set_user_data (private_data->arrow_win[i], widget);
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
            private_data->arrow_win[i] = NULL;
    }
}

static void
hildon_calendar_realize_header                  (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    GdkWindowAttr attributes;
    gint attributes_mask;
    guint arrow_hlength;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    gtk_widget_style_get (widget,
            "scroll-arrow-hlength", &arrow_hlength,
            NULL);
    /* Header window ------------------------------------- */
    if (calendar->display_flags & HILDON_CALENDAR_SHOW_HEADING)
    {
        attributes.wclass = GDK_INPUT_OUTPUT;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.visual = gtk_widget_get_visual (widget);
        attributes.colormap = gtk_widget_get_colormap (widget);
        attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
        attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
        attributes.x = 0 /*widget->style->xthickness*/;
        attributes.y = 0 /*widget->style->ythickness*/;
        attributes.width = widget->allocation.width; /* - 2 * attributes.x */;
        attributes.height = arrow_hlength /*private_data->header_h - 2 * attributes.y*/;
        private_data->header_win = gdk_window_new (widget->window,
                &attributes, attributes_mask);

        attributes.y = arrow_hlength + 2 * CALENDAR_YSEP + private_data->main_h + private_data->day_name_h;

        private_data->footer_win = gdk_window_new(widget->window, 
                &attributes, attributes_mask);

        gdk_window_set_background (private_data->header_win,
                HEADER_BG_COLOR (widget));
        gdk_window_set_background (private_data->footer_win,
                HEADER_BG_COLOR (widget));

        gdk_window_show (private_data->header_win);
        gdk_window_show (private_data->footer_win);
        gdk_window_set_user_data (private_data->header_win, widget);
        gdk_window_set_user_data (private_data->footer_win, widget);
    }
    else
    {
        private_data->header_win = NULL;
        private_data->footer_win = NULL;
    }
    hildon_calendar_realize_arrows (widget);
}

static void
hildon_calendar_realize_day_names               (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    /* Day names  window --------------------------------- */
    if ( calendar->display_flags & HILDON_CALENDAR_SHOW_DAY_NAMES)
    {
        attributes.wclass = GDK_INPUT_OUTPUT;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.visual = gtk_widget_get_visual (widget);
        attributes.colormap = gtk_widget_get_colormap (widget);
        attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;
        attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
        attributes.x = HILDON_DAY_WIDTH + HILDON_WEEKS_EXTRA_WIDTH/*(widget->style->xthickness + INNER_BORDER)*/;
        attributes.y = private_data->header_h;
        attributes.width = widget->allocation.width - attributes.x;
        attributes.height = private_data->day_name_h;
        private_data->day_name_win = gdk_window_new (widget->window,
                &attributes, 
                attributes_mask);
        gdk_window_set_background (private_data->day_name_win, 
                BACKGROUND_COLOR ( GTK_WIDGET (calendar)));

        gdk_window_show (private_data->day_name_win);
        gdk_window_set_user_data (private_data->day_name_win, widget);
    }
    else
    {
        private_data->day_name_win = NULL;
    }
}

static void
hildon_calendar_realize_week_numbers            (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    GdkWindowAttr attributes;
    gint attributes_mask;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    /* Week number window -------------------------------- */
    if (calendar->display_flags & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
    {
        attributes.wclass = GDK_INPUT_OUTPUT;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.visual = gtk_widget_get_visual (widget);
        attributes.colormap = gtk_widget_get_colormap (widget);
        attributes.event_mask = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK;

        attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
        attributes.x = 0 /*widget->style->xthickness + INNER_BORDER*/;
        attributes.y = private_data->header_h; 
        /*+ (widget->style->ythickness + INNER_BORDER))*/;
        attributes.width = HILDON_DAY_WIDTH + HILDON_WEEKS_EXTRA_WIDTH;
        attributes.height = private_data->main_h + private_data->day_name_h;
        private_data->week_win = gdk_window_new (widget->window,
                &attributes, attributes_mask);
        gdk_window_set_background (private_data->week_win,  
                BACKGROUND_COLOR (GTK_WIDGET (calendar)));
        gdk_window_show (private_data->week_win);
        gdk_window_set_user_data (private_data->week_win, widget);
    } 
    else
    {
        private_data->week_win = NULL;
    }
}

static void
hildon_calendar_realize                         (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    GdkWindowAttr attributes;
    gint attributes_mask;
    GdkGCValues values;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
    hildon_calendar_compute_days (calendar);

    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_OUTPUT;
    attributes.window_type = GDK_WINDOW_CHILD;
    attributes.event_mask =  (gtk_widget_get_events (widget) 
            | GDK_EXPOSURE_MASK |GDK_KEY_PRESS_MASK | GDK_SCROLL_MASK);
    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);

    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
    widget->window = gdk_window_new (widget->parent->window,
            &attributes, attributes_mask);

    widget->style = gtk_style_attach (widget->style, widget->window);

    /* Header window ------------------------------------- */
    hildon_calendar_realize_header (widget);
    /* Day names  window --------------------------------- */
    hildon_calendar_realize_day_names (widget);
    /* Week number window -------------------------------- */
    hildon_calendar_realize_week_numbers (widget);
    /* Main Window --------------------------------------  */
    attributes.event_mask =  (gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK
            | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
            | GDK_POINTER_MOTION_MASK | GDK_LEAVE_NOTIFY_MASK);

    attributes.x = HILDON_DAY_WIDTH + HILDON_WEEKS_EXTRA_WIDTH /*private_data->week_width + (widget->style->ythickness + INNER_BORDER)*/;
    attributes.y = (private_data->header_h + private_data->day_name_h 
            + (widget->style->ythickness + INNER_BORDER));
    attributes.width = (widget->allocation.width - attributes.x 
            /*- (widget->style->xthickness + INNER_BORDER)*/);
    attributes.height = private_data->main_h;
    private_data->main_win = gdk_window_new (widget->window,
            &attributes, attributes_mask);
    gdk_window_set_background (private_data->main_win, 
            BACKGROUND_COLOR ( GTK_WIDGET ( calendar)));
    gdk_window_show (private_data->main_win);
    gdk_window_set_user_data (private_data->main_win, widget);
    gdk_window_set_background (widget->window, BACKGROUND_COLOR (widget));
    gdk_window_show (widget->window);
    gdk_window_set_user_data (widget->window, widget);

    /* Set widgets gc */
    calendar->gc = gdk_gc_new (widget->window);

    values.foreground = widget->style->white;
    values.function = GDK_XOR;
    calendar->xor_gc = gdk_gc_new_with_values (widget->window,
            &values,
            GDK_GC_FOREGROUND |
            GDK_GC_FUNCTION);
}

static void
hildon_calendar_unrealize                       (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint i;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (private_data->header_win)
    {
        for (i = 0; i < 4; i++)
        {
            if (private_data->arrow_win[i])
            {
                gdk_window_set_user_data (private_data->arrow_win[i], NULL);
                gdk_window_destroy (private_data->arrow_win[i]);
                private_data->arrow_win[i] = NULL;
            }
        }
        gdk_window_set_user_data (private_data->header_win, NULL);
        gdk_window_destroy (private_data->header_win);
        private_data->header_win = NULL;
        gdk_window_set_user_data (private_data->footer_win, NULL);
        gdk_window_destroy (private_data->footer_win);
        private_data->footer_win = NULL;  
    }

    if (private_data->week_win)
    {
        gdk_window_set_user_data (private_data->week_win, NULL);
        gdk_window_destroy (private_data->week_win);
        private_data->week_win = NULL;      
    }

    if (private_data->main_win)
    {
        gdk_window_set_user_data (private_data->main_win, NULL);
        gdk_window_destroy (private_data->main_win);
        private_data->main_win = NULL;      
    }
    if (private_data->day_name_win)
    {
        gdk_window_set_user_data (private_data->day_name_win, NULL);
        gdk_window_destroy (private_data->day_name_win);
        private_data->day_name_win = NULL;      
    }
    if (calendar->xor_gc)
        g_object_unref (calendar->xor_gc);
    if (calendar->gc)
        g_object_unref (calendar->gc);

    if (GTK_WIDGET_CLASS (parent_class)->unrealize)
        (* GTK_WIDGET_CLASS (parent_class)->unrealize) (widget);
}

static void
hildon_calendar_size_request                    (GtkWidget *widget,
                                                 GtkRequisition *requisition)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    PangoLayout *layout;
    PangoRectangle logical_rect;

    /*gint height;*/
    gint i;
    gchar buffer[255];
    /*gint calendar_margin = CALENDAR_MARGIN;*/
    gint header_width, main_width;
    gint max_header_height = 0;
    gint focus_width;
    gint focus_padding;
    gint arrow_hlength;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    gtk_widget_style_get (GTK_WIDGET (widget),
            "focus-line-width", &focus_width,
            "focus-padding", &focus_padding,
            "scroll-arrow-hlength", &arrow_hlength,
            NULL);

    layout = gtk_widget_create_pango_layout (widget, NULL);

    /*
     * Calculate the requisition width for the widget.
     */

    /* Header width */

    if (calendar->display_flags & HILDON_CALENDAR_SHOW_HEADING)
    {
        private_data->max_month_width = 0;
        for (i = 0; i < 12; i++)
        {
            pango_layout_set_text (layout, private_data->monthname[i], -1);
            pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
            private_data->max_month_width = MAX (private_data->max_month_width,
                    logical_rect.width + 8);
            max_header_height = MAX (max_header_height, logical_rect.height); 
        }
        private_data->max_year_width = 0;
        for (i=0; i<10; i++)
        {
            g_snprintf (buffer, sizeof (buffer), "%d%d%d%d", i,i,i,i);
            pango_layout_set_text (layout, buffer, -1);     
            pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
            private_data->max_year_width = MAX (private_data->max_year_width,
                    logical_rect.width + 8);
            max_header_height = MAX (max_header_height, logical_rect.height); 
        }
    }
    else 
    {
        private_data->max_month_width = 0;
        private_data->max_year_width = 0;
    }

    if (calendar->display_flags & HILDON_CALENDAR_NO_MONTH_CHANGE)
        header_width = (private_data->max_month_width 
                + private_data->max_year_width
                + 3 * 3);
    else
        header_width = (private_data->max_month_width 
                + private_data->max_year_width
                + 4 * private_data->arrow_width + 3 * 3);

    /* Mainwindow labels width */

    private_data->max_day_char_width = 0;
    private_data->min_day_width = 0;
    private_data->max_label_char_ascent = 0;
    private_data->max_label_char_descent = 0;

    for (i = 0; i < 9; i++)
    {
        g_snprintf (buffer, sizeof (buffer), "%d%d", i, i);
        pango_layout_set_text (layout, buffer, -1);         
        pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
        private_data->min_day_width = MAX (private_data->min_day_width,
                logical_rect.width);

        private_data->max_day_char_ascent = MAX (private_data->max_label_char_ascent,
                PANGO_ASCENT (logical_rect));
        private_data->max_day_char_descent = MAX (private_data->max_label_char_descent, 
                PANGO_DESCENT (logical_rect));
    }
    /* We add one to max_day_char_width to be able to make the marked day "bold" */
    private_data->max_day_char_width = private_data->min_day_width / 2 + 1;

    if (calendar->display_flags & HILDON_CALENDAR_SHOW_DAY_NAMES)
        for (i = 0; i < 7; i++)
        {
            pango_layout_set_text (layout, private_data->abbreviated_dayname[i], -1);
            pango_layout_line_get_pixel_extents (pango_layout_get_lines (layout)->data, NULL, &logical_rect);

            /* Hildon: add 4 so that passive focus wouldn't overlap day names */
            private_data->min_day_width = MAX (private_data->min_day_width, logical_rect.width + 4);
            private_data->max_label_char_ascent = MAX (private_data->max_label_char_ascent,
                    PANGO_ASCENT (logical_rect));
            private_data->max_label_char_descent = MAX (private_data->max_label_char_descent, 
                    PANGO_DESCENT (logical_rect));
        }

    private_data->max_week_char_width = 0;
    if (calendar->display_flags & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
        for (i = 0; i < 9; i++)
        {
            g_snprintf (buffer, sizeof (buffer), "%d%d", i, i);
            pango_layout_set_text (layout, buffer, -1);       
            pango_layout_get_pixel_extents (layout, NULL, &logical_rect);
            private_data->max_week_char_width = MAX (private_data->max_week_char_width,
                    logical_rect.width / 2);
        }

    main_width = (7 * (private_data->min_day_width + (focus_padding + focus_width) * 2) + (DAY_XSEP * 6) + CALENDAR_MARGIN * 2
            + (private_data->max_week_char_width
                ? private_data->max_week_char_width * 2 + (focus_padding + focus_width) * 2 + CALENDAR_XSEP * 2
                : 0));

    /* requisition->width = MAX (header_width, main_width + INNER_BORDER * 2) + widget->style->xthickness * 2;
     *
     * FIXME: header_width is broken, when Calendar is themed ! 
     *  Next line is workaround for this bug
     */
    requisition->width = (main_width + INNER_BORDER * 2) + widget->style->xthickness * 2 + HILDON_WEEKS_EXTRA_WIDTH + HILDON_DAYS_EXTRA_WIDTH;

    /*
     * Calculate the requisition height for the widget.
     * This is Hildon calculation
     */

    if (calendar->display_flags & HILDON_CALENDAR_SHOW_HEADING)
        private_data->header_h = arrow_hlength + CALENDAR_YSEP;
    else
        private_data->header_h = 0;

    if (calendar->display_flags & HILDON_CALENDAR_SHOW_DAY_NAMES)
        private_data->day_name_h = HILDON_DAY_HEIGHT;
    else
        private_data->day_name_h = 0;

    private_data->main_h = 6 * HILDON_DAY_HEIGHT;
    requisition->height = 2 * private_data->header_h + private_data->day_name_h + private_data->main_h;

    g_object_unref (layout);
}

static void
hildon_calendar_size_allocate                   (GtkWidget *widget,
                                                 GtkAllocation *allocation)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint xthickness = widget->style->xthickness;
    /*gint ythickness = widget->style->xthickness;*/
    gboolean year_left;
    gint arrow_vlength, arrow_hlength;

    widget->allocation = *allocation;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) 
        year_left = private_data->year_before;
    else
        year_left = !private_data->year_before;

    gtk_widget_style_get (widget,
            "scroll-arrow-vlength", &arrow_vlength,
            "scroll-arrow-hlength", &arrow_hlength,
            NULL);

    if (calendar->display_flags & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
    {
        /* this variable is introduced to avoid breaking week_width because
           of HILDON_WEEKS_EXTRA_WIDTH and HILDON_DAYS_EXTRA_WIDTH appearing
           in calculation of day_width */
        int real_day_width = (private_data->min_day_width
                * ((allocation->width
                        - (xthickness + INNER_BORDER) * 2
                        - (CALENDAR_MARGIN * 2) -  (DAY_XSEP * 6) - CALENDAR_XSEP * 2))
                / (7 * private_data->min_day_width + private_data->max_week_char_width * 2));

        private_data->day_width = (private_data->min_day_width
                * ((allocation->width
                        - (HILDON_WEEKS_EXTRA_WIDTH + HILDON_DAYS_EXTRA_WIDTH)
                        - (xthickness + INNER_BORDER) * 2
                        - (CALENDAR_MARGIN * 2) -  (DAY_XSEP * 6) - CALENDAR_XSEP * 2))
                / (7 * private_data->min_day_width + private_data->max_week_char_width * 2));
        private_data->week_width = ((allocation->width - (xthickness + INNER_BORDER) * 2
                    - (CALENDAR_MARGIN * 2) - (DAY_XSEP * 6) - CALENDAR_XSEP * 2 )
                - real_day_width * 7 + CALENDAR_MARGIN + CALENDAR_XSEP);
    }
    else 
    {
        private_data->day_width = (allocation->width
                - (xthickness + INNER_BORDER) * 2
                - (CALENDAR_MARGIN * 2)
                - (DAY_XSEP * 6))/7;
        private_data->week_width = 0;
    }

    if (GTK_WIDGET_REALIZED (widget))
    {
        gdk_window_move_resize (widget->window,
                allocation->x, allocation->y,
                allocation->width, allocation->height);
        if (private_data->header_win)
            gdk_window_move_resize (private_data->header_win,
                    0, 0, widget->allocation.width, arrow_hlength);
        if (private_data->arrow_win[ARROW_YEAR_LEFT])
        {
            /* if (year_left)
               gdk_window_move_resize (private_data->arrow_win[ARROW_YEAR_LEFT],
               3, 3,
               private_data->arrow_width,
               private_data->header_h - 7);
               else
               gdk_window_move_resize (private_data->arrow_win[ARROW_YEAR_LEFT],
               (allocation->width - 2 * xthickness
               - (3 + 2*private_data->arrow_width 
               + private_data->max_year_width)),
               3,
               private_data->arrow_width,
               private_data->header_h - 7);*/

            gdk_window_move (private_data->arrow_win[ARROW_YEAR_LEFT],
                    (widget->allocation.width - private_data->max_year_width) / 2 - arrow_vlength - HILDON_ARROW_SEP, 0);
        }
        if (private_data->arrow_win[ARROW_YEAR_RIGHT])
        {
            /*        if (year_left)
                      gdk_window_move_resize (private_data->arrow_win[ARROW_YEAR_RIGHT],
                      (private_data->arrow_width 
                      + private_data->max_year_width), 
                      3,
                      private_data->arrow_width,
                      private_data->header_h - 7);
                      else
                      gdk_window_move_resize (private_data->arrow_win[ARROW_YEAR_RIGHT],
                      (allocation->width - 2 * xthickness 
                      - 3 - private_data->arrow_width), 
                      3,
                      private_data->arrow_width,
                      private_data->header_h - 7);*/
            gdk_window_move (private_data->arrow_win[ARROW_YEAR_RIGHT],
                    (widget->allocation.width + private_data->max_year_width) / 2 + HILDON_ARROW_SEP,  0);
        }
        if (private_data->footer_win)
            gdk_window_move_resize (private_data->footer_win,
                    0, private_data->header_h + private_data->day_name_h +  private_data->main_h + CALENDAR_YSEP,
                    widget->allocation.width, arrow_hlength);

        if (private_data->arrow_win[ARROW_MONTH_LEFT])
        {
            /*        if (year_left)
                      gdk_window_move_resize (private_data->arrow_win[ARROW_MONTH_LEFT],
                      (allocation->width - 2 * xthickness
                      - (3 + 2*private_data->arrow_width 
                      + private_data->max_month_width)),
                      3,
                      private_data->arrow_width,
                      private_data->header_h - 7);
                      else
                      gdk_window_move_resize (private_data->arrow_win[ARROW_MONTH_LEFT],
                      3, 3,
                      private_data->arrow_width,
                      private_data->header_h - 7);
                      */

            gdk_window_move (private_data->arrow_win[ARROW_MONTH_LEFT],
                    (widget->allocation.width - private_data->max_month_width) / 2 - arrow_vlength - HILDON_ARROW_SEP, 0);
        }
        if (private_data->arrow_win[ARROW_MONTH_RIGHT])
        {
            /*        if (year_left)
                      gdk_window_move_resize (private_data->arrow_win[ARROW_MONTH_RIGHT],
                      (allocation->width - 2 * xthickness 
                      - 3 - private_data->arrow_width), 
                      3,
                      private_data->arrow_width,
                      private_data->header_h - 7);
                      else
                      gdk_window_move_resize (private_data->arrow_win[ARROW_MONTH_RIGHT],
                      (private_data->arrow_width 
                      + private_data->max_month_width), 
                      3,
                      private_data->arrow_width,
                      private_data->header_h - 7);*/
            gdk_window_move (private_data->arrow_win[ARROW_MONTH_RIGHT],
                    (widget->allocation.width + private_data->max_month_width) / 2 + HILDON_ARROW_SEP, 0); 
        }


        if (private_data->day_name_win)
            gdk_window_move_resize (private_data->day_name_win,
                    private_data->week_width, /*xthickness + INNER_BORDER*/
                    private_data->header_h /*+ (widget->style->ythickness + INNER_BORDER)*/,
                    widget->allocation.width - private_data->week_width /*- (xthickness + INNER_BORDER) * 2*/,
                    private_data->day_name_h);
        if (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_LTR) 
        {
            if (private_data->week_win)
                gdk_window_move_resize (private_data->week_win,
                        0 /*(xthickness + INNER_BORDER)*/,
                        private_data->header_h   /*+ (widget->style->ythickness + INNER_BORDER)*/,
                        HILDON_DAY_WIDTH,
                        private_data->main_h + private_data->day_name_h);
            gdk_window_move_resize (private_data->main_win,
                    private_data->week_width /* + (xthickness + INNER_BORDER)*/,
                    private_data->header_h + private_data->day_name_h
                    /*+ (widget->style->ythickness + INNER_BORDER)*/,
                    widget->allocation.width - private_data->week_width 
                    /*- (xthickness + INNER_BORDER) * 2*/,
                    private_data->main_h);
        }
        else 
        {
            gdk_window_move_resize (private_data->main_win,
                    0 /*(xthickness + INNER_BORDER)*/,
                    private_data->header_h + private_data->day_name_h
                    /*+ (widget->style->ythickness + INNER_BORDER)*/,
                    widget->allocation.width 
                    - private_data->week_width 
                    /*- (xthickness + INNER_BORDER) * 2*/,
                    private_data->main_h);
            if (private_data->week_win)
                gdk_window_move_resize (private_data->week_win,
                        widget->allocation.width 
                        - private_data->week_width 
                        /*- (xthickness + INNER_BORDER)*/,
                        private_data->header_h + private_data->day_name_h
                        /*+ (widget->style->ythickness + INNER_BORDER)*/,
                        private_data->week_width,
                        private_data->main_h);
        }
    }
}

static gboolean
hildon_calendar_expose                          (GtkWidget *widget,
                                                 GdkEventExpose *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (GTK_WIDGET_DRAWABLE (widget))
    {
        if (event->window == private_data->main_win)
            hildon_calendar_paint_main (widget);

        if (event->window == private_data->header_win)
            hildon_calendar_paint_header (widget);
        if (event->window == private_data->footer_win)
            hildon_calendar_paint_footer(widget);

        if (event->window == private_data->day_name_win) 
            hildon_calendar_paint_day_names (widget);

        if (event->window == private_data->week_win) 
            hildon_calendar_paint_week_numbers (widget);
    }

    return FALSE;
}

static void
hildon_calendar_paint_header                    (GtkWidget *widget)
{
    HildonCalendar *calendar;
    GdkGC *gc;
    char buffer[255];
    int x, y;
    gint header_width, cal_height;
    HildonCalendarPrivate *private_data;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    gint arrow_hlength;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (private_data->freeze_count)
    {
        private_data->dirty_header = 1;
        return;
    }

    private_data->dirty_header = 0;
    gc = calendar->gc;

    /* Clear window */
    gdk_window_clear (private_data->header_win);

    header_width = widget->allocation.width /*- 2 * widget->style->xthickness*/;
    cal_height = widget->allocation.height;

    g_snprintf (buffer, sizeof (buffer), "%d", calendar->year);
    layout = gtk_widget_create_pango_layout (widget, buffer);
    pango_layout_get_pixel_extents (layout, NULL, &logical_rect);

    gtk_widget_style_get (widget, "scroll-arrow-hlength", &arrow_hlength, NULL);

    y = (arrow_hlength - logical_rect.height) / 2;
    x = (widget->allocation.width - logical_rect.width) / 2;

    /* Draw year and its arrows */
    gdk_gc_set_foreground (gc, HEADER_FG_COLOR (GTK_WIDGET (calendar)));
    gdk_draw_layout (private_data->header_win, gc, x, y, layout);  

    hildon_calendar_paint_arrow (widget, ARROW_YEAR_LEFT);
    hildon_calendar_paint_arrow (widget, ARROW_YEAR_RIGHT);

    g_object_unref (layout);
}

static void
hildon_calendar_paint_footer                    (GtkWidget *widget)
{
    HildonCalendar *calendar;
    GdkGC *gc;
    char buffer[255];
    int x, y;
    gint header_width, cal_height;
    HildonCalendarPrivate *private_data;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    gint arrow_hlength;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (private_data->freeze_count)
    {
        private_data->dirty_header = 1;
        return;
    }

    private_data->dirty_header = 0;
    gc = calendar->gc;

    /* Clear window */
    gdk_window_clear (private_data->footer_win);

    header_width = widget->allocation.width - 2 * widget->style->xthickness;
    cal_height = widget->allocation.height;

    /* Draw month and its arrows */
    g_snprintf (buffer, sizeof (buffer), "%s", private_data->monthname[calendar->month]);
    layout = gtk_widget_create_pango_layout (widget, buffer);
    pango_layout_get_pixel_extents (layout, NULL, &logical_rect);

    gtk_widget_style_get (widget, "scroll-arrow-hlength", &arrow_hlength, NULL);

    x = (widget->allocation.width - logical_rect.width) / 2;
    y = (arrow_hlength - logical_rect.height) / 2;

    gdk_gc_set_foreground (gc, HEADER_FG_COLOR(GTK_WIDGET (calendar)));
    gdk_draw_layout (private_data->footer_win, gc, x, y, layout);

    hildon_calendar_paint_arrow (widget, ARROW_MONTH_LEFT);
    hildon_calendar_paint_arrow (widget, ARROW_MONTH_RIGHT);

    g_object_unref(layout);
}

static void
hildon_calendar_paint_day_names                 (GtkWidget *widget)
{
    HildonCalendar *calendar;
    GdkGC *gc;
    char buffer[255];
    int day,i;
    int day_width, cal_width;
    gint cal_height;
    int day_wid_sep;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    HildonCalendarPrivate *private_data;
    gint focus_padding;
    gint focus_width;
    gboolean hildonlike;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));
    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    gc = calendar->gc;

    gtk_widget_style_get (GTK_WIDGET (widget),
            "focus-line-width", &focus_width,
            "focus-padding", &focus_padding,
            "hildonlike", &hildonlike,
            NULL);
    /*
     * Handle freeze/thaw functionality
     */

    if (private_data->freeze_count)
    {
        private_data->dirty_day_names = 1;
        return;
    }
    private_data->dirty_day_names = 0;

    /*
     * Clear the window
     */

    gdk_window_clear (private_data->day_name_win);

    day_width = private_data->day_width;
    cal_width = widget->allocation.width;
    cal_height = widget->allocation.height;
    day_wid_sep = day_width + DAY_XSEP;

    /*
     * Draw rectangles as inverted background for the labels.
     */

    /* Hildon: don't paint dayname window */
    if (!hildonlike)
    {
        gdk_gc_set_foreground (gc, SELECTED_BG_COLOR (widget));
        gdk_draw_rectangle (private_data->day_name_win, gc, TRUE,
                CALENDAR_MARGIN, CALENDAR_MARGIN,
                cal_width-CALENDAR_MARGIN * 2,
                private_data->day_name_h - CALENDAR_MARGIN);

        if (calendar->display_flags & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
            gdk_draw_rectangle (private_data->day_name_win, gc, TRUE,
                    CALENDAR_MARGIN,
                    private_data->day_name_h - CALENDAR_YSEP,
                    private_data->week_width - CALENDAR_YSEP - CALENDAR_MARGIN,
                    CALENDAR_YSEP);
    }

    /*
     * Write the labels
     */

    layout = gtk_widget_create_pango_layout (widget, NULL);

    gdk_gc_set_foreground (gc, SELECTED_FG_COLOR (widget));
    for (i = 0; i < 7; i++)
    { 
        guint x = left_x_for_column (calendar, i);

        if (gtk_widget_get_direction (GTK_WIDGET (calendar)) == GTK_TEXT_DIR_RTL)
            day = 6 - i;
        else
            day = i;
        day = (day + private_data->week_start) % 7;
        g_snprintf (buffer, sizeof (buffer), "%s", private_data->abbreviated_dayname[day]);

        pango_layout_set_text (layout, buffer, -1);
        pango_layout_get_pixel_extents (layout, NULL, &logical_rect);

        /* Hildon: draw passive focus for day name */
        if (hildonlike && calendar->focus_col == i)
            gtk_paint_box(GTK_WIDGET (calendar)->style,
                    private_data->day_name_win,
                    GTK_STATE_NORMAL,
                    GTK_SHADOW_OUT, NULL,
                    GTK_WIDGET (calendar), "passive-focus",
                    x,
                    0,
                    logical_rect.width + 4,
                    HILDON_DAY_HEIGHT);

        gdk_draw_layout (private_data->day_name_win, gc,
                x + 2,
                CALENDAR_MARGIN + focus_width + focus_padding + logical_rect.y,
                layout);
    }
    g_object_unref (layout);
}

static void
hildon_calendar_paint_week_numbers              (GtkWidget *widget)
{
    HildonCalendar *calendar;
    GdkGC *gc; 
    gint row, week = 0, year;
    gint x_loc;
    char buffer[10];
    gint y_loc;
    HildonCalendarPrivate *private_data;
    PangoLayout *layout;
    PangoRectangle logical_rect;
    gint focus_padding;
    gint focus_width;
    gboolean hildonlike;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));
    g_return_if_fail (widget->window != NULL);
    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    gc = calendar->gc;

    /*
     * Handle freeze/thaw functionality
     */

    if (private_data->freeze_count)
    {
        private_data->dirty_week = 1;
        return;
    }
    private_data->dirty_week = 0;

    gtk_widget_style_get (GTK_WIDGET (widget),
            "focus-line-width", &focus_width,
            "focus-padding", &focus_padding,
            "hildonlike", &hildonlike,
            NULL);

    /*
     * Clear the window
     */

    gdk_window_clear (private_data->week_win);

    /*
     * Draw a rectangle as inverted background for the labels.
     */

    gdk_gc_set_foreground (gc, SELECTED_BG_COLOR (widget));

    /* Hildon: don't paint background for weekday window */
    if (!hildonlike)
    {
        if (private_data->day_name_win)
            gdk_draw_rectangle (private_data->week_win, gc, TRUE,
                    CALENDAR_MARGIN,
                    0,
                    private_data->week_width - CALENDAR_MARGIN,
                    private_data->main_h + private_data->day_name_h - CALENDAR_MARGIN);
        else
            gdk_draw_rectangle (private_data->week_win, gc, TRUE,
                    CALENDAR_MARGIN,
                    CALENDAR_MARGIN,
                    private_data->week_width - CALENDAR_MARGIN,
                    private_data->main_h - 2 * CALENDAR_MARGIN);
    }

    /*
     * Write the labels
     */

    layout = gtk_widget_create_pango_layout (widget, NULL);

    gdk_gc_set_foreground (gc, SELECTED_FG_COLOR (widget));
    gdk_draw_line(private_data->week_win, gc, 
            HILDON_DAY_WIDTH + 7,
            0,
            HILDON_DAY_WIDTH + 7,
            private_data->main_h + private_data->day_name_h);

    for (row = 0; row < 6; row++)
    {
        year = calendar->year;
        if (calendar->day[row][6] < 15 && row > 3 && calendar->month == 11)
            year++;

        g_return_if_fail (week_of_year (&week, &year,             
                    ((calendar->day[row][6] < 15 && row > 3 ? 1 : 0)
                     + calendar->month) % 12 + 1, calendar->day[row][6]));

        g_snprintf (buffer, sizeof (buffer), "%d", week);
        pango_layout_set_text (layout, buffer, -1); 
        pango_layout_get_pixel_extents (layout, NULL, &logical_rect);

        /* Hildon: draw passive focus for week */
        if (hildonlike && calendar->focus_row == row) 
        {
            guint y = top_y_for_row (calendar, calendar->focus_row + 1);

            gtk_paint_box(GTK_WIDGET (calendar)->style,
                    private_data->week_win,
                    GTK_STATE_NORMAL,
                    GTK_SHADOW_OUT, NULL,
                    GTK_WIDGET (calendar), "passive-focus",
                    0, y,
                    private_data->week_width/* - 4*/,
                    HILDON_DAY_HEIGHT);
        }

        y_loc = private_data->day_name_h + top_y_for_row (calendar, row) + (HILDON_DAY_HEIGHT - logical_rect.height) / 2;
        x_loc = (HILDON_DAY_WIDTH - logical_rect.width) / 2;

        gdk_draw_layout (private_data->week_win, gc, x_loc, y_loc, layout);
    }

    g_object_unref (layout);
}

static void
hildon_calendar_paint_day_num                   (GtkWidget *widget,
                                                 gint day)
{
    HildonCalendar *calendar;
    gint r, c, row, col;
    HildonCalendarPrivate *private_data;  
    g_return_if_fail (HILDON_IS_CALENDAR (widget));

    calendar = HILDON_CALENDAR (widget);

    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    row = -1;
    col = -1;
    for (r = 0; r < 6; r++)
        for (c = 0; c < 7; c++)
            if (calendar->day_month[r][c] == MONTH_CURRENT &&
                    calendar->day[r][c] == day)
            {
                row = r;
                col = c;
            }

    g_return_if_fail (row != -1);
    g_return_if_fail (col != -1);

    hildon_calendar_paint_day (widget, row, col);
}

static void
hildon_calendar_paint_day                       (GtkWidget *widget,
                                                 gint row,
                                                 gint col)
{
    HildonCalendar *calendar;
    GdkGC *gc;
    gchar buffer[255];
    gint day;
    gint x_left;
    gint x_loc;
    gint y_top;
    gint y_loc;
    gint focus_width;
    gboolean hildonlike;

    HildonCalendarPrivate *private_data;
    PangoLayout *layout;
    PangoRectangle logical_rect;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));
    g_return_if_fail (row < 6);
    g_return_if_fail (col < 7);
    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (private_data->main_win == NULL) return;

    /*
     * Handle freeze/thaw functionality
     */

    if (private_data->freeze_count)
    {
        private_data->dirty_main = 1;
        return;
    }

    gtk_widget_style_get (widget, "focus-line-width", &focus_width,
            "hildonlike", &hildonlike, NULL);

    day = calendar->day[row][col];
    x_left = left_x_for_column (calendar, col);
    y_top = top_y_for_row (calendar, row);

    gdk_window_clear_area (private_data->main_win, x_left, y_top,
            HILDON_DAY_WIDTH, HILDON_DAY_HEIGHT);

    gc = calendar->gc;

    if (calendar->day_month[row][col] == MONTH_PREV)
    {
        gdk_gc_set_foreground (gc, PREV_MONTH_COLOR (GTK_WIDGET (calendar)));
    } 
    else if (calendar->day_month[row][col] == MONTH_NEXT)
    {
        gdk_gc_set_foreground (gc, NEXT_MONTH_COLOR (GTK_WIDGET (calendar)));
    } 
    else 
    {
        if (calendar->selected_day == day)
        {
            /* Hildon: use custom graphics */
            if (hildonlike)
            {
                gtk_paint_box(GTK_WIDGET (calendar)->style,
                        private_data->main_win,
                        GTK_STATE_NORMAL,
                        GTK_SHADOW_NONE, NULL,
                        GTK_WIDGET (calendar), "active-day",
                        x_left, y_top,
                        HILDON_DAY_WIDTH,
                        HILDON_DAY_HEIGHT);
            }
            else
            {
                gdk_gc_set_foreground (gc, SELECTED_BG_COLOR (GTK_WIDGET (calendar)));
                gdk_draw_rectangle (private_data->main_win, gc, TRUE, x_left, y_top,
                        HILDON_DAY_WIDTH, HILDON_DAY_HEIGHT);
            }
        } 
        if (calendar->marked_date[day-1])
            gdk_gc_set_foreground (gc, MARKED_COLOR    (GTK_WIDGET (calendar)));
        else
            gdk_gc_set_foreground (gc, NORMAL_DAY_COLOR (GTK_WIDGET (calendar)));
        if (calendar->selected_day == day)
            gdk_gc_set_foreground (gc, SELECTED_FG_COLOR (GTK_WIDGET (calendar)));
        else
            gdk_gc_set_foreground (gc, & (GTK_WIDGET (calendar)->style->fg[GTK_WIDGET_STATE (calendar)]));
    }

    if (GTK_WIDGET_HAS_FOCUS (calendar) &&
            calendar->focus_row == row &&
            calendar->focus_col == col)
    {
        GtkStateType state;

        if (calendar->selected_day == day)
            state = GTK_WIDGET_HAS_FOCUS (widget) ? GTK_STATE_SELECTED : GTK_STATE_ACTIVE;
        else
            state = GTK_STATE_NORMAL;

        gtk_paint_focus (widget->style, 
                private_data->main_win,
                (calendar->selected_day == day) 
                ? GTK_STATE_SELECTED : GTK_STATE_NORMAL, 
                NULL, widget, "calendar-day",
                x_left, y_top, 
                HILDON_DAY_WIDTH, 
                HILDON_DAY_HEIGHT);
    }

    /* Hildon: paint green indicator for current day */
    if (hildonlike && (day == private_data->current_day && calendar->selected_day !=
                private_data->current_day) && (calendar->day_month[row][col] == MONTH_CURRENT))
        hildon_calendar_check_current_date (calendar, x_left, y_top);

    g_snprintf (buffer, sizeof (buffer), "%d", day);
    layout = gtk_widget_create_pango_layout (widget, buffer);
    pango_layout_get_pixel_extents (layout, NULL, &logical_rect);

    x_loc = x_left + (HILDON_DAY_WIDTH - logical_rect.width) / 2;
    y_loc = y_top + (HILDON_DAY_HEIGHT - logical_rect.height) / 2;

    gdk_draw_layout (private_data->main_win, gc,
            x_loc, y_loc, layout);
    if (calendar->marked_date[day-1] &&
            calendar->day_month[row][col] == MONTH_CURRENT)
        gdk_draw_layout (private_data->main_win, gc,
                x_loc-1, y_loc, layout);

    g_object_unref (layout);
}

static void
hildon_calendar_paint_main                      (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint row, col;

    g_return_if_fail (HILDON_IS_CALENDAR (widget));
    g_return_if_fail (widget->window != NULL);

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (private_data->freeze_count)
    {
        private_data->dirty_main = 1;
        return;
    }
    private_data->dirty_main = 0;
    gdk_window_clear (private_data->main_win);

    for (col = 0; col < 7; col++)
        for (row = 0; row < 6; row++)
            hildon_calendar_paint_day (widget, row, col);
}

static void
hildon_calendar_compute_days                    (HildonCalendar *calendar)
{
    HildonCalendarPrivate *private_data;
    gint month;
    gint year;
    gint ndays_in_month;
    gint ndays_in_prev_month;
    gint first_day;
    gint row;
    gint col;
    gint day;

    g_return_if_fail (HILDON_IS_CALENDAR (calendar));

    private_data = HILDON_CALENDAR_GET_PRIVATE (GTK_WIDGET (calendar));

    year = calendar->year;
    month = calendar->month + 1;

    ndays_in_month = month_length[leap (year)][month];

    first_day = day_of_week (year, month, 1);
    first_day = (first_day + 7 - private_data->week_start) % 7;

    /* Compute days of previous month */
    if (month > 1)
        ndays_in_prev_month = month_length[leap (year)][month-1];
    else
        ndays_in_prev_month = month_length[leap (year)][12];
    day = ndays_in_prev_month - first_day + 1;

    row = 0;
    if (first_day > 0)
    {
        for (col = 0; col < first_day; col++)
        {
            calendar->day[row][col] = day;
            calendar->day_month[row][col] = MONTH_PREV;
            day++;
        }
    }

    /* Compute days of current month */
    col = first_day;
    for (day = 1; day <= ndays_in_month; day++)
    {
        calendar->day[row][col] = day;
        calendar->day_month[row][col] = MONTH_CURRENT;

        col++;
        if (col == 7)
        {
            row++;
            col = 0;
        }
    }

    /* Compute days of next month */
    day = 1;
    for (; row <= 5; row++)
    {
        for (; col <= 6; col++)
        {
            calendar->day[row][col] = day;
            calendar->day_month[row][col] = MONTH_NEXT;
            day++;
        }
        col = 0;
    }
}

/**
 * hildon_calendar_get_display_options:
 * @calendar: a #HildonCalendar
 * 
 * Returns the current display options of @calendar. 
 * 
 * Return value: the display options.
 **/
HildonCalendarDisplayOptions 
hildon_calendar_get_display_options             (HildonCalendar *calendar)
{
    g_return_val_if_fail (HILDON_IS_CALENDAR (calendar), 0);

    return calendar->display_flags;
}

/**
 * hildon_calendar_set_display_options:
 * @calendar: a #HildonCalendar
 * @flags: the display options to set
 * 
 * Sets display options (whether to display the heading and the month  
 * headings).
 *
 **/
void
hildon_calendar_set_display_options             (HildonCalendar *calendar,
                                                 HildonCalendarDisplayOptions flags)
{
    HildonCalendarPrivate *private_data;
    gint resize = 0;
    GtkWidget *widget;
    gint i;
    HildonCalendarDisplayOptions old_flags;

    g_return_if_fail (HILDON_IS_CALENDAR (calendar));

    widget = GTK_WIDGET (calendar);
    private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);
    old_flags = calendar->display_flags;

    if (GTK_WIDGET_REALIZED (widget))
    {
        if ((flags ^ calendar->display_flags) & HILDON_CALENDAR_NO_MONTH_CHANGE)
        {
            resize ++;
            if (! (flags & HILDON_CALENDAR_NO_MONTH_CHANGE)
                    && (private_data->header_win))
            {
                calendar->display_flags &= ~HILDON_CALENDAR_NO_MONTH_CHANGE;
                hildon_calendar_realize_arrows (widget);
            }
            else
            {
                for (i = 0; i < 4; i++)
                {
                    if (private_data->arrow_win[i])
                    {
                        gdk_window_set_user_data (private_data->arrow_win[i], 
                                NULL);
                        gdk_window_destroy (private_data->arrow_win[i]);
                        private_data->arrow_win[i] = NULL;
                    }
                }
            }
        }

        if ((flags ^ calendar->display_flags) & HILDON_CALENDAR_SHOW_HEADING)
        {
            resize++;

            if (flags & HILDON_CALENDAR_SHOW_HEADING)
            {
                calendar->display_flags |= HILDON_CALENDAR_SHOW_HEADING;
                hildon_calendar_realize_header (widget);
            }
            else
            {
                for (i = 0; i < 4; i++)
                {
                    if (private_data->arrow_win[i])
                    {
                        gdk_window_set_user_data (private_data->arrow_win[i], 
                                NULL);
                        gdk_window_destroy (private_data->arrow_win[i]);
                        private_data->arrow_win[i] = NULL;
                    }
                }
                gdk_window_set_user_data (private_data->header_win, NULL);
                gdk_window_destroy (private_data->header_win);
                private_data->header_win = NULL;
            }
        }


        if ((flags ^ calendar->display_flags) & HILDON_CALENDAR_SHOW_DAY_NAMES)
        {
            resize++;

            if (flags & HILDON_CALENDAR_SHOW_DAY_NAMES)
            {
                calendar->display_flags |= HILDON_CALENDAR_SHOW_DAY_NAMES;
                hildon_calendar_realize_day_names (widget);
            }
            else
            {
                gdk_window_set_user_data (private_data->day_name_win, NULL);
                gdk_window_destroy (private_data->day_name_win);
                private_data->day_name_win = NULL;
            }
        }

        if ((flags ^ calendar->display_flags) & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
        {
            resize++;

            if (flags & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
            {
                calendar->display_flags |= HILDON_CALENDAR_SHOW_WEEK_NUMBERS;
                hildon_calendar_realize_week_numbers (widget);
            }
            else
            {
                gdk_window_set_user_data (private_data->week_win, NULL);
                gdk_window_destroy (private_data->week_win);
                private_data->week_win = NULL;
            }
        }

        if ((flags ^ calendar->display_flags) & HILDON_CALENDAR_WEEK_START_MONDAY)
            g_warning ("HILDON_CALENDAR_WEEK_START_MONDAY is ignored; the first day of the week is determined from the locale");

        calendar->display_flags = flags;
        if (resize)
            gtk_widget_queue_resize (GTK_WIDGET (calendar));

    } 
    else
        calendar->display_flags = flags;

    g_object_freeze_notify (G_OBJECT (calendar));
    if ((old_flags ^ calendar->display_flags) & HILDON_CALENDAR_SHOW_HEADING)
        g_object_notify (G_OBJECT (calendar), "show-heading");
    if ((old_flags ^ calendar->display_flags) & HILDON_CALENDAR_SHOW_DAY_NAMES)
        g_object_notify (G_OBJECT (calendar), "show-day-names");
    if ((old_flags ^ calendar->display_flags) & HILDON_CALENDAR_NO_MONTH_CHANGE)
        g_object_notify (G_OBJECT (calendar), "no-month-change");
    if ((old_flags ^ calendar->display_flags) & HILDON_CALENDAR_SHOW_WEEK_NUMBERS)
        g_object_notify (G_OBJECT (calendar), "show-week-numbers");
    g_object_thaw_notify (G_OBJECT (calendar));
}

gboolean
hildon_calendar_select_month                    (HildonCalendar *calendar,
                                                 guint month,
                                                 guint year)
{
    HildonCalendarPrivate *priv;

    g_return_val_if_fail (HILDON_IS_CALENDAR (calendar), FALSE);
    g_return_val_if_fail (month <= 11, FALSE);

    priv = HILDON_CALENDAR_GET_PRIVATE (calendar);

    if (priv->max_year && year > priv->max_year)
        year = priv->max_year;
    if (priv->min_year && year < priv->min_year)
        year = priv->min_year;

    calendar->month = month;
    calendar->year  = year;

    hildon_calendar_compute_days (calendar);

    gtk_widget_queue_draw (GTK_WIDGET (calendar));

    g_object_freeze_notify (G_OBJECT (calendar));
    g_object_notify (G_OBJECT (calendar), "month");
    g_object_notify (G_OBJECT (calendar), "year");
    g_object_thaw_notify (G_OBJECT (calendar));

    g_signal_emit (calendar,
            hildon_calendar_signals[MONTH_CHANGED_SIGNAL],
            0);
    return TRUE;
}

void
hildon_calendar_select_day                      (HildonCalendar *calendar,
                                                 guint day)
{
    gint row, col;
    HildonCalendarPrivate *priv;
    g_return_if_fail (HILDON_IS_CALENDAR (calendar));
    g_return_if_fail (day <= 31);
    priv = HILDON_CALENDAR_GET_PRIVATE (calendar);

    for (row = 0; row < 6; row ++)
        for (col = 0; col < 7; col++)
        {
            if (calendar->day_month[row][col] == MONTH_CURRENT
                    && calendar->day[row][col] == day)
            {
                calendar->focus_row = row;
                calendar->focus_col = col;
            }
        }

    if (calendar->month != priv->current_month || 
            calendar->year != priv->current_year)
        hildon_calendar_unmark_day (calendar, priv->current_day);

    /* Deselect the old day */
    if (calendar->selected_day > 0)
    {
        gint selected_day;

        selected_day = calendar->selected_day;
        calendar->selected_day = 0;
        if (GTK_WIDGET_DRAWABLE (GTK_WIDGET (calendar)))
        {
            hildon_calendar_paint_day_num (GTK_WIDGET (calendar), selected_day);
        }
    }

    calendar->selected_day = day;

    /*printf("Selected day = %d\n", day);*/

    /* Select the new day */
    if (day != 0)
    {
        if (GTK_WIDGET_DRAWABLE (GTK_WIDGET (calendar)))
        {
            hildon_calendar_paint_day_num (GTK_WIDGET (calendar), day);
        }
    }

    g_object_notify (G_OBJECT (calendar), "day");
    g_signal_emit (calendar,
            hildon_calendar_signals[DAY_SELECTED_SIGNAL],
            0);
}

static void
hildon_calendar_select_and_focus_day            (HildonCalendar *calendar,
                                                 guint day)
{
    gint old_focus_row = calendar->focus_row;
    gint old_focus_col = calendar->focus_col;
    gint row;
    gint col;

    for (row = 0; row < 6; row ++)
        for (col = 0; col < 7; col++)
        {
            if (calendar->day_month[row][col] == MONTH_CURRENT 
                    && calendar->day[row][col] == day)
            {
                calendar->focus_row = row;
                calendar->focus_col = col;
            }
        }

    if (old_focus_row != -1 && old_focus_col != -1)
        hildon_calendar_paint_day (GTK_WIDGET (calendar), old_focus_row, old_focus_col);

    hildon_calendar_select_day (calendar, day);
}

void
hildon_calendar_clear_marks                     (HildonCalendar *calendar)
{
    guint day;

    g_return_if_fail (HILDON_IS_CALENDAR (calendar));

    for (day = 0; day < 31; day++)
    {
        calendar->marked_date[day] = FALSE;
    }

    calendar->num_marked_dates = 0;

    if (GTK_WIDGET_DRAWABLE (calendar))
    {
        hildon_calendar_paint_main (GTK_WIDGET (calendar));
    }
}

gboolean
hildon_calendar_mark_day                        (HildonCalendar *calendar,
                                                 guint day)
{
    g_return_val_if_fail (HILDON_IS_CALENDAR (calendar), FALSE);
    if (day >= 1 && day <= 31 && calendar->marked_date[day-1] == FALSE)
    {
        calendar->marked_date[day - 1] = TRUE;
        calendar->num_marked_dates++;

        if (GTK_WIDGET_DRAWABLE (GTK_WIDGET (calendar)))
            hildon_calendar_paint_day_num (GTK_WIDGET (calendar), day-1);
    }

    return TRUE;
}

gboolean
hildon_calendar_unmark_day                      (HildonCalendar *calendar,
                                                 guint day)
{
    g_return_val_if_fail (HILDON_IS_CALENDAR (calendar), FALSE);

    if (day >= 1 && day <= 31 && calendar->marked_date[day-1] == TRUE)
    {
        calendar->marked_date[day - 1] = FALSE;
        calendar->num_marked_dates--;

        if (GTK_WIDGET_DRAWABLE (GTK_WIDGET (calendar)))
            hildon_calendar_paint_day_num (GTK_WIDGET (calendar), day-1);
    }

    return TRUE;
}

void
hildon_calendar_get_date                        (HildonCalendar *calendar,
                                                 guint *year,
                                                 guint *month,
                                                 guint *day)
{
    g_return_if_fail (HILDON_IS_CALENDAR (calendar));

    if (year)
        *year = calendar->year;

    if (month)
        *month = calendar->month;

    if (day)
        *day = calendar->selected_day;
}

static void
arrow_action                                    (HildonCalendar *calendar,
                                                 guint arrow)
{
    switch (arrow)
    {
        case ARROW_YEAR_LEFT:
            hildon_calendar_set_year_prev (calendar);
            break;
        case ARROW_YEAR_RIGHT:
            hildon_calendar_set_year_next (calendar);
            break;
        case ARROW_MONTH_LEFT:
            hildon_calendar_set_month_prev (calendar);
            break;
        case ARROW_MONTH_RIGHT:
            hildon_calendar_set_month_next (calendar);
            break;
        default:;
                /* do nothing */
    }

    hildon_calendar_select_and_focus_day(calendar, calendar->selected_day); 
}

static gboolean
calendar_timer                                  (gpointer data)
{
    HildonCalendar *calendar = data;
    HildonCalendarPrivate *private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);
    gboolean retval = FALSE;
    GtkSettings *settings;
    guint timeout;

    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-timeout-repeat", &timeout, NULL);

    GDK_THREADS_ENTER ();

    if (private_data->timer)
    {
        arrow_action (calendar, private_data->click_child);

        if (private_data->need_timer)
        {
            private_data->need_timer = FALSE;
            private_data->timer = g_timeout_add (/*CALENDAR_TIMER_DELAY*/timeout, 
                    (GSourceFunc) calendar_timer, 
                    (gpointer) calendar);
        }
        else 
            retval = TRUE;
    }

    GDK_THREADS_LEAVE ();

    return retval;
}

static void
start_spinning                                  (GtkWidget *widget,
                                                 gint click_child)
{
    HildonCalendarPrivate *private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    GtkSettings *settings;
    guint timeout;

    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-timeout-repeat", &timeout, NULL);

    private_data->click_child = click_child;

    if (!private_data->timer)
    {
        private_data->need_timer = TRUE;
        private_data->timer = g_timeout_add (/*CALENDAR_INITIAL_TIMER_DELAY*/timeout, 
                calendar_timer,
                (gpointer) widget);
    }
}

static void
stop_spinning                                   (GtkWidget *widget)
{
    HildonCalendarPrivate *private_data;

    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (private_data->timer)
    {
        g_source_remove (private_data->timer);
        private_data->timer = 0;
        private_data->need_timer = FALSE;
    }
}

static void
hildon_calendar_destroy                         (GtkObject *object)
{
    stop_spinning (GTK_WIDGET (object));

    GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
hildon_calendar_grab_notify                     (GtkWidget *widget,
                                                 gboolean   was_grabbed)
{
    if (!was_grabbed)
        stop_spinning (widget);
}

static gboolean
hildon_calendar_focus_out                       (GtkWidget *widget,
                                                 GdkEventFocus *event)
{
    HildonCalendarPrivate *private_data;

    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    stop_spinning (widget);

    private_data->in_drag = 0; 

    return FALSE;
}

static gboolean
hildon_calendar_button_press                    (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint arrow = -1;
    gboolean hildonlike;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    gtk_widget_style_get (widget, "hildonlike", &hildonlike, NULL);

    if (!hildonlike || event->type == GDK_2BUTTON_PRESS)
    {
        if (event->window == private_data->main_win)
            hildon_calendar_main_button (widget, event);
    }
    else if (hildonlike && (event->window == private_data->main_win))
    {
        gint x = (gint) (event->x);
        gint y = (gint) (event->y);
        gint row = row_from_y (calendar, y);
        gint col = column_from_x (calendar, x);
        private_data->pressed_day = calendar->day[row][col];

        if ((calendar->year == private_data->min_year &&
                    calendar->month == 0
                    && calendar->day_month[row][col] == MONTH_PREV) ||
                (calendar->year == private_data->max_year &&
                 calendar->month == 11 &&
                 calendar->day_month[row][col] == MONTH_NEXT))
        {}
        else if (calendar->day_month[row][col] == MONTH_CURRENT)
            hildon_calendar_select_and_focus_day (calendar, private_data->pressed_day);

        /* Remember month where button was pressed */
        private_data->pressed_month = calendar->month;
        private_data->slide_stylus = TRUE;
    } 

    if (!GTK_WIDGET_HAS_FOCUS (widget))
        gtk_widget_grab_focus (widget);

    for (arrow = ARROW_YEAR_LEFT; arrow <= ARROW_MONTH_RIGHT; arrow++)
    {
        if (event->window == private_data->arrow_win[arrow])
        {

            /* only call the action on single click, not double */
            if (event->type == GDK_BUTTON_PRESS)
            {
                if (event->button == 1)
                    start_spinning (widget, arrow);

                arrow_action (calendar, arrow);         
            }

            return TRUE;
        }
    }

    return TRUE;
}

static gboolean
hildon_calendar_button_release                  (GtkWidget *widget,
                                                 GdkEventButton *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gboolean hildonlike;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    gtk_widget_style_get(widget, "hildonlike", &hildonlike,
            NULL);

    if (hildonlike && (event->window == private_data->main_win))
    {
        hildon_calendar_main_button (widget, event);
        gint x = (gint) (event->x);
        gint y = (gint) (event->y);
        gint row = row_from_y (calendar, y);
        gint col = column_from_x (calendar, x);
        private_data->prev_col = -1;
        private_data->prev_row = -1;

        if ((private_data->pressed_day == calendar->day[row][col]) &&
                (private_data->pressed_month == calendar->month))
        {
            if (!private_data->is_bad_day)
            {
                g_signal_emit (calendar, hildon_calendar_signals[SELECTED_DATE_SIGNAL], 0);
            }
            else
            {
                private_data->is_bad_day = FALSE;
            }
        }
    }

    if (event->button == 1) 
    {
        stop_spinning (widget);

        if (private_data->in_drag)
            private_data->in_drag = 0;
    }

    private_data->slide_stylus = FALSE;
    return TRUE;
}

static gboolean
hildon_calendar_motion_notify                   (GtkWidget *widget,
                                                 GdkEventMotion *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint event_x, event_y;
    gint row, col;
    gint old_row, old_col;
    gboolean hildonlike;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    event_x = (gint) (event->x);
    event_y = (gint) (event->y);

    gtk_widget_style_get(widget, "hildonlike", &hildonlike,
            NULL);

    if (event->window == private_data->main_win)
    {
        if (hildonlike)
        {
            /* Hildon: make active day to move, when stylus is slided */
            if (private_data->slide_stylus)
            {
                gint c_row = row_from_y (calendar, event_y);
                gint c_col = column_from_x (calendar, event_x);

                if (calendar->day_month[c_row][c_col] == MONTH_PREV ||
                        calendar->day_month[c_row][c_col] == MONTH_NEXT)
                { }
                else if ((private_data->prev_row != c_row || private_data->prev_col != c_col) &&
                        (calendar->highlight_row != -1 && calendar->highlight_col != -1))
                {
                    hildon_calendar_select_and_focus_day (calendar, 
                            calendar->day[c_row][c_col]);
                    /* Update passive focus indicators work weekday number and name */
                    hildon_calendar_paint_week_numbers (GTK_WIDGET (calendar));
                    hildon_calendar_paint_day_names (GTK_WIDGET (calendar));
                }
                private_data->prev_col = c_col;
                private_data->prev_row = c_row;    
            }
        }
        if (private_data->in_drag) 
        {
            if (gtk_drag_check_threshold (widget,
                        private_data->drag_start_x, private_data->drag_start_y,
                        event->x, event->y))
            {
                GdkDragContext *context;
                GtkTargetList *target_list = gtk_target_list_new (NULL, 0);
                gtk_target_list_add_text_targets (target_list, 0);
                context = gtk_drag_begin (widget, target_list, GDK_ACTION_COPY,
                        1, (GdkEvent *)event);


                private_data->in_drag = 0;

                gtk_target_list_unref (target_list);
                gtk_drag_set_icon_default (context);
            }
        }
        else 
        {
            row = row_from_y (calendar, event_y);
            col = column_from_x (calendar, event_x);

            if (row != calendar->highlight_row || calendar->highlight_col != col)
            {
                old_row = calendar->highlight_row;
                old_col = calendar->highlight_col;
                if (old_row > -1 && old_col > -1)
                {
                    calendar->highlight_row = -1;
                    calendar->highlight_col = -1;
                    hildon_calendar_paint_day (widget, old_row, old_col);
                }

                calendar->highlight_row = row;
                calendar->highlight_col = col;

                if (row > -1 && col > -1)
                    hildon_calendar_paint_day (widget, row, col);
            }
        }
    }

    return TRUE;
}

static gboolean
hildon_calendar_enter_notify                    (GtkWidget *widget,
                                                 GdkEventCrossing *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (event->window == private_data->arrow_win[ARROW_MONTH_LEFT])
    {
        private_data->arrow_state[ARROW_MONTH_LEFT] = GTK_STATE_PRELIGHT;
        hildon_calendar_paint_arrow (widget, ARROW_MONTH_LEFT);
    }

    if (event->window == private_data->arrow_win[ARROW_MONTH_RIGHT])
    {
        private_data->arrow_state[ARROW_MONTH_RIGHT] = GTK_STATE_PRELIGHT;
        hildon_calendar_paint_arrow (widget, ARROW_MONTH_RIGHT);
    }

    if (event->window == private_data->arrow_win[ARROW_YEAR_LEFT])
    {
        private_data->arrow_state[ARROW_YEAR_LEFT] = GTK_STATE_PRELIGHT;
        hildon_calendar_paint_arrow (widget, ARROW_YEAR_LEFT);
    }

    if (event->window == private_data->arrow_win[ARROW_YEAR_RIGHT])
    {
        private_data->arrow_state[ARROW_YEAR_RIGHT] = GTK_STATE_PRELIGHT;
        hildon_calendar_paint_arrow (widget, ARROW_YEAR_RIGHT);
    }

    return TRUE;
}

static gboolean
hildon_calendar_leave_notify                    (GtkWidget *widget,
                                                 GdkEventCrossing *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint row;
    gint col;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (event->window == private_data->main_win)
    {
        row = calendar->highlight_row;
        col = calendar->highlight_col;
        calendar->highlight_row = -1;
        calendar->highlight_col = -1;
        if (row > -1 && col > -1)
            hildon_calendar_paint_day (widget, row, col);
    }

    if (event->window == private_data->arrow_win[ARROW_MONTH_LEFT])
    {
        private_data->arrow_state[ARROW_MONTH_LEFT] = GTK_STATE_NORMAL;
        hildon_calendar_paint_arrow (widget, ARROW_MONTH_LEFT);
    }

    if (event->window == private_data->arrow_win[ARROW_MONTH_RIGHT])
    {
        private_data->arrow_state[ARROW_MONTH_RIGHT] = GTK_STATE_NORMAL;
        hildon_calendar_paint_arrow (widget, ARROW_MONTH_RIGHT);
    }

    if (event->window == private_data->arrow_win[ARROW_YEAR_LEFT])
    {
        private_data->arrow_state[ARROW_YEAR_LEFT] = GTK_STATE_NORMAL;
        hildon_calendar_paint_arrow (widget, ARROW_YEAR_LEFT);
    }

    if (event->window == private_data->arrow_win[ARROW_YEAR_RIGHT])
    {
        private_data->arrow_state[ARROW_YEAR_RIGHT] = GTK_STATE_NORMAL;
        hildon_calendar_paint_arrow (widget, ARROW_YEAR_RIGHT);
    }

    return TRUE;
}

static void
hildon_calendar_paint_arrow                     (GtkWidget *widget,
                                                 guint      arrow)
{
    HildonCalendarPrivate *private_data;
    GdkWindow *window;
    GdkGC *gc;
    HildonCalendar *calendar;
    gint state;
    gboolean hildonlike;
    guint arrow_hlength, arrow_vlength;
    /*  gint width, height;*/

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    gtk_widget_style_get (widget,
            "scroll-arrow-hlength", &arrow_hlength,
            "scroll-arrow-vlength", &arrow_vlength,
            "hildonlike", &hildonlike,
            NULL);

    if (private_data->freeze_count)
    {
        private_data->dirty_header = 1;
        return;
    }
    window = private_data->arrow_win[arrow];
    if (window)
    {
        state = private_data->arrow_state[arrow];
        gc = calendar->gc;

        /*      gdk_window_clear (window);*/
        gdk_window_set_background (window, &(widget)->style->bg[state]);
        /*      gdk_drawable_get_size (window, &width, &height);*/
        /*      gdk_window_clear_area (window,
                0,0,
                width,height);*/

        gdk_window_clear(window);

        /* Hildon: added support for dimmed arrows */
        if (hildonlike &&
                ((private_data->min_year && calendar->year <= private_data->min_year) ||
                 (private_data->max_year && calendar->year >= private_data->max_year)))
        {
            if (private_data->min_year &&
                    calendar->year <= private_data->min_year)
            {
                if (arrow == ARROW_YEAR_LEFT)
                    gtk_paint_arrow (widget->style, window, GTK_STATE_INSENSITIVE,
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_LEFT, TRUE,
                            0, 0, arrow_vlength, arrow_hlength);
                else if (arrow == ARROW_YEAR_RIGHT || arrow == ARROW_MONTH_RIGHT)
                    gtk_paint_arrow (widget->style, window, state,
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_RIGHT, TRUE, 
                            0, 0, arrow_vlength, arrow_hlength);
                else if (arrow == ARROW_MONTH_LEFT && calendar->month != 0)
                    gtk_paint_arrow (widget->style, window, state,
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_LEFT, TRUE,
                            0, 0, arrow_vlength, arrow_hlength);
                else if (arrow == ARROW_MONTH_LEFT && !calendar->month)
                    gtk_paint_arrow (widget->style, window, GTK_STATE_INSENSITIVE,
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_LEFT, TRUE,
                            0, 0, arrow_vlength, arrow_hlength);
            }
            else if (private_data->max_year &&
                    calendar->year >= private_data->max_year)
            {
                if (arrow == ARROW_YEAR_RIGHT)
                    gtk_paint_arrow (widget->style, window, GTK_STATE_INSENSITIVE, 
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_RIGHT, TRUE, 
                            0, 0, arrow_vlength, arrow_hlength);
                else if (arrow == ARROW_YEAR_LEFT || arrow == ARROW_MONTH_LEFT)
                    gtk_paint_arrow (widget->style, window, state, 
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_LEFT, TRUE, 
                            0, 0, arrow_vlength, arrow_hlength);
                else if (arrow == ARROW_MONTH_RIGHT && calendar->month != 11)
                    gtk_paint_arrow (widget->style, window, state,
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_RIGHT, TRUE,
                            0, 0, arrow_vlength, arrow_hlength);
                else if (arrow == ARROW_MONTH_RIGHT && calendar->month == 11)
                    gtk_paint_arrow (widget->style, window, GTK_STATE_INSENSITIVE,
                            GTK_SHADOW_OUT, NULL, widget, "calendar",
                            GTK_ARROW_RIGHT, TRUE,
                            0, 0, arrow_vlength, arrow_hlength);
            }
        }
        else
        { 
            if (arrow == ARROW_MONTH_LEFT || arrow == ARROW_YEAR_LEFT)
                gtk_paint_arrow (widget->style, window, state, 
                        GTK_SHADOW_OUT, NULL, widget, "calendar",
                        GTK_ARROW_LEFT, TRUE, 
                        /*                     width/2 - 3, height/2 - 4, 8, 8);*/
                                0, 0, arrow_vlength, arrow_hlength);
            else 
                gtk_paint_arrow (widget->style, window, state,
                        GTK_SHADOW_OUT, NULL, widget, "calendar",
                        GTK_ARROW_RIGHT, TRUE, 
                        /*                     width/2 - 2, height/2 - 4, 8, 8);*/
                                0, 0, arrow_vlength, arrow_hlength);
        }
    }
}

void
hildon_calendar_freeze                          (HildonCalendar *calendar)
{
    g_return_if_fail (HILDON_IS_CALENDAR (calendar));

    HILDON_CALENDAR_GET_PRIVATE (calendar)->freeze_count++;
}

void
hildon_calendar_thaw                            (HildonCalendar *calendar)
{
    HildonCalendarPrivate *private_data;

    g_return_if_fail (HILDON_IS_CALENDAR (calendar));

    private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);

    if (private_data->freeze_count)
        if (!(--private_data->freeze_count))
        {
            if (private_data->dirty_header)
                if (GTK_WIDGET_DRAWABLE (calendar))
                    hildon_calendar_paint_header (GTK_WIDGET (calendar));

            if (private_data->dirty_day_names)
                if (GTK_WIDGET_DRAWABLE (calendar))
                    hildon_calendar_paint_day_names (GTK_WIDGET (calendar));

            if (private_data->dirty_week)
                if (GTK_WIDGET_DRAWABLE (calendar))
                    hildon_calendar_paint_week_numbers (GTK_WIDGET (calendar));

            if (private_data->dirty_main)
                if (GTK_WIDGET_DRAWABLE (calendar))
                    hildon_calendar_paint_main (GTK_WIDGET (calendar));
        }
}

static void
hildon_calendar_set_background                  (GtkWidget *widget)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint i;

    calendar = HILDON_CALENDAR (widget);
    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (GTK_WIDGET_REALIZED (widget))
    {
        for (i = 0; i < 4; i++)
        {
            if (private_data->arrow_win[i])
                gdk_window_set_background (private_data->arrow_win[i], 
                        HEADER_BG_COLOR (widget));
        }
        if (private_data->header_win)
            gdk_window_set_background (private_data->header_win, 
                    HEADER_BG_COLOR (widget));
        if (private_data->day_name_win)
            gdk_window_set_background (private_data->day_name_win, 
                    BACKGROUND_COLOR (widget));
        if (private_data->week_win)
            gdk_window_set_background (private_data->week_win,
                    BACKGROUND_COLOR (widget));
        if (private_data->main_win)
            gdk_window_set_background (private_data->main_win,
                    BACKGROUND_COLOR (widget));
        if (widget->window)
            gdk_window_set_background (widget->window,
                    BACKGROUND_COLOR (widget)); 
    }
}

static void
hildon_calendar_style_set                       (GtkWidget *widget,
                                                 GtkStyle *previous_style)
{
    if (previous_style && GTK_WIDGET_REALIZED (widget))
        hildon_calendar_set_background(widget);
}

static void
hildon_calendar_state_changed                   (GtkWidget *widget,
                                                 GtkStateType previous_state)
{
    HildonCalendarPrivate *private_data;
    int i;

    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (!GTK_WIDGET_IS_SENSITIVE (widget))
    {
        private_data->in_drag = 0;
        stop_spinning (widget);    
    }

    for (i = 0; i < 4; i++)
        if (GTK_WIDGET_IS_SENSITIVE (widget))
            private_data->arrow_state[i] = GTK_STATE_NORMAL;
        else 
            private_data->arrow_state[i] = GTK_STATE_INSENSITIVE;

    hildon_calendar_set_background (widget);
}

static void
hildon_calendar_finalize                        (GObject *object)
{
    HildonCalendarPrivate *private_data;
    int i;

    private_data = HILDON_CALENDAR_GET_PRIVATE (object);

    for (i = 0; i < 7; i++)
        g_free (private_data->abbreviated_dayname[i]);
    for (i = 0; i < 12; i++)
        g_free (private_data->monthname[i]);
    g_free (private_data);

    (* G_OBJECT_CLASS (parent_class)->finalize) (object);
}

static gboolean
hildon_calendar_scroll                          (GtkWidget *widget,
                                                 GdkEventScroll *event)
{
    HildonCalendar *calendar = HILDON_CALENDAR (widget);

    if (event->direction == GDK_SCROLL_UP) 
    {
        if (!GTK_WIDGET_HAS_FOCUS (widget))
            gtk_widget_grab_focus (widget);
        hildon_calendar_set_month_prev (calendar);
    }
    else if (event->direction == GDK_SCROLL_DOWN) 
    {
        if (!GTK_WIDGET_HAS_FOCUS (widget))
            gtk_widget_grab_focus (widget);
        hildon_calendar_set_month_next (calendar);
    }
    else
        return FALSE;

    return TRUE;
}

static void 
move_focus                                      (HildonCalendar *calendar, 
                                                 gint direction)
{
    GtkTextDirection text_dir = gtk_widget_get_direction (GTK_WIDGET (calendar));

    if ((text_dir == GTK_TEXT_DIR_LTR && direction == -1) ||
            (text_dir == GTK_TEXT_DIR_RTL && direction == 1)) 
    {
        if (calendar->focus_col > 0)
            calendar->focus_col--;
        else if (calendar->focus_row > 0)
        {
            calendar->focus_col = 6;
            calendar->focus_row--;
        }
    }
    else 
    {
        if (calendar->focus_col < 6)
            calendar->focus_col++;
        else if (calendar->focus_row < 5)
        {
            calendar->focus_col = 0;
            calendar->focus_row++;
        }
    }
}

static gboolean
hildon_calendar_key_press                       (GtkWidget *widget,
                                                 GdkEventKey *event)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *priv;
    GtkSettings *settings;
    gint return_val;
    gint old_focus_row;
    gint old_focus_col;
    gint row, col, day;
    gboolean knav;

    calendar = HILDON_CALENDAR (widget);
    priv = HILDON_CALENDAR_GET_PRIVATE (calendar);
    return_val = FALSE;

    old_focus_row = calendar->focus_row;
    old_focus_col = calendar->focus_col;

    settings = gtk_settings_get_default ();
    g_object_get (settings, "hildon-keyboard-navigation", &knav, NULL);

    switch (event->keyval)
    {
        case GDK_KP_Left:
        case GDK_Left:
            return_val = TRUE;
            if (event->state & GDK_CONTROL_MASK)
                hildon_calendar_set_month_prev (calendar);
            else
            {
                /* if we are at the first allowed day of the minimum year/month then do nothing */
                if (calendar->year == priv->min_year && calendar->month == 0 &&
                        calendar->day_month[old_focus_row][old_focus_col-1] == MONTH_PREV) 
                {
                    g_signal_emit (calendar, hildon_calendar_signals[ERRONEOUS_DATE_SIGNAL], 0);
                    return TRUE;
                }
                else /* else normal */
                {
                    move_focus (calendar, -1);
                    if (!knav)
                    {
                        hildon_calendar_paint_day (widget, old_focus_row, old_focus_col);
                        hildon_calendar_paint_day (widget, calendar->focus_row,
                                calendar->focus_col);
                    }
                    else if (knav)
                    {
                        gint day_month = calendar->day_month[calendar->focus_row][calendar->focus_col];
                        if (day_month == MONTH_CURRENT && calendar->selected_day != 1)
                        {
                            hildon_calendar_select_day(calendar, calendar->selected_day - 1);
                        }
                        else
                        {
                            if (calendar->month != 0) {
                                calendar->selected_day = month_length[leap (calendar->year)][calendar->month];
                            } else {
                                calendar->selected_day = month_length[leap (calendar->year -1)][12];
                            }
                            hildon_calendar_set_month_prev (calendar);
                        }
                        hildon_calendar_paint_week_numbers (GTK_WIDGET (calendar));
                        hildon_calendar_paint_day_names (GTK_WIDGET (calendar));
                    }
                }
            }
            break;
        case GDK_KP_Right:
        case GDK_Right:
            return_val = TRUE;
            if (event->state & GDK_CONTROL_MASK)
                hildon_calendar_set_month_next (calendar);
            else
            {
                if (calendar->year == priv->max_year && calendar->month == 11 &&
                        calendar->day_month[old_focus_row][old_focus_col+1] == MONTH_NEXT)
                {
                    g_signal_emit (calendar, hildon_calendar_signals[ERRONEOUS_DATE_SIGNAL], 0);
                    return TRUE;
                }
                else 
                {
                    move_focus (calendar, 1);
                    if (!knav)
                    {
                        hildon_calendar_paint_day (widget, old_focus_row, old_focus_col);
                        hildon_calendar_paint_day (widget, calendar->focus_row,
                                calendar->focus_col);
                    }
                    else if (knav)
                    {
                        gint day_month = calendar->day_month[calendar->focus_row][calendar->focus_col];
                        if (day_month == MONTH_CURRENT)
                        {  
                            hildon_calendar_select_day (calendar, calendar->selected_day + 1);
                        }
                        else
                        {
                            calendar->selected_day = 1;
                            hildon_calendar_set_month_next (calendar);
                        }
                        hildon_calendar_paint_week_numbers (GTK_WIDGET (calendar));
                        hildon_calendar_paint_day_names (GTK_WIDGET (calendar)); 
                    } 
                }
            }
            break;
        case GDK_KP_Up:
        case GDK_Up:
            return_val = TRUE;
            if (event->state & GDK_CONTROL_MASK)
                hildon_calendar_set_year_prev (calendar);
            else
            {
                if (calendar->year == priv->min_year && calendar->month == 0 &&
                        calendar->day_month[old_focus_row-1][old_focus_col] == MONTH_PREV)
                {
                    g_signal_emit (calendar, hildon_calendar_signals[ERRONEOUS_DATE_SIGNAL], 0);
                    return TRUE;
                }
                else 
                {
                    if (calendar->focus_row > 0)
                        calendar->focus_row--;
                    if (!knav)
                    {
                        hildon_calendar_paint_day (widget, old_focus_row, old_focus_col);
                        hildon_calendar_paint_day (widget, calendar->focus_row,
                                calendar->focus_col);
                    }
                    else if (knav)
                    {
                        gint day_month = calendar->day_month[calendar->focus_row][calendar->focus_col];
                        if (day_month == MONTH_CURRENT)
                        {
                            if ((calendar->selected_day - 7) <= 0)
                            {
                                if (calendar->month != 0)
                                    calendar->selected_day = month_length[leap (calendar->year)][calendar->month];
                                else
                                    calendar->selected_day = month_length[leap (calendar->year - 1)][12];
                                hildon_calendar_set_month_prev (calendar); 
                            }
                            else
                            {
                                hildon_calendar_select_day (calendar, calendar->selected_day - 7);
                            }
                        }
                        else
                        {
                            calendar->selected_day = calendar->day[calendar->focus_row][calendar->focus_col];
                            hildon_calendar_set_month_prev (calendar);
                        }
                        hildon_calendar_paint_week_numbers (GTK_WIDGET (calendar));
                        hildon_calendar_paint_day_names (GTK_WIDGET (calendar)); 
                    }
                }
            }
            break;
        case GDK_KP_Down:
        case GDK_Down:
            return_val = TRUE;
            if (event->state & GDK_CONTROL_MASK)
                hildon_calendar_set_year_next (calendar);
            else
            {
                if (calendar->year == priv->max_year && calendar->month == 11 &&
                        calendar->day_month[old_focus_row+1][old_focus_col] == MONTH_NEXT)
                {
                    g_signal_emit (calendar, hildon_calendar_signals[ERRONEOUS_DATE_SIGNAL], 0);
                    return TRUE;
                }
                else 
                {

                    if (calendar->focus_row < 5)
                        calendar->focus_row++;
                    if (!knav)
                    {
                        hildon_calendar_paint_day (widget, old_focus_row, old_focus_col);
                        hildon_calendar_paint_day (widget, calendar->focus_row,
                                calendar->focus_col);
                    }
                    else if (knav)
                    {
                        gint day_month = calendar->day_month[calendar->focus_row][calendar->focus_col];
                        if (day_month == MONTH_CURRENT)
                        {
                            if ((calendar->selected_day + 7) > 
                                    month_length[leap (calendar->year)][calendar->month + 1])
                            {
                                calendar->selected_day = 1;
                                hildon_calendar_set_month_next (calendar);
                            }
                            else
                            {
                                hildon_calendar_select_day (calendar, calendar->selected_day + 7);
                            }
                        }
                        else
                        {
                            calendar->selected_day = calendar->day[calendar->focus_row][calendar->focus_col];
                            hildon_calendar_set_month_next (calendar);
                        }
                        hildon_calendar_paint_week_numbers (GTK_WIDGET (calendar));
                        hildon_calendar_paint_day_names (GTK_WIDGET (calendar));
                    } 
                }
            }

            break;
        case GDK_KP_Space:
        case GDK_space:
            row = calendar->focus_row;
            col = calendar->focus_col;
            day = calendar->day[row][col];

            if (row > -1 && col > -1)
            {
                return_val = TRUE;
                hildon_calendar_freeze (calendar);         

                if (calendar->day_month[row][col] == MONTH_PREV)
                    hildon_calendar_set_month_prev (calendar);
                else if (calendar->day_month[row][col] == MONTH_NEXT)
                    hildon_calendar_set_month_next (calendar);

                hildon_calendar_select_and_focus_day (calendar, day);

                hildon_calendar_thaw (calendar);   
            }
    }   

    return return_val;
}

static void
hildon_calendar_set_display_option              (HildonCalendar *calendar,
                                                 HildonCalendarDisplayOptions flag,
                                                 gboolean setting)
{
    HildonCalendarDisplayOptions flags;

    if (setting) 
        flags = calendar->display_flags | flag;
    else
        flags = calendar->display_flags & ~flag; 

    hildon_calendar_set_display_options (calendar, flags);
}

static gboolean
hildon_calendar_get_display_option              (HildonCalendar *calendar,
                                                 HildonCalendarDisplayOptions flag)
{
    return (calendar->display_flags & flag) != 0;
}


static void 
hildon_calendar_set_property                    (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec   *pspec)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;
    gint val;

    calendar = HILDON_CALENDAR (object);
    private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);

    switch (prop_id) 
    {
        case PROP_YEAR:
            hildon_calendar_select_month (calendar,
                    calendar->month,
                    g_value_get_int (value));
            break;
        case PROP_MONTH:
            hildon_calendar_select_month (calendar,
                    g_value_get_int (value),
                    calendar->year);
            break;
        case PROP_DAY:
            hildon_calendar_select_day (calendar,
                    g_value_get_int (value));
            break;
        case PROP_SHOW_HEADING:
            hildon_calendar_set_display_option (calendar,
                    HILDON_CALENDAR_SHOW_HEADING,
                    g_value_get_boolean (value));
            break;
        case PROP_SHOW_DAY_NAMES:
            hildon_calendar_set_display_option (calendar,
                    HILDON_CALENDAR_SHOW_DAY_NAMES,
                    g_value_get_boolean (value));
            break;
        case PROP_NO_MONTH_CHANGE:
            hildon_calendar_set_display_option (calendar,
                    HILDON_CALENDAR_NO_MONTH_CHANGE,
                    g_value_get_boolean (value));
            break;
        case PROP_SHOW_WEEK_NUMBERS:
            hildon_calendar_set_display_option (calendar,
                    HILDON_CALENDAR_SHOW_WEEK_NUMBERS,
                    g_value_get_boolean (value));
            break;
        case PROP_WEEK_START:
            private_data->week_start = g_value_get_int (value);
            break;
        case PROP_MIN_YEAR:
            val = g_value_get_int (value);
            if (val <= private_data->max_year ||
                    val == 0 || private_data->max_year == 0)
            {
                private_data->min_year = val;
                if (val && (calendar->year < val))
                    hildon_calendar_select_month (calendar,
                            calendar->month,
                            private_data->min_year);
            }
            else
                g_warning("min-year cannot be greater than max-year");
            break;
        case PROP_MAX_YEAR:
            val = g_value_get_int (value);
            if (val >= private_data->min_year ||
                    val == 0 || private_data->min_year == 0)
            {
                private_data->max_year = val;
                if (val && (calendar->year > val))
                    hildon_calendar_select_month (calendar,
                            calendar->month,
                            private_data->max_year);
            }
            else
                g_warning("max-year cannot be less than min-year");
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void 
hildon_calendar_get_property                    (GObject *object,
                                                 guint prop_id,
                                                 GValue *value,
                                                 GParamSpec *pspec)
{
    HildonCalendar *calendar;
    HildonCalendarPrivate *private_data;

    calendar = HILDON_CALENDAR (object);
    private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);

    switch (prop_id) 
    {
        case PROP_YEAR:
            g_value_set_int (value, calendar->year);
            break;
        case PROP_MONTH:
            g_value_set_int (value, calendar->month);
            break;
        case PROP_DAY:
            g_value_set_int (value, calendar->selected_day);
            break;
        case PROP_SHOW_HEADING:
            g_value_set_boolean (value, hildon_calendar_get_display_option (calendar,
                        HILDON_CALENDAR_SHOW_HEADING));
            break;
        case PROP_SHOW_DAY_NAMES:
            g_value_set_boolean (value, hildon_calendar_get_display_option (calendar,
                        HILDON_CALENDAR_SHOW_DAY_NAMES));
            break;
        case PROP_NO_MONTH_CHANGE:
            g_value_set_boolean (value, hildon_calendar_get_display_option (calendar,
                        HILDON_CALENDAR_NO_MONTH_CHANGE));
            break;
        case PROP_SHOW_WEEK_NUMBERS:
            g_value_set_boolean (value, hildon_calendar_get_display_option (calendar,
                        HILDON_CALENDAR_SHOW_WEEK_NUMBERS));
            break;
        case PROP_WEEK_START:
            g_value_set_int (value, private_data->week_start);
            break;
        case PROP_MIN_YEAR:
            g_value_set_int (value, private_data->min_year);
            break;
        case PROP_MAX_YEAR:
            g_value_set_int (value, private_data->max_year);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }

}

static void
hildon_calendar_drag_data_get                   (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 GtkSelectionData *selection_data,
                                                 guint info,
                                                 guint time)
{
    HildonCalendar *calendar = HILDON_CALENDAR (widget);
    GDate *date;
    gchar str[128];
    gsize len;

    date = g_date_new_dmy (calendar->selected_day, calendar->month + 1, calendar->year);
    len = g_date_strftime (str, 127, "%x", date);
    gtk_selection_data_set_text (selection_data, str, len);

    g_free (date);
}

/* Get/set whether drag_motion requested the drag data and
 * drag_data_received should thus not actually insert the data,
 * since the data doesn't result from a drop.
 */
static void
set_status_pending                              (GdkDragContext *context,
                                                 GdkDragAction suggested_action)
{
    g_object_set_data (G_OBJECT (context),
            "gtk-calendar-status-pending",
            GINT_TO_POINTER (suggested_action));
}

static GdkDragAction
get_status_pending                              (GdkDragContext *context)
{
    return GPOINTER_TO_INT (g_object_get_data (G_OBJECT (context),
                "gtk-calendar-status-pending"));
}

static void
hildon_calendar_drag_leave                      (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 guint time)
{
    HildonCalendarPrivate *private_data;

    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);
    private_data->drag_highlight = 0;
    gtk_drag_unhighlight (widget);

}

static gboolean
hildon_calendar_drag_motion                     (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 gint x,
                                                 gint y,
                                                 guint time)
{
    HildonCalendarPrivate *private_data;
    GdkAtom target;

    private_data = HILDON_CALENDAR_GET_PRIVATE (widget);

    if (!private_data->drag_highlight) 
    {
        private_data->drag_highlight = 1;
        gtk_drag_highlight (widget);
    }

    target = gtk_drag_dest_find_target (widget, context, NULL);
    if (target == GDK_NONE)
        gdk_drag_status (context, 0, time);
    else {
        set_status_pending (context, context->suggested_action);
        gtk_drag_get_data (widget, context, target, time);
    }

    return TRUE;
}

static gboolean
hildon_calendar_drag_drop                       (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 gint x,
                                                 gint y,
                                                 guint time)
{
    GdkAtom target;

    target = gtk_drag_dest_find_target (widget, context, NULL);  
    if (target != GDK_NONE)
    {
        gtk_drag_get_data (widget, context, 
                target, 
                time);
        return TRUE;
    }

    return FALSE;
}

static void
hildon_calendar_drag_data_received              (GtkWidget *widget,
                                                 GdkDragContext *context,
                                                 gint x,
                                                 gint y,
                                                 GtkSelectionData *selection_data,
                                                 guint info,
                                                 guint time)
{
    HildonCalendar *calendar = HILDON_CALENDAR (widget);
    guint day, month, year;
    gchar *str;
    GDate *date;
    GdkDragAction suggested_action;

    suggested_action = get_status_pending (context);

    if (suggested_action) 
    {
        set_status_pending (context, 0);

        /* We are getting this data due to a request in drag_motion,
         * rather than due to a request in drag_drop, so we are just
         * supposed to call drag_status, not actually paste in the
         * data.
         */
        str = gtk_selection_data_get_text (selection_data);
        if (str) 
        {
            date = g_date_new ();
            g_date_set_parse (date, str);
            if (!g_date_valid (date)) 
                suggested_action = 0;
            g_date_free (date);
            g_free (str);
        }
        else
            suggested_action = 0;

        gdk_drag_status (context, suggested_action, time);

        return;
    }

    date = g_date_new ();
    str = gtk_selection_data_get_text (selection_data);
    if (str) 
    {
        g_date_set_parse (date, str);
        g_free (str);
    }

    if (!g_date_valid (date)) 
    {
        g_warning ("Received invalid date data\n");
        g_date_free (date);       
        gtk_drag_finish (context, FALSE, FALSE, time);
        return;
    }

    day = g_date_get_day (date);
    month = g_date_get_month (date);
    year = g_date_get_year (date);
    g_date_free (date);   

    gtk_drag_finish (context, TRUE, FALSE, time);


    g_object_freeze_notify (G_OBJECT (calendar));
    if (!(calendar->display_flags & HILDON_CALENDAR_NO_MONTH_CHANGE)
            && (calendar->display_flags & HILDON_CALENDAR_SHOW_HEADING))
        hildon_calendar_select_month (calendar, month - 1, year);
    hildon_calendar_select_day (calendar, day);
    g_object_thaw_notify (G_OBJECT (calendar));  
}

/* This function return TRUE if we should mark date and FALSE
 *  otherwise
 */
static void
hildon_calendar_check_current_date              (HildonCalendar *calendar, 
                                                 gint x, 
                                                 gint y)
{
    HildonCalendarPrivate *private_data;

    private_data = HILDON_CALENDAR_GET_PRIVATE (calendar);

    if (calendar->month == private_data->current_month && 
            calendar->year == private_data->current_year)
    {
        gtk_paint_box( GTK_WIDGET (calendar)->style,
                private_data->main_win,
                GTK_STATE_NORMAL,
                GTK_SHADOW_NONE, NULL,
                GTK_WIDGET (calendar), "current-day",
                x, y,
                HILDON_DAY_WIDTH,
                HILDON_DAY_HEIGHT);
    }
}

#define                                         __HILDON_CALENDAR_C__
