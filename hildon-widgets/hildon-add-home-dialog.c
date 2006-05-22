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

static const gchar *hildon_add_home_dialog_get_old_name(HildonAddHomeDialog * dialog);
static void hildon_add_home_dialog_set_old_name(HildonAddHomeDialog * dialog, const gchar* name);
static void hildon_add_home_dialog_set_new_name(HildonAddHomeDialog * dialog, const gchar* name);
static void hildon_add_home_dialog_create_widgets(HildonAddHomeDialog * dialog, gboolean new_isrename);

static void
hildon_add_home_dialog_get_property(GObject * object,
				    guint prop_id,
			            GValue * value,
			            GParamSpec * pspec);
static void
hildon_add_home_dialog_set_property(GObject * object,
				    guint prop_id,
			            const GValue * value,
			            GParamSpec * pspec);
static void
hildon_add_home_dialog_class_init(HildonAddHomeDialogClass * class);
static void hildon_add_home_dialog_init(HildonAddHomeDialog * dialog);

/* private struct */

struct _HildonAddHomeDialogPrivate {
    GtkWidget *desc_label;
    GtkWidget *name_entry; /* Used when isrename */
    GtkWidget *name_label; /* Used when !isrename */
    GtkWidget *caption_name_entry; /* Used when isrename */
    GtkWidget *new_name_entry;
    GtkWidget *caption_new_name_entry;
    GtkSizeGroup *size_group;
    GtkWidget *okButton;
    GtkWidget *cancelButton;
    gboolean isrename;
};

enum
{
  PROP_0,

  PROP_NAME,
  PROP_NEW_NAME,
};

/* Private functions */

static void
hildon_add_home_dialog_class_init(HildonAddHomeDialogClass * klass)
{
    GObjectClass * gobject_class = G_OBJECT_CLASS(klass);
    parent_class = g_type_class_peek_parent(klass);

    gobject_class->set_property = hildon_add_home_dialog_set_property;
    gobject_class->get_property = hildon_add_home_dialog_get_property;

    /**
     * HildonAddHomeDialog:name
     * 
     * The name
     */
    g_object_class_install_property(gobject_class,
				    PROP_NAME,
				    g_param_spec_string ("name",
							("Name"),
							("The name."),
							NULL,
							G_PARAM_READWRITE));

    /**
     * HildonAddHomeDialog:new_name
     * 
     * The new name
     */
    g_object_class_install_property(gobject_class,
				    PROP_NEW_NAME,
				    g_param_spec_string ("new_name",
							("New Name"),
							("The new name."),
							NULL,
							G_PARAM_READWRITE));

    g_type_class_add_private(klass, sizeof(HildonAddHomeDialogPrivate));
}

static void hildon_add_home_dialog_init(HildonAddHomeDialog * dialog)
{
    HildonAddHomeDialogPrivate *priv;

    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);
    priv->name_entry = NULL;
    priv->name_label = NULL;
    priv->caption_name_entry = NULL;
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

    priv->size_group = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);


    /* add description text (only shown when isrename==true */
    priv->desc_label = gtk_label_new(_("ckdg_ib_link_exists"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                           priv->desc_label, FALSE, FALSE, 0);

    /* We don't use the hildon_caption_new() C convenience function, because it demands a non-NULL child widget (via gtk_container_add()). */
    priv->caption_name_entry = GTK_WIDGET( g_object_new( HILDON_TYPE_CAPTION, "size_group", priv->size_group, "label", "addtoHome_editor_caption", "status", HILDON_CAPTION_OPTIONAL, NULL) );
    gtk_widget_show(priv->caption_name_entry);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), priv->caption_name_entry,
                       FALSE, FALSE, 0);


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

static void
hildon_add_home_dialog_get_property (GObject    *object,
			      guint       prop_id,
			      GValue     *value,
			      GParamSpec *pspec)
{
    HildonAddHomeDialog * dialog = HILDON_ADD_HOME_DIALOG(object);
    HildonAddHomeDialogPrivate *priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);
  
    switch(prop_id)
    {
        case PROP_NAME:
        {
            const gchar* name = NULL;
            if(priv->isrename && priv->name_label)
                name = gtk_entry_get_text(GTK_ENTRY(priv->name_label));
            else if(!(priv->isrename) && priv->name_entry)
                name = gtk_entry_get_text(GTK_ENTRY(priv->name_entry));

            g_value_set_string (value, name);
            break;
        }
        case PROP_NEW_NAME:
        {
            const gchar* new_name = NULL;
            if(priv->isrename && priv->new_name_entry)
                new_name = gtk_entry_get_text(GTK_ENTRY(priv->new_name_entry));

            g_value_set_string (value, new_name);
            break;
        }
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
hildon_add_home_dialog_set_window_title (HildonAddHomeDialog * dialog)
{
    HildonAddHomeDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    /* Set up the window title */
    if (priv->isrename)
        gtk_window_set_title(GTK_WINDOW(dialog), _("ckdg_ti_rename_link"));
    else
        gtk_window_set_title(GTK_WINDOW(dialog), "addtoHome_dialog_title");
}


static void
hildon_add_home_dialog_set_property (GObject      *object,
			      guint         prop_id,
			      const GValue *value,
			      GParamSpec   *pspec)
{
    HildonAddHomeDialog * dialog = HILDON_ADD_HOME_DIALOG(object);
    HildonAddHomeDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    switch (prop_id)
    {
    case PROP_NAME:
        hildon_add_home_dialog_set_old_name (dialog, g_value_get_string (value));
        break;
    case PROP_NEW_NAME:
        hildon_add_home_dialog_set_new_name (dialog, g_value_get_string (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
        break;
    }
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
     HildonAddHomeDialog *dialog =
       HILDON_ADD_HOME_DIALOG(g_object_new
			      (HILDON_TYPE_ADD_HOME_DIALOG, "name", name, "new_name", new_name, NULL));

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

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

    if (priv->isrename && priv->new_name_entry)
        return gtk_entry_get_text(GTK_ENTRY(priv->new_name_entry));
    else if(priv->name_entry)
        return gtk_entry_get_text(GTK_ENTRY(priv->name_entry));
    else
        return NULL;
}

static const gchar *hildon_add_home_dialog_get_old_name(HildonAddHomeDialog * dialog)
{
    HildonAddHomeDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog), NULL);
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    if (priv->isrename && priv->name_label)
        return gtk_label_get_text(GTK_LABEL(priv->name_label));
    else if(!priv->isrename && priv->name_entry)
        return gtk_entry_get_text(GTK_ENTRY(priv->name_entry));
    else
        return NULL;
}

static void hildon_add_home_dialog_set_old_name(HildonAddHomeDialog * dialog, const gchar* name)
{
    HildonAddHomeDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    hildon_add_home_dialog_create_widgets(dialog, priv->isrename);

    /* Set text in the widgets: */
    if (priv->isrename)
        gtk_label_set_text(GTK_LABEL(priv->name_label), name);
    else
        gtk_entry_set_text(GTK_ENTRY(priv->name_entry), name);
}

static void hildon_add_home_dialog_set_new_name(HildonAddHomeDialog * dialog, const gchar* new_name)
{
    HildonAddHomeDialogPrivate *priv;
    gchar* name = NULL;
    gboolean new_isrename = FALSE; 

    g_return_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    /* We get the old name, in case we need to set it again in a new widget. */
    name = g_strdup( hildon_add_home_dialog_get_old_name(dialog) );
    new_isrename = (new_name != NULL);

    hildon_add_home_dialog_create_widgets(dialog, new_isrename);


    /* Set text in the widgets: */
    if(priv->isrename)
    {
        gtk_entry_set_text(GTK_ENTRY(priv->new_name_entry), new_name);
        gtk_label_set_text(GTK_LABEL(priv->name_label), (name) ? name : "");
    }
    else
    {
        gtk_entry_set_text(GTK_ENTRY(priv->name_entry), (name) ? name : "");
    }

    if(name)
      g_free(name);
}

static void hildon_add_home_dialog_create_widgets(HildonAddHomeDialog * dialog, gboolean new_isrename)
{
    HildonAddHomeDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    /* Create and destroy widgets, depending on the mode.
     * Note that we are making a small speed sacrifice (recreating the widgets) 
     * in exchange for memory (creating all widgets and just hiding the ones we don't use.)
     */

    if(priv->isrename && !new_isrename)
    {
        /* Remove widgets that were used for isrename mode,
           because we don't need them anymore. */
        if(priv->new_name_entry)
        {
            gtk_widget_destroy(priv->new_name_entry);
            priv->new_name_entry = NULL;
        }

        if(priv->caption_new_name_entry)
        {
            gtk_widget_destroy(priv->caption_new_name_entry);
            priv->caption_new_name_entry = NULL;
        }

        if(priv->name_label) /* A label */
        {
           gtk_widget_destroy(priv->name_label);
           priv->name_label = NULL;
        }

        if(priv->name_entry) /* A box with label inside */
        {
           gtk_widget_destroy(priv->name_entry);
           priv->name_entry = NULL;
        }
    }
    else if(!priv->isrename && new_isrename)
    {
        /* Remove widgets that were used only for !isrename mode,
           because we don't need them anymore. */
        if(priv->name_entry) /* An entry. */
        {
           gtk_widget_destroy(priv->name_entry);
           priv->name_entry = NULL;
        }
    }

    priv->isrename = new_isrename;

    if(priv->isrename)
    {
        /* Create widgets needed for isrename mode: */

        /* Create Entry (in a Caption) for the new name, and pack it into the dialog: */
        if(!priv->new_name_entry)
        {
            priv->new_name_entry = gtk_entry_new();
            gtk_widget_show(priv->new_name_entry);
        }

        if(!priv->caption_new_name_entry)
        {
            priv->caption_new_name_entry = hildon_caption_new(priv->size_group, _("ckdg_fi_rename_name"),
                                           priv->new_name_entry, NULL,
                                           HILDON_CAPTION_OPTIONAL);
            gtk_widget_show(priv->caption_new_name_entry);
            gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), GTK_WIDGET(priv->caption_new_name_entry),
                               FALSE, FALSE, 0);
        }

        /* add description text */
        if(priv->desc_label)
          gtk_widget_show(priv->desc_label);

        /* Create Label (in a Box) for the existing name: */
        if(!priv->name_entry)
        {
            priv->name_label = gtk_label_new(NULL);
            gtk_widget_show(priv->name_label);

            priv->name_entry = gtk_hbox_new(FALSE, 0);
            gtk_widget_show(priv->name_entry);
            gtk_box_pack_start(GTK_BOX(priv->name_entry), priv->name_label, FALSE, FALSE,
                           0);
            gtk_container_add(GTK_CONTAINER(priv->caption_name_entry), GTK_WIDGET(priv->name_entry));
        }
    }
    else
    {
        if(priv->desc_label)
          gtk_widget_hide(priv->desc_label);

        /* Create widgets needed for !isrename mode: */
        if(!priv->name_entry)
        {
            priv->name_entry = gtk_entry_new();
            gtk_widget_show(priv->name_entry);
            gtk_container_add(GTK_CONTAINER(priv->caption_name_entry), GTK_WIDGET(priv->name_entry));
        }
    }

    hildon_add_home_dialog_set_window_title(dialog);

    priv->isrename = new_isrename;
}

void hildon_add_home_dialog_set_name(HildonAddHomeDialog * dialog, const gchar* name)
{
    HildonAddHomeDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_ADD_HOME_DIALOG(dialog));
    priv = HILDON_ADD_HOME_DIALOG_GET_PRIVATE(dialog);

    hildon_add_home_dialog_create_widgets(dialog, priv->isrename);

    /* Set text in the widgets: */
    if (priv->isrename)
        gtk_entry_set_text(GTK_ENTRY(priv->new_name_entry), name);
    else
        gtk_label_set_text(GTK_LABEL(priv->name_entry), name);
}

