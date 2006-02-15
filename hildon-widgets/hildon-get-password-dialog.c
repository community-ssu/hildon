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

#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#include <gtk/gtk.h>

#include "gtk-infoprint.h"
#include "hildon-input-mode-hint.h"

#include <hildon-widgets/hildon-caption.h>
#include <hildon-widgets/hildon-get-password-dialog.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass * parent_class;

typedef struct _HildonGetPasswordDialogPrivate 
  HildonGetPasswordDialogPrivate;

struct _HildonGetPasswordDialogPrivate {
  GtkButton *okButton;
  GtkButton *cancelButton;
  
  GtkLabel *domainLabel;
  HildonCaption *passwordEntry;
};


#define HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(obj) \
 (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
  HILDON_TYPE_GET_PASSWORD_DIALOG, HildonGetPasswordDialogPrivate));

static void
hildon_get_password_dialog_class_init(HildonGetPasswordDialogClass *
                                      class);
static void hildon_get_password_dialog_init(HildonGetPasswordDialog *
                                            widget);
static void hildon_get_password_set_property(GObject * object,
                                             guint prop_id,
                                             const GValue * value,
                                             GParamSpec * pspec);
static void hildon_get_password_get_property(GObject * object,
                                             guint prop_id, GValue * value,
                                             GParamSpec * pspec);
void hildon_get_password_dialog_set_domain(HildonGetPasswordDialog *dialog, 
					   gchar *domain);

static void _invalid_input(GtkWidget *widget, GtkInvalidInputType reason, 
			   gpointer user_data);

enum{
    PROP_NONE = 0,
    PROP_DOMAIN,
    PROP_PASSWORD,
    PROP_NUMBERS_ONLY,
    PROP_CAPTION_LABEL,
    PROP_MAX_CHARS
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
  case PROP_DOMAIN:
    /* Set label text representing password domain */
    gtk_label_set_text(priv->domainLabel, g_value_get_string(value));
    break;
  case PROP_PASSWORD:
    gtk_entry_set_text(GTK_ENTRY
		       (gtk_bin_get_child(GTK_BIN(priv->passwordEntry))),
		       g_value_get_string(value));
    break;
  case PROP_NUMBERS_ONLY:
    /* FIXME: This is broken, property value is not used in any way */
    g_object_set( G_OBJECT
		  (gtk_bin_get_child(GTK_BIN(priv->passwordEntry))),
                  "input-mode", HILDON_INPUT_MODE_HINT_NUMERIC, NULL );
    break;
  case PROP_CAPTION_LABEL:
    hildon_get_password_dialog_set_caption(dialog, g_value_get_string(value));
    break;
  case PROP_MAX_CHARS:
    /* FIXME: This is broken. set_max_characters wants an int, not string */
    hildon_get_password_dialog_set_max_characters(dialog, 
						  g_value_get_string(value));
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
    gint max_length; 

    priv = HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    
    switch (prop_id) {
    case PROP_DOMAIN:
      string = gtk_label_get_text(priv->domainLabel);
      g_value_set_string(value, string);
    case PROP_PASSWORD:
      string = gtk_entry_get_text(GTK_ENTRY(priv->passwordEntry));
      g_value_set_string(value, string);
      break;
    case PROP_CAPTION_LABEL:
      string = hildon_caption_get_label(priv->passwordEntry);
      g_value_set_string(value, string);
      break;
    case PROP_MAX_CHARS:
      max_length = gtk_entry_get_max_length(
	      GTK_ENTRY (hildon_caption_get_control (priv->passwordEntry)));
      g_value_set_int(value, max_length);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
    }
}

static void
hildon_get_password_dialog_class_init(HildonGetPasswordDialogClass * class)
{

  GObjectClass *object_class = G_OBJECT_CLASS(class);
  
  parent_class = g_type_class_peek_parent(class);
  
  /* Override virtual functions */
  object_class->set_property = hildon_get_password_set_property;
  object_class->get_property = hildon_get_password_get_property;
  
  /* Install new properties */
  /* FIXME: Why this is not READWRITE */
  g_object_class_install_property 
    (object_class, 
     PROP_DOMAIN, 
     g_param_spec_string ("domain",
			  "Domain",
			  "Set domain(content) for optional label.",
			  NULL,
			  G_PARAM_WRITABLE));
  
  g_object_class_install_property
    (object_class, 
     PROP_PASSWORD,
     g_param_spec_string ("password",
			  "Password",
			  "Set content for password entry",
			  "DEFAULT",
			  G_PARAM_READWRITE));

  /* FIXME: Why this is not READWRITE?? */
  g_object_class_install_property
    (object_class, 
     PROP_NUMBERS_ONLY,
     g_param_spec_boolean ("numbers_only",
			  "NumbersOnly",
			  "Set entry to accept only numeric values",
			  FALSE,
			  G_PARAM_WRITABLE));

  g_object_class_install_property
    (object_class, 
     PROP_CAPTION_LABEL,
     g_param_spec_string ("caption-label",
			  "Caption Label",
			  "The text to be set as the caption label",
			  NULL,
			  G_PARAM_READWRITE));

  g_object_class_install_property
    (object_class, 
     PROP_MAX_CHARS,
     g_param_spec_int ("max-characters",
		       "Maximum Characters",
		       "The maximum number of characters the password"
		       " dialog accepts",
		       G_MININT,
		       G_MAXINT,
		       0,
		       G_PARAM_READWRITE));

    /* Install private structure */
    g_type_class_add_private(class,
                             sizeof(HildonGetPasswordDialogPrivate));
}

static void
hildon_get_password_dialog_init(HildonGetPasswordDialog * dialog)
{
    GtkSizeGroup * group;
    GtkWidget *control;

    /* Cache private pointer for faster member access */  
    HildonGetPasswordDialogPrivate *priv =
      HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    /* Sizegroup for captions */
    group = GTK_SIZE_GROUP(gtk_size_group_new
			   (GTK_SIZE_GROUP_HORIZONTAL));

    /* Initial properties for the dialog */
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_title(GTK_WINDOW(dialog), 
			 _(HILDON_GET_PASSWORD_DIALOG_TITLE));

    /* Optional password domain label */
    priv->domainLabel = GTK_LABEL(gtk_label_new(NULL));
    
    /* Create buttons */
    priv->okButton =
      GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                       _(HILDON_GET_PASSWORD_DIALOG_OK),
                                       GTK_RESPONSE_OK));
    priv->cancelButton =
      GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                       _(HILDON_GET_PASSWORD_DIALOG_CANCEL),
                                       GTK_RESPONSE_CANCEL));

    /* Create password text entry */
    control = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(control), FALSE);
    priv->passwordEntry = HILDON_CAPTION
      (hildon_caption_new(group,
			  _(HILDON_GET_PASSWORD_DIALOG_PASSWORD ),
			  control, NULL,
			  HILDON_CAPTION_OPTIONAL));
    hildon_caption_set_separator(HILDON_CAPTION(priv->passwordEntry), "");
    

    /* Do the basic layout */
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->domainLabel), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(priv->passwordEntry), FALSE, FALSE, 0);
    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    
    gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);

    /* FIXME: Do not leak group */
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
      HildonGetPasswordDialogPrivate *priv;

      /* Override "get old password" defaults set in dialog_init */
      /* FIXME: This behaviour is confusing */
      gtk_window_set_title(GTK_WINDOW(dialog), 
			     _(HILDON_GET_PASSWORD_VERIFY_DIALOG_TITLE));
		priv = HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
	gtk_button_set_label(priv->okButton, 
			     _(HILDON_GET_PASSWORD_VERIFY_DIALOG_OK));
	gtk_button_set_label(priv->cancelButton, 
			     _(HILDON_GET_PASSWORD_VERIFY_DIALOG_CANCEL));
	hildon_caption_set_label(priv->passwordEntry,
				 _(HILDON_GET_PASSWORD_VERIFY_DIALOG_PASSWORD)
				 );
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

    /* Retrieve the password entry widget */
    entry1 = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(
                       priv->passwordEntry)));

    text1 = GTK_ENTRY(entry1)->text;

    return text1;
}

/**
 * hildon_get_password_dialog_set_domain(GtkWidget *dialog, 
 * @dialog: the dialog
 * @domain: the domain or some other descriptive text to be set.
 * 
 * sets the optional descriptive text
 */

void hildon_get_password_dialog_set_domain(HildonGetPasswordDialog *dialog, 
                                                gchar *domain)
{
  HildonGetPasswordDialogPrivate *priv =
    HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
  gtk_label_set_text(priv->domainLabel, domain);
  
}

/**
 * hildon_get_password_dialog_set_title
 *        (HildonGetPasswordDialog *dialog, gchar *new_title) 
 * @dialog: the dialog
 * @new_ title: the text to be set as the dialog title.
 * 
 * sets the dialog title
 */
void hildon_get_password_dialog_set_title(HildonGetPasswordDialog *dialog,
					  gchar *new_title)

{
  /* FIXME: This method is completely useless, should be deprecated/removed */
  g_return_if_fail (new_title !=NULL);
  gtk_window_set_title(GTK_WINDOW(dialog), 
		       new_title);
}

/**
 * hildon_get_password_dialog_set_caption
 *        (HildonGetPasswordDialog *dialog, gchar *new_caption) 
 * @dialog: the dialog
 * @new_caption: the text to be set as the caption label.
 * 
 * sets the password entry field's neigbouring label.
 */


void hildon_get_password_dialog_set_caption(HildonGetPasswordDialog *dialog,
						  gchar *new_caption)
{
  

  HildonGetPasswordDialogPrivate *priv =
    HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);  
  g_return_if_fail (new_caption !=NULL);
  hildon_caption_set_label(priv->passwordEntry, new_caption);

}

/**
 * hildon_get_password_dialog_set_max_characters
 *        (HildonGetPasswordDialog *dialog, gint max_characters,
 *         gchar *error_ib_message) 
 * @dialog: the dialog
 * @max_characters: the maximum number of characters the password dialog
 * accepts.
 * @new_caption: the text to be set as the caption label.
 * 
 * sets the password entry field's neigbouring label.
 */

void hildon_get_password_dialog_set_max_characters (HildonGetPasswordDialog *dialog, gint max_characters )
{

  HildonGetPasswordDialogPrivate *priv =
    HILDON_GET_PASSWORD_DIALOG_GET_PRIVATE(dialog);  

  g_return_if_fail(max_characters >0);
  g_return_if_fail(dialog);

  /* Apply the given length to password entry */
  gtk_entry_set_max_length(GTK_ENTRY
			   (hildon_caption_get_control
			    (priv->passwordEntry)),
			   max_characters);

  /* Connect callback to show error banner if the limit is exceeded */
  g_signal_connect(GTK_ENTRY
		   (hildon_caption_get_control
		    (priv->passwordEntry)),
		   "invalid_input",
		   G_CALLBACK(_invalid_input),
		   NULL
		   );

}

static void _invalid_input(GtkWidget *widget, GtkInvalidInputType reason, 
			   gpointer user_data) 
{
  if (reason==GTK_INVALID_INPUT_MAX_CHARS_REACHED) {
    gtk_infoprint(GTK_WINDOW(widget), _(HILDON_GET_PASSWORD_DIALOG_MAX_CHARS));
  }

}
