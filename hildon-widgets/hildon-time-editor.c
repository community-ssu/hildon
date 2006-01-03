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

/* HILDON DOC
 * @shortdesc: TimeEditor is a widget for setting, getting and showing a
 * time.
 * @longdesc: The Time Editor widget is used to enter the system time
 * (hours and minutes) in the Date/Time system plugin. It is a composite
 * widget consisting of two GtkEntry widgets that are placed next to each
 * other. The leftmost GtkEntry is used to enter the hours, and it accepts
 * the values 0--23, while the rightmost GtkEntry accepts values 0--59
 * and is used to set the minutes. Between the two GtkEntries there
 * is a label displaying a colon.
 * </para><para>
 * From the usability point of view, the GtkSpinbutton widget would
 * have been a better choice than the GtkEntry widgets, but it uses
 * floating point operations and is thus not acceptable in this
 * project.
 *
 * @seealso: #HildonDateEditor
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <pango/pango.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkentry.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkwidget.h>

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <langinfo.h>
#include <libintl.h>

#include <hildon-widgets/hildon-defines.h>
#include <hildon-widgets/hildon-time-editor.h>
#include <hildon-widgets/hildon-time-picker.h>
#include <hildon-widgets/gtk-infoprint.h> /* FIXME: broken include ? */
#include <hildon-widgets/hildon-input-mode-hint.h>
#include "hildon-composite-widget.h"
#include "hildon-date-editor.h"

#define _(String) dgettext(PACKAGE, String)

/* empty space on left and right side of a GtkEntry. Space needed
is 12, we add 4 extra pixels so that the arabic locale works
correctly. (With 12 only one digit would be shown in the entries).
*/
#define TIME_EDITOR_LBORDER         2
#define TIME_EDITOR_RBORDER         1
#define TIME_EDITOR_HEIGHT         28
#define TIME_EDITOR_CLOCK_BORDER    6
#define ENTRY_BORDER                2
#define ICON_WIDTH                 26
#define ICON_HEIGHT                26
#define ICON_PRESSED                4
#define ICON_NAME                  "qgn_widg_timedit"
#define ICON_SIZE                  "timepicker-size"
#define HILDON_TIME_EDITOR_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     HILDON_TYPE_TIME_EDITOR, HildonTimeEditorPrivate));
#define MIN_DURATION 0
#define MAX_DURATION (3600 * 99) + (60 * 59) + 59

/* Default values for properties */
/* FIXME:  _TIME_LOWER/UPPER_VALUEs aren't really used for anything. */
#define HILDON_TIME_EDITOR_TICKS_VALUE           0
#define HILDON_TIME_EDITOR_SHOW_SECONDS          TRUE
#define HILDON_TIME_EDITOR_DURATION_MODE         FALSE
#define HILDON_TIME_EDITOR_DURATION_LOWER_VALUE  0
#define HILDON_TIME_EDITOR_TIME_LOWER_VALUE      0
#define HILDON_TIME_EDITOR_TIME_UPPER_VALUE      (3600 * 23) + (60 * 59) + 59
#define HILDON_TIME_EDITOR_DURATION_UPPER_VALUE  (3600 * 99) + (60 * 59) + 59

#define HOURS_MAX_24   23
#define HOURS_MAX_12   12
#define HOURS_MIN_24    0
#define HOURS_MIN_12    1
#define MINUTES_MAX 59
#define SECONDS_MAX 59
#define MINUTES_MIN 0
#define SECONDS_MIN 0

static GtkContainerClass *parent_class;

typedef struct _HildonTimeEditorPrivate HildonTimeEditorPrivate;

enum
{
	PROP_TICKS = 1,
	PROP_DURATION_MODE,
	PROP_DURATION_MIN,
	PROP_DURATION_MAX,
	PROP_SHOW_SECONDS
};

typedef enum
{
  VALIDATION_ERROR, /* should never be returned, translates as system error */
  VALIDATION_OK,
  VALIDATION_DURATION_MAX,
  VALIDATION_DURATION_MIN,
  VALIDATION_TIME_HOURS,
  VALIDATION_TIME_MINUTES,
  VALIDATION_TIME_SECONDS,
  VALIDATION_LAST
} HildonValidation;

/***
 * Widget functions
 */

static void
hildon_time_editor_class_init(HildonTimeEditorClass * editor_class);

static void hildon_time_editor_init(HildonTimeEditor * editor);

static void hildon_time_editor_finalize(GObject * obj_self);

static void hildon_time_editor_set_property( GObject *object, guint param_id,
                                       const GValue *value, GParamSpec *pspec );
static void hildon_time_editor_get_property( GObject *object, guint param_id,
                                         GValue *value, GParamSpec *pspec );

static void
hildon_time_editor_forall(GtkContainer * container,
                          gboolean include_internals, GtkCallback callback,
                          gpointer callback_data);
                          
static void hildon_time_editor_destroy(GtkObject * self);

static void hildon_time_editor_add_style(void);

static void
set_widget_allocation(GtkWidget * widget, GtkAllocation * alloc,
                      GtkAllocation * allocation);

/***
 * Signal handlers
 */

static gboolean
hildon_time_editor_entry_focusout(GtkWidget * widget,
                                  GdkEventFocus * event, gpointer data);

static gboolean hildon_time_editor_entry_focusin(GtkWidget * widget,
                                                 GdkEventFocus * event, 
                                                 gpointer data);
static gboolean
hildon_time_editor_mnemonic_activate(GtkWidget * widget,
                                     gboolean group_cycling);

static gboolean
hildon_time_editor_ampm_clicked(GtkWidget * widget, GdkEventButton * event,
                                gpointer data);
static gboolean
hildon_time_editor_icon_clicked(GtkWidget * widget, gpointer data);

static gboolean
hildon_time_editor_entry_clicked(GtkWidget * widget,
                                 GdkEventButton * event, gpointer data);

static void
hildon_time_editor_entry_changed(GtkWidget * widget, gpointer user_data);


static void
hildon_time_editor_size_request(GtkWidget * widget,
                                GtkRequisition * requisition);

static void
hildon_time_editor_size_allocate(GtkWidget * widget,
                                 GtkAllocation * allocation);

static gboolean
hildon_time_editor_entry_keypress(GtkWidget * widget, GdkEventKey * event,
                                  gpointer data);

/***
 * Internal functions
 */

static gboolean hildon_time_editor_check_locale(HildonTimeEditor * editor);

static
void hildon_time_editor_tap_and_hold_setup(GtkWidget * widget,
                                           GtkWidget * menu,
                                           GtkCallback func,
                                           GtkWidgetTapAndHoldFlags flags);

static void
hildon_time_editor_get_max_values(HildonTimeEditor *editor,
        guint * max_hours, guint * min_hours,
        guint * max_minutes, guint * min_minutes,
        guint * max_seconds, guint * min_seconds);
        
static void
hildon_time_editor_validate (HildonTimeEditor *editor);

static HildonValidation
hildon_time_editor_validate_duration (HildonTimeEditor * editor, guint ticks);

static HildonValidation
hildon_time_editor_validate_time (HildonTimeEditor * editor,
                                  guint hours,
                                  guint minutes,
                                  guint seconds,
                                  gboolean mode_24h);

/***
 * Utility functions
 */
 
static void
convert_to_12h (guint *h, guint *m, guint *s, gboolean *am);
static void
convert_to_24h (guint *h, guint *m, guint *s, gboolean am);

static void ticks_to_time (guint ticks,
                           guint *hours,
                           guint *minutes,
                    guint *seconds);


struct _HildonTimeEditorPrivate {
    guint ticks;        /* Current duration in seconds */
    gchar *am_symbol;
    gchar *pm_symbol;

    GtkWidget *eventbox;        /* hold entries */
    GtkWidget *iconbutton;      /* button for icon */

    GtkWidget *h_entry;
    GtkWidget *m_entry;
    GtkWidget *s_entry;
    GtkWidget *label;   /* between h and m */
    GtkWidget *label2;  /* between m and s */
    GtkWidget *ampm;    /* label for showing am or pm */
    GtkWidget *icon;    /* label for showing am or pm */
    GtkWidget *frame;   /* frame around the entries */

    gboolean duration_mode;     /* In HildonDurationEditor mode */
    gboolean show_s;    /* show seconds */
    gboolean ampm_pos_after;    /* is the am/pm shown after others */
    gboolean clock_24h; /* whether to show a 24h clock */
    gboolean am;        /* TRUE == showing am, FALSE == pm */
    gboolean valid_value; /* If entry has an valid value. FIXME: unused */
    
    gboolean validated; /* If the current value has been validated */

    /* Duration editor ranges */
    guint duration_min;
    guint duration_max;
};

GType hildon_time_editor_get_type(void)
{
    static GType editor_type = 0;

    if (!editor_type) {
        static const GTypeInfo editor_info = {
            sizeof(HildonTimeEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_time_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonTimeEditor),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_time_editor_init,
        };
        editor_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                             "HildonTimeEditor",
                                             &editor_info, 0);
    }
    return editor_type;
}

static void hildon_time_editor_forall(GtkContainer * container,
                                      gboolean include_internals,
                                      GtkCallback callback,
                                      gpointer callback_data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;

    editor = HILDON_TIME_EDITOR(container);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    g_return_if_fail(container);
    g_return_if_fail(callback);

    if (!include_internals)
        return;


    /* widget that are always shown */
    (*callback) (priv->h_entry, callback_data);
    (*callback) (priv->m_entry, callback_data);
    (*callback) (priv->iconbutton, callback_data);
    (*callback) (priv->frame, callback_data);
    (*callback) (priv->label, callback_data);
    (*callback) (priv->label2, callback_data);
    (*callback) (priv->s_entry, callback_data);
    (*callback) (priv->eventbox, callback_data);
}

static void hildon_time_editor_destroy(GtkObject * self)
{
    HildonTimeEditorPrivate *priv;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(self);

    if (priv->h_entry) {
        gtk_widget_unparent(priv->h_entry);
        priv->h_entry = NULL;
    }
    if (priv->m_entry) {
        gtk_widget_unparent(priv->m_entry);
        priv->m_entry = NULL;
    }
    if (priv->label) {
        gtk_widget_unparent(priv->label);
        priv->label = NULL;
    }
    if (priv->iconbutton) {
        gtk_widget_unparent(priv->iconbutton);
        priv->iconbutton = NULL;
    }
    if (priv->frame) {
        gtk_widget_unparent(priv->frame);
        priv->frame = NULL;
    }
    if (priv->eventbox) {
        gtk_widget_unparent(priv->eventbox);
        priv->eventbox = NULL;
        priv->ampm = NULL;
    }
    if (priv->label2) {
        gtk_widget_unparent(priv->label2);
        priv->label2 = NULL;
    }
    if (priv->s_entry) {
        gtk_widget_unparent(priv->s_entry);
        priv->s_entry = NULL;
    }
  
    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        GTK_OBJECT_CLASS(parent_class)->destroy(self);

}

static void
hildon_time_editor_class_init(HildonTimeEditorClass * editor_class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(editor_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(editor_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(editor_class);

    parent_class = g_type_class_peek_parent(editor_class);

    g_type_class_add_private(editor_class,
                             sizeof(HildonTimeEditorPrivate));

    object_class->get_property = hildon_time_editor_get_property;
    object_class->set_property = hildon_time_editor_set_property;


    widget_class->mnemonic_activate = hildon_time_editor_mnemonic_activate;
    widget_class->size_request = hildon_time_editor_size_request;
    widget_class->size_allocate = hildon_time_editor_size_allocate;
    widget_class->tap_and_hold_setup =
        hildon_time_editor_tap_and_hold_setup;
    widget_class->focus = hildon_composite_widget_focus;

    container_class->forall = hildon_time_editor_forall;
    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_time_editor_destroy;

    object_class->finalize = hildon_time_editor_finalize;

  /**
   * HildonTimeEditor:ticks:
   *
   * If editor is in duration mode, contains the duration seconds.
   * If not, contains seconds since midnight.
   */
  g_object_class_install_property( object_class, PROP_TICKS,
                                   g_param_spec_uint("ticks",
                                   "Duration value",
                                   "Current value of duration",
                                   0, G_MAXUINT,
                                   HILDON_TIME_EDITOR_TICKS_VALUE,
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonTimeEditor:show_seconds:
   *
   * Controls whether seconds are shown in the editor
   */
  g_object_class_install_property( object_class, PROP_SHOW_SECONDS,
                                   g_param_spec_boolean("show_seconds",
                                   "Show seconds property",
                                   "Controls whether the seconds are shown in the editor",
                                   HILDON_TIME_EDITOR_SHOW_SECONDS,
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonTimeEditor:duration_mode:
   *
   * Controls whether the TimeEditor is in duration mode
   */
  g_object_class_install_property( object_class, PROP_DURATION_MODE,
                                   g_param_spec_boolean("duration_mode",
                                   "Duration mode",
                                   "Controls whether the TimeEditor is in duration mode",
                                   HILDON_TIME_EDITOR_DURATION_MODE,
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonTimeEditor:duration_min:
   *
   * Minimum allowed duration value.
   */
  /* FIXME: min/max values should be MIN_DURATION/MAX_DURATION */
  g_object_class_install_property( object_class, PROP_DURATION_MIN,
                                   g_param_spec_uint("duration_min",
                                   "Minumum duration value",
                                   "Smallest possible duration value",
                                   0, G_MAXUINT,
                                   HILDON_TIME_EDITOR_DURATION_LOWER_VALUE,
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonTimeEditor:duration_max:
   *
   * Maximum allowed duration value.
   */
  g_object_class_install_property( object_class, PROP_DURATION_MAX,
                                   g_param_spec_uint("duration_max",
                                   "Maximum duration value",
                                   "Largest possible duration value",
                                   0, G_MAXUINT,
                                   HILDON_TIME_EDITOR_DURATION_UPPER_VALUE,
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

}

static
void hildon_time_editor_tap_and_hold_setup(GtkWidget * widget,
                                           GtkWidget * menu,
                                           GtkCallback func,
                                           GtkWidgetTapAndHoldFlags flags)
{
    HildonTimeEditorPrivate *priv = HILDON_TIME_EDITOR_GET_PRIVATE(widget);

    /* Forward this tap_and_hold_setup signal to all our child widgets */
    gtk_widget_tap_and_hold_setup(priv->h_entry, menu, func,
                                  GTK_TAP_AND_HOLD_NO_SIGNALS);
    gtk_widget_tap_and_hold_setup(priv->m_entry, menu, func,
                                  GTK_TAP_AND_HOLD_NO_SIGNALS);
    gtk_widget_tap_and_hold_setup(priv->s_entry, menu, func,
                                  GTK_TAP_AND_HOLD_NO_SIGNALS);
    gtk_widget_tap_and_hold_setup(priv->eventbox, menu, func,
                                  GTK_TAP_AND_HOLD_NO_SIGNALS);
    gtk_widget_tap_and_hold_setup(priv->iconbutton, menu, func,
                                  GTK_TAP_AND_HOLD_NONE);
}

static void hildon_time_editor_init(HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;
    guint ticks = 0;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    gtk_widget_push_composite_child();
    
    /* Setup defaults and create widgets */
    priv->ticks = 0;
    priv->show_s = FALSE;
    priv->ampm_pos_after = TRUE;
    priv->clock_24h = TRUE;
    priv->duration_mode = FALSE;
    priv->iconbutton = gtk_button_new();
    priv->h_entry = gtk_entry_new();
    priv->m_entry = gtk_entry_new();
    priv->s_entry = gtk_entry_new();
    priv->ampm = gtk_label_new(NULL);
    priv->label = gtk_label_new(_("Ecdg_ti_time_editor_separator"));
    priv->label2 = gtk_label_new(_("Ecdg_ti_time_editor_separator"));
    priv->icon = gtk_image_new_from_icon_name(ICON_NAME, HILDON_ICON_SIZE_26);
    priv->frame = gtk_frame_new(NULL);
    priv->eventbox = gtk_event_box_new();
    priv->valid_value = TRUE;
    priv->validated = FALSE;
    
    GTK_WIDGET_SET_FLAGS(editor, GTK_NO_WINDOW);
    GTK_WIDGET_UNSET_FLAGS(priv->iconbutton, GTK_CAN_FOCUS);
    
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(priv->eventbox), FALSE);

    gtk_container_set_border_width(GTK_CONTAINER(priv->frame), 0);

    gtk_container_add(GTK_CONTAINER(priv->iconbutton), priv->icon);
    gtk_container_add(GTK_CONTAINER(priv->eventbox), priv->ampm);

    /* No frames for entries, so that they all appear to be inside one
       long entry */
    gtk_entry_set_has_frame(GTK_ENTRY(priv->h_entry), FALSE);
    gtk_entry_set_has_frame(GTK_ENTRY(priv->m_entry), FALSE);
    gtk_entry_set_has_frame(GTK_ENTRY(priv->s_entry), FALSE);
    
    /* Set H:M:S entries to accept only numeric characters */
    g_object_set (G_OBJECT(priv->h_entry), "input-mode",
                  HILDON_INPUT_MODE_HINT_NUMERIC, NULL);
    g_object_set (G_OBJECT(priv->m_entry), "input-mode", 
                  HILDON_INPUT_MODE_HINT_NUMERIC, NULL);
    g_object_set (G_OBJECT(priv->s_entry), "input-mode",
                  HILDON_INPUT_MODE_HINT_NUMERIC, NULL);

    
    /* clicked signal for am/pm label */
    g_signal_connect(G_OBJECT(priv->eventbox), "button_press_event",
                     G_CALLBACK(hildon_time_editor_ampm_clicked), editor);

    /* clicked signal for icon */
    g_signal_connect(G_OBJECT(priv->iconbutton), "clicked",
                     G_CALLBACK(hildon_time_editor_icon_clicked), editor);

    /* clicked signal for hour entry */
    g_signal_connect(G_OBJECT(priv->h_entry), "button-release-event",
                     G_CALLBACK(hildon_time_editor_entry_clicked), editor);
    g_signal_connect(G_OBJECT(priv->h_entry), "focus-in-event",
                     G_CALLBACK(hildon_time_editor_entry_focusin), editor);

    /* clicked signal for minute entry */
    g_signal_connect(G_OBJECT(priv->m_entry), "button-release-event",
                     G_CALLBACK(hildon_time_editor_entry_clicked), editor);
    g_signal_connect(G_OBJECT(priv->m_entry), "focus-in-event",
                     G_CALLBACK(hildon_time_editor_entry_focusin), editor);

    /* clicked signal for second entry */
    g_signal_connect(G_OBJECT(priv->s_entry), "button-release-event",
                     G_CALLBACK(hildon_time_editor_entry_clicked), editor);
    g_signal_connect(G_OBJECT(priv->s_entry), "focus-in-event",
                     G_CALLBACK(hildon_time_editor_entry_focusin), editor);

    /* focus out events */
    g_signal_connect(G_OBJECT(priv->h_entry), "focus-out-event",
                     G_CALLBACK(hildon_time_editor_entry_focusout),
                     editor);
    g_signal_connect(G_OBJECT(priv->m_entry), "focus-out-event",
                     G_CALLBACK(hildon_time_editor_entry_focusout),
                     editor);
    g_signal_connect(G_OBJECT(priv->s_entry), "focus-out-event",
                     G_CALLBACK(hildon_time_editor_entry_focusout),
                     editor);

    /* key press events */
    g_signal_connect(G_OBJECT(priv->h_entry), "key-press-event",
                     G_CALLBACK(hildon_time_editor_entry_keypress),
                     editor);
    g_signal_connect(G_OBJECT(priv->m_entry), "key-press-event",
                     G_CALLBACK(hildon_time_editor_entry_keypress),
                     editor);
    g_signal_connect(G_OBJECT(priv->s_entry), "key-press-event",
                     G_CALLBACK(hildon_time_editor_entry_keypress),
                     editor);

    /* changed signal sets time */
    g_signal_connect_after (G_OBJECT(priv->h_entry), "changed",
                      G_CALLBACK (hildon_time_editor_entry_changed), 
                      editor);
    g_signal_connect_after (G_OBJECT(priv->m_entry), "changed",
                      G_CALLBACK (hildon_time_editor_entry_changed), 
                      editor);
    g_signal_connect_after (G_OBJECT(priv->s_entry), "changed",
                      G_CALLBACK (hildon_time_editor_entry_changed), 
                      editor);
                      
    /* Set ourself as the parent of all the widgets we created */
    gtk_widget_set_parent(priv->iconbutton, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->label, GTK_WIDGET(editor));

    gtk_widget_set_parent(priv->label2, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->s_entry, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->eventbox, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->m_entry, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->h_entry, GTK_WIDGET(editor));

    /* Show created widgets */
    gtk_widget_show(priv->h_entry);
    gtk_widget_show(priv->m_entry);
    gtk_widget_show_all(priv->iconbutton);
    gtk_widget_show(priv->label);

    gtk_widget_set_parent(priv->frame, GTK_WIDGET(editor));
    gtk_widget_show_all(priv->frame);

    /* Update AM/PM settings from locale */
    if (!hildon_time_editor_check_locale(editor)) {
        /* Using 24h clock */
        priv->clock_24h = FALSE;
        gtk_widget_show_all(priv->eventbox);
    }

    /* set the default time to current time. */
    {
        time_t tm;
        struct tm *stm;

        tm = time(NULL);
        stm = localtime(&tm);

        if (stm) {
            /* convert current time to ticks */
            ticks = stm->tm_hour * 3600;
            ticks = ticks + stm->tm_min * 60;
            ticks = ticks + stm->tm_sec;
        }
    }

    hildon_time_editor_set_ticks (editor, ticks);
    
    /* H:M:S fields all take exactly two characters */
    gtk_entry_set_max_length(GTK_ENTRY(priv->h_entry), 2);
    gtk_entry_set_width_chars(GTK_ENTRY(priv->h_entry), 2);
    gtk_entry_set_max_length(GTK_ENTRY(priv->m_entry), 2);
    gtk_entry_set_width_chars(GTK_ENTRY(priv->m_entry), 2);
    gtk_entry_set_max_length(GTK_ENTRY(priv->s_entry), 2);
    gtk_entry_set_width_chars(GTK_ENTRY(priv->s_entry), 2);

    /* Fix appearance */
    hildon_time_editor_add_style();
    gtk_widget_set_name(GTK_WIDGET(priv->iconbutton),
                        "hildon-time-editor-icon");
    
    gtk_widget_pop_composite_child();
}

static void hildon_time_editor_set_property (GObject *object, guint param_id,
                                      const GValue *value, GParamSpec *pspec)
{
  HildonTimeEditor *time_editor = HILDON_TIME_EDITOR(object);
  switch (param_id)
  {
    case PROP_TICKS:
      hildon_time_editor_set_ticks (time_editor, g_value_get_uint(value));
      break;
      
    case PROP_SHOW_SECONDS:
      hildon_time_editor_set_show_seconds (time_editor, g_value_get_boolean(value));
      break;

    case PROP_DURATION_MODE:
      hildon_time_editor_set_duration_mode (time_editor, g_value_get_boolean(value));
      break;

    case PROP_DURATION_MIN:
      hildon_time_editor_set_duration_min (time_editor, g_value_get_uint(value));
      break;
      
    case PROP_DURATION_MAX:
      hildon_time_editor_set_duration_max (time_editor, g_value_get_uint(value));
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static void hildon_time_editor_get_property (GObject *object, guint param_id,
                                            GValue *value, GParamSpec *pspec)
{
  HildonTimeEditor *time_editor = HILDON_TIME_EDITOR(object);
  switch (param_id)
  {

    case PROP_TICKS:
      g_value_set_uint (value, hildon_time_editor_get_ticks (time_editor));
      break;

    case PROP_SHOW_SECONDS:
      g_value_set_boolean (value, hildon_time_editor_get_show_seconds (time_editor));
      break;

    case PROP_DURATION_MODE:
      g_value_set_boolean (value, hildon_time_editor_get_duration_mode (time_editor));
      break;

    case PROP_DURATION_MIN:
      g_value_set_uint (value, hildon_time_editor_get_duration_min (time_editor));
      break;

    case PROP_DURATION_MAX:
      g_value_set_uint (value, hildon_time_editor_get_duration_max (time_editor));
      break;
      
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}


static gboolean
hildon_time_editor_mnemonic_activate( GtkWidget *widget,
                                      gboolean group_cycling)
{
  HildonTimeEditorPrivate *priv = HILDON_TIME_EDITOR_GET_PRIVATE(widget);

  /* Select hour field when mnemonic is activated
     FIXME: why? Hildon doesn't even support mnemonics */
  gtk_widget_grab_focus( priv->h_entry );
  gtk_editable_select_region(GTK_EDITABLE(priv->h_entry), 0, 2);
  return TRUE;
}

/**
 * hildon_time_editor_new:
 *
 * This function creates a new time editor. 
 *
 * Return value: pointer to a new #HildonTimeEditor widget.
 **/

GtkWidget *hildon_time_editor_new(void)
{
    return GTK_WIDGET(g_object_new(HILDON_TYPE_TIME_EDITOR, NULL));
}

static void hildon_time_editor_finalize(GObject * obj_self)
{
    HildonTimeEditorPrivate *priv = HILDON_TIME_EDITOR_GET_PRIVATE(obj_self);

    if (priv->am_symbol)
        g_free(priv->am_symbol);
    if (priv->pm_symbol)
        g_free(priv->pm_symbol);

    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);
}

static void hildon_time_editor_add_style(void)
{
    /* We want to make time editor appear like it's inside one GtkEntry
       field. */
    gtk_rc_parse_string("  style \"hildon-time-editor-icon\" {"
                        "    GtkButton::default_border = { 0, 0, 0, 0 }"
                        "    xthickness = 0"
                        "    ythickness = 0"
                        "    engine \"pixmap\" {"
                        "      image {"
                        "        function = BOX"
                        "      }"
                        "    }"
                        "  }"
                        "  widget \"*.hildon-time-editor-icon\""
                        "    style \"hildon-time-editor-icon\"");
}

/* Convert ticks to H:M:S. Ticks = seconds since 00:00:00. */
static void ticks_to_time (guint ticks,
                           guint *hours,
                           guint *minutes,
                           guint *seconds)
{
  guint h,m,s;

  h = ticks / 3600;
  m = (ticks - h*3600) / 60;
  s = ticks - h*3600 - m*60;

  *hours = h;
  *minutes = m;
  *seconds = s;

}

/**
 * hildon_time_editor_set_ticks:
 * @self: the @HildonTimeEditor widget.
 * @ticks: The duration to set, in seconds.
 *
 * Sets the current duration in seconds. This means seconds from
 * midnight, if not in duration mode. In case of any errors, it tries
 * to fix it.
 * 
 **/

void hildon_time_editor_set_ticks (HildonTimeEditor * editor,
                                   guint ticks)
{
    HildonTimeEditorPrivate *priv;
    HildonValidation validation;
    guint h = 1;
    guint m = 0;
    guint s = 0;
    gchar hours[3]   = "01";
    gchar minutes[3] = "00";
    gchar seconds[3] = "00";

    g_return_if_fail(editor);
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* Validate the value if it is not already validated.
       FIXME: We're changing the ticks, it can't have been validated yet!
       This code is currently validating the previously set ticks, which is
       wrong. */
    if (!priv->validated) {
        if (priv->duration_mode) {
            /* Validate duration. If it's too low or too high, set it to
               min/max value. */
            validation = hildon_time_editor_validate_duration (editor, ticks);
            switch(validation) {
                case VALIDATION_DURATION_MIN:
                    priv->ticks = priv->duration_min;
                    break;
                case VALIDATION_DURATION_MAX:
                    priv->ticks = priv->duration_max;
                    break;
                default:
                    priv->ticks = ticks;
                    break;
            }
        } else {
            /* Check if given time is in valid range. FIXME: Since we're
               converting to H:M:S from ticks, only hours can be too large.
               We're also just duplicating the check from validate_time()
               here, so the call is pretty much useless. */
            ticks_to_time (ticks, &h, &m, &s);
            validation = hildon_time_editor_validate_time (editor, h, m, s, TRUE);
            switch (validation) {
                case VALIDATION_TIME_HOURS:
                    /* ticks value was too large. Set hour to maximum allowed.
                       FIXME: The maximum hour is still 23 here even with 12h
                       clock. The hour is converted to AM/PM value later. */
                    if (priv->clock_24h) {
                        if (h > HOURS_MAX_24) {
                            h = HOURS_MAX_24;
                        } else if (h < HOURS_MIN_24) {
                            h = HOURS_MIN_24;
                        }
                    } else {
                        if (h > HOURS_MAX_12) {
                            h = HOURS_MAX_12;
                        } else if (h < HOURS_MIN_12) {
                            h = HOURS_MIN_12;
                        }
                    }
                    priv->ticks = (3600 * h) + (60 * m) + s;
                    break;
                case VALIDATION_TIME_MINUTES:
                    /* FIXME: we never get here */
                    if (m > MINUTES_MAX) {
                        m = MINUTES_MAX;
                    } else if (m < MINUTES_MIN) {
                        m = MINUTES_MIN;
                    }
                    break;
                case VALIDATION_TIME_SECONDS:
                    /* FIXME: we never get here */
                    if (s > SECONDS_MAX) {
                        s = SECONDS_MAX;
                    } else if (s < SECONDS_MIN) {
                        s = SECONDS_MIN;
                    }
                    priv->ticks = (3600 * h) + (60 * m) + s;
                    break;
                default:
                    priv->ticks = ticks;
                    break;
            }
        }
    } else {
      priv->ticks = ticks;
    }
    
    /* ticks may have been modified, fix H:M:S variables.
       FIXME: once is enough.. */
    ticks_to_time (priv->ticks, &h, &m, &s);
    
    ticks_to_time (priv->ticks, &h, &m, &s);
    
    if (!priv->clock_24h && !priv->duration_mode)
      {
        /* Convert 24h H:M:S values to 12h mode, and update AM/PM state */
        convert_to_12h (&h, &m, &s, &priv->am);
      }

    /* Set H:M:S values to entries */
    g_snprintf(hours,   3, "%02u", h);
    g_snprintf(minutes, 3, "%02u", m);
    g_snprintf(seconds, 3, "%02u", s);
    gtk_entry_set_text(GTK_ENTRY(priv->h_entry), hours);
    gtk_entry_set_text(GTK_ENTRY(priv->m_entry), minutes);
    gtk_entry_set_text(GTK_ENTRY(priv->s_entry), seconds);
	
    priv->valid_value = TRUE;
    priv->validated = FALSE;

    /* Update AM/PM label in case we're in 12h mode */
    gtk_label_set_label(GTK_LABEL(priv->ampm), priv->am ? priv->am_symbol :
                        priv->pm_symbol);
    
    g_object_notify (G_OBJECT (editor), "ticks");
}

/**
 * hildon_time_editor_get_ticks:
 * @self: the @HildonTimeEditor widget.
 *
 * This function returns the current duration, in seconds.
 * This means seconds from midnight, if not in duration mode.
 * 
 * Return value: Current duration in seconds. 
 **/
 
guint hildon_time_editor_get_ticks (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail(editor, 0);
    g_return_val_if_fail(HILDON_IS_TIME_EDITOR(editor), 0);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);
    
    return (priv->ticks);
}

/**
 * hildon_time_editor_set_show_seconds:
 * @editor: The #HildonTimeEditor.
 * @enable: Enable or disable showing of seconds.
 *
 * This function shows or hides the seconds field.
 *
 **/

void hildon_time_editor_set_show_seconds (HildonTimeEditor * editor,
                                        gboolean show_seconds)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(editor);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* show/hide seconds field and its ':' label if the value changed. */
    if (!priv->show_s && show_seconds) {
        priv->show_s = TRUE;

        gtk_widget_show(priv->s_entry);
        gtk_widget_show(priv->label2);
        
    } else if (priv->show_s && !show_seconds) {

        gtk_widget_hide(priv->s_entry);
        gtk_widget_hide(priv->label2);

        priv->show_s = FALSE;
    } else
        return;

    /* Our width changed, do a resize. */
    gtk_widget_queue_resize(GTK_WIDGET(editor));
    
    g_object_notify (G_OBJECT (editor), "show_seconds");
}

/**
 * hildon_time_editor_get_show_seconds:
 * @self: the @HildonTimeEditor widget.
 *
 * This function returns a boolean indicating the visibility of
 * seconds in the @HildonTimeEditor
 *
 * Return value: TRUE if the seconds are visible. 
 **/

gboolean hildon_time_editor_get_show_seconds (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_TIME_EDITOR (editor), FALSE);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    return (priv->show_s);
}

/**
 * hildon_time_editor_set_duration_mode:
 * @editor: The #HildonTimeEditor.
 * @enable: Enable or disable duration editor mode
 *
 * This function sets the duration editor mode in which the maximum hours
 * is 99 and the #HildonTimePicker is disabled.
 *
 **/
 
void hildon_time_editor_set_duration_mode (HildonTimeEditor * editor,
                                         gboolean duration_mode)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(editor);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if (duration_mode && !priv->duration_mode) {
        /* Switch to duration editor mode. */
        priv->duration_mode = duration_mode;
        /* FIXME: Why do we reset the duration range here? */
        hildon_time_editor_set_duration_range(editor, MIN_DURATION,
                                              MAX_DURATION);

        /* There's no AM/PM label or time picker icon in duration mode.
           Make sure they're hidden. */
        if (!priv->clock_24h)
            gtk_widget_hide(GTK_WIDGET(priv->ampm));

        gtk_widget_hide(GTK_WIDGET(priv->eventbox));
        gtk_widget_hide(GTK_WIDGET(priv->icon));

        /* Duration mode has seconds by default. */
        hildon_time_editor_set_show_seconds(editor, TRUE);
    }
    else if (!duration_mode && priv->duration_mode) {
        /* Switch to time editor mode. */
        guint ticks;
        time_t tm;
        struct tm *stm;

        priv->duration_mode = duration_mode;

        /* Make sure AM/PM label and time picker icons are visible if needed */
        if (!priv->clock_24h)
            gtk_widget_show(GTK_WIDGET(priv->ampm));

        gtk_widget_show(GTK_WIDGET(priv->eventbox));
        gtk_widget_show(GTK_WIDGET(priv->icon));
        

	/* Reset the ticks to current time. Anything set in duration mode
         * is bound to be invalid or useless in time mode.
         */
        
        tm = time(NULL);
        stm = localtime(&tm);

        /* FIXME: stm is practically always non-NULL. Just do
           guint ticks = 0 above */
        ticks = HILDON_TIME_EDITOR_TIME_LOWER_VALUE;
        
        if (stm) {
            ticks = stm->tm_hour * 3600;
            ticks = ticks + stm->tm_min * 60;
            ticks = ticks + stm->tm_sec;
        }
        
        hildon_time_editor_set_ticks (editor, ticks);
        
    }
    /* Our width may have changed, do a resize. */
    gtk_widget_queue_resize(GTK_WIDGET(editor));
    
    g_object_notify (G_OBJECT (editor), "duration_mode");
}

/**
 * hildon_time_editor_get_duration_mode:
 * @self: the @HildonTimeEditor widget.
 *
 * This function returns a boolean indicating whether the @HildonTimeEditor
 * is in the duration mode.
 * 
 * Return value: TRUE if the @HildonTimeEditor is in duration mode. 
 **/

gboolean hildon_time_editor_get_duration_mode (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_TIME_EDITOR (editor), FALSE);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    return (priv->duration_mode);
}

/**
 * hildon_time_editor_set_duration_min:
 * @self: the @HildonTimeEditor widget.
 * @duration_min: Mimimum allowed duration.
 *
 * Sets the minimum allowed duration for the duration mode.
 * Note: Has no effect in time mode
 **/

void hildon_time_editor_set_duration_min (HildonTimeEditor * editor,
                                          guint duration_min)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(editor);
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    g_return_if_fail(duration_min >= MIN_DURATION);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if( !priv->duration_mode )
      return;

    priv->duration_min = duration_min;
    
    /* Clamp the current value to the minimum if necessary */
    if (priv->ticks < duration_min)
    {
        hildon_time_editor_set_ticks (editor, duration_min);
    }
    
    g_object_notify (G_OBJECT (editor), "duration_min");
}

/**
 * hildon_time_editor_get_duration_min:
 * @self: the @HildonTimeEditor widget.
 *
 * This function returns the smallest duration the @HildonTimeEditor
 * allows in the duration mode.
 * 
 * Return value: Mimimum allowed duration in seconds. 
 **/
 
guint hildon_time_editor_get_duration_min (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail(editor, 0);
    g_return_val_if_fail(HILDON_IS_TIME_EDITOR(editor), 0);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if( !priv->duration_mode )
      return (0);

    return (priv->duration_min);
}

/**
 * hildon_time_editor_set_duration_max:
 * @self: the @HildonTimeEditor widget.
 * @duration_min: Maximum allowed duration in seconds.
 *
 * Sets the maximum allowed duration in seconds for the duration mode.
 * Note: Has no effect in time mode
 * 
 **/
 
void hildon_time_editor_set_duration_max (HildonTimeEditor * editor,
                                          guint duration_max)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(editor);
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    g_return_if_fail(duration_max <= MAX_DURATION);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if( !priv->duration_mode )
      return;

    priv->duration_max = duration_max;
    
    /* Clamp the current value to the maximum if necessary */
    if (priv->ticks > duration_max)
    {
        hildon_time_editor_set_ticks (editor, duration_max);
    }
    
    g_object_notify (G_OBJECT (editor), "duration_max");
}

/**
 * hildon_time_editor_get_duration_max:
 * @self: the @HildonTimeEditor widget.
 *
 * This function returns the longest duration the @HildonTimeEditor
 * allows in the duration mode.
 * 
 * Return value: Maximum allowed duration in seconds. 
 **/
 
guint hildon_time_editor_get_duration_max (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail(editor, 0);
    g_return_val_if_fail(HILDON_IS_TIME_EDITOR(editor), 0);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if( !priv->duration_mode )
      return (0);

    return (priv->duration_max);
}


/**
 * hildon_time_editor_set_time:
 * @editor: the @HildonTimeEditor widget.
 * @hours: hours
 * @minutes: minutes
 * @seconds: seconds
 *
 * This function sets the time on an existing time editor. If the
 * time specified by the arguments is invalid, it's fixed.
 * The time is assumed to be in 24h format.
 *  
 **/

void hildon_time_editor_set_time(HildonTimeEditor * editor, guint hours,
                                 guint minutes, guint seconds)
{
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    hildon_time_editor_set_ticks (editor, hours * 3600 + minutes * 60 + seconds);

}

/**
 * hildon_time_editor_get_time:
 * @editor: the @HildonTimeEditor widget.
 * @hours: hours
 * @minutes: minutes
 * @seconds: seconds
 *
 * Gets the time of the @HildonTimeEditor widget. The time returned is
 * always in 24h format.
 **/

void hildon_time_editor_get_time(HildonTimeEditor * editor,
                                 guint * hours,
                                 guint * minutes, guint * seconds)
{
    HildonTimeEditorPrivate *priv;
    
    g_return_if_fail(editor);
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    ticks_to_time (hildon_time_editor_get_ticks (editor),
                   hours, minutes, seconds);

}

/**
 * hildon_time_editor_set_duration_range:
 * @editor: the @HildonTimeEditor widget.
 * @min_seconds: minimum allowed time in seconds
 * @max_seconds: maximum allowed time in seconds
 *
 * Sets the duration editor time range of the @HildonTimeEditor widget.
 **/

void hildon_time_editor_set_duration_range(HildonTimeEditor * editor,
                                           guint min_seconds,
                                           guint max_seconds)
{
    HildonTimeEditorPrivate *priv;
    guint tmp;
    
    g_return_if_fail(editor);
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);
    /* Swap values if reversed */
    if (min_seconds > max_seconds)
      {
        tmp = max_seconds;
        max_seconds = min_seconds;
        min_seconds = tmp;
      }
    
    hildon_time_editor_set_duration_max (editor, max_seconds);
    hildon_time_editor_set_duration_min (editor, min_seconds);

    if (priv->duration_mode) {
        /* Set minimum allowed value for duration editor.
           FIXME: Shouldn't it be changed only if it's not in range?
           If not, update the API documentation above. */
        hildon_time_editor_set_ticks(editor, min_seconds);
    }
}

/**
 * hildon_time_editor_get_duration_range:
 * @editor: the @HildonTimeEditor widget.
 * @min_seconds: pointer to guint
 * @max_seconds: pointer to guint
 *
 * Gets the duration editor time range of the @HildonTimeEditor widget.
 **/

void hildon_time_editor_get_duration_range(HildonTimeEditor * editor,
                                           guint * min_seconds,
                                           guint * max_seconds)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(editor);
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    *min_seconds = priv->duration_min;
    *max_seconds = priv->duration_max;
}

static gboolean hildon_time_editor_check_locale(HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;
    gchar *t_fm;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* Update AM/PM symbols */
    priv->am_symbol = g_strdup(nl_langinfo(AM_STR));
    priv->pm_symbol = g_strdup(nl_langinfo(PM_STR));

    if (!strcmp(priv->am_symbol, ""))
        return TRUE;
    else {
        /* 12h clock mode. Check if AM/PM should be before or after time.
           %p is the AM/PM string, so we assume that if the format string
           begins with %p it's in the beginning, and in any other case it's
           in the end (although that's not necessarily the case).
           FIXME: no need to g_strdup() here. */
        t_fm = g_strdup(nl_langinfo(T_FMT_AMPM));
        if (!strncmp(t_fm, "%p", 2))
            priv->ampm_pos_after = FALSE;

        /* We want to show AM/PM in lowercase. FIXME: leaks memory. */
        priv->am_symbol =
            g_ascii_strdown((const gchar *) priv->am_symbol, -1);
        priv->pm_symbol =
            g_ascii_strdown((const gchar *) priv->pm_symbol, -1);
        g_free(t_fm);
        return FALSE;
    }
}

static gboolean hildon_time_editor_entry_focusin(GtkWidget * widget,
                                                 GdkEventFocus * event, 
                                                 gpointer data)
{
    /* If we were trying to move away from a field with invalid value,
       we get moved back to it. Here we want to select the text in the field.
       The !button check checks that the entry wasn't focused with a mouse
       click.

       The selection happens temporarily if we got here with left/right
       keys, but it gets immediately unselected within same call due to some
       inner entry/clipboard magic. */
    if (!GTK_ENTRY(widget)->button)
        gtk_editable_select_region(GTK_EDITABLE(widget), 0, 2);

    return FALSE;
}


/* Update ticks from current H:M:S entries. If they're invalid, show an
   infoprint and update the fields unless they're empty. */
void
hildon_time_editor_validate (HildonTimeEditor *editor)
{
    guint max_hours = 0;
    guint min_hours = 0;
    guint max_minutes = 0;
    guint min_minutes = 0;
    guint max_seconds = 0;
    guint min_seconds = 0;

    HildonTimeEditorPrivate *priv;
    HildonValidation validation;

    GtkWindow *window;
    guint h,m,s;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    window = GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (editor),
                                                  GTK_TYPE_WINDOW));

    hildon_time_editor_get_max_values(editor, 
            &max_hours, &min_hours, 
            &max_minutes, &min_minutes, 
            &max_seconds, &min_seconds);
            
    /* No empty values thank you */
    if (strlen(GTK_ENTRY(priv->h_entry)->text) == 0)
      {
        /* Empty hour field */
        if (!priv->duration_mode)
          {
            if (priv->clock_24h)
              {
                gtk_infoprintf(window,
                               _("Ckct_ib_set_a_value_within_range"),
                               HOURS_MIN_24, HOURS_MAX_24);
              }
            else
              {
                gtk_infoprintf(window,
                               _("Ckct_ib_set_a_value_within_range"),
                               HOURS_MIN_12, HOURS_MAX_12);
              }
          }
        else
          {
            gtk_infoprintf(window,
                           _("Ckct_ib_set_a_value_within_range"),
                           min_hours, max_hours);
          }
        /* FIXME: No point in setting the ticks to current value. */
        hildon_time_editor_set_ticks (editor, priv->ticks);
        gtk_widget_grab_focus (priv->h_entry);
      }
    else if (strlen(GTK_ENTRY(priv->m_entry)->text) == 0)
      {
        /* Empty minute field */
        if (!priv->duration_mode)
          {
            gtk_infoprintf(window,
                           _("Ckct_ib_set_a_value_within_range"),
                           MINUTES_MIN, MINUTES_MAX);
          }
        else
          {
            gtk_infoprintf(window,
                           _("Ckct_ib_set_a_value_within_range"),
                           min_minutes, max_minutes);
          }
        /* FIXME: No point in setting the ticks to current value. */
        hildon_time_editor_set_ticks (editor, priv->ticks);
        gtk_widget_grab_focus (priv->m_entry);
      }
    else if (strlen(GTK_ENTRY(priv->s_entry)->text) == 0)
      {
        /* Empty seconds field */
        if (!priv->duration_mode)
          {
            gtk_infoprintf(window,
                           _("Ckct_ib_set_a_value_within_range"),
                           SECONDS_MIN, SECONDS_MAX);
          }
        else
          {
            gtk_infoprintf(window,
                           _("Ckct_ib_set_a_value_within_range"),
                           min_seconds, max_seconds);
          }
        /* FIXME: No point in setting the ticks to current value. */
        hildon_time_editor_set_ticks (editor, priv->ticks);
        gtk_widget_grab_focus (priv->s_entry);
      }
    else 
      {

        h = (guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->h_entry)));
        m = (guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->m_entry)));
        s = (guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->s_entry)));
      
        if (priv->duration_mode)
          {
            /* Ensure that duration is in valid range. If it's not, just set
               it to its min/max value. Too large minute and second values are
               also just wrapped up. */
            validation = hildon_time_editor_validate_duration (editor,
                                                               h*3600 + m*60 + s);
          
            switch (validation)
              {
                case VALIDATION_DURATION_MIN:
                  gtk_infoprintf(window,
                                 _("Ckct_ib_min_allowed_duration_hts"),
                                 min_hours, min_minutes, min_seconds);
                  hildon_time_editor_set_ticks (editor, priv->duration_min);
                  break;
                case VALIDATION_DURATION_MAX:
                  gtk_infoprintf(window,
                                 _("Ckct_ib_max_allowed_duration_hts"),
                                 max_hours, max_minutes, max_seconds);
                  hildon_time_editor_set_ticks (editor, priv->duration_max);
                  break;
                default:
                  hildon_time_editor_set_ticks (editor, h*3600 + m*60 + s);
                  break;
              }
          }
        else
          {
            /* Validate H:M:S values. FIXME: More than one field can be broken,
               should we update them? Although maybe not needed since focus is
               moved back to the first broken field, and next validation moves
               to next field.. */
            validation = hildon_time_editor_validate_time (editor, h, m, s, priv->clock_24h);
            switch (validation)
              {
                case VALIDATION_TIME_HOURS:
                  /* At least hour field is too large. */
                  if (priv->clock_24h)
                    {
                      if (h > HOURS_MAX_24)
                        {
                          gtk_infoprintf(window,
                                         _("Ckct_ib_maximum_value"),
                                         HOURS_MAX_24);
                          h = HOURS_MAX_24;
                        }
                      else
                        {
                          gtk_infoprintf(window,
                                         _("Ckct_ib_minimum_value"),
                                         HOURS_MIN_24);
                          h = HOURS_MIN_24;
                        }
                    }
                  else
                    {
                      if (h > HOURS_MAX_12)
                        {
                          gtk_infoprintf(window,
                                         _("Ckct_ib_maximum_value"),
                                         HOURS_MAX_12);
                          h = HOURS_MAX_12;
                        }
                      else
                        {
                          gtk_infoprintf(window,
                                         _("Ckct_ib_minimum_value"),
                                         HOURS_MIN_12);
                          h = HOURS_MIN_12;
                        }
                    }
                  if (!priv->clock_24h)
                    convert_to_24h (&h, &m, &s, priv->am);
                  hildon_time_editor_set_time (editor, h, m, s);
                  /* Move focus back to hour field */
                  gtk_widget_grab_focus (priv->h_entry);
                  gtk_editable_select_region(GTK_EDITABLE(priv->h_entry), 0, 2);
                  break;
                case VALIDATION_TIME_MINUTES:
                  if (m > MINUTES_MAX)
                    {
                      gtk_infoprintf(window,
                                     _("Ckct_ib_maximum_value"),
                                     MINUTES_MAX);
                      m = MINUTES_MAX;
                    }
                  else
                    {
                      gtk_infoprintf(window,
                                     _("Ckct_ib_minimum_value"),
                                     MINUTES_MIN);
                      m = MINUTES_MIN;
                    }
                  if (!priv->clock_24h)
                    convert_to_24h (&h, &m, &s, priv->am);
                  hildon_time_editor_set_time (editor, h, m, s);
                  /* Move focus back to minute field */
                  gtk_widget_grab_focus (priv->m_entry);
                  gtk_editable_select_region(GTK_EDITABLE(priv->m_entry), 0, 2);
                  break;
                case VALIDATION_TIME_SECONDS:
                  if (s > SECONDS_MAX)
                    {
                      gtk_infoprintf(window,
                                     _("Ckct_ib_maximum_value"),
                                     SECONDS_MAX);
                      s = SECONDS_MAX;
                    }
                  else
                    {
                      gtk_infoprintf(window,
                                     _("Ckct_ib_minimum_value"),
                                     SECONDS_MIN);
                      s = SECONDS_MIN;
                    }
                  if (!priv->clock_24h)
                    convert_to_24h (&h, &m, &s, priv->am);
                  hildon_time_editor_set_time (editor, h, m, s);
                  /* Move focus back to second field */
                  gtk_widget_grab_focus (priv->s_entry);
                  gtk_editable_select_region(GTK_EDITABLE(priv->s_entry), 0, 2);
                  break;
                default:
                  /* Given time is valid. */
                  if (!priv->clock_24h)
                    {
                      convert_to_24h (&h, &m, &s, priv->am);
                    }
                  hildon_time_editor_set_time (editor, h, m, s);
                  break;
              }
          }
        
      }
}

static gboolean hildon_time_editor_entry_focusout(GtkWidget * widget,
                                                  GdkEventFocus * event,
                                                  gpointer data)
{
  HildonTimeEditor *editor;

  editor = HILDON_TIME_EDITOR(data);

  /* Validate the given time and update ticks. */
  hildon_time_editor_validate (editor);

  return FALSE;
}

static gboolean
hildon_time_editor_ampm_clicked(GtkWidget * widget,
                                GdkEventButton * event, gpointer data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv = NULL;

    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(data, FALSE);


    editor = HILDON_TIME_EDITOR(data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* First validate the given time and update ticks. */
    hildon_time_editor_validate (editor);

    /* Apply the AM/PM change by moving the current time by 12 hours */
    if (priv->am) {
      if (priv->ticks >= (12*3600))
        {
          /* FIXME: never reached */
          hildon_time_editor_set_ticks (editor, priv->ticks - 12*3600);
        } else 
        {
          /* 00:00 .. 11:59 -> 12:00 .. 23:59 */
          hildon_time_editor_set_ticks (editor, priv->ticks + 12*3600);
        }
    } else {
        /* 12:00 .. 23:59 -> 00:00 .. 11:59 */
        hildon_time_editor_set_ticks (editor, priv->ticks - 12*3600);
    }
    return FALSE;
}

static gboolean
hildon_time_editor_icon_clicked(GtkWidget * widget, gpointer data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;
    GtkWidget *picker;
    GtkWidget *parent;
    guint h, m, s, result;

    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(data, FALSE);

    editor = HILDON_TIME_EDITOR(data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* icon is passive in duration editor mode */
    if (priv->duration_mode)
        return FALSE;

    /* Launch HildonTimePicker dialog */
    parent = gtk_widget_get_ancestor(GTK_WIDGET(editor), GTK_TYPE_WINDOW);
    picker = hildon_time_picker_new(GTK_WINDOW(parent));

    hildon_time_editor_get_time(editor, &h, &m, &s);
    hildon_time_picker_set_time(HILDON_TIME_PICKER(picker), h, m);

    result = gtk_dialog_run(GTK_DIALOG(picker));
    switch (result) {
    case GTK_RESPONSE_OK:
    case GTK_RESPONSE_ACCEPT:
        /* Use the selected time */
        hildon_time_picker_get_time(HILDON_TIME_PICKER(picker), &h, &m);
        hildon_time_editor_set_time(editor, h, m, 0);
        break;
    default:
        break;
    }

    gtk_widget_destroy(picker);
    return FALSE;
}

static gboolean hildon_time_editor_entry_clicked(GtkWidget * widget,
                                                 GdkEventButton * event,
                                                 gpointer data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;

    editor = HILDON_TIME_EDITOR (data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE (editor);

    /* If the focus has been grabbed back before the "clicked"
     * signal gets processed, don't highlight the text.
     * This happens when input in one H:M:S field is invalid and we're
     * trying to move to another field. The focus moves back to the invalid
     * field.
     */
    if (gtk_widget_is_focus (widget))
        gtk_editable_select_region(GTK_EDITABLE(widget), 0, 2);

    return FALSE;
}

static void hildon_time_editor_size_request(GtkWidget * widget,
                                            GtkRequisition * requisition)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;
    GtkRequisition req;

    editor = HILDON_TIME_EDITOR(widget);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* Call size_request for each of our child widget. We don't use all of
       the return values. */
    gtk_widget_size_request(priv->frame, &req);
    /* FIXME: next line is useless */
    *requisition = req;

    /* Calculate our width. First reserve some space for borders */
    requisition->width = TIME_EDITOR_LBORDER + TIME_EDITOR_RBORDER;

    /* Add each child widget's width */
    gtk_widget_size_request(priv->h_entry, &req);
    requisition->width += req.width;

    gtk_widget_size_request(priv->m_entry, &req);
    requisition->width += req.width;

    gtk_widget_size_request(priv->label, &req);
    requisition->width += req.width;

    if (priv->iconbutton && GTK_WIDGET_VISIBLE(priv->iconbutton))
    {
        gtk_widget_size_request(priv->iconbutton, &req);
        /* Hardcoded icon button size.. */
        requisition->width += ICON_WIDTH + ICON_PRESSED + 
                              TIME_EDITOR_CLOCK_BORDER;
    }

    if (priv->show_s) {
        /* Add width for seconds entry + label */
        gtk_widget_size_request(priv->s_entry, &req);
        requisition->width += req.width;

        gtk_widget_size_request(priv->label2, &req);
        requisition->width += req.width;
    }

    if (!priv->clock_24h && !priv->duration_mode) {
        /* Add width for AM/PM label */
        gtk_widget_size_request(priv->eventbox, &req);
        requisition->width += req.width + 4;
    }

    /* Hardcoded height.. */
    requisition->height = TIME_EDITOR_HEIGHT + widget->style->ythickness * 2;
}

static void set_widget_allocation(GtkWidget * widget,
                                  GtkAllocation * alloc,
                                  GtkAllocation * allocation)
{
    GtkRequisition child_requisition;

    /* Get this widget's wanted width */
    gtk_widget_get_child_requisition(widget, &child_requisition);

    /* allocation = whole time editor's allocation.
       alloc = allocation for this one child widget. */
    if (allocation->width + allocation->x >
        alloc->x + child_requisition.width)
        /* We have enough space */
        alloc->width = child_requisition.width;
    else {
        /* Not enough space. Give it as much as there's left. If this isn't
           the rightmost child widget, the rest get zero width. */
        alloc->width = allocation->width - (alloc->x - allocation->x);
        if (alloc->width < 0)
            alloc->width = 0;
    }
    /* Allocate the widget and update alloc->x to contain the X position
       for the next child widget. */
    gtk_widget_size_allocate(widget, alloc);
    alloc->x += alloc->width;
}

static void hildon_time_editor_size_allocate(GtkWidget * widget,
                                             GtkAllocation * allocation)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;
    GtkAllocation alloc;
    GtkAllocation child_alloc;
    GtkRequisition child_requisition;
    gint mod_w = 0;

    editor = HILDON_TIME_EDITOR(widget);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    widget->allocation = *allocation;

    /* Set alloc to contain frame's size and position.
       Leave ythickness pixels of padding at top and bottom */
    alloc.y = widget->allocation.y + widget->style->ythickness;

    if (widget->allocation.height > (TIME_EDITOR_HEIGHT +
                                     widget->style->ythickness * 2)) {
        /* We have more vertical space than needed, move to center */
        alloc.height = TIME_EDITOR_HEIGHT;
        alloc.y +=
            (widget->allocation.height - TIME_EDITOR_HEIGHT) / 2;
    } else
        alloc.height =
            widget->allocation.height - widget->style->ythickness * 2;

    /* Make sure the height didn't get negative (even padding doesn't fit) */
    if (alloc.height < 0)
        alloc.height = 0;

    /* Get the whole widget's wanted width and set our X position */
    gtk_widget_get_child_requisition(widget, &child_requisition);
    if (allocation->width > child_requisition.width) {
        /* We have more horizontal space than needed, move to center */
        mod_w = (allocation->width - child_requisition.width) / 2;
        alloc.x = allocation->x + mod_w;
    } else
        alloc.x = allocation->x;


    /* FIXME: below check is always true */
    if (priv->frame && GTK_WIDGET_VISIBLE(priv->frame))
    {
        /* Frame's width is widget's full width minus the space reserved for
           time picker icon and borders. */
        alloc.width = child_requisition.width -
            TIME_EDITOR_CLOCK_BORDER - ICON_WIDTH - ICON_PRESSED;
        gtk_widget_size_allocate(priv->frame, &alloc);
    }

    /* FIXME: below check is always true (priv->icon may be hidden though) */
    if (priv->iconbutton && GTK_WIDGET_VISIBLE(priv->iconbutton)) {
        gtk_widget_get_child_requisition(priv->iconbutton, 
                &child_requisition);

        /* Place icon button after time fields and some border */
        child_alloc.x = alloc.x + alloc.width + TIME_EDITOR_CLOCK_BORDER;

        /* Make the icon centered vertically */
        if(alloc.height > ICON_HEIGHT)
            child_alloc.y = alloc.y + (alloc.height - ICON_HEIGHT) / 2 -1;
        else
            child_alloc.y = alloc.y;
        child_alloc.height = ICON_HEIGHT + ICON_PRESSED / 2;
        child_alloc.width = ICON_WIDTH + ICON_PRESSED;
        gtk_widget_size_allocate(priv->iconbutton, &child_alloc);
    }

    /* Allocate widgets appearing inside frame.
       child_alloc.x keeps the position where the next widget should placed.
       Widget heights start from a constant, but they change a bit:

        - Labels already add ythickness internally, so we give labels that
          much more height.
        - For entries we want ENTRY_BORDER amount of padding at top and bottom.
    */
    child_alloc.x = alloc.x + TIME_EDITOR_LBORDER;
    child_alloc.y = alloc.y;
    child_alloc.height = TIME_EDITOR_HEIGHT;
    child_alloc.width = -1;

    /* Start with AM/PM label when it comes before time */
    if (!priv->duration_mode) {
        if (!priv->clock_24h && !priv->ampm_pos_after)
          {
            /* FIXME: ythickness modification missing from here */
            set_widget_allocation(priv->eventbox, &child_alloc,
                    &widget->allocation);
          }
    }

    /* FIXME: below check is always true */
    if (priv->h_entry && GTK_WIDGET_VISIBLE(priv->h_entry))
      {
        /* Allocate hours entry. */
        child_alloc.y += ENTRY_BORDER;
        child_alloc.height -= ENTRY_BORDER * 2;
        set_widget_allocation(priv->h_entry, &child_alloc,
                &widget->allocation);
        child_alloc.y -= ENTRY_BORDER;
        child_alloc.height += ENTRY_BORDER * 2;
      }

    /* FIXME: below check is always true */
    if (priv->label && GTK_WIDGET_VISIBLE(priv->label))
      {
        /* Allocate the label between H:M. */
        child_alloc.y -= widget->style->ythickness;
        set_widget_allocation(priv->label, &child_alloc,
                &widget->allocation);
        child_alloc.y += widget->style->ythickness;
      }
    /* FIXME: below check is always true */
    if (priv->m_entry && GTK_WIDGET_VISIBLE(priv->m_entry))
      {
        /* Allocate minutes entry. */
        child_alloc.y += ENTRY_BORDER;
        child_alloc.height -= ENTRY_BORDER * 2;
        set_widget_allocation(priv->m_entry, &child_alloc,
                &widget->allocation);
        child_alloc.y -= ENTRY_BORDER;
        child_alloc.height += ENTRY_BORDER * 2;
      }
    
    if (priv->show_s) {
        /* Seconds are shown */
        /* FIXME: below check is always true */
        if (priv->label2)
          {
            /* Allocate the label between M:S. */
            child_alloc.y -= widget->style->ythickness;
            set_widget_allocation(priv->label2, &child_alloc,
                    &widget->allocation);
            child_alloc.y += widget->style->ythickness;
          }

        /* FIXME: below check is always true */
        if (priv->s_entry)
          {
            /* Allocate seconds entry. */
            child_alloc.y += ENTRY_BORDER;
            child_alloc.height -= ENTRY_BORDER * 2;
            set_widget_allocation(priv->s_entry, &child_alloc,
                    &widget->allocation);
            child_alloc.y -= ENTRY_BORDER;
            child_alloc.height += ENTRY_BORDER * 2;
          }
    }

    /* Finish with AM/PM label when it comes after time */
    if (!priv->duration_mode) {
        if (!priv->clock_24h && priv->ampm_pos_after)
          {
            child_alloc.y -= widget->style->ythickness;
            set_widget_allocation(priv->eventbox, &child_alloc,
                    &widget->allocation);
            child_alloc.y += widget->style->ythickness;
          }
    }
}

static gboolean hildon_time_editor_entry_keypress(GtkWidget * widget,
                                                  GdkEventKey * event,
                                                  gpointer data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;
    gint pos;

    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(event, FALSE);
    g_return_val_if_fail(data, FALSE);

    editor = HILDON_TIME_EDITOR(data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    pos = gtk_editable_get_position(GTK_EDITABLE(widget));

    if (event->keyval == GDK_Return) {
        /* Return key popups up time picker dialog. Visually it looks as if
           the time picker icon was clicked. Before opening the time picker
           the fields are first validated and fixed. */
        hildon_time_editor_validate (editor);
        _gtk_button_set_depressed(GTK_BUTTON(priv->iconbutton), TRUE);
        hildon_time_editor_icon_clicked(widget, data);
        _gtk_button_set_depressed(GTK_BUTTON(priv->iconbutton), FALSE);
        return TRUE;
    }
    
    if  (event->keyval == GDK_KP_Enter)
        return FALSE;

    /* Left/right keys may move focus to next/previous entry (see below).
       Here we're disabling wrapping from leftmost entry to rightmost, and
       vice versa. FIXME: Just delete this code, and the wrapping handling
       code from below.. */
    if (event->keyval == GDK_KP_Left || event->keyval == GDK_Left) {
        if (pos == 0 && widget == priv->h_entry) {
            return TRUE;
        }
    }

    if (event->keyval == GDK_KP_Right || event->keyval == GDK_Right) {
        if (pos >= strlen(GTK_ENTRY(widget)->text)) {
            if ((widget == priv->m_entry
                 && !GTK_WIDGET_REALIZED(priv->s_entry))
                || (widget == priv->s_entry
                    && GTK_WIDGET_REALIZED(priv->s_entry))) {
                return TRUE;
            }
        }
    }

    /* If entry is filled with number hardware key press,
       validate the entry. */
    if (event->keyval >= GDK_0 && event->keyval <= GDK_9) {
        GtkWidgetClass *c = GTK_WIDGET_GET_CLASS(widget);

        c->key_press_event(widget, event);

        if (GTK_IS_ENTRY (widget))
          {
            if (strlen (gtk_entry_get_text (GTK_ENTRY (widget))) == 2)
              {
                hildon_time_editor_validate (editor);
              }
          }

        return TRUE;
    }
    /* tab pressed in hour entry */
    else if (widget == priv->h_entry && (event->keyval == GDK_Tab ||
                   event->keyval == GDK_KP_Tab)) {
        gtk_widget_grab_focus(priv->m_entry);
        return TRUE;
    }
    /* tab pressed in minute entry */
    else if (widget == priv->m_entry && (event->keyval == GDK_Tab ||
                   event->keyval == GDK_KP_Tab)) {
        if (priv->show_s)
            gtk_widget_grab_focus(priv->s_entry);
        else
            gtk_widget_grab_focus(priv->h_entry);
        return TRUE;
    }
    /* tab pressed in second entry */
    else if (widget == priv->s_entry && (event->keyval == GDK_Tab ||
                   event->keyval == GDK_KP_Tab)) {
        gtk_widget_grab_focus(priv->h_entry);
        return TRUE;
    }
    /* left tab pressed in second entry */
    else if (widget == priv->s_entry &&
             event->keyval == GDK_ISO_Left_Tab) {
        gtk_widget_grab_focus(priv->m_entry);
        return TRUE;
    }
    /* left tab pressed in minute entry */
    else if (widget == priv->m_entry &&
             event->keyval == GDK_ISO_Left_Tab) {
        gtk_widget_grab_focus(priv->h_entry);
        return TRUE;
    }
    /* left tab pressed in hour entry */
    else if (widget == priv->h_entry &&
             event->keyval == GDK_ISO_Left_Tab) {
        if (priv->show_s)
            gtk_widget_grab_focus(priv->s_entry);
        else
            gtk_widget_grab_focus(priv->m_entry);
        return TRUE;
    }
    /* right arrow pressed in hour entry */
    else if (widget == priv->h_entry &&
             (event->keyval == GDK_Right || event->keyval == GDK_KP_Right)
             && pos >= GTK_ENTRY(priv->h_entry)->text_length) {
        gtk_widget_grab_focus(priv->m_entry);
        gtk_editable_set_position(GTK_EDITABLE(priv->m_entry), 0);
        return TRUE;
    }
    /* right arrow pressed in minute entry */
    else if (widget == priv->m_entry &&
             (event->keyval == GDK_Right || event->keyval == GDK_KP_Right)
             && pos >= GTK_ENTRY(priv->m_entry)->text_length) {
        if (priv->show_s) {
            gtk_widget_grab_focus(priv->s_entry);
            gtk_editable_set_position(GTK_EDITABLE(priv->s_entry), 0);
        } else {
          /* FIXME: the code is wrapping focus to leftmost entry, but this
             code path is blocked at the beginning of this function */
            gtk_widget_grab_focus(priv->h_entry);
            gtk_editable_set_position(GTK_EDITABLE(priv->h_entry), 0);
        }
        return TRUE;
    }
    /* right arrow pressed in second entry */
    else if (widget == priv->s_entry &&
             (event->keyval == GDK_Right || event->keyval == GDK_KP_Right)
             && pos >= GTK_ENTRY(priv->s_entry)->text_length) {
      /* FIXME: the code is wrapping focus to leftmost entry, but this
         code path is blocked at the beginning of this function */
        gtk_widget_grab_focus(priv->h_entry);
        gtk_editable_set_position(GTK_EDITABLE(priv->h_entry), 0);
        return TRUE;
    }
    /* left arrow key pressed in hour entry */
    else if (widget == priv->h_entry &&
             (event->keyval == GDK_Left || event->keyval == GDK_KP_Left) &&
             pos <= 0) {
        /* FIXME: the code is wrapping focus to rightmost entry, but this
           code path is blocked at the beginning of this function */
        if (priv->show_s) {
            gtk_widget_grab_focus(priv->s_entry);
            gtk_editable_set_position(GTK_EDITABLE(priv->s_entry), -1);
        } else {
            gtk_widget_grab_focus(priv->m_entry);
            gtk_editable_set_position(GTK_EDITABLE(priv->m_entry), -1);
        }
        return TRUE;
    }
    /* left arrow key pressed in minute entry */
    else if (widget == priv->m_entry &&
             (event->keyval == GDK_Left || event->keyval == GDK_KP_Left) &&
             pos <= 0) {
        gtk_widget_grab_focus(priv->h_entry);
        gtk_editable_set_position(GTK_EDITABLE(priv->h_entry), -1);
        return TRUE;
    }
    /* left arrow key pressed in seconds entry */
    else if (widget == priv->s_entry &&
             (event->keyval == GDK_Left || event->keyval == GDK_KP_Left) &&
             pos <= 0) {
        gtk_widget_grab_focus(priv->m_entry);
        gtk_editable_set_position(GTK_EDITABLE(priv->m_entry), -1);
        return TRUE;
    }
    /* pass other arrow key presses and backspace and del onwards */
    else if (event->keyval == GDK_Left      ||
             event->keyval == GDK_KP_Left   ||
             event->keyval == GDK_Right     ||
             event->keyval == GDK_KP_Right  ||
             event->keyval == GDK_Up        ||
             event->keyval == GDK_KP_Up     ||
             event->keyval == GDK_Down      ||
             event->keyval == GDK_KP_Down   ||
             event->keyval == GDK_BackSpace ||
             event->keyval == GDK_Delete    ||
             event->keyval == GDK_KP_Delete)
        return FALSE;   /* pass the keypress on */

    /* ingore other keys */
    return TRUE;
}


/* Returns VALIDATION_OK           if ticks is ok,
           VALIDATION_DURATION_MIN if it's too low,
           VALIDATION_DURATION_MAX if it's too high. */
static HildonValidation
hildon_time_editor_validate_duration (HildonTimeEditor * editor, guint ticks)
{
  HildonTimeEditorPrivate *priv;

  g_return_val_if_fail (editor, VALIDATION_ERROR);

  priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

  if (ticks > priv->duration_max)
    return VALIDATION_DURATION_MAX;

  if (ticks < priv->duration_min)
    return VALIDATION_DURATION_MIN;

  return (VALIDATION_OK);
}

/* Returns VALIDATION_OK if given H:M:S values are valid, or
           VALIDATION_TIME_HOURS,
           VALIDATION_TIME_MINUTES or
           VALIDATION_TIME_SECONDS 
              if some of them contain values out of valid range */
static HildonValidation
hildon_time_editor_validate_time (HildonTimeEditor * editor,
                                  guint hours,
                                  guint minutes,
                                  guint seconds,
                                  gboolean mode_24h)
{
  HildonTimeEditorPrivate *priv;

  g_return_val_if_fail (editor, VALIDATION_ERROR);

  priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

  priv->validated = FALSE;

  /* Check that hours are within allowed range (0..12/23) */
  if (mode_24h) {
    if (hours < HOURS_MIN_24 || hours > HOURS_MAX_24)
        return VALIDATION_TIME_HOURS;
  } else {
    if (hours < HOURS_MIN_12 || hours > HOURS_MAX_12)
        return VALIDATION_TIME_HOURS;
  }

  /* Check that minutes are within allowed range (0..59) */
  if (minutes < MINUTES_MIN || minutes > MINUTES_MAX)
    return VALIDATION_TIME_MINUTES;
  
  /* Check that seconds are within allowed range (0..59) */
  if (seconds < SECONDS_MIN || seconds > SECONDS_MAX)
    return VALIDATION_TIME_SECONDS;

  priv->validated = TRUE;
  return (VALIDATION_OK);
}

static void
hildon_time_editor_entry_changed(GtkWidget * widget, gpointer user_data)
{
  HildonTimeEditor *editor;

  editor = HILDON_TIME_EDITOR(user_data);

  /* Do nothing. We can't validate it immediately after each change or
     eg. changing hour in steps 19->29->22 would give errors in 29 step.
     FIXME: just remove this function. */
/*  hildon_time_editor_validate (editor);*/
}

/* NOTE: This function is mostly broken for the duration mode */
static void
hildon_time_editor_get_max_values(HildonTimeEditor *editor, 
        guint * pmax_hours, guint * pmin_hours,
        guint * pmax_minutes, guint * pmin_minutes,
        guint * pmax_seconds, guint * pmin_seconds)
{
    guint max_hours;
    guint max_minutes;
    guint max_seconds;
    guint min_hours;
    guint min_minutes;
    guint min_seconds;

    HildonTimeEditorPrivate *priv;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* Get duration min/max values as H:M:S. These are only used in
       duration mode. */
    max_hours   = priv->duration_max / 3600;
    max_minutes = (priv->duration_max - (max_hours * 3600)) / 60;
    max_seconds = priv->duration_max - (max_hours * 3600) - (max_minutes * 60);
    min_hours   = priv->duration_min / 3600;
    min_minutes = (priv->duration_min - (min_hours * 3600)) / 60;
    min_seconds = priv->duration_min - (min_hours * 3600) - (min_minutes * 60);

    /* Determine max and min values for duration mode */
    if (priv->duration_mode)
    {
      /* if the widget has focus, the value could be out of range, so
         use the calculated values then
       */
      if (!gtk_widget_is_focus (priv->h_entry))
        {
          /* Hour field isn't focused, use it for getting min/max mins/seconds.
             If hour isn't exactly the min/max hour value, the M:S fields
             are 00:00/59:59 */
          if ((guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->h_entry))) < max_hours)
            {
              max_minutes = 59;
              max_seconds = 59;
            }
          if ((guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->h_entry))) > min_hours)
            {
              min_minutes = 0;
              min_seconds = 0;
            }
        }
      if (!gtk_widget_is_focus (priv->m_entry))
        {
          /* Minute field isn't focused, use it for getting min/max seconds. */
          if ((guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->m_entry))) < max_minutes)
            {
              max_seconds = 59;
            }
          if ((guint) atoi(gtk_entry_get_text(GTK_ENTRY(priv->m_entry))) > min_minutes)
            {
              min_seconds = 0;
            }
        }
    }
    /* 24h clock mode */
    else if (priv->clock_24h) {
        max_hours = 23;
        max_seconds = max_minutes = 59;
        min_seconds = min_minutes = min_hours = 0;
    }
    /* 12h clock mode */
    else {
        max_hours = 12;
        min_hours = 1;
        max_seconds = max_minutes = 59;
        min_seconds = min_minutes = 0;
    }

    *pmax_hours   = max_hours;
    *pmax_minutes = max_minutes;
    *pmax_seconds = max_seconds;
    *pmin_hours   = min_hours;
    *pmin_minutes = min_minutes;
    *pmin_seconds = min_seconds;
  
}

/*** 
 * Utility functions
 */

static void
convert_to_12h (guint *h, guint *m, guint *s, gboolean *am)
{
  if (*h == 0) /* 00:00 to 00:59  add 12 hours */
    {
      *h += 12;
      *am = TRUE;
    }
  else if (*h >= 1 && *h < 12) /* 01:00 to 11:59  straight to am  */
    {
      *am = TRUE;
    }
  else if (*h >= 12 && *h < 13) /* 12:00 to 12:59  straight to pm  */
    {
      *am = FALSE;
    }
  else if (*h >= 13 && *h < 24 ) /* 13:00 to 23:59  subtract 12 hours */
    {
      *h -= 12;
      *am = FALSE;
    }
}

static void
convert_to_24h (guint *h, guint *m, guint *s, gboolean am)
{
  if (*h == 12 && am) /* 12 midnight - 12:59 AM  subtract 12 hours  */
    {
      *h -= 12;
    }
  else if (!am && *h >= 1 && *h < 12)    /* 1:00 PM - 11:59 AM   add 12 hours */
    {
      *h += 12;
    }
}

/***
 * Deprecated functions
 */


#ifndef HILDON_DISABLE_DEPRECATED

/**
 * hildon_time_editor_show_seconds:
 * @editor: The #HildonTimeEditor.
 * @enable: Enable or disable showing of seconds.
 *
 * This function is deprecated, use @hildon_time_editor_set_show_seconds instead.
 *
 **/
void hildon_time_editor_show_seconds(HildonTimeEditor * editor,
                                     gboolean enable)
{
    hildon_time_editor_set_show_seconds (editor, enable);
}
/**
 * hildon_time_editor_enable_duration_mode:
 * @editor: The #HildonTimeEditor.
 * @enable: Enable or disable duration editor mode
 *
 * This function is deprecated, use @hildon_time_editor_set_duration_mode instead.
 *
 **/
void hildon_time_editor_enable_duration_mode(HildonTimeEditor * editor,
                                             gboolean enable)
{
    hildon_time_editor_set_duration_mode (editor, enable);
}

#endif /* HILDON_DISABLE_DEPRECATED */
