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
 * @file hildon-set-password-dialog.c
 *
 * This file implements API for Hildon Set and Change Password dialog.
 * 
 *  Change password dialog:
 *   @desc: Change Password Dialog is used to change an existing one in
 *   situtations where the password can be removed. Unselecting the check
 *   box dims the password fields below it. If the dialog is accepted with
 *   'OK' while the check box is unselected, a Confirmation Note is played.
 *   If a Confirmation Note Dialog is accepted with 'Remove', the password
 *   protection is removed.  
 * 
 *  Set password dialog:
 *   @desc: Set Password Dialog is used to define a password, or change a
 *   password that cannot be removed.
 *   @seealso: #HildonGetPasswordDialog
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtktogglebutton.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkentry.h>
#include <gtk/gtksignal.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

#include <hildon-widgets/hildon-caption.h>
#include <hildon-widgets/gtk-infoprint.h>
#include "hildon-set-password-dialog.h"
#include "hildon-note.h"

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass *parent_class;

#define HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(obj)\
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
  HILDON_TYPE_SET_PASSWORD_DIALOG, HildonSetPasswordDialogPrivate));

typedef struct
_HildonSetPasswordDialogPrivate HildonSetPasswordDialogPrivate;

static void
hildon_set_password_dialog_class_init(HildonSetPasswordDialogClass *
                                      class);
static void hildon_set_password_dialog_init(HildonSetPasswordDialog *
                                            dialog);
static void hildon_checbox_toggled(GtkWidget * widget, gpointer data);
static void
hildon_set_password_response_change(GtkDialog * d, gint arg1,
                                    GtkWindow * parent);
static void
hildon_set_password_response_set(GtkDialog * d, gint arg1,
                                 GtkWindow * parent);
static gboolean
hildon_set_password_dialog_released(GtkWidget * widget,
                                    GdkEventButton * event,
                                    gpointer user_data);
static gboolean
hildon_set_password_dialog_keypress(GtkWidget * widget,
                                    GdkEventKey * event, gpointer data);

static GObject *
hildon_set_password_dialog_constructor(GType type,
                                       guint n_construct_properties,
                                       GObjectConstructParam *
                                       construct_properties);
static void hildon_set_password_set_property(GObject * object,
                                             guint prop_id,
                                             const GValue * value,
                                             GParamSpec * pspec);
static void hildon_set_password_get_property(GObject * object,
                                             guint prop_id, GValue * value,
                                             GParamSpec * pspec);

/* Private struct */

struct _HildonSetPasswordDialogPrivate {
    /* Checkbox tab */
    GtkWidget *caption;
    GtkWidget *checkbox;

    /* Tab one */
    GtkWidget *entry1;
    GtkWidget *caption1;
    gchar *caption1_string;

    /* Tab two */
    GtkWidget *entry2;
    GtkWidget *caption2;
    gchar *caption2_string;

    /* OK/Cancel buttons */
    GtkWidget *okButton;
    GtkWidget *cancelButton;

    gboolean protection;

};

enum {
    PROP_NONE = 0,
    PROP_PASSWORD,
    PROP_HILDON_PASSWORD_DIALOG
};

/* Private functions */
static void
hildon_set_password_set_property(GObject * object,
                                 guint prop_id,
                                 const GValue * value, GParamSpec * pspec)
{
    HildonSetPasswordDialog *dialog = HILDON_SET_PASSWORD_DIALOG(object);
    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_PASSWORD:
        gtk_entry_set_text(GTK_ENTRY(priv->entry1), g_value_get_string(value));
        break;
    case PROP_HILDON_PASSWORD_DIALOG:
        priv->protection = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_set_password_get_property(GObject * object,
                                 guint prop_id,
                                 GValue * value, GParamSpec * pspec)
{
    HildonSetPasswordDialog *dialog = HILDON_SET_PASSWORD_DIALOG(object);
    HildonSetPasswordDialogPrivate *priv;
    const gchar *string;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_PASSWORD:
        string = gtk_entry_get_text(GTK_ENTRY(priv->entry1));
        g_value_set_string(value, string);
        break;
    case PROP_HILDON_PASSWORD_DIALOG:
        g_value_set_boolean(value, g_value_get_boolean(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static GObject *
hildon_set_password_dialog_constructor(GType type,
                                       guint n_construct_properties,
                                       GObjectConstructParam *
                                       construct_properties)
{
    GObject *dialog;
    GObjectClass *parent_class;
    HildonSetPasswordDialogPrivate *priv;
    GtkSizeGroup *group;

    parent_class = G_OBJECT_CLASS(g_type_class_peek_parent
                                  (g_type_class_peek(type)));
    dialog = parent_class->constructor(type, n_construct_properties,
                                       construct_properties);

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    priv->checkbox = NULL;

    group = GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    if (priv->protection == TRUE) {
        /* create checkbox */
        priv->caption1_string = _("ckdg_fi_dialog_c_passwd_new_pwd");
        priv->caption2_string = _("ckdg_fi_dialog_c_passwd_ver_pwd");

        priv->checkbox = gtk_check_button_new();
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->checkbox),
                                     TRUE);
        gtk_widget_show(priv->checkbox);

        priv->caption = hildon_caption_new(group,
                                           _("ckdg_fi_dialog_c_passwd_pwd_protect"),
                                           priv->checkbox,
                                           NULL, HILDON_CAPTION_OPTIONAL);

        gtk_signal_connect(GTK_OBJECT(priv->checkbox), "toggled",
                           G_CALLBACK(hildon_checbox_toggled), dialog);

        g_signal_connect(GTK_OBJECT(priv->checkbox),
                         "key-press-event",
                         G_CALLBACK(hildon_set_password_dialog_keypress),
                         dialog);

        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                           priv->caption, TRUE, TRUE, 0);
        gtk_widget_show(priv->caption);
    } else {
        priv->caption1_string = _("ecdg_fi_set_passwd_enter_pwd");
        priv->caption2_string = _("ecdg_fi_set_passwd_confirm");
    }

    /* create tab1 */
    priv->entry1 = gtk_entry_new();
    gtk_widget_show(priv->entry1);

    priv->caption1 = hildon_caption_new(group,
                                        priv->caption1_string,
                                        priv->entry1,
                                        NULL, HILDON_CAPTION_OPTIONAL);

    gtk_entry_set_visibility(GTK_ENTRY(priv->entry1), FALSE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       priv->caption1, TRUE, TRUE, 0);
    gtk_widget_show(priv->caption1);

    /* create tab2 */
    priv->entry2 = gtk_entry_new();
    gtk_widget_show(priv->entry2);

    priv->caption2 = hildon_caption_new(group,
                                        priv->caption2_string,
                                        priv->entry2,
                                        NULL, HILDON_CAPTION_OPTIONAL);

    gtk_entry_set_visibility(GTK_ENTRY(priv->entry2), FALSE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       priv->caption2, TRUE, TRUE, 0);

    gtk_widget_show(priv->caption2);

    /* Callback functions */
    g_signal_connect(priv->entry1, "button-release-event",
                     G_CALLBACK(hildon_set_password_dialog_released),
                     NULL);
    g_signal_connect(priv->entry2, "button-release-event",
                     G_CALLBACK(hildon_set_password_dialog_released),
                     NULL);

    gtk_signal_connect(GTK_OBJECT(priv->entry1),
                       "key-press-event",
                       G_CALLBACK(hildon_set_password_dialog_keypress),
                       dialog);
    gtk_signal_connect(GTK_OBJECT(priv->entry2),
                       "key-press-event",
                       G_CALLBACK(hildon_set_password_dialog_keypress),
                       dialog);

    gtk_window_set_title(GTK_WINDOW(dialog),
                         _(priv->protection
                           ? "ckdg_ti_dialog_c_passwd_change_password"
                           : "ecdg_ti_set_password"));

    /* Create the OK/CANCEL buttons */
    priv->okButton = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                           _(priv->protection
                                             ? "ckdg_bd_change_password_dialog_ok"
                                             : "ecdg_bd_set_password_dialog_ok"),
                                           GTK_RESPONSE_OK);
    priv->cancelButton = gtk_dialog_add_button(GTK_DIALOG(dialog),
                                               _(priv->protection
                                                 ? "ckdg_bd_change_password_dialog_cancel"
                                                 : "ecdg_bd_set_password_dialog_cancel"),
                                               GTK_RESPONSE_CANCEL);

    gtk_window_resize(GTK_WINDOW(dialog), 370, 110);

    gtk_widget_show(priv->okButton);
    gtk_widget_show(priv->cancelButton);


    return dialog;
}

static void
hildon_set_password_dialog_class_init(HildonSetPasswordDialogClass * class)
{
    GParamSpec *pspec1, *pspec2;

    GObjectClass *object_class = G_OBJECT_CLASS(class);

    parent_class = g_type_class_peek_parent(class);

    object_class->set_property = hildon_set_password_set_property;
    object_class->get_property = hildon_set_password_get_property;
    object_class->constructor = hildon_set_password_dialog_constructor;

    pspec1 = g_param_spec_boolean("modify_protection",
                                 "Password type",
                                 "Set type to dialog",
                                 TRUE,
                                 G_PARAM_CONSTRUCT_ONLY |
                                 G_PARAM_READWRITE);

    g_object_class_install_property(object_class,
                                    PROP_HILDON_PASSWORD_DIALOG, pspec1);
    
    pspec2 = g_param_spec_string("password",
				"Pasword content",
				"Set content to dialog",
				"DEFAULT",
				G_PARAM_READWRITE);
    
    g_object_class_install_property(object_class,
                                    PROP_PASSWORD, pspec2);

    g_type_class_add_private(class,
                             sizeof(HildonSetPasswordDialogPrivate));
}


static void
hildon_set_password_dialog_init(HildonSetPasswordDialog * dialog)
{
    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    priv->protection = FALSE;
}

static void
hildon_set_password_response_change(GtkDialog * dialog, gint arg1,
                                    GtkWindow * parent)
{
    GtkEntry *entry1;
    GtkEntry *entry2;
    gchar *text1;
    gchar *text2;

    HildonNote *note;
    gint i;

    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    entry1 = GTK_ENTRY(hildon_caption_get_control
                       (HILDON_CAPTION(priv->caption1)));

    entry2 = GTK_ENTRY(hildon_caption_get_control
                       (HILDON_CAPTION(priv->caption2)));

    text1 = GTK_ENTRY(entry1)->text;
    text2 = GTK_ENTRY(entry2)->text;
    if (arg1 == GTK_RESPONSE_OK){
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->checkbox))){
           if(strcmp(text1, "" ) != 0){
                if(strcmp (text1, text2) == 0){
                    priv->protection = TRUE;
                } else if(strcmp(text2, "" ) == 0){
                    g_signal_stop_emission_by_name(G_OBJECT(dialog),
                            "response");
                    gtk_infoprint (NULL,
                                  _("ckdg_ib_dialog_c_passwd_unmatched_pwd"));
                    gtk_widget_grab_focus(GTK_WIDGET(entry2));
                } else{
                    g_signal_stop_emission_by_name(G_OBJECT(dialog),
                            "response");
                    gtk_entry_set_text(entry1, "");
                    gtk_entry_set_text(entry2, "");
                    gtk_infoprint (NULL,
                                  _("ckdg_ib_dialog_c_passwd_unmatched_pwd"));
                    gtk_widget_grab_focus(GTK_WIDGET(entry1));
                }
            } else {
                g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
                gtk_infoprint (NULL, _("ecdg_ib_password_is_empt"));
            }
        } else{
	    note = HILDON_NOTE(hildon_note_new_confirmation(
			       GTK_WINDOW(dialog),
			       _("ckdg_nc_dialog_c_passwd_remove_pwd")));
	    
            hildon_note_set_button_text(HILDON_NOTE(note),
                                    _("ckdg_bd_dialog_c_passwd_remove_button"));

            i = gtk_dialog_run(GTK_DIALOG(note));

            gtk_widget_destroy(GTK_WIDGET(note));

            if (i == GTK_RESPONSE_OK)
	        priv->protection = FALSE;
            else {
	        priv->protection = TRUE;
                g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
	    }
        }
    }else{
        priv->protection = TRUE;
    }
}

static void
hildon_set_password_response_set(GtkDialog * dialog, gint arg1,
                                 GtkWindow * parent)
{
    GtkEntry *entry1;
    GtkEntry *entry2;
    gchar *text1;
    gchar *text2;

    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    entry1 = GTK_ENTRY(hildon_caption_get_control
                       (HILDON_CAPTION(priv->caption1)));

    entry2 = GTK_ENTRY(hildon_caption_get_control
                       (HILDON_CAPTION(priv->caption2)));

    text1 = GTK_ENTRY(entry1)->text;
    text2 = GTK_ENTRY(entry2)->text;

    if (arg1 == GTK_RESPONSE_OK) {
        if (strcmp (text1, "") != 0) {
            if (strcmp (text1, text2) == 0) {
                priv->protection = TRUE;
            } else if (strcmp (text2, "") == 0) {
                g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
                gtk_infoprint (NULL, _("ecdg_ib_password_is_empty"));
                gtk_widget_grab_focus (GTK_WIDGET (priv->entry2));
            } else {
                g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
	        gtk_entry_set_text(entry1, "");
		gtk_entry_set_text(entry2, "");
	        gtk_infoprint (NULL, _("ecdg_ib_passwords_do_not_match"));
                gtk_widget_grab_focus(GTK_WIDGET(priv->entry1));
	    }
        } else {
            g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
            gtk_entry_set_text(entry2, "");
            gtk_infoprint (NULL, _("ecdg_ib_password_is_empty"));
            gtk_widget_grab_focus(GTK_WIDGET(priv->entry1));
        }
    } else { 
        priv->protection = FALSE;
    }    	  
}

static void hildon_checbox_toggled(GtkWidget * widget, gpointer data)
{
    HildonSetPasswordDialog *dialog;
    HildonSetPasswordDialogPrivate *priv;

    dialog = HILDON_SET_PASSWORD_DIALOG(data);
    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        gtk_widget_set_sensitive(GTK_WIDGET(priv->entry1), TRUE);
        gtk_widget_set_sensitive(GTK_WIDGET(priv->entry2), TRUE);
    } else {
        gtk_widget_set_sensitive(GTK_WIDGET(priv->entry1), FALSE);
        gtk_widget_set_sensitive(GTK_WIDGET(priv->entry2), FALSE);
    }
}

static gboolean
hildon_set_password_dialog_released(GtkWidget * widget,
                                    GdkEventButton * event,
                                    gpointer user_data)
{
    GtkEditable *editable = GTK_EDITABLE(widget);

    gtk_editable_select_region(GTK_EDITABLE(editable), 0, -1);
    gtk_widget_grab_focus(GTK_WIDGET(editable));

    return FALSE;
}

static gboolean
hildon_set_password_dialog_keypress(GtkWidget * widget,
                                    GdkEventKey * event, gpointer data)
{
    HildonSetPasswordDialog *dialog;
    HildonSetPasswordDialogPrivate *priv;

    dialog = HILDON_SET_PASSWORD_DIALOG(data);
    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    if ((widget == priv->checkbox) &&
             ((event->keyval == GDK_Left) || (event->keyval == GDK_KP_Left)
              || (event->keyval == GDK_Right)
              || (event->keyval == GDK_KP_Right))) {
        gtk_widget_grab_focus(priv->checkbox);

        return TRUE;
    } else if ((widget == priv->checkbox) &&
               ((event->keyval == GDK_Return) ||
                (event->keyval == GDK_KP_Enter))) {
        if (gtk_toggle_button_get_active
            (GTK_TOGGLE_BUTTON(priv->checkbox))) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->checkbox),
                                         FALSE);
        } else {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->checkbox),
                                         TRUE);
        }
        return TRUE;
    } else if ((event->keyval == GDK_Return)
               || (event->keyval == GDK_KP_Enter)) {
        gtk_dialog_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

        return TRUE;
    } else {
        return FALSE;   /* pass the keypress on */
    }
}

/* Public functions */

/**
 * hildon_set_password_dialog_get_type:
 *
 * Returns GType for HildonPasswordDialog as produced by
 * g_type_register_static().
 *
 * Return value: HildonSetPasswordDialog type
 **/
GType hildon_set_password_dialog_get_type()
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonSetPasswordDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_set_password_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonSetPasswordDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_set_password_dialog_init
        };

        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonSetPasswordDialog",
                                             &dialog_info, 0);
    }
    return dialog_type;
}

/**
 * hildon_set_password_dialog_new:
 * @parent: parent window; can be NULL
 * @modify_protection: TRUE creates a new change password dialog and FALSE
 *                     creates a new set password dialog 
 * 
 * Construct a new HildonSetPasswordDialog.
 *
 * Return value: a new #GtkWidget of type HildonSetPasswordDialog
 **/

GtkWidget *hildon_set_password_dialog_new(GtkWindow * parent,
                                          gboolean modify_protection)
{
    GtkWidget *dialog = g_object_new
        (HILDON_TYPE_SET_PASSWORD_DIALOG, "modify_protection",
         modify_protection, NULL);

    if (modify_protection == TRUE) {
        g_signal_connect(G_OBJECT(dialog), "response",
                         G_CALLBACK(hildon_set_password_response_change),
                         parent);
    } else {
        g_signal_connect(G_OBJECT(dialog), "response",
                         G_CALLBACK(hildon_set_password_response_set),
                         parent);
    }
    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }

    return dialog;
}

/**
 * hildon_set_password_dialog_new_with_default:
 * @parent: parent window; can be NULL
 * @password: a default password to be shown in password field.
 * @modify_protection: TRUE creates a new change password dialog and FALSE
 *                     creates a new set password dialog 
 * 
 * Same as #hildon_set_password_dialog_new, but with a default password
 * in password field.
 *
 * Return value: a new #GtkWidget of type HildonSetPasswordDialog
 **/

GtkWidget *hildon_set_password_dialog_new_with_default
                                         (GtkWindow *parent,
					  gchar *password,
                                          gboolean modify_protection)
{
    GtkWidget *dialog;

    dialog = hildon_set_password_dialog_new(parent, 
					    modify_protection);
    if(password != NULL)
        g_object_set(G_OBJECT(dialog), "password", password, NULL);
    
    return dialog;
}

/**
 * hildon_set_password_dialog_get_password:
 * @dialog: pointer to HildonSetPasswordDialog
 * 
 * Returns current password.
 *
 * Return value: changed password ( if the dialog is successfully 
 * accepted with 'OK' ( and when the check box is 'ON' ( in Change Password
 * Dialog ))
 **/
const gchar
    *hildon_set_password_dialog_get_password(HildonSetPasswordDialog *
                                             dialog)
{
    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    return GTK_ENTRY(priv->entry1)->text;
}

/**
 * hildon_set_password_dialog_get_protected:
 * @dialog: pointer to HildonSetPasswordDialog
 * 
 * Returns protection mode.
 *
 * Return value: password protection mode ( TRUE when the protection is
 *               'ON' and FALSE when the protection is 'OFF' )
 **/
gboolean
hildon_set_password_dialog_get_protected(HildonSetPasswordDialog * dialog)
{
    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    return priv->protection;
}
