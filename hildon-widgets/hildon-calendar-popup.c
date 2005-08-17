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

/* HILDON DOC
 * @shortdesc: CalendarPopup allows choosing a date from a popup calendar.
 * @longdesc: The Calendar popup is a dialog that contains a GtkCalendar 
 * widget. The pop-up is cancelled by
 * either a mouse click outside of the dialog or pressing the ESC
 * key.
 * </para><para>
 * 
 * @seealso: #HildonDateEditor, #HildonTimeEditor
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <gtk/gtkcalendar.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <langinfo.h>
#include <time.h>
#include <libintl.h>
#include <hildon-widgets/hildon-calendar-popup.h>
#include <hildon-widgets/gtk-infoprint.h>

#define MAX_YEAR 2100
#define MIN_YEAR 1980

#define _(String) dgettext(PACKAGE, String)

#define HILDON_CALENDAR_POPUP_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE\
        ((obj), HILDON_CALENDAR_POPUP_TYPE, HildonCalendarPopupPrivate));

static GtkDialog *parent_class;

typedef struct _HildonCalendarPopupPrivate HildonCalendarPopupPrivate;

static void init_dmy(guint year, guint month, guint day, guint * d,
                     guint * m, guint * y);

static void
hildon_calendar_popup_class_init(HildonCalendarPopupClass * cal_class);

static void hildon_calendar_popup_init(HildonCalendarPopup * cal);

static gboolean hildon_calendar_day_selected(GtkWidget * widget,
                                             gpointer data);

static void hildon_calendar_allow_exit(GtkWidget * self, gpointer data);

static gboolean hildon_calendar_deny_exit(GtkWidget * self);

static gboolean hildon_key_pressed(GtkWidget * widget, GdkEventKey * event,
                                   gpointer data);

struct _HildonCalendarPopupPrivate {
    GtkWidget *cal;
    gboolean can_exit;
};

GType hildon_calendar_popup_get_type(void)
{
    static GType popup_type = 0;

    if (!popup_type) {
        static const GTypeInfo popup_info = {
            sizeof(HildonCalendarPopupClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_calendar_popup_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonCalendarPopup),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_calendar_popup_init,
        };
        popup_type = g_type_register_static(GTK_TYPE_DIALOG,
                                            "HildonCalendarPopup",
                                            &popup_info, 0);
    }

    return popup_type;
}

/**
 * hildon_calendar_popup_new:
 * @parent: parent window for dialog
 * @year: initial year
 * @month: initial month
 * @day: initial day
 *
 * This function returns a new HildonCalendarPopup. The initially
 * selected date is specified by the parameters (year, month, day).
 * If the specified date is invalid, the current date is used. 
 *
 * Return value: Pointer to a new @HildonCalendarPopup widget.
 **/
GtkWidget *hildon_calendar_popup_new(GtkWindow * parent, guint year,
                                     guint month, guint day)
{
    HildonCalendarPopup *cal = NULL;
    HildonCalendarPopupPrivate *priv;
    guint dtmp, mtmp, ytmp;

    init_dmy(year, month, day, &dtmp, &mtmp, &ytmp);

    cal = HILDON_CALENDAR_POPUP(g_object_new(HILDON_CALENDAR_POPUP_TYPE,
                                             NULL));
    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    if (parent) {
        gtk_window_set_transient_for(GTK_WINDOW(cal), parent);
    }

    /* Select day, month, year */
    gtk_calendar_select_month(GTK_CALENDAR(priv->cal), mtmp - 1, ytmp);
    gtk_calendar_select_day(GTK_CALENDAR(priv->cal), dtmp);

    return GTK_WIDGET(cal);
}

/**
 * hildon_calendar_popup_set_date:
 * @cal: the @HildonCalendarPopup widget
 * @year: year
 * @month: month
 * @day: day
 *
 * This function activates a new date on the calendar popup.
 **/


void
hildon_calendar_popup_set_date(HildonCalendarPopup * cal,
                               guint year, guint month, guint day)
{
    HildonCalendarPopupPrivate *priv;

    g_return_if_fail(HILDON_IS_CALENDAR_POPUP(cal));

    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);
    gtk_calendar_clear_marks(GTK_CALENDAR(priv->cal));

    gtk_calendar_select_month(GTK_CALENDAR(priv->cal), month - 1, year);
    gtk_calendar_select_day(GTK_CALENDAR(priv->cal), day);
}

/**
 * hildon_calendar_popup_get_date:
 * @cal: the @HildonCalendarPopup widget
 * @year: year
 * @month: month
 * @day: day
 *
 * This function is used to get the currently selected year, month,
 * and day. 
 **/

void
hildon_calendar_popup_get_date(HildonCalendarPopup * cal,
                               guint * year, guint * month, guint * day)
{
    HildonCalendarPopupPrivate *priv;

    g_return_if_fail(HILDON_IS_CALENDAR_POPUP(cal));

    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);
    gtk_calendar_get_date(GTK_CALENDAR(priv->cal), year, month, day);
    *month = *month + 1;

    if (!g_date_valid_dmy(*day, *month, *year)) {
        *day = g_date_get_days_in_month(*month, *year);
    }
}

static void
hildon_calendar_popup_class_init(HildonCalendarPopupClass * cal_class)
{
    parent_class = g_type_class_peek_parent(cal_class);
    g_type_class_add_private(cal_class,
                             sizeof(HildonCalendarPopupPrivate));
}

static void hildon_calendar_popup_init(HildonCalendarPopup * cal)
{
    HildonCalendarPopupPrivate *priv;
    static int set_domain = 1;
    gchar *week_start;

    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    if (set_domain) {
        (void) bindtextdomain(PACKAGE, LOCALEDIR);
        set_domain = 0;
    }

    priv->can_exit = FALSE;
    priv->cal = gtk_calendar_new();

    /* first day of the week is obtained from the PO file */
    week_start = _("week_start");
    if (week_start[0] >= '0' && week_start[0] <= '6' && week_start[1] == 0)
      g_object_set(G_OBJECT(priv->cal),
                   "week-start", week_start[0] - '0', NULL);

    gtk_calendar_set_display_options(GTK_CALENDAR(priv->cal),
                                     GTK_CALENDAR_SHOW_HEADING |
                                     GTK_CALENDAR_SHOW_DAY_NAMES |
                                     GTK_CALENDAR_SHOW_WEEK_NUMBERS);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(cal)->vbox), priv->cal,
                       TRUE, TRUE, 0);
    gtk_dialog_set_has_separator(GTK_DIALOG(cal), FALSE);
    gtk_dialog_add_button(GTK_DIALOG(cal), _("Ecdg_bd_calendar_popout_done"),
                            GTK_RESPONSE_OK);
    gtk_widget_show(priv->cal);

    /* Connect signals */
    g_signal_connect(G_OBJECT(priv->cal), "day-selected",
                     G_CALLBACK(hildon_calendar_day_selected), cal);

    g_signal_connect(G_OBJECT(priv->cal), "key-press-event",
                     G_CALLBACK(hildon_key_pressed), cal);
    g_signal_connect_swapped(G_OBJECT(priv->cal), "button-release-event",
                             G_CALLBACK(hildon_calendar_deny_exit), cal);
    g_signal_connect_swapped(G_OBJECT(priv->cal), "month-changed",
                             G_CALLBACK(hildon_calendar_deny_exit), cal);
    g_signal_connect_swapped(G_OBJECT(priv->cal), "button-press-event",
                             G_CALLBACK(hildon_calendar_deny_exit), cal);

    g_signal_connect(G_OBJECT(priv->cal), "selected_date",
                     G_CALLBACK(hildon_calendar_allow_exit), cal);

    gtk_widget_realize(GTK_WIDGET(cal));
    gdk_window_set_decorations(GTK_WIDGET(cal)->window, GDK_DECOR_BORDER);
    gtk_widget_grab_focus(priv->cal);
}

static gboolean
hildon_calendar_day_selected(GtkWidget * widget, gpointer data)
{
    HildonCalendarPopup *cal;
    HildonCalendarPopupPrivate *priv;

    cal = HILDON_CALENDAR_POPUP(data);
    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    if (priv->can_exit) {
        gtk_dialog_response(GTK_DIALOG(data), GTK_RESPONSE_ACCEPT);
        return TRUE;
    }
    return FALSE;
}

static gboolean
hildon_key_pressed(GtkWidget * widget, GdkEventKey * event, gpointer data)
{
    HildonCalendarPopup *cal;
    HildonCalendarPopupPrivate *priv;

    g_return_val_if_fail(data, FALSE);

    cal = HILDON_CALENDAR_POPUP(data);
    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    if (event->keyval == GDK_Return) {
        priv->can_exit = TRUE;
        gtk_dialog_response(GTK_DIALOG(cal), GTK_RESPONSE_OK);
        return TRUE;
    }

    if ((event->keyval == GDK_Escape) || (event->keyval == GDK_F1) ||
        (event->keyval == GDK_F2) || (event->keyval == GDK_F3) ||
        (event->keyval == GDK_F4) || (event->keyval == GDK_F5) ||
        (event->keyval == GDK_F6) || (event->keyval == GDK_F7) ||
        (event->keyval == GDK_F8)) {
        gtk_dialog_response(GTK_DIALOG(cal), GTK_RESPONSE_CANCEL);
        return TRUE;
    }

    return FALSE;
}

static void
init_dmy(guint year, guint month, guint day, guint * d, guint * m,
         guint * y)
{
    GDate date;

    if (year > MAX_YEAR) {
        *d = 31;
        *m = 12;
        *y = MAX_YEAR;
    } else if (year < MIN_YEAR) {
        *d = 1;
        *m = 1;
        *y = MIN_YEAR;
    } else if (!g_date_valid_dmy(day, month, year)) {
        g_date_clear(&date, 1);
        g_date_set_time(&date, time(NULL));

        *d = g_date_get_day(&date);
        *m = g_date_get_month(&date);
        *y = g_date_get_year(&date);
    } else {
        *d = day;
        *m = month;
        *y = year;
    }
}

static void
hildon_calendar_allow_exit(GtkWidget * self, gpointer data)
{
    HildonCalendarPopup *cal;
    HildonCalendarPopupPrivate *priv;

    g_return_if_fail (GTK_IS_WIDGET (self));
    g_return_if_fail (HILDON_IS_CALENDAR_POPUP (data)); 
    
    cal = HILDON_CALENDAR_POPUP(data);
    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);
  
    gtk_dialog_response(GTK_DIALOG(cal), GTK_RESPONSE_OK);
}

static gboolean hildon_calendar_deny_exit(GtkWidget * self)
{
    HildonCalendarPopup *cal;
    HildonCalendarPopupPrivate *priv;

    cal = HILDON_CALENDAR_POPUP(self);
    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    priv->can_exit = FALSE;
    return FALSE;
}
