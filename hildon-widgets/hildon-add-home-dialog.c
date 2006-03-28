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
#include <gdk/gdkkeysyms.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>

#include "hildon-add-home-dialog.h"
#include <hildon-widgets/hildon-caption.h>

#define _(String) dgettext(PACKAGE, String)
#define MAX_ERR_MSG 256
#define HILDON_ADD_HOME_DIALOG_WIDTH 370
#define HILDON_ADD_HOME_DIALOG_HEIGHT 100
#define HILDON_MAX_TITLE_LENGTH 256
#define HILDON_HOME_MAX_SHORTCUT_LEN 255

#define HILDON_ADD_HOME_DIALOG_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
				HILDON_TYPE_ADD_HOME_DIALOG, \
				HildonAddHomeDialogPrivate));

typedef struct _HildonAddHomeDialogPrivate HildonAddHomeDialogPrivate;

static GtkDialogClass *parent_class;

static void
hildon_add_home_dialog_class_init(HildonAddHomeDialogClass * class);
static void hildon_add_home_dialog_init(HildonAddHomeDialog * dialog);

/* private struct */

struct _HildonAddHomeDialogPrivate {
    GtkWidget *desc_label;
    GtkWidget *name_entry;
    GtkWidget *new_name_entry;
    GtkWidget *okButton;
    GtkWidget *cancelButton;
    gboolean isrename;
};

/* Private functions */

static void
hildon_add_home_dialog_class_init(HildonAddHomeDialogClass * class)
{
    parent_class = g_type_class_peek_parent(class);
    g_type_class_add_private(class, sizeof(HildonAddHomeDialogPrivate));
}

static void hildon_add_home_dialog_init(HildonAddHomeDialog * dialog)
{
    HildonAddHomeDialogPrivate *priv;

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);
    priv->name_entry = NULL;
    priv->new_name_entry = NULL;
    priv->isrename = FALSE;

    priv->okButton = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                           "addtoHome_button_ok",
                                           GTK_RESPONSE_OK);
    priv->cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                               "addtoHome_button_cancel",
                                               GTK_RESPONSE_CANCEL);

    gtk_window_resize(GTK_WINDOW(dialog),
                      HILDON_ADD_HOME_DIALOG_WIDTH,
                      HILDON_ADD_HOME_DIALOG_HEIGHT);

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);
}

/* Public functions */

GType hildon_add_home_dialog_get_type(void)
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonAddHomeDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_add_home_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonAddHomeDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_add_home_dialog_init
        };

        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonAddHomeDialog",
                                             &dialog_info, 0);
    }
    return dialog_type;
}

/**
 * hildon_add_home_dialog_new:
 * @parent: parent window for the dialog
 * @name: name to show in the entry (or label, if @new_name is not NULL)
 * @new_name: name to show in the new name entry. If this is not NULL
 * the widget acts as a RenameShortcutDialog.
 *
 * Creates a new Add to Home dialog or Rename Shortcut dialog.
 *
 * Returns: the new dialog.
 */
GtkWidget *hildon_add_home_dialog_new(GtkWindow * parent,
                                      const gchar * name,
                                      const gchar * new_name)
{
    HildonAddHomeDialog *dialog;
    HildonAddHomeDialogPrivate *priv;
    GtkWidget *caption;
    GtkSizeGroup *size_group =
        gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);

    dialog =
        HILDON_ADD_HOME_DIALOG(g_object_new
                               (HILDON_TYPE_ADD_HOME_DIALOG, NULL));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

    /* Set up the window title */
    if (new_name != NULL) {
        priv->isrename = TRUE;
        gtk_window_set_title(GTK_WINDOW(dialog), _("ckdg_ti_rename_link"));
    } else {
        priv->isrename = FALSE;
        gtk_window_set_title(GTK_WINDOW(dialog), "addtoHome_dialog_title");
    }

    /* add description text */
    if (priv->isrename) {
        priv->desc_label =
            gtk_label_new(_("ckdg_ib_link_exists"));
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                           priv->desc_label, FALSE, FALSE, 0);
    }

    /* add name entry (or label ) */
    if (priv->isrename) {
        GtkWidget *label = gtk_label_new((name) ? name : "");

        priv->name_entry = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start(GTK_BOX(priv->name_entry), label, FALSE, FALSE,
                           0);
    } else {
        priv->name_entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(priv->name_entry),
                           (name) ? name : "");
    }
    caption = hildon_caption_new(size_group, "addtoHome_editor_caption",
                                 priv->name_entry, NULL,
                                 HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), caption,
                       FALSE, FALSE, 0);

    /* add new name entry */
    if (priv->isrename) {
        priv->new_name_entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(priv->new_name_entry), new_name);
        caption = hildon_caption_new(size_group, _("ckdg_fi_rename_name"),
                                     priv->new_name_entry, NULL,
                                     HILDON_CAPTION_OPTIONAL);
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), caption,
                           FALSE, FALSE, 0);
    }

    /* highlight proper entry */
    if (priv->isrename) {
        gtk_widget_grab_focus(priv->new_name_entry);
        gtk_editable_select_region(GTK_EDITABLE(priv->new_name_entry), 0,
                                   -1);
    } else {
        gtk_widget_grab_focus(priv->name_entry);
        gtk_editable_select_region(GTK_EDITABLE(priv->name_entry), 0, -1);
    }

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);

    return GTK_WIDGET(dialog);
}

/**
 * hildon_add_home_dialog_get_name:
 * @dialog: the dialog
 *
 * Returns: the string the user has entered in the entry
 */
const gchar *hildon_add_home_dialog_get_name(HildonAddHomeDialog * dialog)
{
    HildonAddHomeDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog), NULL);
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    if (priv->isrename)
        return gtk_entry_get_text(GTK_ENTRY(priv->new_name_entry));
    else
        return gtk_entry_get_text(GTK_ENTRY(priv->name_entry));
}
