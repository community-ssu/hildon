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
 * @shortdesc: FindReplaceDialog is a dialog for setting options for
 * search/replace functionality in an application.
 * @longdesc: FindReplaceDialog is a dialog for setting options for
 * search/replace functionality in an application.
 * The dialog has three modes depending on what kind of data the search is
 * being made to (normal, read-only, immutable).
 * Also a options dialog for different search options can be added (will be
 * implemented later).
 */

#include <glib.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#include <hildon-find-replace-dialog.h>
#include <hildon-search.h>
#include <hildon-widgets/hildon-caption.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass *parent_class;
static gboolean show_replace_entry = TRUE;

typedef struct _HildonFindReplaceDialogPrivate
    HildonFindReplaceDialogPrivate;
struct _HildonFindReplaceDialogPrivate {
    GtkButton *findButton;
    GtkButton *closeButton;
    GtkButton *replaceButton;
    GtkButton *optionsButton;

    HildonCaption *findEntry;
    HildonCaption *replaceEntry;

    HildonReplaceType mode;

    GtkDialog *optionsDialog;
};

#define HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(o)\
        (G_TYPE_INSTANCE_GET_PRIVATE ((o),\
         HILDON_TYPE_FIND_REPLACE_DIALOG,\
         HildonFindReplaceDialogPrivate))

static void
hildon_find_replace_dialog_class_init(HildonFindReplaceDialogClass *class);
static void hildon_find_replace_dialog_init(HildonFindReplaceDialog *widget);
static void toggle_replace(GtkButton *button, gpointer data);
static void
open_options_dialog(GtkWidget *widget, HildonFindReplaceDialog *dialog);
static void hildon_find_replace_dialog_response_cb(GtkDialog *dialog,
                                                   gint response,
                                                   gpointer data);
static void hildon_find_replace_options_dialog_response_cb(GtkDialog *dialog,
                                                           gint response,
                                                           gpointer data);

static void hildon_find_replace_dialog_response_cb(GtkDialog *dialog,
                                                   gint response,
                                                   gpointer data)
{
    g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
    if (response == GTK_RESPONSE_OK)
        g_print("user clicked ok in find/replace dialog\n");

    gtk_widget_hide(GTK_WIDGET(dialog));
}
static void hildon_find_replace_options_dialog_response_cb(GtkDialog *dialog,
                                                           gint response,
                                                           gpointer data)
{
    g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
    if (response == GTK_RESPONSE_OK)
        g_print("user clicked ok in find/replace-options dialog\n");

    gtk_widget_hide(GTK_WIDGET(dialog));
    gtk_window_present(GTK_WINDOW(data));
}

static void
open_options_dialog(GtkWidget *widget, HildonFindReplaceDialog *dialog)
{
    HildonFindReplaceDialogPrivate *priv;

    priv = HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(dialog);

    g_return_if_fail(GTK_IS_DIALOG(priv->optionsDialog));
    g_signal_connect(G_OBJECT(priv->optionsDialog), "response",
                     G_CALLBACK
                     (hildon_find_replace_options_dialog_response_cb),
                     dialog);
    gtk_dialog_run(priv->optionsDialog);
}

static void toggle_replace(GtkButton *button, gpointer self)
{
    HildonFindReplaceDialog *dialog;
    HildonFindReplaceDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_FIND_REPLACE_DIALOG(self));

    dialog = HILDON_FIND_REPLACE_DIALOG(self);
    priv = HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(dialog);

    if (show_replace_entry)
    {
        g_object_ref(priv->replaceEntry);
        gtk_widget_hide(GTK_WIDGET(priv->replaceEntry));
        show_replace_entry = FALSE;
    }
    else
    {
        gtk_widget_show(GTK_WIDGET(priv->replaceEntry));
        show_replace_entry = TRUE;
    }

}

static void
hildon_find_replace_dialog_class_init(HildonFindReplaceDialogClass *class)
{
    parent_class = g_type_class_peek_parent(class);
    g_type_class_add_private(class,
                             sizeof(HildonFindReplaceDialogPrivate));
}

static void
hildon_find_replace_dialog_init(HildonFindReplaceDialog *dialog)
{
    HildonFindReplaceDialogPrivate *priv =
        HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(dialog);
    GtkSizeGroup *group =
        GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));
    GtkWidget *control;

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_title(GTK_WINDOW(dialog), _("Find and Replace"));

    priv->findButton = GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                                        _("Find"),
                                                        GTK_RESPONSE_OK));

    priv->replaceButton =
        GTK_BUTTON(
    gtk_dialog_add_button(GTK_DIALOG(dialog),_("Replace"), GTK_RESPONSE_NONE));
    g_signal_connect(priv->replaceButton, "clicked",
                     G_CALLBACK(toggle_replace), dialog);

    priv->optionsButton =
        GTK_BUTTON(
    gtk_dialog_add_button(GTK_DIALOG(dialog),
                       _("Options"), GTK_RESPONSE_NONE));

    priv->closeButton =
        GTK_BUTTON(gtk_dialog_add_button
                   (GTK_DIALOG(dialog), _("Close"), GTK_RESPONSE_CANCEL));

    control = gtk_entry_new();
    priv->findEntry = HILDON_CAPTION(hildon_caption_new(GTK_SIZE_GROUP(group),
                                     _("Find"), GTK_WIDGET(control), NULL,
                                     HILDON_CAPTION_OPTIONAL));

    control = gtk_entry_new();
    priv->replaceEntry = HILDON_CAPTION(hildon_caption_new(group,
                                        _("Replace with"), control, NULL,
                                        HILDON_CAPTION_OPTIONAL));

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->findEntry), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->replaceEntry), FALSE, FALSE, 0);

    priv->optionsDialog = GTK_DIALOG(dialog);

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);

    gtk_widget_hide(GTK_WIDGET(priv->replaceEntry));
    show_replace_entry = FALSE;
}

GType hildon_find_replace_dialog_get_type(void)
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonFindReplaceDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_find_replace_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonFindReplaceDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_find_replace_dialog_init
        };
        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonFindReplaceDialog",
                                             &dialog_info, 0);
    }

    return dialog_type;
}

/**
 * hildon_find_replace_dialog_new:
 * @parent: the parent window of the dialog.
 * @type: type (HildonReplaceType) of replace dialog, can be one of: 
 * HILDON_REPLACE_IMMUTABLE, HILDON_REPLACE_READ_ONLY,
 * HILDON_REPLACE_NORMAL.
 * 
 * Creates a new #HildonFindReplaceDialog widget with Find and Close
 * buttons. This is the only thing that the dialog will do (plus a find
 * entry) if the mode is
 * HILDON_REPLACE_IMMUTABLE. Otherwise, if the parameter type
 * (HildonReplaceType) is 
 * HILDON_REPLACE_NORMAL or HILDON_REPLACE_READ_ONLY then a Replace button
 * fill be added. 
 * Pressing replace button will add a 'Replace with' field into the dialog.
 * If dialog is 
 * of type HILDON_REPLACE_READ_ONLY then the replace button will be
 * insensitive. 
 *
 * Return value: the newly created #HildonFindReplaceDialog
 */
GtkWidget *hildon_find_replace_dialog_new(GtkWindow *parent,
                                          HildonReplaceType type)
{
    HildonFindReplaceDialog *self =
        HILDON_FIND_REPLACE_DIALOG(g_object_new(
                                   HILDON_TYPE_FIND_REPLACE_DIALOG, NULL));

    HildonFindReplaceDialogPrivate *priv =
        HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(self);

    priv->mode = type;
    g_signal_connect(G_OBJECT(self), "response",
                     G_CALLBACK(hildon_find_replace_dialog_response_cb),
                     NULL);

    gtk_widget_hide(GTK_WIDGET(priv->optionsButton));
    if (priv->mode == HILDON_REPLACE_IMMUTABLE)
        gtk_widget_hide(GTK_WIDGET(priv->replaceButton));
    else if (priv->mode == HILDON_REPLACE_READ_ONLY)
        gtk_widget_set_sensitive(GTK_WIDGET(priv->replaceButton), FALSE);

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(self), parent);

    return GTK_WIDGET(self);
}

/**
 * hildon_find_replace_dialog_new_with_options:
 * @parent: the parent window of the dialog.
 * @dialog: the options dialog.
 * @type: HildonReplaceType for mode.
 *
 * Creates a new #HildonFindReplaceDialog widget with Find,
 * Options and Close buttons.
 * This is the only thing that the dialog will do (plus a find entry)
 * if the mode is
 * HILDON_REPLACE_IMMUTABLE. Otherwise, if the parameter type
 * (HildonReplaceType) is 
 * HILDON_REPLACE_NORMAL or HILDON_REPLACE_READ_ONLY then a Replace button
 * fill be added. 
 * Pressing replace button will add a 'Replace with' field into the dialog.
 * If dialog is 
 * of type HILDON_REPLACE_READ_ONLY then the replace button will be
 * insensitive. 
 *
 * Options Dialog is a feature that will be implemented later. 
 * 
 * Return value: the newly created #HildonFindReplaceDialog
 */
GtkWidget *hildon_find_replace_dialog_new_with_options(GtkWindow *parent,
                                                       GtkDialog *dialog,
                                                       HildonReplaceType type)
{
    HildonFindReplaceDialog *self = HILDON_FIND_REPLACE_DIALOG(g_object_new(
                                    HILDON_TYPE_FIND_REPLACE_DIALOG, NULL));
    HildonFindReplaceDialogPrivate *priv =
        HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(self);

    g_signal_connect(G_OBJECT(self), "response",
                     G_CALLBACK(hildon_find_replace_dialog_response_cb), NULL);

    priv->optionsDialog = GTK_DIALOG(dialog);
    priv->mode = type;

    if (priv->optionsDialog != NULL)
        g_signal_connect(G_OBJECT(priv->optionsButton), "clicked",
                         G_CALLBACK(open_options_dialog), self);
    else
        gtk_widget_hide(GTK_WIDGET(priv->optionsButton));

    if (priv->mode == HILDON_REPLACE_IMMUTABLE)
    {
        gtk_widget_hide(GTK_WIDGET(priv->replaceEntry));
        gtk_widget_hide(GTK_WIDGET(priv->replaceButton));
    }
    else if (priv->mode == HILDON_REPLACE_READ_ONLY)
    {
        gtk_widget_hide(GTK_WIDGET(priv->replaceEntry));
        gtk_widget_set_sensitive(GTK_WIDGET(priv->replaceButton), FALSE);
    }

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(self), parent);

    if (dialog)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(self));

    return GTK_WIDGET(self);
}

/**
 * hildon_find_replace_dialog_get_text:
 * @dialog: the dialog.
 *
 * Gets the text that's in the find entry.
 *
 * Return value: a pointer to the find string.
 */
const gchar *
hildon_find_replace_dialog_get_text(HildonFindReplaceDialog *dialog)
{
    HildonFindReplaceDialogPrivate *priv =
        HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(dialog);
    return gtk_entry_get_text(GTK_ENTRY(
                              hildon_caption_get_control(priv->findEntry)));
}

/**
 * hildon_find_replace_dialog_get_new_text:
 * @dialog: the dialog.
 * 
 * Gets the text that's in the replace entry.
 * 
 * Return value: a pointer to the replace string.
 */
const gchar*
hildon_find_replace_dialog_get_new_text(HildonFindReplaceDialog *dialog)
{
    HildonFindReplaceDialogPrivate *priv =
        HILDON_FIND_REPLACE_DIALOG_GET_PRIVATE(dialog);
    return gtk_entry_get_text(GTK_ENTRY(hildon_caption_get_control(
                              priv->replaceEntry)));
}
