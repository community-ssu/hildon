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
 * SECTION:hildon-set-password-dialog
 * @short_description: A dialog used to set, change or remove a password
 * @see_also: #HildonGetPasswordDialog
 *
 * HildonSetPasswordDialog allows setting and changing a password. 
 * 
 * In Change mode: Dialog is used to change or remove an existing
 * password. Unselecting the check box dims the password fields below
 * it. If the dialog is accepted with 'OK' while the check box is
 * unselected, a Confirmation Note is shown.  If the Confirmation Note
 * Dialog is accepted with 'Remove', the password protection is removed.  
 * 
 * In Set mode: Set Password Dialog is used to define a password, or
 * change a password that cannot be removed.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <glib.h>

#include <errno.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>

#include <hildon-widgets/hildon-caption.h>
#include <hildon-widgets/gtk-infoprint.h>
#include <hildon-widgets/hildon-set-password-dialog.h>
#include <hildon-widgets/hildon-note.h>
#include <hildon-widgets/hildon-defines.h>

#include <libintl.h>
#define  _(String) dgettext(PACKAGE, String)
#define c_(String) dgettext("hildon-common-strings", String)

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
static void hildon_checkbox_toggled(GtkWidget * widget, gpointer dialog);

static void
hildon_set_password_response_change(GtkDialog * d, gint arg1, gpointer unused);
static void
hildon_set_password_response_set(GtkDialog * d, gint arg1, gpointer unused);

static void create_contents(HildonSetPasswordDialog *dialog);
static void hildon_set_password_set_property(GObject * object,
                                             guint prop_id,
                                             const GValue * value,
                                             GParamSpec * pspec);
static void hildon_set_password_get_property(GObject * object,
                                             guint prop_id, GValue * value,
                                             GParamSpec * pspec);

/* Private struct */
struct _HildonSetPasswordDialogPrivate {
    GtkWidget *checkboxCaption;
    GtkWidget *checkbox;

    GtkLabel *domainLabel;

    GtkWidget *pwd1stEntry;
    GtkWidget *pwd1stCaption;
    gchar *pwd1stCaption_string;

    GtkWidget *pwd2ndEntry;
    GtkWidget *pwd2ndCaption;
    gchar *pwd2ndCaption_string;

    GtkWidget *okButton;
    GtkWidget *cancelButton;

    gboolean protection;
};

enum {
    PROP_NONE = 0,
    PROP_DOMAIN,
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
    case PROP_DOMAIN:
        /* Update domain label to display new value */
        gtk_label_set_text(priv->domainLabel, g_value_get_string(value));
	break;
    case PROP_PASSWORD:
        /* Update password entry to display new value */
        gtk_entry_set_text(GTK_ENTRY(priv->pwd1stEntry), g_value_get_string(value));
        break;
    case PROP_HILDON_PASSWORD_DIALOG:
        /* Note this is a G_PARAM_CONSTRUCT_ONLY type property */
        priv->protection = g_value_get_boolean(value);

	/* We now have the necessary information to populate the dialog */
	create_contents(dialog);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_set_password_get_property(GObject    * object,
                                 guint        prop_id,
                                 GValue     * value,
                                 GParamSpec * pspec)
{
    HildonSetPasswordDialogPrivate *priv = NULL;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(object);
    
    switch (prop_id) {
    case PROP_DOMAIN:
        g_value_set_string(value, gtk_label_get_text(priv->domainLabel));
	break;
    case PROP_PASSWORD:
        g_value_set_string(value,
			   gtk_entry_get_text(GTK_ENTRY(priv->pwd1stEntry)));
        break;
    case PROP_HILDON_PASSWORD_DIALOG:
        g_value_set_boolean(value, priv->protection);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}


static void
create_contents(HildonSetPasswordDialog *dialog)
{
    HildonSetPasswordDialogPrivate *priv = NULL;

    GtkSizeGroup *group;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    priv->checkbox = NULL;

    /* Size group for labels */
    group = GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    /* Setup and pack domain label */
    priv->domainLabel = GTK_LABEL(gtk_label_new(NULL));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
		       GTK_WIDGET(priv->domainLabel), FALSE, FALSE, 0);
    gtk_widget_show(GTK_WIDGET(priv->domainLabel));

    if (priv->protection == TRUE) {
        /* Use Change Password Dialog strings */
        priv->pwd1stCaption_string = _(HILDON_SET_MODIFY_PASSWORD_DIALOG_PASSWORD);
        priv->pwd2ndCaption_string = _(HILDON_SET_MODIFY_PASSWORD_DIALOG_VERIFY_PASSWORD);

        /* Setup checkbox to enable/disable password protection */
        priv->checkbox = gtk_check_button_new();
        gtk_widget_show(priv->checkbox);
        priv->checkboxCaption = hildon_caption_new
	  (group,
	   _(HILDON_SET_MODIFY_PASSWORD_DIALOG_LABEL),
	   priv->checkbox,
	   NULL, HILDON_CAPTION_OPTIONAL);
	hildon_caption_set_separator(HILDON_CAPTION(priv->checkboxCaption), "");
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                           priv->checkboxCaption, TRUE, TRUE, 0);
        gtk_widget_show(priv->checkboxCaption);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->checkbox),
                                     TRUE);
        gtk_signal_connect(GTK_OBJECT(priv->checkbox), "toggled",
                           G_CALLBACK(hildon_checkbox_toggled), dialog);
 
	/* Setup appropriate response handler */
        g_signal_connect(G_OBJECT(dialog), "response",
                         G_CALLBACK(hildon_set_password_response_change),
                         NULL);
    } else {
        /* Use Set Password Dialog strings */
        priv->pwd1stCaption_string = _(HILDON_SET_PASSWORD_DIALOG_PASSWORD);
        priv->pwd2ndCaption_string = _(HILDON_SET_PASSWORD_DIALOG_VERIFY_PASSWORD);

	/* Setup appropriate response handler */
        g_signal_connect(G_OBJECT(dialog), "response",
                         G_CALLBACK(hildon_set_password_response_set),
                         NULL);
    }

    /* Create the password field */
    priv->pwd1stEntry = gtk_entry_new();
    gtk_widget_show(priv->pwd1stEntry);
    priv->pwd1stCaption = hildon_caption_new(group,
                                        priv->pwd1stCaption_string,
                                        priv->pwd1stEntry,
                                        NULL, HILDON_CAPTION_OPTIONAL);
    hildon_caption_set_separator(HILDON_CAPTION(priv->pwd1stCaption), "");
    gtk_entry_set_visibility(GTK_ENTRY(priv->pwd1stEntry), FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       priv->pwd1stCaption, TRUE, TRUE, 0);
    gtk_widget_show(priv->pwd1stCaption);

    /* Create the password verify field */
    priv->pwd2ndEntry = gtk_entry_new();
    gtk_widget_show(priv->pwd2ndEntry);
    priv->pwd2ndCaption = hildon_caption_new(group,
                                        priv->pwd2ndCaption_string,
                                        priv->pwd2ndEntry,
                                        NULL, HILDON_CAPTION_OPTIONAL);
    hildon_caption_set_separator(HILDON_CAPTION(priv->pwd2ndCaption), "");
    gtk_entry_set_visibility(GTK_ENTRY(priv->pwd2ndEntry), FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       priv->pwd2ndCaption, TRUE, TRUE, 0);
    gtk_widget_show(priv->pwd2ndCaption);

    /* Set dialog title */
    gtk_window_set_title(GTK_WINDOW(dialog),
                         _(priv->protection
                           ? HILDON_SET_MODIFY_PASSWORD_DIALOG_TITLE
                           : HILDON_SET_PASSWORD_DIALOG_TITLE));

    /* Create the OK/CANCEL buttons */
    priv->okButton = gtk_dialog_add_button
      (GTK_DIALOG(dialog),  _(priv->protection
			      ? HILDON_SET_MODIFY_PASSWORD_DIALOG_OK
			      : HILDON_SET_PASSWORD_DIALOG_OK),
       GTK_RESPONSE_OK);
    priv->cancelButton = gtk_dialog_add_button
      (GTK_DIALOG(dialog), _(priv->protection
			     ? HILDON_SET_MODIFY_PASSWORD_DIALOG_CANCEL
			     : HILDON_SET_PASSWORD_DIALOG_CANCEL),
       GTK_RESPONSE_CANCEL);
    
    gtk_widget_show(priv->okButton);
    gtk_widget_show(priv->cancelButton);

    /* Ensure group is freed when all its contents have been removed */
    g_object_unref(group);
}

static void
hildon_set_password_dialog_class_init(HildonSetPasswordDialogClass * class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(class);

    parent_class = g_type_class_peek_parent(class);

    /* Override virtual methods */
    object_class->set_property = hildon_set_password_set_property;
    object_class->get_property = hildon_set_password_get_property;

    /* Install new properties */
    g_object_class_install_property(object_class, 
		     PROP_DOMAIN, 
		     g_param_spec_string ("domain",
					  "Domain",
					  "Set Domain (content) for domain label.",
					  NULL,
					  G_PARAM_READWRITE));

    g_object_class_install_property(object_class, 
		     PROP_HILDON_PASSWORD_DIALOG, 
		     g_param_spec_boolean ("modify_protection",
					  "Password type",
					  "Set type to dialog",
					  TRUE, 
					  G_PARAM_CONSTRUCT_ONLY |
					  G_PARAM_READWRITE));

    g_object_class_install_property(object_class, 
		     PROP_PASSWORD, 
		     g_param_spec_string ("password",
					  "Password content",
					  "Set content to dialog",
					  "DEFAULT",
					  G_PARAM_READWRITE));

    /* Install private structure */
    g_type_class_add_private(class,
                             sizeof(HildonSetPasswordDialogPrivate));
}

static void
hildon_set_password_dialog_init(HildonSetPasswordDialog * dialog)
{
    /* Most of the initializations are done in create_contents()
       after the 'modify_protection' property has been set */

    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
}


/* We come here when response button is clicked and dialog 
   is used to change existing password. */
static void
hildon_set_password_response_change(GtkDialog * dialog, gint arg1,
                                    gpointer unused)
{
    GtkEntry *pwd1stEntry;
    GtkEntry *pwd2ndEntry;
    gchar *text1;
    gchar *text2;
    HildonNote *note;
    gint i;
    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    /* Password and verification */
    pwd1stEntry = GTK_ENTRY(gtk_bin_get_child
			    (GTK_BIN(priv->pwd1stCaption)));
    pwd2ndEntry = GTK_ENTRY(gtk_bin_get_child
			    (GTK_BIN(priv->pwd2ndCaption)));
    text1 = GTK_ENTRY(pwd1stEntry)->text;
    text2 = GTK_ENTRY(pwd2ndEntry)->text;

    /* User accepted the dialog */
    if (arg1 == GTK_RESPONSE_OK){
      /* Is the checkbox marked, so password protection is still in use? */  
      if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->checkbox))){
    /* Yes, Something is given as password as well? */
	if (text1[0] != '\0') {
	  if (strcmp (text1, text2) == 0) {
	    /* Passwords match, so accept change password */
	    priv->protection = TRUE;
	  } else if (text2[0] == '\0') {
	    /* Second field is empty, so show error, but don't clear fields */
	    g_signal_stop_emission_by_name(G_OBJECT(dialog),
					   "response");
	    gtk_infoprint (NULL,
			   c_(HILDON_SET_PASSWORD_DIALOG_MISMATCH));
	    gtk_widget_grab_focus(GTK_WIDGET(pwd2ndEntry));
	  } else {
	    /* Error: Passwords don't match, so start over */
	    g_signal_stop_emission_by_name(G_OBJECT(dialog),
					   "response");
	    gtk_entry_set_text(pwd1stEntry, "");
	    gtk_entry_set_text(pwd2ndEntry, "");
	    gtk_infoprint (NULL,
			   c_(HILDON_SET_PASSWORD_DIALOG_MISMATCH));
	    gtk_widget_grab_focus(GTK_WIDGET(pwd1stEntry));
	  }
	} else {
	  /* No, the password is empty */
	  g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
	  if (text2[0] == '\0') {
	    /* Error: Both fields are empty */
	    gtk_infoprint (NULL, c_(HILDON_SET_PASSWORD_DIALOG_EMPTY));
	  } else {
	    /* Error: Second field doesn't match
	       the empty first field, so start over */
	    gtk_infoprint (NULL, c_(HILDON_SET_PASSWORD_DIALOG_MISMATCH));
	    gtk_entry_set_text(pwd2ndEntry, "");
	  }
	  gtk_widget_grab_focus(GTK_WIDGET(pwd1stEntry));
	}
      } else {
        /* No, user wants to remove password protection. 
           Confirm remove password protection */
	note = HILDON_NOTE(hildon_note_new_confirmation
			   (GTK_WINDOW(dialog),
			    c_(HILDON_SET_PASSWORD_DIALOG_REMOVE_PROTECTION
			      )));
      
	hildon_note_set_button_texts
	  (HILDON_NOTE(note),
	   c_(HILDON_REMOVE_PROTECTION_CONFIRMATION_REMOVE), 
	   c_(HILDON_REMOVE_PROTECTION_CONFIRMATION_CANCEL));
	
        /* Display confirmation note */
	i = gtk_dialog_run(GTK_DIALOG(note));
	
	gtk_widget_destroy(GTK_WIDGET(note));
	
	if (i == GTK_RESPONSE_OK)
	  /* Remove password protection */
	  priv->protection = FALSE;
	else {
	  /* Remove password protection cancelled */
	  priv->protection = TRUE;
	  g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
	}
      }

    } else {
      /* Watch out for fading boolean values */
      priv->protection = TRUE;
    }
}

/* We come here when response button is clicked and dialog 
   is used to set new password. */
static void
hildon_set_password_response_set(GtkDialog * dialog, gint arg1,
                                 gpointer unused)
{
    GtkEntry *pwd1stEntry;
    GtkEntry *pwd2ndEntry;
    gchar *text1;
    gchar *text2;

    HildonSetPasswordDialogPrivate *priv;

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    /* Password and confirmation */
    pwd1stEntry = GTK_ENTRY(gtk_bin_get_child
			    (GTK_BIN(priv->pwd1stCaption)));
    pwd2ndEntry = GTK_ENTRY(gtk_bin_get_child
			    (GTK_BIN(priv->pwd2ndCaption)));
    text1 = GTK_ENTRY(pwd1stEntry)->text;
    text2 = GTK_ENTRY(pwd2ndEntry)->text;

    if (arg1 == GTK_RESPONSE_OK) {
        /* User provided something for password? */
        if (text1[0] != '\0') {
            if (strcmp (text1, text2) == 0) {
	        /* Passwords match, so accept set password */
                priv->protection = TRUE;
            } else if (text2[0] == '\0') {
                /* Second field is empty, so show error,
                   but don't clear the fields */
                g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
                gtk_infoprint (NULL, c_(HILDON_SET_PASSWORD_DIALOG_MISMATCH));
                gtk_widget_grab_focus (GTK_WIDGET (priv->pwd2ndEntry));
            } else {
                /* Error: Passwords don't match, so start over */
                g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
	        gtk_entry_set_text(pwd1stEntry, "");
		gtk_entry_set_text(pwd2ndEntry, "");
	        gtk_infoprint (NULL, c_(HILDON_SET_PASSWORD_DIALOG_MISMATCH));
                gtk_widget_grab_focus(GTK_WIDGET(priv->pwd1stEntry));
	    }
        } else {
          /* First field is empty */
          g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
	  if (text2[0] == '\0') {
	    /* Error: Both fields are empty */
	    gtk_infoprint (NULL, c_(HILDON_SET_PASSWORD_DIALOG_EMPTY));
	  } else {
	    /* Error: Second field doesn't match
	       the empty first field, so start over */
	    gtk_infoprint (NULL, c_(HILDON_SET_PASSWORD_DIALOG_MISMATCH));
	    gtk_entry_set_text(pwd2ndEntry, "");
	  }
	  gtk_widget_grab_focus(GTK_WIDGET(pwd1stEntry));
        }
    } else { 
        /* Watch out for fading boolean values */
        priv->protection = FALSE;
    }    	  
}

static void hildon_checkbox_toggled(GtkWidget * widget, gpointer dialog)
{
    HildonSetPasswordDialogPrivate *priv =
      HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
    gboolean active;

    /* If the user enabled/disabled the password protection feature
       we enable/disable password entries accordingly */
    active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gtk_widget_set_sensitive(GTK_WIDGET(priv->pwd1stEntry), active);
    gtk_widget_set_sensitive(GTK_WIDGET(priv->pwd2ndEntry), active);
}

/* Public functions */

/**
 * hildon_set_password_dialog_get_type:
 *
 * Returns GType for HildonPasswordDialog as produced by
 * g_type_register_static().
 *
 * Returns: HildonSetPasswordDialog type
 */
GType hildon_set_password_dialog_get_type(void)
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
 * Constructs a new HildonSetPasswordDialog.
 *
 * Returns: a new #GtkWidget of type HildonSetPasswordDialog
 */

GtkWidget *hildon_set_password_dialog_new(GtkWindow * parent,
                                          gboolean modify_protection)
{
    return hildon_set_password_dialog_new_with_default(parent, "",
						       modify_protection);
}

/**
 * hildon_set_password_dialog_new_with_default:
 * @parent: parent window; can be NULL
 * @password: a default password to be shown in password field
 * @modify_protection: TRUE creates a new change password dialog and FALSE
 *                     creates a new set password dialog 
 * 
 * Same as #hildon_set_password_dialog_new, but with a default password
 * in password field.
 *
 * Returns: a new #GtkWidget of type HildonSetPasswordDialog
 */

GtkWidget *hildon_set_password_dialog_new_with_default
                                         (GtkWindow *parent,
					  const gchar *password,
                                          gboolean modify_protection)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_SET_PASSWORD_DIALOG,
				     "modify_protection", modify_protection,
				     "password", password, NULL);

    if (parent != NULL) {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }
   
    return dialog;
}

/**
 * hildon_set_password_dialog_get_password:
 * @dialog: pointer to HildonSetPasswordDialog
 * 
 * Returns current password.
 *
 * Returns: changed password ( if the dialog is successfully 
 * accepted with 'OK' ( and when the check box is 'ON' ( in Change Password
 * Dialog ))
 */
const gchar
    *hildon_set_password_dialog_get_password(HildonSetPasswordDialog *
                                             dialog)
{
    HildonSetPasswordDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_SET_PASSWORD_DIALOG(dialog), NULL);

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    return GTK_ENTRY(priv->pwd1stEntry)->text;
}

/**
 * hildon_set_password_dialog_get_protected:
 * @dialog: pointer to HildonSetPasswordDialog
 * 
 * Returns the protection mode.
 *
 * Returns: password protection mode ( TRUE when the protection is
 *               'ON' and FALSE when the protection is 'OFF' )
 */
gboolean
hildon_set_password_dialog_get_protected(HildonSetPasswordDialog * dialog)
{
    HildonSetPasswordDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_SET_PASSWORD_DIALOG(dialog), FALSE);

    priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);

    return priv->protection;
}

/**
 * hildon_set_password_dialog_set_domain(GtkWidget *dialog, 
 * @dialog: the dialog
 * @domain: the domain or some other descriptive text to be set
 * 
 * Sets the optional descriptive text.
 */

void hildon_set_password_dialog_set_domain(HildonSetPasswordDialog *dialog, 
					   const gchar *domain)
{
  HildonSetPasswordDialogPrivate *priv = NULL;

  g_return_if_fail(HILDON_IS_SET_PASSWORD_DIALOG(dialog));

  priv = HILDON_SET_PASSWORD_DIALOG_GET_PRIVATE(dialog);
  gtk_label_set_text(priv->domainLabel, domain);  
}
