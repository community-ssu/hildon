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

/* 
 * @file hildon-note.c
 *
 * This file contains API for conformation, information
 * and cancel notes. 
 *
 * 9/2004 Removed animation type of cancel note as separate task.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hildon-note.h"
#include <gtk/gtklabel.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkbutton.h>
#include <libintl.h>
#include <hildon-widgets/hildon-defines.h>

/* For now, we use D-BUS directly to request sound playback. This
   should later be replaced with calls to a wrapper API. */

#define DBUS_API_SUBJECT_TO_CHANGE
#include <dbus/dbus.h>

/* Can these be included from somewhere? */

#define OSSO_MEDIA_SERVICE "com.nokia.osso_media_server"
#define OSSO_MEDIA_PATH "/com/nokia/osso_media_server"
#define OSSO_MEDIA_INTERFACE "com.nokia.osso_media_server.sound"
#define OSSO_MEDIA_PLAY_METHOD "play_sound"

#define CONFIRMATION_SOUND_PATH "file:///usr/share/sounds/"\
                                "ui-confirmation_note.wav"
#define INFORMATION_SOUND_PATH "file:///usr/share/sounds/"\
                               "ui-information_note.wav"

#define HILDON_NOTE_CONFIRMATION_STOCK        "hildon-note-confirmation"
#define HILDON_NOTE_INFORMATION_STOCK         "hildon-note-info"
#define HILDON_NOTE_DELETE_CONFIRMATION_STOCK \
        "hildon-note-delete-confirmation"

#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass *parent_class;

#define HILDON_NOTE_TITLE  _("note")

#define HILDON_NOTE_GET_PRIVATE(obj)\
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
  HILDON_TYPE_NOTE, HildonNotePrivate));

typedef struct _HildonNotePrivate HildonNotePrivate;

static void hildon_note_class_init(HildonNoteClass * class);
static void hildon_note_init(HildonNote * dialog);

static void hildon_note_create_form(GtkDialog * dialog, GtkWidget * item,
                                    gboolean IsHorizontal);
static void hildon_note_finalize(GObject * obj_self);
static void hildon_note_map_event(GtkWidget * widget);
static GObject *hildon_note_constructor(GType type,
                                        guint n_construct_properties,
                                        GObjectConstructParam
                                        * construct_properties);
static void hildon_note_set_property(GObject * object,
                                     guint prop_id,
                                     const GValue * value,
                                     GParamSpec * pspec);
static void hildon_note_get_property(GObject * object,
                                     guint prop_id,
                                     GValue * value, GParamSpec * pspec);

/* common measurement */
const int _HILDON_NOTE_BORDER_WIDTH = 20;       /* for top and bottom
                                                   border is height, for
                                                   left and right is
                                                   width */
const int _HILDON_NOTE_TEXT_HEIGHT = 26;

/* height, width for confirmation notes and information notes
   button height 47, text height =24?
*/
const int _HILDON_NOTE_CONFIRMATION_TEXT_MIN_WIDTH = 180; /*270-20-20-50*/
const int _HILDON_NOTE_CONFIRMATION_TEXT_MAX_WIDTH = 370; /*460-20-20-50*/
const int _HILDON_NOTE_CONFIRMATION_NOTATION_SIDE = 50;

/* height, width for cancel notes with progress bar
 */
const int _HILDON_NOTE_CANCEL_PROGRESS_HEIGHT = 153; /* 6+24+26+30+20+47 */
const int _HILDON_NOTE_CANCEL_PROGRESS_WIDTH = 215;
const int _HILDON_NOTE_PROGRESS_HEIGHT = 30;
const int _HILDON_NOTE_PROGRESS_WIDTH = 215;

/* height, width for cancel notes with animation
 */
const int _HILDON_NOTE_CANCEL_ANIME_HEIGHT = 177; /* 6+24+26+54+20+47 */
 /* This is more than in the specifications -> the specification width is 
    not enough for the button.. (nor the text) */
const int _HILDON_NOTE_CANCEL_ANIME_WIDTH = 181;
const int _HILDON_NOTE_ANIME_HEIGHT = 54;
const int _HILDON_NOTE_ANIME_WIDTH = 26;

struct _HildonNotePrivate {
    GtkWidget *okButton;
    GtkWidget *cancelButton;
    GtkWidget *label;

    gint note_n;
    GtkWidget *progressbar;
    gchar *icon;

    gchar *original_description;
};

enum {
    PROP_NONE = 0,
    PROP_HILDON_NOTE_TYPE,
    PROP_HILDON_NOTE_DESCRIPTION,
    PROP_HILDON_NOTE_ICON,
    PROP_HILDON_NOTE_PROGRESSBAR
};

/* This function is just a modified version of two_lines_truncate
 * in gtk-infoprint.c */
static void
hildon_note_five_line_truncate(const HildonNote * note, const gchar * text)
{
    gchar *result = NULL;
    PangoLayout *layout;
    PangoContext *context;
    gchar *str = NULL;
    int max_width = _HILDON_NOTE_CONFIRMATION_TEXT_MAX_WIDTH;
    HildonNotePrivate *priv;

    priv = HILDON_NOTE_GET_PRIVATE(note);

    if (priv->original_description != NULL)
        g_free(priv->original_description);

    priv->original_description = g_strdup(text);

    if (text == NULL) {
        str = g_strdup("");
    } else {
        str = g_strdup(text);
    }

    context = gtk_widget_get_pango_context(GTK_WIDGET(note));

    {
        gchar *lines[5] = { NULL, NULL, NULL, NULL, NULL };
        guint current_line = 0;
        guint last_line;

        layout = pango_layout_new(context);
        pango_layout_set_text(layout, str, -1);
        pango_layout_set_width(layout, max_width * PANGO_SCALE);
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);
        last_line = MIN(4, pango_layout_get_line_count(layout) - 1);

        for (current_line = 0;
             current_line <= last_line;
             current_line++) {
            PangoLayoutLine *line = pango_layout_get_line(layout,
                                                          current_line);

            if (current_line == last_line)
                lines[current_line] = g_strdup(str + line->start_index);
            else
                lines[current_line] = g_strndup(str + line->start_index,
                                                line->length);

            pango_layout_line_ref(line);
            pango_layout_line_unref(line);
        }

        g_object_unref(layout);

        layout = pango_layout_new(context);
        pango_layout_set_text(layout, lines[last_line], -1);

        {
            PangoLayoutLine *line;
            gint index = 0;

            if (pango_layout_get_line_count(layout) > 5) {
                gchar *templine = NULL;

                line = pango_layout_get_line(layout, 0);
                templine = g_strndup(lines[last_line], line->length);
                g_free(lines[last_line]);
                lines[last_line] = g_strconcat(templine, "\342\200\246", NULL);
                g_free(templine);
            }

            if (pango_layout_xy_to_index(layout,
                                         max_width * PANGO_SCALE, 0,
                                         &index, NULL) == TRUE) {
                gint ellipsiswidth;
                gchar *tempresult;
                PangoLayout *ellipsis = pango_layout_new(context);

                pango_layout_set_text(ellipsis, "\342\200\246", -1);
                pango_layout_get_size(ellipsis, &ellipsiswidth, NULL);
                pango_layout_xy_to_index(layout,
                                         max_width * PANGO_SCALE -
                                         ellipsiswidth, 0, &index,
                                         NULL);
                g_object_unref(G_OBJECT(ellipsis));
                tempresult = g_strndup(lines[last_line], index);
                lines[last_line] = g_strconcat(tempresult,
                                               "\342\200\246",
                                               NULL);
                g_free(tempresult);
            }
        }

        for (current_line = 0; current_line <= last_line; current_line++)
            g_strchomp(lines[current_line]);

        result = g_strconcat(lines[0], "\n",
                             lines[1], "\n",
                             lines[2], "\n",
                             lines[3], "\n",
                             lines[4], "\n", NULL);

        for (current_line = 0; current_line <= last_line; current_line++)
            g_free(lines[current_line]);

        g_object_unref(layout);

    }

    if (result == NULL)
        result = g_strdup(str);

    gtk_label_set_text(GTK_LABEL(priv->label), result);

    g_free(str);
    g_free(result);

}

static void
hildon_note_one_line_truncate(const HildonNote * note, const gchar * text)
{
    PangoLayout *layout;
    PangoContext *context;
    gchar *str = NULL;
    int max_width = _HILDON_NOTE_CONFIRMATION_TEXT_MAX_WIDTH;
    HildonNotePrivate *priv;
    PangoLayoutLine *line;
    gint index = 0;

    priv = HILDON_NOTE_GET_PRIVATE(note);

    if (priv->original_description != NULL)
        g_free(priv->original_description);

    priv->original_description = g_strdup(text);

    str = g_strdup(text == NULL ? "" : text);
    context = gtk_widget_get_pango_context(GTK_WIDGET(note));

    layout = pango_layout_new(context);
    pango_layout_set_text(layout, str, -1);
    pango_layout_set_width(layout, max_width * PANGO_SCALE);
    pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

    if (pango_layout_get_line_count(layout) > 1) {
        gchar *templine = NULL;

        line = pango_layout_get_line(layout, 0);
        templine = g_strndup(str, line->length);
        g_free(str);
        str = g_strconcat(templine, "\342\200\246", NULL);
        g_free(templine);
    }

    if (pango_layout_xy_to_index(layout,
                                 max_width * PANGO_SCALE, 0,
                                 &index, NULL) == TRUE) {
        gint ellipsiswidth;
        gchar *tempresult;
        PangoLayout *ellipsis = pango_layout_new(context);

        pango_layout_set_text(ellipsis, "\342\200\246", -1);
        pango_layout_get_size(ellipsis, &ellipsiswidth, NULL);
        pango_layout_xy_to_index(layout,
                                 max_width * PANGO_SCALE -
                                 ellipsiswidth, 0, &index,
                                 NULL);
        g_object_unref(G_OBJECT(ellipsis));
        tempresult = g_strndup(str, index);
        str = g_strconcat(tempresult,
                          "\342\200\246",
                          NULL);
        g_free(tempresult);
    }

    g_object_unref(layout);
    gtk_label_set_text(GTK_LABEL(priv->label), str);
    g_free(str);
}

static void
hildon_note_set_property(GObject * object,
                         guint prop_id,
                         const GValue * value, GParamSpec * pspec)
{
    HildonNote *note = HILDON_NOTE(object);
    HildonNotePrivate *priv;

    priv = HILDON_NOTE_GET_PRIVATE(note);

    switch (prop_id) {
    case PROP_HILDON_NOTE_TYPE:
        priv->note_n = g_value_get_int(value);
        break;
    case PROP_HILDON_NOTE_DESCRIPTION:
        if (priv->note_n == HILDON_NOTE_PROGRESSBAR_TYPE)
            hildon_note_one_line_truncate(note, g_value_get_string(value));
        else
            hildon_note_five_line_truncate(note, g_value_get_string(value));
        break;
    case PROP_HILDON_NOTE_ICON:
	if( priv->icon )
	    g_free(priv->icon);
        priv->icon = g_value_dup_string(value);
        break;
    case PROP_HILDON_NOTE_PROGRESSBAR:
        priv->progressbar = g_value_get_object(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_note_get_property(GObject * object,
                         guint prop_id, GValue * value, GParamSpec * pspec)
{
    HildonNote *note = HILDON_NOTE(object);
    HildonNotePrivate *priv;

    priv = HILDON_NOTE_GET_PRIVATE(note);

    switch (prop_id) {
    case PROP_HILDON_NOTE_TYPE:
        g_value_set_int(value, g_value_get_int(value));
        break;
    case PROP_HILDON_NOTE_DESCRIPTION:
        if (priv->original_description != NULL) {
            g_value_set_string(value, priv->original_description);
        } else {
            g_value_set_string(value, "");
        }
        break;
    case PROP_HILDON_NOTE_ICON:
        g_value_set_string(value, priv->icon);
        break;
    case PROP_HILDON_NOTE_PROGRESSBAR:
        g_value_set_object(value, priv->progressbar);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

GType hildon_note_get_type()
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonNoteClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_note_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonNote),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_note_init
        };

        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonNote",
                                             &dialog_info, 0);
    }
    return dialog_type;
}

static GObject *hildon_note_constructor(GType type,
                                        guint n_construct_properties,
                                        GObjectConstructParam *
                                        construct_properties)
{
    GObject *dialog;
    HildonNotePrivate *priv;
    GtkWidget *item = NULL;
    gboolean IsHorizontal = TRUE;


    dialog = G_OBJECT_CLASS(parent_class)->constructor
             (type, n_construct_properties, construct_properties);
    priv = HILDON_NOTE_GET_PRIVATE(dialog);

    if (priv->note_n == HILDON_NOTE_CONFIRMATION_TYPE ||
        priv->note_n == HILDON_NOTE_CONFIRMATION_BUTTON_TYPE ||
        priv->note_n == HILDON_NOTE_INFORMATION_THEME_TYPE ||
        priv->note_n == HILDON_NOTE_INFORMATION_TYPE) {
        if (priv->note_n == HILDON_NOTE_CONFIRMATION_TYPE) {
            /* ok button clickable with mouse or whatever */
            priv->okButton = 
                gtk_dialog_add_button(GTK_DIALOG(dialog),
                                      _("Ecdg_bd_confirmation_note_ok"),
                                      GTK_RESPONSE_OK);
            /* cancel button clickable with mouse or whatever */
            priv->cancelButton =
                gtk_dialog_add_button(GTK_DIALOG(dialog),
                                      _("Ecdg_bd_confirmation_note_cancel"),
                                      GTK_RESPONSE_CANCEL);

        } else if (priv->note_n == HILDON_NOTE_INFORMATION_TYPE || 
		   priv->note_n == HILDON_NOTE_INFORMATION_THEME_TYPE ) {
            priv->okButton = NULL;
            /* cancel button clickable with mouse or whatever */
            priv->cancelButton =
                gtk_dialog_add_button(GTK_DIALOG(dialog),
                                      _("Ecdg_bd_information_note_ok"),
                                      GTK_RESPONSE_CANCEL);
        }

        gtk_widget_realize(GTK_WIDGET(dialog));
        gdk_window_set_decorations(GTK_WIDGET(dialog)->window,
                                   GDK_DECOR_BORDER);

        if ((priv->note_n == HILDON_NOTE_INFORMATION_TYPE || priv->note_n == HILDON_NOTE_INFORMATION_THEME_TYPE) && 
	    priv->icon) {
            item = gtk_image_new_from_icon_name(priv->icon,
                                            HILDON_ICON_SIZE_BIG_NOTE);
	}
	else {
            if (priv->note_n == HILDON_NOTE_CONFIRMATION_TYPE ||
                priv->note_n == HILDON_NOTE_CONFIRMATION_BUTTON_TYPE) {
                    item = gtk_image_new_from_icon_name
                        ("qgn_note_confirm", 
                         HILDON_ICON_SIZE_50); 
            } else {
                    item =
                        gtk_image_new_from_icon_name
                        ("qgn_note_info", 
                         HILDON_ICON_SIZE_50);
            }
        }

    } else {
        priv->cancelButton = 
            gtk_dialog_add_button(GTK_DIALOG(dialog),
                                  _("Ecdg_bd_cancel_note_cancel"),
                                  GTK_RESPONSE_CANCEL);
        IsHorizontal = FALSE;
        priv->okButton = NULL;

        item = priv->progressbar;

        gtk_widget_realize(GTK_WIDGET(dialog));
        gdk_window_set_decorations(GTK_WIDGET(dialog)->window,
                                   GDK_DECOR_BORDER);
    }

    hildon_note_create_form(GTK_DIALOG(dialog), item, IsHorizontal);

    return dialog;
}

static void hildon_note_class_init(HildonNoteClass * class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);

    /* set the global parent_class */
    parent_class = g_type_class_peek_parent(class);

    g_type_class_add_private(class, sizeof(HildonNotePrivate));

    object_class->finalize = hildon_note_finalize;

    widget_class->map = hildon_note_map_event;
    object_class->set_property = hildon_note_set_property;
    object_class->get_property = hildon_note_get_property;
    object_class->constructor = hildon_note_constructor;

    g_object_class_install_property(object_class,
        PROP_HILDON_NOTE_TYPE,
        g_param_spec_int("note_type",
                         "note type",
                         "Set type to dialog",
                         0, 5, 5,
                         G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
        PROP_HILDON_NOTE_DESCRIPTION,
        g_param_spec_string("description",
                            "note description",
                            "Description for note",
                            "",
                            G_PARAM_READABLE | G_PARAM_WRITABLE));

    g_object_class_install_property(object_class,
        PROP_HILDON_NOTE_ICON,
        g_param_spec_string("icon",
                            "note icon",
                            "Icon for note",
                            "",
                            G_PARAM_CONSTRUCT_ONLY
                            | G_PARAM_READABLE
                            | G_PARAM_WRITABLE));
    g_object_class_install_property(object_class,
        PROP_HILDON_NOTE_PROGRESSBAR,
        g_param_spec_object("progressbar",
                            "Progressbar widget",
                            "The progressbar for note",
                            GTK_TYPE_PROGRESS_BAR,
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
}

static void hildon_note_init(HildonNote * dialog)
{
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE(dialog);

    priv->label = gtk_label_new(NULL);
    priv->original_description = NULL;
    priv->icon = NULL;

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_title(GTK_WINDOW(dialog), HILDON_NOTE_TITLE);
}

static void hildon_note_map_event(GtkWidget * widget)
{
  DBusConnection *conn = NULL;
  DBusMessage *msg = NULL;
  DBusError err;
  dbus_bool_t b;
  HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE(widget);

  GTK_WIDGET_CLASS (parent_class)->map(widget);

  /* Is the type of the note such that we will have to play a sound? */

  if (priv->note_n != HILDON_NOTE_INFORMATION_TYPE &&
      priv->note_n != HILDON_NOTE_INFORMATION_THEME_TYPE &&
      priv->note_n != HILDON_NOTE_CONFIRMATION_TYPE &&
      priv->note_n != HILDON_NOTE_CONFIRMATION_BUTTON_TYPE)
    {
      return;
    }

  dbus_error_init(&err);
  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);

  if (conn == NULL)
    {
      dbus_error_free(&err);
      return;
    }

  msg = dbus_message_new_method_call(OSSO_MEDIA_SERVICE, 
				     OSSO_MEDIA_PATH,
				     OSSO_MEDIA_INTERFACE, 
				     OSSO_MEDIA_PLAY_METHOD);
  if (msg == NULL)
    {
      dbus_connection_unref(conn);
      return;
    }

  dbus_message_set_auto_activation(msg, FALSE);
  dbus_message_set_no_reply(msg, TRUE);

  if (priv->note_n == HILDON_NOTE_INFORMATION_TYPE ||
      priv->note_n == HILDON_NOTE_INFORMATION_THEME_TYPE)
    {
      dbus_message_append_args(msg, DBUS_TYPE_STRING, INFORMATION_SOUND_PATH,
			       DBUS_TYPE_INT32, 1, DBUS_TYPE_INVALID);
    }
  else if (priv->note_n == HILDON_NOTE_CONFIRMATION_TYPE ||
	   priv->note_n ==  HILDON_NOTE_CONFIRMATION_BUTTON_TYPE)
    {
      dbus_message_append_args(msg, DBUS_TYPE_STRING,
			       CONFIRMATION_SOUND_PATH,
			       DBUS_TYPE_INT32, 1, DBUS_TYPE_INVALID);
    }

  b = dbus_connection_send(conn, msg, NULL);

  if (!b)
    {
      dbus_message_unref(msg);
      return;
    }
  dbus_connection_flush(conn);
  dbus_message_unref(msg);
  
}

static void hildon_note_finalize(GObject * obj_self)
{
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE(obj_self);

    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);

    if(priv->icon)
	g_free(priv->icon);

    if (priv->original_description != NULL)
        g_free(priv->original_description);
}

static void
hildon_note_create_form(GtkDialog * dialog, GtkWidget * item,
                        gboolean IsHorizontal)
{
    GtkWidget *box;
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE(dialog);

    if (IsHorizontal) {
        box = gtk_hbox_new(FALSE, 10);
        gtk_container_add(GTK_CONTAINER(dialog->vbox), box);

        if (item) {
            GtkWidget *alignment = gtk_alignment_new(0, 0, 0, 0);

            gtk_box_pack_start(GTK_BOX(box), alignment, FALSE, FALSE, 0);
            gtk_container_add(GTK_CONTAINER(alignment), item);
        }
        gtk_box_pack_start(GTK_BOX(box), priv->label, FALSE, FALSE, 0);

    } else {
        box = gtk_vbox_new(FALSE, 10);
        gtk_container_add(GTK_CONTAINER(dialog->vbox), box);
        gtk_box_pack_start(GTK_BOX(box), priv->label, FALSE, FALSE, 0);

        if (item) {
            gtk_box_pack_start(GTK_BOX(box), item, FALSE, FALSE, 0);
        }
    }
    
    gtk_widget_show_all(box);
}

/**
 * hildon_note_new_confirmation_add_buttons:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly.
 *   In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * @Varargs: Arguments pairs for new buttons(label and return value)
 * 
 * Create a new confirmation note. Confirmation note has a text, 
 * two buttons and infinite number of additional buttons and an icon.
 *
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_confirmation_add_buttons(GtkWindow * parent,
                                                    const gchar *
                                                    description, ...)
{
    va_list args;
    char *message = " ";
    int value;

    GtkWidget *conf_note =
        g_object_new(HILDON_TYPE_NOTE,
                     "note_type", HILDON_NOTE_CONFIRMATION_BUTTON_TYPE,
                     "description", description,
                     "icon", "qgn_note_confirm", /*HILDON_NOTE_CONFIRMATION_STOCK, */
                     NULL);

    g_return_val_if_fail(conf_note, FALSE);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(conf_note), parent);

    va_start(args, description);

    while (TRUE) {
        message = va_arg(args, char *);

        if (!message) {
            break;
        }
        value = va_arg(args, int);

        gtk_dialog_add_button(GTK_DIALOG(conf_note), message, value);
    }

    va_end(args);

    return conf_note;
}


/**
 * hildon_note_new_confirmation:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * 
 * Create a new confirmation note. Confirmation note has a text, 
 * two buttons and an default confirmation stock icon.
 *
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_confirmation(GtkWindow * parent,
                                        const gchar * description)
{
    return hildon_note_new_confirmation_with_icon_name
        (parent, description, "qgn_note_confirm"); /*HILDON_NOTE_CONFIRMATION_STOCK);*/
}


/**
 * hildon_note_new_confirmation_with_icon_stock:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * @stock_id: Icon to be displayed. If NULL, default icon is used.
 * 
 * Create a new confirmation note. Confirmation note has a text, 
 * two buttons and an icon.
 *
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_confirmation_with_icon_stock(GtkWindow * parent,
                                                        const gchar *
                                                        description,
                                                        const gchar *
                                                        stock_id)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_NOTE,
                                     "note_type",
                                     HILDON_NOTE_CONFIRMATION_TYPE,
                                     "description", description, "icon",
                                     stock_id, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    return dialog;
}

/**
 * hildon_note_new_confirmation_with_icon_name:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * @icon_name: Icon to be displayed. If NULL, default icon is used.
 * 
 * Create a new confirmation note. Confirmation note has a text, 
 * two buttons and an icon.
 *
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_confirmation_with_icon_name(GtkWindow * parent,
                                                        const gchar *
                                                        description,
                                                        const gchar *
                                                        icon_name)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_NOTE,
                                     "note_type",
                                     HILDON_NOTE_CONFIRMATION_TYPE,
                                     "description", description, "icon",
                                     icon_name, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    return dialog;
}

/**
 * hildon_note_new_information:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * 
 * Create a new information note. Information note has a text,
 * 'OK' labeled button and an icon.
 * 
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_information(GtkWindow * parent,
                                       const gchar * description)
{
    return hildon_note_new_information_with_icon_stock
        (parent, description, "qgn_note_info");
}

/**
 * hildon_note_new_information_with_icon_stock:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * @stock_id: Icon to be displayed. If NULL, default icon is used.
 * 
 * Create a new information note. Information note has a text,
 * 'OK' labeled button and an icon.
 * 
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_information_with_icon_stock(GtkWindow * parent,
                                                       const gchar *
                                                       description,
                                                       const gchar *
                                                       stock_id)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_NOTE,
                                     "note_type",
                                     HILDON_NOTE_INFORMATION_TYPE,
                                     "description", description,
                                     "icon", stock_id, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    return dialog;
}

/**
 * hildon_note_new_information_with_icon_name:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * @icon_name: Icon to be displayed. If NULL, default icon is used.
 * 
 * Create a new information note. Information note has a text,
 * 'OK' labeled button and an icon.
 * 
 * Return value: A #GtkWidget pointer of the note.
 */
GtkWidget *hildon_note_new_information_with_icon_name(GtkWindow * parent,
                                                       const gchar *
                                                       description,
                                                       const gchar *
                                                       icon_name)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_NOTE,
                                     "note_type",
                                     HILDON_NOTE_INFORMATION_THEME_TYPE,
                                     "description", description,
                                     "icon", icon_name, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    return dialog;
}

/**
 * hildon_note_new_information_with_icon_theme:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The message to confirm.
 * @icon: #GtkIconTheme icon to be displayed.
 * 
 * Create a new information note. Information note has a text,
 * 'OK' labeled button and an icon.
 * 
 * Return value: A #GtkWidget pointer of the note. 
 * If NULL, default icon is used.
 */
GtkWidget *hildon_note_new_information_with_icon_theme(GtkWindow *parent,
                                                       const gchar *description,
                                                       const gchar *icon)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_NOTE,
                                     "note_type",
                                     HILDON_NOTE_INFORMATION_THEME_TYPE,
                                     "description", description,
                                     "icon", icon, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    return dialog;
}

/**
 * hildon_note_new_cancel_with_progress_bar:
 * @parent: The parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked with
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: The action to cancel.
 * @progressbar: A pointer to #GtkProgressBar to be filled with the
 *   progressbar assigned to this note. Use this to set the fraction of
 *   progressbar done.
 *
 * Create a new cancel note with a progress bar. The note has a text,
 * 'Cancel' labeled button and a progress bar.
 *
 * Return value: A #GtkDialog. Use this to get rid of this note when you
 *   no longer need it.
 */
GtkWidget *hildon_note_new_cancel_with_progress_bar(GtkWindow * parent,
                                                    const gchar *
                                                    description,
                                                    GtkProgressBar *
                                                    progressbar)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_NOTE,
                                     "note_type",
                                     HILDON_NOTE_PROGRESSBAR_TYPE,
                                     "description", description,
                                     "progressbar",
                                     progressbar, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    return dialog;
}


/**
 * hildon_note_set_button_text:
 * @note: A #HildonNote
 * @text: Sets the button text and if there is two buttons in dialog, 
 *   the button texts will be &lt;text&gt;, "Cancel".  
 *
 * Sets the button text to be used by this hildon_note widget.
 */
void hildon_note_set_button_text(HildonNote * note, const gchar * text)
{
    HildonNotePrivate *priv;

    priv = HILDON_NOTE_GET_PRIVATE(HILDON_NOTE(note));
    if (priv->okButton) {
        gtk_button_set_label(GTK_BUTTON(priv->okButton), text);
        gtk_button_set_label(GTK_BUTTON(priv->cancelButton),
                             _("Ecdg_bd_confirmation_note_cancel"));
    } else {
        gtk_button_set_label(GTK_BUTTON(priv->cancelButton), text);
    }
}
