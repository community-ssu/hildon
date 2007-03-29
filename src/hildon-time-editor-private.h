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

#ifndef                                         __HILDON_TIME_EDITOR_PRIVATE_H__
#define                                         __HILDON_TIME_EDITOR_PRIVATE_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TIME_EDITOR_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj),\
                                                HILDON_TYPE_TIME_EDITOR, HildonTimeEditorPrivate));

/* Indices for h/m/s entries in priv->entries */
enum {
    ENTRY_HOURS,
    ENTRY_MINS,
    ENTRY_SECS,
  
    ENTRY_COUNT
};

typedef struct                                  _HildonTimeEditorPrivate  HildonTimeEditorPrivate;

struct                                          _HildonTimeEditorPrivate 
{
    guint      ticks;                           /* Current duration in seconds  */

    gchar     *am_symbol;
    gchar     *pm_symbol;

    GtkWidget *iconbutton;                      /* button for icon              */

    GtkWidget *frame;                           /* frame around the entries     */
    GtkWidget *entries[ENTRY_COUNT];            /* h, m, s entries              */
    GtkWidget *hm_label;                        /* between hour and minute      */
    GtkWidget *sec_label;                       /* between minute and second    */
    GtkWidget *ampm_label;                      /* label for showing am or pm   */

    GtkWidget *error_widget;                    /* field to highlight in idle   */
    GtkWidget *ampm_button;                     /* am/pm change button */


    gboolean   duration_mode;                   /* In HildonDurationEditor mode */
    gboolean   show_seconds;                    /* show seconds                 */
    gboolean   show_hours;                      /* show hours                   */

    gboolean   ampm_pos_after;                  /* is am/pm shown after others  */
    gboolean   clock_24h;                       /* whether to show a 24h clock  */
    gboolean   am;                              /* TRUE == am, FALSE == pm      */

    guint      duration_min;                    /* duration editor ranges       */
    guint      duration_max;                    /* duration editor ranges       */

    guint      highlight_idle;
    gboolean   skipper;                         /* FIXME (MDK): To prevent us from looping inside the validation events. 
                                                   When set to TRUE further validations (that can happen from-inside other 
                                                   validations) are being skipped. Nasty hack to cope with a bad design. */
};

G_END_DECLS

#endif                                          /* __HILDON_TIME_EDITOR_H__ */
