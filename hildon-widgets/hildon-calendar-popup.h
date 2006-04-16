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

#ifndef __HILDON_CALENDAR_POPUP_H__
#define __HILDON_CALENDAR_POPUP_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
/**
 * HILDON_TYPE_CALENDAR_POPUP:
 *
 * Macro for getting type of calendar popup.
 * Since: 0.12.10
 */
#define HILDON_TYPE_CALENDAR_POPUP ( hildon_calendar_popup_get_type() )

/**
 * HILDON_CALENDAR_POPUP_TYPE:
 *
 * Deprecated: use #HILDON_TYPE_CALENDAR_POPUP instead
 */
#define HILDON_CALENDAR_POPUP_TYPE HILDON_TYPE_CALENDAR_POPUP

#define HILDON_CALENDAR_POPUP(obj) (GTK_CHECK_CAST (obj,\
  HILDON_TYPE_CALENDAR_POPUP, HildonCalendarPopup))
#define HILDON_CALENDAR_POPUP_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
  HILDON_TYPE_CALENDAR_POPUP, HildonCalendarPopupClass))
#define HILDON_IS_CALENDAR_POPUP(obj) (GTK_CHECK_TYPE (obj,\
  HILDON_TYPE_CALENDAR_POPUP))
#define HILDON_IS_CALENDAR_POPUP_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_CALENDAR_POPUP))

/**
 * HildonCalendarPopup:
 *
 * Internal struct for calendar popup.
 */
typedef struct _HildonCalendarPopup HildonCalendarPopup;
typedef struct _HildonCalendarPopupClass HildonCalendarPopupClass;

/* Note: CalendarPopup is no longer derived from GtkWindow
   but from GtkDialog */

struct _HildonCalendarPopup {
    GtkDialog par;
};

struct _HildonCalendarPopupClass {
    GtkDialogClass parent_class;
};

GType hildon_calendar_popup_get_type(void) G_GNUC_CONST;

GtkWidget *hildon_calendar_popup_new(GtkWindow * parent, guint year,
                                     guint month, guint day);

void hildon_calendar_popup_set_date(HildonCalendarPopup * cal,
                                    guint year, guint month, guint day);

void hildon_calendar_popup_get_date(HildonCalendarPopup * cal,
                                    guint * year, guint * month,
                                    guint * day);

G_END_DECLS
#endif
