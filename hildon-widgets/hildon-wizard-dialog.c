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
#include <hildon-widgets/hildon-defines.h>

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
static void hildon_wizard_dialog_response(HildonWizardDialog *
                                          wizard_dialog, gint response_id,
                                          gpointer data);
static void hildon_wizard_dialog_set_property(GObject * object,
                                              guint property_id,
                                              const GValue * value,
                                              GParamSpec * pspec);
static void hildon_wizard_dialog_get_property(GObject * object,
                                              guint property_id,
                                              GValue * value,
                                              GParamSpec * pspec);
static void hildon_wizard_dialog_finalize(GObject * object);


enum {
    PROP_WIZARD_NAME = 1,
    PROP_WIZARD_NOTEBOOK
};

struct _HildonWizardDialogPrivate {
    gchar *wizard_name;
    GtkNotebook *notebook;
    GtkBox *box;
    GtkImage *image;
};


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

    object_class->set_property = hildon_wizard_dialog_set_property;
    object_class->get_property = hildon_wizard_dialog_get_property;
    object_class->finalize = hildon_wizard_dialog_finalize;

    g_object_class_install_property(object_class, PROP_WIZARD_NAME,
        g_param_spec_string("wizard-name",
                            "Wizard Name",
                            "Set the HildonWizardDialog name",
                            NULL,
                            G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_WIZARD_NOTEBOOK,
        g_param_spec_object("wizard-notebook",
                            "Wizard Notebook",
                            "Set the GtkNotebook to be used in the "
                              "HildonWizardDialog",
                            GTK_TYPE_NOTEBOOK, G_PARAM_READWRITE));
}

static void hildon_wizard_dialog_finalize(GObject * object)
{
    g_free(HILDON_WIZARD_DIALOG(object)->priv->wizard_name);
    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void hildon_wizard_dialog_init(HildonWizardDialog * wizard_dialog)
{
    HildonWizardDialogPrivate *priv =
        G_TYPE_INSTANCE_GET_PRIVATE(wizard_dialog,
                                    HILDON_TYPE_WIZARD_DIALOG,
                                    HildonWizardDialogPrivate);
    GtkDialog *dialog = GTK_DIALOG(wizard_dialog);

    wizard_dialog->priv = priv;
    priv->box = GTK_BOX(gtk_hbox_new(FALSE, 0));
    priv->image = GTK_IMAGE(gtk_image_new_from_stock("hildon-wizard-icon",
                                                     HILDON_ICON_SIZE_WIDG_WIZARD));
    priv->notebook = NULL;
    priv->wizard_name = NULL;

    gtk_box_pack_start_defaults(GTK_BOX(dialog->vbox),
                                GTK_WIDGET(priv->box));
    gtk_box_pack_start_defaults(GTK_BOX(priv->box),
                                GTK_WIDGET(priv->image));

    gtk_dialog_add_button(dialog, _("Ecdg_bd_wizard_cancel"),
                          HILDON_WIZARD_DIALOG_CANCEL);
    gtk_dialog_add_button(dialog, _("Ecdg_bd_wizard_previous"),
                          HILDON_WIZARD_DIALOG_PREVIOUS);
    gtk_dialog_add_button(dialog, _("Ecdg_bd_wizard_next"),
                          HILDON_WIZARD_DIALOG_NEXT);
    gtk_dialog_add_button(dialog, _("Ecdg_bd_wizard_finish"),
                          HILDON_WIZARD_DIALOG_FINISH);

    g_signal_connect(G_OBJECT(dialog), "response",
                     G_CALLBACK(hildon_wizard_dialog_response), NULL);

    gtk_dialog_set_response_sensitive(GTK_DIALOG(wizard_dialog),
                                      HILDON_WIZARD_DIALOG_PREVIOUS,
                                      FALSE);
    gtk_dialog_set_response_sensitive(GTK_DIALOG(wizard_dialog),
                                      HILDON_WIZARD_DIALOG_FINISH,
                                      FALSE);

    hildon_wizard_dialog_create_title(wizard_dialog);
}


static void
hildon_wizard_dialog_set_property(GObject * object, guint property_id,
                                  const GValue * value, GParamSpec * pspec)
{
    HildonWizardDialogPrivate *priv = HILDON_WIZARD_DIALOG(object)->priv;

    switch (property_id) {
    case PROP_WIZARD_NAME:
        if (priv->wizard_name)
            g_free(priv->wizard_name);
        priv->wizard_name = g_strdup((gchar *) g_value_get_string(value));
        if (priv->notebook)
            hildon_wizard_dialog_create_title(HILDON_WIZARD_DIALOG
                                              (object));
        break;
    case PROP_WIZARD_NOTEBOOK:
        priv->notebook = GTK_NOTEBOOK(g_value_get_object(value));
        gtk_notebook_set_show_tabs(priv->notebook, FALSE);
        gtk_box_pack_start_defaults(GTK_BOX(priv->box),
                                    GTK_WIDGET(priv->notebook));
        if (priv->wizard_name)
            hildon_wizard_dialog_create_title(HILDON_WIZARD_DIALOG
                                              (object));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}


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
    /*
    const gchar *step = _("step");
    const gchar *welcome = _("Welcome");
    const gchar *wizard_completed = _("Wizard completed");
    */
    if (!notebook)
        return;
    pages = gtk_notebook_get_n_pages(notebook);
    current = gtk_notebook_get_current_page(priv->notebook);
    if (current < 0)
        current = 0;

    if (current == 0) {
        str = g_strdup_printf(_("Ecdg_ti_wizard_welcome"), 
                              priv->wizard_name, pages);
    } else if (current + 1 == pages) {
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
    /*
    if (current == 0)
        str = g_strdup_printf("%s %s %d/%d %s", priv->wizard_name, step,
                              current + 1, pages, welcome);
    else if (current + 1 == pages)
        str = g_strdup_printf("%s %s %d/%d %s",
                              priv->wizard_name, step, current + 1, pages,
                              wizard_completed);
    else {
        gchar *steps = (gchar *) gtk_notebook_get_tab_label_text(notebook,
            gtk_notebook_get_nth_page(notebook, current));
        str = g_strdup_printf("%s %s %d/%d: %s", priv->wizard_name, step,
                              current + 1, pages, steps);
    }
    */
    gtk_window_set_title(GTK_WINDOW(wizard_dialog), str);
    g_free(str);
}


static void hildon_wizard_dialog_response(HildonWizardDialog *
                                          wizard_dialog, gint response_id,
                                          gpointer data)
{
    HildonWizardDialogPrivate *priv = wizard_dialog->priv;
    GtkNotebook *notebook = priv->notebook;
    gint current = 0;
    gint last = gtk_notebook_get_n_pages(notebook) - 1;

    switch (response_id) {
    case HILDON_WIZARD_DIALOG_PREVIOUS:
        gtk_notebook_prev_page(notebook);
        current = gtk_notebook_get_current_page(notebook);
        if (current == 0)
        {
            gtk_dialog_set_response_sensitive
                (GTK_DIALOG(wizard_dialog),
                 HILDON_WIZARD_DIALOG_PREVIOUS,
                 FALSE);
            gtk_dialog_set_response_sensitive
                (GTK_DIALOG(wizard_dialog),
                 HILDON_WIZARD_DIALOG_FINISH,
                 FALSE);
        }
        if (current != last)
            gtk_dialog_set_response_sensitive(GTK_DIALOG(wizard_dialog),
                                              HILDON_WIZARD_DIALOG_NEXT,
                                              TRUE);

        g_signal_stop_emission_by_name(wizard_dialog, "response");
        break;
    case HILDON_WIZARD_DIALOG_NEXT:
        gtk_notebook_next_page(notebook);
        current = gtk_notebook_get_current_page(notebook);
        if (gtk_notebook_get_current_page(notebook) != 0)
        {
            gtk_dialog_set_response_sensitive
                (GTK_DIALOG(wizard_dialog),
                 HILDON_WIZARD_DIALOG_PREVIOUS,
                 TRUE);
            gtk_dialog_set_response_sensitive
                (GTK_DIALOG(wizard_dialog),
                 HILDON_WIZARD_DIALOG_FINISH,
                 TRUE);
        }
        if (current == last)
            gtk_dialog_set_response_sensitive(GTK_DIALOG(wizard_dialog),
                                              HILDON_WIZARD_DIALOG_NEXT,
                                              FALSE);
        g_signal_stop_emission_by_name(wizard_dialog, "response");
        break;
    case HILDON_WIZARD_DIALOG_CANCEL:
    case HILDON_WIZARD_DIALOG_FINISH:
        return;
    };

    if (current == last || current == 0)
        gtk_widget_show(GTK_WIDGET(priv->image));
    else
        gtk_widget_hide(GTK_WIDGET(priv->image));
    hildon_wizard_dialog_create_title(wizard_dialog);
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
