/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
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
#ifndef __HILDON_FILE_HANDLING_NOTE_H__
#define __HILDON_FILE_HANDLING_NOTE_H__


#ifndef HILDON_DISABLE_DEPRECATED

#include "hildon-note.h"

G_BEGIN_DECLS
#define HILDON_TYPE_FILE_HANDLING_NOTE \
  ( hildon_file_handling_note_get_type() )
#define HILDON_FILE_HANDLING_NOTE(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_FILE_HANDLING_NOTE,\
   HildonFileHandlingNote))
#define HILDON_FILE_HANDLING_NOTE_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_FILE_HANDLING_NOTE,\
   HildonFileHandlingNoteClass))
#define HILDON_IS_FILE_HANDLING_NOTE(obj) (GTK_CHECK_TYPE (obj,\
  HILDON_TYPE_FILE_HANDLING_NOTE))
#define HILDON_IS_FILE_HANDLING_NOTE_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_FILE_HANDLING_NOTE))
typedef struct _HildonFileHandlingNote HildonFileHandlingNote;
typedef struct _HildonFileHandlingNoteClass HildonFileHandlingNoteClass;

struct _HildonFileHandlingNote {
    HildonNote parent;
};

struct _HildonFileHandlingNoteClass {
    HildonNoteClass parent_class;
};

/* Note creation functions */
GtkWidget *hildon_file_handling_note_new_moving(GtkWindow * parent);
GtkWidget *hildon_file_handling_note_new_deleting(GtkWindow * parent);
GtkWidget *hildon_file_handling_note_new_opening(GtkWindow * parent);
GtkWidget *hildon_file_handling_note_new_saving(GtkWindow * parent);

/* Function for progressbar status setting */
void hildon_file_handling_note_set_fraction(HildonFileHandlingNote * note,
                                            gfloat frac);
void hildon_file_handling_note_set_counter_and_name(HildonFileHandlingNote
                                                    * note, guint current,
                                                    guint maximum,
                                                    const gchar * name);
void hildon_file_handling_note_set_name(HildonFileHandlingNote * note,
                                        const gchar * name);

GType hildon_file_handling_note_get_type(void);

G_END_DECLS
#endif /* HILDON_DISABLE_DEPRECATED */

#endif /* __HILDON_FILE_HANDLING_NOTE_H__ */
