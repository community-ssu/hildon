/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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
 * SECTION:hildon-name-password-dialog
 * @short_description: A widget which allows a user to enter an username
 * and a password
 * @see_also: #HildonGetPasswordDialog, #HildonSetPasswordDialog
 *
 * #HildonNamePasswordDialog is used to enter a username and password
 * when accessing a password protected function. The widget performs no
 * input checking and is used only for retrieving a user name and a
 * password. 
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
  GtkEntry *nameEntry;
  GtkEntry *passwordEntry;
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
    HildonNamePasswordDialog        *dialog = NULL;
    HildonNamePasswordDialogPrivate *priv   = NULL;

    dialog = HILDON_NAME_PASSWORD_DIALOG(object);
    priv   = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_CONTENT:
      /* Set the password domain text */
      hildon_name_password_dialog_set_domain(dialog, g_value_get_string(value));
      break;
    case PROP_NAME:
      /* Set the current username displayed in the dialog */
      gtk_entry_set_text(priv->nameEntry, g_value_get_string(value));
      break;
    case PROP_PASSWORD:
      /* Set the currently entered password */
      gtk_entry_set_text(priv->passwordEntry, g_value_get_string(value));
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
    HildonNamePasswordDialog        *dialog = NULL;
    HildonNamePasswordDialogPrivate *priv   = NULL;

    dialog = HILDON_NAME_PASSWORD_DIALOG(object);
    priv   = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_CONTENT:
      g_value_set_string(value, gtk_label_get_text(priv->domainLabel));
      break;
    case PROP_NAME:
      g_value_set_string(value, hildon_name_password_dialog_get_name(dialog));
      break;
    case PROP_PASSWORD:
      g_value_set_string(value, hildon_name_password_dialog_get_password(dialog));
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
    HildonCaption *caption;
    
    /* Initialize dialog */
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_title(GTK_WINDOW(dialog), _(HILDON_NAME_PASSWORD_DIALOG_TITLE));

    /* Optional domain name label */    
    priv->domainLabel = GTK_LABEL(gtk_label_new(NULL));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), 
	  GTK_WIDGET(priv->domainLabel), FALSE, FALSE, 0);

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
    priv->nameEntry = GTK_ENTRY(gtk_entry_new());
    g_object_set (priv->nameEntry, "hildon-input-mode", HILDON_GTK_INPUT_MODE_FULL);
    caption = HILDON_CAPTION(hildon_caption_new
				     (group,
				      _(HILDON_NAME_PASSWORD_DIALOG_NAME ),
				      GTK_WIDGET(priv->nameEntry), NULL,
				      HILDON_CAPTION_OPTIONAL));
    hildon_caption_set_separator(caption, "");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), 
                       GTK_WIDGET(caption), FALSE, FALSE, 0);

    /* Setup password entry */
    priv->passwordEntry = GTK_ENTRY(gtk_entry_new());
    g_object_set (priv->passwordEntry, "hildon-input-mode", HILDON_GTK_INPUT_MODE_FULL);
    gtk_entry_set_visibility(GTK_ENTRY(priv->passwordEntry), FALSE);
    caption =
      HILDON_CAPTION(hildon_caption_new(group,
                                        _(HILDON_NAME_PASSWORD_DIALOG_PASSWORD),
                                        GTK_WIDGET(priv->passwordEntry),
                                        NULL,
                                        HILDON_CAPTION_OPTIONAL));
    hildon_caption_set_separator(caption, "");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(caption), FALSE, FALSE, 0);

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
    
    /* Ensure group is freed when all its contents have been removed */
    g_object_unref(group);
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
 * @parent: the parent window of the dialog
 *
 * Creates a new #HildonNamePasswordDialog widget with Ok and Close
 * buttons.
 *
 * Returns: the newly created #HildonNamePasswordDialog
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
 * @parent: the parent window of the dialog
 * @name: default name, NULL if unset
 * @password: default password, NULL if unset
 * 
 * Same as #hildon_name_password_dialog_new, but with a 
 * default name and password.
 *
 * Returns: the newly created #HildonNamePasswordDialog
 */
GtkWidget *hildon_name_password_dialog_new_with_default(GtkWindow   *parent,
					                const gchar *name,
					                const gchar *password)
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
 * @dialog: the dialog
 *
 * Gets the text that's in the name entry.
 *
 * Returns: a pointer to the name string.
 */
const gchar *hildon_name_password_dialog_get_name(HildonNamePasswordDialog
                                                  * dialog)
{
    HildonNamePasswordDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_NAME_PASSWORD_DIALOG(dialog), NULL);

    priv = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    return gtk_entry_get_text(priv->nameEntry);
}

/**
 * hildon_name_password_dialog_get_password:
 * @dialog: the dialog
 * 
 * Gets the text that's in the password entry.
 * 
 * Returns: a pointer to the password string
 */
const gchar *hildon_name_password_dialog_get_password(HildonNamePasswordDialog
                                                      * dialog)
{
    HildonNamePasswordDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_NAME_PASSWORD_DIALOG(dialog), NULL);

    priv = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    return gtk_entry_get_text(priv->passwordEntry);
}

/**
 * hildon_name_password_dialog_set_domain(GtkWidget *dialog, 
 * @dialog: the dialog
 * @domain: the domain or some other descriptive text to be set
 * 
 * sets the optional descriptive text
 */

void hildon_name_password_dialog_set_domain(HildonNamePasswordDialog *dialog, 
					    const gchar *domain)
{
  HildonNamePasswordDialogPrivate *priv;

  g_return_if_fail(HILDON_IS_NAME_PASSWORD_DIALOG(dialog));

  priv = HILDON_NAME_PASSWORD_DIALOG_GET_PRIVATE(dialog);
  gtk_label_set_text(priv->domainLabel, domain);
}
