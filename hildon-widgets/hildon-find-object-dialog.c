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

#include <glib.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>

#include <hildon-find-object-dialog.h>
#include <hildon-search.h>

#define _(String) dgettext(PACKAGE, String)
#define MAX_ERR_MSG 256

#define HILDON_FIND_OBJECT_DIALOG_WIDTH 370
#define HILDON_FIND_OBJECT_DIALOG_HEIGHT 100

static GtkDialogClass *parent_class;

typedef struct _HildonFindObjectDialogPrivate
    HildonFindObjectDialogPrivate;
struct _HildonFindObjectDialogPrivate {
    GtkButton *findButton;
    GtkButton *closeButton;
    GtkButton *optionsButton;
    GtkLabel *label;
    HildonSearch *search;
    GtkComboBoxEntry *combo;
    GtkDialog *optionsDialog;
};

#define HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_FIND_OBJECT_DIALOG,\
    HildonFindObjectDialogPrivate))

static void
hildon_find_object_dialog_class_init(HildonFindObjectDialogClass * class);
static void hildon_find_object_dialog_init(HildonFindObjectDialog *
                                           widget);
static void hildon_find_object_dialog_finalize(GObject * obj_self);
static void open_options_dialog(GtkWidget * widget,
                                HildonFindObjectDialog * dialog);
static void hildon_find_object_dialog_response_cb(HildonFindObjectDialog *
                                                  dialog, gint response,
                                                  gpointer data);
static void hildon_find_object_options_dialog_response_cb(GtkDialog *
                                                          dialog,
                                                          gint response,
                                                          gpointer data);

static void
hildon_find_object_dialog_class_init(HildonFindObjectDialogClass * class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);

    parent_class = g_type_class_peek_parent(class);
    object_class->finalize = hildon_find_object_dialog_finalize;

    g_type_class_add_private(class, sizeof(HildonFindObjectDialogPrivate));
}

static void hildon_find_object_dialog_finalize(GObject * obj_self)
{
    HildonFindObjectDialog *self;
    g_return_if_fail(HILDON_IS_FIND_OBJECT_DIALOG(obj_self));
    self = HILDON_FIND_OBJECT_DIALOG(obj_self);

    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);
}

static void
add_to_history(GtkEntry * entry, HildonFindObjectDialog * dialog)
{

    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *new_text;
    gchar *str_data;
    gint string_pos = 0;
    gboolean string_found = FALSE;
    gint count = 0;
    gboolean has_next;
    HildonFindObjectDialogPrivate *priv =
        HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(dialog);

    new_text = g_strdup((gchar *) gtk_entry_get_text(GTK_ENTRY(entry)));
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(priv->combo));

    if (strlen(new_text) == 0) {

    } else {
        has_next = gtk_tree_model_get_iter_first(model, &iter);
        while (has_next) {
            gtk_tree_model_get(model, &iter, 0, &str_data, -1);
            count++;
            if (strcasecmp(new_text, str_data) == 0) {
                string_found = TRUE;
                string_pos = count;
            }
            has_next = gtk_tree_model_iter_next(model, &iter);
            g_free(str_data);
        }
        if ((string_found)) {
            gtk_combo_box_remove_text(GTK_COMBO_BOX(priv->combo),
                                      string_pos - 1);
        } else if (count >= 4) {
            gtk_combo_box_remove_text(GTK_COMBO_BOX(priv->combo), count - 1);
        }
    }

    /* This has to be done like this, because otherwise gtkcombobox will
       return wrong value to the user when getting the currently selected
       item. */
    gtk_combo_box_prepend_text(GTK_COMBO_BOX(priv->combo), new_text);
    gtk_entry_set_text(GTK_ENTRY(entry), new_text);
    g_free(new_text);
}


static void hildon_find_object_dialog_init(HildonFindObjectDialog * dialog)
{

    HildonFindObjectDialogPrivate *priv =
        HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(dialog);

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    gtk_window_set_title(GTK_WINDOW(dialog), _("Find"));

    priv->findButton = GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                                        _("Find"),
                                                        GTK_RESPONSE_OK));

    priv->optionsButton =
        GTK_BUTTON(gtk_button_new_with_label(_("Options")));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->action_area),
                       GTK_WIDGET(priv->optionsButton), FALSE, TRUE, 0);

    priv->closeButton =
        GTK_BUTTON(gtk_dialog_add_button
                   (GTK_DIALOG(dialog), _("Close"), GTK_RESPONSE_CANCEL));

    priv->search = HILDON_SEARCH(hildon_search_new("gtk-find"));
    priv->combo = GTK_COMBO_BOX_ENTRY(gtk_combo_box_entry_new_text());

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->search), FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(priv->search), GTK_WIDGET(priv->combo));

    priv->label = GTK_LABEL(gtk_label_new(NULL));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->label), FALSE, FALSE, 0);

    gtk_window_resize(GTK_WINDOW(dialog),
                      HILDON_FIND_OBJECT_DIALOG_WIDTH,
                      HILDON_FIND_OBJECT_DIALOG_HEIGHT);

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);

}

static void
open_options_dialog(GtkWidget * widget, HildonFindObjectDialog * dialog)
{

    HildonFindObjectDialogPrivate *priv;
    priv = HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(dialog);
    g_return_if_fail(GTK_IS_DIALOG(priv->optionsDialog));

    g_signal_connect(G_OBJECT(priv->optionsDialog), "response",
                     G_CALLBACK(hildon_find_object_options_dialog_response_cb),
                     dialog);

    gtk_dialog_run(priv->optionsDialog);
}
static void
hildon_find_object_dialog_response_cb(HildonFindObjectDialog * dialog,
                                      gint response, gpointer data)
{

    HildonFindObjectDialogPrivate *priv =
        HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(dialog);

    if (response == GTK_RESPONSE_OK)
        add_to_history(GTK_ENTRY(GTK_BIN(priv->combo)->child), dialog);
}

static void
hildon_find_object_options_dialog_response_cb(GtkDialog * dialog,
                                              gint response, gpointer data)
{
    gtk_widget_hide(GTK_WIDGET(dialog));
    gtk_window_present(GTK_WINDOW(data));
}

/* Public functions */

/**
 * hildon_find_object_dialog_set_label:
 * @dialog: the #HildonFindObjectDialog which label wants to be changed
 * @label: gchar pointer to a string 
 *
 * Sets the label beneath the combobox in a #HildonFindObjectDialog widget.
 */
void
hildon_find_object_dialog_set_label(HildonFindObjectDialog * dialog,
                                    const gchar * label)
{
    HildonFindObjectDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_FIND_OBJECT_DIALOG(dialog));
    priv = HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(dialog);
    gtk_label_set_text(priv->label, label);
}

GType hildon_find_object_dialog_get_type(void)
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonFindObjectDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_find_object_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonFindObjectDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_find_object_dialog_init
        };

        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonFindObjectDialog",
                                             &dialog_info, 0);
    }
    return dialog_type;
}

/**
 * hildon_find_object_dialog_new:
 * @parent: the parent window of the dialog.
 *
 * Creates a new #HildonFindObjectDialog widget with Find and Close
 * buttons.
 *
 * Return value: the newly created #HildonFindObjectDialog
 */
GtkWidget *hildon_find_object_dialog_new(GtkWindow * parent)
{
    HildonFindObjectDialog *self =
        HILDON_FIND_OBJECT_DIALOG(g_object_new
                                  (HILDON_TYPE_FIND_OBJECT_DIALOG, NULL));

    HildonFindObjectDialogPrivate *priv =
        HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(self);

    g_signal_connect(G_OBJECT(self), "response",
                     G_CALLBACK(hildon_find_object_dialog_response_cb), NULL);

    gtk_container_remove(GTK_CONTAINER(GTK_DIALOG(self)->action_area),
                         GTK_WIDGET(priv->optionsButton));

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(self), parent);

    return GTK_WIDGET(self);
}

/**
 * hildon_find_object_dialog_new_with_options:
 * @parent: the parent window of the dialog.
 * @dialog: the options dialog to be shown on press on the options button.
 *
 * Creates a new #HildonFindObjectDialog widget with Find, Options and 
 * Close buttons. Sets the options dialog to be transient for the find 
 * object dialog.
 *
 * Return value: the newly created #HildonFindObjectDialog
 */

GtkWidget *hildon_find_object_dialog_new_with_options(GtkWindow * parent,
                                                      GtkDialog * dialog)
{
    HildonFindObjectDialog *self =
        HILDON_FIND_OBJECT_DIALOG(g_object_new
                                  (HILDON_TYPE_FIND_OBJECT_DIALOG, NULL));
    HildonFindObjectDialogPrivate *priv =
        HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(self);

    g_signal_connect(G_OBJECT(self), "response",
                     G_CALLBACK(hildon_find_object_dialog_response_cb), NULL);

    priv->optionsDialog = GTK_DIALOG(dialog);

    g_signal_connect(G_OBJECT(priv->optionsButton), "clicked",
                     G_CALLBACK(open_options_dialog), self);

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(self), parent);

    if (dialog)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(self));

    return GTK_WIDGET(self);
}

/**
 * hildon_find_object_dialog_get_text:
 * @dialog: the dialog.
 *
 * Gets the text that's active in the combobox (i.e. the find string).
 *
 * Return value: a pointer to the find string.
 */

const gchar *hildon_find_object_dialog_get_text(HildonFindObjectDialog *
                                                dialog)
{
    HildonFindObjectDialogPrivate *priv =
        HILDON_FIND_OBJECT_DIALOG_GET_PRIVATE(dialog);
    return gtk_entry_get_text(GTK_ENTRY(GTK_BIN(priv->combo)->child));
}
