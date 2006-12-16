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

#ifndef                                         __HILDON_CALENDAR_POPUP_PRIVATE_H__
#define                                         __HILDON_CALENDAR_POPUP_PRIVATE_H__

#include                                        <gtk/gtkdialog.h>

G_BEGIN_DECLS

typedef struct                                  _HildonCalendarPopupPrivate HildonCalendarPopupPrivate;

#define                                         HILDON_CALENDAR_POPUP_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE \
                                                ((obj), HILDON_TYPE_CALENDAR_POPUP, HildonCalendarPopupPrivate));

struct                                          _HildonCalendarPopupPrivate 
{
    GtkWidget *cal;
};

G_END_DECLS

#endif                                          /* __HILDON_CALENDAR_POPUP_PRIVATE_H__ */
