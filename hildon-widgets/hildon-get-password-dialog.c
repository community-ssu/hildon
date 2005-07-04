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
 * @file hildon-get-password-dialog.c
 *
 * This file contains the API implementation for Hildon Get Password
 * dialog.
 *  Get password dialog:
 *   @desc: Get Password Dialog is used to enter a password when accessing
 *   a password protected function 
 * 
 * Get old password dialog:
 *   @desc: Get Old Password is used to enter the current password 
 *          in order to either change or remove the existing password.
 *
 *  @seealso: #HildonSetPasswordDialog
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtkentry.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkeditable.h>
#include <gtk/gtksignal.h>
#include <gdk/gdkkeysyms.h>

#include <hildon-widgets/hildon-caption.h>
#include "hildon-get-password-dialog.h"

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass *parent_class;


#define PASSWORD_DIALOG_WIDTH       370
#define PASSWORD_DIALOG_HEIGHT      100

#define ENTER_PASSWORD_CHAR         _("ecdg_fi_verify_pwd_enter_pwd")
#define OK_CHAR                     _("ecdg_bd_verify_password_dialog_ok")
#define CANCEL_CHAR                 _("ecdg_bd_verify_password_dialog_cancel")
#define PASSWORD_PROTECTED_CHAR     _("ecdg_bd_verify_password")
#define ENTER_CURRENT_PASSWORD_CHAR _("ecdg_ti_get_old_password")

#define HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(obj) \
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
  HILDON_TYPE_GET_PASSWORD_DIALOG, HildonGetPasswordDialogPrivate));

typedef struct _HildonGetPasswordDialogPrivate
    HildonGetPasswordDialogPrivate;

static void
hildon_get_password_dialog_class_init(HildonGetPasswordDialogClass *
                                      class);
static void hildon_get_password_dialog_init(HildonGetPasswordDialog *
                                            widget);
static gboolean hildon_get_password_dialog_released(GtkWidget * widget,
                                                    GdkEventButton * event,
                                                    gpointer user_data);
static gboolean special_key_listener(GtkWidget * widget,
                                     GdkEventKey * event,
                                     gpointer user_data);

static void hildon_get_password_set_property(GObject * object,
                                             guint prop_id,
                                             const GValue * value,
                                             GParamSpec * pspec);
static void hildon_get_password_get_property(GObject * object,
                                             guint prop_id, GValue * value,
                                             GParamSpec * pspec);

/* private struct */
struct _HildonGetPasswordDialogPrivate {
    /* Tab one */
    GtkWidget *entry1;
    GtkWidget *caption1;

    /* OK/Cancel buttons */
    GtkWidget *okButton;
    GtkWidget *cancelButton;
};

enum{
    PROP_NONE = 0,
    PROP_PASSWORD
};

/* Private functions */
static void
hildon_get_password_set_property(GObject * object,
                                 guint prop_id,
                                 const GValue * value, GParamSpec * pspec)
{
    HildonGetPasswordDialog *dialog = HILDON_GET_PASSWORD_DIALOG(object);
    HildonGetPasswordDialogPrivate *priv;

    priv = HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_PASSWORD:
        gtk_entry_set_text(GTK_ENTRY(priv->entry1), g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_get_password_get_property(GObject * object,
                                 guint prop_id,
                                 GValue * value, GParamSpec * pspec)
{
    HildonGetPasswordDialog *dialog = HILDON_GET_PASSWORD_DIALOG(object);
    HildonGetPasswordDialogPrivate *priv;
    const gchar *string;

    priv = HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_PASSWORD:
        string = gtk_entry_get_text(GTK_ENTRY(priv->entry1));
        g_value_set_string(value, string);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}
static void
hildon_get_password_dialog_class_init(HildonGetPasswordDialogClass * class)
{
    GParamSpec *pspec1;
    GObjectClass *object_class = G_OBJECT_CLASS(class);
  
    parent_class = g_type_class_peek_parent(class);

    object_class->set_property = hildon_get_password_set_property;
    object_class->get_property = hildon_get_password_get_property;

    pspec1 = g_param_spec_string("password",
				 "Password content",
				 "Set content to dialog",
				 "DEFAULT",
				 G_PARAM_READWRITE);

    g_object_class_install_property (object_class,
				     PROP_PASSWORD,pspec1);
    
    g_type_class_add_private(class,
                             sizeof(HildonGetPasswordDialogPrivate));
}

static void
hildon_get_password_dialog_init(HildonGetPasswordDialog * dialog)
{
    HildonGetPasswordDialogPrivate *priv;
    GtkSizeGroup *group;

    priv = HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    group = GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    /* create tab */
    priv->entry1 = gtk_entry_new();
    gtk_widget_show(priv->entry1);

    priv->caption1 = hildon_caption_new(group,
                                        ENTER_PASSWORD_CHAR,
                                        priv->entry1,
                                        NULL, HILDON_CAPTION_OPTIONAL);

    gtk_entry_set_visibility(GTK_ENTRY(priv->entry1), FALSE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       priv->caption1, TRUE, TRUE, 0);
    gtk_widget_show(priv->caption1);

    /* Callback functions */
    g_signal_connect(G_OBJECT(priv->entry1), "key-press-event",
                     G_CALLBACK(special_key_listener), dialog);

    g_signal_connect(priv->entry1, "button-release-event",
                     G_CALLBACK(hildon_get_password_dialog_released),
                     NULL);

    /* Create the OK/CANCEL buttons */
    priv->okButton = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                           OK_CHAR, GTK_RESPONSE_OK);
    priv->cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                               CANCEL_CHAR,
                                               GTK_RESPONSE_CANCEL);
    gtk_window_resize(GTK_WINDOW(dialog),
                      PASSWORD_DIALOG_WIDTH, PASSWORD_DIALOG_HEIGHT);
}

static gboolean
hildon_get_password_dialog_released(GtkWidget * widget,
                                    GdkEventButton * event,
                                    gpointer user_data)
{
    GtkEditable *editable = GTK_EDITABLE(widget);

    gtk_editable_select_region(GTK_EDITABLE(editable), 0, -1);
    gtk_widget_grab_focus(GTK_WIDGET(editable));

    return FALSE;
}

static gboolean
special_key_listener(GtkWidget * widget,
                     GdkEventKey * event, gpointer user_data)
{
    HildonGetPasswordDialog *dialog =
        HILDON_GET_PASSWORD_DIALOG(user_data);

    if ((event->keyval == GDK_Return) || (event->keyval == GDK_KP_Enter)) {
        gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    }

    return FALSE;
}

/* Public functions */

/**
 * hildon_get_password_dialog_get_type:
 *
 * Returns GType for HildonGetPasswordDialog as produced by
 * g_type_register_static().
 *
 * Return value: HildonGetPasswordDialog type
 **/
GType hildon_get_password_dialog_get_type()
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonGetPasswordDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_get_password_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonGetPasswordDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_get_password_dialog_init
        };

        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonGetPasswordDialog",
                                             &dialog_info, 0);
    }
    return dialog_type;
}

/**
 * hildon_get_password_dialog_new:
 * @parent: parent window; can be NULL
 * @get_old_password_title: FALSE creates a new get password dialog and
 *                     TRUE creates a new get old password dialog 
 * 
 * Construct a new HildonGetPasswordDialog.
 *
 * Return value: a new #GtkWidget of type HildonGetPasswordDialog
 **/
GtkWidget *hildon_get_password_dialog_new(GtkWindow * parent,
                                          gboolean get_old_password_title)
{
    HildonGetPasswordDialog *dialog = g_object_new
        (HILDON_TYPE_GET_PASSWORD_DIALOG,
         NULL);

    if (get_old_password_title == FALSE) {
        gtk_window_set_title(GTK_WINDOW(dialog), PASSWORD_PROTECTED_CHAR);
    } else {
        gtk_window_set_title(GTK_WINDOW(dialog),
                             ENTER_CURRENT_PASSWORD_CHAR);
    }

    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }

    return GTK_WIDGET(dialog);
}

/**
 * hildon_get_password_dialog_new:
 * @parent: parent window; can be NULL
 * @password: a default password to be shown in password field.
 * @get_old_password_title: FALSE creates a new get password dialog and
 *                     TRUE creates a new get old password dialog 
 * 
 * Same as #hildon_get_password_dialog_new but with a default password
 * in password field.
 *
 * Return value: a new #GtkWidget of type HildonGetPasswordDialog
 **/
GtkWidget *hildon_get_password_dialog_new_with_default (GtkWindow * parent,
							gchar *password,
                                               gboolean get_old_password_title)
{
    GtkWidget *dialog;

    dialog = hildon_get_password_dialog_new(parent, get_old_password_title);
    if(password != NULL)
        g_object_set(G_OBJECT(dialog), "password", password, NULL);

    return GTK_WIDGET(dialog);
}

/**
 * hildon_get_password_dialog_get_password:
 * @dialog: pointer to HildonSetPasswordDialog
 * 
 * Gets the currently inputted password.
 *
 * Return value: current password ( if the dialog is successfully 
 * accepted with 'OK'  )
 **/
const gchar
    *hildon_get_password_dialog_get_password(HildonGetPasswordDialog *
                                             dialog)
{
    GtkEntry *entry1;
    gchar *text1;

    HildonGetPasswordDialogPrivate *priv;

    priv = HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    entry1 = GTK_ENTRY(hildon_caption_get_control
                       (HILDON_CAPTION(priv->caption1)));

    text1 = GTK_ENTRY(entry1)->text;

    return text1;
}
