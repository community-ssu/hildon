/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

/**
 * SECTION:hildon-number-editor
 * @short_description: A widget used to enter a number within a pre-defined range.
 *
 * HildonNumberEditor is used to enter a number from a specific range. 
 * There are two buttons to scroll the value in number field. 
 * Manual input is also possible.
 *
 * <example>
 * <title>HildonNumberEditor example</title>
 * <programlisting>
 * number_editor = hildon_number_editor_new (-250, 500);
 * hildon_number_editor_set_range (number_editor, 0, 100);
 * </programlisting>
 * </example>
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-number-editor.h"
#include                                        <gdk/gdkkeysyms.h>
#include                                        <gtk/gtk.h>
#include                                        <string.h>
#include                                        <stdio.h>
#include                                        <stdlib.h>
#include                                        "hildon-marshalers.h"
#include                                        "hildon-private.h"
#include                                        "hildon-defines.h"
#include                                        "hildon-enum-types.h"
#include                                        "hildon-banner.h"
#include                                        <libintl.h>
#include                                        "hildon-number-editor-private.h"

#define                                         _(String) dgettext("hildon-libs", String)

/*Pixel spec defines*/
#define                                         NUMBER_EDITOR_HEIGHT 30

/* Size of plus and minus buttons */
#define                                         BUTTON_HEIGHT 30

#define                                         BUTTON_WIDTH 30

static void
hildon_number_editor_class_init                 (HildonNumberEditorClass *editor_class);

static void
hildon_number_editor_init                       (HildonNumberEditor *editor);

static gboolean
hildon_number_editor_entry_focusout             (GtkWidget *widget, 
                                                 GdkEventFocus *event,
                                                 gpointer data);

static void
hildon_number_editor_entry_changed              (GtkWidget *widget, 
                                                 gpointer data);

static void
hildon_number_editor_size_request               (GtkWidget *widget,
                                                 GtkRequisition *requisition);

static void
set_widget_allocation                           (GtkWidget *widget, 
                                                 GtkAllocation *alloc,
                                                 const GtkAllocation *allocation);

static void
hildon_number_editor_size_allocate              (GtkWidget *widget,
                                                 GtkAllocation *allocation);

static gboolean
hildon_number_editor_entry_keypress             (GtkWidget *widget, 
                                                 GdkEventKey *event,
                                                 gpointer data);

static gboolean
hildon_number_editor_button_pressed             (GtkWidget *widget, 
                                                 GdkEventButton *event,
                                                 gpointer data);

static gboolean
hildon_number_editor_entry_button_released      (GtkWidget *widget,
                                                 GdkEventButton *event,
                                                 gpointer data);
static gboolean
hildon_number_editor_button_released            (GtkWidget *widget,
                                                 GdkEvent *event,
                                                 HildonNumberEditor *editor);
static gboolean
do_mouse_timeout                                (HildonNumberEditor *editor);

static void
change_numbers                                  (HildonNumberEditor *editor, 
                                                 gint update);

static void
hildon_number_editor_forall                     (GtkContainer *container, 
                                                 gboolean include_internals,
                                                 GtkCallback callback, 
                                                 gpointer callback_data);

static void
hildon_number_editor_destroy                    (GtkObject *self);

static gboolean
hildon_number_editor_start_timer                (HildonNumberEditor *editor);

static void
hildon_number_editor_finalize                   (GObject *self);

static gboolean
hildon_number_editor_range_error                (HildonNumberEditor *editor,
                                                 HildonNumberEditorErrorType type);

static gboolean
hildon_number_editor_select_all                 (HildonNumberEditorPrivate *priv);

static void
hildon_number_editor_validate_value             (HildonNumberEditor *editor, 
                                                 gboolean allow_intermediate);
    
static void 
hildon_number_editor_set_property               (GObject * object,
                                                 guint prop_id,
                                                 const GValue * value,
                                                 GParamSpec * pspec);

static void
hildon_number_editor_get_property               (GObject *object,
                                                 guint prop_id,
                                                 GValue *value, 
                                                 GParamSpec * pspec);

enum
{
    RANGE_ERROR,
    LAST_SIGNAL
};

enum {
    PROP_0,
    PROP_VALUE
};

static GtkContainerClass*                       parent_class;

static guint                                    HildonNumberEditor_signal[LAST_SIGNAL] = {0};

/**
 * hildon_number_editor_get_type:
 *
 * Returns GType for HildonNumberEditor.
 *
 * Returns: HildonNumberEditor type
 */
GType G_GNUC_CONST
hildon_number_editor_get_type                   (void)
{
    static GType editor_type = 0;

    if (!editor_type)
    {
        static const GTypeInfo editor_info =
        {
            sizeof (HildonNumberEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_number_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonNumberEditor),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_number_editor_init,
        };
        editor_type = g_type_register_static (GTK_TYPE_CONTAINER,
                "HildonNumberEditor",
                &editor_info, 0);
    }
    return editor_type;
}

static void
hildon_number_editor_class_init                 (HildonNumberEditorClass *editor_class)
{
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS (editor_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (editor_class);
    GObjectClass *gobject_class = G_OBJECT_CLASS (editor_class);

    g_type_class_add_private (editor_class,
            sizeof (HildonNumberEditorPrivate));

    parent_class = g_type_class_peek_parent (editor_class);

    widget_class->size_request              = hildon_number_editor_size_request;
    widget_class->size_allocate             = hildon_number_editor_size_allocate;
    widget_class->focus                     = hildon_private_composite_focus;

    editor_class->range_error = hildon_number_editor_range_error;

    /* Because we derived our widget from GtkContainer, we should override 
       forall method */
    container_class->forall                 = hildon_number_editor_forall;
    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_number_editor_destroy;
    gobject_class->finalize                 = hildon_number_editor_finalize;
    gobject_class->set_property             = hildon_number_editor_set_property;
    gobject_class->get_property             = hildon_number_editor_get_property;

    /**
     * HildonNumberEditor:value:
     *
     * The current value of the number editor.
     */
    g_object_class_install_property (gobject_class, PROP_VALUE,
            g_param_spec_int ("value",
                "Value",
                "The current value of number editor",
                G_MININT,
                G_MAXINT,
                0, G_PARAM_READWRITE));

    HildonNumberEditor_signal[RANGE_ERROR] =
        g_signal_new ("range_error", HILDON_TYPE_NUMBER_EDITOR,
                G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET
                (HildonNumberEditorClass, range_error),
                g_signal_accumulator_true_handled, NULL,
                _hildon_marshal_BOOLEAN__ENUM,
                G_TYPE_BOOLEAN, 1, HILDON_TYPE_NUMBER_EDITOR_ERROR_TYPE);
}

static void
hildon_number_editor_forall                     (GtkContainer *container, 
                                                 gboolean include_internals,
                                                 GtkCallback callback, 
                                                 gpointer callback_data)
{
    HildonNumberEditorPrivate *priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (container);

    g_assert (callback != NULL);
    g_assert (priv);

    if (! include_internals)
        return;

    /* Enumerate child widgets */
    (*callback) (priv->minus, callback_data);
    (*callback) (priv->num_entry, callback_data);
    (*callback) (priv->plus, callback_data);
}

static void
hildon_number_editor_destroy                    (GtkObject *self)
{
    HildonNumberEditorPrivate *priv;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (self);
    g_assert (priv);

    /* Free child widgets */
    if (priv->minus)
    {
        gtk_widget_unparent (priv->minus);
        priv->minus = NULL;
    }
    if (priv->num_entry)
    {
        gtk_widget_unparent (priv->num_entry);
        priv->num_entry = NULL;
    }
    if (priv->plus)
    {
        gtk_widget_unparent (priv->plus);
        priv->plus = NULL;
    }

    if (GTK_OBJECT_CLASS (parent_class)->destroy)
        GTK_OBJECT_CLASS (parent_class)->destroy(self);
}

static void
hildon_number_editor_stop_repeat_timer          (HildonNumberEditorPrivate *priv)
{
    g_assert (priv != NULL);

    if (priv->button_event_id)
    {
        g_source_remove (priv->button_event_id);
        priv->button_event_id = 0;
    }
}

static void
hildon_number_editor_finalize                   (GObject *self)
{
    HildonNumberEditorPrivate *priv;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (self);
    g_assert (priv);

    /* Free timers */
    hildon_number_editor_stop_repeat_timer (priv);

    if (priv->select_all_idle_id)
        g_source_remove (priv->select_all_idle_id);

    /* Call parent class finalize, if have one */
    if (G_OBJECT_CLASS (parent_class)->finalize)
        G_OBJECT_CLASS (parent_class)->finalize(self);
}

static void
hildon_number_editor_init                       (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    GTK_WIDGET_SET_FLAGS (GTK_WIDGET (editor), GTK_NO_WINDOW);

    /* Create child widgets */
    priv->num_entry = gtk_entry_new ();
    priv->minus = gtk_button_new ();
    priv->plus = gtk_button_new ();

    gtk_widget_set_name (priv->minus, "ne-minus-button");
    gtk_widget_set_name (priv->plus, "ne-plus-button" );
    gtk_widget_set_size_request (priv->minus, BUTTON_WIDTH, BUTTON_HEIGHT);
    gtk_widget_set_size_request (priv->plus, BUTTON_WIDTH, BUTTON_HEIGHT);
    gtk_entry_set_alignment (GTK_ENTRY(priv->num_entry), 1);

    GTK_WIDGET_UNSET_FLAGS (priv->minus, GTK_CAN_FOCUS);
    GTK_WIDGET_UNSET_FLAGS (priv->plus, GTK_CAN_FOCUS);

    priv->button_event_id = 0;
    priv->select_all_idle_id = 0;

    gtk_widget_set_parent (priv->minus, GTK_WIDGET (editor));
    gtk_widget_set_parent (priv->num_entry, GTK_WIDGET (editor));
    gtk_widget_set_parent (priv->plus, GTK_WIDGET (editor));

    /* Connect child widget signals */
    g_signal_connect (GTK_OBJECT (priv->num_entry), "changed",
            G_CALLBACK (hildon_number_editor_entry_changed),
            editor);

    g_signal_connect (GTK_OBJECT (priv->num_entry), "focus-out-event",
            G_CALLBACK (hildon_number_editor_entry_focusout),
            editor);

    g_signal_connect (GTK_OBJECT (priv->num_entry), "key-press-event",
            G_CALLBACK (hildon_number_editor_entry_keypress),
            editor);

    g_signal_connect (GTK_OBJECT (priv->num_entry), "button-release-event",
            G_CALLBACK (hildon_number_editor_entry_button_released),
            NULL);

    g_signal_connect (GTK_OBJECT (priv->minus), "button-press-event",
            G_CALLBACK (hildon_number_editor_button_pressed),
            editor);

    g_signal_connect (GTK_OBJECT (priv->plus), "button-press-event",
            G_CALLBACK (hildon_number_editor_button_pressed),
            editor);

    g_signal_connect (GTK_OBJECT (priv->minus), "button-release-event",
            G_CALLBACK (hildon_number_editor_button_released),
            editor);

    g_signal_connect (GTK_OBJECT (priv->plus), "button-release-event",
            G_CALLBACK (hildon_number_editor_button_released),
            editor);

    g_signal_connect (GTK_OBJECT (priv->minus), "leave-notify-event",
            G_CALLBACK(hildon_number_editor_button_released),
            editor);

    g_signal_connect (GTK_OBJECT (priv->plus), "leave-notify-event",
            G_CALLBACK (hildon_number_editor_button_released),
            editor);

    g_object_set (G_OBJECT (priv->num_entry),
            "hildon-input-mode", HILDON_GTK_INPUT_MODE_NUMERIC, NULL);

    gtk_widget_show (priv->num_entry);
    gtk_widget_show (priv->minus);
    gtk_widget_show (priv->plus);

    hildon_number_editor_set_range (editor, G_MININT, G_MAXINT);
}

static gboolean
hildon_number_editor_entry_button_released      (GtkWidget *widget,
                                                 GdkEventButton *event,
                                                 gpointer data)
{
    gtk_editable_select_region (GTK_EDITABLE (widget), 0, -1);
    return FALSE;
}

static gboolean
hildon_number_editor_button_released            (GtkWidget *widget, 
                                                 GdkEvent *event,
                                                 HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);  
    g_assert (priv);

    hildon_number_editor_stop_repeat_timer (priv);
    return FALSE;
}

/* Format given number to editor field, no checks performed, all signals
   are sent normally. */
static void
hildon_number_editor_real_set_value             (HildonNumberEditorPrivate *priv, 
                                                 gint value)
{
    /* FIXME: That looks REALLY bad */
    gchar buffer[32];

    /* Update text in entry to new value */
    g_snprintf (buffer, sizeof (buffer), "%d", value);
    gtk_entry_set_text (GTK_ENTRY (priv->num_entry), buffer);
}

static gboolean
hildon_number_editor_button_pressed             (GtkWidget *widget, 
                                                 GdkEventButton *event,
                                                 gpointer data)
{
    /* FIXME: XXX Why aren't we using hildon_number_editor_start_timer here? XXX */
    /* Need to fetch current value from entry and increment or decrement
       it */

    HildonNumberEditor *editor;
    HildonNumberEditorPrivate *priv;
    GtkSettings *settings;
    guint timeout;

    g_assert (HILDON_IS_NUMBER_EDITOR (data));

    editor = HILDON_NUMBER_EDITOR (data);
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-timeout-initial", &timeout, NULL);

    /* Save type of button pressed */
    if (GTK_BUTTON (widget) == GTK_BUTTON (priv->plus))
        priv->button_type = 1;
    else
        priv->button_type = -1;

    /* Start repetition timer */
    if (! priv->button_event_id)
    {
        change_numbers (editor, priv->button_type);
        priv->button_event_id = g_timeout_add (timeout,
                (GSourceFunc) hildon_number_editor_start_timer,
                editor);
    }

    return FALSE;
}

static gboolean
hildon_number_editor_start_timer                (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;
    GtkSettings *settings;
    guint timeout;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    settings = gtk_settings_get_default ();
    g_object_get (settings, "gtk-timeout-repeat", &timeout, NULL);

    priv->button_event_id = g_timeout_add (timeout,
            (GSourceFunc) do_mouse_timeout,
            editor);

    return FALSE;
}

static gboolean
do_mouse_timeout                                (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;
    g_assert (HILDON_IS_NUMBER_EDITOR (editor));
    
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    GDK_THREADS_ENTER ();

    /* Update value based on button held */
    change_numbers (editor, priv->button_type);

    GDK_THREADS_LEAVE ();

    return TRUE;
}

/* Changes the current number value by the amount of update
   and verifies the result. */
static void
change_numbers                                  (HildonNumberEditor *editor, 
                                                 gint update)
{
    HildonNumberEditorPrivate *priv;
    gint current_value;

    g_assert (HILDON_IS_NUMBER_EDITOR (editor));

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    current_value = hildon_number_editor_get_value (editor);

    /* We need to rerun validation by hand, since validation
       done in "changed" callback allows intermediate values */
    hildon_number_editor_real_set_value (priv, current_value + update);
    hildon_number_editor_validate_value (editor, FALSE);
    g_object_notify (G_OBJECT (editor), "value");
}

static void
add_select_all_idle                             (HildonNumberEditorPrivate *priv)
{
    g_assert (priv);

    if (! priv->select_all_idle_id)
    {
        priv->select_all_idle_id =
            g_idle_add((GSourceFunc) hildon_number_editor_select_all, priv);
    }    
}

static void
hildon_number_editor_validate_value             (HildonNumberEditor *editor, 
                                                 gboolean allow_intermediate)
{
    HildonNumberEditorPrivate *priv;
    gint error_code, fixup_value;
    const gchar *text;
    long value;
    gchar *tail;
    gboolean r;

    g_assert (HILDON_IS_NUMBER_EDITOR(editor));

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    text = gtk_entry_get_text (GTK_ENTRY (priv->num_entry));
    error_code = -1;
    fixup_value = priv->default_val;

    if (text && text[0])
    { 
        /* Try to convert entry text to number */
        value = strtol (text, &tail, 10);

        /* Check if conversion succeeded */
        if (tail[0] == 0)
        {    
            /* Check if value is in allowed range. This is tricky in those
               cases when user is editing a value. 
               For example: Range = [100, 500] and user have just inputted "4".
               This should not lead into error message. Otherwise value is
               resetted back to "100" and next "4" press will reset it back
               and so on. */
            if (allow_intermediate)
            {
                /* We now have the following error cases:
                 * If inputted value as above maximum and
                 maximum is either positive or then maximum
                 negative and value is positive.
                 * If inputted value is below minimum and minimum
                 is negative or minumum positive and value
                 negative or zero.
                 In all other cases situation can be fixed just by
                 adding new numbers to the string.
                 */
                if (value > priv->end && (priv->end >= 0 || (priv->end < 0 && value >= 0)))
                {
                    error_code = HILDON_NUMBER_EDITOR_ERROR_MAXIMUM_VALUE_EXCEED;
                    fixup_value = priv->end;
                }
                else if (value < priv->start && (priv->start < 0 || (priv->start >= 0 && value <= 0)))
                {
                    error_code = HILDON_NUMBER_EDITOR_ERROR_MINIMUM_VALUE_EXCEED;
                    fixup_value = priv->start;
                }
            }
            else
            {
                if (value > priv->end) {
                    error_code = HILDON_NUMBER_EDITOR_ERROR_MAXIMUM_VALUE_EXCEED;
                    fixup_value = priv->end;
                }
                else if (value < priv->start) {
                    error_code = HILDON_NUMBER_EDITOR_ERROR_MINIMUM_VALUE_EXCEED;
                    fixup_value = priv->start;
                }
            }
        }
        /* The only valid case when conversion can fail is when we
           have plain '-', intermediate forms are allowed AND
           minimum bound is negative */
        else if (! allow_intermediate || strcmp (text, "-") != 0 || priv->start >= 0)
            error_code = HILDON_NUMBER_EDITOR_ERROR_ERRONEOUS_VALUE;
    }
    else if (! allow_intermediate)
        error_code = HILDON_NUMBER_EDITOR_ERROR_ERRONEOUS_VALUE;

    if (error_code != -1)
    {
        /* If entry is empty and intermediate forms are nor allowed, 
           emit error signal */
        /* Change to default value */
        hildon_number_editor_set_value (editor, fixup_value);
        g_signal_emit (editor, HildonNumberEditor_signal[RANGE_ERROR], 0, error_code, &r);
        add_select_all_idle (priv);
    }
}

static void 
hildon_number_editor_entry_changed              (GtkWidget *widget, 
                                                 gpointer data)
{
    g_assert (HILDON_IS_NUMBER_EDITOR (data));
    hildon_number_editor_validate_value (HILDON_NUMBER_EDITOR (data), TRUE);
    g_object_notify (G_OBJECT (data), "value");
}

static void
hildon_number_editor_size_request               (GtkWidget *widget,
                                                 GtkRequisition *requisition)
{
    HildonNumberEditor *editor;
    HildonNumberEditorPrivate *priv;
    GtkRequisition req;

    editor = HILDON_NUMBER_EDITOR (widget);
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    /* Requested size is size of all child widgets plus border space */
    gtk_widget_size_request (priv->minus, &req);
    requisition->width = req.width;

    gtk_widget_size_request (priv->num_entry, &req);
    requisition->width += req.width;

    gtk_widget_size_request (priv->plus, &req);
    requisition->width += req.width;

    requisition->width += HILDON_MARGIN_DEFAULT * 2;

    /* FIXME: XXX Height is fixed */
    requisition->height = NUMBER_EDITOR_HEIGHT;
}

/* Update alloc->width so widget fits, update alloc->x to point to free space */
static void
set_widget_allocation                           (GtkWidget *widget, 
                                                 GtkAllocation *alloc,
                                                 const GtkAllocation *allocation)
{
    GtkRequisition child_requisition;

    gtk_widget_get_child_requisition (widget, &child_requisition);

    /* Fit to widget width */
    if (allocation->width + allocation->x > alloc->x + child_requisition.width)
        alloc->width = child_requisition.width;
    else
    {
        alloc->width = allocation->width - (alloc->x - allocation->x);
        if (alloc->width < 0)
            alloc->width = 0;
    }

    gtk_widget_size_allocate (widget, alloc);
    /* Update x position */
    alloc->x += alloc->width;
}

static void
hildon_number_editor_size_allocate              (GtkWidget *widget,
                                                 GtkAllocation *allocation)
{
    HildonNumberEditor *editor;
    HildonNumberEditorPrivate *priv;
    GtkAllocation alloc;

    editor = HILDON_NUMBER_EDITOR (widget);
    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    widget->allocation = *allocation;

    /* Add upper border */
    alloc.y = widget->allocation.y + widget->style->ythickness;

    /* Fix height */
    if (widget->allocation.height > NUMBER_EDITOR_HEIGHT)
    {
        alloc.height = NUMBER_EDITOR_HEIGHT - widget->style->ythickness * 2;
        alloc.y += (widget->allocation.height - NUMBER_EDITOR_HEIGHT) / 2;
    }
    else
        alloc.height = widget->allocation.height - widget->style->ythickness * 2;

    if (alloc.height < 0)
        alloc.height = 0;

    /* Add left border */
    alloc.x = allocation->x + widget->style->xthickness;

    /* Allocate positions for widgets (left-to-right) */
    set_widget_allocation(priv->minus, &alloc, &widget->allocation);
    alloc.x += HILDON_MARGIN_DEFAULT;

    set_widget_allocation(priv->num_entry, &alloc, &widget->allocation);
    alloc.x += HILDON_MARGIN_DEFAULT;

    set_widget_allocation(priv->plus, &alloc, &widget->allocation);
}

static gboolean
hildon_number_editor_entry_focusout             (GtkWidget *widget, 
                                                 GdkEventFocus *event,
                                                 gpointer data)
{
    g_assert (HILDON_IS_NUMBER_EDITOR(data));

    hildon_number_editor_validate_value (HILDON_NUMBER_EDITOR(data), FALSE);
    return FALSE;
}

static gboolean
hildon_number_editor_entry_keypress             (GtkWidget *widget, 
                                                 GdkEventKey *event,
                                                 gpointer data)
{
    GtkEditable *editable;
    gint cursor_pos;

    g_assert (HILDON_IS_NUMBER_EDITOR (data));

    editable = GTK_EDITABLE (widget);
    cursor_pos = gtk_editable_get_position (editable);

    switch (event->keyval)
    {
        case GDK_Left:
            /* If the cursor is on the left, try to decrement */
            if (cursor_pos == 0) {
                change_numbers (HILDON_NUMBER_EDITOR (data), -1);
                return TRUE;
            }
            break;

        case GDK_Right:
            /* If the cursor is on the right, try to increment */
            if (cursor_pos >= g_utf8_strlen(gtk_entry_get_text (GTK_ENTRY (widget)), -1))
            {
                change_numbers (HILDON_NUMBER_EDITOR (data), 1);
                gtk_editable_set_position(editable, cursor_pos);
                return TRUE;
            }
            break;

        default:
            break;
    };

    return FALSE;
}

static gboolean
hildon_number_editor_range_error                (HildonNumberEditor *editor,
                                                 HildonNumberEditorErrorType type)
{

    gint min, max;
    gchar *err_msg = NULL;
    HildonNumberEditorPrivate *priv;

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    min = priv->start;
    max = priv->end;

    /* Construct error message */
    switch (type)
    {
        case HILDON_NUMBER_EDITOR_ERROR_MAXIMUM_VALUE_EXCEED:
            err_msg = g_strdup_printf (_("ckct_ib_maximum_value"), max, max);
            break;

        case HILDON_NUMBER_EDITOR_ERROR_MINIMUM_VALUE_EXCEED:
            err_msg = g_strdup_printf (_("ckct_ib_minimum_value"), min, min);
            break;

        case HILDON_NUMBER_EDITOR_ERROR_ERRONEOUS_VALUE:
            err_msg =
                g_strdup_printf (_("ckct_ib_set_a_value_within_range"), min, max);
            break;
    }

    /* Infoprint error */
    if (err_msg)
    {
        hildon_banner_show_information (GTK_WIDGET (GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET(editor),
                                        GTK_TYPE_WINDOW))), NULL, err_msg);
        g_free(err_msg);
    }

    return TRUE;
}

/**
 * hildon_number_editor_new:
 * @min: minimum accepted value
 * @max: maximum accepted value
 * 
 * Creates new number editor
 *
 * Returns: a new #HildonNumberEditor widget
 */
GtkWidget*
hildon_number_editor_new                        (gint min, 
                                                 gint max)
{
    HildonNumberEditor *editor = g_object_new (HILDON_TYPE_NUMBER_EDITOR, NULL);

    /* Set user inputted range to editor */
    hildon_number_editor_set_range (editor, min, max);

    return GTK_WIDGET (editor);
}

/**
 * hildon_number_editor_set_range:
 * @editor: a #HildonNumberEditor widget
 * @min: minimum accepted value
 * @max: maximum accepted value
 *
 * Sets accepted number range for editor
 */
void
hildon_number_editor_set_range                  (HildonNumberEditor *editor, 
                                                 gint min, 
                                                 gint max)
{
    HildonNumberEditorPrivate *priv;
    gchar buffer_min[32], buffer_max[32];
    gint a, b;

    g_return_if_fail (HILDON_IS_NUMBER_EDITOR (editor));

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    /* Set preferences */
    priv->start = MIN (min, max);
    priv->end = MAX (min, max);

    /* Find maximum allowed length of value */
    g_snprintf (buffer_min, sizeof (buffer_min), "%d", min);
    g_snprintf (buffer_max, sizeof (buffer_max), "%d", max);
    a = strlen (buffer_min);
    b = strlen (buffer_max);

    /* Set maximum size of entry */
    gtk_entry_set_width_chars (GTK_ENTRY (priv->num_entry), MAX (a, b));
    hildon_number_editor_set_value (editor, priv->start);
}

/**
 * hildon_number_editor_get_value:
 * @editor: pointer to #HildonNumberEditor
 *
 * Returns: current NumberEditor value
 */
gint
hildon_number_editor_get_value                  (HildonNumberEditor *editor)
{
    HildonNumberEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_NUMBER_EDITOR (editor), 0);

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    return atoi (gtk_entry_get_text (GTK_ENTRY (priv->num_entry)));
}

/**
 * hildon_number_editor_set_value:
 * @editor: pointer to #HildonNumberEditor
 * @value: numeric value for number editor
 *
 * Sets numeric value for number editor
 */
void
hildon_number_editor_set_value                  (HildonNumberEditor *editor, 
                                                 gint value)
{
    HildonNumberEditorPrivate *priv;

    g_return_if_fail (HILDON_IS_NUMBER_EDITOR (editor));

    priv = HILDON_NUMBER_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    g_return_if_fail (value <= priv->end);
    g_return_if_fail (value >= priv->start);

    priv->default_val = value;
    hildon_number_editor_real_set_value (priv, value);
    g_object_notify (G_OBJECT(editor), "value");
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
hildon_number_editor_select_all                 (HildonNumberEditorPrivate *priv)
{   
    GDK_THREADS_ENTER ();
    gtk_editable_select_region (GTK_EDITABLE (priv->num_entry), 0, -1);
    priv->select_all_idle_id = 0;
    GDK_THREADS_LEAVE ();
    return FALSE;
} 

static void
hildon_number_editor_set_property               (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonNumberEditor *editor;

    editor = HILDON_NUMBER_EDITOR (object);

    switch (prop_id) {

        case PROP_VALUE:
            hildon_number_editor_set_value (editor, g_value_get_int (value));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
hildon_number_editor_get_property               (GObject *object,
                                                 guint prop_id, 
                                                 GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonNumberEditor *editor;

    editor = HILDON_NUMBER_EDITOR (object);

    switch (prop_id) {

        case PROP_VALUE:
            g_value_set_int(value, hildon_number_editor_get_value (editor));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

