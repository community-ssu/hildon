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
 * @file hildon-range-editor.c
 * 
 * This file implements the HildonRangeEditor widget.
 *
 */

/* HILDON DOC
 * @desc: Range Editor is used to define the range some attribute. Accepted
 * number type is integer and '-' character is also acceptable. Range can
 * be used in application area and in dialog.  
 * 
 */

#include <pango/pango.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkselection.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkdnd.h>
#include <gtk/gtksignal.h>
#include <gtk/gtkentry.h>
#include <gdk/gdkkeysyms.h>
#include <stdio.h>
#include <glib/gprintf.h>
#include <string.h>
#include <stdlib.h>
#include <hildon-widgets/hildon-input-mode-hint.h>

#include "hildon-range-editor.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(string) dgettext(PACKAGE, string)

/* Alignment in entry box ( 0 = left, 1 = right ) */
#define DEFAULT_ALIGNMENT 1
/* Amount of padding to add to each side of the separator */
#define DEFAULT_PADDING 3

#define DEFAULT_START -999
#define DEFAULT_END 999
#define DEFAULT_LENGTH 4

#define HILDON_RANGE_EDITOR_GET_PRIVATE(obj) \
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
 HILDON_RANGE_EDITOR_TYPE, HildonRangeEditorPrivate));

typedef struct _HildonRangeEditorPrivate HildonRangeEditorPrivate;

enum
{
  PROP_LOWER = 1,
  PROP_HIGHER,
  PROP_MIN,
  PROP_MAX,
  PROP_SEPARATOR
};

/*our parent class*/
static GtkContainerClass *parent_class = NULL;

/*Init functions*/
static void
hildon_range_editor_class_init (HildonRangeEditorClass *editor_class);
static void
hildon_range_editor_init (HildonRangeEditor *editor);
static void
hildon_range_editor_forall (GtkContainer *container,
                            gboolean include_internals, GtkCallback callback,
                            gpointer callback_data);
static void
hildon_range_editor_destroy (GtkObject *self);

/*size and font functions */
static void
hildon_range_editor_size_request (GtkWidget *widget,
                                  GtkRequisition *requisition);
static void
hildon_range_editor_size_allocate (GtkWidget *widget,
                                  GtkAllocation *allocation);
static gboolean
hildon_range_editor_entry_focus_in (GtkEditable *editable,
                                    GdkEventFocus *event,
                                    HildonRangeEditor *editor);
static gboolean
hildon_range_editor_entry_focus_out (GtkEditable *editable,
                                    GdkEventFocus *event,
                                    HildonRangeEditor *editor);
static gboolean
hildon_range_editor_entry_keypress (GtkWidget *widget, GdkEventKey *event,
                                    HildonRangeEditor *editor);
static gboolean
hildon_range_editor_released (GtkEditable *editable, GdkEventButton *event,
                              HildonRangeEditor *editor);
static gboolean
hildon_range_editor_press (GtkEditable *editable, GdkEventButton *event,
                           HildonRangeEditor *editor);
static gboolean
hildon_range_editor_mnemonic_activate (GtkWidget *widget,
                                       gboolean group_cycling);

static void hildon_range_editor_set_property( GObject *object, guint param_id,
                                       const GValue *value, GParamSpec *pspec );
static void hildon_range_editor_get_property( GObject *object, guint param_id,
                                         GValue *value, GParamSpec *pspec );

/* Private struct */
struct _HildonRangeEditorPrivate
{
    GtkWidget *start_entry;
    GtkWidget *end_entry;
    GtkWidget *label;
    gint range_limits_start;
    gint range_limits_end;
    gboolean bp;
};

/* Private functions */
static void
hildon_range_editor_class_init  (HildonRangeEditorClass *editor_class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(editor_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(editor_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(editor_class);

    /* set the global parent_class */
    parent_class = g_type_class_peek_parent(editor_class);

    /* now the object stuff */
    g_type_class_add_private(editor_class,
                             sizeof(HildonRangeEditorPrivate));

    gobject_class->set_property = hildon_range_editor_set_property;
    gobject_class->get_property = hildon_range_editor_get_property;
    widget_class->size_request = hildon_range_editor_size_request;
    widget_class->size_allocate = hildon_range_editor_size_allocate;

    widget_class->mnemonic_activate = hildon_range_editor_mnemonic_activate;

    container_class->forall = hildon_range_editor_forall;
    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_range_editor_destroy;

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_int("hildon_range_editor_entry_alignment",
                         "Hildon RangeEditor entry alignment",
                         "Hildon RangeEditor entry alignment", 0, 1,
                         DEFAULT_ALIGNMENT,
                         G_PARAM_READABLE));

    gtk_widget_class_install_style_property(widget_class,
        g_param_spec_int("hildon_range_editor_separator_padding",
                         "Hildon RangeEditor separator padding",
                         "Hildon RangeEditor separaror padding",
                         G_MININT, G_MAXINT,
                         DEFAULT_PADDING,
                         G_PARAM_READABLE));

  /**
   * HildonRangeEditor:min:
   *
   * Minimum value in a range.
   * Default: -999
   */
  g_object_class_install_property( gobject_class, PROP_MIN,
                                   g_param_spec_int("min",
                                   "Minimum value",
                                   "Minimum value in a range",
                                   G_MININT, G_MAXINT,
                                   DEFAULT_START, G_PARAM_CONSTRUCT | 
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonRangeEditor:max:
   *
   * Maximum value in a range.
   * Default: 999
   */
  g_object_class_install_property( gobject_class, PROP_MAX,
                                   g_param_spec_int("max",
                                   "Maximum value",
                                    "Maximum value in a range",
                                   G_MININT, G_MAXINT,
                                   DEFAULT_END, G_PARAM_CONSTRUCT | 
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonRangeEditor:lower:
   *
   * Current value in the entry presenting lower end of selected range.
   * Default: -999
   */
  g_object_class_install_property( gobject_class, PROP_LOWER,
                                   g_param_spec_int("lower",
                                   "Current lower value",
           "Current value in the entry presenting lower end of selected range",
                                   G_MININT, G_MAXINT,
                                   DEFAULT_START, G_PARAM_CONSTRUCT | 
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonRangeEditor:higher:
   *
   * Current value in the entry presenting higher end of selected range.
   * Default: 999
   */
  g_object_class_install_property( gobject_class, PROP_HIGHER,
                                   g_param_spec_int("higher",
                                   "Current higher value",
           "Current value in the entry presenting higher end of selected range",
                                   G_MININT, G_MAXINT,
                                   DEFAULT_END, G_PARAM_CONSTRUCT | 
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );

  /**
   * HildonRangeEditor:separator:
   *
   * Separator string to separate range editor entries.
   * Default: "-"
   */
  g_object_class_install_property( gobject_class, PROP_SEPARATOR,
                                   g_param_spec_string("separator",
                                   "Separator",
                                    "Separator string to separate entries",
                                   _("Ckct_wi_range_separator"),
                                   G_PARAM_CONSTRUCT | 
                                   G_PARAM_READABLE | G_PARAM_WRITABLE) );
}

static void
hildon_range_editor_init (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;

    gint range_editor_entry_alignment;
    gint range_editor_separator_padding;

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);

    GTK_WIDGET_SET_FLAGS(editor, GTK_NO_WINDOW);
    
    gtk_widget_push_composite_child();
    
    priv->start_entry = GTK_WIDGET(gtk_entry_new());
    priv->end_entry = GTK_WIDGET(gtk_entry_new());
    priv->label = GTK_WIDGET(gtk_label_new(_("Ckct_wi_range_separator")));
    priv->bp = FALSE;

    /* Get values from gtkrc (or use defaults) */
    gtk_widget_style_get(GTK_WIDGET(editor),
                         "hildon_range_editor_entry_alignment",
                         &range_editor_entry_alignment,
                         "hildon_range_editor_separator_padding",
                         &range_editor_separator_padding, NULL);

    /* Add padding to separator */
    gtk_misc_set_padding (GTK_MISC(priv->label),
                          range_editor_separator_padding, 0);

    /* Align the text to right in entry box */
    gtk_entry_set_alignment(GTK_ENTRY(priv->start_entry),
                            range_editor_entry_alignment);
    gtk_entry_set_alignment(GTK_ENTRY(priv->end_entry),
                            range_editor_entry_alignment);

    gtk_widget_set_parent(priv->start_entry, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->end_entry, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->label, GTK_WIDGET(editor));
    
    g_signal_connect(G_OBJECT(priv->start_entry), "button-release-event",
                     G_CALLBACK(hildon_range_editor_released), editor);
    g_signal_connect(G_OBJECT(priv->end_entry), "button-release-event",
                     G_CALLBACK(hildon_range_editor_released), editor);
                     
    g_signal_connect(G_OBJECT(priv->start_entry), "button-press-event",
                     G_CALLBACK(hildon_range_editor_press), editor);
    g_signal_connect(G_OBJECT(priv->end_entry), "button-press-event",
                     G_CALLBACK(hildon_range_editor_press), editor);
                     
    g_signal_connect(G_OBJECT(priv->start_entry), "key-press-event",
                       G_CALLBACK(hildon_range_editor_entry_keypress), editor);
    g_signal_connect(G_OBJECT(priv->end_entry), "key-press-event",
                       G_CALLBACK(hildon_range_editor_entry_keypress), editor);

    g_signal_connect(G_OBJECT(priv->start_entry), "focus-in-event",
                     G_CALLBACK(hildon_range_editor_entry_focus_in), editor);
    g_signal_connect(G_OBJECT(priv->end_entry), "focus-in-event",
                     G_CALLBACK(hildon_range_editor_entry_focus_in), editor);

    g_signal_connect(G_OBJECT(priv->start_entry), "focus-out-event",
                     G_CALLBACK(hildon_range_editor_entry_focus_out), editor);
    g_signal_connect(G_OBJECT(priv->end_entry), "focus-out-event",
                     G_CALLBACK(hildon_range_editor_entry_focus_out), editor);
                       
    g_object_set( G_OBJECT(priv->start_entry),
                    "input-mode", HILDON_INPUT_MODE_HINT_NUMERIC, NULL );
    g_object_set( G_OBJECT(priv->end_entry),
                    "input-mode", HILDON_INPUT_MODE_HINT_NUMERIC, NULL );
    
    gtk_widget_show(priv->start_entry);
    gtk_widget_show(priv->end_entry);
    gtk_widget_show(priv->label);

    gtk_widget_pop_composite_child();
}

static void hildon_range_editor_set_property (GObject *object, guint param_id,
                                       const GValue *value, GParamSpec *pspec)
{
  HildonRangeEditor *editor = HILDON_RANGE_EDITOR(object);
  switch (param_id)
  {
    case PROP_LOWER:
      hildon_range_editor_set_lower (editor, g_value_get_int (value));
      break;

    case PROP_HIGHER:
      hildon_range_editor_set_higher (editor, g_value_get_int (value));
      break;

    case PROP_MIN:
      hildon_range_editor_set_min (editor, g_value_get_int (value));
      break;

    case PROP_MAX:
      hildon_range_editor_set_max (editor, g_value_get_int (value));
      break;

    case PROP_SEPARATOR:
      hildon_range_editor_set_separator (editor,
                                         g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static void hildon_range_editor_get_property( GObject *object, guint param_id,
                                         GValue *value, GParamSpec *pspec )
{
  HildonRangeEditor *editor = HILDON_RANGE_EDITOR(object);
  switch (param_id)
  {
    case PROP_LOWER:
      g_value_set_int (value, hildon_range_editor_get_lower (editor));
      break;

    case PROP_HIGHER:
      g_value_set_int (value, hildon_range_editor_get_higher (editor));
      break;

    case PROP_MIN:
      g_value_set_int (value, hildon_range_editor_get_min (editor));
      break;

    case PROP_MAX:
      g_value_set_int (value, hildon_range_editor_get_max (editor));
      break;

    case PROP_SEPARATOR:
      g_value_set_string (value, hildon_range_editor_get_separator (editor));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static gboolean
hildon_range_editor_entry_focus_in (GtkEditable *editable,
                                    GdkEventFocus *event,
                                    HildonRangeEditor *editor)
{
  HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
  if(priv->bp)
  {
    priv->bp = FALSE;
    return FALSE;
  }
  if (GTK_WIDGET(editable) == priv->start_entry)
    gtk_editable_select_region(editable, -1, 0);
  else
    gtk_editable_select_region(editable, 0, -1);
  return FALSE;
}

static gboolean
hildon_range_editor_entry_focus_out (GtkEditable *editable,
                                    GdkEventFocus *event,
                                    HildonRangeEditor *editor)
{
  HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
  hildon_range_editor_set_range( editor,
                            g_strtod(GTK_ENTRY(priv->start_entry)->text, NULL),
                            g_strtod(GTK_ENTRY(priv->end_entry)->text, NULL));
  return FALSE;
}

static gboolean
hildon_range_editor_press (GtkEditable *editable, GdkEventButton *event,
                           HildonRangeEditor *editor)
{
  HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
  priv->bp = TRUE;
  return FALSE;
}

static gboolean
hildon_range_editor_mnemonic_activate (GtkWidget *widget,
                                       gboolean group_cycling)
{
  HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(widget);
  gtk_widget_grab_focus( priv->start_entry );
  return TRUE;
}

static void
hildon_range_editor_forall (GtkContainer *container,
                            gboolean include_internals,
                            GtkCallback callback, gpointer callback_data)
{
    HildonRangeEditorPrivate *priv;

    g_return_if_fail(container);
    g_return_if_fail(callback);

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE(container);

    if (!include_internals)
      return;
    
    (*callback) (priv->start_entry, callback_data);
    (*callback) (priv->end_entry, callback_data);
    (*callback) (priv->label, callback_data);
}

static void
hildon_range_editor_destroy(GtkObject *self)
{
    HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(self);

    if (priv->start_entry)
    {
      gtk_widget_unparent(priv->start_entry);
      priv->start_entry = NULL;
    }
    if (priv->end_entry)
    {
      gtk_widget_unparent(priv->end_entry);
      priv->end_entry = NULL;
    }
    if (priv->label)
    {
      gtk_widget_unparent(priv->label);
      priv->label = NULL;
    }

    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        GTK_OBJECT_CLASS(parent_class)->destroy(self);
}


static void
hildon_range_editor_size_request(GtkWidget *widget,
                                 GtkRequisition *requisition)
{
    HildonRangeEditorPrivate *priv = NULL;
    GtkRequisition lab_req, mreq;
    
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE(widget);

    gtk_entry_get_width_chars(GTK_ENTRY(priv->end_entry));
    
    gtk_widget_size_request(priv->start_entry, &mreq);
    gtk_widget_size_request(priv->end_entry, &mreq);
    gtk_widget_size_request(priv->label, &lab_req);

    requisition->width = mreq.width * 2 + lab_req.width +
                         widget->style->xthickness * 2;
    requisition->height = mreq.height + widget->style->ythickness * 2;
    requisition->height = MAX (requisition->height, lab_req.height);
}

static void
hildon_range_editor_size_allocate(GtkWidget *widget,
                                  GtkAllocation *allocation)
{
    HildonRangeEditorPrivate *priv;
    GtkAllocation child1_allocation, child2_allocation, child3_allocation;

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE(widget);

    widget->allocation = *allocation;

    if (priv->start_entry && GTK_WIDGET_VISIBLE(priv->start_entry))
      {
        GtkRequisition child_requisition;

        gtk_widget_get_child_requisition(priv->start_entry,
                                         &child_requisition);

        child1_allocation.x = allocation->x;
        child1_allocation.y = allocation->y;

        child1_allocation.width = child_requisition.width;
        child1_allocation.height = allocation->height;

        gtk_widget_size_allocate(priv->start_entry, &child1_allocation);
      }

    if (priv->label && GTK_WIDGET_VISIBLE(priv->label))
      {
        GtkRequisition child_requisition;

        gtk_widget_get_child_requisition(priv->label, &child_requisition);

        child2_allocation.x = child1_allocation.x + child1_allocation.width;
        child2_allocation.y = allocation->y;
        child2_allocation.width = child_requisition.width + 4;
        child2_allocation.height = allocation->height;

        gtk_widget_size_allocate (priv->label, &child2_allocation);
      }

    if (priv->end_entry && GTK_WIDGET_VISIBLE(priv->end_entry))
      {
        GtkRequisition child_requisition;

        gtk_widget_get_child_requisition (priv->end_entry, &child_requisition);

        child3_allocation.x = child2_allocation.x + child2_allocation.width;
        child3_allocation.y = allocation->y;

        child3_allocation.width = child_requisition.width;
        child3_allocation.height = allocation->height;

        gtk_widget_size_allocate(priv->end_entry, &child3_allocation);
      }
}

static gboolean
hildon_range_editor_released(GtkEditable *editable, GdkEventButton *event,
                             HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
    if (GTK_WIDGET(editable) == priv->start_entry)
      gtk_editable_select_region(editable, -1, 0);
    else
      gtk_editable_select_region(editable, 0, -1); 
    return FALSE;
}

static gboolean
hildon_range_editor_entry_keypress(GtkWidget *widget, GdkEventKey *event,
                                   HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    GtkWidget *wdgt;
    gint pos;
    gchar *str;

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);

    wdgt = widget == priv->start_entry ? priv->end_entry : priv->start_entry;
    str = GTK_ENTRY(widget)->text;
    pos = gtk_editable_get_position(GTK_EDITABLE(widget));
 
    if (pos > 0 && (event->keyval == GDK_minus ||
                    event->keyval == GDK_KP_Subtract))
       return TRUE;

    if (strlen(str) == pos)
      pos = -1;

    if ((widget == priv->start_entry &&
        (event->keyval == GDK_Tab || (pos == -1 &&
        (event->keyval == GDK_Right || event->keyval == GDK_KP_Right)))) ||
        (widget == priv->end_entry &&
        (event->keyval == GDK_ISO_Left_Tab|| (pos == 0 &&
        (event->keyval == GDK_Left || event->keyval == GDK_KP_Left)))))
    {
      gtk_widget_grab_focus(wdgt);
      if (widget == priv->start_entry)
      {
        gtk_editable_set_position(GTK_EDITABLE(wdgt), -1);
        gtk_editable_select_region(GTK_EDITABLE(wdgt), 0, -1);
      }
      else
      {
        gtk_editable_set_position(GTK_EDITABLE(wdgt), 0);
        gtk_editable_select_region(GTK_EDITABLE(wdgt), -1, 0);
      }
    }
    else if ((event->keyval >= GDK_0 && event->keyval <= GDK_9) ||
             (event->keyval >= GDK_KP_0 && event->keyval <= GDK_KP_9) ||
             (event->keyval == GDK_minus || event->keyval == GDK_KP_Subtract) ||
             event->keyval == GDK_Up || event->keyval == GDK_Down ||
             event->keyval == GDK_Right || event->keyval == GDK_KP_Right ||
             event->keyval == GDK_Left || event->keyval == GDK_KP_Left ||
             event->keyval == GDK_BackSpace || event->keyval == GDK_Delete)
    {
      return FALSE;
    }
    return TRUE;
}

/* Public functions */

/**
 * hildon_range_editor_get_type:
 * @Returns : GType of #HildonRangeEditor.
 *
 * Initialises, and returns the type of a hildon range editor.
 */
GType
hildon_range_editor_get_type (void)
{
    static GType editor_type = 0;

    if (!editor_type)
      {
        static const GTypeInfo editor_info =
          {
            sizeof(HildonRangeEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_range_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonRangeEditor),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_range_editor_init,
          };
        editor_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                             "HildonRangeEditor",
                                             &editor_info, 0);
      }
    return editor_type;
}

/**
 * hildon_range_editor_new:
 *
 * HildonRangeEditor contains two GtkEntrys that accept numbers and minus. 
 *
 * Return value: pointer to a new @HildonRangeEditor widget.
 **/
GtkWidget *
hildon_range_editor_new (void)
{
    return GTK_WIDGET(g_object_new(HILDON_RANGE_EDITOR_TYPE, NULL));
}


/**
 * hildon_range_editor_new_with_separator:
 * @separator: A string that is shown between the numbers.
 *
 * HildonRangeEditor contains two @GtkEntrys that accept numbers. 
 * @separator is displayed between two entrys.
 * CHECKME: Use '-' as a separator in the case of null separator?
 *
 * Return value: pointer to a new @HildonRangeEditor widget.
 **/
GtkWidget *
hildon_range_editor_new_with_separator (gchar *separator)
{
    return GTK_WIDGET (g_object_new (HILDON_RANGE_EDITOR_TYPE,
                                     "separator", separator, NULL));
}


/**
 * hildon_range_editor_set_range:
 * @editor: the #HildonRangeEditor widget.
 * @start: range's start value. 
 * @end: range's end value.
 *
 * Sets a range to the editor. (The current value)
 *
 * Sets the range of the @HildonRangeEditor widget.
 **/
void
hildon_range_editor_set_range (HildonRangeEditor *editor, gint start, gint end)
{
    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));

    hildon_range_editor_set_lower (editor, start);
    hildon_range_editor_set_higher (editor, end);
}


/**
 * hildon_range_editor_get_range:
 * @editor: the #HildonRangeEditor widget.
 * @start: ranges start value.
 * @end: ranges end value.
 *
 * Gets the range of the @HildonRangeEditor widget.
 **/
void
hildon_range_editor_get_range (HildonRangeEditor *editor, gint *start,
                               gint *end)
{
    HildonRangeEditorPrivate *priv;
    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor) || start || end);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    *start = hildon_range_editor_get_lower (editor);
    *end = hildon_range_editor_get_higher (editor);
}


/**
 * hildon_range_editor_set_limits:
 * @editor: the #HildonRangeEditor widget.
 * @start: minimum acceptable value (default: no limit).
 * @end: maximum accecptable value (default: no limit).
 *
 * if start's or end's value is larger than maximum acceptable integer
 * '999999999', function sets the maximum value for integer
 *
 * if start's or end's value is smaller than minimum acceptable integer
 * '-99999999', function sets the minimum value for integer
 * 
 * Sets the range of the @HildonRangeEditor widget.
 **/
void
hildon_range_editor_set_limits (HildonRangeEditor *editor, gint start,
                                gint end)
{
    hildon_range_editor_set_min (editor, start);
    hildon_range_editor_set_max (editor, end);
}

void
hildon_range_editor_set_lower (HildonRangeEditor *editor, gint value)
{
    HildonRangeEditorPrivate *priv;
    gchar range[12];
    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    if (priv->range_limits_start <= value && priv->range_limits_end >= value)
      g_sprintf (range, "%d", value);
    else
      g_sprintf (range, "%d", priv->range_limits_start);

    gtk_entry_set_text (GTK_ENTRY (priv->start_entry), range);
    g_object_notify (G_OBJECT (editor), "lower");
}

void
hildon_range_editor_set_higher (HildonRangeEditor *editor, gint value)
{
    HildonRangeEditorPrivate *priv;
    gchar range[12];
    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    if (priv->range_limits_start <= value && priv->range_limits_end >= value)
      g_sprintf (range, "%d", value);
    else
      g_sprintf (range, "%d", priv->range_limits_end);

    gtk_entry_set_text (GTK_ENTRY (priv->end_entry), range);
    g_object_notify (G_OBJECT (editor), "higher");
}

gint
hildon_range_editor_get_lower (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (editor, 0);
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return (gint)g_strtod (GTK_ENTRY (priv->start_entry)->text, NULL);
}

gint
hildon_range_editor_get_higher (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (editor, 0);
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return (gint)g_strtod (GTK_ENTRY (priv->end_entry)->text, NULL);
}

void
hildon_range_editor_set_min (HildonRangeEditor *editor, gint value)
{
    HildonRangeEditorPrivate *priv;
    gchar end_range[12], start_range[12];
    GtkEntry *start_entry, *end_entry;
    gint length = 0;

    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    g_sprintf (start_range, "%d", value);
    start_entry = GTK_ENTRY (priv->start_entry);

    if (priv->range_limits_end < value)
      hildon_range_editor_set_max (editor, value);
    else
    {
      g_sprintf (end_range, "%d", priv->range_limits_end);
      end_entry = GTK_ENTRY (priv->end_entry);
      length = MAX (strlen (start_range), strlen (end_range));

      gtk_entry_set_width_chars (start_entry, length);
      gtk_entry_set_max_length (start_entry, length);
      gtk_entry_set_width_chars (end_entry, length);
      gtk_entry_set_max_length (end_entry, length);
    }

    if (hildon_range_editor_get_lower (editor) < value)
      gtk_entry_set_text (start_entry, start_range);

    priv->range_limits_start = value;
    g_object_notify (G_OBJECT (editor), "min");
}

void
hildon_range_editor_set_max (HildonRangeEditor *editor, gint value)
{
    HildonRangeEditorPrivate *priv;
    gchar start_range[12], end_range[12];
    GtkEntry *start_entry, *end_entry;
    gint length = 0;

    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    g_sprintf (end_range, "%d", value);
    end_entry = GTK_ENTRY (priv->end_entry);

    if (priv->range_limits_start > value)
      hildon_range_editor_set_min (editor, value);
    else
    {
      g_sprintf (start_range, "%d", priv->range_limits_start);
      start_entry = GTK_ENTRY (priv->start_entry);
      length = MAX (strlen (end_range), strlen (start_range));

      gtk_entry_set_width_chars (start_entry, length);
      gtk_entry_set_max_length (start_entry, length);
      gtk_entry_set_width_chars (end_entry, length);
      gtk_entry_set_max_length (end_entry, length);
    }

    if (hildon_range_editor_get_higher (editor) > value)
      gtk_entry_set_text (end_entry, end_range);

    priv->range_limits_end = value;
    g_object_notify (G_OBJECT (editor), "max");
}

gint
hildon_range_editor_get_min (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (editor, 0);
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return priv->range_limits_start;
}

gint
hildon_range_editor_get_max (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (editor, 0);
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return priv->range_limits_end;
}

void
hildon_range_editor_set_separator (HildonRangeEditor *editor,
                                   const gchar *separator)
{
    HildonRangeEditorPrivate *priv;
    g_return_if_fail (editor);
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    gtk_label_set_text (GTK_LABEL (priv->label), separator);
    g_object_notify (G_OBJECT(editor), "separator");
}

const gchar *
hildon_range_editor_get_separator (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (editor, 0);
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return gtk_label_get_text (GTK_LABEL (priv->label));
}
