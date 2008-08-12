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
 * SECTION:hildon-time-selector
 * @short_description: A widget to select the current time.
 *
 * HildonTimeSelector is a time widget, equivalent to hildon-calendar, but with a multi-column
 * approach
 *
 */

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
#include <gconf/gconf-client.h>

#include "hildon-time-selector.h"

#define HILDON_TIME_SELECTOR_GET_PRIVATE(obj)                           \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_TIME_SELECTOR, HildonTimeSelectorPrivate))

G_DEFINE_TYPE (HildonTimeSelector, hildon_time_selector, HILDON_TYPE_TOUCH_SELECTOR)

#define INIT_YEAR 100
#define LAST_YEAR 50    /* since current year */

#define _(String)  dgettext("hildon-libs", String)

/* #define _(String) "%I:%M %p" debug purposes */


/* FIXME: we should get this two props from the clock ui headers */
#define CLOCK_GCONF_PATH "/apps/clock"
#define CLOCK_GCONF_IS_24H_FORMAT CLOCK_GCONF_PATH  "/time-format"

enum {
  COLUMN_STRING,
  COLUMN_INT,
  TOTAL_MODEL_COLUMNS
};

enum {
  COLUMN_HOURS = 0,
  COLUMN_MINUTES,
  COLUMN_AMPM,
  TOTAL_TIME_COLUMNS
};

struct _HildonTimeSelectorPrivate
{
  GtkTreeModel *hours_model;
  GtkTreeModel *minutes_model;
  GtkTreeModel *ampm_model;

  gboolean ampm_format;         /* if using am/pm format or 24 h one */

  gboolean pm;                  /* if we are on pm (only useful if ampm_format == TRUE) */

  gint creation_hours;
  gint creation_minutes;
};

static void hildon_time_selector_finalize (GObject * object);

/* private functions */
static GtkTreeModel *_create_hours_model (HildonTimeSelector * selector);
static GtkTreeModel *_create_minutes_model (HildonTimeSelector * selector);
static GtkTreeModel *_create_ampm_model (HildonTimeSelector * selector);

static void _get_real_time (gint * hours, gint * minutes);
static void _manage_ampm_selection_cb (HildonTouchSelector * selector,
                                       gint num_column, gpointer data);
static void _check_ampm_format (HildonTimeSelector * selector);
static void _set_pm (HildonTimeSelector * selector, gboolean pm);

static gchar *_custom_print_func (HildonTouchSelector * selector);

static void
hildon_time_selector_class_init (HildonTimeSelectorClass * class)
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
  gobject_class->finalize = hildon_time_selector_finalize;

  /* GtkWidget */

  /* GtkContainer */

  /* signals */

  g_type_class_add_private (object_class, sizeof (HildonTimeSelectorPrivate));
}


static void
hildon_time_selector_init (HildonTimeSelector * selector)
{
  selector->priv = HILDON_TIME_SELECTOR_GET_PRIVATE (selector);

  GTK_WIDGET_SET_FLAGS (GTK_WIDGET (selector), GTK_NO_WINDOW);
  gtk_widget_set_redraw_on_allocate (GTK_WIDGET (selector), FALSE);

  hildon_touch_selector_set_print_func (HILDON_TOUCH_SELECTOR (selector),
                                        _custom_print_func);

  _get_real_time (&selector->priv->creation_hours,
                  &selector->priv->creation_minutes);

  _check_ampm_format (selector);

  selector->priv->hours_model = _create_hours_model (selector);
  selector->priv->minutes_model = _create_minutes_model (selector);

  hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                            selector->priv->hours_model, TRUE);

  hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                            selector->priv->minutes_model, TRUE);

  if (selector->priv->ampm_format) {
    selector->priv->ampm_model = _create_ampm_model (selector);

    hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                              selector->priv->ampm_model, TRUE);

    g_signal_connect (G_OBJECT (selector),
                      "changed", G_CALLBACK (_manage_ampm_selection_cb),
                      NULL);
  }

  /* By default we should select the current day */
  hildon_time_selector_set_time (selector,
                                 selector->priv->creation_hours,
                                 selector->priv->creation_minutes);

}

static void
hildon_time_selector_finalize (GObject * object)
{
  HildonTimeSelector *selector = NULL;

  selector = HILDON_TIME_SELECTOR (object);

  /* FIXME: FILL ME !! */

  (*G_OBJECT_CLASS (hildon_time_selector_parent_class)->finalize) (object);
}

/* ------------------------------ PRIVATE METHODS ---------------------------- */

static gchar *
_custom_print_func (HildonTouchSelector * touch_selector)
{
  gchar *result = NULL;
  struct tm tm = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  HildonTimeSelector *selector = NULL;
  static gchar string[255];
  guint hours = 0;
  guint minutes = 0;

  selector = HILDON_TIME_SELECTOR (touch_selector);

  hildon_time_selector_get_time (selector, &hours, &minutes);

  tm.tm_min = minutes;
  tm.tm_hour = hours;

  if (selector->priv->ampm_format) {
    strftime (string, 255, _("wdgt_va_12h_hours"), &tm);
  } else {
    strftime (string, 255, _("wdgt_va_24h_hours"), &tm);
  }


  result = g_strdup (string);

  return result;
}

static GtkTreeModel *
_create_minutes_model (HildonTimeSelector * selector)
{
  GtkListStore *store_minutes = NULL;
  gint i = 0;
  gchar *label = NULL;
  GtkTreeIter iter;

  store_minutes = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
  for (i = 0; i <= 59; i++) {
    label = g_strdup_printf ("%02d", i);

    gtk_list_store_append (store_minutes, &iter);
    gtk_list_store_set (store_minutes, &iter,
                        COLUMN_STRING, label, COLUMN_INT, i, -1);
    g_free (label);
  }

  return GTK_TREE_MODEL (store_minutes);
}

static GtkTreeModel *
_create_hours_model (HildonTimeSelector * selector)
{
  GtkListStore *store_hours = NULL;
  gint i = 0;
  gchar *label = NULL;
  GtkTreeIter iter;
  gint last_hour = 12;
  gint addition = 0;

  if (selector->priv->ampm_format) {
    last_hour = 12;
    addition = 1;
  } else {
    last_hour = 24;
    addition = 0;
  }

  store_hours = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
  for (i = 0; i < last_hour; i++) {
    label = g_strdup_printf ("%02d", i + addition);

    gtk_list_store_append (store_hours, &iter);
    gtk_list_store_set (store_hours, &iter,
                        COLUMN_STRING, label, COLUMN_INT, i, -1);
    g_free (label);
  }

  return GTK_TREE_MODEL (store_hours);
}

static GtkTreeModel *
_create_ampm_model (HildonTimeSelector * selector)
{
  GtkListStore *store_ampm = NULL;
  GtkTreeIter iter;

  store_ampm = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);

  gtk_list_store_append (store_ampm, &iter);
  gtk_list_store_set (store_ampm, &iter, COLUMN_STRING, "am",   /* FIXME: use nl_langinfo am/pm strings */
                      COLUMN_INT, 0, -1);

  gtk_list_store_append (store_ampm, &iter);
  gtk_list_store_set (store_ampm, &iter,
                      COLUMN_STRING, "pm", COLUMN_INT, 1, -1);

  return GTK_TREE_MODEL (store_ampm);
}

static void
_get_real_time (gint * hours, gint * minutes)
{
  time_t secs;
  struct tm *tm = NULL;

  secs = time (NULL);
  tm = localtime (&secs);

  if (hours != NULL) {
    *hours = tm->tm_hour;
  }

  if (minutes != NULL) {
    *minutes = tm->tm_min;
  }
}

static void
_manage_ampm_selection_cb (HildonTouchSelector * touch_selector,
                           gint num_column, gpointer data)
{
  HildonTimeSelector *selector = NULL;
  gint pm;
  GtkTreeIter iter;

  g_return_if_fail (HILDON_IS_TIME_SELECTOR (touch_selector));
  selector = HILDON_TIME_SELECTOR (touch_selector);

  if (num_column == COLUMN_AMPM) {
    hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                           COLUMN_AMPM, &iter);
    gtk_tree_model_get (selector->priv->ampm_model, &iter, COLUMN_INT, &pm, -1);

    selector->priv->pm = pm;
  }
}

static void
_check_ampm_format (HildonTimeSelector * selector)
{
  GConfClient *client = NULL;
  gboolean value = FALSE;
  GError *error = NULL;

  client = gconf_client_get_default ();
  value = gconf_client_get_bool (client, CLOCK_GCONF_IS_24H_FORMAT, &error);
  if (error != NULL) {
    g_warning
      ("Error trying to get gconf variable %s, using 24h format by default",
       CLOCK_GCONF_IS_24H_FORMAT);
    g_error_free (error);
  }

  selector->priv->ampm_format = value;
  selector->priv->pm = TRUE;
}

static void
_set_pm (HildonTimeSelector * selector, gboolean pm)
{
  GtkTreeIter iter;

  selector->priv->pm = pm;

  gtk_tree_model_iter_nth_child (selector->priv->ampm_model, &iter, NULL, pm);

  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         COLUMN_AMPM, &iter, FALSE);
}

/* ------------------------------ PUBLIC METHODS ---------------------------- */

/**
 * hildon_time_selector_new:
 *
 * Creates a new #HildonTimeSelector
 *
 * Returns: a new #HildonTimeSelector
 **/
GtkWidget *
hildon_time_selector_new ()
{
  return g_object_new (HILDON_TYPE_TIME_SELECTOR, NULL);
}

/*
 * hours (0-23)
 * minutes (0-59)
 */
gboolean
hildon_time_selector_set_time (HildonTimeSelector * selector,
                               guint hours, guint minutes)
{
  GtkTreeIter iter;
  gint hours_item = 0;

  g_return_val_if_fail (hours >= 0 && hours <= 23, FALSE);
  g_return_val_if_fail (minutes >= 0 && minutes <= 59, FALSE);

  if (selector->priv->ampm_format) {
    if (hours > 12) {
      hours_item = hours - 13;
    } else {
      hours_item = hours - 1;
    }

    _set_pm (selector, hours >= 12);
  } else {
    hours_item = hours;
  }

  gtk_tree_model_iter_nth_child (selector->priv->hours_model, &iter, NULL,
                                 hours_item);
  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         COLUMN_HOURS, &iter, FALSE);

  gtk_tree_model_iter_nth_child (selector->priv->minutes_model, &iter, NULL,
                                 minutes);
  hildon_touch_selector_set_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                         COLUMN_MINUTES, &iter, FALSE);

  return TRUE;
}

void
hildon_time_selector_get_time (HildonTimeSelector * selector,
                               guint * hours, guint * minutes)
{
  GtkTreeIter iter;

  if (hours != NULL) {
    hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                           COLUMN_HOURS, &iter);
    gtk_tree_model_get (selector->priv->hours_model,
                        &iter, COLUMN_INT, hours, -1);
    if (selector->priv->ampm_format) {
      *hours += selector->priv->pm * 12;
    }
  }

  if (minutes != NULL) {
    hildon_touch_selector_get_active_iter (HILDON_TOUCH_SELECTOR (selector),
                                           COLUMN_MINUTES, &iter);
    gtk_tree_model_get (selector->priv->minutes_model,
                        &iter, COLUMN_INT, minutes, -1);
  }
}
