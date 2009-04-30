/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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

#ifndef                                         __HILDON_DATE_EDITOR_PRIVATE_H__
#define                                         __HILDON_DATE_EDITOR_PRIVATE_H__

G_BEGIN_DECLS

#define                                         HILDON_DATE_EDITOR_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE((obj),\
                                                HILDON_TYPE_DATE_EDITOR, HildonDateEditorPrivate));

typedef struct                                  _HildonDateEditorPrivate HildonDateEditorPrivate;

struct                                          _HildonDateEditorPrivate 
{
    /* Cache of values in the entries, used in setting only parts of the date */
    guint year;                                 /* current year  in the entry */
    guint month;                                /* current month in the entry */
    guint day;                                  /* current day   in the entry */

    gboolean calendar_icon_pressed;

    GtkWidget *frame;                           /* borders around the date */
    GtkWidget *d_button_image;                  /* icon */
    GtkWidget *d_box_date;                      /* hbox for date */

    GtkWidget *d_entry;                         /* GtkEntry for day */
    GtkWidget *m_entry;                         /* GtkEntry for month */
    GtkWidget *y_entry;                         /* GtkEntry for year */

    GList *delims;                              /* List of delimeters between the fields 
                                                 * (and possible at the ends) */
    GtkWidget *calendar_icon;

    gboolean skip_validation;                   /* don't validate date at all */

    gint min_year;                              /* minimum year allowed */
    gint max_year;                              /* maximum year allowed */
};

G_END_DECLS

#endif                                          /* __HILDON_DATE_EDITOR_PRIVATE_H__ */
