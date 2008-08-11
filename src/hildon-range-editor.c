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
 * SECTION:hildon-range-editor
 * @short_description: A widget is used to ask bounds of a range.
 *
 * HidlonRangeEditor allows entering a pair of integers, e.g. the lower
 * and higher bounds of a range. A minimum and maximum can also be set
 * for the bounds.
 * 
 * <example>
 * <programlisting>
 *      range_editor = hildon_range_editor_new ();
 *      hildon_range_editor_set_limits (editor, start, end );
 *      hildon_range_editor_get_range (editor, &amp;start, &amp;end);
 * </programlisting>
 * </example>
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-range-editor.h"
#include                                        <gtk/gtkbox.h>
#include                                        <gtk/gtklabel.h>
#include                                        <gtk/gtksignal.h>
#include                                        <gtk/gtkentry.h>
#include                                        <gdk/gdkkeysyms.h>
#include                                        <glib/gprintf.h>
#include                                        <string.h>
#include                                        <stdlib.h>
#include                                        "hildon-banner.h"
#include                                        <libintl.h>
#include                                        "hildon-range-editor-private.h"

#define                                         _(string) \
                                                dgettext("hildon-libs", string)

/* Alignment in entry box ( 0 = left, 1 = right ) */

#define                                         DEFAULT_ALIGNMENT 1

/* Amount of padding to add to each side of the separator */

#define                                         DEFAULT_PADDING 3

#define                                         DEFAULT_START -999

#define                                         DEFAULT_END    999

#define                                         DEFAULT_LENGTH 4

static GtkContainerClass                        *parent_class = NULL;

static void
hildon_range_editor_class_init                  (HildonRangeEditorClass *editor_class);

static void
hildon_range_editor_init                        (HildonRangeEditor *editor);

static void
hildon_range_editor_forall                      (GtkContainer *container,
                                                 gboolean include_internals, 
                                                 GtkCallback callback,
                                                 gpointer callback_data);

static void
hildon_range_editor_destroy                     (GtkObject *self);

static void
hildon_range_editor_size_request                (GtkWidget *widget,
                                                 GtkRequisition *requisition);

static void
hildon_range_editor_size_allocate               (GtkWidget *widget,
                                                 GtkAllocation *allocation);

static gboolean
hildon_range_editor_entry_focus_in              (GtkEditable *editable,
                                                 GdkEventFocus *event,
                                                 HildonRangeEditor *editor);

static gboolean
hildon_range_editor_entry_focus_out             (GtkEditable *editable,
                                                 GdkEventFocus *event,
                                                 HildonRangeEditor *editor);

static gboolean
hildon_range_editor_entry_keypress              (GtkWidget *widget, 
                                                 GdkEventKey *event,
                                                 HildonRangeEditor *editor);

static gboolean
hildon_range_editor_released                    (GtkEditable *editable, 
                                                 GdkEventButton *event,
                                                 HildonRangeEditor *editor);

static gboolean
hildon_range_editor_press                       (GtkEditable *editable, 
                                                 GdkEventButton *event,
                                                 HildonRangeEditor *editor);

static void 
hildon_range_editor_set_property                (GObject *object, 
                                                 guint param_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec);

static void
hildon_range_editor_get_property                (GObject *object, 
                                                 guint param_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec);

static void
hildon_range_editor_entry_changed               (GtkWidget *widget, 
                                                 HildonRangeEditor *editor);

enum
{
    PROP_0,
    PROP_LOWER = 1,
    PROP_HIGHER,
    PROP_MIN,
    PROP_MAX,
    PROP_SEPARATOR
};

static void
hildon_range_editor_class_init                  (HildonRangeEditorClass *editor_class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (editor_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (editor_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS (editor_class);

    parent_class = g_type_class_peek_parent (editor_class);

    g_type_class_add_private (editor_class, sizeof (HildonRangeEditorPrivate));

    gobject_class->set_property = hildon_range_editor_set_property;
    gobject_class->get_property = hildon_range_editor_get_property;
    widget_class->size_request  = hildon_range_editor_size_request;
    widget_class->size_allocate = hildon_range_editor_size_allocate;

    container_class->forall = hildon_range_editor_forall;
    GTK_OBJECT_CLASS (editor_class)->destroy = hildon_range_editor_destroy;

    gtk_widget_class_install_style_property (widget_class,
            g_param_spec_int ("hildon_range_editor_entry_alignment",
                "Hildon RangeEditor entry alignment",
                "Hildon RangeEditor entry alignment", 0, 1,
                DEFAULT_ALIGNMENT,
                G_PARAM_READABLE));

    gtk_widget_class_install_style_property (widget_class,
            g_param_spec_int ("hildon_range_editor_separator_padding",
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
    g_object_class_install_property (gobject_class, PROP_MIN,
            g_param_spec_int ("min",
                "Minimum value",
                "Minimum value in a range",
                G_MININT, G_MAXINT,
                DEFAULT_START, G_PARAM_CONSTRUCT | 
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonRangeEditor:max:
     *
     * Maximum value in a range.
     * Default: 999
     */
    g_object_class_install_property (gobject_class, PROP_MAX,
            g_param_spec_int ("max",
                "Maximum value",
                "Maximum value in a range",
                G_MININT, G_MAXINT,
                DEFAULT_END, G_PARAM_CONSTRUCT | 
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonRangeEditor:lower:
     *
     * Current value in the entry presenting lower end of selected range.
     * Default: -999
     */
    g_object_class_install_property (gobject_class, PROP_LOWER,
            g_param_spec_int ("lower",
                "Current lower value",
                "Current value in the entry presenting lower end of selected range",
                G_MININT, G_MAXINT,
                DEFAULT_START, G_PARAM_CONSTRUCT | 
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonRangeEditor:higher:
     *
     * Current value in the entry presenting higher end of selected range.
     * Default: 999
     */
    g_object_class_install_property (gobject_class, PROP_HIGHER,
            g_param_spec_int ("higher",
                "Current higher value",
                "Current value in the entry presenting higher end of selected range",
                G_MININT, G_MAXINT,
                DEFAULT_END, G_PARAM_CONSTRUCT | 
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonRangeEditor:separator:
     *
     * Separator string to separate range editor entries.
     * Default: "-"
     */
    g_object_class_install_property (gobject_class, PROP_SEPARATOR,
            g_param_spec_string ("separator",
                "Separator",
                "Separator string to separate entries",
                _("ckct_wi_range_separator"),
                G_PARAM_CONSTRUCT | 
                G_PARAM_READABLE | G_PARAM_WRITABLE));
}

static void
hildon_range_editor_init                        (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;

    gint range_editor_entry_alignment;
    gint range_editor_separator_padding;

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    GTK_WIDGET_SET_FLAGS (editor, GTK_NO_WINDOW);

    gtk_widget_push_composite_child ();

    priv->start_entry = gtk_entry_new ();
    priv->end_entry = gtk_entry_new ();
    priv->label = gtk_label_new (_("ckct_wi_range_separator"));
    priv->bp = FALSE;

    /* Get values from gtkrc (or use defaults) */
    /* FIXME: This is broken, styles are not yet attached */
    gtk_widget_style_get (GTK_WIDGET (editor),
            "hildon_range_editor_entry_alignment",
            &range_editor_entry_alignment,
            "hildon_range_editor_separator_padding",
            &range_editor_separator_padding, NULL);

    /* Add padding to separator */
    gtk_misc_set_padding (GTK_MISC (priv->label),
            range_editor_separator_padding, 0);

    /* Align the text to right in entry box */
    gtk_entry_set_alignment (GTK_ENTRY (priv->start_entry),
            range_editor_entry_alignment);
    gtk_entry_set_alignment (GTK_ENTRY (priv->end_entry),
            range_editor_entry_alignment);

    gtk_widget_set_composite_name (priv->start_entry, "start_entry");
    gtk_widget_set_composite_name (priv->end_entry, "end_entry");
    gtk_widget_set_composite_name (priv->label, "separator_label");
    gtk_widget_set_parent (priv->start_entry, GTK_WIDGET (editor));
    gtk_widget_set_parent (priv->end_entry, GTK_WIDGET (editor));
    gtk_widget_set_parent (priv->label, GTK_WIDGET (editor));

    g_signal_connect (G_OBJECT (priv->start_entry), "button-release-event",
            G_CALLBACK (hildon_range_editor_released), editor);
    g_signal_connect (G_OBJECT (priv->end_entry), "button-release-event",
            G_CALLBACK (hildon_range_editor_released), editor);

    g_signal_connect (G_OBJECT (priv->start_entry), "button-press-event",
            G_CALLBACK (hildon_range_editor_press), editor);
    g_signal_connect (G_OBJECT (priv->end_entry), "button-press-event",
            G_CALLBACK (hildon_range_editor_press), editor);

    g_signal_connect (G_OBJECT (priv->start_entry), "key-press-event",
            G_CALLBACK (hildon_range_editor_entry_keypress), editor);
    g_signal_connect (G_OBJECT (priv->end_entry), "key-press-event",
            G_CALLBACK (hildon_range_editor_entry_keypress), editor);

    g_signal_connect (G_OBJECT (priv->start_entry), "focus-in-event",
            G_CALLBACK (hildon_range_editor_entry_focus_in), editor);
    g_signal_connect (G_OBJECT (priv->end_entry), "focus-in-event",
            G_CALLBACK (hildon_range_editor_entry_focus_in), editor);

    g_signal_connect (G_OBJECT (priv->start_entry), "focus-out-event",
            G_CALLBACK (hildon_range_editor_entry_focus_out), editor);
    g_signal_connect (G_OBJECT (priv->end_entry), "focus-out-event",
            G_CALLBACK (hildon_range_editor_entry_focus_out), editor);
    g_signal_connect (priv->start_entry, "changed", 
            G_CALLBACK (hildon_range_editor_entry_changed), editor);
    g_signal_connect (priv->end_entry, "changed", 
            G_CALLBACK (hildon_range_editor_entry_changed), editor);

#ifdef MAEMO_GTK 
    g_object_set (G_OBJECT (priv->start_entry),
            "hildon-input-mode", HILDON_GTK_INPUT_MODE_NUMERIC, NULL);

    g_object_set( G_OBJECT (priv->end_entry),
            "hildon-input-mode", HILDON_GTK_INPUT_MODE_NUMERIC, NULL);
#endif 

    gtk_widget_show (priv->start_entry);
    gtk_widget_show (priv->end_entry);
    gtk_widget_show (priv->label);

    gtk_widget_pop_composite_child();
}

static void 
hildon_range_editor_set_property                (GObject *object, 
                                                 guint param_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec)
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

static void
hildon_range_editor_get_property                (GObject *object, 
                                                 guint param_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec)
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

static void
hildon_range_editor_entry_validate              (HildonRangeEditor *editor, 
                                                 GtkWidget *edited_entry, 
                                                 gboolean allow_intermediate)
{
    HildonRangeEditorPrivate *priv;
    const gchar *text;
    long value;
    gint min, max, fixup;
    gchar *tail;
    gchar buffer[256];
    gboolean error = FALSE;

    g_assert(HILDON_IS_RANGE_EDITOR(editor));
    g_assert(GTK_IS_ENTRY(edited_entry));

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
    g_assert (priv);

    /* Find the valid range for the modified component */
    if (edited_entry == priv->start_entry) {
        min = hildon_range_editor_get_min (editor);
        max = hildon_range_editor_get_higher (editor);
    } else {
        min = hildon_range_editor_get_lower (editor);
        max = hildon_range_editor_get_max (editor);
    }

    text = gtk_entry_get_text (GTK_ENTRY (edited_entry));

    if (text && text [0])
    { 
        /* Try to convert entry text to number */
        value = strtol(text, &tail, 10);

        /* Check if conversion succeeded */
        if (tail [0] == 0)
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
                 negative.
                 In all other cases situation can be fixed just by
                 adding new numbers to the string.
                 */
                if (value > max && (max >= 0 || (max < 0 && value >= 0)))
                {
                    error = TRUE;
                    fixup = max;
                    g_snprintf (buffer, sizeof (buffer), _("ckct_ib_maximum_value"), max);
                }
                else if (value < min && (min < 0 || (min >= 0 && value < 0)))
                {
                    error = TRUE;
                    fixup = min;
                    g_snprintf (buffer, sizeof (buffer), _("ckct_ib_minimum_value"), min);
                }
            }
            else
            {
                if (value > max) {
                    error = TRUE;
                    fixup = max;
                    g_snprintf (buffer, sizeof (buffer), _("ckct_ib_maximum_value"), max);
                }
                else if (value < min) {
                    error = TRUE;
                    fixup = min;
                    g_snprintf (buffer, sizeof (buffer), _("ckct_ib_minimum_value"), min);
                }
            }

            if (error) {
                if (edited_entry == priv->start_entry)
                    hildon_range_editor_set_lower (editor, fixup);
                else
                    hildon_range_editor_set_higher (editor, fixup);
            }
        }
        /* The only valid case when conversion can fail is when we
           have plain '-', intermediate forms are allowed AND
           minimum bound is negative */
        else if (!allow_intermediate || strcmp(text, "-") != 0 || min >= 0) {
            error = TRUE;
            g_snprintf (buffer, sizeof (buffer), _("ckct_ib_set_a_value_within_range"), min, max);
        }
    }
    else if (! allow_intermediate) {
        error = TRUE;
        g_snprintf (buffer, sizeof (buffer), _("ckct_ib_set_a_value_within_range"), min, max);
    }

    if (error)
    {
        hildon_banner_show_information (edited_entry, NULL, buffer);
        gtk_widget_grab_focus (edited_entry);
    }
}

static gboolean
hildon_range_editor_entry_focus_in              (GtkEditable *editable,
                                                 GdkEventFocus *event,
                                                 HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
    g_assert (priv);

    if (priv->bp)
    {
        priv->bp = FALSE;
        return FALSE;
    }

    if (GTK_WIDGET (editable) == priv->start_entry)
        gtk_editable_select_region (editable, -1, 0);
    else
        gtk_editable_select_region (editable, 0, -1);
    return FALSE;
}

/* Gets and sets the current range. This has two useful side effects:
 * Values are now sorted to the correct order
 * Out of range values are clamped to range */
static void 
hildon_range_editor_apply_current_range         (HildonRangeEditor *editor)
{
    g_assert (HILDON_IS_RANGE_EDITOR (editor));

    hildon_range_editor_set_range (editor,
            hildon_range_editor_get_lower (editor),
            hildon_range_editor_get_higher (editor));
}

static void 
hildon_range_editor_entry_changed               (GtkWidget *widget, 
                                                 HildonRangeEditor *editor)
{
    g_assert (HILDON_IS_RANGE_EDITOR (editor));

    hildon_range_editor_entry_validate (editor, widget, TRUE);
}

static gboolean
hildon_range_editor_entry_focus_out             (GtkEditable *editable,
                                                 GdkEventFocus *event,
                                                 HildonRangeEditor *editor)
{
    g_assert (HILDON_IS_RANGE_EDITOR(editor));

    hildon_range_editor_entry_validate (editor, GTK_WIDGET (editable), FALSE);  
    return FALSE;
}

static gboolean
hildon_range_editor_press                       (GtkEditable *editable, 
                                                 GdkEventButton *event,
                                                 HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
    g_assert (priv);

    priv->bp = TRUE;
    return FALSE;
}

static void
hildon_range_editor_forall                      (GtkContainer *container,
                                                 gboolean include_internals,
                                                 GtkCallback callback, 
                                                 gpointer callback_data)
{
    HildonRangeEditorPrivate *priv;

    g_assert (HILDON_IS_RANGE_EDITOR (container));
    g_assert (callback != NULL);

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (container);
    g_assert (priv);

    if (! include_internals)
        return;

    (*callback) (priv->start_entry, callback_data);
    (*callback) (priv->end_entry, callback_data);
    (*callback) (priv->label, callback_data);
}

static void
hildon_range_editor_destroy                     (GtkObject *self)
{
    HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE (self);

    if (priv->start_entry)
    {
        gtk_widget_unparent (priv->start_entry);
        priv->start_entry = NULL;
    }
    if (priv->end_entry)
    {
        gtk_widget_unparent (priv->end_entry);
        priv->end_entry = NULL;
    }
    if (priv->label)
    {
        gtk_widget_unparent (priv->label);
        priv->label = NULL;
    }

    if (GTK_OBJECT_CLASS (parent_class)->destroy)
        GTK_OBJECT_CLASS (parent_class)->destroy (self);
}


static void
hildon_range_editor_size_request                (GtkWidget *widget,
                                                 GtkRequisition *requisition)
{
    HildonRangeEditorPrivate *priv = NULL;
    GtkRequisition lab_req, mreq;

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (widget);
    g_assert (priv);

    gtk_entry_get_width_chars (GTK_ENTRY (priv->end_entry));

    gtk_widget_size_request (priv->start_entry, &mreq);
    gtk_widget_size_request (priv->end_entry, &mreq);
    gtk_widget_size_request (priv->label, &lab_req);

    /* Width for entries and separator label and border */
    requisition->width = mreq.width * 2 + lab_req.width +
        widget->style->xthickness * 2;
    /* Add vertical border */
    requisition->height = mreq.height + widget->style->ythickness * 2;
    /* Fit label height */
    requisition->height = MAX (requisition->height, lab_req.height);
}

static void
hildon_range_editor_size_allocate               (GtkWidget *widget,
                                                 GtkAllocation *allocation)
{
    HildonRangeEditorPrivate *priv;
    GtkAllocation child1_allocation, child2_allocation, child3_allocation;

    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (widget);
    g_assert (priv);

    widget->allocation = *allocation;

    /* Allocate entries, left-to-right */
    if (priv->start_entry && GTK_WIDGET_VISIBLE (priv->start_entry))
    {
        GtkRequisition child_requisition;

        gtk_widget_get_child_requisition (priv->start_entry,
                &child_requisition);

        child1_allocation.x = allocation->x;
        child1_allocation.y = allocation->y;

        child1_allocation.width = child_requisition.width;
        child1_allocation.height = allocation->height;

        gtk_widget_size_allocate (priv->start_entry, &child1_allocation);
    }

    if (priv->label && GTK_WIDGET_VISIBLE (priv->label))
    {
        GtkRequisition child_requisition;

        gtk_widget_get_child_requisition (priv->label, &child_requisition);

        child2_allocation.x = child1_allocation.x + child1_allocation.width;
        child2_allocation.y = allocation->y;
        /* Add spacing */
        child2_allocation.width = child_requisition.width + 4;
        child2_allocation.height = allocation->height;

        gtk_widget_size_allocate (priv->label, &child2_allocation);
    }

    if (priv->end_entry && GTK_WIDGET_VISIBLE (priv->end_entry))
    {
        GtkRequisition child_requisition;

        gtk_widget_get_child_requisition (priv->end_entry, &child_requisition);

        child3_allocation.x = child2_allocation.x + child2_allocation.width;
        child3_allocation.y = allocation->y;

        child3_allocation.width = child_requisition.width;
        child3_allocation.height = allocation->height;

        gtk_widget_size_allocate (priv->end_entry, &child3_allocation);
    }
}

/* Button released inside entries */
static gboolean
hildon_range_editor_released                    (GtkEditable *editable, 
                                                 GdkEventButton *event,
                                                 HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv = HILDON_RANGE_EDITOR_GET_PRIVATE(editor);
    g_assert (priv);

    if (GTK_WIDGET (editable) == priv->start_entry)
        gtk_editable_select_region(editable, -1, 0);
    else
        gtk_editable_select_region(editable, 0, -1); 

    return FALSE;
}

static gboolean
hildon_range_editor_entry_keypress              (GtkWidget *widget, 
                                                 GdkEventKey *event,
                                                 HildonRangeEditor *editor)
{
    const gchar *text;
    gint cursor_pos;

    g_assert(HILDON_IS_RANGE_EDITOR (editor));

    text = gtk_entry_get_text (GTK_ENTRY (widget));
    cursor_pos = gtk_editable_get_position (GTK_EDITABLE (widget));

    switch (event->keyval)
    {
        case GDK_Left:
            /* If we are on the first character and press left, 
               try to move to previous field */
            if (cursor_pos == 0) {
                (void) gtk_widget_child_focus (GTK_WIDGET (editor), GTK_DIR_LEFT);
                return TRUE;
            }
            break;

        case GDK_Right:
            /* If the cursor is on the right, try to move to the next field */
            if (cursor_pos >= g_utf8_strlen (text, -1)) {
                (void) gtk_widget_child_focus (GTK_WIDGET (editor), GTK_DIR_RIGHT);
                return TRUE;
            }
            break;

        default:
            break;
    };

    return FALSE;
}

static void 
hildon_range_editor_refresh_widths              (HildonRangeEditorPrivate *priv)
{
    gchar start_range[32], end_range[32];
    gint length;

    /* Calculate length of entry so extremes would fit */
    g_snprintf (start_range, sizeof (start_range), "%d", priv->range_limits_start);
    g_snprintf (end_range, sizeof (end_range), "%d", priv->range_limits_end);
    length = MAX (g_utf8_strlen (start_range, -1), g_utf8_strlen (end_range, -1));

    gtk_entry_set_width_chars (GTK_ENTRY (priv->start_entry), length);
    gtk_entry_set_max_length (GTK_ENTRY (priv->start_entry), length);
    gtk_entry_set_width_chars (GTK_ENTRY (priv->end_entry), length);
    gtk_entry_set_max_length (GTK_ENTRY (priv->end_entry), length);
}

/**
 * hildon_range_editor_get_type:
 * 
 * Initializes, and returns the type of a hildon range editor.
 * 
 * @Returns : GType of #HildonRangeEditor
 * 
 */
GType G_GNUC_CONST
hildon_range_editor_get_type                    (void)
{
    static GType editor_type = 0;

    if (! editor_type)
    {
        static const GTypeInfo editor_info =
        {
            sizeof (HildonRangeEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_range_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonRangeEditor),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_range_editor_init,
        };
        editor_type = g_type_register_static (GTK_TYPE_CONTAINER,
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
 * Returns: pointer to a new @HildonRangeEditor widget
 */
GtkWidget*
hildon_range_editor_new                         (void)
{
    return GTK_WIDGET (g_object_new (HILDON_TYPE_RANGE_EDITOR, NULL));
}

/**
 * hildon_range_editor_new_with_separator:
 * @separator: a string that is shown between the numbers
 *
 * HildonRangeEditor contains two Gtk entries that accept numbers. 
 * A separator is displayed between two entries. 
 * CHECKME: Use '-' as a separator in the case of null separator?
 * 
 * Returns: pointer to a new @HildonRangeEditor widget
 */
GtkWidget*
hildon_range_editor_new_with_separator          (const gchar *separator)
{
    return GTK_WIDGET (g_object_new (HILDON_TYPE_RANGE_EDITOR,
                "separator", separator, NULL));
}

/**
 * hildon_range_editor_set_range:
 * @editor: the #HildonRangeEditor widget
 * @start: range's start value 
 * @end: range's end value
 *
 * Sets a range to the editor. (The current value)
 *
 * Sets the range of the @HildonRangeEditor widget.
 */
void
hildon_range_editor_set_range                   (HildonRangeEditor *editor, 
                                                 gint start,
                                                 gint end)
{
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));

    /* Make sure that the start/end appear in the correct order */
    hildon_range_editor_set_lower (editor, MIN (start, end));
    hildon_range_editor_set_higher (editor, MAX (start, end));
}

/**
 * hildon_range_editor_get_range:
 * @editor: the #HildonRangeEditor widget
 * @start: ranges start value
 * @end: ranges end value
 *
 * Gets the range of the @HildonRangeEditor widget.
 */
void
hildon_range_editor_get_range                   (HildonRangeEditor *editor, 
                                                 gint *start,
                                                 gint *end)
{
    HildonRangeEditorPrivate *priv;

    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor) && start && end);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    *start = hildon_range_editor_get_lower (editor);
    *end = hildon_range_editor_get_higher (editor);
}

/**
 * hildon_range_editor_set_limits:
 * @editor: the #HildonRangeEditor widget
 * @start: minimum acceptable value (default: no limit)
 * @end:   maximum acceptable value (default: no limit)
 *
 * Sets the range of the @HildonRangeEditor widget.
 */
void
hildon_range_editor_set_limits                  (HildonRangeEditor *editor, 
                                                 gint start,
                                                 gint end)
{
    /* FIXME: Setting start/end as separate steps can modify
       the inputted range unneedlesly */
    hildon_range_editor_set_min (editor, start);
    hildon_range_editor_set_max (editor, end);
}

void
hildon_range_editor_set_lower                   (HildonRangeEditor *editor, 
                                                 gint value)
{
    HildonRangeEditorPrivate *priv;
    gchar buffer[32];

    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    g_snprintf(buffer, sizeof (buffer), "%d", 
            CLAMP (value, priv->range_limits_start, priv->range_limits_end));

    /* Update entry text with new value */
    gtk_entry_set_text (GTK_ENTRY (priv->start_entry), buffer);
    g_object_notify (G_OBJECT (editor), "lower");
}

void
hildon_range_editor_set_higher                  (HildonRangeEditor *editor, 
                                                 gint value)
{
    HildonRangeEditorPrivate *priv;
    gchar buffer[32];

    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    g_snprintf(buffer, sizeof(buffer), "%d", 
            CLAMP(value, priv->range_limits_start, priv->range_limits_end));

    /* Update entry text with new value */
    gtk_entry_set_text (GTK_ENTRY (priv->end_entry), buffer);
    g_object_notify (G_OBJECT (editor), "higher");
}

gint
hildon_range_editor_get_lower                   (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);
    return atoi(gtk_entry_get_text(GTK_ENTRY(priv->start_entry)));
}

gint
hildon_range_editor_get_higher                  (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);
    return atoi (gtk_entry_get_text(GTK_ENTRY (priv->end_entry)));
}

void
hildon_range_editor_set_min                     (HildonRangeEditor *editor, 
                                                 gint value)
{
    HildonRangeEditorPrivate *priv;

    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));

    /* We can cause many properties to change */
    g_object_freeze_notify (G_OBJECT(editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);
    priv->range_limits_start = value;

    if (priv->range_limits_end < value)
        hildon_range_editor_set_max (editor, value);
    /* Setting maximum applies widths and range in this case */
    else {
        hildon_range_editor_refresh_widths (priv);
        hildon_range_editor_apply_current_range (editor);
    }

    g_object_notify (G_OBJECT (editor), "min");
    g_object_thaw_notify (G_OBJECT (editor));
}

void
hildon_range_editor_set_max                     (HildonRangeEditor *editor, 
                                                 gint value)
{
    HildonRangeEditorPrivate *priv;

    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));

    /* We can cause many properties to change */
    g_object_freeze_notify (G_OBJECT (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);
    priv->range_limits_end = value;

    if (priv->range_limits_start > value)
        hildon_range_editor_set_min (editor, value);
    /* Setting minimum applies widths and range in this case */
    else {
        hildon_range_editor_refresh_widths (priv);
        hildon_range_editor_apply_current_range (editor);
    }

    g_object_notify (G_OBJECT (editor), "max");
    g_object_thaw_notify (G_OBJECT (editor));
}

gint
hildon_range_editor_get_min                     (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return priv->range_limits_start;
}

gint
hildon_range_editor_get_max                     (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), 0);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return priv->range_limits_end;
}

void
hildon_range_editor_set_separator               (HildonRangeEditor *editor,
                                                 const gchar *separator)
{
    HildonRangeEditorPrivate *priv;
    g_return_if_fail (HILDON_IS_RANGE_EDITOR (editor));
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    gtk_label_set_text (GTK_LABEL (priv->label), separator);
    g_object_notify (G_OBJECT(editor), "separator");
}

const gchar*
hildon_range_editor_get_separator               (HildonRangeEditor *editor)
{
    HildonRangeEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_RANGE_EDITOR (editor), NULL);
    priv = HILDON_RANGE_EDITOR_GET_PRIVATE (editor);

    return gtk_label_get_text (GTK_LABEL (priv->label));
}
