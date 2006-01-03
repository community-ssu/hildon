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

enum
{
  TABLE,

  FRAME_HOURS,
  FRAME_MMINUTES,
  FRAME_LMINUTES,
  FRAME_AMPM,

  EBOX_HOURS,
  EBOX_MMINUTES,
  EBOX_LMINUTES,
  EBOX_AMPM,

  LABEL_HOURS,
  LABEL_MMINUTES,
  LABEL_LMINUTES,
  LABEL_AMPM,
  LABEL_COLON,

  BUTTON_HOURS_UP,
  BUTTON_HOURS_DOWN,
  BUTTON_MMINUTES_UP,
  BUTTON_MMINUTES_DOWN,
  BUTTON_LMINUTES_UP,
  BUTTON_LMINUTES_DOWN,

  WIDGET_COUNT
};

static GtkDialogClass *parent_class;

/*< private > m == minutes and h == hours
              m == more significant and l == less significant
              mm == minutes more significant (==tens of minutes) etc.*/
struct _HildonTimePickerPrivate
{
  GtkWidget *child[WIDGET_COUNT];

  gchar *am_symbol;
  gchar *pm_symbol;

  guint key_repeat;
  guint minutes; /* time in minutes since midnight */
  gint mul;
  guint timer_id;

  guint c12h             : 1; /* 12 hour clock, show AM/PM */
  guint ampm_pos         : 1; /* FALSE = AM/PM at left, TRUE = at right */
  guint button_press     : 1;
  guint start_key_repeat : 1;
};

enum
{
  PROP_MINUTES = 1,
  PROP_AMPM /* FIXME: unused */
};


static void
hildon_time_picker_class_init( HildonTimePickerClass *klass );

static void
hildon_time_picker_init( HildonTimePicker *picker );

static gboolean
hildon_time_picker_timeout( gpointer data );

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
hildon_time_picker_get_property( GObject *object, guint param_id,
			                           GValue *value, GParamSpec *pspec );

static void
hildon_time_picker_set_property( GObject *object, guint param_id,
			                           const GValue *value, GParamSpec *pspec );

static gboolean
hildon_time_picker_event_box_focus_in( GtkWidget *widget, GdkEvent *event,
                                       gpointer data );

static gboolean
hildon_time_picker_event_box_focus_out( GtkWidget *widget, GdkEvent *event,
                                        gpointer data );

static gboolean
hildon_time_picker_event_box_key_press( GtkWidget *widget,  GdkEventKey *event,
                                        HildonTimePicker *picker );

static gboolean
hildon_time_picker_event_box_key_release( GtkWidget *widget,  GdkEventKey *event,
                                          HildonTimePicker *picker );

static gboolean
hildon_time_picker_event_box_press( GtkWidget *widget,  GdkEventKey *event,
                                    gpointer data );

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
                                     0, 1440, 0,
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
  GtkSettings *settings = NULL;
  GtkDialog *dialog = GTK_DIALOG(picker);
  GtkTable *table = NULL;
  GtkWidget *maintocenter;
  struct tm *local = NULL;
  time_t stamp;
  gint i = 0;
  GtkSizeGroup *size_group;

  picker->priv = priv;

  gtk_widget_push_composite_child();

  /* Pack all our internal widgets into a table */
  priv->child[TABLE] = gtk_table_new( 3, 6, FALSE );
  table = GTK_TABLE(priv->child[TABLE]);

  /* Put everything centered into window */
  maintocenter = gtk_alignment_new( 0.5, 0, 0, 0 );

  /* Create our internal widgets */
  for( i = FRAME_HOURS; i <= FRAME_LMINUTES; i++ )
  {
    priv->child[i] = gtk_frame_new( NULL );
    gtk_container_set_border_width( GTK_CONTAINER( priv->child[i] ), 0 );
  }

  for( i = BUTTON_HOURS_UP; i <= BUTTON_LMINUTES_DOWN; i++ )
    priv->child[i] = gtk_button_new();

  for( i = EBOX_HOURS; i <= EBOX_LMINUTES; i++ )
    priv->child[i] = gtk_event_box_new();

  for( i = LABEL_HOURS; i <= LABEL_LMINUTES; i++ )
  {
    /* minute fields are "0" initially */
    priv->child[i] = gtk_label_new( "0" );
    /* Add 1 pixel of padding on top and bottom of labels */
    gtk_misc_set_padding( GTK_MISC( priv->child[i] ), 0, 1 );
    gtk_widget_set_name( priv->child[i], "osso-LargeFont" );
  }

  /* Hour field defaults to "00" */
  gtk_label_set_text (GTK_LABEL (priv->child[LABEL_HOURS]), "00");

  /* Label between hour and minutes */
  priv->child[LABEL_COLON] = gtk_label_new(_("Ecdg_ti_time_picker_separator"));
  gtk_widget_set_name( priv->child[LABEL_COLON], "osso-LargeFont" );

  priv->minutes = 0;
  priv->mul = 0;
  priv->key_repeat = 0;
  priv->start_key_repeat = FALSE;
  priv->timer_id = 0;
  priv->button_press = FALSE;

  /* Get AM/PM strings from locale. If they're set, the time is wanted
     in 12 hour mode. */
  priv->am_symbol = nl_langinfo(AM_STR);
  priv->pm_symbol = nl_langinfo(PM_STR);

  priv->c12h = priv->am_symbol[0] == '\0' ? FALSE : TRUE;

  /* Get current time for initializing fields */
  stamp = time( NULL );
  local = localtime( &stamp );

  /* Pack widgets into table */
  gtk_table_attach( table, priv->child[BUTTON_HOURS_UP],      1, 2, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[FRAME_HOURS],          1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[BUTTON_HOURS_DOWN],    1, 2, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[LABEL_COLON],          2, 3, 1, 2, GTK_SHRINK, GTK_SHRINK, 6, 0 );
  gtk_table_attach( table, priv->child[BUTTON_MMINUTES_UP],   3, 4, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[FRAME_MMINUTES],       3, 4, 1, 2, GTK_FILL,   GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[BUTTON_MMINUTES_DOWN], 3, 4, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[BUTTON_LMINUTES_UP],   4, 5, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[FRAME_LMINUTES],       4, 5, 1, 2, GTK_FILL,   GTK_SHRINK, 0, 0 );
  gtk_table_attach( table, priv->child[BUTTON_LMINUTES_DOWN], 4, 5, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0 );

  gtk_table_set_row_spacing( table, 0, 6 );
  gtk_table_set_row_spacing( table, 1, 6 );

  /* Put minute labels and buttons into same size group, so they each have
     the exact same horizontal width. */
  size_group = gtk_size_group_new( GTK_SIZE_GROUP_HORIZONTAL );
  gtk_size_group_add_widget( size_group, priv->child[BUTTON_MMINUTES_UP] );
  gtk_size_group_add_widget( size_group, priv->child[FRAME_MMINUTES] );
  gtk_size_group_add_widget( size_group, priv->child[BUTTON_MMINUTES_DOWN] );
  gtk_size_group_add_widget( size_group, priv->child[BUTTON_LMINUTES_UP] );
  gtk_size_group_add_widget( size_group, priv->child[FRAME_LMINUTES] );
  gtk_size_group_add_widget( size_group, priv->child[BUTTON_LMINUTES_DOWN] );
  g_object_unref( size_group ); /* Added widgets hold references */

  if( priv->c12h )
  {
    GtkWidget *ampmtotop = NULL;
    guint placement = 0;

    /* 12h clock mode. Check if AM/PM should be before or after time.
       %p is the AM/PM string, so we assume that if the format string
       begins with %p it's in the beginning, and in any other case it's
       in the end (although that's not necessarily the case). */
    priv->ampm_pos = strncmp(nl_langinfo(T_FMT_AMPM), "%p", 2) ? TRUE : FALSE;
    /* Show the AM/PM label centered vertically */
    ampmtotop = gtk_alignment_new( 0, 0.5, 0, 0 );
    priv->child[FRAME_AMPM] = gtk_frame_new( NULL );
    priv->child[EBOX_AMPM] = gtk_event_box_new();
    priv->child[LABEL_AMPM] = gtk_label_new( priv->pm_symbol );

    /* Pack AM/PM widgets. If AM/PM is before time, pack them into table
       position 0..1. If it's after time, pack to 5..6 */
    placement = priv->ampm_pos * 5;

    gtk_container_add( GTK_CONTAINER(ampmtotop), priv->child[FRAME_AMPM] );
    gtk_container_add( GTK_CONTAINER(priv->child[FRAME_AMPM]),
                       priv->child[EBOX_AMPM] );
    gtk_container_add( GTK_CONTAINER(priv->child[EBOX_AMPM]),
                       priv->child[LABEL_AMPM] );
    gtk_table_attach( table, ampmtotop, placement, placement + 1, 1, 2,
                      GTK_SHRINK, GTK_SHRINK, 0, 0 );
    gtk_table_set_col_spacing( table, placement - 1, 6 );

    /* Connect AM/PM signal handlers */
    g_signal_connect( G_OBJECT(priv->child[EBOX_AMPM]), "button-release-event",
                      (GCallback)hildon_time_picker_ampm_release, picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_AMPM]), "focus-out-event",
                      (GCallback)hildon_time_picker_event_box_focus_out,
                      picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_AMPM]), "focus-in-event",
                      (GCallback)hildon_time_picker_event_box_focus_in, picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_AMPM]), "key-release-event",
                      (GCallback)hildon_time_picker_event_box_key_release,
                      picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_AMPM]), "key-press-event",
                      (GCallback)hildon_time_picker_event_box_key_press,
                      picker );

    /* Set AM/PM evenbox focusable */
    g_object_set( G_OBJECT(priv->child[EBOX_AMPM]), "can-focus", TRUE, NULL );
    gtk_widget_set_events( priv->child[EBOX_AMPM],
                           GDK_FOCUS_CHANGE_MASK | GDK_BUTTON_PRESS_MASK );
  }
  else
    priv->child[FRAME_AMPM] = priv->child[LABEL_AMPM] =
                              priv->child[EBOX_AMPM] = NULL;

  gtk_widget_pop_composite_child();

  /* Set default time to current time */
  hildon_time_picker_set_time( picker, local->tm_hour, local->tm_min );

  /* Get button press repeater timeout from settings (in milliseconds) */
  settings = gtk_settings_get_default();
  g_object_get( settings, "gtk-update-timeout", &priv->key_repeat, NULL );

  /* Handle hour and minute fields */
  for( i = 0; i < 3; i++ )
  {
    gtk_container_add( GTK_CONTAINER(priv->child[FRAME_HOURS + i]),
                       priv->child[EBOX_HOURS + i] );
    gtk_container_add( GTK_CONTAINER(priv->child[EBOX_HOURS + i]),
                       priv->child[LABEL_HOURS + i] );

    /* Connect signals */
    g_signal_connect( G_OBJECT(priv->child[EBOX_HOURS + i]), "key-release-event",
                      (GCallback)hildon_time_picker_event_box_key_release,
                      picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_HOURS + i]), "key-press-event",
                      (GCallback)hildon_time_picker_event_box_key_press,
                      picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_HOURS + i]), "focus-in-event",
                      (GCallback)hildon_time_picker_event_box_focus_in, picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_HOURS + i]), "focus-out-event",
                      (GCallback)hildon_time_picker_event_box_focus_out,
                      picker );
    g_signal_connect( G_OBJECT(priv->child[EBOX_HOURS + i]),
                      "button-press-event",
                      (GCallback)hildon_time_picker_event_box_press, NULL );

    /* Name the buttons */
    gtk_widget_set_name( priv->child[BUTTON_HOURS_UP + i*2],
                         "hildon-time-picker-up" );
    gtk_widget_set_name( priv->child[BUTTON_HOURS_DOWN + i*2],
                         "hildon-time-picker-down" );
  }

  /* Handle hour/minute up/down buttons */
  for( i = BUTTON_HOURS_UP; i <= BUTTON_LMINUTES_DOWN; i++ )
  {
    g_object_set( G_OBJECT(priv->child[i]), "can-focus", FALSE, NULL );

    /* Connect signals */
    g_signal_connect( G_OBJECT(priv->child[i]), "button-press-event",
                      (GCallback)hildon_time_picker_arrow_press, picker );
    g_signal_connect( G_OBJECT(priv->child[i]), "button-release-event",
                      (GCallback)hildon_time_picker_arrow_release, picker );
  }

  /* Handle hour/minute eventboxes */
  for( i = EBOX_HOURS; i <= EBOX_LMINUTES; i++ )
  {
    g_object_set( G_OBJECT(priv->child[i]), "can-focus", TRUE, NULL );

    /* Set events */
    gtk_widget_set_events( priv->child[i],
                           GDK_FOCUS_CHANGE_MASK | GDK_BUTTON_PRESS_MASK );
  }

  /* This dialog isn't modal */
  gtk_window_set_modal( GTK_WINDOW(dialog), FALSE );
  /* And final dialog packing */
  gtk_dialog_set_has_separator( dialog, FALSE );
  gtk_dialog_add_button( dialog, _("ecdg_bd_time_picker_close"),
                         GTK_RESPONSE_OK );

  gtk_container_add( GTK_CONTAINER(maintocenter), priv->child[TABLE] );
  gtk_box_pack_start( GTK_BOX(dialog->vbox), maintocenter, TRUE, FALSE, 0 );

  /* FIXME: no point in setting the time to its current value */
  hildon_time_picker_change_time (picker, priv->minutes);

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
  gint i;
  HildonTimePickerPrivate *priv = HILDON_TIME_PICKER(widget)->priv;

  /* Widget is now mapped. Set border for the dialog. */
  gdk_window_set_decorations( widget->window, GDK_DECOR_BORDER );

  /* Update hour/minute up/down buttons sizes from style properties */
  gtk_widget_style_get( widget,
                        "arrow-width", &width,
                        "arrow-height", &height, NULL );
  for( i=BUTTON_HOURS_UP; i <= BUTTON_LMINUTES_DOWN; i++)
    gtk_widget_set_size_request( priv->child[i], width, height );

  GTK_WIDGET_CLASS(parent_class)->map( widget );
}


static gboolean
hildon_time_picker_event_box_press( GtkWidget *widget,  GdkEventKey *event,
                                    gpointer data )
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
  hildon_time_picker_change_time( picker, picker->priv->minutes > 720 ?
                                  picker->priv->minutes - 720 :
                                  picker->priv->minutes + 720 );
  return FALSE;
}

static gboolean
hildon_time_picker_arrow_press( GtkWidget *widget, GdkEvent *event,
                                HildonTimePicker *picker )
{
  HildonTimePickerPrivate *priv = picker->priv;
  gint newval = 0;

  /* Make sure we don't add repeat timer twice. Normally it shouldn't
     happen but WM can cause button release to be lost. */
  if( priv->button_press )
    return FALSE;

  priv->start_key_repeat = priv->button_press = TRUE;

  /* Move the focus to field whose arrow was clicked, and get the minute
     change multiplier. */
  if( widget == priv->child[BUTTON_HOURS_UP] )
  {
    priv->mul = 60;
    gtk_widget_grab_focus( priv->child[EBOX_HOURS] );
  }
  else if( widget == priv->child[BUTTON_MMINUTES_UP] )
  {
    priv->mul = 10;
    gtk_widget_grab_focus( priv->child[EBOX_MMINUTES] );
  }
  else if( widget == priv->child[BUTTON_LMINUTES_UP] )
  {
    priv->mul = 1;
    gtk_widget_grab_focus( priv->child[EBOX_LMINUTES] );
  }
  else if( widget == priv->child[BUTTON_HOURS_DOWN] )
  {
    priv->mul = -60;
    gtk_widget_grab_focus( priv->child[EBOX_HOURS] );
  }
  else if( widget == priv->child[BUTTON_MMINUTES_DOWN] )
  {
    priv->mul = -10;
    gtk_widget_grab_focus( priv->child[EBOX_MMINUTES] );
  }
  else
  {
    priv->mul = -1;
    gtk_widget_grab_focus( priv->child[EBOX_LMINUTES] );
  }

  /* Change the time now, wrapping if needed. */
  newval = priv->minutes + priv->mul;
  if( newval < 0 )
    newval += 1440;

  hildon_time_picker_change_time( picker, newval );
  /* Keep changing the time as long as button is being pressed.
     The first repeat takes 3 times longer to start than the rest. */
  priv->timer_id = g_timeout_add( priv->key_repeat * 3, hildon_time_picker_timeout, picker );
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
                                       gpointer data )
{
  /* Draw the widget in selected state so focus shows clearly. */
  gtk_widget_set_state( widget, GTK_STATE_SELECTED );
  return FALSE;
}

static gboolean
hildon_time_picker_event_box_focus_out( GtkWidget *widget, GdkEvent *event,
                                        gpointer data )
{
  /* Draw the widget in normal state */
  gtk_widget_set_state( widget, GTK_STATE_NORMAL );
  return FALSE;
}

static gboolean
hildon_time_picker_event_box_key_press( GtkWidget *widget, GdkEventKey *event,
                                        HildonTimePicker *picker )
{
  HildonTimePickerPrivate *priv = picker->priv;

  /* If mouse button is already being pressed, ignore this keypress */
  if( priv->timer_id )
    return TRUE;

  /* Handle keypresses in hour/minute/AMPM fields */
  switch( event->keyval )
  {
    case GDK_Up:
      if( widget == priv->child[EBOX_HOURS] )
      {
        /* Fake a hour button up press */
        hildon_time_picker_arrow_press( priv->child[BUTTON_HOURS_UP], NULL,
                                        picker );
        gtk_widget_set_state( priv->child[BUTTON_HOURS_UP], GTK_STATE_SELECTED );
      }
      else if( widget == priv->child[EBOX_MMINUTES] )
      {
        /* Fake a 10-minute button up press */
        hildon_time_picker_arrow_press( priv->child[BUTTON_MMINUTES_UP], NULL,
                                        picker );
        gtk_widget_set_state( priv->child[BUTTON_MMINUTES_UP],
                              GTK_STATE_SELECTED );
      }
      else if( widget == priv->child[EBOX_LMINUTES] )
      {
        /* Fake a minute button up press */
        hildon_time_picker_arrow_press( priv->child[BUTTON_LMINUTES_UP], NULL,
                                        picker );
        gtk_widget_set_state( priv->child[BUTTON_LMINUTES_UP],
                              GTK_STATE_SELECTED );
      }
      else
        /* Fake a AM/PM button release */
        hildon_time_picker_ampm_release( priv->child[EBOX_AMPM], NULL, picker );
      return TRUE;

    case GDK_Down:
      if( widget == priv->child[EBOX_HOURS] )
      {
        /* Fake a hour button down press */
        hildon_time_picker_arrow_press( priv->child[BUTTON_HOURS_DOWN], NULL,
                                        picker );
        gtk_widget_set_state( priv->child[BUTTON_HOURS_DOWN],
                              GTK_STATE_SELECTED );
      }
      else if( widget == priv->child[EBOX_MMINUTES] )
      {
        /* Fake a 10-minute button down press */
        hildon_time_picker_arrow_press( priv->child[BUTTON_MMINUTES_DOWN], NULL,
                                        picker );
        gtk_widget_set_state( priv->child[BUTTON_MMINUTES_DOWN],
                              GTK_STATE_SELECTED );
      }
      else if( widget == priv->child[EBOX_LMINUTES] )
      {
        /* Fake a minute button down press */
        hildon_time_picker_arrow_press( priv->child[BUTTON_LMINUTES_DOWN], NULL,
                                        picker );
        gtk_widget_set_state( priv->child[BUTTON_LMINUTES_DOWN],
                              GTK_STATE_SELECTED );
      }
      else
        /* Fake a AM/PM button release */
        hildon_time_picker_ampm_release( priv->child[EBOX_AMPM], NULL, picker );
      return TRUE;

    case GDK_Left:
      /* If we're in leftmost field, stop this keypress signal.
         Otherwise let the default key handler move focus to field in left. */
      if( priv->c12h )
      {
        if( !priv->ampm_pos )
        {
          if( widget == priv->child[EBOX_AMPM] )
            return TRUE;
        }
        else if( widget == priv->child[EBOX_HOURS] )
          return TRUE;
      }
      else if( widget == priv->child[EBOX_HOURS] )
        return TRUE;

    break;

    case GDK_Right:
      /* If we're in rightmost field, stop this keypress signal.
         Otherwise let the default key handler move focus to field in right. */
      if( priv->c12h )
      {
        if( priv->ampm_pos )
        {
          if( widget == priv->child[EBOX_AMPM] )
            return TRUE;
        }
        else if( widget == priv->child[EBOX_LMINUTES] )
          return TRUE;
      }
      else if( widget == priv->child[EBOX_LMINUTES] )
        return TRUE;
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
  gint i;

  /* Fake a button release if in key-press handler we faked a button press. */
  switch( event->keyval )
  {
    case GDK_Up:
      for( i = BUTTON_HOURS_UP; i <= BUTTON_LMINUTES_UP; i += 2 )
        gtk_widget_set_state( picker->priv->child[i], GTK_STATE_NORMAL );
      hildon_time_picker_arrow_release( widget, NULL, picker );
    break;

    case GDK_Down:
      for( i = BUTTON_HOURS_DOWN; i <= BUTTON_LMINUTES_DOWN; i += 2 )
        gtk_widget_set_state( picker->priv->child[i], GTK_STATE_NORMAL );
      hildon_time_picker_arrow_release( widget, NULL, picker );
      break;
  }
  return FALSE;
}

/* Button up/down is being pressed. Update the time. */
static gboolean
hildon_time_picker_timeout( gpointer data )/* FIXME: use meaningful name */
{
  HildonTimePicker *picker = NULL;
  HildonTimePickerPrivate *priv = NULL;
  gint newval = 0;

  /* FIXME: the following condition never occurs */
  /* luc: function should check its parameters. Otherwise document the use of the function "parameter must not be NULL". */
  if( !data )
    return FALSE;

  picker = HILDON_TIME_PICKER(data);
  priv = picker->priv;

  /* FIXME: the following condition never occurs */
  if( !picker->priv->timer_id )
    return FALSE;

  /* Change the time, wrapping if needed */
  newval = priv->minutes + priv->mul;
  if( newval < 0 )
    newval += 1440;/*FIXME: document, or use 24*60 if this is the case */

  hildon_time_picker_change_time( picker, newval );

  if( priv->start_key_repeat )
  {
    /* This is the first repeat. Shorten the timeout to key_repeat
       (instead of the first time's 3*key_repeat) */
    priv->timer_id = g_timeout_add( priv->key_repeat, hildon_time_picker_timeout, picker );
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
  minutes %= 1440;

  if( priv->minutes == minutes )
    return;

  /* Minutes changed. Update widgets to show the new time. */
  priv->minutes = minutes;

  if( priv->c12h )
  {
    /* am < 12:00 (720 minutes) <= pm */
    ampm = !((guint)(minutes / 720));
    /* 12:00 - 23:59 -> 00:00 - 11:59 */
    minutes %= 720;
    if( minutes < 60 )
      /* 00:mm is always shown as 12:mm */
      minutes += 720;

    /* Update the AM/PM label */
    gtk_label_set_text( GTK_LABEL(priv->child[LABEL_AMPM]),
                        ampm ? priv->am_symbol : priv->pm_symbol );
  }

  /* Update hour and minute fields */
  hours = minutes / 60;
  minutes %= 60;

  sprintf( str, "%02d", hours );
  gtk_label_set_text( GTK_LABEL(priv->child[LABEL_HOURS]), str );

  sprintf( str, "%02d", minutes );
  gtk_label_set_text( GTK_LABEL(priv->child[LABEL_LMINUTES]), &str[1] );

  str[1] = '\0';
  gtk_label_set_text( GTK_LABEL(priv->child[LABEL_MMINUTES]), &str[0] );

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
  hildon_time_picker_change_time( picker, hours * 60 + minutes );
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
  *hours = current / 60;
  *minutes = current % 60;
}
