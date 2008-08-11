/* vim:set sw=4 expandtab cino=(0:
 *
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
 * SECTION:hildon-date-editor
 * @short_description: A widget which queries a date from user or opens
 * a HildonCalendarPopup.
 * @see_also: #HildonCalendarPopup, #HildonTimeEditor
 * 
 * HildonDateEditor is a widget with three entry fields (day, month,
 * year) and an icon (button): clicking on the icon opens up a
 * HildonCalendarPopup.
 *
 * <example>
 * <programlisting>
 * guint y, m, d;
 * GtkDialog *dialog;
 * GtkWidget *date_editor;
 * <!-- -->
 * dialog = GTK_DIALOG (gtk_dialog_new ());
 * date_editor = hildon_date_editor_new ();
 * <!-- -->
 * gtk_box_pack_start (GTK_BOX (dialog->vbox), gtk_label_new ("Choose a date"), FALSE, FALSE, 10);
 * gtk_box_pack_start (GTK_BOX (dialog->vbox), date_editor, FALSE, FALSE, 10);
 * gtk_dialog_add_button (dialog, "Close", GTK_RESPONSE_CANCEL);
 * <!-- -->
 * gtk_widget_show_all (GTK_WIDGET (dialog));
 * gtk_dialog_run (dialog);
 * <!-- -->
 * hildon_date_editor_get_date (HILDON_DATE_EDITOR (date_editor), &amp;y, &amp;m, &amp;d);
 * g_debug ("Date: %u-%u-%u", y, m, d);
 * <!-- -->
 * </programlisting>
 * </example>
 *
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-date-editor.h"
#include                                        <glib.h>
#include                                        <gtk/gtk.h>
#include                                        <gtk/gtkenums.h>
#include                                        <gdk/gdkkeysyms.h>
#include                                        <time.h>
#include                                        <stdlib.h>
#include                                        <stdio.h>
#include                                        <string.h>
#include                                        "hildon-calendar-popup.h"
#include                                        "hildon-defines.h"
#include                                        "hildon-private.h"
#include                                        "hildon-marshalers.h"
#include                                        "hildon-enum-types.h"
#include                                        "hildon-time-editor.h"
#include                                        "hildon-banner.h"
#include                                        <libintl.h>
#include                                        "hildon-date-editor-private.h"

#define                                         _(string) dgettext("hildon-libs", string)

#define                                         c_(string) dgettext("hildon-common-strings", string)

#define                                         ENTRY_BORDERS 11

#define                                         DATE_EDITOR_HEIGHT 30

#define                                         DAY_ENTRY_WIDTH 2

#define                                         MONTH_ENTRY_WIDTH 2

#define                                         YEAR_ENTRY_WIDTH 4

#define                                         DEFAULT_MIN_YEAR 1970

#define                                         DEFAULT_MAX_YEAR 2037

static GtkContainerClass*                       parent_class;

static void
hildon_date_editor_class_init                   (HildonDateEditorClass *editor_class);

static void 
hildon_date_editor_init                         (HildonDateEditor *editor);

static gboolean
hildon_date_editor_icon_press                   (GtkWidget *widget, 
                                                 gpointer data);

static gboolean
hildon_date_editor_released                     (GtkWidget *widget, 
                                                 gpointer data);

static gboolean
hildon_date_editor_keypress                     (GtkWidget *widget, 
                                                 GdkEventKey *event,
                                                 gpointer data);

static gboolean
hildon_date_editor_keyrelease                   (GtkWidget *widget, 
                                                 GdkEventKey *event,
                                                 gpointer data);
static gboolean
hildon_date_editor_clicked                      (GtkWidget *widget, 
                                                 gpointer data);

static gint
hildon_date_editor_entry_validate               (GtkWidget *widget, 
                                                 gpointer data);

static void
hildon_date_editor_entry_changed                (GtkEditable *widget, 
                                                 gpointer data);

static gboolean
hildon_date_editor_entry_focus_out              (GtkWidget *widget, 
                                                 GdkEventFocus *event,
                                                 gpointer data);

static gboolean
hildon_date_editor_date_error                   (HildonDateEditor *editor, 
                                                 HildonDateTimeError type);

static gboolean 
hildon_date_editor_entry_focus_in               (GtkWidget *widget,
                                                 GdkEventFocus *event,
                                                 gpointer data);

static void
hildon_date_editor_get_property                 (GObject *object, 
                                                 guint param_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec);

static void 
hildon_date_editor_set_property                 (GObject *object, 
                                                 guint param_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec);
static void
hildon_child_forall                             (GtkContainer *container,
                                                 gboolean include_internals,
                                                 GtkCallback callback, 
                                                 gpointer callback_data);

static void 
hildon_date_editor_destroy                      (GtkObject *self);

static void
hildon_date_editor_size_allocate                (GtkWidget *widget,
                                                 GtkAllocation *allocation);

static void
hildon_date_editor_size_request                 (GtkWidget *widget,
                                                 GtkRequisition *requisition);
static gboolean
hildon_date_editor_focus                        (GtkWidget *widget,
                                                 GtkDirectionType direction);
static gboolean
hildon_date_editor_entry_select_all             (GtkWidget *widget);

/* Property indices */
enum
{
    PROP_0, 
    PROP_DAY,
    PROP_MONTH,
    PROP_YEAR,
    PROP_MIN_YEAR,
    PROP_MAX_YEAR
};

enum 
{
    DATE_ERROR,
    LAST_SIGNAL
};

static guint                                    date_editor_signals[LAST_SIGNAL] = { 0 };

/**
 * hildon_date_editor_get_type:
 *
 * Initializes and returns the type of a hildon date editor.
 *
 * @Returns: GType of #HildonDateEditor
 */
GType G_GNUC_CONST
hildon_date_editor_get_type                     (void)
{
    static GType editor_type = 0;

    if (! editor_type) {
        static const GTypeInfo editor_info = {
            sizeof (HildonDateEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_date_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonDateEditor),
            0,          /* n_preallocs */
            (GInstanceInitFunc) hildon_date_editor_init,
        };
        editor_type = g_type_register_static (GTK_TYPE_CONTAINER,
                "HildonDateEditor",
                &editor_info, 0);
    }

    return editor_type;
}

static void
hildon_date_editor_class_init                   (HildonDateEditorClass *editor_class)
{
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS (editor_class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (editor_class);
    GObjectClass *gobject_class = G_OBJECT_CLASS (editor_class);

    parent_class = g_type_class_peek_parent (editor_class);

    g_type_class_add_private (editor_class, sizeof (HildonDateEditorPrivate));

    gobject_class->set_property             = hildon_date_editor_set_property;
    gobject_class->get_property             = hildon_date_editor_get_property;
    widget_class->size_request              = hildon_date_editor_size_request;
    widget_class->size_allocate             = hildon_date_editor_size_allocate;
    widget_class->focus                     = hildon_date_editor_focus;

    container_class->forall                 = hildon_child_forall;
    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_date_editor_destroy;

    editor_class->date_error                = (gpointer) hildon_date_editor_date_error; 

    date_editor_signals[DATE_ERROR] =
        g_signal_new ("date-error",
                G_OBJECT_CLASS_TYPE (gobject_class),
                G_SIGNAL_RUN_LAST,
                G_STRUCT_OFFSET (HildonDateEditorClass, date_error),
                g_signal_accumulator_true_handled, NULL,
                _hildon_marshal_BOOLEAN__ENUM,
                G_TYPE_BOOLEAN, 1, HILDON_TYPE_DATE_TIME_ERROR);

    /**
     * HildonDateEditor:year:
     *
     * Current year.
     */
    g_object_class_install_property (gobject_class, PROP_YEAR,
            g_param_spec_uint ("year",
                "Current year",
                "Current year",
                1, 10000,
                2007,
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonDateEditor:month:
     *
     * Current month.
     */
    g_object_class_install_property (gobject_class, PROP_MONTH,
            g_param_spec_uint ("month",
                "Current month",
                "Current month",
                1, 12,
                1,
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonDateEditor:day:
     *
     * Current day.
     */
    g_object_class_install_property (gobject_class, PROP_DAY,
            g_param_spec_uint ("day",
                "Current day",
                "Current day",
                1, 31,
                1,
                G_PARAM_READABLE | G_PARAM_WRITABLE));

    /**
     * HildonDateEditor:min-year:
     *
     * Minimum valid year.
     */
    g_object_class_install_property (gobject_class, PROP_MIN_YEAR,
            g_param_spec_uint ("min-year",
                "Minimum valid year",
                "Minimum valid year",
                1, 10000,
                DEFAULT_MIN_YEAR,
                G_PARAM_READWRITE));

    /**
     * HildonDateEditor:max-year:
     *
     * Maximum valid year.
     */
    g_object_class_install_property (gobject_class, PROP_MAX_YEAR,
            g_param_spec_uint ("max-year",
                "Maximum valid year",
                "Maximum valid year",
                1, 10000,
                DEFAULT_MAX_YEAR,
                G_PARAM_READWRITE));
}

/* Forces setting of the icon to certain state. Used initially
   and from the actual setter function */
static void
real_set_calendar_icon_state                    (HildonDateEditorPrivate *priv, 
                                                 gboolean pressed)
{
    g_assert (priv);

    gtk_image_set_from_icon_name (GTK_IMAGE (priv->calendar_icon),
            pressed ? "qgn_widg_datedit_pr" : "qgn_widg_datedit", HILDON_ICON_SIZE_SMALL);

    priv->calendar_icon_pressed = pressed;
}

/* Sets the icon to given state (normal/pressed). Returns
   info if the state actually changed. */
static gboolean 
hildon_date_editor_set_calendar_icon_state      (HildonDateEditor *editor,
                                                 gboolean pressed)
{
    HildonDateEditorPrivate *priv;

    g_assert (HILDON_IS_DATE_EDITOR (editor));

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    if (pressed != priv->calendar_icon_pressed) {
        real_set_calendar_icon_state (priv, pressed);
        return TRUE;
    }

    return FALSE;
}

/* Packing day, month and year entries depend on locale settings
   We find out the order and all separators by converting a known
   date to default format and inspecting the result string */
static void 
apply_locale_field_order                        (HildonDateEditorPrivate *priv)
{
    GDate locale_test_date;
    GtkWidget *delim;
    gchar buffer[256];
    gchar *iter, *delim_text;

    g_date_set_dmy (&locale_test_date, 1, 2, 1970);
    (void) g_date_strftime (buffer, sizeof (buffer), "%x", &locale_test_date);    
    iter = buffer;

    while (*iter)
    {
        gchar *endp;
        unsigned long value;

        /* Try to convert the current location into number. */
        value = strtoul (iter, &endp, 10);

        /* If the conversion didn't progress or the detected value was
           unknown (we used a fixed date, you remember), we treat 
           current position as a literal */
        switch (value)
        {
            case 1:
                gtk_box_pack_start (GTK_BOX (priv->d_box_date),
                        priv->d_entry, FALSE, FALSE, 0);
                break;

            case 2:
                gtk_box_pack_start (GTK_BOX (priv->d_box_date),
                        priv->m_entry, FALSE, FALSE, 0);
                break;

            case 70:   /* %x format uses only 2 numbers for some locales */
            case 1970:
                gtk_box_pack_start (GTK_BOX (priv->d_box_date),
                        priv->y_entry, FALSE, FALSE, 0);
                break;

            default:
                /* All non-number characters starting from current position 
                   form the delimeter */
                for (endp = iter; *endp; endp++)
                    if (g_ascii_isdigit (*endp))
                        break;

                /* Now endp points one place past the delimeter text */
                delim_text = g_strndup (iter, endp - iter);
                delim = gtk_label_new (delim_text);
                gtk_box_pack_start (GTK_BOX (priv->d_box_date),
                        delim, FALSE, FALSE, 0);

                priv->delims = g_list_append (priv->delims, delim);
                g_free(delim_text);

                break;
        };

        iter = endp;
    }
}

static void 
hildon_date_editor_init                         (HildonDateEditor *editor)
{
    HildonDateEditorPrivate *priv;
    GDate cur_date;

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    GTK_WIDGET_SET_FLAGS (GTK_WIDGET (editor), GTK_NO_WINDOW);

    gtk_widget_push_composite_child ();

    /* initialize values */
    g_date_clear (&cur_date, 1);
    g_date_set_time (&cur_date, time (NULL));

    priv->day = g_date_get_day (&cur_date);
    priv->month = g_date_get_month (&cur_date);
    priv->year = g_date_get_year (&cur_date);
    priv->min_year = DEFAULT_MIN_YEAR;
    priv->max_year = DEFAULT_MAX_YEAR;

    /* make widgets */
    priv->frame = gtk_frame_new (NULL);
    gtk_container_set_border_width (GTK_CONTAINER (priv->frame), 0);

    priv->d_entry = gtk_entry_new ();
    priv->m_entry = gtk_entry_new ();
    priv->y_entry = gtk_entry_new ();

#ifdef MAEMO_GTK
    g_object_set (G_OBJECT(priv->d_entry), "hildon-input-mode", 
            HILDON_GTK_INPUT_MODE_NUMERIC, NULL);
    g_object_set (G_OBJECT(priv->m_entry), "hildon-input-mode", 
            HILDON_GTK_INPUT_MODE_NUMERIC, NULL);
    g_object_set (G_OBJECT(priv->y_entry), "hildon-input-mode", 
            HILDON_GTK_INPUT_MODE_NUMERIC, NULL);
#endif

    /* set entry look */
    gtk_entry_set_width_chars (GTK_ENTRY (priv->d_entry), DAY_ENTRY_WIDTH);
    gtk_entry_set_width_chars (GTK_ENTRY (priv->m_entry), MONTH_ENTRY_WIDTH);
    gtk_entry_set_width_chars (GTK_ENTRY (priv->y_entry), YEAR_ENTRY_WIDTH);

    gtk_entry_set_max_length (GTK_ENTRY (priv->d_entry), DAY_ENTRY_WIDTH);
    gtk_entry_set_max_length (GTK_ENTRY (priv->m_entry), MONTH_ENTRY_WIDTH);
    gtk_entry_set_max_length (GTK_ENTRY (priv->y_entry), YEAR_ENTRY_WIDTH);

    gtk_entry_set_has_frame (GTK_ENTRY (priv->d_entry), FALSE);
    gtk_entry_set_has_frame (GTK_ENTRY (priv->m_entry), FALSE);
    gtk_entry_set_has_frame (GTK_ENTRY (priv->y_entry), FALSE);

    gtk_widget_set_composite_name (priv->d_entry, "day_entry");
    gtk_widget_set_composite_name (priv->m_entry, "month_entry");
    gtk_widget_set_composite_name (priv->y_entry, "year_entry");

    priv->d_box_date = gtk_hbox_new (FALSE, 0);

    priv->d_button_image = gtk_button_new ();
    priv->calendar_icon = gtk_image_new ();
    real_set_calendar_icon_state (priv, FALSE);
    GTK_WIDGET_UNSET_FLAGS (priv->d_button_image, GTK_CAN_FOCUS | GTK_CAN_DEFAULT);

    apply_locale_field_order (priv);

    gtk_container_add (GTK_CONTAINER (priv->frame), priv->d_box_date);
    gtk_container_add (GTK_CONTAINER (priv->d_button_image), priv->calendar_icon);
    gtk_button_set_relief (GTK_BUTTON (priv->d_button_image), GTK_RELIEF_NONE);
    gtk_button_set_focus_on_click (GTK_BUTTON (priv->d_button_image), FALSE);

    gtk_widget_set_parent (priv->frame, GTK_WIDGET (editor));
    gtk_widget_set_parent (priv->d_button_image, GTK_WIDGET (editor));
    gtk_widget_show_all (priv->frame);
    gtk_widget_show_all (priv->d_button_image);

    /* image button signal connects */
    g_signal_connect (GTK_OBJECT (priv->d_button_image), "pressed",
            G_CALLBACK (hildon_date_editor_icon_press), editor);
    g_signal_connect (GTK_OBJECT (priv->d_button_image), "released",
            G_CALLBACK (hildon_date_editor_released), editor);
    g_signal_connect (GTK_OBJECT (priv->d_button_image), "clicked",
            G_CALLBACK (hildon_date_editor_clicked), editor);
    g_signal_connect (GTK_OBJECT (priv->d_button_image), "key_press_event",
            G_CALLBACK (hildon_date_editor_keypress), editor);

    /* entry signal connects */    
    g_signal_connect (GTK_OBJECT (priv->d_entry), "focus-in-event",
            G_CALLBACK (hildon_date_editor_entry_focus_in), editor);

    g_signal_connect (GTK_OBJECT (priv->m_entry), "focus-in-event",
            G_CALLBACK (hildon_date_editor_entry_focus_in), editor);

    g_signal_connect (GTK_OBJECT (priv->y_entry), "focus-in-event",
            G_CALLBACK (hildon_date_editor_entry_focus_in), editor);

    g_signal_connect (GTK_OBJECT (priv->d_entry), "focus-out-event",
            G_CALLBACK (hildon_date_editor_entry_focus_out), editor);

    g_signal_connect (GTK_OBJECT (priv->m_entry), "focus-out-event",
            G_CALLBACK (hildon_date_editor_entry_focus_out), editor);

    g_signal_connect (GTK_OBJECT (priv->y_entry), "focus-out-event",
            G_CALLBACK (hildon_date_editor_entry_focus_out), editor);

    g_signal_connect (GTK_OBJECT (priv->d_entry), "key-press-event",
            G_CALLBACK (hildon_date_editor_keypress), editor);

    g_signal_connect (GTK_OBJECT (priv->m_entry), "key-press-event",
            G_CALLBACK (hildon_date_editor_keypress), editor);

    g_signal_connect (GTK_OBJECT (priv->y_entry), "key-press-event",
            G_CALLBACK (hildon_date_editor_keypress), editor);

    g_signal_connect (GTK_OBJECT (priv->d_entry), "key-release-event",
            G_CALLBACK (hildon_date_editor_keyrelease), editor);

    g_signal_connect(GTK_OBJECT(priv->m_entry), "key-release-event",
            G_CALLBACK(hildon_date_editor_keyrelease), editor);

    g_signal_connect (GTK_OBJECT (priv->y_entry), "key-release-event",
            G_CALLBACK (hildon_date_editor_keyrelease), editor);

    hildon_date_editor_set_date (editor, priv->year, priv->month, priv->day);

    g_signal_connect (GTK_OBJECT (priv->d_entry), "changed",
            G_CALLBACK (hildon_date_editor_entry_changed), editor);

    g_signal_connect (GTK_OBJECT (priv->m_entry), "changed",
            G_CALLBACK (hildon_date_editor_entry_changed), editor);

    g_signal_connect (GTK_OBJECT (priv->y_entry), "changed",
            G_CALLBACK (hildon_date_editor_entry_changed), editor);

    gtk_widget_pop_composite_child ();
}

static void 
hildon_date_editor_set_property                 (GObject *object, 
                                                 guint param_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonDateEditor *editor = HILDON_DATE_EDITOR (object);
    HildonDateEditorPrivate *priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    gint val;

    g_assert (priv);

    switch (param_id)
    {
        case PROP_YEAR:
            hildon_date_editor_set_year (editor, g_value_get_uint (value));
            break;

        case PROP_MONTH:
            hildon_date_editor_set_month (editor, g_value_get_uint (value));
            break;

        case PROP_DAY:
            hildon_date_editor_set_day (editor, g_value_get_uint (value));
            break;

        case PROP_MIN_YEAR:
            val = g_value_get_uint (value);
            priv->min_year = val;
            /* Clamp current year */
            if (hildon_date_editor_get_year (editor) < priv->min_year)
                hildon_date_editor_set_year (editor, priv->min_year);
            break;

        case PROP_MAX_YEAR:
            val = g_value_get_uint (value);
            priv->max_year = val;
            /* Clamp current year */
            if (hildon_date_editor_get_year (editor) > priv->max_year)
                hildon_date_editor_set_year (editor, priv->max_year);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

static void 
hildon_date_editor_get_property                 (GObject *object, 
                                                 guint param_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonDateEditor *editor = HILDON_DATE_EDITOR (object);
    HildonDateEditorPrivate *priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);

    switch (param_id)
    {
        case PROP_YEAR:
            g_value_set_uint (value, hildon_date_editor_get_year (editor));
            break;

        case PROP_MONTH:
            g_value_set_uint (value, hildon_date_editor_get_month (editor));
            break;

        case PROP_DAY:
            g_value_set_uint (value, hildon_date_editor_get_day (editor));
            break;

        case PROP_MIN_YEAR:
            g_value_set_uint (value, priv->min_year);
            break;

        case PROP_MAX_YEAR:
            g_value_set_uint (value, priv->max_year);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
            break;
    }
}

static void 
hildon_child_forall                             (GtkContainer *container,
                                                 gboolean include_internals,
                                                 GtkCallback callback,
                                                 gpointer callback_data)
{
    HildonDateEditor *editor;
    HildonDateEditorPrivate *priv;

    g_assert (HILDON_IS_DATE_EDITOR (container));
    g_assert (callback);

    editor = HILDON_DATE_EDITOR (container);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    if (include_internals) {
        (*callback) (priv->frame, callback_data);
        (*callback) (priv->d_button_image, callback_data);
    }
}

static void 
hildon_date_editor_destroy                      (GtkObject *self)
{
    HildonDateEditorPrivate *priv;

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (self);
    g_assert (priv);

    if (priv->frame) {
        gtk_widget_unparent (priv->frame);
        priv->frame = NULL;
    }
    if (priv->d_button_image) {
        gtk_widget_unparent (priv->d_button_image);
        priv->d_button_image = NULL;
    }
    if (priv->delims) {
        g_list_free (priv->delims);
        priv->delims = NULL;
    }

    if (GTK_OBJECT_CLASS (parent_class)->destroy)
        GTK_OBJECT_CLASS (parent_class)->destroy (self);
}

/**
 * hildon_date_editor_new:
 *
 * Creates a new date editor. The current system date
 * is shown in the editor.
 *
 * Returns: pointer to a new @HildonDateEditor widget.
 */
GtkWidget*
hildon_date_editor_new                          (void)
{
    return (GtkWidget *) g_object_new (HILDON_TYPE_DATE_EDITOR, NULL);
}

/**
 * hildon_date_editor_set_date:
 * @date: the @HildonDateEditor widget
 * @year: year
 * @month: month
 * @day: day
 *
 * Sets the date shown in the editor. 
 */
void 
hildon_date_editor_set_date                     (HildonDateEditor *editor,
                                                 guint year, 
                                                 guint month, 
                                                 guint day)
{
    HildonDateEditorPrivate *priv;

    g_return_if_fail (HILDON_IS_DATE_EDITOR (editor));

    /* This function cannot be implemented by calling
       component setters, since applying the individual
       values one by one can make the date temporarily
       invalid (depending on what the previous values were), 
       which in turn causes that the desired date
       is not set (even though it's valid). We must set all the
       components at one go and not try to do any validation etc
       there in between. */

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    if (g_date_valid_dmy (day, month, year))
    {
        GDate date;
        gchar buffer[256];

        priv->year = year;
        priv->month = month;
        priv->day = day;

        g_date_set_dmy (&date, day, month, year);

        /* We apply the new values, but do not want automatic focus move
           etc to take place */
        g_snprintf (buffer, sizeof (buffer), "%04d", year);
        g_signal_handlers_block_by_func (priv->y_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);
        gtk_entry_set_text (GTK_ENTRY (priv->y_entry), buffer);
        g_signal_handlers_unblock_by_func (priv->y_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);

        g_date_strftime (buffer, sizeof (buffer), "%m", &date);
        g_signal_handlers_block_by_func (priv->m_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);
        gtk_entry_set_text (GTK_ENTRY (priv->m_entry), buffer);
        g_signal_handlers_unblock_by_func (priv->m_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);

        g_date_strftime (buffer, sizeof (buffer), "%d", &date);
        g_signal_handlers_block_by_func (priv->d_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);
        gtk_entry_set_text (GTK_ENTRY (priv->d_entry), buffer);
        g_signal_handlers_unblock_by_func (priv->d_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);

        g_object_notify (G_OBJECT (editor), "year");
        g_object_notify (G_OBJECT (editor), "month");
        g_object_notify (G_OBJECT (editor), "day");
    }
}

/**
 * hildon_date_editor_get_date:
 * @date: the @HildonDateEditor widget
 * @year: year
 * @month: month
 * @day: day
 *
 * Gets the date represented by the date editor.
 * You can pass NULL to any of the pointers if
 * you're not interested in obtaining it. 
 *
 */
void 
hildon_date_editor_get_date                     (HildonDateEditor *date,
                                                 guint *year, 
                                                 guint *month, 
                                                 guint *day)
{
    HildonDateEditorPrivate *priv;

    g_return_if_fail (HILDON_IS_DATE_EDITOR (date));

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (date);

    /* FIXME: The role of priv->{day,month,year} members vs. entry contents
       is unclear. They do not neccesarily match and still the texts are
       used as return values and members for some internal validation!!
       At least a partly reason is to allow empty text to become
       0 return value, while members are restricted to valid ranges?!
       However, if we change the current way, we are likely to break 
       some applications  if they rely on some specific way how this 
       widget currently handles empty values and temporarily invalid values.

       The key issue is this: What should the _get methods return while
       user is editing a field and the result is incomplete. The
       partial result? The last good result? If we return partial result
       we also need a way to inform if the date is not valid. Current
       implementation is some kind of hybrid of these two...

       for example:
       hildon_date_editor_set_day(editor, hildon_date_editor_get_day(editor));

       easily fails, since set_day tries to force validity while get_day
       doesn't do that.

       Proposal: Always return the same values that are shown in the
       fields. We add a separate flag (Or use GDate) to 
       indicate if the current date is valid. This would allow 
       setters to make the date invalid as well. */

    if (year != NULL)
        *year = /*priv->year;*/
        (guint) atoi (gtk_entry_get_text (GTK_ENTRY (priv->y_entry)));
    if (month != NULL)
        *month = /*priv->month;*/
        (guint) atoi (gtk_entry_get_text (GTK_ENTRY (priv->m_entry)));
    if (day != NULL)
        *day = /*priv->day;*/ 
        (guint) atoi (gtk_entry_get_text (GTK_ENTRY (priv->d_entry)));
}

/* icon button press event */
static gboolean 
hildon_date_editor_icon_press                   (GtkWidget *widget,
                                                 gpointer data)
{
    g_assert (GTK_IS_WIDGET (widget));
    g_assert (HILDON_IS_DATE_EDITOR (data));

    hildon_date_editor_set_calendar_icon_state (HILDON_DATE_EDITOR (data), TRUE);

    return FALSE;
}

static gboolean 
hildon_date_editor_entry_focus_in               (GtkWidget *widget,
                                                 GdkEventFocus *event,
                                                 gpointer data)
{
    g_idle_add ((GSourceFunc) hildon_date_editor_entry_select_all, GTK_ENTRY (widget));

    return FALSE;
}


static void
popup_calendar_dialog                           (HildonDateEditor *ed)
{
    guint y = 0, m = 0, d = 0;
    GtkWidget *popup;
    GtkWidget *parent;
    guint result;
    GValue val = {0, };

    hildon_date_editor_get_date (ed, &y, &m, &d);

    parent = gtk_widget_get_ancestor (GTK_WIDGET (ed), GTK_TYPE_WINDOW);
    popup = hildon_calendar_popup_new (GTK_WINDOW (parent), y, m, d);

    g_value_init (&val, G_TYPE_INT);
    /* Set max/min year in calendar popup to date editor values */
    g_object_get_property (G_OBJECT (ed), "min-year", &val);
    g_object_set_property (G_OBJECT (popup), "min-year", &val);
    g_object_get_property (G_OBJECT (ed), "max-year", &val);
    g_object_set_property (G_OBJECT (popup), "max-year", &val);

    /* Pop up calendar */
    result = gtk_dialog_run (GTK_DIALOG (popup));
    switch (result) {
        case GTK_RESPONSE_OK:
        case GTK_RESPONSE_ACCEPT:
            hildon_calendar_popup_get_date (HILDON_CALENDAR_POPUP (popup), &y,
                    &m, &d);
            hildon_date_editor_set_date (ed, y, m, d);
    }

    gtk_widget_destroy (popup);
}

/* button released */
static gboolean 
hildon_date_editor_released                     (GtkWidget *widget,
                                                 gpointer data)
{
    HildonDateEditor *ed;

    g_assert (GTK_IS_WIDGET (widget));
    g_assert (HILDON_IS_DATE_EDITOR (data));

    ed = HILDON_DATE_EDITOR (data);

    /* restores the icon state. The clicked cycle raises the dialog */
    hildon_date_editor_set_calendar_icon_state (ed, FALSE);

    return FALSE;
}

/* button released */
static gboolean 
hildon_date_editor_clicked                      (GtkWidget *widget,
                                                 gpointer data)
{
    HildonDateEditor *ed;

    g_assert (GTK_IS_WIDGET (widget));
    g_assert (HILDON_IS_DATE_EDITOR (data));

    ed = HILDON_DATE_EDITOR (data);

    /* restores the non-clicked button state and raises the dialog */
    hildon_date_editor_set_calendar_icon_state (ed, FALSE);
    popup_calendar_dialog (ed);

    return FALSE;
}

/* This is called whenever some editor filed loses the focus and
   when the all of the fields are filled. 
   Earlier this was called whenever an entry changed */
/* FIXME: Validation on focus_out is broken by concept */
static gint
hildon_date_editor_entry_validate               (GtkWidget *widget, 
                                                 gpointer data)
{
    HildonDateEditor *ed;
    HildonDateEditorPrivate *priv;
    gint d, m, y, max_days;
    gboolean r;  /* temp return values for signals */
    const gchar *text;        
    gint error_code = HILDON_DATE_TIME_ERROR_NO_ERROR;

    g_assert (HILDON_IS_DATE_EDITOR (data));
    g_assert (GTK_IS_ENTRY (widget));

    ed = HILDON_DATE_EDITOR (data);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (ed);
    g_assert (priv);

    if (priv->skip_validation)
        return error_code;

    /*check if the calling entry is empty*/
    text = gtk_entry_get_text (GTK_ENTRY (widget));
    if(text == NULL || text[0] == 0)
    {
        if (widget == priv->d_entry)
            g_signal_emit (ed, date_editor_signals[DATE_ERROR], 0, HILDON_DATE_TIME_ERROR_EMPTY_DAY, &r);
        else if(widget == priv->m_entry)
            g_signal_emit (ed, date_editor_signals[DATE_ERROR], 0, HILDON_DATE_TIME_ERROR_EMPTY_MONTH, &r);
        else
            g_signal_emit (ed, date_editor_signals[DATE_ERROR], 0, HILDON_DATE_TIME_ERROR_EMPTY_YEAR, &r);

        /* restore empty entry to safe value */
        hildon_date_editor_set_date (ed, priv->year, priv->month, priv->day);
        return error_code;
    }

    /* Ok, we now check validity. Some fields can be empty */
    text = gtk_entry_get_text (GTK_ENTRY (priv->d_entry));
    if (text == NULL || text[0] == 0) return error_code;
    d = atoi (text);
    text = gtk_entry_get_text (GTK_ENTRY (priv->m_entry));
    if (text == NULL || text[0] == 0) return error_code;
    m = atoi (text);
    text = gtk_entry_get_text (GTK_ENTRY (priv->y_entry));
    if (text == NULL || text[0] == 0) return error_code;
    y = atoi (text);

    /* Did it actually change? */
    if (d != priv->day || m != priv->month || y != priv->year)
    {
        /* We could/should use hildon_date_editor_set_year and such functions
         * to set the date, instead of use gtk_entry_set_text, and then change
         * the priv member but hildon_date_editor_set_year and such functions
         * check if the date is valid, we do want to do date validation check
         * here according to spec */

        /* Validate month */
        if (widget == priv->m_entry) {
            if (m < 1) {
                error_code = HILDON_DATE_TIME_ERROR_MIN_MONTH;
                m = 1;
            }
            else if (m > 12) {
                error_code = HILDON_DATE_TIME_ERROR_MAX_MONTH;
                m = 12;
            }
        }

        /* Validate year */
        if(widget == priv->y_entry) {
            if (y < priv->min_year) {
                error_code = HILDON_DATE_TIME_ERROR_MIN_YEAR;
                y = priv->min_year;
            }
            else if (y > priv->max_year) {
                error_code =   HILDON_DATE_TIME_ERROR_MAX_YEAR;
                y = priv->max_year;
            }
        }

        /* Validate day. We have to do this in every case, since
           changing month or year can make the day number to be invalid */
        max_days = g_date_get_days_in_month (m,y);
        if(d < 1) {
            error_code = HILDON_DATE_TIME_ERROR_MIN_DAY;
            d = 1;
        }
        else if (d > max_days) {
            if (d > 31) {         
                error_code = HILDON_DATE_TIME_ERROR_MAX_DAY;
                d = max_days;
            }
            else {                /* the date does not exist (is invalid) */
                error_code = HILDON_DATE_TIME_ERROR_INVALID_DATE;
                /* check what was changed and restore previous value */
                if (widget == priv->y_entry)
                    y = priv->year;
                else if (widget == priv->m_entry)
                    m = priv->month;
                else
                    d = priv->day;
            }
        }

        if (error_code != HILDON_DATE_TIME_ERROR_NO_ERROR)
        {
            g_signal_emit (ed, date_editor_signals[DATE_ERROR], 0, error_code, &r);

            g_idle_add ((GSourceFunc) hildon_date_editor_entry_select_all, widget);
        }
    }

    /* Fix and reformat the date after error signal is processed. 
       reformatting can be needed even in a such case that numerical
       values of the date components are the same as earlier. */
    hildon_date_editor_set_date (ed, y, m, d);
    return error_code;
}

/* Idle callback */
static gboolean
hildon_date_editor_entry_select_all             (GtkWidget *widget)
{
    GDK_THREADS_ENTER ();

    gtk_editable_select_region (GTK_EDITABLE (widget), 0, -1);

    GDK_THREADS_LEAVE ();

    return FALSE;
}

/* When entry becomes full, we move the focus to the next field.
   If we are on the last field, the whole contents are validated. */
static void
hildon_date_editor_entry_changed                (GtkEditable *ed, 
                                                 gpointer data)
{
    GtkEntry *entry;
    gint error_code;
    
    HildonDateEditorPrivate *priv;
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (HILDON_DATE_EDITOR (data));

    g_assert (GTK_IS_ENTRY (ed));
    g_assert (HILDON_IS_DATE_EDITOR (data));
    g_assert (priv);

    entry = GTK_ENTRY (ed);

    /* If day entry is full, move to next entry or validate */
    if (g_utf8_strlen (gtk_entry_get_text (entry), -1) == gtk_entry_get_max_length (entry))
    {
        error_code = hildon_date_editor_entry_validate (GTK_WIDGET (entry), data);
        if (error_code == HILDON_DATE_TIME_ERROR_NO_ERROR)
        {
            priv->skip_validation = TRUE;
            gtk_widget_child_focus (GTK_WIDGET (data), GTK_DIR_RIGHT);
        }
    } else {
        priv->skip_validation = FALSE;
    }
}

static gboolean 
hildon_date_editor_keyrelease                   (GtkWidget *widget,
                                                 GdkEventKey *event,
                                                 gpointer data)
{
    HildonDateEditor *ed;
    HildonDateEditorPrivate *priv;

    g_return_val_if_fail (data, FALSE);
    g_return_val_if_fail (widget, FALSE);

    ed = HILDON_DATE_EDITOR (data);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (ed);

    if (event->keyval == GDK_KP_Enter || 
        event->keyval == GDK_Return ||
        event->keyval == GDK_ISO_Enter) {
        if (hildon_date_editor_set_calendar_icon_state (ed, FALSE))
        {
            popup_calendar_dialog (ed);
            return TRUE;
        }
    } else if (event->keyval == GDK_Escape)
        priv->skip_validation = FALSE;

    return FALSE;
}

/* keyboard handling */
static gboolean 
hildon_date_editor_keypress                     (GtkWidget *widget,
                                                 GdkEventKey *event,
                                                 gpointer data)
{
    HildonDateEditor *ed;
    HildonDateEditorPrivate *priv;

    g_assert (HILDON_IS_DATE_EDITOR (data));
    g_assert (GTK_IS_ENTRY (widget));

    ed = HILDON_DATE_EDITOR (data);

    switch (event->keyval) {
        case GDK_Return:
        case GDK_ISO_Enter:
            /* Ignore return value, since we want to handle event at all times.
               otherwise vkb would popup when the keyrepeat starts. */
            hildon_date_editor_set_calendar_icon_state (ed, TRUE);
            return TRUE;
        case GDK_Escape:
            priv = HILDON_DATE_EDITOR_GET_PRIVATE (ed);
            priv->skip_validation = TRUE;
            break;
        default:
            break;
    }

    return FALSE;
}

static gboolean 
hildon_date_editor_entry_focus_out              (GtkWidget *widget,
                                                 GdkEventFocus *event,
                                                 gpointer data)
{
    HildonDateEditor *ed;
    HildonDateEditorPrivate *priv;

    g_assert (HILDON_IS_DATE_EDITOR (data));

    ed = HILDON_DATE_EDITOR (data);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (ed);
    g_assert (priv);

    hildon_date_editor_entry_validate (widget, data);
    priv->skip_validation = FALSE;

    return FALSE;
}

static gboolean 
hildon_date_editor_date_error                   (HildonDateEditor *editor,
                                                 HildonDateTimeError type)
{
    HildonDateEditorPrivate *priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);
    
    switch (type)
    {
        case HILDON_DATE_TIME_ERROR_MAX_DAY:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_maximum_value"), 31);
            break;

        case HILDON_DATE_TIME_ERROR_MAX_MONTH:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_maximum_value"), 12);
            break;

        case HILDON_DATE_TIME_ERROR_MAX_YEAR:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_maximum_value"), priv->max_year);
            break;

        case HILDON_DATE_TIME_ERROR_MIN_DAY:
        case HILDON_DATE_TIME_ERROR_MIN_MONTH:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_minimum_value"), 1);
            break;

        case HILDON_DATE_TIME_ERROR_MIN_YEAR:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_minimum_value"), priv->min_year);
            break;

        case HILDON_DATE_TIME_ERROR_EMPTY_DAY:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_set_a_value_within_range"), 1, 31);
            break;

        case HILDON_DATE_TIME_ERROR_EMPTY_MONTH:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_set_a_value_within_range"), 1, 12);
            break;

        case HILDON_DATE_TIME_ERROR_EMPTY_YEAR:
            hildon_banner_show_informationf (GTK_WIDGET (editor), NULL, _("ckct_ib_set_a_value_within_range"),
                    priv->min_year, priv->max_year);
            break;

        case HILDON_DATE_TIME_ERROR_INVALID_CHAR:
            hildon_banner_show_information (GTK_WIDGET (editor), NULL, c_("ckct_ib_illegal_character"));
            break;

        case HILDON_DATE_TIME_ERROR_INVALID_DATE:
            hildon_banner_show_information (GTK_WIDGET (editor), NULL, _("ckct_ib_date_does_not_exist"));
            break;

        default:
            /*default error message ?*/
            break;
    }

    return TRUE;
}

static void 
hildon_date_editor_size_request                 (GtkWidget *widget,
                                                 GtkRequisition *requisition)
{
    HildonDateEditor *ed;
    HildonDateEditorPrivate *priv;
    GtkRequisition f_req, img_req;

    g_assert (GTK_IS_WIDGET (widget));
    g_assert (requisition != NULL);

    ed = HILDON_DATE_EDITOR (widget);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (ed);
    g_assert (priv);

    /* Our own children affect our size */
    gtk_widget_size_request (priv->frame, &f_req);
    gtk_widget_size_request (priv->d_button_image, &img_req);

    /* calculate our size */
    requisition->width = f_req.width + img_req.width + HILDON_MARGIN_DEFAULT;

    /* FIXME: Fixed size is bad! We should use the maximum of our children, but
       doing so would break current pixel specifications, since
       the text entry by itself is already 30px tall + then frame takes
       something */
    requisition->height = DATE_EDITOR_HEIGHT;
}

static void 
hildon_date_editor_size_allocate                (GtkWidget *widget,
                                                 GtkAllocation *allocation)
{
    HildonDateEditor *ed;
    HildonDateEditorPrivate *priv;
    GtkAllocation f_alloc, img_alloc;
    GtkRequisition req;
    GtkRequisition max_req;
    GList *iter;
    gboolean rtl;

    g_assert (GTK_IS_WIDGET (widget));
    g_assert (allocation != NULL);

    ed = HILDON_DATE_EDITOR (widget);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (ed);

    rtl = (gtk_widget_get_direction (widget) == GTK_TEXT_DIR_RTL);
    widget->allocation = *allocation;

    gtk_widget_get_child_requisition (widget, &max_req);

    /* Center vertically */
    f_alloc.y = img_alloc.y = allocation->y +
        MAX (allocation->height - max_req.height, 0) / 2;

    /* Center horizontally */
    f_alloc.x = img_alloc.x = allocation->x +
        MAX (allocation->width - max_req.width, 0) / 2;

    /* calculate allocations */
    if (GTK_WIDGET_VISIBLE (widget)) {
        /* allocate frame */
        gtk_widget_get_child_requisition (priv->frame, &req);

        f_alloc.width = req.width;
        f_alloc.height = max_req.height;

        /* allocate icon */
        gtk_widget_get_child_requisition (priv->d_button_image,
                &req);

        img_alloc.x += f_alloc.width + HILDON_MARGIN_DEFAULT;
        img_alloc.width = req.width;
        img_alloc.height = max_req.height;

        if (rtl)
        {
            img_alloc.x = f_alloc.x;
            f_alloc.x  += img_alloc.width + HILDON_MARGIN_DEFAULT;
        }

        if (GTK_WIDGET_VISIBLE (priv->d_button_image)) {
            gtk_widget_size_allocate (priv->d_button_image, &img_alloc);
        }

        if (GTK_WIDGET_VISIBLE (priv->frame)) {
            gtk_widget_size_allocate (priv->frame, &f_alloc);
        }
    }

    /* FIXME: We really should not alloc delimeters by hand (since they
       are not our own children, but we need to force to appear 
       higher. This ugly hack is needed to compensate the forced
       height in size_request. */
    for (iter = priv->delims; iter; iter = iter->next)
    {
        GtkWidget *delim;
        GtkAllocation alloc;

        delim = GTK_WIDGET (iter->data);
        alloc = delim->allocation;
        alloc.height = max_req.height; 
        alloc.y = priv->d_entry->allocation.y - 2;

        gtk_widget_size_allocate (delim, &alloc);
    }
}

static gboolean
hildon_date_editor_focus                      (GtkWidget *widget,
                                               GtkDirectionType direction)
{
  gboolean retval;
  GtkDirectionType effective_direction;

  g_assert (HILDON_IS_DATE_EDITOR (widget));

  retval = hildon_private_composite_focus (widget, direction, &effective_direction);

  if (retval == TRUE)
    return GTK_WIDGET_CLASS (parent_class)->focus (widget, effective_direction);
  else
    return FALSE;
}

/**
 * hildon_date_editor_set_year:
 * @editor: the @HildonDateEditor widget
 * @year: year
 *
 * Sets the year shown in the editor. 
 *
 * Returns: TRUE if the year is valid and has been set.
 */
gboolean 
hildon_date_editor_set_year                     (HildonDateEditor *editor, 
                                                 guint year)
{
    HildonDateEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_DATE_EDITOR (editor), FALSE);

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    if (g_date_valid_dmy (priv->day, priv->month, year))
    {
        gchar buffer[256];
        priv->year = year;

        g_snprintf (buffer, sizeof (buffer), "%04d", year);

        /* We apply the new day, but do not want automatic focus move
           etc to take place */
        g_signal_handlers_block_by_func (priv->y_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);
        gtk_entry_set_text (GTK_ENTRY (priv->y_entry), buffer);
        g_signal_handlers_unblock_by_func (priv->y_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);

        g_object_notify (G_OBJECT(editor), "year");
        return TRUE;
    }

    return FALSE;
}

/**
 * hildon_date_editor_set_month:
 * @editor: the @HildonDateEditor widget
 * @month: month
 *
 * Sets the month shown in the editor. 
 *
 * Returns: TRUE if the month is valid and has been set.
 */
gboolean 
hildon_date_editor_set_month                    (HildonDateEditor *editor, 
                                                 guint month)
{
    HildonDateEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_DATE_EDITOR (editor), FALSE);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    if (g_date_valid_dmy (priv->day, month, priv->year))
    {
        GDate date;
        gchar buffer[256];

        priv->month = month;
        g_date_set_dmy (&date, priv->day, month, priv->year);
        g_date_strftime (buffer, sizeof(buffer), "%m", &date);

        /* We apply the new day, but do not want automatic focus move
           etc to take place */
        g_signal_handlers_block_by_func (priv->m_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);
        gtk_entry_set_text (GTK_ENTRY (priv->m_entry), buffer);
        g_signal_handlers_unblock_by_func (priv->m_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);

        g_object_notify (G_OBJECT (editor), "month");
        return TRUE;
    }
    return FALSE;
}

/**
 * hildon_date_editor_set_day:
 * @editor: the @HildonDateEditor widget
 * @day: day
 *
 * Sets the day shown in the editor. 
 *
 * Returns: TRUE if the day is valid and has been set.
 */
gboolean 
hildon_date_editor_set_day                      (HildonDateEditor *editor, 
                                                 guint day)
{
    HildonDateEditorPrivate *priv;

    g_return_val_if_fail (HILDON_IS_DATE_EDITOR (editor), FALSE);
    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    if (g_date_valid_dmy (day, priv->month, priv->year))
    {
        GDate date;
        gchar buffer[256];

        priv->day = day;
        g_date_set_dmy (&date, day, priv->month, priv->year);
        g_date_strftime (buffer, sizeof (buffer), "%d", &date);

        /* We apply the new day, but do not want automatic focus move
           etc to take place */
        g_signal_handlers_block_by_func (priv->d_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);
        gtk_entry_set_text (GTK_ENTRY (priv->d_entry), buffer);
        g_signal_handlers_unblock_by_func (priv->d_entry, 
                (gpointer) hildon_date_editor_entry_changed, editor);

        g_object_notify (G_OBJECT(editor), "day");
        return TRUE;
    }
    return FALSE;
}

/**
 * hildon_date_editor_get_year:
 * @editor: the @HildonDateEditor widget
 *
 * Returns: the current year shown in the editor.
 */
guint
hildon_date_editor_get_year                     (HildonDateEditor *editor)
{
    HildonDateEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_DATE_EDITOR(editor), 0);

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    return (guint) atoi (gtk_entry_get_text (GTK_ENTRY (priv->y_entry)));
}

/**
 * hildon_date_editor_get_month:
 * @editor: the @HildonDateEditor widget
 *
 * Gets the month shown in the editor. 
 *
 * Returns: the current month shown in the editor.
 */
guint 
hildon_date_editor_get_month                    (HildonDateEditor *editor)
{
    HildonDateEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_DATE_EDITOR (editor), 0);

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    return (guint) atoi (gtk_entry_get_text (GTK_ENTRY (priv->m_entry)));
}

/**
 * hildon_date_editor_get_day:
 * @editor: the @HildonDateEditor widget
 *
 * Gets the day shown in the editor. 
 *
 * Returns: the current day shown in the editor
 */
guint 
hildon_date_editor_get_day                      (HildonDateEditor *editor)
{
    HildonDateEditorPrivate *priv;
    g_return_val_if_fail (HILDON_IS_DATE_EDITOR (editor), 0);

    priv = HILDON_DATE_EDITOR_GET_PRIVATE (editor);
    g_assert (priv);

    return (guint) atoi (gtk_entry_get_text (GTK_ENTRY (priv->d_entry)));
}

