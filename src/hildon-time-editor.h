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

#ifndef                                         __HILDON_TIME_EDITOR_H__
#define                                         __HILDON_TIME_EDITOR_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_TIME_EDITOR \
                                                (hildon_time_editor_get_type())

#define                                         HILDON_TIME_EDITOR(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_TIME_EDITOR, HildonTimeEditor))

#define                                         HILDON_TIME_EDITOR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_TIME_EDITOR, \
                                                HildonTimeEditorClass))

#define                                         HILDON_IS_TIME_EDITOR(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_TIME_EDITOR))

#define                                         HILDON_IS_TIME_EDITOR_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_TIME_EDITOR))

typedef enum 
{
    HILDON_DATE_TIME_ERROR_NO_ERROR = -1,
    HILDON_DATE_TIME_ERROR_MAX_HOURS,
    HILDON_DATE_TIME_ERROR_MAX_MINS,
    HILDON_DATE_TIME_ERROR_MAX_SECS,
    HILDON_DATE_TIME_ERROR_MAX_DAY,
    HILDON_DATE_TIME_ERROR_MAX_MONTH,
    HILDON_DATE_TIME_ERROR_MAX_YEAR,
    HILDON_DATE_TIME_ERROR_MIN_HOURS,
    HILDON_DATE_TIME_ERROR_MIN_MINS,
    HILDON_DATE_TIME_ERROR_MIN_SECS,
    HILDON_DATE_TIME_ERROR_MIN_DAY,
    HILDON_DATE_TIME_ERROR_MIN_MONTH,
    HILDON_DATE_TIME_ERROR_MIN_YEAR,
    HILDON_DATE_TIME_ERROR_EMPTY_HOURS,
    HILDON_DATE_TIME_ERROR_EMPTY_MINS,
    HILDON_DATE_TIME_ERROR_EMPTY_SECS,
    HILDON_DATE_TIME_ERROR_EMPTY_DAY,
    HILDON_DATE_TIME_ERROR_EMPTY_MONTH,
    HILDON_DATE_TIME_ERROR_EMPTY_YEAR,
    HILDON_DATE_TIME_ERROR_MIN_DURATION,
    HILDON_DATE_TIME_ERROR_MAX_DURATION,
    HILDON_DATE_TIME_ERROR_INVALID_CHAR,
    HILDON_DATE_TIME_ERROR_INVALID_DATE,
    HILDON_DATE_TIME_ERROR_INVALID_TIME
}                                               HildonDateTimeError;

typedef struct                                  _HildonTimeEditor HildonTimeEditor;

typedef struct                                  _HildonTimeEditorClass HildonTimeEditorClass;

struct                                          _HildonTimeEditor 
{
    GtkContainer parent;
};

struct                                          _HildonTimeEditorClass 
{
    GtkContainerClass parent_class;

    gboolean (*time_error) (HildonTimeEditor *editor,
                            HildonDateTimeError type);
};

GType G_GNUC_CONST
hildon_time_editor_get_type                     (void);

GtkWidget*
hildon_time_editor_new                          (void);

void      
hildon_time_editor_set_time                     (HildonTimeEditor *editor,
                                                 guint hours,
                                                 guint minutes,
                                                 guint seconds);

void       
hildon_time_editor_get_time                     (HildonTimeEditor *editor,
                                                 guint *hours,
                                                 guint *minutes,
                                                 guint *seconds);

void       
hildon_time_editor_set_duration_range           (HildonTimeEditor *editor,
                                                 guint min_seconds,
                                                 guint max_seconds);

void       
hildon_time_editor_get_duration_range           (HildonTimeEditor *editor,
                                                 guint *min_seconds,
                                                 guint *max_seconds);

void       
hildon_time_editor_set_ticks                    (HildonTimeEditor *editor, 
                                                 guint ticks);

guint    
hildon_time_editor_get_ticks                    (HildonTimeEditor *editor);

void       
hildon_time_editor_set_show_seconds             (HildonTimeEditor *editor, 
                                                 gboolean show_seconds);

gboolean
hildon_time_editor_get_show_seconds             (HildonTimeEditor *editor);

void       
hildon_time_editor_set_show_hours               (HildonTimeEditor *editor, 
                                                 gboolean show_hours);

gboolean  
hildon_time_editor_get_show_hours               (HildonTimeEditor *editor);

void     
hildon_time_editor_set_duration_mode            (HildonTimeEditor *editor, 
                                                 gboolean duration_mode);

gboolean   
hildon_time_editor_get_duration_mode            (HildonTimeEditor *editor);

void    
hildon_time_editor_set_duration_min             (HildonTimeEditor *editor, 
                                                 guint duration_min);

guint     
hildon_time_editor_get_duration_min             (HildonTimeEditor *editor);

void    
hildon_time_editor_set_duration_max             (HildonTimeEditor *editor, 
                                                 guint duration_max);

guint 
hildon_time_editor_get_duration_max             (HildonTimeEditor *editor);

void 
hildon_time_editor_get_time_separators          (GtkLabel *hm_sep_label,
                                                 GtkLabel *ms_sep_label);

G_END_DECLS

#endif                                          /* __HILDON_TIME_EDITOR_H__ */
