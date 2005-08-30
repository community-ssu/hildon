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
 * @file hildon-number-editor.c
 *
 * This file contains the implementation of Hildon Number Editor
 *
 */

#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <pango/pango.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hildon-number-editor.h"
#include "hildon-marshalers.h"
#include <hildon-widgets/gtk-infoprint.h>
#include "hildon-composite-widget.h"
#include <hildon-widgets/hildon-input-mode-hint.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

/*Pixel spec defines*/
#define SPACE_BORDER 6
#define NUMBER_EDITOR_HEIGHT 30

#define BUTTON_HEIGHT 30
#define BUTTON_WIDTH 30

#define HILDON_NUMBER_EDITOR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_NUMBER_EDITOR, \
        HildonNumberEditorPrivate));

typedef struct _HildonNumberEditorPrivate HildonNumberEditorPrivate;

static void
hildon_number_editor_class_init (HildonNumberEditorClass *editor_class);

static void
hildon_number_editor_init (HildonNumberEditor *editor);

static gboolean
hildon_number_editor_entry_focusout (GtkWidget *widget, GdkEventFocus *event,
                                     gpointer data);

static void
hildon_number_editor_entry_changed (GtkWidget *widget, gpointer data);

static void
hildon_number_editor_size_request (GtkWidget *widget,
                                   GtkRequisition *requisition);

static void
set_widget_allocation (GtkWidget *widget, GtkAllocation *alloc,
                       GtkAllocation *allocation);

static void
hildon_number_editor_size_allocate (GtkWidget *widget,
                                    GtkAllocation *allocation);

static gboolean
hildon_number_editor_entry_keypress (GtkWidget *widget, GdkEventKey *event,
                                     gpointer data);

static gboolean
hildon_number_editor_button_pressed (GtkWidget *widget, GdkEventButton *event,
                                     gpointer data);

static gboolean
hildon_number_editor_entry_button_released (GtkWidget *widget,
                                           GdkEventButton *event,
                                           gpointer data);
static gboolean
hildon_number_editor_button_released (GtkWidget *widget,
                                      GdkEvent *event,
                                      HildonNumberEditor *editor);
static gboolean
do_mouse_timeout (HildonNumberEditor *editor);

static gboolean
change_numbers (HildonNumberEditor *editor, gint type, gint cursor_pos);

static gchar *
integer_to_string (gint value);

static void
hildon_number_editor_forall (GtkContainer *container, gboolean include_internals,
                     GtkCallback callback, gpointer callback_data);

static void
hildon_number_editor_destroy (GtkObject *self);

static gboolean
hildon_number_editor_start_timer (HildonNumberEditor *editor);

static void
hildon_number_editor_finalize (GObject *self);

static gboolean
hildon_number_editor_mnemonic_activate( GtkWidget *widget,
                                        gboolean group_cycle );
static gboolean
hildon_number_editor_error_handler(HildonNumberEditor *editor,
				   HildonNumberEditorErrorType type);

static gboolean
hildon_number_editor_select_all (HildonNumberEditorPrivate *priv);
    
enum
{
  RANGE_ERROR,

  LAST_SIGNAL
};

static GtkContainerClass *parent_class;

static guint HildonNumberEditor_signal[LAST_SIGNAL] = {0};

struct _HildonNumberEditorPrivate
{
    GtkWidget *num_entry;
    GtkWidget *plus;
    GtkWidget *minus;

    gint start;
    gint end;
    gint default_val;
    gint button_type;
    guint button_event_id;

    gboolean negative;
};


GType hildon_number_editor_get_type(void)
{
    static GType editor_type = 0;

    if (!editor_type)
      {
        static const GTypeInfo editor_info =
          {
            sizeof(HildonNumberEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_number_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonNumberEditor),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_number_editor_init,
          };
        editor_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                             "HildonNumberEditor",
                                             &editor_info, 0);
      }
    return editor_type;
}

static void
hildon_number_editor_class_init(HildonNumberEditorClass * editor_class)
{
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(editor_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(editor_class);

    g_type_class_add_private(editor_class,
                             sizeof(HildonNumberEditorPrivate));

    parent_class = g_type_class_peek_parent(editor_class);

    widget_class->size_request = hildon_number_editor_size_request;
    widget_class->size_allocate = hildon_number_editor_size_allocate;
    widget_class->mnemonic_activate = hildon_number_editor_mnemonic_activate;
    widget_class->focus = hildon_composite_widget_focus;

    editor_class->error_handler = hildon_number_editor_error_handler;

    /* Because we derived our widget from GtkContainer, we should override 
       forall method */
    container_class->forall = hildon_number_editor_forall;
    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_number_editor_destroy;
    G_OBJECT_CLASS(editor_class)->finalize = hildon_number_editor_finalize;

    HildonNumberEditor_signal[RANGE_ERROR] =
      g_signal_new("range_error", HILDON_TYPE_NUMBER_EDITOR,
		   G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET
		   (HildonNumberEditorClass, error_handler),
		   g_signal_accumulator_true_handled, NULL,
		   _hildon_marshal_BOOLEAN__INT,
		   G_TYPE_BOOLEAN, 1, G_TYPE_INT);
}

static void
hildon_number_editor_forall(GtkContainer *container, gboolean include_internals,
                            GtkCallback callback, gpointer callback_data)
{
    HildonNumberEditorPrivate *priv =
        HILDON_NUMBER_EDITOR_GET_PRIVATE(container);

    g_return_if_fail(callback != NULL);              

    if (!include_internals)
        return;

    (*callback) (priv->minus, callback_data);
    (*callback) (priv->num_entry, callback_data);
    (*callback) (priv->plus, callback_data);
}

static void
hildon_number_editor_destroy(GtkObject *self)
{
    HildonNumberEditorPrivate *priv;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(self);

    if (priv->minus)
      {
        gtk_widget_unparent(priv->minus);
        priv->minus = NULL;
      }
    if (priv->num_entry)
      {
        gtk_widget_unparent(priv->num_entry);
        priv->num_entry = NULL;
      }
    if (priv->plus)
      {
        gtk_widget_unparent(priv->plus);
        priv->plus = NULL;
      }

    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        GTK_OBJECT_CLASS(parent_class)->destroy(self);
}

static void
hildon_number_editor_finalize (GObject *self)
{
   HildonNumberEditorPrivate *priv;

   priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(self);

   /* Free timer */
   if (priv->button_event_id)
     g_source_remove (priv->button_event_id);

    /* Call parent class finalize, if have one */
    if (G_OBJECT_CLASS (parent_class)->finalize)
        G_OBJECT_CLASS (parent_class)->finalize(self);
}

static void
hildon_number_editor_init (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
    GTK_WIDGET_SET_FLAGS(GTK_WIDGET(editor), GTK_NO_WINDOW);

    priv->num_entry = gtk_entry_new();
    priv->minus = gtk_button_new();
    priv->plus = gtk_button_new();

    gtk_widget_set_name( priv->minus, "minus-button" );
    gtk_widget_set_name( priv->plus, "plus-button" );
    gtk_widget_set_size_request( priv->minus, BUTTON_WIDTH, BUTTON_HEIGHT );
    gtk_widget_set_size_request( priv->plus, BUTTON_WIDTH, BUTTON_HEIGHT );
    gtk_entry_set_alignment (GTK_ENTRY(priv->num_entry), 1);
    
    GTK_WIDGET_UNSET_FLAGS( priv->minus, GTK_CAN_FOCUS );
    GTK_WIDGET_UNSET_FLAGS( priv->plus, GTK_CAN_FOCUS );
    
    priv->button_event_id = 0;

    gtk_widget_set_parent(priv->minus, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->num_entry, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->plus, GTK_WIDGET(editor));

    g_signal_connect(GTK_OBJECT(priv->num_entry), "changed",
                     G_CALLBACK(hildon_number_editor_entry_changed),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->num_entry), "focus-out-event",
                     G_CALLBACK(hildon_number_editor_entry_focusout),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->num_entry), "key-press-event",
                     G_CALLBACK(hildon_number_editor_entry_keypress),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->num_entry), "button-release-event",
                     G_CALLBACK(hildon_number_editor_entry_button_released),
                     NULL);

    g_signal_connect(GTK_OBJECT(priv->minus), "button-press-event",
                     G_CALLBACK(hildon_number_editor_button_pressed),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->plus), "button-press-event",
                     G_CALLBACK(hildon_number_editor_button_pressed),
                     editor);
                     
    g_signal_connect(GTK_OBJECT(priv->minus), "button-release-event",
                     G_CALLBACK(hildon_number_editor_button_released),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->plus), "button-release-event",
                     G_CALLBACK(hildon_number_editor_button_released),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->minus), "leave-notify-event",
                     G_CALLBACK(hildon_number_editor_button_released),
                     editor);

    g_signal_connect(GTK_OBJECT(priv->plus), "leave-notify-event",
                     G_CALLBACK(hildon_number_editor_button_released),
                     editor);

    g_object_set( G_OBJECT(priv->num_entry),
                  "input-mode", HILDON_INPUT_MODE_HINT_NUMERIC, NULL );
    
    gtk_widget_show(priv->num_entry);
    gtk_widget_show(priv->minus);
    gtk_widget_show(priv->plus);

    hildon_number_editor_set_range(editor, G_MININT, G_MAXINT);
}

static gboolean
hildon_number_editor_mnemonic_activate (GtkWidget *widget,
                                        gboolean group_cycle)
{
  HildonNumberEditorPrivate *priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(widget);
  gtk_widget_grab_focus(priv->num_entry);
  gtk_editable_select_region(GTK_EDITABLE(priv->num_entry), 0, -1);
  return TRUE;
}

static gboolean
hildon_number_editor_entry_button_released (GtkWidget *widget,
                                           GdkEventButton *event,
                                           gpointer data)
{
  gtk_editable_select_region(GTK_EDITABLE(widget), 0, -1);
  return FALSE;
}

static gboolean
hildon_number_editor_button_released (GtkWidget *widget, GdkEvent *event,
                                      HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv =
        HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
        
    if (priv->button_event_id)
      {
        g_source_remove(priv->button_event_id);
        priv->button_event_id = 0;
      }
    return FALSE;
}

static gboolean
hildon_number_editor_button_pressed (GtkWidget *widget, GdkEventButton *event,
                                     gpointer data)
{
    /* Need to fetch current value from entry and increment or decrement
       it */
    HildonNumberEditor *editor;
    HildonNumberEditorPrivate *priv;
    GtkSettings *settings;
    guint timeout;

    editor = HILDON_NUMBER_EDITOR(data);
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
    settings = gtk_settings_get_default();
    g_object_get(settings, "gtk-initial-timeout", &timeout, NULL);

    if (GTK_BUTTON(widget) == GTK_BUTTON(priv->plus))
        priv->button_type = 1;
    else
        priv->button_type = -1;

    if (!priv->button_event_id)
      {
        do_mouse_timeout(editor);
        priv->button_event_id = g_timeout_add (timeout,
                                (GSourceFunc) hildon_number_editor_start_timer,
                                editor);
      }

    return FALSE;
}

static gboolean
hildon_number_editor_start_timer (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;
    GtkSettings *settings;
    guint timeout;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
    settings = gtk_settings_get_default();
    g_object_get(settings, "gtk-update-timeout", &timeout, NULL);

    priv->button_event_id = g_timeout_add(timeout,
                                          (GSourceFunc) do_mouse_timeout,
                                          editor);
    return FALSE;
}

static gboolean
do_mouse_timeout (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;
    gboolean success;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
    success = TRUE;

    if (priv->button_type == 1)
      {
        if (change_numbers(editor, 1, -1) == FALSE)
            success = FALSE;

      }
    else if (change_numbers(editor, 2, -1) == FALSE)
        success = FALSE;

    if (!success)
      {
        gtk_editable_select_region(GTK_EDITABLE(priv->num_entry), 0, -1);
        return FALSE;
      }
    else
        return TRUE;
}

static gboolean
change_numbers (HildonNumberEditor *editor, gint type, gint cursor_pos)
{
    gboolean r;
    gint nvalue;
    gchar *snvalue;
    HildonNumberEditorPrivate *priv =
        HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
    nvalue = atoi(GTK_ENTRY(priv->num_entry)->text);

    if (type == 1)
      {
        if (nvalue < priv->end)
            nvalue += 1;
        else
          {
	    g_signal_emit(editor, HildonNumberEditor_signal[RANGE_ERROR], 
			  0, MAXIMUM_VALUE_EXCEED, &r);
            return FALSE;
          }
      }
    else
      {
        if (nvalue > priv->start)
            nvalue -= 1;

        else
          {
	    g_signal_emit(editor, HildonNumberEditor_signal[RANGE_ERROR], 
			  0, MINIMUM_VALUE_EXCEED, &r);
            return FALSE;
          }
      }

    /* write value to num_entry */
    snvalue = integer_to_string(nvalue);
    gtk_entry_set_text(GTK_ENTRY(priv->num_entry), snvalue);

    if (cursor_pos != -1)
        gtk_editable_set_position(GTK_EDITABLE(priv->num_entry),
                                  cursor_pos);

    if (snvalue)
        g_free(snvalue);

    return TRUE;
}

static gchar *
integer_to_string (gint nvalue)
{
    return g_strdup_printf("%d", nvalue);
}

static void
hildon_number_editor_entry_changed (GtkWidget *widget, gpointer data)
{
  HildonNumberEditor *editor;
  HildonNumberEditorPrivate *priv;
  gchar *tmpstr;
  gint value;
  gchar *tail = NULL;
  gboolean r;

  editor = HILDON_NUMBER_EDITOR(data);
  priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);

  tmpstr = GTK_ENTRY(priv->num_entry)->text;

  if (strlen(tmpstr) > 0)
    {
      tmpstr = NULL;
      value = strtol(GTK_ENTRY(priv->num_entry)->text, &tail, 10);
      if (!strncmp(tail, "\0", 1) || !strncmp(tail, "-", 1))
	{    
	  if (atoi(GTK_ENTRY(priv->num_entry)->text) > priv->end)
	    {
	      g_signal_emit(editor,HildonNumberEditor_signal[RANGE_ERROR], 
			    0, MAXIMUM_VALUE_EXCEED, &r);
	      tmpstr = integer_to_string(priv->end);
	      gtk_entry_set_text(GTK_ENTRY(priv->num_entry), tmpstr);
              g_idle_add ((GSourceFunc)hildon_number_editor_select_all, priv);
	      if (tmpstr)
		g_free(tmpstr);
	    }
	  else if (atoi(GTK_ENTRY(priv->num_entry)->text) < priv->start) {
	    g_signal_emit(editor, HildonNumberEditor_signal[RANGE_ERROR], 
			  0, MINIMUM_VALUE_EXCEED, &r);
	    tmpstr = integer_to_string(priv->start);
	    gtk_entry_set_text(GTK_ENTRY(priv->num_entry), tmpstr);
            g_idle_add ((GSourceFunc)hildon_number_editor_select_all, priv);
	    if (tmpstr)
	      g_free(tmpstr);
	  }
	}
      else
	{
	  g_signal_emit(editor, HildonNumberEditor_signal[RANGE_ERROR], 
			0, ERRONEOUS_VALUE, &r);
	  tmpstr = integer_to_string(priv->start);
	  gtk_entry_set_text(GTK_ENTRY(priv->num_entry), tmpstr);
          g_idle_add ((GSourceFunc)hildon_number_editor_select_all, priv);
	  if (tmpstr)
	    g_free(tmpstr);
	}
    }
}

static void
hildon_number_editor_size_request (GtkWidget *widget,
                                  GtkRequisition *requisition)
{
    HildonNumberEditor *editor;
    HildonNumberEditorPrivate *priv;
    GtkRequisition req;

    editor = HILDON_NUMBER_EDITOR(widget);
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);

    gtk_widget_size_request(priv->minus, &req);
    requisition->width = req.width;

    gtk_widget_size_request(priv->num_entry, &req);
    requisition->width += req.width;

    gtk_widget_size_request(priv->plus, &req);
    requisition->width += req.width;

    requisition->width += SPACE_BORDER * 2;
    requisition->height = NUMBER_EDITOR_HEIGHT;
}

static void
set_widget_allocation (GtkWidget *widget, GtkAllocation *alloc,
                      GtkAllocation *allocation)
{
    GtkRequisition child_requisition;

    gtk_widget_get_child_requisition(widget, &child_requisition);

    if (allocation->width + allocation->x >
        alloc->x + child_requisition.width)
        alloc->width = child_requisition.width;
    else
      {
        alloc->width = allocation->width - (alloc->x - allocation->x);
        if (alloc->width < 0)
            alloc->width = 0;
      }

    gtk_widget_size_allocate(widget, alloc);
    alloc->x += alloc->width;
}

static void
hildon_number_editor_size_allocate (GtkWidget *widget,
                                    GtkAllocation *allocation)
{
  HildonNumberEditor *editor;
  HildonNumberEditorPrivate *priv;
  GtkAllocation alloc;

  editor = HILDON_NUMBER_EDITOR(widget);
  priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);

  widget->allocation = *allocation;

/*Init start values*/
  alloc.y = widget->allocation.y + widget->style->ythickness;

  if (widget->allocation.height > NUMBER_EDITOR_HEIGHT)
    {
      alloc.height = NUMBER_EDITOR_HEIGHT - widget->style->ythickness * 2;
      alloc.y += (widget->allocation.height - NUMBER_EDITOR_HEIGHT) / 2;
    }
  else
      alloc.height = widget->allocation.height - widget->style->ythickness * 2;


  
  if (alloc.height < 0)
    alloc.height = 0;

  alloc.x = allocation->x + widget->style->xthickness;

  set_widget_allocation(priv->minus, &alloc, &widget->allocation);
  alloc.x += SPACE_BORDER;

  set_widget_allocation(priv->num_entry, &alloc, &widget->allocation);
  alloc.x += SPACE_BORDER;

  set_widget_allocation(priv->plus, &alloc, &widget->allocation);
}

static gboolean
hildon_number_editor_entry_focusout (GtkWidget *widget, GdkEventFocus *event,
                                     gpointer data)
{
    HildonNumberEditor *editor;
    HildonNumberEditorPrivate *priv;
    gchar *str;
    gboolean r;

    editor = HILDON_NUMBER_EDITOR(data);
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
    
    /* empty entry, must infoprint error message */
    if (!strlen(GTK_ENTRY(priv->num_entry)->text))
      {
	g_signal_emit(editor, HildonNumberEditor_signal[RANGE_ERROR], 
		      0, ERRONEOUS_VALUE, &r);
        /* Changing to default value */
        str = integer_to_string(priv->default_val);
        gtk_entry_set_text(GTK_ENTRY(priv->num_entry), str);
        gtk_editable_select_region(GTK_EDITABLE(priv->num_entry), 0, -1);
        if (str)
            g_free(str);
      }
    return FALSE;
}

static gboolean
hildon_number_editor_entry_keypress (GtkWidget *widget, GdkEventKey *event,
                                     gpointer data)
{
    HildonNumberEditorPrivate *priv;
    gint tmp_pos;
    gchar *scnd_chr;
    gboolean ret_val = FALSE;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(data);
    tmp_pos = gtk_editable_get_position( GTK_EDITABLE(priv->num_entry) );
    
    if( (event->keyval == GDK_Left) || (event->keyval == GDK_Right) )
    {
      if( ((event->keyval == GDK_Left) && tmp_pos == 0 ) ||
           (event->keyval == GDK_Left && gtk_editable_get_selection_bounds 
                                                (GTK_EDITABLE (priv->num_entry),
                                                 0, NULL)) )
      {
        if (change_numbers(HILDON_NUMBER_EDITOR(data), 2, -1) == FALSE)
        { 
          gtk_editable_select_region(GTK_EDITABLE(priv->num_entry), 0, -1);
          return TRUE;
        }
      }
      else if ( (event->keyval == GDK_Right) )
      {
        gchar *str = gtk_editable_get_chars( GTK_EDITABLE(priv->num_entry),
                                             0, -1 );
        if( strlen(str) == tmp_pos )
        {
          if (change_numbers(HILDON_NUMBER_EDITOR(data), 1, tmp_pos) == FALSE)
          { 
            gtk_editable_select_region(GTK_EDITABLE(priv->num_entry), 0, -1);
            g_free (str);
            return TRUE;
          }
        }
       g_free (str);
      }
      return FALSE;
    }
   
    scnd_chr = gtk_editable_get_chars( GTK_EDITABLE(priv->num_entry),
                                              0, 1 );
    if (event->keyval == GDK_minus || event->keyval == GDK_KP_Subtract)
    {
      if (tmp_pos > 0)
           ret_val = TRUE;
      if (!strncmp (scnd_chr, "-", 1))
           ret_val = TRUE;
    }
    else if ((event->keyval == GDK_Up) || (event->keyval == GDK_Down)
        || (event->keyval == GDK_KP_Up) || (event->keyval == GDK_KP_Down))
        ret_val = FALSE;
    else if (((event->keyval >= GDK_0) && (event->keyval <= GDK_9)) ||
             ((event->keyval >= GDK_KP_0) && (event->keyval <= GDK_KP_9))
             || (event->keyval == GDK_BackSpace)
             || (event->keyval == GDK_Delete)
             || (event->keyval == GDK_Return)
             || (((event->keyval == GDK_minus)
             || (event->keyval == GDK_KP_Subtract))))
        ret_val = FALSE;
    else
      ret_val = TRUE;

   g_free (scnd_chr);
   return ret_val;
}

static gboolean
hildon_number_editor_error_handler(HildonNumberEditor *editor,
				   HildonNumberEditorErrorType type)
{

  gint min, max;
  gchar *err_msg = NULL;
  HildonNumberEditorPrivate *priv;

  priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);
  min = priv->start;
  max = priv->end;

  /* Construct different error message */
  switch (type)
    {
    case MAXIMUM_VALUE_EXCEED:
      err_msg = g_strdup_printf(_("Ckct_ib_maximum_value"), max, max);
      break;
    case MINIMUM_VALUE_EXCEED:
      err_msg = g_strdup_printf(_("Ckct_ib_minimum_value"), min, min);
      break;
    case ERRONEOUS_VALUE:
      err_msg =
	g_strdup_printf(_("Ckct_ib_set_a_value_within_range"), min, max);
      break;
    }
  
  if (err_msg)
    {
      gtk_infoprint(GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(editor),
						       GTK_TYPE_WINDOW)), err_msg);
      g_free(err_msg);
    }

  return TRUE;
}


/**
 * hildon_number_editor_new:
 * @min: Minimum accepted value
 * @max: Maximum accepted value
 * 
 * This function creates new number editor
 *
 * Return value: a new #HildonNumberEditor widget.
 */
GtkWidget *
hildon_number_editor_new (gint min, gint max)
{
    HildonNumberEditor *editor =
        g_object_new(HILDON_TYPE_NUMBER_EDITOR, NULL);

    /* Set user inputted range to editor */
    hildon_number_editor_set_range(editor, min, max);

    return GTK_WIDGET(editor);
}

/**
 * hildon_number_editor_set_range:
 * @editor: a #HildonNumberEditor widget
 * @min: Minimum accepted value
 * @max: Maximum accepted value
 *
 * This function set accepted number range for editor
 */
void
hildon_number_editor_set_range (HildonNumberEditor *editor, gint min, gint max)
{
    HildonNumberEditorPrivate *priv;
    gchar *str, *str2;
    gint a, b, entry_len;

    g_return_if_fail(HILDON_IS_NUMBER_EDITOR(editor));

    if (min > max)
      {
        gint temp = min;

        min = max;
        max = temp;
      }

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);

    /* we need to store user inputted values */
    priv->start = min;
    priv->end = max;
    priv->default_val = min;

    priv->negative = min < 0 ? TRUE : FALSE;

    str = integer_to_string(max);
    str2 = integer_to_string(min);
    a = strlen(str);
    b = strlen(str2);

    if (a >= b)
        entry_len = a;
    else
        entry_len = b;
    

    gtk_entry_set_width_chars(GTK_ENTRY(priv->num_entry), entry_len);

    gtk_entry_set_text(GTK_ENTRY(priv->num_entry), str);
    gtk_widget_queue_resize(GTK_WIDGET(editor));

    if (str)
        g_free(str);
    if (str2)
        g_free(str2);
}

/**
 * hildon_number_editor_get_value:
 * @editor: pointer to #HildonNumberEditor
 *
 * This function returns current value of number editor
 *
 * Return value: Current NumberEditor value
 */
gint
hildon_number_editor_get_value (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;
    gint value;

    g_return_val_if_fail(HILDON_IS_NUMBER_EDITOR(editor), 0);

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);

    value = atoi(GTK_ENTRY(priv->num_entry)->text);

    return value;
}

/**
 * hildon_number_editor_set_value:
 * @editor: pointer to #HildonNumberEditor
 * @value: numeric value for number editor
 *
 * This function sets numeric value to number editor
 */
void
hildon_number_editor_set_value (HildonNumberEditor *editor, gint value)
{
    HildonNumberEditorPrivate *priv;
    gchar *str;

    g_return_if_fail(HILDON_IS_NUMBER_EDITOR(editor));

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE(editor);

    g_return_if_fail(value <= priv->end);
    g_return_if_fail(value >= priv->start);

    priv->default_val = value;
        
    str = integer_to_string(value);
    gtk_entry_set_text(GTK_ENTRY(priv->num_entry), str);
    if (str)
        g_free(str);
}

/* When calling gtk_entry_set_text, the entry widget does things that can
 * cause the whole widget to redraw. This redrawing is delayed and if any
 * selections are made right after calling the gtk_entry_set_text the
 * setting of the selection might seem to have no effect.
 *
 * If the selection is delayed with a lower priority than the redrawing,
 * the selection should stick. Calling this function with g_idle_add should
 * do it.
 */
static gboolean
hildon_number_editor_select_all (HildonNumberEditorPrivate *priv)
{   
    gtk_editable_select_region(GTK_EDITABLE(priv->num_entry), 0, -1);
    return FALSE;
} 

