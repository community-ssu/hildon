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

/**
 * SECTION:hildon-date-selector
 * @short_description: A widget to select the current date.
 *
 * HildonDateSelector is a date widget, equivalent to hildon-calendar, but with a multi-column
 * approach
 *
 */


#include "hildon-date-selector.h"

#define _GNU_SOURCE     /* needed for GNU nl_langinfo_l */
#define __USE_GNU       /* needed for locale */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include <string.h>
#include <stdlib.h>

#include <libintl.h>
#include <time.h>
#include <langinfo.h>
#include <locale.h>

#define HILDON_DATE_SELECTOR_GET_PRIVATE(obj)                           \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_DATE_SELECTOR, HildonDateSelectorPrivate))

G_DEFINE_TYPE (HildonDateSelector, hildon_date_selector, HILDON_TYPE_TOUCH_SELECTOR)

#define INIT_YEAR 100
#define LAST_YEAR 50    /* since current year */

#define _(String) dgettext("hildon-libs", String)

/* #define _(String) "%A %e. %B %Y"  debug purposes */

enum
{
  COLUMN_STRING,
  COLUMN_INT,
  N_COLUMNS
};

enum
{
  DAY,
  MONTH,
  YEAR
};

struct _HildonDateSelectorPrivate
{
  GtkTreeModel *year_model;
  GtkTreeModel *month_model;
  GtkTreeModel *day_model;

  GSList *column_order;
  gint day_column;
  gint month_column;
  gint year_column;             /* it depends on the locale */

  gchar *format;                /* day/month/year format, depends on locale */

  gint creation_day;
  gint creation_month;
  gint creation_year;           /* date at creation time */

  gchar *monthname[12];
};

static void hildon_date_selector_finalize (GObject * object);

/* private functions */
static GtkTreeModel *_create_day_model (HildonDateSelector * selector);
static GtkTreeModel *_create_year_model (HildonDateSelector * selector);
static GtkTreeModel *_create_month_model (HildonDateSelector * selector);

static void _get_real_date (gint * year, gint * month, gint * day);
static void _locales_init (HildonDateSelectorPrivate * priv);

static void _manage_selector_change_cb (HildonTouchSelector * selector,
                                        gint num_column, gpointer data);

static GtkTreeModel *_update_day_model (HildonDateSelector * selector);

static gint _month_days (gint month, gint year);
static void _init_column_order (HildonDateSelector * selector);

static gchar *_custom_print_func (HildonTouchSelector * selector);

/***************************************************************************/
/* The following date routines are taken from the lib_date package.  Keep
 * them separate in case we want to update them if a newer lib_date comes
 * out with fixes.  */

typedef unsigned int N_int;

typedef unsigned long N_long;

typedef signed long Z_long;

typedef enum
{ false = FALSE, true = TRUE } boolean;

#define                                         and &&  /* logical (boolean) operators: lower case */

#define                                         or ||

static const N_int month_length[2][13] = {
  {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static const N_int days_in_months[2][14] = {
  {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
  {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366}
};

static Z_long _calc_days (N_int year, N_int mm, N_int dd);

static N_int _day_of_week (N_int year, N_int mm, N_int dd);

static boolean _leap (N_int year);


static boolean
_leap (N_int year)
{
  return ((((year % 4) == 0) and ((year % 100) != 0)) or ((year % 400) == 0));
}

static N_int
_day_of_week (N_int year, N_int mm, N_int dd)
{
  Z_long days;

  days = _calc_days (year, mm, dd);
  if (days > 0L) {
    days--;
    days %= 7L;
    days++;
  }
  return ((N_int) days);
}

static Z_long
_year_to_days (N_int year)
{
  return (year * 365L + (year / 4) - (year / 100) + (year / 400));
}

static Z_long
_calc_days (N_int year, N_int mm, N_int dd)
{
  boolean lp;

  if (year < 1)
    return (0L);
  if ((mm < 1) or (mm > 12))
    return (0L);
  if ((dd < 1) or (dd > month_length[(lp = _leap (year))][mm]))
    return (0L);
  return (_year_to_days (--year) + days_in_months[lp][mm] + dd);
}

static void
hildon_date_selector_class_init (HildonDateSelectorClass * class)
{
  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  gobject_class = (GObjectClass *) class;
  object_class = (GtkObjectClass *) class;
  widget_class = (GtkWidgetClass *) class;
  container_class = (GtkContainerClass *) class;

  /* GObject */
  gobject_class->finalize = hildon_date_selector_finalize;

  /* GtkWidget */

  /* GtkContainer */

  /* signals */

  g_type_class_add_private (object_class, sizeof (HildonDateSelectorPrivate));
}

static void
hildon_date_selector_init (HildonDateSelector * selector)
{
  GSList *iter = NULL;
  gint current_item = 0;

  selector->priv = HILDON_DATE_SELECTOR_GET_PRIVATE (selector);

  GTK_WIDGET_SET_FLAGS (GTK_WIDGET (selector), GTK_NO_WINDOW);
  gtk_widget_set_redraw_on_allocate (GTK_WIDGET (selector), FALSE);

  hildon_touch_selector_set_print_func (HILDON_TOUCH_SELECTOR (selector),
                                        _custom_print_func);

  _locales_init (selector->priv);

  _init_column_order (selector);

  _get_real_date (&selector->priv->creation_year,
                  &selector->priv->creation_month, &selector->priv->creation_day);

  selector->priv->year_model = _create_year_model (selector);
  selector->priv->month_model = _create_month_model (selector);
  selector->priv->day_model = _create_day_model (selector);

  /* We add the columns: FIXME: check the locale order */
  iter = selector->priv->column_order;
  for (iter = selector->priv->column_order; iter; iter = g_slist_next (iter)) {
    current_item = GPOINTER_TO_INT (iter->data);

    switch (current_item) {
    case DAY:
      hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                                selector->priv->day_model);
      break;
    case MONTH:
      hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                                selector->priv->month_model);
      break;
    case YEAR:
      hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                                selector->priv->year_model);
      break;
    default:
      g_error ("Current column order incorrect");
      break;
    }
  }

  g_signal_connect (G_OBJECT (selector),
                    "changed", G_CALLBACK (_manage_selector_change_cb), NULL);

  /* By default we should select the current day */
  hildon_date_selector_select_current_date (selector, selector->priv->creation_year,
                                            selector->priv->creation_month,
                                            selector->priv->creation_day);
}

static void
hildon_date_selector_finalize (GObject * object)
{
  HildonDateSelector *selector = NULL;
  gint i = 0;

  selector = HILDON_DATE_SELECTOR (object);

  for (i = 0; i < 12; i++) {
    g_free (selector->priv->monthname[i]);
  }

  g_slist_free (selector->priv->column_order);

  g_free (selector->priv);

  (*G_OBJECT_CLASS (hildon_date_selector_parent_class)->finalize) (object);
}

/* ------------------------------ PRIVATE METHODS ---------------------------- */
static gchar *
_custom_print_func (HildonTouchSelector * touch_selector)
{
  HildonDateSelector *selector = NULL;
  gchar *result = NULL;
  guint year, month, day;
  gint day_of_week = 0;
  static gchar string[255];
  struct tm tm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

  selector = HILDON_DATE_SELECTOR (touch_selector);

  hildon_date_selector_get_date (selector, &year, &month, &day);
  day_of_week = _day_of_week (year, month + 1, day) % 7;

  tm.tm_mday = day;
  tm.tm_mon = month;
  tm.tm_year = year - 1900;
  tm.tm_wday = day_of_week;

  strftime (string, 255, _("wdgt_va_date_long"), &tm);

  result = g_strdup (string);

  return result;
}

/* This was copied from hildon-calendar */
static void
_locales_init (HildonDateSelectorPrivate * priv)
{
  /* Hildon: This is not exactly portable, see
   * http://bugzilla.gnome.org/show_bug.cgi?id=343415
   * The labels need to be instance variables as the startup wizard changes
   * locale on runtime.
   */
  locale_t l;

  l = newlocale (LC_TIME_MASK, setlocale (LC_MESSAGES, NULL), NULL);

  priv->monthname[0] = g_locale_to_utf8 (nl_langinfo_l (MON_1, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[1] = g_locale_to_utf8 (nl_langinfo_l (MON_2, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[2] = g_locale_to_utf8 (nl_langinfo_l (MON_3, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[3] = g_locale_to_utf8 (nl_langinfo_l (MON_4, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[4] = g_locale_to_utf8 (nl_langinfo_l (MON_5, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[5] = g_locale_to_utf8 (nl_langinfo_l (MON_6, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[6] = g_locale_to_utf8 (nl_langinfo_l (MON_7, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[7] = g_locale_to_utf8 (nl_langinfo_l (MON_8, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[8] = g_locale_to_utf8 (nl_langinfo_l (MON_9, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[9] = g_locale_to_utf8 (nl_langinfo_l (MON_10, l),
                                         -1, NULL, NULL, NULL);
  priv->monthname[10] = g_locale_to_utf8 (nl_langinfo_l (MON_11, l),
                                          -1, NULL, NULL, NULL);
  priv->monthname[11] = g_locale_to_utf8 (nl_langinfo_l (MON_12, l),
                                          -1, NULL, NULL, NULL);

  priv->format = g_locale_to_utf8 (nl_langinfo_l (D_FMT, l),
                                   -1, NULL, NULL, NULL);

  freelocale (l);
}

static void
_init_column_order (HildonDateSelector * selector)
{
  gchar *current_order[3] = { NULL, NULL, NULL };
  gchar *day_pos = NULL;
  gchar *month_pos = NULL;
  gchar *year_pos = NULL;
  gint i, c;
  gchar *aux = NULL;

  g_debug ("Current format: %s", selector->priv->format);

  /* search each token on the format */
  day_pos = g_strrstr (selector->priv->format, "%d");

  month_pos = g_strrstr (selector->priv->format, "%m");
  year_pos = g_strrstr (selector->priv->format, "%y");
  if (year_pos == NULL) {
    year_pos = g_strrstr (selector->priv->format, "%Y");
  }


  if ((day_pos == NULL) || (month_pos == NULL) || (year_pos == NULL)) {
    g_error ("Wrong date format");      /* so default values */

    selector->priv->day_column = 0;
    selector->priv->month_column = 1;
    selector->priv->year_column = 2;
    selector->priv->column_order = g_slist_append (NULL, GINT_TO_POINTER (DAY));
    selector->priv->column_order =
      g_slist_append (selector->priv->column_order, GINT_TO_POINTER (MONTH));
    selector->priv->column_order =
      g_slist_append (selector->priv->column_order, GINT_TO_POINTER (YEAR));
  }

  /* sort current_order with this values (bubble sort) */
  current_order[0] = day_pos;
  current_order[1] = month_pos;
  current_order[2] = year_pos;

  for (c = 1; c <= 2; c++) {
    for (i = 0; i < 3 - c; i++) {
      if (current_order[i] > current_order[i + 1]) {
        aux = current_order[i];
        current_order[i] = current_order[i + 1];
        current_order[i + 1] = aux;
      }
    }
  }

  /* fill the column positions */
  selector->priv->column_order = NULL;
  c = 0;
  for (i = 0; i < 3; i++) {
    if (current_order[i] == day_pos) {
      selector->priv->column_order =
        g_slist_append (selector->priv->column_order, GINT_TO_POINTER (DAY));
      selector->priv->day_column = c++;
    }
    if (current_order[i] == month_pos) {
      selector->priv->column_order =
        g_slist_append (selector->priv->column_order, GINT_TO_POINTER (MONTH));
      selector->priv->month_column = c++;
    }
    if (current_order[i] == year_pos) {
      selector->priv->column_order =
        g_slist_append (selector->priv->column_order, GINT_TO_POINTER (YEAR));
      selector->priv->year_column = c++;
    }
  }
}


static GtkTreeModel *
_create_day_model (HildonDateSelector * selector)
{
  GtkListStore *store_days = NULL;
  gint i = 0;
  gchar *label = NULL;
  GtkTreeIter iter;

  store_days = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
  for (i = 1; i < 32; i++) {
    label = g_strdup_printf ("%d", i);

    gtk_list_store_append (store_days, &iter);
    gtk_list_store_set (store_days, &iter,
                        COLUMN_STRING, label, COLUMN_INT, i, -1);
    g_free (label);
  }

  return GTK_TREE_MODEL (store_days);
}

static GtkTreeModel *
_create_year_model (HildonDateSelector * selector)
{
  GtkListStore *store_years = NULL;
  gint real_year = 0;
  gint i = 0;
  gchar *label = NULL;
  GtkTreeIter iter;

  real_year = selector->priv->creation_year;

  store_years = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
  for (i = real_year - INIT_YEAR; i < real_year + LAST_YEAR; i++) {
    label = g_strdup_printf ("%d", i);

    gtk_list_store_append (store_years, &iter);
    gtk_list_store_set (store_years, &iter,
                        COLUMN_STRING, label, COLUMN_INT, i, -1);
    g_free (label);
  }

  return GTK_TREE_MODEL (store_years);
}

static GtkTreeModel *
_create_month_model (HildonDateSelector * selector)
{
  GtkTreeIter iter;
  gint i = 0;
  GtkListStore *store_months = NULL;
  gchar *label = NULL;

  store_months = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
  for (i = 0; i < 12; i++) {
    label = g_strdup_printf ("%s", selector->priv->monthname[i]);

    gtk_list_store_append (store_months, &iter);
    gtk_list_store_set (store_months, &iter, COLUMN_STRING, label,      /* the label with the month */
                        COLUMN_INT, i,  /* the month number */
                        -1);
    g_free (label);
  }

  return GTK_TREE_MODEL (store_months);
}

static GtkTreeModel *
_update_day_model (HildonDateSelector * selector)
{
  GtkListStore *store_days = NULL;
  gint i = 0;
  GtkTreeIter iter;
  gchar *label = NULL;
  guint current_day = 0;
  guint current_year = 0;
  guint current_month = 0;
  guint num_days = 31;

  hildon_date_selector_get_date (selector, NULL, NULL, &current_day);

  hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         selector->priv->month_column, &iter);
  gtk_tree_model_get (selector->priv->month_model,
                      &iter, COLUMN_INT, &current_month, -1);

  hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         selector->priv->year_column, &iter);
  gtk_tree_model_get (selector->priv->year_model,
                      &iter, COLUMN_INT, &current_year, -1);

  num_days = _month_days (current_month, current_year);

  store_days = GTK_LIST_STORE (selector->priv->day_model);
  gtk_list_store_clear (store_days);

  for (i = 1; i <= num_days; i++) {
    label = g_strdup_printf ("%d", i);

    gtk_list_store_append (store_days, &iter);
    gtk_list_store_set (store_days, &iter,
                        COLUMN_STRING, label, COLUMN_INT, i, -1);
    g_free (label);
  }

  /* now we select a day */
  if (current_day >= num_days) {
    current_day = num_days;
  }

  hildon_date_selector_select_day (selector, current_day);

  return GTK_TREE_MODEL (store_days);
}


static void
_get_real_date (gint * year, gint * month, gint * day)
{
  time_t secs;
  struct tm *tm = NULL;

  secs = time (NULL);
  tm = localtime (&secs);

  if (year != NULL) {
    *year = 1900 + tm->tm_year;
  }

  if (month != NULL) {
    *month = tm->tm_mon;
  }

  if (day != NULL) {
    *day = tm->tm_mday;
  }
}


static void
_manage_selector_change_cb (HildonTouchSelector * touch_selector,
                            gint num_column, gpointer data)
{
  HildonDateSelector *selector = NULL;

  g_return_if_fail (HILDON_IS_DATE_SELECTOR (touch_selector));
  selector = HILDON_DATE_SELECTOR (touch_selector);

  if ((num_column == selector->priv->month_column) ||
      (num_column == selector->priv->year_column)) /* it is required to check that with
                                                    * the years too,remember: leap years
                                                    */
  {
    _update_day_model (selector);
  }
}

static gint
_month_days (gint month, gint year)
{
  gint month_days[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
  };

  g_return_val_if_fail (month >= 0 && month <= 12, -1);

  return month_days[_leap (year)][month];
}


/* ------------------------------ PUBLIC METHODS ---------------------------- */

/**
 * hildon_date_selector_new:
 * @:
 *
 * Creates a new #HildonDateSelector
 *
 * Returns: a new #HildonDateSelector
 **/
GtkWidget *
hildon_date_selector_new ()
{
  return g_object_new (HILDON_TYPE_DATE_SELECTOR, NULL);
}



/**
 * hildon_date_selector_select_date:
 * @selector: the #HildonDateSelector
 * @year:  the current year
 * @month: the current month (0-11)
 * @day:   the current day (1-31, 1-30, 1-29, 1-28) depends on the month
 *
 * Sets the current active date on the #HildonDateSelector widget
 *
 **/
gboolean
hildon_date_selector_select_current_date (HildonDateSelector * selector,
                                          guint year, guint month, guint day)
{
  GtkTreeIter iter;
  gint min_year = 0;
  gint max_year = 0;
  gint num_days = 0;

  min_year = selector->priv->creation_year - INIT_YEAR;
  max_year = selector->priv->creation_year + LAST_YEAR;

  g_return_val_if_fail (year > min_year && year < max_year, FALSE);
  g_return_val_if_fail (month >= 0 && month < 12, FALSE);

  num_days = _month_days (month, year);
  g_return_val_if_fail (day > 0 && day < num_days, FALSE);


  gtk_tree_model_iter_nth_child (selector->priv->year_model, &iter, NULL,
                                 year - selector->priv->creation_year +
                                 INIT_YEAR);
  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         selector->priv->year_column, &iter,
                                         FALSE);

  gtk_tree_model_iter_nth_child (selector->priv->month_model, &iter, NULL,
                                 month);
  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         selector->priv->month_column, &iter,
                                         FALSE);

  gtk_tree_model_iter_nth_child (selector->priv->day_model, &iter, NULL,
                                 day - 1);
  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         selector->priv->day_column, &iter,
                                         FALSE);

  return TRUE;
}


/**
 * hildon_date_selector_select_date:
 * @selector: the #HildonDateSelector
 * @year:  to set the current year
 * @month: to set the current month (0-11)
 * @day:   to the current day (1-31, 1-30, 1-29, 1-28) depends on the month
 *
 * Gets the current active date on the #HildonDateSelector widget
 *
 *
 **/
void
hildon_date_selector_get_date (HildonDateSelector * selector,
                               guint * year, guint * month, guint * day)
{
  GtkTreeIter iter;

  if (year != NULL) {
    hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                           selector->priv->year_column, &iter);
    gtk_tree_model_get (selector->priv->year_model,
                        &iter, COLUMN_INT, year, -1);
  }

  if (month != NULL) {
    hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                           selector->priv->month_column, &iter);
    gtk_tree_model_get (selector->priv->month_model,
                        &iter, COLUMN_INT, month, -1);
  }


  if (day != NULL) {
    if (hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                               selector->priv->day_column, &iter))
    {
      gtk_tree_model_get (selector->priv->day_model,
                          &iter, COLUMN_INT, day, -1);
    }
/*       *day = *day - 1;  */
  }

}


void
hildon_date_selector_select_day (HildonDateSelector * selector, guint day)
{
  GtkTreeIter iter;

  gtk_tree_model_iter_nth_child (selector->priv->day_model, &iter, NULL,
                                 day - 1);
  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         selector->priv->day_column, &iter,
                                         FALSE);
}
