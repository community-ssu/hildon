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

/**
 * SECTION:hildon-time-editor
 * @short_description: A widget used to enter time or duration in hours, minutes,
 * and optional seconds
 * @see_also: #HildonTimePicker
 * 
 * HildonTimeEditor is used to edit time or duration. Time mode is
 * restricted to normal 24 hour cycle, but Duration mode can select any
 * amount of time up to 99 hours.  It consists of entries for hours,
 * minutes and seconds, and pm/am indicator as well as a button which
 * popups a #HildonTimePicker dialog.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtkhbox.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkframe.h>
#include <gdk/gdkkeysyms.h>

#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <langinfo.h>
#include <libintl.h>

#include <hildon-widgets/hildon-defines.h>
#include <hildon-widgets/hildon-time-editor.h>
#include <hildon-widgets/hildon-time-picker.h>
#include <hildon-widgets/hildon-banner.h>
#include <hildon-widgets/hildon-input-mode-hint.h>
#include <hildon-widgets/hildon-private.h>
#include "hildon-composite-widget.h"
#include "hildon-marshalers.h"
#include "hildon-libs-enum-types.h"

#define _(String) dgettext(PACKAGE, String)

#define HILDON_TIME_EDITOR_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     HILDON_TYPE_TIME_EDITOR, HildonTimeEditorPrivate));

#define TICKS(h,m,s) ((h) * 3600 + (m) * 60 + (s))

#define TIME_EDITOR_HEIGHT         30
#define ICON_PRESSED                4
#define ICON_NAME                  "qgn_widg_timedit"
#define ICON_SIZE                  "timepicker-size"
#define MIN_DURATION 0
#define MAX_DURATION TICKS(99, 59, 59)

/* Default values for properties */
#define HILDON_TIME_EDITOR_TICKS_VALUE           0
#define HILDON_TIME_EDITOR_DURATION_MODE         FALSE
#define HILDON_TIME_EDITOR_DURATION_LOWER_VALUE  0
#define HILDON_TIME_EDITOR_DURATION_UPPER_VALUE  TICKS(99, 59, 59)

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
	PROP_SHOW_SECONDS,
	PROP_SHOW_HOURS
};

/* Indices for h/m/s entries in priv->entries */
enum {
  ENTRY_HOURS,
  ENTRY_MINS,
  ENTRY_SECS,

  ENTRY_COUNT
};

/* Signals */
enum {
  TIME_ERROR,
  LAST_SIGNAL
};

/* Error codes categories */
enum {
  MAX_VALUE,
  MIN_VALUE,
  WITHIN_RANGE,
  NUM_ERROR_CODES
};

static guint time_editor_signals[LAST_SIGNAL] = { 0 };
static guint hour_errors[NUM_ERROR_CODES] = { MAX_HOURS, MIN_HOURS, EMPTY_HOURS  };
static guint  min_errors[NUM_ERROR_CODES] = { MAX_MINS,  MIN_MINS,  EMPTY_MINS   };
static guint  sec_errors[NUM_ERROR_CODES] = { MAX_SECS,  MIN_SECS,  EMPTY_SECS   };

struct _HildonTimeEditorPrivate {
    guint      ticks;                /* Current duration in seconds  */

    gchar     *am_symbol;
    gchar     *pm_symbol;

    GtkWidget *iconbutton;           /* button for icon              */

    GtkWidget *frame;                /* frame around the entries     */
    GtkWidget *entries[ENTRY_COUNT]; /* h, m, s entries              */
    GtkWidget *hm_label;             /* between hour and minute      */
    GtkWidget *sec_label;            /* between minute and second    */
    GtkWidget *ampm_label;           /* label for showing am or pm   */

    GtkWidget *error_widget;         /* field to highlight in idle   */
    GtkWidget *ampm_button;          /* am/pm change button */


    gboolean   duration_mode;        /* In HildonDurationEditor mode */
    gboolean   show_seconds;         /* show seconds                 */
    gboolean   show_hours;           /* show hours                   */

    gboolean   ampm_pos_after;       /* is am/pm shown after others  */
    gboolean   clock_24h;            /* whether to show a 24h clock  */
    gboolean   am;                   /* TRUE == am, FALSE == pm      */

    guint      duration_min;         /* duration editor ranges       */
    guint      duration_max;         /* duration editor ranges       */

    guint      highlight_idle;
};

/***
 * Widget functions
 */

static void hildon_time_editor_class_init  (HildonTimeEditorClass *editor_class);
static void hildon_time_editor_init        (HildonTimeEditor      *editor);

static void hildon_time_editor_finalize    (GObject      *obj_self);

static void hildon_time_editor_set_property(GObject      *object,
                                            guint         param_id,
                                            const GValue *value,
                                            GParamSpec   *pspec);

static void hildon_time_editor_get_property(GObject      *object,
                                            guint         param_id,
                                            GValue       *value,
                                            GParamSpec   *pspec);

static void hildon_time_editor_forall(GtkContainer *container,
                                      gboolean      include_internals,
                                      GtkCallback   callback,
                                      gpointer      callback_data);
                          
static void hildon_time_editor_destroy(GtkObject * self);

/***
 * Signal handlers
 */

static gboolean hildon_time_editor_entry_focusout(GtkWidget     *widget,
                                                  GdkEventFocus *event,
                                                  gpointer       data);

static gboolean hildon_time_editor_entry_focusin(GtkWidget      *widget,
                                                 GdkEventFocus  *event, 
                                                 gpointer        data);

static gboolean hildon_time_editor_time_error(HildonTimeEditor *editor,
					      HildonTimeEditorErrorType type);

static gboolean hildon_time_editor_ampm_clicked(GtkWidget       *widget,
                                                gpointer         data);

static gboolean hildon_time_editor_icon_clicked(GtkWidget       *widget,
                                                gpointer         data);

static void     hildon_time_editor_size_request(GtkWidget       *widget,
                                                GtkRequisition  *requisition);

static void     hildon_time_editor_size_allocate(GtkWidget      *widget,
                                                 GtkAllocation  *allocation);

static gboolean hildon_time_editor_entry_keypress(GtkWidget     *widget,
                                                  GdkEventKey   *event,
                                                  gpointer       data);

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
hildon_time_editor_validate (HildonTimeEditor *editor, gboolean allow_intermediate);

static void hildon_time_editor_set_to_current_time (HildonTimeEditor * editor);

static gboolean
_hildon_time_editor_entry_select_all(GtkWidget *widget);


/***
 * Utility functions
 */
 
static void convert_to_12h (guint *h, gboolean *am);
static void convert_to_24h (guint *h, gboolean  am);

static void ticks_to_time (guint  ticks,
                           guint *hours,
                           guint *minutes,
                           guint *seconds);

static void
hildon_time_editor_inserted_text  (GtkEditable * editable,
                                   gchar * new_text,
                                   gint new_text_length,
                                   gint * position,
                                   gpointer user_data);

GType hildon_time_editor_get_type(void)
{
    static GType editor_type = 0;

    if (!editor_type) {
        static const GTypeInfo editor_info = {
            sizeof(HildonTimeEditorClass),
            NULL,       /* base_init      */
            NULL,       /* base_finalize  */
            (GClassInitFunc) hildon_time_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data     */
            sizeof(HildonTimeEditor),
            0,          /* n_preallocs    */
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

    g_assert(HILDON_IS_TIME_EDITOR(container));
    g_assert(callback != NULL);

    editor = HILDON_TIME_EDITOR(container);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if (!include_internals)
        return;

    /* widget that are always shown */
    (*callback) (priv->iconbutton, callback_data);
    (*callback) (priv->frame, callback_data);
}

static void hildon_time_editor_destroy(GtkObject * self)
{
    HildonTimeEditorPrivate *priv;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(self);

    if (priv->iconbutton) {
        gtk_widget_unparent(priv->iconbutton);
        priv->iconbutton = NULL;
    }
    if (priv->frame) {
        gtk_widget_unparent(priv->frame);
        priv->frame = NULL;
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
    widget_class->size_request = hildon_time_editor_size_request;
    widget_class->size_allocate = hildon_time_editor_size_allocate;
    widget_class->tap_and_hold_setup =
        hildon_time_editor_tap_and_hold_setup;
    widget_class->focus = hildon_composite_widget_focus;

    container_class->forall = hildon_time_editor_forall;
    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_time_editor_destroy;

    object_class->finalize = hildon_time_editor_finalize;

    editor_class->time_error = hildon_time_editor_time_error; 
    
    time_editor_signals[TIME_ERROR] =
        g_signal_new("time-error",
		     G_OBJECT_CLASS_TYPE(object_class),
		     G_SIGNAL_RUN_LAST,
		     G_STRUCT_OFFSET(HildonTimeEditorClass, time_error),
		     g_signal_accumulator_true_handled, NULL,
		     _hildon_marshal_BOOLEAN__ENUM,
		     G_TYPE_BOOLEAN, 1, HILDON_TYPE_TIME_EDITOR_ERROR_TYPE);

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
                                   FALSE,
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonTimeEditor:show_hours:
   *
   * Controls whether hours are shown in the editor
   */
  g_object_class_install_property( object_class, PROP_SHOW_HOURS,
                                   g_param_spec_boolean("show_hours",
                                   "Show hours field",
                                   "Controls whether the hours field is shown in the editor",
                                   TRUE,
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
  g_object_class_install_property( object_class, PROP_DURATION_MIN,
                                   g_param_spec_uint("duration_min",
                                   "Minumum duration value",
                                   "Smallest possible duration value",
                                   MIN_DURATION, MAX_DURATION,
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
    gint i;

    /* Forward this tap_and_hold_setup signal to all our child widgets */
    for (i = 0; i < ENTRY_COUNT; i++)
    {
      gtk_widget_tap_and_hold_setup(priv->entries[i], menu, func,
                                    GTK_TAP_AND_HOLD_NO_SIGNALS);
    }
    gtk_widget_tap_and_hold_setup(priv->ampm_button, menu, func,
                                  GTK_TAP_AND_HOLD_NO_SIGNALS);
    gtk_widget_tap_and_hold_setup(priv->iconbutton, menu, func,
                                  GTK_TAP_AND_HOLD_NONE);
}

static void hildon_time_editor_entry_changed(GtkWidget *widget, gpointer data)
{
    g_assert(HILDON_IS_TIME_EDITOR(data));
    hildon_time_editor_validate(HILDON_TIME_EDITOR(data), TRUE);
}

static void hildon_time_editor_init(HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;
    GtkWidget *hbox, *icon;
    gint i;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    gtk_widget_push_composite_child();
    
    /* Setup defaults and create widgets */
    priv->ticks          = 0;
    priv->show_seconds   = FALSE;
    priv->show_hours     = TRUE;
    priv->ampm_pos_after = TRUE;
    priv->clock_24h      = TRUE;
    priv->duration_mode  = FALSE;
    priv->iconbutton     = gtk_button_new();
    priv->ampm_label     = gtk_label_new(NULL);
    priv->hm_label       = gtk_label_new(NULL);
    priv->sec_label      = gtk_label_new(NULL);
    priv->frame          = gtk_frame_new(NULL);
    priv->ampm_button    = gtk_button_new();

    icon = gtk_image_new_from_icon_name(ICON_NAME, HILDON_ICON_SIZE_WIDG);
    hbox = gtk_hbox_new(FALSE, 0);

    GTK_WIDGET_SET_FLAGS(editor, GTK_NO_WINDOW);
    GTK_WIDGET_UNSET_FLAGS(priv->iconbutton, GTK_CAN_FOCUS | GTK_CAN_DEFAULT);
    
    gtk_container_set_border_width(GTK_CONTAINER(priv->frame), 0);

    gtk_container_add(GTK_CONTAINER(priv->iconbutton), icon);
    gtk_container_add(GTK_CONTAINER(priv->ampm_button), priv->ampm_label);
    gtk_button_set_relief(GTK_BUTTON(priv->ampm_button), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click(GTK_BUTTON(priv->ampm_button), FALSE);

    /* Create hour, minute and second entries */
    for (i = 0; i < ENTRY_COUNT; i++)
    {
      priv->entries[i] = gtk_entry_new();

      /* No frames for entries, so that they all appear to be inside one long entry */
      gtk_entry_set_has_frame(GTK_ENTRY(priv->entries[i]), FALSE);

      /* Set the entries to accept only numeric characters */
      g_object_set (priv->entries[i], "input-mode",
                    HILDON_INPUT_MODE_HINT_NUMERIC, NULL);

      /* The entry fields all take exactly two characters */
      gtk_entry_set_max_length  (GTK_ENTRY(priv->entries[i]), 2);
      gtk_entry_set_width_chars (GTK_ENTRY(priv->entries[i]), 2);

      g_signal_connect(priv->entries[i], "focus-in-event",
                       G_CALLBACK(hildon_time_editor_entry_focusin), editor);
      g_signal_connect(priv->entries[i], "focus-out-event",
                       G_CALLBACK(hildon_time_editor_entry_focusout), editor);
      g_signal_connect(priv->entries[i], "key-press-event",
                       G_CALLBACK(hildon_time_editor_entry_keypress), editor);
      g_signal_connect(priv->entries[i], "changed",
                       G_CALLBACK(hildon_time_editor_entry_changed), editor);
    
      /* inserted signal sets time */
      g_signal_connect_after (G_OBJECT(priv->entries[i]), "insert_text",
			      G_CALLBACK (hildon_time_editor_inserted_text), 
			      editor);
    }
    
    /* clicked signal for am/pm label */
    g_signal_connect(G_OBJECT(priv->ampm_button), "clicked",
                     G_CALLBACK(hildon_time_editor_ampm_clicked), editor);

    /* clicked signal for icon */
    g_signal_connect(G_OBJECT(priv->iconbutton), "clicked",
                     G_CALLBACK(hildon_time_editor_icon_clicked), editor);

    /* Set ourself as the parent of all the widgets we created */
    gtk_widget_set_parent(priv->iconbutton, GTK_WIDGET(editor));
    gtk_box_pack_start(GTK_BOX(hbox), priv->entries[ENTRY_HOURS], FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), priv->hm_label,             FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), priv->entries[ENTRY_MINS],  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), priv->sec_label,            FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), priv->entries[ENTRY_SECS],  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), priv->ampm_button,          FALSE, FALSE, 0);
    gtk_misc_set_padding(GTK_MISC(priv->ampm_label), 0, 0);

    gtk_container_add(GTK_CONTAINER(priv->frame), hbox);

    /* Show created widgets */
    gtk_widget_set_parent(priv->frame, GTK_WIDGET(editor));
    gtk_widget_show_all(priv->frame);
    gtk_widget_show_all(priv->iconbutton);

    /* Update AM/PM and time separators settings from locale */
    if (!hildon_time_editor_check_locale(editor)) {
        /* Using 12h clock */
        priv->clock_24h = FALSE;
    } else {
        gtk_widget_hide(priv->ampm_button);
    }
 
    if (!priv->show_seconds) {
        gtk_widget_hide(priv->sec_label);
        gtk_widget_hide(priv->entries[ENTRY_SECS]);
    }

    /* set the default time to current time. */
    hildon_time_editor_set_to_current_time (editor);

    gtk_widget_pop_composite_child();
}

static void hildon_time_editor_set_property (GObject      *object,
                                             guint         param_id,
                                             const GValue *value,
                                             GParamSpec   *pspec)
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

    case PROP_SHOW_HOURS:
      hildon_time_editor_set_show_hours (time_editor, g_value_get_boolean(value));
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

static void hildon_time_editor_get_property (GObject    *object,
                                             guint       param_id,
                                             GValue     *value,
                                             GParamSpec *pspec)
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

    case PROP_SHOW_HOURS:
      g_value_set_boolean (value, hildon_time_editor_get_show_hours (time_editor));
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

/**
 * hildon_time_editor_new:
 *
 * This function creates a new time editor. 
 *
 * Returns: pointer to a new #HildonTimeEditor widget
 */

GtkWidget *hildon_time_editor_new(void)
{
    return GTK_WIDGET(g_object_new(HILDON_TYPE_TIME_EDITOR, NULL));
}

static void hildon_time_editor_finalize(GObject * obj_self)
{
    HildonTimeEditorPrivate *priv = HILDON_TIME_EDITOR_GET_PRIVATE(obj_self);

    g_free(priv->am_symbol);
    g_free(priv->pm_symbol);

    if (priv->highlight_idle)
        g_source_remove(priv->highlight_idle);

    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);
}

/**
 * _hildon_time_editor_get_time_separators:
 * @editor: the #HildonTimeEditor
 * @hm_sep_label: the label that will show the hour:minutes separator
 * @ms_sep_label: the label that will show the minutes:seconds separator
 *
 * Gets hour-minute separator and minute-second separator from current
 * locale and sets then to the labels we set as parameters. Both
 * parameters can be NULL if you just want to assing one separator.
 *
 */
void 
_hildon_time_editor_get_time_separators(GtkLabel *hm_sep_label,
                                        GtkLabel *ms_sep_label)
{
    gchar buffer[256];
    gchar *separator;
    GDate locale_test_date;
    gchar *iter, *endp = NULL;

    /* Get localized time string */
    g_date_set_dmy(&locale_test_date, 1, 2, 1970);
    (void) g_date_strftime(buffer, sizeof(buffer), "%X", &locale_test_date);

    if (hm_sep_label != NULL)
      {
        /* Find h-m separator */
        iter = buffer;
        while (*iter && g_ascii_isdigit(*iter)) iter++;
    
        /* Extract h-m separator*/
        endp = iter;
        while (*endp && !g_ascii_isdigit(*endp)) endp++;
        separator = g_strndup(iter, endp - iter);
        gtk_label_set_label(hm_sep_label, separator);
        g_free(separator);
      }

    if (ms_sep_label != NULL)
      {      
        /* Find m-s separator */
        iter = endp;
        while (*iter && g_ascii_isdigit(*iter)) iter++;
    
        /* Extract m-s separator*/
        endp = iter;
        while (*endp && !g_ascii_isdigit(*endp)) endp++;
        separator = g_strndup(iter, endp - iter);
        gtk_label_set_label(ms_sep_label, separator);
        g_free(separator);
      }

}

/* Convert ticks to H:M:S. Ticks = seconds since 00:00:00. */
static void ticks_to_time (guint ticks,
                           guint *hours,
                           guint *minutes,
                           guint *seconds)
{
  guint left;

  *hours = ticks / 3600;
  left   = ticks % 3600;
  *minutes = left / 60;
  *seconds = left % 60;
}

/**
 * hildon_time_editor_set_ticks:
 * @editor: the #HildonTimeEditor widget
 * @ticks: the duration to set, in seconds
 *
 * Sets the current duration in seconds. This means seconds from
 * midnight, if not in duration mode. In case of any errors, it tries
 * to fix it.
 */

void hildon_time_editor_set_ticks (HildonTimeEditor * editor,
                                   guint ticks)
{
    HildonTimeEditorPrivate *priv;
    guint i, h, m, s;
    gchar str[3];

    g_assert(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* Validate ticks. If it's too low or too high, set it to
       min/max value for the current mode. */
    if (priv->duration_mode)
        priv->ticks = CLAMP(ticks, priv->duration_min, priv->duration_max);
    else {
        /* Check that ticks value is valid. We only need to check that hours
           don't exceed 23. */
        ticks_to_time (ticks, &h, &m, &s);
        if (h > HOURS_MAX_24)
            ticks = TICKS(HOURS_MAX_24, m, s);

        priv->ticks = ticks;
    }
    
    /* Get the time in H:M:S. */
    ticks_to_time (priv->ticks, &h, &m, &s);
    
    if (!priv->clock_24h && !priv->duration_mode)
      {
        /* Convert 24h H:M:S values to 12h mode, and update AM/PM state */
        convert_to_12h (&h, &priv->am);
      }

    /* Set H:M:S values to entries. We  do not want to invoke validation
       callbacks (since they can cause new call to this function), so we 
       block signals while setting values. */
    for (i = 0; i < ENTRY_COUNT; i++)
    {
      g_signal_handlers_block_by_func(priv->entries[i],
        (gpointer) hildon_time_editor_entry_changed, editor);

      g_signal_handlers_block_by_func(priv->entries[i],
        (gpointer) hildon_time_editor_inserted_text, editor);
      
      g_signal_handlers_block_by_func(priv->entries[i],
	(gpointer) hildon_time_editor_entry_focusout, editor);

    }

    g_snprintf(str, sizeof(str), "%02u", h);
    gtk_entry_set_text(GTK_ENTRY(priv->entries[ENTRY_HOURS]), str);

    g_snprintf(str, sizeof(str), "%02u", m);
    gtk_entry_set_text(GTK_ENTRY(priv->entries[ENTRY_MINS]), str);

    g_snprintf(str, sizeof(str), "%02u", s);
    gtk_entry_set_text(GTK_ENTRY(priv->entries[ENTRY_SECS]), str);

    for (i = 0; i < ENTRY_COUNT; i++)
    {
      g_signal_handlers_unblock_by_func(priv->entries[i],
        (gpointer) hildon_time_editor_entry_changed, editor);

      g_signal_handlers_unblock_by_func(priv->entries[i],
        (gpointer) hildon_time_editor_inserted_text, editor);

      g_signal_handlers_unblock_by_func(priv->entries[i],
	(gpointer) hildon_time_editor_entry_focusout, editor);

   }

    /* Update AM/PM label in case we're in 12h mode */
    gtk_label_set_label(GTK_LABEL(priv->ampm_label),
			priv->am ? priv->am_symbol : priv->pm_symbol);
    
    g_object_notify (G_OBJECT (editor), "ticks");
}

static void
hildon_time_editor_set_to_current_time (HildonTimeEditor * editor)
{
    time_t now;
    const struct tm *tm;

    now = time(NULL);
    tm = localtime(&now);

    if (tm != NULL)
        hildon_time_editor_set_time(editor, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

/**
 * hildon_time_editor_get_ticks:
 * @editor: the #HildonTimeEditor widget
 *
 * This function returns the current duration, in seconds.
 * This means seconds from midnight, if not in duration mode.
 * 
 * Returns: current duration in seconds 
 */
 
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
 * @editor: the #HildonTimeEditor
 * @show_seconds: enable or disable showing of seconds
 *
 * This function shows or hides the seconds field.
 */

void hildon_time_editor_set_show_seconds (HildonTimeEditor * editor,
                                        gboolean show_seconds)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if (show_seconds != priv->show_seconds) {
        priv->show_seconds = show_seconds;

        /* show/hide seconds field and its ':' label if the value changed. */
        if (show_seconds) {
            gtk_widget_show(priv->entries[ENTRY_SECS]);
            gtk_widget_show(priv->sec_label);        
        } else {    
            gtk_widget_hide(priv->entries[ENTRY_SECS]);
            gtk_widget_hide(priv->sec_label);
        }
    
        g_object_notify (G_OBJECT (editor), "show_seconds");
    }
}

/**
 * hildon_time_editor_get_show_seconds:
 * @editor: the #HildonTimeEditor widget
 *
 * This function returns a boolean indicating the visibility of
 * seconds in the #HildonTimeEditor
 *
 * Returns: TRUE if the seconds are visible 
 */

gboolean hildon_time_editor_get_show_seconds (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_TIME_EDITOR (editor), FALSE);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    return (priv->show_seconds);
}

/**
 * hildon_time_editor_set_duration_mode:
 * @editor: the #HildonTimeEditor
 * @duration_mode: enable or disable duration editor mode
 *
 * This function sets the duration editor mode in which the maximum hours
 * is 99.
 */
 
void hildon_time_editor_set_duration_mode (HildonTimeEditor * editor,
                                         gboolean duration_mode)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if (duration_mode != priv->duration_mode) {
        priv->duration_mode = duration_mode;

        if (duration_mode) {
            /* FIXME: Why do we reset the duration range here?
               Would change API, so won't touch this for now. */
            hildon_time_editor_set_duration_range(editor, MIN_DURATION,
                                                      MAX_DURATION);
            /* There's no AM/PM label or time picker icon in duration mode.
               Make sure they're hidden. */
            gtk_widget_hide(GTK_WIDGET(priv->ampm_label));
            gtk_widget_hide(GTK_WIDGET(priv->ampm_button));
            gtk_widget_hide(GTK_WIDGET(priv->iconbutton));
            /* Duration mode has seconds by default. */
            hildon_time_editor_set_show_seconds(editor, TRUE);
        } else {
            /* Make sure AM/PM label and time picker icons are visible if needed */
            if (!priv->clock_24h)
                gtk_widget_show(GTK_WIDGET(priv->ampm_label));

            gtk_widget_show(GTK_WIDGET(priv->ampm_button));
            gtk_widget_show(GTK_WIDGET(priv->iconbutton));        

        	/* Reset the ticks to current time. Anything set in duration mode
             * is bound to be invalid or useless in time mode.
             */
            hildon_time_editor_set_to_current_time (editor);
        }

        g_object_notify (G_OBJECT (editor), "duration_mode");
    }
}

/**
 * hildon_time_editor_get_duration_mode:
 * @editor: the #HildonTimeEditor widget
 *
 * This function returns a boolean indicating whether the #HildonTimeEditor
 * is in the duration mode.
 * 
 * Returns: TRUE if the #HildonTimeEditor is in duration mode 
 */

gboolean hildon_time_editor_get_duration_mode (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_TIME_EDITOR (editor), FALSE);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    return (priv->duration_mode);
}

/**
 * hildon_time_editor_set_duration_min:
 * @editor: the #HildonTimeEditor widget
 * @duration_min: mimimum allowed duration
 *
 * Sets the minimum allowed duration for the duration mode.
 * Note: Has no effect in time mode
 */

void hildon_time_editor_set_duration_min (HildonTimeEditor * editor,
                                          guint duration_min)
{
    HildonTimeEditorPrivate *priv;

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
 * @editor: the #HildonTimeEditor widget
 *
 * This function returns the smallest duration the #HildonTimeEditor
 * allows in the duration mode.
 * 
 * Returns: minimum allowed duration in seconds 
 */
 
guint hildon_time_editor_get_duration_min (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail(HILDON_IS_TIME_EDITOR(editor), 0);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if( !priv->duration_mode )
      return (0);

    return (priv->duration_min);
}

/**
 * hildon_time_editor_set_duration_max:
 * @editor: the #HildonTimeEditor widget
 * @duration_max: maximum allowed duration in seconds
 *
 * Sets the maximum allowed duration in seconds for the duration mode.
 * Note: Has no effect in time mode
 */
 
void hildon_time_editor_set_duration_max (HildonTimeEditor * editor,
                                          guint duration_max)
{
    HildonTimeEditorPrivate *priv;

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
 * @editor: the #HildonTimeEditor widget
 *
 * This function returns the longest duration the #HildonTimeEditor
 * allows in the duration mode.
 * 
 * Returns: maximum allowed duration in seconds 
 */
 
guint hildon_time_editor_get_duration_max (HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail(HILDON_IS_TIME_EDITOR(editor), 0);

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if( !priv->duration_mode )
      return (0);

    return (priv->duration_max);
}


/**
 * hildon_time_editor_set_time:
 * @editor: the #HildonTimeEditor widget
 * @hours: hours
 * @minutes: minutes
 * @seconds: seconds
 *
 * This function sets the time on an existing time editor. If the
 * time specified by the arguments is invalid, it's fixed.
 * The time is assumed to be in 24h format.
 */

void hildon_time_editor_set_time(HildonTimeEditor * editor, guint hours,
                                 guint minutes, guint seconds)
{
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    hildon_time_editor_set_ticks (editor, TICKS(hours, minutes, seconds));
}

/**
 * hildon_time_editor_get_time:
 * @editor: the #HildonTimeEditor widget
 * @hours: hours
 * @minutes: minutes
 * @seconds: seconds
 *
 * Gets the time of the #HildonTimeEditor widget. The time returned is
 * always in 24h format.
 */

void hildon_time_editor_get_time(HildonTimeEditor * editor,
                                 guint * hours,
                                 guint * minutes, guint * seconds)
{
    HildonTimeEditorPrivate *priv;
    
    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    ticks_to_time (hildon_time_editor_get_ticks (editor),
                   hours, minutes, seconds);
}

/**
 * hildon_time_editor_set_duration_range:
 * @editor: the #HildonTimeEditor widget
 * @min_seconds: minimum allowed time in seconds
 * @max_seconds: maximum allowed time in seconds
 *
 * Sets the duration editor time range of the #HildonTimeEditor widget.
 */

void hildon_time_editor_set_duration_range(HildonTimeEditor * editor,
                                           guint min_seconds,
                                           guint max_seconds)
{
    HildonTimeEditorPrivate *priv;
    guint tmp;
    
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
           Would change API, so won't touch this for now. */
        hildon_time_editor_set_ticks(editor, min_seconds);
    }
}

/**
 * hildon_time_editor_get_duration_range:
 * @editor: the #HildonTimeEditor widget
 * @min_seconds: pointer to guint
 * @max_seconds: pointer to guint
 *
 * Gets the duration editor time range of the #HildonTimeEditor widget.
 */

void hildon_time_editor_get_duration_range(HildonTimeEditor * editor,
                                           guint * min_seconds,
                                           guint * max_seconds)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    *min_seconds = priv->duration_min;
    *max_seconds = priv->duration_max;
}

static gboolean hildon_time_editor_check_locale(HildonTimeEditor * editor)
{
    HildonTimeEditorPrivate *priv;

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* Update time separator symbols */
    _hildon_time_editor_get_time_separators(GTK_LABEL(priv->hm_label), GTK_LABEL(priv->sec_label));
 
    /* Get AM/PM symbols. */
    priv->am_symbol = g_strdup(nl_langinfo(AM_STR));
    priv->pm_symbol = g_strdup(nl_langinfo(PM_STR));

    if (priv->am_symbol[0] == '\0')
        return TRUE;
    else {
        /* 12h clock mode. Check if AM/PM should be before or after time.
           %p is the AM/PM string, so we assume that if the format string
           begins with %p it's in the beginning, and in any other case it's
           in the end (although that's not necessarily the case). */
        if (strncmp(nl_langinfo(T_FMT_AMPM), "%p", 2) == 0)
            priv->ampm_pos_after = FALSE;
        return FALSE;
    }
}

static gboolean hildon_time_editor_entry_focusin(GtkWidget * widget,
                                                 GdkEventFocus * event, 
                                                 gpointer data)
{
    g_idle_add((GSourceFunc) _hildon_time_editor_entry_select_all,
               GTK_ENTRY(widget));

    return FALSE;
}

static gboolean 
hildon_time_editor_time_error(HildonTimeEditor *editor,
			      HildonTimeEditorErrorType type)
{
  return TRUE;
}

/* Returns negative if we didn't get value,
 * and should stop further validation 
 */
static gint validated_conversion(HildonTimeEditorPrivate *priv,
                                 GtkWidget               *field,
                                 gint                     min,
                                 gint                     max,
                                 gint                     def_value,
                                 gboolean                 allow_intermediate,
                                 guint                   *error_code,
                                 GString                 *error_string)
{
    const gchar *text;
    gchar *tail;
    long value;

    text = gtk_entry_get_text(GTK_ENTRY(field));

    if (text && text[0])
    {
        /* Try to convert entry text to number */
        value = strtol(text, &tail, 10);

        /* Check if conversion succeeded */
        if ((tail[0] == 0) && !(text[0] == '-'))
        {    
            if (value > max) {
                g_string_printf(error_string, _("ckct_ib_maximum_value"), max);
                priv->error_widget = field;
                *error_code = MAX_VALUE;
                return max;
		    }
            if (value < min && !allow_intermediate) {
                g_string_printf(error_string, _("ckct_ib_minimum_value"), min);
                priv->error_widget = field;
                *error_code = MIN_VALUE;
                return min;
            }

            return value;
        }
        /* We'll handle failed conversions soon */
	else
	{
	    if ((tail[0] == '-') || (text[0] == '-'))
              {
		g_string_printf(error_string, _("ckct_ib_minimum_value"), min);
		priv->error_widget = field;
		*error_code = MIN_VALUE;
		return min;
	      }
	}
    }
    else if (allow_intermediate) 
        return -1;  /* Empty field while user is still editing. No error, but
                       cannot validate either... */
    else /* Empty field: show error and set value to minimum allowed */
      {
        g_string_printf(error_string, _("ckct_ib_set_a_value_within_range"), min, max);
        priv->error_widget = field;
        *error_code = WITHIN_RANGE;
        return def_value;
      }

    /* Empty field and not allowed intermediated OR failed conversion */
    g_string_printf(error_string, _("ckct_ib_set_a_value_within_range"), min, max);
    priv->error_widget = field;
    *error_code = WITHIN_RANGE;
    return -1;
}

static void
hildon_time_editor_real_validate(HildonTimeEditor *editor, 
    gboolean allow_intermediate, GString *error_string)
{
    HildonTimeEditorPrivate *priv;
    guint h, m, s, ticks;
    guint error_code;
    guint max_hours, min_hours, def_hours;
    guint max_minutes, min_minutes, def_minutes;
    guint max_seconds, min_seconds, def_seconds;
    gboolean r;

    g_assert(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);
    
    /* Find limits for field based validation. */
    if (priv->duration_mode)
    {
        ticks_to_time(priv->duration_min, &min_hours, &min_minutes, &min_seconds);
        ticks_to_time(priv->duration_max, &max_hours, &max_minutes, &max_seconds);
    } else {
        if (priv->clock_24h) {
            max_hours = HOURS_MAX_24;
            min_hours = HOURS_MIN_24;
        } else {
            max_hours = HOURS_MAX_12;
            min_hours = HOURS_MIN_12;
        }
    }

    hildon_time_editor_get_time(editor, &def_hours, &def_minutes, &def_seconds);

    /* Get time components from fields and validate them... */
    if (priv->show_hours) {
        h = validated_conversion(priv, priv->entries[ENTRY_HOURS], min_hours, max_hours, def_hours,
            allow_intermediate, &error_code, error_string);
	if (priv->error_widget == priv->entries[ENTRY_HOURS])
	  g_signal_emit (editor, time_editor_signals [TIME_ERROR], 0, hour_errors[error_code], &r);
        if ((gint) h < 0) return;
    }
    else h = 0;
    m = validated_conversion(priv, priv->entries[ENTRY_MINS], MINUTES_MIN, MINUTES_MAX, def_minutes,
        allow_intermediate, &error_code, error_string);
    if (priv->error_widget == priv->entries[ENTRY_MINS])
	  g_signal_emit (editor, time_editor_signals [TIME_ERROR], 0, min_errors[error_code], &r);
    if ((gint) m < 0) return;
    if (priv->show_seconds) {
        s = validated_conversion(priv, priv->entries[ENTRY_SECS], SECONDS_MIN, SECONDS_MAX, def_seconds,
            allow_intermediate, &error_code, error_string);
	if (priv->error_widget == priv->entries[ENTRY_SECS])
	      g_signal_emit (editor, time_editor_signals [TIME_ERROR], 0, sec_errors[error_code], &r);
        if ((gint) s < 0) return;
    } 
    else s = 0;

    /* Ok, we now do separate check that tick count is valid for duration mode */      
    if (priv->duration_mode)
    {          
        ticks = TICKS(h, m, s);

        if (ticks < priv->duration_min && !allow_intermediate)
        {
            g_string_printf(error_string,
                _("ckct_ib_min_allowed_duration_hts"), 
                min_hours, min_minutes, min_seconds);
            hildon_time_editor_set_ticks (editor, priv->duration_min);
            priv->error_widget = priv->show_hours ? priv->entries[ENTRY_HOURS] : priv->entries[ENTRY_MINS];
	    g_signal_emit (editor, time_editor_signals[TIME_ERROR], 0, MIN_DUR, &r);
            return;
        }
        else if (ticks > priv->duration_max)
        {
            g_string_printf(error_string,
                _("ckct_ib_max_allowed_duration_hts"), 
                max_hours, max_minutes, max_seconds);
            hildon_time_editor_set_ticks (editor, priv->duration_max);
            priv->error_widget = priv->show_hours ? priv->entries[ENTRY_HOURS] : priv->entries[ENTRY_MINS];
	    g_signal_emit (editor, time_editor_signals[TIME_ERROR], 0, MAX_DUR, &r);
            return;
        }
    }
    else if (!priv->clock_24h)
        convert_to_24h (&h, priv->am);

    /* The only case when we do not want to refresh the
       time display, is when the user is editing a value 
       (unless the value was out of bounds and we have to fix it) */
    if (!allow_intermediate || priv->error_widget)
        hildon_time_editor_set_time (editor, h, m, s);
}

/* Setting text to entries causes entry to recompute itself
   in idle callback, which remove selection. Because of this
   we need to do selection in idle as well. */
static gboolean highlight_callback(gpointer data)
{
    HildonTimeEditorPrivate *priv;
    GtkWidget *widget;
    gint i;

    GDK_THREADS_ENTER ();
    
    g_assert(HILDON_IS_TIME_EDITOR(data));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(data);
    widget = priv->error_widget;
    priv->error_widget = NULL;
    priv->highlight_idle = 0;

    g_assert(GTK_IS_ENTRY(widget));

    /* Avoid revalidation because it will issue the date_error signal
       twice when there is an empty field. We must block the signal
       for all the entries because we do not know where the focus
       comes from */
    for (i = 0; i < ENTRY_COUNT; i++)
      g_signal_handlers_block_by_func(priv->entries[i],
				      (gpointer) hildon_time_editor_entry_focusout, data);
    gtk_editable_select_region(GTK_EDITABLE(widget), 0, -1);
    gtk_widget_grab_focus(widget);
    for (i = 0; i < ENTRY_COUNT; i++)
      g_signal_handlers_unblock_by_func(priv->entries[i],
					(gpointer) hildon_time_editor_entry_focusout, data);

    GDK_THREADS_LEAVE ();

    return FALSE;
}

/* Update ticks from current H:M:S entries. If they're invalid, show an
   infoprint and update the fields unless they're empty. */
static void
hildon_time_editor_validate (HildonTimeEditor *editor, gboolean allow_intermediate)
{
    HildonTimeEditorPrivate *priv;
    GString *error_message;

    g_assert(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* if there is already an error we do nothing until it will be managed by the idle */
    if (priv->highlight_idle == 0)
      {
        error_message = g_string_new(NULL);
        hildon_time_editor_real_validate(editor, 
                                         allow_intermediate, error_message);
        
        if (priv->error_widget) 
          {
            hildon_banner_show_information(priv->error_widget, NULL,
                                           error_message->str);
            
            priv->highlight_idle = g_idle_add(highlight_callback, editor);
          }

        g_string_free(error_message, TRUE);
      }
}

/* on inserted text, if entry has two digits, jumps to the next field. */
static void
hildon_time_editor_inserted_text  (GtkEditable * editable,
                                   gchar * new_text,
                                   gint new_text_length,
                                   gint * position,
                                   gpointer user_data) 
{
  HildonTimeEditor *editor;
  GtkEntry *entry;
  gchar *value;
  HildonTimeEditorPrivate *priv;

  entry = GTK_ENTRY(editable);
  editor = HILDON_TIME_EDITOR(user_data);

  priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

  /* if there is already an error we don't have to do anything */ 
  if (!priv->error_widget)
    {
 
      value = (gchar *) gtk_entry_get_text(entry);
  
      if (strlen(value) == 2)
        {
          if (GTK_WIDGET(editable) == priv->entries[ENTRY_HOURS]) 
            {
              /* We already checked the input in changed signal, but 
               * now we will re-check it again in focus-out we 
               * intermediate flag set to FALSE */
              gtk_widget_grab_focus(priv->entries[ENTRY_MINS]);
              *position = -1;
            }
          else if (GTK_WIDGET(editable) == priv->entries[ENTRY_MINS] &&
                   GTK_WIDGET_VISIBLE (priv->entries[ENTRY_SECS])) 
            {
              /* See above */
              gtk_widget_grab_focus(priv->entries[ENTRY_SECS]);
              *position = -1;
            }
        }
    }   
}

static gboolean hildon_time_editor_entry_focusout(GtkWidget * widget,
                                                  GdkEventFocus * event,
                                                  gpointer data)
{
  g_assert(HILDON_IS_TIME_EDITOR(data));

  /* Validate the given time and update ticks. */
  hildon_time_editor_validate(HILDON_TIME_EDITOR(data), FALSE);

  return FALSE;
}

static gboolean
hildon_time_editor_ampm_clicked(GtkWidget * widget,
                                gpointer data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;

    g_assert(GTK_IS_WIDGET(widget));
    g_assert(HILDON_IS_TIME_EDITOR(data));

    editor = HILDON_TIME_EDITOR(data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* First validate the given time and update ticks. */
    hildon_time_editor_validate (editor, FALSE);

    /* Apply the AM/PM change by moving the current time by 12 hours */
    if (priv->am) {
        /* 00:00 .. 11:59 -> 12:00 .. 23:59 */
        hildon_time_editor_set_ticks (editor, priv->ticks + 12*3600);
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
    GtkWidget *picker;
    GtkWidget *parent;
    guint h, m, s, result;
    HildonTimeEditorPrivate *priv;

    g_assert(HILDON_IS_TIME_EDITOR(data));

    editor = HILDON_TIME_EDITOR(data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    /* icon is passive in duration editor mode */
    if (hildon_time_editor_get_duration_mode(editor))
        return FALSE;

    /* Validate and do not launch if broken */
    hildon_time_editor_validate(HILDON_TIME_EDITOR(data), FALSE);
    if (priv->error_widget != NULL)
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
        gtk_widget_grab_focus (editor);
        break;
    default:
        break;
    }

    gtk_widget_destroy(picker);
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

    /* Get frame's size */
    gtk_widget_size_request(priv->frame, requisition);

    if (GTK_WIDGET_VISIBLE(priv->iconbutton))
    {
        gtk_widget_size_request(priv->iconbutton, &req);
        /* Reserve space for icon */
        requisition->width += req.width + ICON_PRESSED +
          HILDON_MARGIN_DEFAULT;
    }

    /* FIXME: It's evil to use hardcoded TIME_EDITOR_HEIGHT. For now we'll
       want to force this since themes might have varying thickness values
       which cause the height to change. */
    requisition->height = TIME_EDITOR_HEIGHT;
}

static void hildon_time_editor_size_allocate(GtkWidget * widget,
                                             GtkAllocation * allocation)
{
    HildonTimeEditorPrivate *priv = HILDON_TIME_EDITOR_GET_PRIVATE(widget);
    GtkAllocation alloc;
    GtkRequisition req, max_req;

    widget->allocation = *allocation;
    gtk_widget_get_child_requisition(widget, &max_req);

    /* Center horizontally */
    alloc.x = allocation->x + MAX(allocation->width - max_req.width, 0) / 2;
    /* Center vertically */
    alloc.y = allocation->y + MAX(allocation->height - max_req.height, 0) / 2;
    
    /* allocate frame */
    gtk_widget_get_child_requisition(priv->frame, &req);

    alloc.width = req.width;
    alloc.height = max_req.height;
    gtk_widget_size_allocate(priv->frame, &alloc);

    /* allocate icon */
    if (GTK_WIDGET_VISIBLE(priv->iconbutton)) {
        gtk_widget_get_child_requisition(priv->iconbutton, &req);

        alloc.x += alloc.width + HILDON_MARGIN_DEFAULT;
        alloc.width = req.width;
        gtk_widget_size_allocate(priv->iconbutton, &alloc);
    }

    /* FIXME: ugly way to move labels up. They just don't seem move up
       otherwise. This is likely because we force the editor to be
       smaller than it otherwise would be. */
    alloc = priv->ampm_label->allocation;
    alloc.y = allocation->y - 2;
    alloc.height = max_req.height + 2;
    gtk_widget_size_allocate(priv->ampm_label, &alloc);

    alloc = priv->hm_label->allocation;
    alloc.y = allocation->y - 2;
    alloc.height = max_req.height + 2;
    gtk_widget_size_allocate(priv->hm_label, &alloc);

    alloc = priv->sec_label->allocation;
    alloc.y = allocation->y - 2;
    alloc.height = max_req.height + 2;
    gtk_widget_size_allocate(priv->sec_label, &alloc);
}

static gboolean hildon_time_editor_entry_keypress(GtkWidget * widget,
                                                  GdkEventKey * event,
                                                  gpointer data)
{
    HildonTimeEditor *editor;
    HildonTimeEditorPrivate *priv;
    gint cursor_pos;

    g_assert(GTK_IS_ENTRY(widget));
    g_assert(event != NULL);
    g_assert(HILDON_IS_TIME_EDITOR(data));

    editor = HILDON_TIME_EDITOR(data);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);
    cursor_pos = gtk_editable_get_position(GTK_EDITABLE(widget));

    switch (event->keyval)
    {
        case GDK_Return:
            /* Return key popups up time picker dialog. Visually it looks as if
               the time picker icon was clicked. Before opening the time picker
               the fields are first validated and fixed. */
            hildon_time_editor_validate (editor, FALSE);
            hildon_gtk_button_set_depressed(GTK_BUTTON(priv->iconbutton), TRUE);
            hildon_time_editor_icon_clicked(widget, data);
            hildon_gtk_button_set_depressed(GTK_BUTTON(priv->iconbutton), FALSE);
            return TRUE;

        case GDK_Left:
            /* left arrow pressed in the entry. If we are on first position, try to
               move to the previous field. */
            if (cursor_pos == 0) {
                (void) gtk_widget_child_focus(GTK_WIDGET(editor), GTK_DIR_LEFT);
                return TRUE;
            }
            break;

        case GDK_Right:
            /* right arrow pressed in the entry. If we are on last position, try to
               move to the next field. */
            if (cursor_pos >= g_utf8_strlen(gtk_entry_get_text(GTK_ENTRY(widget)), -1)) {
                (void) gtk_widget_child_focus(GTK_WIDGET(editor), GTK_DIR_RIGHT);    
                return TRUE;
            }
            break;

        default:
            break;
    };

    return FALSE;
}

/*** 
 * Utility functions
 */

static void
convert_to_12h (guint *h, gboolean *am)
{
  g_assert(0 <= *h && *h < 24);

  /* 00:00 to 00:59  add 12 hours      */
  /* 01:00 to 11:59  straight to am    */
  /* 12:00 to 12:59  straight to pm    */
  /* 13:00 to 23:59  subtract 12 hours */

  if      (       *h == 0       ) { *am = TRUE;  *h += 12;}
  else if (  1 <= *h && *h < 12 ) { *am = TRUE;           }
  else if ( 12 <= *h && *h < 13 ) { *am = FALSE;          }
  else                            { *am = FALSE; *h -= 12;}
}

static void
convert_to_24h (guint *h, gboolean am)
{
  if (*h == 12 && am) /* 12 midnight - 12:59 AM  subtract 12 hours  */
    {
      *h -= 12;
    }
  else if (!am && 1 <= *h && *h < 12)    /* 1:00 PM - 11:59 AM   add 12 hours */
    {
      *h += 12;
    }
}

/**
 * hildon_time_editor_set_show_hours:
 * @editor: The #HildonTimeEditor.
 * @enable: Enable or disable showing of hours.
 *
 * This function shows or hides the hours field.
 *
 * Since: 0.12.4
 **/
void hildon_time_editor_set_show_hours(HildonTimeEditor * editor,
                                       gboolean show_hours)
{
    HildonTimeEditorPrivate *priv;

    g_return_if_fail(HILDON_IS_TIME_EDITOR(editor));

    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    if (show_hours != priv->show_hours) {
        priv->show_hours = show_hours;

        /* show/hide hours field and its ':' label if the value changed. */
        if (show_hours) {
            gtk_widget_show(priv->entries[ENTRY_HOURS]);
            gtk_widget_show(priv->hm_label);        
        } else {    
            gtk_widget_hide(priv->entries[ENTRY_HOURS]);
            gtk_widget_hide(priv->hm_label);
        }
    
        g_object_notify (G_OBJECT (editor), "show_hours");
    }
}

/**
 * hildon_time_editor_get_show_hours:
 * @self: the @HildonTimeEditor widget.
 *
 * This function returns a boolean indicating the visibility of
 * hours in the @HildonTimeEditor
 *
 * Return value: TRUE if hours are visible. 
 *
 * Since: 0.12.4-1
 **/
gboolean hildon_time_editor_get_show_hours(HildonTimeEditor *editor)
{
    HildonTimeEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_TIME_EDITOR (editor), FALSE);
    priv = HILDON_TIME_EDITOR_GET_PRIVATE(editor);

    return priv->show_hours;
}

/***
 * Deprecated functions
 */

/**
 * hildon_time_editor_show_seconds:
 * @editor: the #HildonTimeEditor
 * @enable: enable or disable showing of seconds
 *
 * This function is deprecated, 
 * use #hildon_time_editor_set_show_seconds instead.
 */
void hildon_time_editor_show_seconds(HildonTimeEditor * editor,
                                     gboolean enable)
{
    hildon_time_editor_set_show_seconds (editor, enable);
}
/**
 * hildon_time_editor_enable_duration_mode:
 * @editor: the #HildonTimeEditor
 * @enable: enable or disable duration editor mode
 *
 * This function is deprecated, 
 * use #hildon_time_editor_set_duration_mode instead.
 */
void hildon_time_editor_enable_duration_mode(HildonTimeEditor * editor,
                                             gboolean enable)
{
    hildon_time_editor_set_duration_mode (editor, enable);
}

/* Idle callback */
static gboolean
_hildon_time_editor_entry_select_all (GtkWidget *widget)
{
	GDK_THREADS_ENTER ();
	gtk_editable_select_region(GTK_EDITABLE(widget), 0, -1);
	GDK_THREADS_LEAVE ();
	return FALSE;
}
