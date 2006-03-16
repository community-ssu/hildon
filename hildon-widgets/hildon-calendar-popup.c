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

/**
 * SECTION:hildon-calendar-popup
 * @shortdesc: CalendarPopup allows choosing a date from a popup calendar.
 * @longdesc: The Calendar popup is a dialog that contains a GtkCalendar 
 * widget. The pop-up is cancelled by pressing the ESC key.
 * </para><para>
 * 
 * @seealso: #HildonDateEditor, #HildonTimeEditor
 * 
 * HildonCalendarPopup is a dialog which contains a GtkCalendar.  It
 * also contains arrow buttons for changing the month/year. If an
 * entered date is invalid, an information message will be shown.
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

static void hildon_calendar_selected_date(GtkWidget * self, gpointer cal_popup);

static gboolean hildon_key_pressed(GtkWidget * widget, GdkEventKey * event,
                                   gpointer cal_popup);

static void hildon_calendar_popup_set_property(GObject * object, guint property_id,
                                    const GValue * value, GParamSpec * pspec);
static void hildon_calendar_popup_get_property(GObject * object, guint property_id,
                                    GValue * value, GParamSpec * pspec);

enum {
  PROP_0,
  PROP_DAY,
  PROP_MONTH,
  PROP_YEAR,
  PROP_MIN_YEAR,
  PROP_MAX_YEAR
};

struct _HildonCalendarPopupPrivate {
    GtkWidget *cal;
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
 * Returns: new @HildonCalendarPopup widget
 */
GtkWidget *hildon_calendar_popup_new(GtkWindow * parent, guint year,
                                     guint month, guint day)
{
    HildonCalendarPopup *cal = NULL;
    HildonCalendarPopupPrivate *priv;
    guint dtmp, mtmp, ytmp;

    init_dmy(year, month, day, &dtmp, &mtmp, &ytmp);

    /* Create new HildonCalendarPopup */
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
 * activates a new date on the calendar popup.
 **/
void
hildon_calendar_popup_set_date(HildonCalendarPopup * cal,
                               guint year, guint month, guint day)
{
    HildonCalendarPopupPrivate *priv;

    g_return_if_fail(HILDON_IS_CALENDAR_POPUP(cal));

    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    /* Remove all visual markers */
    gtk_calendar_clear_marks(GTK_CALENDAR(priv->cal));

    /* Set a new date */
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
 * Gets the currently selected year, month, and day. 
 */
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
    GObjectClass *object_class = G_OBJECT_CLASS(cal_class);
    parent_class = g_type_class_peek_parent(cal_class);
    
    object_class->set_property = hildon_calendar_popup_set_property;
    object_class->get_property = hildon_calendar_popup_get_property;

    g_type_class_add_private(cal_class,
                             sizeof(HildonCalendarPopupPrivate));

    /* Install new properties for the GObject_class */
    g_object_class_install_property(object_class, PROP_MIN_YEAR,
                                    g_param_spec_uint("min-year",
                                                      "Minimum valid year",
                                                      "Minimum valid year",
                                                      1, 2100,
                                                      1970,
                                                      G_PARAM_WRITABLE));

    g_object_class_install_property(object_class, PROP_MAX_YEAR,
                                    g_param_spec_uint("max-year",
                                                      "Maximum valid year",
                                                      "Maximum valid year",
                                                      1, 2100,
                                                      2037,
                                                      G_PARAM_WRITABLE));

    g_object_class_install_property(object_class, PROP_DAY,
				    g_param_spec_int ("day",
						      "Day",
						      "currently selected day",
						      G_MININT,
						      G_MAXINT,
						      0,
						      G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_MONTH,
				    g_param_spec_int ("month",
						      "Month",
						      "currently selected month",
						      G_MININT,
						      G_MAXINT,
						      0,
						      G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_YEAR,
				    g_param_spec_int ("year",
						      "Year",
						      "the currently selected year",
						      G_MININT,
						      G_MAXINT,
						      0,
						      G_PARAM_READWRITE));

}

static void hildon_calendar_popup_init(HildonCalendarPopup * cal)
{
    HildonCalendarPopupPrivate *priv;
    static int set_domain = 1;

    priv = HILDON_CALENDAR_POPUP_GET_PRIVATE(cal);

    /* set the domain directory for different language */
    if (set_domain) {
        (void) bindtextdomain(PACKAGE, LOCALEDIR);
        set_domain = 0;
    }

    priv->cal = gtk_calendar_new();

    /* dialog options and packing */
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
    g_signal_connect(G_OBJECT(priv->cal), "key-press-event",
                     G_CALLBACK(hildon_key_pressed), cal);

    g_signal_connect(G_OBJECT(priv->cal), "selected_date",
                     G_CALLBACK(hildon_calendar_selected_date), cal);

    /* set decorations, needs realizing first*/
    gtk_widget_realize(GTK_WIDGET(cal));
    gdk_window_set_decorations(GTK_WIDGET(cal)->window, GDK_DECOR_BORDER);

}


/*
 * Signal handler for key-press-event. Closes the dialog for some
 * special keys.
 */
static gboolean
hildon_key_pressed(GtkWidget * widget, GdkEventKey * event, gpointer cal_popup)
{
    g_assert(HILDON_IS_CALENDAR_POPUP(cal_popup));

    /* Handle Return key press as OK response */
    if (event->keyval == GDK_Return)
    {
        gtk_dialog_response(GTK_DIALOG(cal_popup), GTK_RESPONSE_OK);
        return TRUE;
    }

    /* Handle Esc key press as CANCEL response */
    if ((event->keyval == GDK_Escape))
    {
        gtk_dialog_response(GTK_DIALOG(cal_popup), GTK_RESPONSE_CANCEL);
        return TRUE;
    }

    return FALSE;
}


/*
 * Validates the given date or initializes it with the current date
 */
static void
init_dmy(guint year, guint month, guint day, guint *d, guint *m, guint *y)
{
    GDate date;

    /* Initialize the date with a valid selected date */ 
    if (g_date_valid_dmy(day, month, year)) {
        *d = day;
        *m = month;
        *y = year;
    } else { 

      /* If selected date is invalid initialize the date with current date */ 
        g_date_clear(&date, 1);
        g_date_set_time(&date, time(NULL));

        *d = g_date_get_day(&date);
        *m = g_date_get_month(&date);
        *y = g_date_get_year(&date);
    }
}

/*
 * Exits the dialog when "selected_date" signal is emmited. The
 * "selected_date" signal is a Hildon addition to GtkCalendar and is
 * emitted on button-release.
 */
static void
hildon_calendar_selected_date(GtkWidget * self, gpointer cal_popup)
{
    g_assert(GTK_IS_WIDGET (self));
    g_assert(HILDON_IS_CALENDAR_POPUP (cal_popup));
 
    gtk_dialog_response(GTK_DIALOG(cal_popup), GTK_RESPONSE_OK);
}


static void hildon_calendar_popup_set_property(GObject * object, guint property_id,
                                    const GValue * value, GParamSpec * pspec)
{
    HildonCalendarPopupPrivate *priv = 
	HILDON_CALENDAR_POPUP_GET_PRIVATE(HILDON_CALENDAR_POPUP (object));

    switch (property_id) {
    case PROP_DAY:
        g_object_set_property(G_OBJECT(priv->cal), pspec->name, value);
        break;
    case PROP_MONTH:
        g_object_set_property(G_OBJECT(priv->cal), pspec->name, value);
        break;
    case PROP_YEAR:
        g_object_set_property(G_OBJECT(priv->cal), pspec->name, value);
        break;
    case PROP_MIN_YEAR:
        g_object_set_property(G_OBJECT(priv->cal), "min-year", value);
        break;
    case PROP_MAX_YEAR:
        g_object_set_property(G_OBJECT(priv->cal), "max-year", value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void hildon_calendar_popup_get_property(GObject * object, guint property_id,
                                    GValue * value, GParamSpec * pspec)
{
    HildonCalendarPopupPrivate *priv = 
	HILDON_CALENDAR_POPUP_GET_PRIVATE(HILDON_CALENDAR_POPUP (object));

    switch (property_id) {
    case PROP_DAY:
        g_object_get_property(G_OBJECT(priv->cal), pspec->name, value);
        break;
    case PROP_MONTH:
        g_object_get_property(G_OBJECT(priv->cal), pspec->name, value);
        break;
    case PROP_YEAR:
        g_object_get_property(G_OBJECT(priv->cal), pspec->name, value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
