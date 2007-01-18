/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
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
 * SECTION:hildon-find-toolbar
 * @short_description: A special toolbar to be used with HildonAppView
 * @see_also: #HildonAppView
 *
 * HildonFindToolbar is a predefined toolbar for text searching purpose. 
 * It contains a GtkListStore which has the text items that the user has 
 * searched. But once the application is terminated, or HildonFindToolbar 
 * is trashed. Programmer is responsible for getting the GtkListStore through 
 * property "list", if he/she wants to use the information in the future.
 * And through the same property, programmer is able to set the GtkListStore. 
 * Note, once the search button is pressed, string in the GtkComboxEntry is 
 * automatically added to the existing model, unless it is empty.
 */    

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        "hildon-find-toolbar.h"
#include                                        "hildon-defines.h"
#include                                        <gdk/gdkkeysyms.h>
#include                                        <gtk/gtklabel.h>
#include                                        <gtk/gtkentry.h>
#include                                        <gtk/gtkbutton.h>
#include                                        <gtk/gtktoolbutton.h>
#include                                        <gtk/gtktoolitem.h>
#include                                        <gtk/gtkcomboboxentry.h>
#include                                        <gtk/gtkseparatortoolitem.h>
#include                                        <string.h>
#include                                        <libintl.h>
#include                                        "hildon-find-toolbar-private.h"

#define                                         _(String) \
                                                dgettext(PACKAGE, String)

/*same define as gtkentry.c as entry will further handle this*/

#define                                         MAX_SIZE G_MAXUSHORT

#define                                         FIND_LABEL_XPADDING 6

#define                                         FIND_LABEL_YPADDING 0

static GtkTreeModel*
hildon_find_toolbar_get_list_model              (HildonFindToolbarPrivate *priv);

static GtkEntry*
hildon_find_toolbar_get_entry                   (HildonFindToolbarPrivate *priv);

static gboolean
hildon_find_toolbar_filter                      (GtkTreeModel *model,
                                                 GtkTreeIter *iter,
                                                 gpointer self);

static void
hildon_find_toolbar_apply_filter                (HildonFindToolbar *self,  
                                                 GtkTreeModel *model);

static void
hildon_find_toolbar_get_property                (GObject *object,
                                                 guint prop_id,
                                                 GValue *value,
                                                 GParamSpec *pspec);

static void
hildon_find_toolbar_set_property                (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec);

static gboolean
hildon_find_toolbar_find_string                 (HildonFindToolbar *self,
                                                 GtkTreeIter *iter,
                                                 gint column,
                                                 const gchar *string);

static gboolean
hildon_find_toolbar_history_append              (HildonFindToolbar *self,
                                                 gpointer data);

static void
hildon_find_toolbar_emit_search                 (GtkButton *button, 
                                                 gpointer self);

static void
hildon_find_toolbar_emit_close                  (GtkButton *button, 
                                                 gpointer self);

static void
hildon_find_toolbar_emit_invalid_input          (GtkEntry *entry, 
                                                 GtkInvalidInputType type, 
                                                 gpointer self);

static void
hildon_find_toolbar_entry_activate              (GtkWidget *widget,
                                                 gpointer user_data);

static void
hildon_find_toolbar_class_init                  (HildonFindToolbarClass *klass);

static void
hildon_find_toolbar_init                        (HildonFindToolbar *self);

enum
{
    SEARCH = 0,
    CLOSE,
    INVALID_INPUT,
    HISTORY_APPEND,

    LAST_SIGNAL
};

enum
{
    PROP_0,
    PROP_LABEL = 1,
    PROP_PREFIX,
    PROP_LIST,
    PROP_COLUMN,
    PROP_MAX,
    PROP_HISTORY_LIMIT
};

static guint                                    HildonFindToolbar_signal [LAST_SIGNAL] = {0};

GType G_GNUC_CONST
hildon_find_toolbar_get_type                    (void)
{
    static GType find_toolbar_type = 0;

    if (! find_toolbar_type) {
        static const GTypeInfo find_toolbar_info = {
            sizeof(HildonFindToolbarClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_find_toolbar_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonFindToolbar),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_find_toolbar_init,
        };
        find_toolbar_type = g_type_register_static (GTK_TYPE_TOOLBAR,
                "HildonFindToolbar",
                &find_toolbar_info, 0);
    }

    return find_toolbar_type;
}

static GtkTreeModel*
hildon_find_toolbar_get_list_model              (HildonFindToolbarPrivate *priv)
{
    GtkTreeModel *filter_model =
        gtk_combo_box_get_model (GTK_COMBO_BOX (priv->entry_combo_box));

    return filter_model == NULL ? NULL :
        gtk_tree_model_filter_get_model (GTK_TREE_MODEL_FILTER (filter_model));
}

static GtkEntry*
hildon_find_toolbar_get_entry                   (HildonFindToolbarPrivate *priv)
{
    return GTK_ENTRY (gtk_bin_get_child (GTK_BIN (priv->entry_combo_box)));
}

static gboolean
hildon_find_toolbar_filter                      (GtkTreeModel *model,
                                                 GtkTreeIter *iter,
                                                 gpointer self)
{
    GtkTreePath *path;
    const gint *indices;
    gint n;
    gint limit;
    gint total;

    total = gtk_tree_model_iter_n_children (model, NULL);
    g_object_get (self, "history_limit", &limit, NULL);
    path = gtk_tree_model_get_path (model, iter);
    indices = gtk_tree_path_get_indices (path);

    /* set the row's index, list store has only one level */
    n = indices [0];
    gtk_tree_path_free (path);

    /*if the row is among the latest "history_limit" additions of the 
     * model, then we show it */
    if( (total - limit <= n) && (n < total) )
        return TRUE;
    else
        return FALSE;
}

static void
hildon_find_toolbar_apply_filter                (HildonFindToolbar *self,  
                                                 GtkTreeModel *model)
{
    GtkTreeModel *filter;
    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (self);
    g_assert (priv);

    /* Create a filter for the given model. Its only purpose is to hide
       the oldest entries so only "history_limit" entries are visible. */
    filter = gtk_tree_model_filter_new (model, NULL);

    gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER(filter), 
            hildon_find_toolbar_filter,
            self, NULL);

    gtk_combo_box_set_model (GTK_COMBO_BOX (priv->entry_combo_box), filter);

    /* ComboBox keeps the only needed reference to the filter */
    g_object_unref (filter);
}

static void
hildon_find_toolbar_get_property                (GObject *object,
                                                 guint prop_id,
                                                 GValue *value,
                                                 GParamSpec *pspec)
{
    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (object);
    g_assert (priv);

    const gchar *string;
    gint c_n, max_len;

    switch (prop_id)
    {
        case PROP_LABEL:
            string = gtk_label_get_text (GTK_LABEL (priv->label));
            g_value_set_string (value, string);
            break;

        case PROP_PREFIX:
            string = gtk_entry_get_text (hildon_find_toolbar_get_entry(priv));
            g_value_set_string (value, string);
            break;

        case PROP_LIST:
            g_value_set_object (value, hildon_find_toolbar_get_list_model(priv));
            break;

        case PROP_COLUMN:
            c_n = gtk_combo_box_entry_get_text_column (priv->entry_combo_box);
            g_value_set_int (value, c_n);
            break;

        case PROP_MAX:
            max_len = gtk_entry_get_max_length (hildon_find_toolbar_get_entry(priv));
            g_value_set_int (value, max_len);
            break;

        case PROP_HISTORY_LIMIT:
            g_value_set_int (value, priv->history_limit);
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static void
hildon_find_toolbar_set_property                (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec)
{
    HildonFindToolbar *self = HILDON_FIND_TOOLBAR(object);
    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (object);
    g_assert (priv);

    GtkTreeModel *model;
    const gchar *string;

    switch (prop_id)
    {
        case PROP_LABEL:
            string = g_value_get_string (value);   
            gtk_label_set_text (GTK_LABEL (priv->label), string);
            break;

        case PROP_PREFIX:
            string = g_value_get_string (value);
            gtk_entry_set_text (hildon_find_toolbar_get_entry(priv), string);
            break;

        case PROP_LIST:
            model = GTK_TREE_MODEL (g_value_get_object(value));
            hildon_find_toolbar_apply_filter (self, model);
            break;

        case PROP_COLUMN:
            gtk_combo_box_entry_set_text_column (priv->entry_combo_box,
                    g_value_get_int (value));
            break;

        case PROP_MAX:
            gtk_entry_set_max_length (hildon_find_toolbar_get_entry(priv),
                    g_value_get_int (value));
            break;

        case PROP_HISTORY_LIMIT:
            priv->history_limit = g_value_get_int (value);

            /* Re-apply the history limit to the model. */
            model = hildon_find_toolbar_get_list_model (priv);
            if (model != NULL)
            {
                /* Note that refilter function doesn't update the status of the
                   combobox popup arrow, so we'll just recreate the filter. */
                hildon_find_toolbar_apply_filter (self, model);

                if (gtk_combo_box_entry_get_text_column (priv->entry_combo_box) == -1)
                {
                    /* FIXME: This is only for backwards compatibility, although
                       probably nothing actually relies on it. The behavior was only
                       an accidental side effect of original code */
                    gtk_combo_box_entry_set_text_column (priv->entry_combo_box, 0);
                }
            }
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
            break;
    }
}

static gboolean
hildon_find_toolbar_find_string                 (HildonFindToolbar *self,
                                                 GtkTreeIter *iter,
                                                 gint column,
                                                 const gchar *string)
{
    GtkTreeModel *model = NULL;
    gchar *old_string;
    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (self);
    g_assert (priv);

    model = hildon_find_toolbar_get_list_model (priv);

    if (gtk_tree_model_get_iter_first (model, iter))
    {
        do {
            gtk_tree_model_get (model, iter, column, &old_string, -1);
            if (old_string != NULL && strcmp (string, old_string) == 0)
            {
                /* Found it */
                return TRUE;
            }
        } while (gtk_tree_model_iter_next (model, iter));
    }

    return FALSE;
}

static gboolean
hildon_find_toolbar_history_append              (HildonFindToolbar *self,
                                                 gpointer data) 
{
    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (self);
    g_assert (priv);

    gchar *string;
    gint column = 0;
    GtkTreeModel *model = NULL;
    GtkListStore *list = NULL;
    GtkTreeIter iter;
    gboolean self_create = FALSE;

    g_object_get (self, "prefix", &string, NULL);

    if (*string == '\0')
    {
        /* empty prefix, ignore */
        g_free (string);
        return TRUE;
    }


    /* If list store is set, get it */
    model = hildon_find_toolbar_get_list_model(priv);
    if(model != NULL)
    {
        list = GTK_LIST_STORE (model);
        g_object_get(self, "column", &column, NULL);

        if (column < 0)
        {
            /* Column number is -1 if "column" property hasn't been set but
               "list" property is. */
            g_free (string);
            return TRUE;
        }

        /* Latest string is always the first one in list. If the string
           already exists, remove it so there are no duplicates in list. */
        if (hildon_find_toolbar_find_string (self, &iter, column, string))
            gtk_list_store_remove (list, &iter);
    }
    else
    {
        /* No list store set. Create our own. */
        list = gtk_list_store_new (1, G_TYPE_STRING);
        model = GTK_TREE_MODEL (list);
        self_create = TRUE;
    }

    /* Add the string to first in list */
    gtk_list_store_append (list, &iter);
    gtk_list_store_set (list, &iter, column, string, -1);

    if(self_create)
    {
        /* Add the created list to ComboBoxEntry */
        hildon_find_toolbar_apply_filter (self, model);
        /* ComboBoxEntry keeps the only needed reference to this list */
        g_object_unref (list);

        /* Set the column only after ComboBoxEntry's model is set
           in hildon_find_toolbar_apply_filter() */
        g_object_set (self, "column", 0, NULL);
    }
    else
    {
        /* Refilter to get the oldest entry hidden from history */
        gtk_tree_model_filter_refilter (GTK_TREE_MODEL_FILTER(
                    gtk_combo_box_get_model (GTK_COMBO_BOX(priv->entry_combo_box))));
    }

    g_free (string);

    return TRUE;
}

static void
hildon_find_toolbar_emit_search                 (GtkButton *button, 
                                                 gpointer self)
{
    gboolean rb;

    /* Clicked search button. Perform search and add search prefix to history */
    g_signal_emit_by_name(self, "search", NULL);
    g_signal_emit_by_name(self, "history_append", &rb, NULL);
}

static void
hildon_find_toolbar_emit_close                  (GtkButton *button, 
                                                 gpointer self)
{
    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (self);
    g_assert (priv);

    GtkWidget *entry = gtk_bin_get_child (GTK_BIN (priv->entry_combo_box));
    if (GTK_WIDGET_HAS_FOCUS (entry))
    {
        hildon_gtk_im_context_hide (GTK_ENTRY (entry)->im_context);
    }

    /* Clicked close button */
    g_signal_emit_by_name (self, "close", NULL);
}

static void
hildon_find_toolbar_emit_invalid_input          (GtkEntry *entry, 
                                                 GtkInvalidInputType type, 
                                                 gpointer self)
{
    if(type == GTK_INVALID_INPUT_MAX_CHARS_REACHED)
        g_signal_emit_by_name (self, "invalid_input", NULL);
}

static void
hildon_find_toolbar_entry_activate              (GtkWidget *widget,
                                                 gpointer user_data)
{
    GtkWidget *find_toolbar = GTK_WIDGET (user_data);
    gboolean rb;  

    /* NB#40936 stop focus from moving to next widget */
    g_signal_stop_emission_by_name (widget, "activate");

    g_signal_emit_by_name (find_toolbar, "search", NULL);
    g_signal_emit_by_name (find_toolbar, "history_append", &rb, NULL);
}

static void
hildon_find_toolbar_class_init                  (HildonFindToolbarClass *klass)
{
    GObjectClass *object_class;

    g_type_class_add_private (klass, sizeof (HildonFindToolbarPrivate));

    object_class = G_OBJECT_CLASS(klass);

    object_class->get_property = hildon_find_toolbar_get_property;
    object_class->set_property = hildon_find_toolbar_set_property;

    klass->history_append = (gpointer) hildon_find_toolbar_history_append;

    g_object_class_install_property(object_class, PROP_LABEL, 
            g_param_spec_string("label", 
                "Label", "Displayed name for"
                " find-toolbar",
                _("ecdg_ti_find_toolbar_label"),
                G_PARAM_READWRITE |
                G_PARAM_CONSTRUCT));

    g_object_class_install_property(object_class, PROP_PREFIX, 
            g_param_spec_string("prefix", 
                "Prefix", "Search string", NULL,
                G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_LIST,
            g_param_spec_object("list",
                "List"," GtkListStore model where "
                "history list is kept",
                GTK_TYPE_LIST_STORE,
                G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_COLUMN,
            g_param_spec_int("column",
                "Column", "Column number in GtkListStore "
                "where history list strings are kept",
                0, G_MAXINT,
                0, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_MAX,
            g_param_spec_int("max_characters",
                "Maximum number of characters",
                "Maximum number of characters "
                "in search string",
                0, MAX_SIZE,
                0, G_PARAM_READWRITE |
                G_PARAM_CONSTRUCT));

    g_object_class_install_property(object_class, PROP_HISTORY_LIMIT,
            g_param_spec_int("history_limit",
                "Maximum number of history items",
                "Maximum number of history items "
                "in search combobox",
                0, G_MAXINT,
                5, G_PARAM_READWRITE |
                G_PARAM_CONSTRUCT));

    /**
     * HildonFindToolbar::search:
     * @toolbar: the toolbar which received the signal
     * 
     * Gets emitted when the find button is pressed.
     */ 
    HildonFindToolbar_signal[SEARCH] = 
        g_signal_new(
                "search", HILDON_TYPE_FIND_TOOLBAR,
                G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET 
                (HildonFindToolbarClass, search),
                NULL, NULL, gtk_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

    /**
     * HildonFindToolbar::close:
     * @toolbar: the toolbar which received the signal
     * 
     * Gets emitted when the close button is pressed.
     */ 
    HildonFindToolbar_signal[CLOSE] = 
        g_signal_new(
                "close", HILDON_TYPE_FIND_TOOLBAR,
                G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET 
                (HildonFindToolbarClass, close),
                NULL, NULL, gtk_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

    /**
     * HildonFindToolbar::invalid-input:
     * @toolbar: the toolbar which received the signal
     * 
     * Gets emitted when the maximum search prefix length is reached and
     * user tries to type more.
     */ 
    HildonFindToolbar_signal[INVALID_INPUT] = 
        g_signal_new(
                "invalid_input", HILDON_TYPE_FIND_TOOLBAR,
                G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET 
                (HildonFindToolbarClass, invalid_input),
                NULL, NULL, gtk_marshal_VOID__VOID,
                G_TYPE_NONE, 0);

    /**
     * HildonFindToolbar::history-append:
     * @toolbar: the toolbar which received the signal
     * 
     * Gets emitted when the current search prefix should be added to history.
     */ 
    HildonFindToolbar_signal[HISTORY_APPEND] = 
        g_signal_new(
                "history_append", HILDON_TYPE_FIND_TOOLBAR,
                G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET 
                (HildonFindToolbarClass, history_append),
                g_signal_accumulator_true_handled, NULL, 
                gtk_marshal_BOOLEAN__VOID,
                G_TYPE_BOOLEAN, 0);
}

static void
hildon_find_toolbar_init                        (HildonFindToolbar *self)
{
    GtkToolItem *label_container;
    GtkToolItem *entry_combo_box_container;

    HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (self);
    g_assert (priv);

    /* Create the label */
    priv->label = gtk_label_new (_("ecdg_ti_find_toolbar_label"));

    gtk_misc_set_padding (GTK_MISC (priv->label), FIND_LABEL_XPADDING,
            FIND_LABEL_YPADDING);

    label_container = gtk_tool_item_new ();
    gtk_container_add (GTK_CONTAINER (label_container), 
            priv->label);

    gtk_widget_show_all (GTK_WIDGET (label_container));
    gtk_toolbar_insert (GTK_TOOLBAR (self), label_container, -1);

    /* ComboBoxEntry for search prefix string / history list */
    priv->entry_combo_box = GTK_COMBO_BOX_ENTRY (gtk_combo_box_entry_new ());
    g_signal_connect (hildon_find_toolbar_get_entry(priv),
            "invalid_input", 
            G_CALLBACK(hildon_find_toolbar_emit_invalid_input), self);

    entry_combo_box_container = gtk_tool_item_new ();

    gtk_tool_item_set_expand (entry_combo_box_container, TRUE);
    gtk_container_add (GTK_CONTAINER (entry_combo_box_container),
            GTK_WIDGET (priv->entry_combo_box));
    gtk_widget_show_all(GTK_WIDGET (entry_combo_box_container));
    gtk_toolbar_insert (GTK_TOOLBAR (self), entry_combo_box_container, -1);
    g_signal_connect (hildon_find_toolbar_get_entry (priv),
            "activate",
            G_CALLBACK(hildon_find_toolbar_entry_activate), self);

    /* Find button */
    priv->find_button = gtk_tool_button_new (
            gtk_image_new_from_icon_name ("qgn_toolb_browser_gobutton",
                HILDON_ICON_SIZE_TOOLBAR),
            "Find");

    g_signal_connect (priv->find_button, "clicked",
            G_CALLBACK(hildon_find_toolbar_emit_search), self);
    gtk_widget_show_all( GTK_WIDGET(priv->find_button));
    gtk_toolbar_insert ( GTK_TOOLBAR(self), priv->find_button, -1);
    if ( GTK_WIDGET_CAN_FOCUS( GTK_BIN(priv->find_button)->child) )
        GTK_WIDGET_UNSET_FLAGS(
                GTK_BIN(priv->find_button)->child, GTK_CAN_FOCUS);

    /* Separator */
    priv->separator = gtk_separator_tool_item_new();
    gtk_widget_show(GTK_WIDGET(priv->separator));
    gtk_toolbar_insert (GTK_TOOLBAR(self), priv->separator, -1);

    /* Close button */
    priv->close_button = gtk_tool_button_new (
            gtk_image_new_from_icon_name ("qgn_toolb_gene_close",
                HILDON_ICON_SIZE_TOOLBAR),
            "Close");
    g_signal_connect(priv->close_button, "clicked",
            G_CALLBACK(hildon_find_toolbar_emit_close), self);
    gtk_widget_show_all(GTK_WIDGET(priv->close_button));
    gtk_toolbar_insert (GTK_TOOLBAR(self), priv->close_button, -1);
    if ( GTK_WIDGET_CAN_FOCUS( GTK_BIN(priv->close_button)->child) )
        GTK_WIDGET_UNSET_FLAGS(
                GTK_BIN(priv->close_button)->child, GTK_CAN_FOCUS);
}

/**
 * hildon_find_toolbar_new:
 * @label: label for the find_toolbar, NULL to set the label to 
 *         default "Find"
 * 
 * Returns a new HildonFindToolbar.
 *
 * Returns: a new HildonFindToolbar
 */

GtkWidget*
hildon_find_toolbar_new                         (const gchar *label)
{
    GtkWidget *findtoolbar;

    findtoolbar = GTK_WIDGET (g_object_new (HILDON_TYPE_FIND_TOOLBAR, NULL));

    if (label != NULL)
        g_object_set(findtoolbar, "label", label, NULL);

    return findtoolbar;
}

/**
 * hildon_find_toolbar_new_with_model
 * @label: label for the find_toolbar, NULL to set the label to 
 *         default "Find"
 * @model: a @GtkListStore
 * @column: indicating which column the search histry list will 
 *          retreive string from
 * 
 * Returns a new HildonFindToolbar, with a model.
 *
 * Returns: a new #HildonFindToolbar
 */
GtkWidget*
hildon_find_toolbar_new_with_model              (const gchar *label,
                                                 GtkListStore *model,
                                                 gint column)
{
    GtkWidget *findtoolbar;

    findtoolbar = hildon_find_toolbar_new (label);

    g_object_set (findtoolbar, "list", model, "column", column, NULL);

    return findtoolbar;
}

/**
 * hildon_find_toolbar_highlight_entry
 * @ftb: find Toolbar whose entry is to be highlighted
 * @get_focus: if user passes TRUE to this value, then the text in
 * the entry will not only get highlighted, but also get focused.
 * 
 */
void
hildon_find_toolbar_highlight_entry             (HildonFindToolbar *self,
                                                 gboolean get_focus)
{
    GtkEntry *entry = NULL;
    HildonFindToolbarPrivate *priv;

    g_return_if_fail (HILDON_IS_FIND_TOOLBAR (self));
    priv = HILDON_FIND_TOOLBAR_GET_PRIVATE (self);
    g_assert (priv);

    entry = hildon_find_toolbar_get_entry (priv);

    gtk_editable_select_region (GTK_EDITABLE (entry), 0, -1);

    if(get_focus)
        gtk_widget_grab_focus (GTK_WIDGET (entry));
}

