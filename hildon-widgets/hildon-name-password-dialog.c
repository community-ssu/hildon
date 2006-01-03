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
  
  GtkLabel *domainLabel;
  HildonCaption *nameEntry;
  HildonCaption *passwordEntry;
};

/* Macro to access the private data of the object instance */
#define HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), HILDON_TYPE_NAME_PASSWORD_DIALOG,\
    HildonNamePasswordDialogPrivate))

enum{
    PROP_NONE = 0,
    PROP_CONTENT,
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
    case PROP_CONTENT:

    /* Set the password domain text
       FIXME: Why this is labeled as CONTENT?? */
	gtk_label_set_text(priv->domainLabel, g_value_get_string(value));
	break;
    case PROP_NAME:

      /* Set the current username displayed in the dialog */
      /* FIXME: Remove the combination of tabs/spaces to make the code look nice */
      gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child
				   (GTK_BIN(
					    priv->nameEntry))), 
			 g_value_get_string(value));
      break;
    case PROP_PASSWORD:

      /* Set the currently entered password */
      gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child
				   (GTK_BIN(
					    priv->passwordEntry))), 
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
    case PROP_CONTENT:
      string = gtk_label_get_text(priv->domainLabel);
      g_value_set_string(value, string);
      break;
    case PROP_NAME:
      /* FIXME: 1) intendation, 2) use g_object_get_property for entry */
      string = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child
					    (GTK_BIN(
						     priv->nameEntry))));

      g_value_set_string(value, string);
      break;
    case PROP_PASSWORD:
     /* FIXME: 1) intendation, 2) use g_object_get_property for entry */
     string = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child
					    (GTK_BIN(
						     priv->passwordEntry))));
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
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    
    parent_class = g_type_class_peek_parent(class);
    
    /* Override virtual functions */
    object_class->set_property = hildon_name_password_dialog_set_property;
    object_class->get_property = hildon_name_password_dialog_get_property;
    
    /* Install new properties */
    g_object_class_install_property(object_class, 
		    PROP_CONTENT, 
		    g_param_spec_string ("content",
			    "Content",
			    "Set content for content label.",
			    NULL,
			    G_PARAM_READWRITE));

    g_object_class_install_property(object_class,
                    PROP_NAME, 
                    g_param_spec_string ("name",
                            "Name",
                            "Set content for name entry.",
                            "DEFAULT",
                            G_PARAM_READWRITE));
    
    g_object_class_install_property(object_class, 
                    PROP_PASSWORD,
                    g_param_spec_string ("password",
                            "Password",
                            "Set content for password entry",
                            "DEFAULT",
                            G_PARAM_READWRITE));

    /* Install private data structure */
    g_type_class_add_private(class,
                             sizeof(HildonNamePasswordDialogPrivate));
}

static void
hildon_name_password_dialog_init(HildonNamePasswordDialog * dialog)
{
    /* Access private structure */
    HildonNamePasswordDialogPrivate *priv =
        HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    /* Size group for captions */
    GtkSizeGroup *group =
        GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));
    GtkWidget *control;
    
    /* Initialize dialog */
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_title(GTK_WINDOW(dialog), _(HILDON_NAME_PASSWORD_DIALOG_TITLE));

    /* Optional domain name label */    
    priv->domainLabel = GTK_LABEL(gtk_label_new(NULL));

    /* Create buttons */    
    priv->okButton =
      GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                       _(HILDON_NAME_PASSWORD_DIALOG_OK),
                                       GTK_RESPONSE_OK));
    priv->closeButton =
      GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                       _(HILDON_NAME_PASSWORD_DIALOG_CANCEL),
                                       GTK_RESPONSE_CANCEL));

    /* Setup user name entry */
    control = gtk_entry_new();
    priv->nameEntry = HILDON_CAPTION(hildon_caption_new
				     (group,
				      _(HILDON_NAME_PASSWORD_DIALOG_NAME ),
				      control, NULL,
				      HILDON_CAPTION_OPTIONAL));
    hildon_caption_set_separator(priv->nameEntry, "");

    /* Setup password entry */
    control = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(control), FALSE);
    priv->passwordEntry =
      HILDON_CAPTION(hildon_caption_new(group,
                                        _(HILDON_NAME_PASSWORD_DIALOG_PASSWORD),
                                        control,
                                        NULL,
                                        HILDON_CAPTION_OPTIONAL));
    hildon_caption_set_separator(priv->passwordEntry, "");

    /* Build layout */
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), 
	  GTK_WIDGET(priv->domainLabel), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), 
                       GTK_WIDGET(priv->nameEntry), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->passwordEntry), FALSE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    
    /* FIXME: Do not leak group */
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

    /* FIXME: Intendation */
    return gtk_entry_get_text(GTK_ENTRY(
			      (gtk_bin_get_child
			       (GTK_BIN(
				priv->nameEntry)))));
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

    /* FIXME: Intendation */
    return gtk_entry_get_text(GTK_ENTRY
			      (gtk_bin_get_child
			       (GTK_BIN(
				(priv->passwordEntry)))));
}

/**
 * hildon_name_password_dialog_set_domain(GtkWidget *dialog, 
 * @dialog: the dialog
 * @domain: the domain or some other descriptive text to be set.
 * 
 * sets the optional descriptive text
 */

void hildon_name_password_dialog_set_domain(HildonNamePasswordDialog *dialog, 
                                                gchar *domain)
{
  HildonNamePasswordDialogPrivate *priv =
    HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
  gtk_label_set_text(priv->domainLabel, domain);
  
}
