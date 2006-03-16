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

/* FIXME: should be moved to hildon-fm (file handling, depends on gnome-vfs) */

/**
 * SECTION:hildon-insert-object-dialog
 * @short_description: A dialog that enables the user to embed an object 
 * into rich text
 *
 * #HildonInsertObjectDialog is a dialog that enables the user to insert a
 * new or existing object into a Multi-line (Rich Text) Editor as an
 * embedded object.
 *
 * FIXME: deprecated widget. Shall not be used anywhere.
 * 
 */ 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtkmain.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtkdialog.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtksizegroup.h>
#include <gtk/gtkbox.h>
#include <string.h>

#include <libintl.h>

#include <hildon-insert-object-dialog.h>
#include <hildon-widgets/hildon-caption.h>

#define _(String) dgettext(PACKAGE, String)

#define MAX_ERR_MSG 256

#define HILDON_INSERT_OBJECT_DIALOG_WIDTH 300
#define HILDON_INSERT_OBJECT_DIALOG_HEIGHT 100

#define PROJECT_TEMPLATE_DIR "/tmp"
#define RUN_AT_TERMINATION 0


static GtkDialogClass *parent_class;

struct _HildonInsertObjectDialogPrivate {
    GtkButton *insertBtn;
    GtkButton *existingBtn;
    GtkButton *cancelBtn;
    GtkEntry *entry;
    GtkLabel *label;
    GtkComboBox *combo;
    GtkSizeGroup *group;

    HildonCaption *comboCap;
    HildonCaption *entryCap;
    HildonCaption *locationCap;

    GnomeVFSDirectoryHandle *dir;
    char *mimetype;

    guint keysnooper;
};

#define HILDON_INSERT_OBJECT_DIALOG_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
      HILDON_TYPE_INSERT_OBJECT_DIALOG, \
      HildonInsertObjectDialogPrivate) )

static void
hildon_insert_object_dialog_class_init(HildonInsertObjectDialogClass *
                                       class);
static void hildon_insert_object_dialog_init(HildonInsertObjectDialog *
                                             dialog);
static void hildon_insert_object_dialog_finalize(GObject * obj_self);
static gint hildon_insert_object_dialog_key_snooper(GtkWidget * gadget,
                                                    GdkEventKey * event,
                                                    GtkWidget * widget);
static void hildon_insert_object_dialog_set_property(GObject * object,
                                     guint prop_id,
                                     const GValue * value,
                                     GParamSpec * pspec);
static void hildon_insert_object_dialog_get_property(GObject * object,
                                     guint prop_id,
                                     GValue * value, GParamSpec * pspec);

enum {
    PROP_0,
    PROP_NAME,
    PROP_MIME_TYPE
};

static gboolean hildon_insert_object_foo(gpointer data)
{
    gnome_vfs_shutdown();
    g_message("GnomeVFS shutdown succeed!\n");
    return FALSE;
}

/* Private functions */
static void
hildon_insert_object_dialog_class_init(HildonInsertObjectDialogClass *
                                       class)
{

    GObjectClass *object_class = G_OBJECT_CLASS(class);

    parent_class = g_type_class_peek_parent(class);
    g_type_class_add_private(class,
                             sizeof(HildonInsertObjectDialogPrivate));

    object_class->finalize = hildon_insert_object_dialog_finalize;
    object_class->set_property = hildon_insert_object_dialog_set_property;
    object_class->get_property = hildon_insert_object_dialog_get_property;

    g_object_class_install_property(object_class, PROP_NAME,
        g_param_spec_string("name",
                            "Name",
                            "The text in name field",
                            "", G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_MIME_TYPE,
        g_param_spec_string("mime-type",
                            "Mime-Type",
                            "The mime type selected in the combobox",
                            GNOME_VFS_MIME_TYPE_UNKNOWN,
			    G_PARAM_READABLE));
}

static void hildon_insert_object_dialog_finalize(GObject * obj_self)
{
    HildonInsertObjectDialogPrivate *priv;
    HildonInsertObjectDialog *dlg;

    g_return_if_fail(HILDON_IS_INSERT_OBJECT_DIALOG(obj_self));

    dlg = HILDON_INSERT_OBJECT_DIALOG(obj_self);
    priv = HILDON_INSERT_OBJECT_DIALOG_GET_PRIVATE(dlg);

    gtk_key_snooper_remove(priv->keysnooper);

    if (G_OBJECT_CLASS(parent_class)->finalize)
        G_OBJECT_CLASS(parent_class)->finalize(obj_self);
}


static void
hildon_insert_object_dialog_init(HildonInsertObjectDialog * dialog)
{

    GnomeVFSResult result;
    gchar *dir_path = "";
    GnomeVFSFileInfo *file = gnome_vfs_file_info_new();
    GList *mimelist = NULL;
    GtkTreeModel *combo_tree = NULL;
    GtkTreeIter combo_iter;
    gboolean valid = FALSE;


    dialog->priv = HILDON_INSERT_OBJECT_DIALOG_GET_PRIVATE(dialog);
    gtk_dialog_set_has_separator(GTK_DIALOG(dialog), FALSE);

    gtk_window_set_title(GTK_WINDOW(dialog), _("ckdg_ti_insert_insert_objec"));

    dialog->priv->group =
        GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));

    dialog->priv->insertBtn =
        GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                         _("ckdg_bd_insert_new"),
                                         GTK_RESPONSE_OK));

    dialog->priv->existingBtn =
        GTK_BUTTON(gtk_dialog_add_button
                   (GTK_DIALOG(dialog), _("ckdg_bd_insert_existing"),
                    HILDON_RESPONSE_INSERT_EXISTING));

    dialog->priv->cancelBtn =
        GTK_BUTTON(gtk_dialog_add_button(GTK_DIALOG(dialog),
                                         _("ckdg_bd_insert_cancel"),
                                         GTK_RESPONSE_CANCEL));

    dialog->priv->label = GTK_LABEL(gtk_label_new(NULL));
    dialog->priv->locationCap =
        HILDON_CAPTION(hildon_caption_new(dialog->priv->group,
                                          _("Location"),
                                          GTK_WIDGET(dialog->priv->label),
                                          NULL, HILDON_CAPTION_OPTIONAL));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(dialog->priv->locationCap), FALSE, FALSE,
                       0);

    dialog->priv->entry = GTK_ENTRY(gtk_entry_new());
    dialog->priv->entryCap =
        HILDON_CAPTION(hildon_caption_new(dialog->priv->group,
                                          _("Name"),
                                          GTK_WIDGET(dialog->priv->entry),
                                          NULL, HILDON_CAPTION_OPTIONAL));

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(dialog->priv->entryCap), FALSE, FALSE,
                       0);

    dialog->priv->combo = GTK_COMBO_BOX(gtk_combo_box_new_text());

    dialog->priv->comboCap =
        HILDON_CAPTION(hildon_caption_new(dialog->priv->group,
                                          _("Mime type"),
                                          GTK_WIDGET(dialog->priv->combo),
                                          NULL, HILDON_CAPTION_OPTIONAL));

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),
                       GTK_WIDGET(dialog->priv->comboCap), FALSE, FALSE,
                       0);

    dialog->priv->mimetype = (char *) GNOME_VFS_MIME_TYPE_UNKNOWN;

    /* Initialize gnome-vfs */
    if (gnome_vfs_initialized() == FALSE) {
        gtk_quit_add(RUN_AT_TERMINATION, hildon_insert_object_foo, NULL);

        if (gnome_vfs_init() == FALSE) {
            g_error("GnomeVFS can not be run");
            return;
        } else
            g_message("GnomeVFS init!");
    } else
        g_message("GnomeVFS initialized!");


    /* Read registered mime types into combobox */
    mimelist = gnome_vfs_get_registered_mime_types();

    while (mimelist) {
        if (gnome_vfs_mime_get_description((char *) (mimelist->data)) !=
            NULL)
            gtk_combo_box_append_text
                (dialog->priv->combo,
                 gnome_vfs_mime_get_description
                     ((char *) (mimelist->data)));
        else
            gtk_combo_box_append_text(dialog->priv->combo,
                                      (char *) (mimelist->data));

        mimelist = mimelist->next;
    }

    gnome_vfs_mime_registered_mime_type_list_free(mimelist);

    /* Read directory path into label and first file into entry */
    /* dir_path = g_strconcat( (const char *)g_get_current_dir(),
       PROJECT_TEMPLATE_DIR, NULL); */
    dir_path = g_strconcat(g_getenv((const char *) "PWD"),
                           PROJECT_TEMPLATE_DIR, NULL);

    result = gnome_vfs_directory_open(&(dialog->priv->dir), dir_path,
                                      GNOME_VFS_FILE_INFO_DEFAULT |
                                      GNOME_VFS_FILE_INFO_GET_MIME_TYPE);
    if (result != GNOME_VFS_OK) {
        g_warning("Could not read project templates directory\n");
        gtk_label_set_label(dialog->priv->label,
                            "Could not read project templates directory");
        return;
    } else {
        gtk_label_set_label(dialog->priv->label, (gchar *) dir_path);

        while (gnome_vfs_directory_read_next(dialog->priv->dir, file) ==
               GNOME_VFS_OK) {
            if (file->name != NULL
                && file->type == GNOME_VFS_FILE_TYPE_REGULAR) {
                gtk_entry_set_text(dialog->priv->entry,
                                   (gchar *) file->name);

                combo_tree = gtk_combo_box_get_model(dialog->priv->combo);
                valid =
                    gtk_tree_model_get_iter_first(combo_tree, &combo_iter);

                while (valid) {
                    gchar *str_data;

                    gtk_tree_model_get(combo_tree, &combo_iter, 0,
                                       &str_data, -1);

                    if (strcmp(str_data,
                               gnome_vfs_mime_get_description
                               ((char *) file->mime_type)) == 0) {
                        dialog->priv->mimetype = (char *) file->mime_type;
                        gtk_combo_box_set_active_iter(dialog->priv->combo,
                                                      &combo_iter);
                        g_free(str_data);
                        break;
                    }
                    g_free(str_data);
                    valid =
                        gtk_tree_model_iter_next(combo_tree, &combo_iter);
                }

                break;
            }

        }
    }


    if (gnome_vfs_directory_close(dialog->priv->dir) != GNOME_VFS_OK)
        g_warning("Could not close the project templates directory\n");
    else
        g_message("Gnome VFS dir closed \n");

    g_free(dir_path);

    dialog->priv->keysnooper =
        (guint) (gtk_key_snooper_install
                 ((GtkKeySnoopFunc)
                  (hildon_insert_object_dialog_key_snooper),
                  GTK_WIDGET(dialog)));

    gtk_window_resize(GTK_WINDOW(dialog),
                      HILDON_INSERT_OBJECT_DIALOG_WIDTH,
                      HILDON_INSERT_OBJECT_DIALOG_HEIGHT);

    gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);
    gtk_widget_show_all(GTK_DIALOG(dialog)->action_area);

    gnome_vfs_file_info_unref(file);

}

static gint hildon_insert_object_dialog_key_snooper(GtkWidget * gadget,
                                                    GdkEventKey * event,
                                                    GtkWidget * widget)
{
    if (event->type == GDK_KEY_RELEASE) {
        switch (event->keyval) {
        case GDK_I:
        case GDK_i:
            gtk_dialog_response(GTK_DIALOG(widget),
                                HILDON_RESPONSE_INSERT_EXISTING);
            break;
            /* case GDK_C: case GDK_c: gtk_dialog_response(
               GTK_DIALOG(widget), GTK_RESPONSE_CANCEL ); break; */
        }
    }
    return FALSE;
}


/* Public functions */
GType hildon_insert_object_dialog_get_type(void)
{
    static GType dialog_type = 0;

    if (!dialog_type) {
        static const GTypeInfo dialog_info = {
            sizeof(HildonInsertObjectDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_insert_object_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonInsertObjectDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_insert_object_dialog_init
        };

        dialog_type = g_type_register_static(GTK_TYPE_DIALOG,
                                             "HildonInsertObjectDialog",
                                             &dialog_info, 0);
    }
    return dialog_type;

}

/**
 * hildon_insert_object_dialog_new:
 * @parent: the parent window of the dialog
 *
 * Creates a new #HildonInsertObjectDialog widget.
 *
 * Returns: the newly created #HildonInsertObjectDialog
 */
GtkWidget *hildon_insert_object_dialog_new(GtkWindow * parent)
{
    GtkWidget *self = g_object_new(HILDON_TYPE_INSERT_OBJECT_DIALOG, NULL);

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(self), parent);

    return GTK_WIDGET(self);
}

/**
 * hildon_insert_object_dialog_get_name:
 * @dialog: the dialog
 *
 * Gets the text in name field of the file dialog. 
 * This specifies the object to be inserted.
 *
 * Returns: a pointer to the name string
 */
const gchar *hildon_insert_object_dialog_get_name(HildonInsertObjectDialog
                                                  * dialog)
{

    g_return_val_if_fail(HILDON_IS_INSERT_OBJECT_DIALOG(dialog), NULL);

    return gtk_entry_get_text(dialog->priv->entry);
}

/**
 * hildon_insert_object_dialog_get_mime_type:
 * @dialog: the dialog
 *
 * Gets the mime type selected in the combobox.
 *
 * Returns: a pointer to the mime type string, must not be freed
 */
const gchar
    *hildon_insert_object_dialog_get_mime_type(HildonInsertObjectDialog *
                                               dialog)
{
    /* GtkTreeIter iter; gchar *return_string; GtkTreeModel *tree_model;

       tree_model = gtk_combo_box_get_model(dialog->priv->combo);

       if(gtk_combo_box_get_active_iter(dialog->priv->combo,&iter)){
       gtk_tree_model_get (tree_model, &iter, 0, &return_string, -1);
       return return_string; } else { return NULL; } */


    return dialog->priv->mimetype;
}

static void
hildon_insert_object_dialog_set_property(GObject * object,
                         guint prop_id,
                         const GValue * value, GParamSpec * pspec)
{
    HildonInsertObjectDialog *dialog;

    dialog = HILDON_INSERT_OBJECT_DIALOG(object);

    switch (prop_id) {
    case PROP_NAME:
       gtk_entry_set_text(dialog->priv->entry, g_value_get_string(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void
hildon_insert_object_dialog_get_property(GObject * object,
                         guint prop_id, GValue * value, GParamSpec * pspec)
{
    HildonInsertObjectDialog *dialog;

    dialog = HILDON_INSERT_OBJECT_DIALOG(object);

    switch (prop_id) {
    case PROP_NAME:
        g_value_set_string(value, gtk_entry_get_text(dialog->priv->entry));
        break;
    case PROP_MIME_TYPE:
        g_value_set_string(value, dialog->priv->mimetype);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

