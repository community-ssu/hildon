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
 * SECTION:hildon-note
 * @short_description: A widget to ask confirmation from the user.
 *
 * Notes are used to for confirmation (OK/Cancel/etc.) from the user.
 * A simple note contains an information text and an OK button to be
 * pressed.  Additional features such as progress bars or animation can
 * also be included. 
 * 
 * <example>
 * <title>HildonNote example</title>
 * <programlisting>
 * <!-- -->
 * hildon_note_new_confirmation (window, "Confirmation message...");
 * <!-- -->
 * i = gtk_dialog_run (GTK_DIALOG (note));
 * gtk_widget_destroy (GTK_WIDGET (note));
 * <!-- -->
 * if (i == GTK_RESPONSE_OK)
 *      gtk_infoprint (window, "User pressed 'OK' button'");
 * else
 *      gtk_infoprint (window, "User pressed 'Cancel' button" );
 * </programlisting>
 * </example>
 */

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-note.h"
#include                                        <gtk/gtklabel.h>
#include                                        <gtk/gtkimage.h>
#include                                        <gtk/gtkhbox.h>
#include                                        <gtk/gtkalignment.h>
#include                                        <gtk/gtkvbox.h>
#include                                        <gtk/gtkbutton.h>
#include                                        <libintl.h>
#include                                        "hildon-defines.h"
#include                                        "hildon-sound.h"
#include                                        "hildon-banner.h" 
#include                                        "hildon-enum-types.h"
#include                                        <stdio.h>
#include                                        <string.h>
#include                                        "hildon-note-private.h"

#define                                         CONFIRMATION_SOUND_PATH \
                                                "/usr/share/sounds/ui-confirmation_note.wav"

#define                                         INFORMATION_SOUND_PATH \
                                                "/usr/share/sounds/ui-information_note.wav"

#define                                         HILDON_NOTE_CONFIRMATION_ICON \
                                                "qgn_note_confirm"

#define                                         HILDON_NOTE_INFORMATION_ICON \
                                                "qgn_note_info"

#define                                         _(String) dgettext("hildon-libs", String)

static void 
hildon_note_class_init                          (HildonNoteClass *class);

static void
hildon_note_init                                (HildonNote *dialog);

static void 
hildon_note_rebuild                             (HildonNote *note);

static void
hildon_note_finalize                            (GObject *obj_self);

static void
hildon_note_realize                             (GtkWidget *widget);

static void 
hildon_note_set_property                        (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec);

static void
hildon_note_get_property                        (GObject *object,
                                                 guint prop_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec);

static gboolean
sound_handling                                  (GtkWidget *widget, 
                                                 GdkEventExpose *event, 
                                                 gpointer data);

enum 
{
    PROP_0,
    PROP_HILDON_NOTE_TYPE,
    PROP_HILDON_NOTE_DESCRIPTION,
    PROP_HILDON_NOTE_ICON,
    PROP_HILDON_NOTE_PROGRESSBAR,
    PROP_HILDON_NOTE_STOCK_ICON
};

static GtkDialogClass*                          parent_class;

static void
hildon_note_set_property                        (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value, 
                                                 GParamSpec * pspec)
{
    HildonNote *note = HILDON_NOTE (object);
    HildonNotePrivate *priv;
    GtkWidget *widget;

    priv = HILDON_NOTE_GET_PRIVATE (note);
    g_assert (priv);

    switch (prop_id) {

        case PROP_HILDON_NOTE_TYPE:
            priv->note_n = g_value_get_enum (value);
            hildon_note_rebuild (note);
            break;

        case PROP_HILDON_NOTE_DESCRIPTION:
            if (priv->original_description) 
                    g_free (priv->original_description);
            priv->original_description = g_value_dup_string (value);

            gtk_label_set_text (GTK_LABEL (priv->label), priv->original_description);
            /* FIXME Is the "original_description" used anywhere? */
            
            break;

        case PROP_HILDON_NOTE_ICON:
            gtk_image_set_from_icon_name (GTK_IMAGE (priv->icon), 
                    g_value_get_string(value), HILDON_ICON_SIZE_BIG_NOTE);
            break;

        case PROP_HILDON_NOTE_STOCK_ICON:
            gtk_image_set_from_stock (GTK_IMAGE (priv->icon), 
                    g_value_get_string (value), HILDON_ICON_SIZE_BIG_NOTE);
            break;

        case PROP_HILDON_NOTE_PROGRESSBAR:
            widget = g_value_get_object (value);
            if (widget != priv->progressbar)
            {
                if (priv->progressbar)
                    g_object_unref (priv->progressbar);

                priv->progressbar = widget;

                if (widget)
                {
                    g_object_ref (widget);
                    gtk_object_sink (GTK_OBJECT (widget));
                }

                hildon_note_rebuild (note);
            }
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
hildon_note_get_property                        (GObject *object,
                                                 guint prop_id, 
                                                 GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonNote *note = HILDON_NOTE (object);
    HildonNotePrivate *priv;

    priv = HILDON_NOTE_GET_PRIVATE (note);

    switch (prop_id) {

        case PROP_HILDON_NOTE_TYPE:
            g_value_set_enum (value, priv->note_n);
            break;

        case PROP_HILDON_NOTE_DESCRIPTION:
            g_value_set_string (value, priv->original_description);
            break;

        case PROP_HILDON_NOTE_ICON:
            g_object_get_property (G_OBJECT (priv->icon), "icon-name", value);
            break;

        case PROP_HILDON_NOTE_STOCK_ICON:
            g_object_get_property (G_OBJECT (priv->icon), "stock", value);
            break;

        case PROP_HILDON_NOTE_PROGRESSBAR:
            g_value_set_object (value, priv->progressbar);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

/**
 * hildon_note_get_type:
 *
 * Returns GType for HildonNote.
 *
 * Returns: HildonNote type
 */
GType G_GNUC_CONST
hildon_note_get_type                            (void)
{
    static GType dialog_type = 0;

    if (! dialog_type) {
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
        dialog_type = g_type_register_static (GTK_TYPE_DIALOG,
                "HildonNote",
                &dialog_info, 0);
    }
    return dialog_type;
}

static void 
hildon_note_class_init                          (HildonNoteClass *class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (class);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (class);

    /* set the global parent_class */
    parent_class = g_type_class_peek_parent (class);

    g_type_class_add_private (class, sizeof (HildonNotePrivate));

    object_class->finalize      = hildon_note_finalize;
    object_class->set_property  = hildon_note_set_property;
    object_class->get_property  = hildon_note_get_property;
    widget_class->realize       = hildon_note_realize;

    g_object_class_install_property (object_class,
            PROP_HILDON_NOTE_TYPE,
            g_param_spec_enum ("note-type",
                "note type",
                "The type of the note dialog",
                hildon_note_type_get_type (),
                HILDON_NOTE_TYPE_CONFIRMATION,
                G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

    /**
     * HildonNote:description:
     *
     * Description for the note.
     */
    g_object_class_install_property (object_class,
            PROP_HILDON_NOTE_DESCRIPTION,
            g_param_spec_string ("description",
                "note description",
                "The text that appears in the note dialog",
                "",
                G_PARAM_READWRITE));

    /**
     * HildonNote:icon:
     *
     * Icon for the note.
     */
    g_object_class_install_property (object_class,
            PROP_HILDON_NOTE_ICON,
            g_param_spec_string ("icon",
                "note icon",
                "The name of the icon that appears in the note dialog",
                "",
                G_PARAM_READWRITE));

    /**
     * HildonNote:stock-icon:
     *
     * Stock icon name for the note.
     */
    g_object_class_install_property (object_class,
            PROP_HILDON_NOTE_STOCK_ICON,
            g_param_spec_string ("stock-icon",
                "Stock note icon",
                "The stock name of the icon that appears in the note dialog",
                "",
                G_PARAM_READWRITE));

    /**
     * HildonNote:progressbar:
     *
     * Progressbar for the note (if any).
     */
    g_object_class_install_property (object_class,
            PROP_HILDON_NOTE_PROGRESSBAR,
            g_param_spec_object ("progressbar",
                "Progressbar widget",
                "The progressbar that appears in the note dialog",
                GTK_TYPE_PROGRESS_BAR,
                G_PARAM_READWRITE));
}

static void 
hildon_note_init                                (HildonNote *dialog)
{
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE (dialog);
    g_assert (priv);

    priv->label = gtk_label_new (NULL);
    gtk_label_set_line_wrap (GTK_LABEL (priv->label), TRUE);
    
    priv->icon  = gtk_image_new ();

    /* Acquire real references to our internal children, since
       they are not nessecarily packed into container in each
       layout */
    g_object_ref (priv->label);
    g_object_ref (priv->icon);

    gtk_object_sink (GTK_OBJECT (priv->label));
    gtk_object_sink (GTK_OBJECT (priv->icon));

    gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
}


static void 
hildon_note_finalize                            (GObject *obj_self)
{
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE (obj_self);
    g_assert (priv);

    /* FIXME Some of this stuff should be moved to dispose */

    /* Free internal data */
    if (priv->label)
        g_object_unref (priv->label);

    if (priv->icon)
        g_object_unref (priv->icon);

    if (priv->progressbar)
        g_object_unref (priv->progressbar);

    if (priv->original_description)
        g_free (priv->original_description);

    G_OBJECT_CLASS (parent_class)->finalize (obj_self);
}

static void
hildon_note_realize                             (GtkWidget *widget)
{
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE (widget);
    g_assert (priv);

    /* Make widget->window accessible */
    GTK_WIDGET_CLASS (parent_class)->realize (widget);

    /* Border only, no titlebar */
    gdk_window_set_decorations (widget->window, GDK_DECOR_BORDER);

    /* Because ESD is synchronous, we wish to play sound after the
       note is already on screen to avoid blocking its appearance */
    if (priv->sound_signal_handler == 0)
        priv->sound_signal_handler = g_signal_connect_after(widget, 
                "expose-event", G_CALLBACK (sound_handling), NULL);
}

/* Helper function for removing a widget from it's container.
   we own a separate reference to each object we try to unpack,
   so extra referencing is not needed. */
static void 
unpack_widget                                   (GtkWidget *widget)
{
    g_assert (widget == NULL || GTK_IS_WIDGET (widget));

    if (widget && widget->parent)
        gtk_container_remove (GTK_CONTAINER (widget->parent), widget);
}

static void
hildon_note_rebuild                             (HildonNote *note)
{
    GtkDialog *dialog;
    HildonNotePrivate *priv;
    gboolean IsHorizontal = TRUE;

    g_assert (HILDON_IS_NOTE (note));

    priv = HILDON_NOTE_GET_PRIVATE (note);
    g_assert (priv);

    dialog = GTK_DIALOG (note);

    /* Reuse exiting content widgets for new layout */
    unpack_widget (priv->label);
    unpack_widget (priv->icon);
    unpack_widget (priv->progressbar);

    /* Destroy old layout and buttons */
    if (priv->box) {
        gtk_widget_destroy (priv->box);
        priv->box = NULL;
    }
    if (priv->okButton) {
        gtk_widget_destroy (priv->okButton);
        priv->okButton = NULL;
    }
    if (priv->cancelButton) {
        gtk_widget_destroy (priv->cancelButton);
        priv->cancelButton = NULL;
    }

    /* Add needed buttons and images for each note type */
    switch (priv->note_n)
    {
        case HILDON_NOTE_TYPE_CONFIRMATION:
            priv->okButton = gtk_dialog_add_button (dialog,
                    _("ecdg_bd_confirmation_note_ok"), GTK_RESPONSE_OK);
            priv->cancelButton = gtk_dialog_add_button (dialog,
                    _("ecdg_bd_confirmation_note_cancel"), GTK_RESPONSE_CANCEL);

            /* Fall through */
        case HILDON_NOTE_TYPE_CONFIRMATION_BUTTON:
            gtk_image_set_from_icon_name (GTK_IMAGE (priv->icon),
                    HILDON_NOTE_CONFIRMATION_ICON, 
                    HILDON_ICON_SIZE_BIG_NOTE);
            break;

        case HILDON_NOTE_TYPE_INFORMATION_THEME:
        case HILDON_NOTE_TYPE_INFORMATION:
            /* Add clickable OK button (cancel really,
               but doesn't matter since this is info) */
            priv->cancelButton = gtk_dialog_add_button (dialog,
                    _("ecdg_bd_information_note_ok"), GTK_RESPONSE_CANCEL);
            gtk_image_set_from_icon_name (GTK_IMAGE (priv->icon),
                    HILDON_NOTE_INFORMATION_ICON,
                    HILDON_ICON_SIZE_BIG_NOTE);
            break;

        case HILDON_NOTE_TYPE_PROGRESSBAR:
            priv->cancelButton = gtk_dialog_add_button (dialog,
                    _("ecdg_bd_cancel_note_cancel"), GTK_RESPONSE_CANCEL);
            IsHorizontal = FALSE;
            break;

        default:
            break;
    }

    if (IsHorizontal) {
        /* Pack item with label horizontally */
        priv->box = gtk_hbox_new (FALSE, HILDON_MARGIN_DEFAULT);
        gtk_container_add (GTK_CONTAINER (dialog->vbox), priv->box);

        if (priv->icon) {
            GtkWidget *alignment = gtk_alignment_new (0, 0, 0, 0);

            gtk_box_pack_start (GTK_BOX (priv->box), alignment, FALSE, FALSE, 0);
            gtk_container_add (GTK_CONTAINER (alignment), priv->icon);
        }
        gtk_box_pack_start (GTK_BOX (priv->box), priv->label, TRUE, TRUE, 0);

    } else {
        /* Pack item with label vertically */
        priv->box = gtk_vbox_new (FALSE, HILDON_MARGIN_DOUBLE);
        gtk_container_add (GTK_CONTAINER (dialog->vbox), priv->box);
        gtk_box_pack_start (GTK_BOX (priv->box), priv->label, TRUE, TRUE, 0);

        if (priv->progressbar)
            gtk_box_pack_start (GTK_BOX (priv->box), priv->progressbar, FALSE, FALSE, 0);
    }

    gtk_widget_show_all (priv->box);
}

/**
 * hildon_note_new_confirmation_add_buttons:
 * @parent: the parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly.
 *   In GTK the X window ID can be checked using
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: the message to confirm
 * @Varargs: arguments pairs for new buttons(label and return value). 
 *   Terminate the list with %NULL value.
 * 
 * Create a new confirmation note with custom buttons. Confirmation
 * note has a text and any number of buttons. It's important to note
 * that even though the name of the function might suggest, the
 * default ok/cancel buttons are not appended but you have to provide
 * all of the buttons.
 *
 * FIXME: This doc seems to be wrong, the two buttons aren't added so
 * it would only contain the "additional" buttons? However, changing
 * this would break those applications that rely on current behaviour.
 *
 * Returns: A #GtkWidget pointer of the note
 */
GtkWidget*
hildon_note_new_confirmation_add_buttons        (GtkWindow *parent,
                                                 const gchar *description,
                                                 ...)
{
    va_list args;
    char *message;
    int value;

    g_return_val_if_fail (description != NULL, NULL);

    GtkWidget *conf_note =
        g_object_new (HILDON_TYPE_NOTE,
                "note-type", HILDON_NOTE_TYPE_CONFIRMATION_BUTTON,
                "description", description,
                "icon", HILDON_NOTE_CONFIRMATION_ICON, 
                NULL);

    if (parent != NULL)
        gtk_window_set_transient_for (GTK_WINDOW (conf_note), parent);

    /* Add the buttons from varargs */
    va_start(args, description);

    while (TRUE) {
        message = va_arg (args, char *);

        if (! message) {
            break;
        }
        value = va_arg (args, int);

        gtk_dialog_add_button (GTK_DIALOG (conf_note), message, value);
    }

    va_end (args);

    return conf_note;
}


/**
 * hildon_note_new_confirmation:
 * @parent: the parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked using
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: the message to confirm
 * 
 * Create a new confirmation note. Confirmation note has text (description)
 * that you specify, two buttons and a default confirmation stock icon.
 *
 * Returns: a #GtkWidget pointer of the note
 */
GtkWidget*
hildon_note_new_confirmation                    (GtkWindow *parent,
                                                 const gchar *description)
{
    return hildon_note_new_confirmation_with_icon_name
        (parent, description, HILDON_NOTE_CONFIRMATION_ICON);
}

/**
 * hildon_note_new_confirmation_with_icon_name:
 * @parent: the parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked using
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: the message to confirm
 * @icon_name: icon to be displayed. If NULL, default icon is used.
 * 
 * Create a new confirmation note. Confirmation note has text(description) 
 * that you specify, two buttons and an icon.
 *
 * Returns: a #GtkWidget pointer of the note
 */
GtkWidget*
hildon_note_new_confirmation_with_icon_name     (GtkWindow *parent,
                                                 const gchar *description,
                                                 const gchar *icon_name)
{
    GtkWidget *dialog = NULL;

    g_return_val_if_fail (description != NULL, NULL);

    dialog = g_object_new (HILDON_TYPE_NOTE,
            "note-type",
            HILDON_NOTE_TYPE_CONFIRMATION,
            "description", description, "icon",
            icon_name, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

    return dialog;
}

/**
 * hildon_note_new_information:
 * @parent: the parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked using
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: the message to confirm
 * 
 * Create a new information note. Information note has a text(description) 
 * that you specify, an OK button and an icon.
 * 
 * Returns: a #GtkWidget pointer of the note
 */
GtkWidget*
hildon_note_new_information                     (GtkWindow *parent,
                                                 const gchar *description)
{
    return hildon_note_new_information_with_icon_name
        (parent, description, HILDON_NOTE_INFORMATION_ICON);
}

/**
 * hildon_note_new_information_with_icon_name:
 * @parent: the parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked using
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: the message to confirm
 * @icon_name: icon to be displayed. If NULL, default icon is used.
 * 
 * Create a new information note. Information note has text(description) 
 * that you specify, an OK button and an icon.
 * 
 * Returns: a #GtkWidget pointer of the note
 */
GtkWidget*
hildon_note_new_information_with_icon_name      (GtkWindow * parent,
                                                 const gchar *description,
                                                 const gchar *icon_name)
{
    GtkWidget *dialog = NULL;

    g_return_val_if_fail (description != NULL, NULL);
    g_return_val_if_fail (icon_name != NULL, NULL);

    dialog = g_object_new (HILDON_TYPE_NOTE,
            "note-type",
            HILDON_NOTE_TYPE_INFORMATION_THEME,
            "description", description,
            "icon", icon_name, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

    return dialog;
}

/* FIXME This documentation string LIES! */

/**
 * hildon_note_new_cancel_with_progress_bar:
 * @parent: the parent window. The X window ID of the parent window
 *   has to be the same as the X window ID of the application. This is
 *   important so that the window manager could handle the windows
 *   correctly. In GTK the X window ID can be checked using
 *   GDK_WINDOW_XID(GTK_WIDGET(parent)->window).
 * @description: the action to cancel
 * @progressbar: a pointer to #GtkProgressBar to be filled with the
 *   progressbar assigned to this note. Use this to set the fraction of
 *   progressbar done. This parameter can be %NULL as well, in which
 *   case plain text cancel note appears.
 *
 * Create a new cancel note with a progress bar. Cancel note has 
 * text(description) that you specify, a Cancel button and a progress bar.
 *
 * Returns: a #GtkDialog. Use this to get rid of this note when you
 *   no longer need it.
 */
GtkWidget*
hildon_note_new_cancel_with_progress_bar        (GtkWindow *parent,
                                                 const gchar *description,
                                                 GtkProgressBar *progressbar)
{
    GtkWidget *dialog = NULL;

    g_return_val_if_fail (description != NULL, NULL);

    dialog = g_object_new (HILDON_TYPE_NOTE,
            "note-type",
            HILDON_NOTE_TYPE_PROGRESSBAR,
            "description", description,
            "progressbar",
            progressbar, NULL);

    if (parent != NULL)
        gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);

    return dialog;
}


/**
 * hildon_note_set_button_text:
 * @note: a #HildonNote
 * @text: sets the button text and if there is two buttons in dialog, 
 *   the button texts will be &lt;text&gt;, "Cancel".  
 *
 * Sets the button text to be used by the hildon_note widget.
 */
void 
hildon_note_set_button_text                     (HildonNote *note, 
                                                 const gchar *text)
{
    HildonNotePrivate *priv;

    g_return_if_fail (HILDON_IS_NOTE (note));

    priv = HILDON_NOTE_GET_PRIVATE (HILDON_NOTE (note));
    g_assert (priv);

    if (priv->okButton) {
        gtk_button_set_label (GTK_BUTTON (priv->okButton), text);
        gtk_button_set_label (GTK_BUTTON (priv->cancelButton),
                _("ecdg_bd_confirmation_note_cancel"));
    } else {
        gtk_button_set_label (GTK_BUTTON (priv->cancelButton), text);
    }
}

/**
 * hildon_note_set_button_texts:
 * @note: a #HildonNote
 * @text_ok: the new text of the default OK button
 * @text_cancel: the new text of the default cancel button 
 *
 * Sets the button texts to be used by this hildon_note widget.
 */
void 
hildon_note_set_button_texts                    (HildonNote *note,
                                                 const gchar *text_ok,
                                                 const gchar *text_cancel)
{
    HildonNotePrivate *priv;

    g_return_if_fail (HILDON_IS_NOTE (note));

    priv = HILDON_NOTE_GET_PRIVATE (HILDON_NOTE (note));
    g_assert (priv);

    if (priv->okButton) {
        gtk_button_set_label (GTK_BUTTON (priv->okButton), text_ok);
        gtk_button_set_label (GTK_BUTTON (priv->cancelButton), text_cancel);
    } else {
        gtk_button_set_label (GTK_BUTTON (priv->cancelButton), text_cancel);
    }
}

/* We play a system sound when the note comes visible */
static gboolean
sound_handling                                  (GtkWidget *widget, 
                                                 GdkEventExpose *event, 
                                                 gpointer data)
{
    HildonNotePrivate *priv = HILDON_NOTE_GET_PRIVATE (widget);
    g_assert (priv);

    g_signal_handler_disconnect (widget, priv->sound_signal_handler);

    priv->sound_signal_handler = 0;

    switch (priv->note_n)
    {
        case HILDON_NOTE_TYPE_INFORMATION:
        case HILDON_NOTE_TYPE_INFORMATION_THEME:
            hildon_play_system_sound (INFORMATION_SOUND_PATH);
            break;

        case HILDON_NOTE_TYPE_CONFIRMATION:
        case HILDON_NOTE_TYPE_CONFIRMATION_BUTTON:
            hildon_play_system_sound (CONFIRMATION_SOUND_PATH);
            break;

        default:
            break;
    };

    return FALSE;
}
