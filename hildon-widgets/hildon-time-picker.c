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
 * @file hildon-time-picker.c
 * 
 * This file implements the HildonTimePicker widget. This widget
 * is used with HildonTimeEditor widget to set time.
 *
 */

/* HILDON DOC
 * @shortdesc: TimePicker is a widget for setting a time.
 * @longdesc: The TimePicker contains two fields for setting the
 * time. Arrow buttons can be used to increment and decrement time. If
 * specified in systems locale setting AM/PM button is displayed.
 * 
 * @seealso: #HildonTimeEditor
 */

#include "hildon-time-picker.h"
#include <hildon-widgets/hildon-defines.h>
#include <hildon-widgets/gtk-infoprint.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <langinfo.h>
#include <libintl.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _(String) dgettext(PACKAGE, String)

#define HILDON_TIME_PICKER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     HILDON_TYPE_TIME_PICKER, HildonTimePickerPrivate))

#define DEFAULT_HOURS 1
#define DEFAULT_MINUTES 1
#define DEFAULT_ARROW_WIDTH 26
#define DEFAULT_ARROW_HEIGHT 26

#define MINS_IN_1H  (60)
#define MINS_IN_24H (MINS_IN_1H * 24)
#define MINS_IN_12H (MINS_IN_1H * 12)

#define HILDON_TIME_PICKER_LABEL_X_PADDING 0
#define HILDON_TIME_PICKER_LABEL_Y_PADDING 1

enum
{
  WIDGET_GROUP_HOURS,
  WIDGET_GROUP_10_MINUTES,
  WIDGET_GROUP_1_MINUTES,
  WIDGET_GROUP_AMPM,

  WIDGET_GROUP_COUNT
};

enum
{
  BUTTON_UP,
  BUTTON_DOWN,

  BUTTON_COUNT
};

typedef struct
{
  GtkWidget *frame;
  GtkWidget *eventbox;
  GtkLabel *label;

  /* buttons are used for hours and minutes, but not for am/pm */
  GtkWidget *buttons[BUTTON_COUNT];

} HildonTimePickerWidgetGroup;

static GtkDialogClass *parent_class;

struct _HildonTimePickerPrivate
{
  HildonTimePickerWidgetGroup widgets[WIDGET_GROUP_COUNT];

  gchar *am_symbol;
  gchar *pm_symbol;

  guint key_repeat;
  guint minutes; /* time in minutes since midnight */
  gint mul; /* for key repeat handling */
  guint timer_id;

  guint show_ampm        : 1; /* 12 hour clock, show AM/PM */
  guint ampm_left        : 1;
  guint button_press     : 1;
  guint start_key_repeat : 1;
};

enum
{
  PROP_MINUTES = 1
};


static gint button_multipliers[WIDGET_GROUP_COUNT][2] =
{
  { MINS_IN_1H, -MINS_IN_1H },
  { 10, -10 },
  { 1, -1 },
  { 0, 0 }
};

static void
hildon_time_picker_class_init( HildonTimePickerClass *klass );

static void
hildon_time_picker_init( HildonTimePicker *picker );

static gboolean
hildon_time_picker_key_repeat_timeout( gpointer tpicker );

static void
hildon_time_picker_change_time( HildonTimePicker *picker, guint minutes );

static gboolean
hildon_time_picker_ampm_release( GtkWidget *widget, GdkEvent *event,
                                 HildonTimePicker *picker );

static gboolean
hildon_time_picker_arrow_press( GtkWidget *widget, GdkEvent *event,
                                HildonTimePicker *picker );
static gboolean
hildon_time_picker_arrow_release( GtkWidget *widget, GdkEvent *event,
                                  HildonTimePicker *picker );

static void
hildon_time_picker_finalize( GObject *object );

static void
hildon_time_picker_get_property( GObject *object, guint param_id,
			                           GValue *value, GParamSpec *pspec );

static void
hildon_time_picker_set_property( GObject *object, guint param_id,
			                           const GValue *value, GParamSpec *pspec );

static gboolean
hildon_time_picker_event_box_focus_in( GtkWidget *widget, GdkEvent *event,
                                       gpointer unused );

static gboolean
hildon_time_picker_event_box_focus_out( GtkWidget *widget, GdkEvent *event,
                                        gpointer unused );

static gboolean
hildon_time_picker_event_box_key_press( GtkWidget *widget,  GdkEventKey *event,
                                        HildonTimePicker *picker );

static gboolean
hildon_time_picker_event_box_key_release( GtkWidget *widget,  GdkEventKey *event,
                                          HildonTimePicker *picker );

static gboolean
hildon_time_picker_event_box_button_press( GtkWidget *widget,  GdkEventKey *event,
                                          gpointer unused );

static void
hildon_time_picker_map( GtkWidget *widget );


GType hildon_time_picker_get_type( void )
{
  static GType picker_type = 0;

  if( !picker_type )
  {
    static const GTypeInfo picker_info =
      {
        sizeof(HildonTimePickerClass),
        NULL,       /* base_init */
        NULL,       /* base_finalize */
        (GClassInitFunc)hildon_time_picker_class_init,
        NULL,       /* class_finalize */
        NULL,       /* class_data */
        sizeof(HildonTimePicker),
        0,          /* n_preallocs */
        (GInstanceInitFunc)hildon_time_picker_init,
      };
    picker_type = g_type_register_static( GTK_TYPE_DIALOG, "HildonTimePicker",
                                          &picker_info, 0 );
  }
  return picker_type;
}


static void
hildon_time_picker_class_init( HildonTimePickerClass *klass )
{
  GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  parent_class = g_type_class_peek_parent( klass );

  gobject_class->finalize = hildon_time_picker_finalize;
  gobject_class->get_property = hildon_time_picker_get_property;
  gobject_class->set_property = hildon_time_picker_set_property;
  widget_class->map = hildon_time_picker_map;

  /**
   * HildonTimePicker:minutes:
   *
   * Currently selected time in minutes since midnight.
   */
  g_object_class_install_property( gobject_class, PROP_MINUTES,
                                   g_param_spec_uint("minutes",
                                     "Current minutes",
                                     "The selected time in minutes "
                                     "since midnight",
                                     0, MINS_IN_24H, 0,
                                     G_PARAM_READABLE | G_PARAM_WRITABLE) );

  gtk_widget_class_install_style_property( widget_class,
				                               g_param_spec_uint("arrow-width",
                                         _("Arrow width"),
                                         _("Increase/decrease arrows width."),
                                         0, G_MAXUINT,
                                         DEFAULT_ARROW_WIDTH,
                                         G_PARAM_READABLE) );

  gtk_widget_class_install_style_property( widget_class,
				                               g_param_spec_uint("arrow-height",
                                         _("Arrow height"),
                                         _("Increase/decrease arrows height."),
                                         0, G_MAXUINT,
                                         DEFAULT_ARROW_HEIGHT,
                                         G_PARAM_READABLE) );

  g_type_class_add_private( klass, sizeof(HildonTimePickerPrivate) );
}

static void hildon_time_picker_init( HildonTimePicker *picker )
{
  HildonTimePickerPrivate *priv = HILDON_TIME_PICKER_GET_PRIVATE(picker);
  gint widget_group_table_column_pos[WIDGET_GROUP_COUNT];
  GtkSettings *settings = NULL;
  GtkDialog *dialog = GTK_DIALOG(picker);
  GtkTable *table = NULL;
  GtkWidget *maintocenter, *colon_label;
  const struct tm *local = NULL;
  time_t stamp;
  gint i = 0;
  GtkSizeGroup *size_group;

  picker->priv = priv;

  widget_group_table_column_pos[WIDGET_GROUP_HOURS] = 1;
  widget_group_table_column_pos[WIDGET_GROUP_10_MINUTES] = 3;
  widget_group_table_column_pos[WIDGET_GROUP_1_MINUTES] = 4;
  widget_group_table_column_pos[WIDGET_GROUP_AMPM] = 5;

  /* Get AM/PM strings from locale. If they're set, the time is wanted
     in 12 hour mode. */
  priv->am_symbol = g_strdup(nl_langinfo(AM_STR));
  priv->pm_symbol = g_strdup(nl_langinfo(PM_STR));

  priv->show_ampm = priv->am_symbol[0] != '\0';
  if (priv->show_ampm)
  {
    /* Check if AM/PM should be before or after time.
       %p is the AM/PM string, so we assume that if the format string
       begins with %p it's in the beginning, and in any other case it's
       in the end (although that's not necessarily the case). */
    if (strncmp(nl_langinfo(T_FMT_AMPM), "%p", 2) == 0)
    {
      /* Before time. Update column position. */
      priv->ampm_left = TRUE;
      widget_group_table_column_pos[WIDGET_GROUP_AMPM] = 0;
    }
  }

  gtk_widget_push_composite_child();

  /* Pack all our internal widgets into a table */
  table = GTK_TABLE(gtk_table_new(3, 6, FALSE));

  /* Put everything centered into window */
  maintocenter = gtk_alignment_new( 0.5, 0, 0, 0 );

  /* Create our internal widgets */
  for (i = 0; i < WIDGET_GROUP_COUNT; i++)
  {
    HildonTimePickerWidgetGroup *group = &priv->widgets[i];
    gint table_column = widget_group_table_column_pos[i];

    /* Create frame and attach to table. With AM/PM label we're attaching
       it later. */
    group->frame = gtk_frame_new(NULL);
    if (i != WIDGET_GROUP_AMPM)
    {
      gtk_table_attach(table, group->frame, table_column, table_column + 1,
                       1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
    }
    /* FIXME: is it needed to force it to 0 here? */
    gtk_container_set_border_width(GTK_CONTAINER(group->frame), 0);

    /* Create eventbox inside frame */
    group->eventbox = gtk_event_box_new();
    gtk_container_add(GTK_CONTAINER(group->frame), group->eventbox);

    g_object_set(group->eventbox, "can-focus", TRUE, NULL);
    gtk_widget_set_events(group->eventbox,
                          GDK_FOCUS_CHANGE_MASK | GDK_BUTTON_PRESS_MASK );

    /* Connect signals to eventbox */
    g_signal_connect(group->eventbox, "key-release-event",
                     G_CALLBACK(hildon_time_picker_event_box_key_release),
                     picker);
    g_signal_connect(group->eventbox, "key-press-event",
                     G_CALLBACK(hildon_time_picker_event_box_key_press),
                     picker);
    g_signal_connect(group->eventbox, "focus-in-event",
                     G_CALLBACK(hildon_time_picker_event_box_focus_in),
                     picker);
    g_signal_connect(group->eventbox, "focus-out-event",
                     G_CALLBACK(hildon_time_picker_event_box_focus_out),
                     picker);
    g_signal_connect(group->eventbox, "button-press-event",
                     G_CALLBACK(hildon_time_picker_event_box_button_press),
                     picker);

    /* Create label inside eventbox */
    group->label = GTK_LABEL(gtk_label_new(NULL));
    gtk_container_add(GTK_CONTAINER(group->eventbox), GTK_WIDGET(group->label));

    if (i != WIDGET_GROUP_AMPM)
    {
      gint button;

      /* Add some padding to hour and minute labels, and make them bigger */
      gtk_misc_set_padding(GTK_MISC(group->label),
                           HILDON_TIME_PICKER_LABEL_X_PADDING,
                           HILDON_TIME_PICKER_LABEL_Y_PADDING);
      gtk_widget_set_name(GTK_WIDGET(group->label), "osso-LargeFont");

      /* Create up and down buttons for hours and mins */
      for (button = 0; button < BUTTON_COUNT; button++)
      {
        gint table_row = button == BUTTON_UP ? 0 : 2;

        group->buttons[button] = gtk_button_new();
        gtk_table_attach(table, group->buttons[button],
                         table_column, table_column + 1,
                         table_row, table_row + 1,
                         GTK_SHRINK, GTK_SHRINK, 0, 0);
        g_object_set(group->buttons[button], "can-focus", FALSE, NULL);

        /* Connect signals */
        g_signal_connect(group->buttons[button], "button-press-event",
                         G_CALLBACK(hildon_time_picker_arrow_press), picker);
        g_signal_connect(group->buttons[button], "button-release-event",
                         G_CALLBACK(hildon_time_picker_arrow_release), picker);
      }

      gtk_widget_set_name(group->buttons[BUTTON_UP],
                          "hildon-time-picker-up");
      gtk_widget_set_name(group->buttons[BUTTON_DOWN],
                          "hildon-time-picker-down");
    }
  }

  /* Label between hour and minutes */
  colon_label = gtk_label_new(_("Ecdg_ti_time_picker_separator"));
  gtk_table_attach(table, colon_label, 2, 3, 1, 2,
                   GTK_SHRINK, GTK_SHRINK, 6, 0); /* FIXME: magic */
  gtk_widget_set_name(colon_label, "osso-LargeFont" );

  priv->minutes = 0;
  priv->mul = 0;
  priv->key_repeat = 0;
  priv->start_key_repeat = FALSE;
  priv->timer_id = 0;
  priv->button_press = FALSE;

  gtk_table_set_row_spacing( table, 0, 6 );
  gtk_table_set_row_spacing( table, 1, 6 );

  /* Put minute labels and buttons into same size group, so they each have
     the exact same horizontal width. */
  size_group = gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );
  for (i = WIDGET_GROUP_10_MINUTES; i < WIDGET_GROUP_1_MINUTES; i++)
  {
    gtk_size_group_add_widget(size_group, priv->widgets[i].frame);
    gtk_size_group_add_widget(size_group, priv->widgets[i].buttons[BUTTON_UP]);
    gtk_size_group_add_widget(size_group,
                              priv->widgets[i].buttons[BUTTON_DOWN]);
  }
  g_object_unref( size_group ); /* Added widgets hold references */

  if (priv->show_ampm)
  {
    gint table_column = widget_group_table_column_pos[WIDGET_GROUP_AMPM];
    GtkWidget *ampmtotop = NULL;

    /* Show the AM/PM label centered vertically */
    ampmtotop = gtk_alignment_new( 0, 0.5, 0, 0 );
    gtk_table_attach(table, ampmtotop, table_column, table_column + 1,
                     1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
    gtk_container_add(GTK_CONTAINER(ampmtotop),
                      priv->widgets[WIDGET_GROUP_AMPM].frame);

    if (table_column != 0)
      gtk_table_set_col_spacing(table, table_column - 1, 9);

    /* Connect AM/PM signal handlers */
    g_signal_connect(priv->widgets[WIDGET_GROUP_AMPM].eventbox,
                     "button-release-event",
                     G_CALLBACK(hildon_time_picker_ampm_release), picker);
  }

  gtk_widget_pop_composite_child();

  /* Get button press repeater timeout from settings (in milliseconds) */
  settings = gtk_settings_get_default();
  g_object_get( settings, "gtk-update-timeout", &priv->key_repeat, NULL );

  /* This dialog isn't modal */
  gtk_window_set_modal( GTK_WINDOW(dialog), FALSE );
  /* And final dialog packing */
  gtk_dialog_set_has_separator( dialog, FALSE );
  gtk_dialog_add_button( dialog, _("ecdg_bd_time_picker_close"),
                         GTK_RESPONSE_OK );

  gtk_container_add( GTK_CONTAINER(maintocenter), GTK_WIDGET(table) );
  gtk_box_pack_start( GTK_BOX(dialog->vbox), maintocenter, TRUE, FALSE, 0 );

  /* Set default time to current time */
  stamp = time( NULL );
  local = localtime( &stamp );
  hildon_time_picker_set_time( picker, local->tm_hour, local->tm_min );

  gtk_widget_show_all( maintocenter );
}

static void
hildon_time_picker_set_property( GObject *object, guint param_id,
			                            const GValue *value, GParamSpec *pspec )
{
  HildonTimePicker *picker = HILDON_TIME_PICKER(object);

  switch( param_id )
  {
    case PROP_MINUTES:
      hildon_time_picker_change_time( picker, g_value_get_uint(value) );
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static void
hildon_time_picker_finalize( GObject *object )
{
  HildonTimePicker *picker = HILDON_TIME_PICKER(object);

  /* Make sure the timer is stopped */
  if (picker->priv->timer_id)
    g_source_remove(picker->priv->timer_id);

  g_free(picker->priv->am_symbol);
  g_free(picker->priv->pm_symbol);

  if (G_OBJECT_CLASS(parent_class)->finalize)
    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
hildon_time_picker_get_property( GObject *object, guint param_id,
			                            GValue *value, GParamSpec *pspec )
{
  HildonTimePickerPrivate *priv = HILDON_TIME_PICKER(object)->priv;

  switch( param_id )
  {
    case PROP_MINUTES:
      g_value_set_uint( value, priv->minutes );
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static void
hildon_time_picker_map( GtkWidget *widget )
{
  guint width, height;
  gint i, button;
  HildonTimePickerPrivate *priv = HILDON_TIME_PICKER(widget)->priv;

  /* Widget is now mapped. Set border for the dialog. */
  gdk_window_set_decorations( widget->window, GDK_DECOR_BORDER );

  /* Update hour/minute up/down buttons sizes from style properties */
  gtk_widget_style_get( widget,
                        "arrow-width", &width,
                        "arrow-height", &height, NULL );
  for (i = 0; i < WIDGET_GROUP_COUNT; i++)
  {
    if (priv->widgets[i].buttons[0] != NULL)
    {
      for (button = 0; button < BUTTON_COUNT; button++)
      {
        gtk_widget_set_size_request(priv->widgets[i].buttons[button],
                                    width, height);
      }
    }
  }

  GTK_WIDGET_CLASS(parent_class)->map( widget );
}


static gboolean
hildon_time_picker_event_box_button_press( GtkWidget *widget,
                                           GdkEventKey *event, gpointer unused )
{
  /* Clicked hour/minute field. Move focus to it. */
  gtk_widget_grab_focus( widget );
  return FALSE;
}

static gboolean
hildon_time_picker_ampm_release( GtkWidget *widget, GdkEvent *event,
                                 HildonTimePicker *picker )
{
  /* Clicked AM/PM label. Move focus to it and move the time by 12 hours. */
  gtk_widget_grab_focus( widget );
  hildon_time_picker_change_time( picker, picker->priv->minutes > MINS_IN_12H ?
                                  picker->priv->minutes - MINS_IN_12H :
                                  picker->priv->minutes + MINS_IN_12H );
  return FALSE;
}

static gboolean
hildon_time_picker_arrow_press( GtkWidget *widget, GdkEvent *event,
                                HildonTimePicker *picker )
{
  HildonTimePickerPrivate *priv = picker->priv;
  gint i, button;
  gint newval = 0;

  /* Make sure we don't add repeat timer twice. Normally it shouldn't
     happen but WM can cause button release to be lost. */
  if( priv->button_press )
    return FALSE;

  priv->start_key_repeat = priv->button_press = TRUE;

  /* Find the widget which was clicked */
  priv->mul = 0;
  for (i = 0; i < WIDGET_GROUP_COUNT; i++)
  {
    for (button = 0; button < BUTTON_COUNT; button++)
    {
      if (priv->widgets[i].buttons[button] == widget)
      {
        /* Update multiplier and move the focus to the clicked field */
        priv->mul = button_multipliers[i][button];
        gtk_widget_grab_focus(priv->widgets[i].eventbox);
        break;
      }
    }
  }
  g_assert(priv->mul != 0);

  /* Change the time now, wrapping if needed. */
  newval = priv->minutes + priv->mul;
  if( newval < 0 )
    newval += MINS_IN_24H;

  hildon_time_picker_change_time( picker, newval );

  /* Keep changing the time as long as button is being pressed.
     The first repeat takes 3 times longer to start than the rest. */
  priv->timer_id = g_timeout_add(priv->key_repeat * 3,
                                 hildon_time_picker_key_repeat_timeout,
                                 picker);
  return FALSE;
}

static gboolean
hildon_time_picker_arrow_release( GtkWidget *widget, GdkEvent *event,
                                  HildonTimePicker *picker )
{
  HildonTimePickerPrivate *priv = picker->priv;
  if( priv->timer_id )
  {
    /* Stop repeat timer */
    g_source_remove( priv->timer_id );
    priv->timer_id = 0;
  }
  priv->button_press = FALSE;
  return FALSE;
}

static gboolean
hildon_time_picker_event_box_focus_in( GtkWidget *widget, GdkEvent *event,
                                       gpointer unused )
{
  /* Draw the widget in selected state so focus shows clearly. */
  gtk_widget_set_state( widget, GTK_STATE_SELECTED );
  return FALSE;
}

static gboolean
hildon_time_picker_event_box_focus_out( GtkWidget *widget, GdkEvent *event,
                                        gpointer unused )
{
  /* Draw the widget in normal state */
  gtk_widget_set_state( widget, GTK_STATE_NORMAL );
  return FALSE;
}

static gint
hildon_time_picker_lookup_eventbox_group(HildonTimePicker *picker,
                                         GtkWidget *widget)
{
  gint i;

  for (i = 0; i < WIDGET_GROUP_COUNT; i++)
  {
    if (picker->priv->widgets[i].eventbox == widget)
      return i;
  }
  return -1;
}

static gboolean
hildon_time_picker_event_box_key_press( GtkWidget *widget, GdkEventKey *event,
                                        HildonTimePicker *picker )
{
  HildonTimePickerPrivate *priv = picker->priv;
  HildonTimePickerWidgetGroup *group;
  gint group_idx;

  /* If mouse button is already being pressed, ignore this keypress */
  if( priv->timer_id )
    return TRUE;

  group_idx = hildon_time_picker_lookup_eventbox_group(picker, widget);
  group = group_idx < 0 ? NULL : &picker->priv->widgets[group_idx];

  /* Handle keypresses in hour/minute/AMPM fields */
  switch( event->keyval )
  {
    case GDK_Up:
    case GDK_Down:
      if (group != NULL)
      {
        gint button = event->keyval == GDK_Up ? BUTTON_UP : BUTTON_DOWN;

        if (group->buttons[button] != NULL)
        {
          /* Fake a button up/down press */
          hildon_time_picker_arrow_press(group->buttons[button], NULL, picker);
          gtk_widget_set_state(group->buttons[button], GTK_STATE_SELECTED);
        }
        else
        {
          /* Fake a AM/PM button release */
          g_assert(group_idx == WIDGET_GROUP_AMPM);
          hildon_time_picker_ampm_release(group->eventbox, NULL, picker);
        }
      }
      return TRUE;

    case GDK_Left:
      /* If we're in leftmost field, stop this keypress signal.
         Otherwise let the default key handler move focus to field in left. */
      if (priv->show_ampm && priv->ampm_left)
      {
        /* AM/PM is the leftmost field */
        if (group_idx == WIDGET_GROUP_AMPM)
          return TRUE;
      }
      else
      {
        /* Hours is the leftmost field */
        if (group_idx == WIDGET_GROUP_HOURS)
          return TRUE;
      }
      break;

    case GDK_Right:
      /* If we're in rightmost field, stop this keypress signal.
         Otherwise let the default key handler move focus to field in right. */
      if (priv->show_ampm && !priv->ampm_left)
      {
        /* AM/PM is the rightmost field */
        if (group_idx == WIDGET_GROUP_AMPM)
          return TRUE;
      }
      else
      {
        /* 1-minutes is the leftmost field */
        if (group_idx == WIDGET_GROUP_1_MINUTES)
          return TRUE;
      }
      break;
    
    case GDK_Escape:
      gtk_dialog_response (GTK_DIALOG (picker), GTK_RESPONSE_CANCEL);
      return TRUE;

    case GDK_Return:
      gtk_dialog_response (GTK_DIALOG (picker), GTK_RESPONSE_OK);
      return TRUE;
  }

  return FALSE;
}

static gboolean
hildon_time_picker_event_box_key_release( GtkWidget *widget, GdkEventKey *event,
                                          HildonTimePicker *picker )
{
  HildonTimePickerWidgetGroup *group;
  gint group_idx;

  /* Fake a button release if in key-press handler we faked a button press. */
  switch( event->keyval )
  {
    case GDK_Up:
    case GDK_Down:
      group_idx = hildon_time_picker_lookup_eventbox_group(picker, widget);
      if (group_idx >= 0)
      {
        gint button = event->keyval == GDK_Up ? BUTTON_UP : BUTTON_DOWN;

        group = &picker->priv->widgets[group_idx];
        if (group->buttons[button] != NULL)
        {
          /* Fake a button up/down press */
          gtk_widget_set_state(group->buttons[button], GTK_STATE_NORMAL);
          hildon_time_picker_arrow_release(group->buttons[button],
                                           NULL, picker);
        }
      }
      break;
  }
  return FALSE;
}

/* Button up/down is being pressed. Update the time. */
static gboolean
hildon_time_picker_key_repeat_timeout( gpointer tpicker )
{
  HildonTimePicker *picker = HILDON_TIME_PICKER(tpicker);
  HildonTimePickerPrivate *priv = NULL;
  gint newval = 0;

  g_assert(picker != NULL);

  priv = picker->priv;

  /* Change the time, wrapping if needed */
  newval = priv->minutes + priv->mul;
  if( newval < 0 )
    newval += MINS_IN_24H;

  hildon_time_picker_change_time( picker, newval );

  if( priv->start_key_repeat )
  {
    /* This is the first repeat. Shorten the timeout to key_repeat
       (instead of the first time's 3*key_repeat) */
    priv->timer_id = g_timeout_add(priv->key_repeat,
                                   hildon_time_picker_key_repeat_timeout,
                                   picker);
    priv->start_key_repeat = FALSE;
    return FALSE;
  }

  return TRUE;
}



static void
hildon_time_picker_change_time( HildonTimePicker *picker, guint minutes )
{
  HildonTimePickerPrivate *priv = picker->priv;
  gchar str[3] = "00";
  guint hours = 0;
  gboolean ampm = TRUE;

  /* If the minutes isn't in valid range, wrap them. */
  minutes %= MINS_IN_24H;

  if( priv->minutes == minutes )
    return;

  /* Minutes changed. Update widgets to show the new time. */
  priv->minutes = minutes;

  if (priv->show_ampm)
  {
    /* am < 12:00 <= pm */
    ampm = !((guint)(minutes / MINS_IN_12H));
    /* 12:00 - 23:59 -> 00:00 - 11:59 */
    minutes %= MINS_IN_12H;
    if( minutes < MINS_IN_1H )
      /* 00:mm is always shown as 12:mm */
      minutes += MINS_IN_12H;

    /* Update the AM/PM label */
    gtk_label_set_text(priv->widgets[WIDGET_GROUP_AMPM].label,
                       ampm ? priv->am_symbol : priv->pm_symbol);
  }

  /* Update hour and minute fields */
  hours = minutes / MINS_IN_1H;
  minutes %= MINS_IN_1H;

  snprintf(str, sizeof(str), "%02d", hours);
  gtk_label_set_text(priv->widgets[WIDGET_GROUP_HOURS].label, str);

  snprintf(str, sizeof(str), "%d", minutes / 10);
  gtk_label_set_text(priv->widgets[WIDGET_GROUP_10_MINUTES].label, str);

  snprintf(str, sizeof(str), "%d", minutes % 10);
  gtk_label_set_text(priv->widgets[WIDGET_GROUP_1_MINUTES].label, str);

  g_object_notify( G_OBJECT(picker), "minutes" );
}

/**
 * hildon_time_picker_new:
 * @parent: parent window.
 *
 * Hildon Time Picker shows time picker dialog. Close button is placed
 * in dialog's action area and time picker is placed in dialogs vbox.
 * Actual time picker consists two #GtkLabel fields one for hour and
 * one for minutes, arrow buttons and an AM/PM button. A : is placed
 * between hour and minute fields.
 *
 * Return value: pointer to a new #HildonTimePicker widget.
 **/
GtkWidget *hildon_time_picker_new( GtkWindow *parent )
{
  GtkWidget *widget = g_object_new( HILDON_TYPE_TIME_PICKER,
                                    "minutes", 360, NULL );

  if( parent )
    gtk_window_set_transient_for( GTK_WINDOW(widget), parent );

  return GTK_WIDGET(widget);
}

/**
 * hildon_time_picker_set_time:
 * @picker: the #HildonTimePicker widget.
 * @hours: hours
 * @minutes: minutes
 *
 * Sets the time of the #HildonTimePicker widget.
 **/
void hildon_time_picker_set_time( HildonTimePicker *picker,
                                  guint hours, guint minutes )
{
  g_return_if_fail( HILDON_IS_TIME_PICKER(picker) );
  hildon_time_picker_change_time( picker, hours * MINS_IN_1H + minutes );
}

/**
 * hildon_time_picker_get_time:
 * @picker: the #HildonTimePicker widget.
 * @hours: hours
 * @minutes: minutes
 *
 * Gets the time of the #HildonTimePicker widget.
 **/
void hildon_time_picker_get_time( HildonTimePicker *picker,
                                  guint *hours, guint *minutes )
{
  guint current;
  g_return_if_fail( HILDON_IS_TIME_PICKER(picker) );

  current = picker->priv->minutes;
  *hours = current / MINS_IN_1H;
  *minutes = current % MINS_IN_1H;
}
