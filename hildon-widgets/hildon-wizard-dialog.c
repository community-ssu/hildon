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

#include <gtk/gtkdialog.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkbox.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkbutton.h>
#include <hildon-widgets/hildon-defines.h>
#include <hildon-widgets/hildon-marshalers.h>

#include "hildon-wizard-dialog.h"

#include <libintl.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _(String) dgettext(PACKAGE, String)

static GtkDialogClass *parent_class;

static void hildon_wizard_dialog_class_init(HildonWizardDialogClass *
                                            wizard_dialog_class);
static void hildon_wizard_dialog_init(HildonWizardDialog * wizard_dialog);
static void
hildon_wizard_dialog_create_title(HildonWizardDialog * wizard_dialog);
static void direction_button_clicked(GtkWidget *widget,
                                     HildonWizardDialog *wizard_dialog);
static void hildon_wizard_dialog_set_property(GObject * object,
                                              guint property_id,
                                              const GValue * value,
                                              GParamSpec * pspec);
static void hildon_wizard_dialog_get_property(GObject * object,
                                              guint property_id,
                                              GValue * value,
                                              GParamSpec * pspec);
static void hildon_wizard_dialog_finalize(GObject * object);
static void hildon_wizard_dialog_update_state(HildonWizardDialog *
					      wizard_dialog);
static gboolean hildon_wizard_dialog_page_change (HildonWizardDialog *
						  wizard_dialog,
						  gint response,
						  gpointer user_data);


enum {
    PROP_WIZARD_NAME = 1,
    PROP_WIZARD_NOTEBOOK
};

enum {
  PAGE_CHANGE = 0,

  LAST_SIGNAL
};

struct _HildonWizardDialogPrivate {
    gchar *wizard_name;
    GtkNotebook *notebook;
    GtkBox *box;
    GtkWidget *image;
    GtkWidget *previous_button;
    GtkWidget *next_button;
};

static guint HildonWizardDialog_signal[LAST_SIGNAL] = {0};

GType hildon_wizard_dialog_get_type(void)
{
    static GType wizard_dialog_type = 0;

    if (!wizard_dialog_type) {
        static const GTypeInfo wizard_dialog_info = {
            sizeof(HildonWizardDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_wizard_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonWizardDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_wizard_dialog_init,
        };
        wizard_dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                                    "HildonWizardDialog",
                                                    &wizard_dialog_info,
                                                    0);
    }
    return wizard_dialog_type;
}


static void
hildon_wizard_dialog_class_init(HildonWizardDialogClass *
                                wizard_dialog_class)
{
    GObjectClass *object_class = G_OBJECT_CLASS(wizard_dialog_class);

    parent_class = g_type_class_peek_parent(wizard_dialog_class);

    g_type_class_add_private(wizard_dialog_class,
                             sizeof(HildonWizardDialogPrivate));

    /* Override virtual methods */
    object_class->set_property = hildon_wizard_dialog_set_property;
    object_class->get_property = hildon_wizard_dialog_get_property;
    object_class->finalize = hildon_wizard_dialog_finalize;

    wizard_dialog_class->page_change = hildon_wizard_dialog_page_change;
    
    /**
     * HildonWizardDialog:wizard-name:
     *
     * The name of the wizard.
     */
    g_object_class_install_property(object_class, PROP_WIZARD_NAME,
        g_param_spec_string("wizard-name",
                            "Wizard Name",
                            "The name of the HildonWizardDialog",
                            NULL,
                            G_PARAM_READWRITE));

    /**
     * HildonWizardDialog:wizard-notebook:
     *
     * The notebook object, which is used by the HildonWizardDialog.
     */
    g_object_class_install_property(object_class, PROP_WIZARD_NOTEBOOK,
        g_param_spec_object("wizard-notebook",
                            "Wizard Notebook",
                            "GtkNotebook object to be used in the "
                              "HildonWizardDialog",
                            GTK_TYPE_NOTEBOOK, G_PARAM_READWRITE));
    /**
     * HildonWizardDialog:page:
     * 
     * Gets emitted when the current page is about to change.
     * It provides buttons response code as a parameter.
     * Users handler can return TRUE to prevent page changing
     * (e.g. in case of invalid data) or FALSE to change page.
     * If FALSE is returned than default handler is called that
     * change the page (hildon_wizard_dialog_page_change).
     */ 
    HildonWizardDialog_signal[PAGE_CHANGE] = 
    			    g_signal_new(
			    "page_change", HILDON_TYPE_WIZARD_DIALOG,
			    G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET 
			    (HildonWizardDialogClass, page_change),
			    g_signal_accumulator_true_handled, NULL, 
			    _hildon_marshal_BOOLEAN__INT,
			    G_TYPE_BOOLEAN, 1, G_TYPE_INT);
    
}

static void hildon_wizard_dialog_finalize(GObject * object)
{
    g_free(HILDON_WIZARD_DIALOG(object)->priv->wizard_name);
    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void hildon_wizard_dialog_init(HildonWizardDialog * wizard_dialog)
{
    /* Initialize private structure for faster member access */
    HildonWizardDialogPrivate *priv =
        G_TYPE_INSTANCE_GET_PRIVATE(wizard_dialog,
                                    HILDON_TYPE_WIZARD_DIALOG,
                                    HildonWizardDialogPrivate);
    GtkDialog *dialog = GTK_DIALOG(wizard_dialog);

    /* Init internal widgets */
    GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
    gtk_dialog_set_has_separator(dialog, FALSE);
    wizard_dialog->priv = priv;
    priv->box = GTK_BOX(gtk_hbox_new(FALSE, 0));
    priv->image = gtk_image_new_from_icon_name("qgn_widg_wizard",
					       HILDON_ICON_SIZE_WIDG_WIZARD);

    /* Default values for user provided properties */
    priv->notebook = NULL;
    priv->wizard_name = NULL;

    /* Build wizard layout */
    gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox),
                                GTK_WIDGET(priv->box));
    gtk_box_pack_start_defaults(GTK_BOX(priv->box), GTK_WIDGET(vbox));
    gtk_box_pack_start(GTK_BOX(vbox),GTK_WIDGET(priv->image), FALSE, FALSE, 0);

    /* Initialize cancel button */
    gtk_dialog_add_button(dialog, _("Ecdg_bd_wizard_cancel"),
                          HILDON_WIZARD_DIALOG_CANCEL);

    /* Initialize previous button */
    priv->previous_button =
        gtk_button_new_with_label(_("Ecdg_bd_wizard_previous"));
    gtk_box_pack_start(GTK_BOX(dialog->action_area),
                       priv->previous_button, FALSE, TRUE, 0);
    g_signal_connect(priv->previous_button, "clicked",
                     G_CALLBACK(direction_button_clicked), wizard_dialog);

    /* Initialize next button */
    priv->next_button =
        gtk_button_new_with_label(_("Ecdg_bd_wizard_next"));
    gtk_box_pack_start(GTK_BOX(dialog->action_area),
                       priv->next_button, FALSE, TRUE, 0);
    g_signal_connect(priv->next_button, "clicked",
                     G_CALLBACK(direction_button_clicked), wizard_dialog);

    /* Initialize finish button */
    gtk_dialog_add_button(dialog, _("Ecdg_bd_wizard_finish"),
                          HILDON_WIZARD_DIALOG_FINISH);

    /* Set initial button states */
    gtk_dialog_set_response_sensitive(GTK_DIALOG(wizard_dialog),
                                      HILDON_WIZARD_DIALOG_FINISH,
                                      FALSE);
    gtk_widget_set_sensitive(priv->previous_button, FALSE);

    hildon_wizard_dialog_create_title(wizard_dialog);
}


static void
hildon_wizard_dialog_set_property(GObject * object, guint property_id,
                                  const GValue * value, GParamSpec * pspec)
{
    HildonWizardDialogPrivate *priv = HILDON_WIZARD_DIALOG(object)->priv;

    switch (property_id) {
    case PROP_WIZARD_NAME:

        /* Set new wizard name. This name will appear in titlebar */
        if (priv->wizard_name)
            g_free(priv->wizard_name);
        priv->wizard_name = g_strdup((gchar *) g_value_get_string(value));

        /* We need notebook in order to create title, since page information
           is used in title generation */
        if (priv->notebook)
            hildon_wizard_dialog_create_title(HILDON_WIZARD_DIALOG
                                              (object));
        break;
    case PROP_WIZARD_NOTEBOOK:
        priv->notebook = GTK_NOTEBOOK(g_value_get_object(value));

        /* Set the default properties for the notebook (disable tabs,
         * and remove borders) to make it look like a nice wizard widget */
        gtk_notebook_set_show_tabs(priv->notebook, FALSE);
        gtk_notebook_set_show_border(priv->notebook, FALSE);
        gtk_box_pack_start_defaults(GTK_BOX(priv->box),
                                    GTK_WIDGET(priv->notebook));

        /* Update dialog title to reflact current page stats etc */        
        if (priv->wizard_name)
	{
            hildon_wizard_dialog_create_title(HILDON_WIZARD_DIALOG
                                              (object));
	    hildon_wizard_dialog_update_state(HILDON_WIZARD_DIALOG
                                              (object));
	}
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

/*
 * Creates the title of the dialog taking into account the current 
 * page of the notebook.
 */
static void
hildon_wizard_dialog_get_property(GObject * object, guint property_id,
                                  GValue * value, GParamSpec * pspec)
{
    HildonWizardDialogPrivate *priv = HILDON_WIZARD_DIALOG(object)->priv;

    switch (property_id) {
    case PROP_WIZARD_NAME:
        g_value_set_string(value, priv->wizard_name);
        break;
    case PROP_WIZARD_NOTEBOOK:
        g_value_set_object(value, priv->notebook);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}


static void
hildon_wizard_dialog_create_title(HildonWizardDialog * wizard_dialog)
{
    gint pages, current;
    gchar *str = NULL;
    HildonWizardDialogPrivate *priv = wizard_dialog->priv;
    GtkNotebook *notebook = priv->notebook;

    if (!notebook)
        return;

    /* Get page information, we'll need that when creating title */
    pages = gtk_notebook_get_n_pages(notebook);
    current = gtk_notebook_get_current_page(priv->notebook);
    if (current < 0)
        current = 0;

    /* the welcome title on the initial page */
    if (current == 0) {
        str = g_strdup_printf(_("Ecdg_ti_wizard_welcome"), 
                              priv->wizard_name, pages);
    } else if (current + 1 == pages) {

        /* Other pages contain step information. 
           FIXME: why the follwing two blocks are identical */
        gchar *steps = (gchar *) gtk_notebook_get_tab_label_text(notebook,
            gtk_notebook_get_nth_page(notebook, current));
        str = g_strdup_printf(_("Ecdg_ti_wizard_step"), 
                              priv->wizard_name, current + 1, pages, steps);
    } else {
        gchar *steps = (gchar *) gtk_notebook_get_tab_label_text(notebook,
            gtk_notebook_get_nth_page(notebook, current));
        str = g_strdup_printf(_("Ecdg_ti_wizard_step"), 
                              priv->wizard_name, current + 1, pages, steps);
    }

    /* Update the dialog to display the generated title */
    gtk_window_set_title(GTK_WINDOW(wizard_dialog), str);
    g_free(str);
}

/* This function is called when next/previous buttons are clicked.
   We dim buttons based on our new page.

   FIXME: Why on earth these buttons are not response buttos like
          other buttons in action area?? Now they cannot be conveniently
          dimmed.
*/
static void
direction_button_clicked(GtkWidget *widget, HildonWizardDialog *wizard_dialog)
{
    HildonWizardDialogPrivate *priv = wizard_dialog->priv;
    gboolean rb;
	
    /* Previous button clicked, we probably are on the first page now */
    if (widget == priv->previous_button)
    {
	g_signal_emit_by_name (wizard_dialog,
			       "page_change",
			       HILDON_WIZARD_DIALOG_PREVIOUS,
			       &rb,
			       NULL);
    }
    else /* handle the "Next" button */
    {
	g_signal_emit_by_name (wizard_dialog,
			       "page_change",
			       HILDON_WIZARD_DIALOG_NEXT,
			       &rb,
			       NULL);
    }
}

/* Updates the state of buttons and wizard icon
 * based on the current page.
 */
static void
hildon_wizard_dialog_update_state(HildonWizardDialog *wizard_dialog)
{
	HildonWizardDialogPrivate *priv = wizard_dialog->priv;
	GtkNotebook *notebook = priv->notebook;
	gint current =  gtk_notebook_get_current_page(notebook);
	gint last = gtk_notebook_get_n_pages(notebook) - 1;

	if (current==0)
	{
		/* first page */
		gtk_widget_set_sensitive(priv->previous_button, FALSE);
		
		if (current!=last)
		{
			/* multi-page wizard */
			gtk_widget_set_sensitive(priv->next_button, TRUE);
			gtk_dialog_set_response_sensitive
				(GTK_DIALOG(wizard_dialog),
				 HILDON_WIZARD_DIALOG_FINISH,
				 FALSE);
		}
		else
		{
			/* one page wizard */
			gtk_widget_set_sensitive(priv->next_button, FALSE);
			gtk_dialog_set_response_sensitive
				(GTK_DIALOG(wizard_dialog),
				 HILDON_WIZARD_DIALOG_FINISH,
				 TRUE);
		}
	}
	else if (current==last)
	{
		/* last page */
		gtk_widget_set_sensitive(priv->next_button, FALSE);

		gtk_widget_set_sensitive(priv->previous_button, TRUE);
		gtk_dialog_set_response_sensitive
			(GTK_DIALOG(wizard_dialog),
			 HILDON_WIZARD_DIALOG_FINISH,
			 TRUE);
	}
	else
	{
		/* in the middle */
		gtk_widget_set_sensitive(priv->next_button, TRUE);

		gtk_widget_set_sensitive(priv->previous_button, TRUE);
		gtk_dialog_set_response_sensitive
			(GTK_DIALOG(wizard_dialog),
			 HILDON_WIZARD_DIALOG_FINISH,
			 TRUE);
	}
	
    /* We show the default image on first and last pages */
    if (current == last || current == 0)
        gtk_widget_show(GTK_WIDGET(priv->image));
    else
        gtk_widget_hide(GTK_WIDGET(priv->image));
	
}

/* Deafult handler for page_change event.
 * it is just change wizard-notebook current page and update buttons state
 */
static gboolean
hildon_wizard_dialog_page_change (HildonWizardDialog *wizard_dialog,
				  gint response,
				  gpointer user_data)
{
    HildonWizardDialogPrivate *priv = wizard_dialog->priv;
    GtkNotebook *notebook = priv->notebook;

    /* Previous button clicked, we probably are on the first page now */
    if (response == HILDON_WIZARD_DIALOG_PREVIOUS)
    {
        gtk_notebook_prev_page(notebook);
    }
    else if (response == HILDON_WIZARD_DIALOG_NEXT)/* handle the "Next" button */
    {
        /* Move forward */
        gtk_notebook_next_page(notebook);
    }

    
    /* New page number may appear in the title, update it */
    hildon_wizard_dialog_create_title(wizard_dialog);

    /* Update buttons state and wizard icon*/
    hildon_wizard_dialog_update_state (wizard_dialog);
    
    return TRUE;
}

/**
 * hildon_wizard_dialog_new:
 * @parent: A #GtkWindow
 * @wizard_name: The name of dialog
 * @notebook: The notebook to be shown on the dialog
 *
 * Creates a new #HildonWizardDialog
 *
 * Return value: A new HildonWizardDialog
 */
GtkWidget *hildon_wizard_dialog_new(GtkWindow * parent,
                                    const char *wizard_name,
                                    GtkNotebook * notebook)
{
    GtkWidget *widget;

    g_return_val_if_fail(GTK_IS_NOTEBOOK(notebook), NULL);

    widget = GTK_WIDGET(g_object_new(HILDON_TYPE_WIZARD_DIALOG,
                                     "wizard-name", wizard_name,
                                     "wizard-notebook", notebook, NULL));
    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(widget), parent);

    return widget;
}
