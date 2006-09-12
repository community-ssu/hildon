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
 * the License or any later version.
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
 * SECTION:hildon-file-handling-note
 * @short_description: Displaying the notification when a move 
 * operation is in progress. 
 * @see_also: #HildonFileHandlingNote
 * 
 * This is the notification displayed when a move operation is in
 * progress.  The notification uses a progress bar to indicate the
 * progress of the operation. For operation containing multiple items, a
 * separe progress bar is shown for each item. The notification has a
 * single button, which allows users to stop the move operation.
 */

#include "hildon-note.h"
#include "hildon-file-handling-note.h"
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <glib.h>
#include <glib-object.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Note types */
#define MOVING_TYPE 0
#define DELETING_TYPE 1
#define OPENING_TYPE 2
#define SAVING_TYPE 3

/*#define _(String) dgettext(PACKAGE, String)*/
#define _(String) dgettext("hildon-fm", String) /* FIXME: this file should be moved to hildon-fm */

#define HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(obj)\
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
        HILDON_TYPE_FILE_HANDLING_NOTE, HildonFileHandlingNotePrivate));

typedef struct _HildonFileHandlingNotePrivate
    HildonFileHandlingNotePrivate;

struct _HildonFileHandlingNotePrivate {
    guint note_type;
};


static HildonNote *parent_class;


/* standard forbids empty source file, therefore the ifdef must be
   placed here. */

static void
hildon_file_handling_note_class_init(HildonFileHandlingNoteClass * class);

static void hildon_file_handling_note_init(HildonFileHandlingNote *
                                           dialog);

static void hildon_file_handling_note_finalize(GObject * obj_self);




GType hildon_file_handling_note_get_type()
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonFileHandlingNoteClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_file_handling_note_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonFileHandlingNote),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_file_handling_note_init
        };

        dialog_type = g_type_register_static(HILDON_TYPE_NOTE,
                                             "HildonFileHandlingNote",
                                             &dialog_info, 0);
    }
    return dialog_type;
}

static void
hildon_file_handling_note_class_init(HildonFileHandlingNoteClass * class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);

    parent_class = g_type_class_peek_parent(class);
    g_type_class_add_private(class, sizeof(HildonFileHandlingNotePrivate));
    object_class->finalize = hildon_file_handling_note_finalize;
}

static void hildon_file_handling_note_init(HildonFileHandlingNote * dialog)
{
    HildonFileHandlingNotePrivate *priv;

    priv = HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(dialog);
    priv->note_type = OPENING_TYPE;
}

static void hildon_file_handling_note_finalize(GObject * obj_self)
{
    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);
}

/**
 * hildon_file_handling_note_set_counter_and_name:
 * @note: the #HildonFileHandlingNote widget
 * @current: progress, current item being processed
 * @maximum: maximum value for counter (number of items)
 * @name: filename
 *
 * This function sets current counter value, maximum
 * counter value and filename for dialog 
 *
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
void hildon_file_handling_note_set_counter_and_name(HildonFileHandlingNote
                                                    * note, guint current,
                                                    guint maximum,
                                                    const gchar * name)
{
    gchar str[255];
    HildonNote *p_note = HILDON_NOTE(note);
    HildonFileHandlingNotePrivate *priv;

    priv = HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(note);

    if (priv->note_type == MOVING_TYPE) {
      if (maximum == 1)
        {
          g_snprintf(str, 255, _("sfil_nw_moving_file"), name);
          hildon_note_set_button_text(p_note, _("sfil_bd_moving_file"));
        } else {
          g_snprintf(str, 255, _("docm_nw_moving_files"),
                     current, maximum, name);
          hildon_note_set_button_text(p_note, _("docm_bd_moving_files"));
        }
    } else if (priv->note_type == SAVING_TYPE) {
      if (maximum == 1)
        {
          g_snprintf(str, 255, _("docm_nw_saving_file"), name);
          hildon_note_set_button_text(p_note, _("docm_bd_saving_file"));
        } else {
          g_snprintf(str, 255, _("docm_nw_saving_files"),
                   current, maximum, name);
          hildon_note_set_button_text(p_note, _("docm_bd_saving_files"));
        }
    } else if (priv->note_type == OPENING_TYPE) {
      if (maximum == 1)
        {
          g_snprintf(str, 255, _("docm_nw_opening_file"), name);
          hildon_note_set_button_text(p_note, _("docm_bd_opening_file"));
        } else {
          g_snprintf(str, 255, _("docm_nw_opening_files"),
                     current, maximum, name);
          hildon_note_set_button_text(p_note, _("docm_bd_opening_files"));
        }
    } else if (priv->note_type == DELETING_TYPE) {
      if (maximum == 1)
        {
          g_snprintf(str, 255, _("docm_nw_deleting_file"), name);
          hildon_note_set_button_text(p_note, _("docm_bd_deleting_file"));
        } else {
          g_snprintf(str, 255, _("docm_nw_deleting_files"),
                     current, maximum, name);
          hildon_note_set_button_text(p_note, _("docm_bd_deleting_files"));
        }
    }

    g_object_set(p_note, "description", str, NULL);
}

/**
 * hildon_file_handling_note_set_name:
 * @note: the @HildonFileHandlingNote widget
 * @name: filename
 *
 * This function sets the filename for dialog
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
void hildon_file_handling_note_set_name(HildonFileHandlingNote * note,
                                        const gchar * name)
{
    gchar str[255];
    HildonNote *p_note = HILDON_NOTE(note);
    HildonFileHandlingNotePrivate *priv =
        HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(note);

    if (priv->note_type == MOVING_TYPE) {
        g_snprintf(str, 255, _("sfil_nw_moving_file"), name);
    } else if (priv->note_type == SAVING_TYPE) {
        g_snprintf(str, 255, _("docm_nw_saving_file"), name);
    } else if (priv->note_type == OPENING_TYPE) {
        g_snprintf(str, 255, _("docm_nw_opening_file"), name);
    } else if (priv->note_type == DELETING_TYPE) {
        g_snprintf(str, 255, _("docm_nw_deleting_file"), name);
    }

    g_object_set(p_note, "description", str, NULL);
}

/**
 * hildon_file_handling_note_set_fraction:
 * @note: the @HildonFileHandlingNote widget
 * @frac: value for progress bar

 * This function sets fraction value for
 * progress bar.
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
void hildon_file_handling_note_set_fraction(HildonFileHandlingNote * note,
                                            gfloat frac)
{
    GtkWidget *progbar;

    g_object_get(note, "progressbar", &progbar, NULL);
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progbar), frac);
}

/**
 * hildon_file_handling_note_new_moving:
 * @parent: parent GtkWindow 
 *
 * This function creates new dialog
 * which is "moving" type.
 *
 * Returns: a new #HildonFileHandlingNote
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
GtkWidget *hildon_file_handling_note_new_moving(GtkWindow * parent)
{
    GtkWidget *progbar;
    HildonFileHandlingNote *file_note;
    HildonFileHandlingNotePrivate *priv;

    progbar = gtk_progress_bar_new();

    file_note = g_object_new(HILDON_TYPE_FILE_HANDLING_NOTE,
                             "note_type", HILDON_NOTE_PROGRESSBAR_TYPE,
                             "description", _("sfil_nw_moving_file"), 
                             "progressbar", progbar, NULL);

    priv = HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(file_note);
    priv->note_type = MOVING_TYPE;

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(file_note), parent);

    return GTK_WIDGET(file_note);
}

/**
 * hildon_file_handling_note_new_deleting:
 * @parent: parent GtkWindow 
 *
 * This function creates new dialog
 * which is "deleting" type.
 *
 * Returns: a new #HildonFileHandlingNote
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
GtkWidget *hildon_file_handling_note_new_deleting(GtkWindow * parent)
{
    GtkWidget *progbar;
    HildonFileHandlingNote *file_note;
    HildonFileHandlingNotePrivate *priv;

    progbar = gtk_progress_bar_new();

    file_note = g_object_new(HILDON_TYPE_FILE_HANDLING_NOTE,
                             "note_type", HILDON_NOTE_PROGRESSBAR_TYPE,
                             "description", _("docm_nw_deleting_file"),
                             "progressbar", progbar, NULL);

    priv = HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(file_note);
    priv->note_type = DELETING_TYPE;

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(file_note), parent);

    return GTK_WIDGET(file_note);
}

/**
 * hildon_file_handling_note_new_opening:
 * @parent: parent GtkWindow 
 *
 * This function creates new dialog
 * which is "opening" type
 *
 * Returns: a new #HildonFileHandlingNote
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
GtkWidget *hildon_file_handling_note_new_opening(GtkWindow * parent)
{
    GtkWidget *progbar;
    HildonFileHandlingNote *file_note;
    HildonFileHandlingNotePrivate *priv;

    progbar = gtk_progress_bar_new();

    file_note = g_object_new(HILDON_TYPE_FILE_HANDLING_NOTE,
                             "note_type", HILDON_NOTE_PROGRESSBAR_TYPE,
                             "description", _("docm_nw_opening_file"),
                             "progressbar", progbar, NULL);

    priv = HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(file_note);
    priv->note_type = OPENING_TYPE;

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(file_note), parent);

    return GTK_WIDGET(file_note);
}

/**
 * hildon_file_handling_note_new_saving:
 * @parent: parent GtkWindow 
 *
 * This function creates new dialog
 * which is "saving" type.
 *
 * Returns: a new #HildonFileHandlingNote
 * DEPRECATED: use hildon-note instead of hildon-file-handling-note.
 */
GtkWidget *hildon_file_handling_note_new_saving(GtkWindow * parent)
{
    GtkWidget *progbar;
    HildonFileHandlingNote *file_note;
    HildonFileHandlingNotePrivate *priv;

    progbar = gtk_progress_bar_new();

    file_note = g_object_new(HILDON_TYPE_FILE_HANDLING_NOTE,
                             "note_type", HILDON_NOTE_PROGRESSBAR_TYPE,
                             "description", _("docm_nw_saving_file"),
                             "progressbar", progbar, NULL);

    priv = HILDON_FILE_HANDLING_NOTE_GET_PRIVATE(file_note);
    priv->note_type = SAVING_TYPE;

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(file_note), parent);

    return GTK_WIDGET(file_note);
}

