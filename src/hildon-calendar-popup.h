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

#ifndef                                         __HILDON_CALENDAR_POPUP_H__
#define                                         __HILDON_CALENDAR_POPUP_H__

#include                                        <gtk/gtkdialog.h>

G_BEGIN_DECLS

typedef struct                                  _HildonCalendarPopup HildonCalendarPopup;

typedef struct                                  _HildonCalendarPopupClass HildonCalendarPopupClass;

#define                                         HILDON_TYPE_CALENDAR_POPUP \
                                                (hildon_calendar_popup_get_type())

#define                                         HILDON_CALENDAR_POPUP(obj) (GTK_CHECK_CAST (obj,\
                                                HILDON_TYPE_CALENDAR_POPUP, HildonCalendarPopup))

#define                                         HILDON_CALENDAR_POPUP_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_CALENDAR_POPUP, HildonCalendarPopupClass))

#define                                         HILDON_IS_CALENDAR_POPUP(obj) (GTK_CHECK_TYPE (obj,\
                                                HILDON_TYPE_CALENDAR_POPUP))

#define                                         HILDON_IS_CALENDAR_POPUP_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_CALENDAR_POPUP))

#define                                         HILDON_CALENDAR_POPUP_GET_CLASS(obj) \
                                                ((HildonCalendarPopupClass *) G_OBJECT_GET_CLASS(obj))

struct                                          _HildonCalendarPopup 
{
    GtkDialog parent;
};

struct                                          _HildonCalendarPopupClass 
{
    GtkDialogClass parent_class;
};

GType G_GNUC_CONST
hildon_calendar_popup_get_type                  (void);

GtkWidget*
hildon_calendar_popup_new                       (GtkWindow *parent, 
                                                 guint year,
                                                 guint month, 
                                                 guint day);

void 
hildon_calendar_popup_set_date                  (HildonCalendarPopup *cal,
                                                 guint year, 
                                                 guint month, 
                                                 guint day);

void 
hildon_calendar_popup_get_date                  (HildonCalendarPopup *cal,
                                                 guint *year, 
                                                 guint *month,
                                                 guint *day);

G_END_DECLS

#endif                                          /* __HILDON_CALENDAR_POPUP_H__ */
