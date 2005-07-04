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
 * @shortdesc: NamePasswordDialog is a dialog for getting the user name
 * and password.
 * @longdesc: NamePasswordDialog is a simple dummy dialog for getting
 * name&password from the user.
 */

#include <glib.h>
#include <gtk/gtk.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#include <hildon-name-password-dialog.h>
#include <hildon-widgets/hildon-caption.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass *parent_class;

typedef struct _HildonNamePasswordDialogPrivate
    HildonNamePasswordDialogPrivate;

struct _HildonNamePasswordDialogPrivate {
    GtkButton *okButton;
    GtkButton *closeButton;

    HildonCaption *nameEntry;
    HildonCaption *passwordEntry;
};

#define HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_NAME_PASSWORD_DIALOG,\
    HildonNamePasswordDialogPrivate))

enum{
    PROP_NONE = 0,
    PROP_NAME,
    PROP_PASSWORD
};

static void
hildon_name_password_dialog_class_init(HildonNamePasswordDialogClass *class);
static void hildon_name_password_dialog_init(HildonNamePasswordDialog *widget);
static void hildon_name_password_dialog_set_property(GObject * object,
                                                     guint prop_id,
                                                     const GValue * value, 
						     GParamSpec * pspec);
static void hildon_name_password_dialog_get_property(GObject * object,
                                                     guint prop_id,
                                                     GValue * value, 
						     GParamSpec * pspec);
static void
hildon_name_password_dialog_set_property(GObject * object,
                                         guint prop_id,
                                         const GValue * value, GParamSpec * pspec)
{
    HildonNamePasswordDialog *dialog = HILDON_NAME_PASSWORD_DIALOG(object);
    HildonNamePasswordDialogPrivate *priv;

    priv = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_NAME:
        gtk_entry_set_text(GTK_ENTRY(
		           hildon_caption_get_control(priv->nameEntry)), 
			   g_value_get_string(value));
	break;
    case PROP_PASSWORD:
        gtk_entry_set_text(GTK_ENTRY(
		       hildon_caption_get_control(priv->passwordEntry)),
			   g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_name_password_dialog_get_property(GObject * object,
                                         guint prop_id,
                                         GValue * value, GParamSpec * pspec)
{
    HildonNamePasswordDialog *dialog = HILDON_NAME_PASSWORD_DIALOG(object);
    HildonNamePasswordDialogPrivate *priv;
    const gchar *string;

    priv = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_NAME:
        string = gtk_entry_get_text(GTK_ENTRY(
		hildon_caption_get_control(priv->nameEntry)));
	g_value_set_string(value, string);
	break;
    case PROP_PASSWORD:
        string = gtk_entry_get_text(GTK_ENTRY(
		hildon_caption_get_control(priv->passwordEntry)));
        g_value_set_string(value, string);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}
static void
hildon_name_password_dialog_class_init(HildonNamePasswordDialogClass *class)
{
    GParamSpec *pspec1, *pspec2;
  
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    parent_class = g_type_class_peek_parent(class);
    
    object_class->set_property = hildon_name_password_dialog_set_property;
    object_class->get_property = hildon_name_password_dialog_get_property;
    
    pspec1 = g_param_spec_string("name",
				"Name content",
				"Set content to dialog",
				"DEFAULT",
				G_PARAM_READWRITE);
    
    g_object_class_install_property(object_class,
                                    PROP_NAME, pspec1);
    
    pspec2 = g_param_spec_string("password",
				"Pasword content",
				"Set content to dialog",
				"DEFAULT",
				G_PARAM_READWRITE);
    
    g_object_class_install_property(object_class,
                                    PROP_PASSWORD, pspec2);
    g_type_class_add_private(class,
                             sizeof(HildonNamePasswordDialogPrivate));
}

static void
hildon_name_password_dialog_init(HildonNamePasswordDialog * dialog)
{
    HildonNamePasswordDialogPrivate *priv =
        HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    GtkSizeGroup *group =
        GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));
    GtkWidget *control;

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_title(GTK_WINDOW(dialog), _("frw_ti_get_user_name_and_pwd"));

    priv->okButton =
      GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                       _("frw_bd_get_user_name_and_pwd_ok"),
                                       GTK_RESPONSE_OK));
    priv->closeButton =
      GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                       _("frw_bd_get_user_name_and_pwd_cancel"),
                                       GTK_RESPONSE_CANCEL));

    control = gtk_entry_new();
    gtk_widget_show_all(control);
    priv->nameEntry = HILDON_CAPTION(hildon_caption_new(group,
                       _("frw_ti_get_user_name_and_pwd_enter_user_name"),
                                                        control, NULL,
                       HILDON_CAPTION_OPTIONAL));

    control = gtk_entry_new();
    gtk_widget_show_all(control);
    gtk_entry_set_visibility(GTK_ENTRY(control), FALSE);
    priv->passwordEntry =
      HILDON_CAPTION(hildon_caption_new(group,
                                        _("frw_ti_get_user_name_and_pwd_enter_pwd"),
                                        control,
                                        NULL,
                                        HILDON_CAPTION_OPTIONAL));

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), 
                       GTK_WIDGET(priv->nameEntry), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->passwordEntry), FALSE, FALSE, 0);

    gtk_widget_grab_focus(GTK_WIDGET(priv->nameEntry));
    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
}

GType hildon_name_password_dialog_get_type(void)
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonNamePasswordDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_name_password_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonNamePasswordDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_name_password_dialog_init
        };
        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonNamePasswordDialog",
                                             &dialog_info, 0);
    }

    return dialog_type;
}

/**
 * hildon_name_password_dialog_new:
 * @parent: the parent window of the dialog.
 *
 * Creates a new #HildonNamePasswordDialog widget with Ok and Close
 * buttons.
 *
 * Return value: the newly created #HildonNamePasswordDialog
 */
GtkWidget *hildon_name_password_dialog_new(GtkWindow * parent)
{
    GtkWidget *self = g_object_new(HILDON_TYPE_NAME_PASSWORD_DIALOG,NULL);

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(self), parent);
    return self;
}

/**
 * hildon_name_password_dialog_new_with_default:
 * @parent: the parent window of the dialog.
 * @name: default name, nul-terminated string, if you want
 * to leave it unset, pass a NULL pointer.
 * @password: default password, nul-terminated string, 
 * if you want to leave it unset, pass a NULL pointer.
 * 
 * Same as #hildon_name_password_dialog_new, but with a 
 * default name and password.
 *
 * Return value: the newly created #HildonNamePasswordDialog
 */
GtkWidget *hildon_name_password_dialog_new_with_default
                                          (GtkWindow * parent,
					   gchar *name,
					   gchar *password)
{
    GtkWidget *self = hildon_name_password_dialog_new(parent);

    if(name != NULL)
      g_object_set(G_OBJECT(self), "name", name, NULL);
    if(password != NULL)
      g_object_set(G_OBJECT(self), "password", password, NULL);
    
    return self;
}

/**
 * hildon_name_password_dialog_get_name:
 * @dialog: the dialog.
 *
 * Gets the text that's in the name entry.
 *
 * Return value: a pointer to the name string.
 */
const gchar *hildon_name_password_dialog_get_name(HildonNamePasswordDialog
                                                  * dialog)
{
    HildonNamePasswordDialogPrivate *priv =
        HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    return gtk_entry_get_text(GTK_ENTRY(
                              hildon_caption_get_control(priv->nameEntry)));
}

/**
 * hildon_name_password_dialog_get_password:
 * @dialog: the dialog.
 * 
 * Gets the text that's in the password entry.
 * 
 * Return value: a pointer to the password string.
 */
const gchar *hildon_name_password_dialog_get_password(HildonNamePasswordDialog
                                                      * dialog)
{
    HildonNamePasswordDialogPrivate *priv =
        HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    return gtk_entry_get_text(GTK_ENTRY(hildon_caption_get_control(
                              priv->passwordEntry)));
}
