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

#include "hildon-find-toolbar.h"
#include "hildon-defines.h"
#include <gdk/gdkkeysyms.h>

#include <gtk/gtklabel.h>
#include <gtk/gtkentry.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtktoolbutton.h>
#include <gtk/gtktoolitem.h>
#include <gtk/gtkcomboboxentry.h>
#include <gtk/gtkseparatortoolitem.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

/*same define as gtkentry.c as entry will further handle this*/
#define MAX_SIZE G_MAXUSHORT
#define FIND_LABEL_XPADDING 6
#define FIND_LABEL_YPADDING 0

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
  PROP_LABEL = 1,
  PROP_PREFIX,
  PROP_LIST,
  PROP_COLUMN,
  PROP_MAX,
  PROP_HISTORY_LIMIT
};

struct _HildonFindToolbarPrivate
{
  GtkWidget*		label;
  GtkWidget*		entry_combo_box;
  GtkToolItem*		find_button;
  GtkToolItem*		separator;
  GtkToolItem*		close_button;

  gint			history_limit;
};
static guint HildonFindToolbar_signal[LAST_SIGNAL] = {0};

G_DEFINE_TYPE(HildonFindToolbar, \
	      hildon_find_toolbar, GTK_TYPE_TOOLBAR)

static gboolean
hildon_find_toolbar_filter(GtkTreeModel *model,
			   GtkTreeIter *iter,
			   gpointer self)
{
  GtkTreePath *path;
  gint *indices;
  gint n;
  gint limit;

  g_object_get(G_OBJECT(self), "history_limit", &limit, NULL);
  path = gtk_tree_model_get_path(model, iter);
  indices = gtk_tree_path_get_indices (path);

  /* List store has only one level. If this row's index number is larger
     than history_limit, we want to filter it. */
  n = indices[0];
  gtk_tree_path_free(path);
  
  if(n < limit)
    return TRUE;
  else
    return FALSE;
}

static void
hildon_find_toolbar_apply_filter(HildonFindToolbar *self, 
				 GtkListStore *list,
				 gboolean set_column)
{
  GtkTreeModel *filter;
  gint c_n = 0;
  HildonFindToolbarPrivate *priv = self->priv;

  /* Create a filter for the given list store. Its only purpose is to hide
     the oldest entries so only "history_limit" entries are visible. */
  filter = gtk_tree_model_filter_new(GTK_TREE_MODEL(list), NULL);

  /* FIXME: next two lines are useless */
  if(gtk_combo_box_get_model(GTK_COMBO_BOX(priv->entry_combo_box)) != NULL)
    g_object_get(G_OBJECT(self), "column", &c_n, NULL);
  
  gtk_tree_model_filter_set_visible_func(GTK_TREE_MODEL_FILTER(filter), 
					 hildon_find_toolbar_filter, 
					 (gpointer) self, NULL);
  gtk_combo_box_set_model(GTK_COMBO_BOX(priv->entry_combo_box), filter);

  /* ComboBox keeps the only needed reference to the filter */
  g_object_unref(filter);

  /* If "column" property hasn't been set yet and we have set_column=TRUE,
     set the column to 0. This is done when we're creating the list store
     internally. */
  if ( gtk_combo_box_entry_get_text_column(
	GTK_COMBO_BOX_ENTRY(priv->entry_combo_box)) == -1 && set_column)
    gtk_combo_box_entry_set_text_column(
	GTK_COMBO_BOX_ENTRY(priv->entry_combo_box), c_n);
}

static void
hildon_find_toolbar_get_property(GObject     *object,
				 guint        prop_id,
				 GValue      *value,
				 GParamSpec  *pspec)
{
  const gchar *string;
  gint c_n;
  HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR(object)->priv;
  
  switch (prop_id)
    {
    case PROP_LABEL:
      string = gtk_label_get_text(GTK_LABEL(priv->label));
      g_value_set_string(value, string);
      break;
    case PROP_PREFIX:
      string = gtk_entry_get_text(GTK_ENTRY(gtk_bin_get_child
				  (GTK_BIN(priv->entry_combo_box))));
      g_value_set_string(value, string);
      break;
    case PROP_LIST:
      g_value_set_object(value, 
			 (gpointer) gtk_tree_model_filter_get_model(
			 GTK_TREE_MODEL_FILTER(gtk_combo_box_get_model(
			 GTK_COMBO_BOX(priv->entry_combo_box)))));
      break;
    case PROP_COLUMN:
      c_n = gtk_combo_box_entry_get_text_column(
	GTK_COMBO_BOX_ENTRY(priv->entry_combo_box));
      g_value_set_int(value, c_n);
      break;
    case PROP_MAX:
      g_value_set_int(value, gtk_entry_get_max_length(GTK_ENTRY(gtk_bin_get_child
						(GTK_BIN(priv->entry_combo_box)))));
      break;
    case PROP_HISTORY_LIMIT:
      g_value_set_int(value, priv->history_limit);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hildon_find_toolbar_set_property(GObject      *object,
				 guint         prop_id,
				 const GValue *value,
				 GParamSpec   *pspec)
{
  const gchar *string;
  GtkTreeModel *model;
  HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR(object)->priv;
  
  switch (prop_id)
    {
    case PROP_LABEL:
      string = g_value_get_string(value);	
      gtk_label_set_text(GTK_LABEL(priv->label), string);
      break;
    case PROP_PREFIX:
      string = g_value_get_string(value);
      gtk_entry_set_text(GTK_ENTRY(gtk_bin_get_child
			(GTK_BIN(priv->entry_combo_box))), 
			string);
      break;
    case PROP_LIST:
      hildon_find_toolbar_apply_filter(HILDON_FIND_TOOLBAR(object), 
				GTK_LIST_STORE(g_value_get_object(value)), 
				FALSE);
      break;
    case PROP_COLUMN:
      gtk_combo_box_entry_set_text_column (
       GTK_COMBO_BOX_ENTRY(priv->entry_combo_box), g_value_get_int(value));
      break;
    case PROP_MAX:
      gtk_entry_set_max_length(GTK_ENTRY(gtk_bin_get_child
			      (GTK_BIN(priv->entry_combo_box))), 
			      g_value_get_int(value));
      break;
    case PROP_HISTORY_LIMIT:
      priv->history_limit = g_value_get_int(value);
      model = gtk_combo_box_get_model(GTK_COMBO_BOX(priv->entry_combo_box));
      if(model != NULL)
	{
	  /*FIXME refilter function doesnt update the status of the combobox
	   * pop up arrowi, which will cause crash, recall apply filter 
	   * solves the problem*/
	  
	  /*gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(model));*/

          /* FIXME: don't call with set_column=TRUE since it sets column to 0.
             We can't know that it's correct. */
	  hildon_find_toolbar_apply_filter(HILDON_FIND_TOOLBAR(object),GTK_LIST_STORE(
			gtk_tree_model_filter_get_model(GTK_TREE_MODEL_FILTER(model))),
					   TRUE);
	}
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static gboolean
hildon_find_toolbar_history_append(HildonFindToolbar *self,
				   gpointer data) 
{
  gchar *string;
  gchar *old_string;
  gboolean occupy;
  gint c_n = 0;
  GtkListStore *list = NULL;
  GtkTreeIter iter;
  gboolean self_create = FALSE;
  HildonFindToolbarPrivate *priv = HILDON_FIND_TOOLBAR(self)->priv;
  
  g_object_get(G_OBJECT(self), "prefix", &string, NULL);
  
  if(strcmp(string, "") != 0)
    {
      GtkTreeModel *model = NULL;

      /* If list store is set, get it */
      if(gtk_combo_box_get_model(GTK_COMBO_BOX(priv->entry_combo_box)) != NULL)
      {
          list = GTK_LIST_STORE(gtk_tree_model_filter_get_model(
                     GTK_TREE_MODEL_FILTER(gtk_combo_box_get_model(
                     GTK_COMBO_BOX(priv->entry_combo_box)))));
          model = GTK_TREE_MODEL (list);
      }
      if(list == NULL)
	{
          /* No list store set. Create our own. */
	  list = gtk_list_store_new(1, G_TYPE_STRING);
	  self_create = TRUE;
	}
      else
	g_object_get(G_OBJECT(self), "column", &c_n, NULL);


      /* Latest string is always the first one in list. */
      /* Remove item if too many, remove also duplicates. */
      if (model)
      {      
      occupy = gtk_tree_model_get_iter_first(model, &iter);

      while(occupy)
      {
        gtk_tree_model_get (model, &iter, c_n, &old_string, -1);
        if(old_string != NULL && strcmp(string, old_string) == 0)
          {
            /* Found it, remove. */
            occupy = FALSE;
            gtk_list_store_remove (list, &iter);
          }
        else
          occupy = gtk_tree_model_iter_next(model, &iter);
      }

      /* If we have more than history_limit amount of items
       * in the list, remove one. */
      if(gtk_tree_model_iter_n_children(model, NULL) >= priv->history_limit)
        {
          gtk_tree_model_get_iter_first(model, &iter);
          gtk_list_store_remove(list, &iter);
        }
    }

  /* Column number is -1 if "column" property hasn't been set but
         "list" property is. */
    if(c_n >= 0)
    {/* Add the string to first in list */
        gtk_list_store_append(list, &iter);
        gtk_list_store_set(list, &iter, c_n, string, -1);
        if(self_create)
	  {
              /* Add the created list to ComboBoxEntry */
	      hildon_find_toolbar_apply_filter(self, list, TRUE);
              /* ComboBoxEntry keeps the only needed reference to this list */
	      g_object_unref(list);
	    }
	  else
            /* Refilter to get the oldest entry hidden from history */
	    gtk_tree_model_filter_refilter(GTK_TREE_MODEL_FILTER(
		gtk_combo_box_get_model(GTK_COMBO_BOX(priv->entry_combo_box))));
	}
    }
  g_free(string);
 
  return TRUE;
}

static void
hildon_find_toolbar_emit_search(GtkButton *button, gpointer self)
{
  gboolean rb;

  /* Clicked search button. Perform search and add search prefix to history */
  g_signal_emit_by_name(self, "search", NULL);
  g_signal_emit_by_name(self, "history_append", &rb, NULL);
}

static void
hildon_find_toolbar_emit_close(GtkButton *button, gpointer self)
{
  /* Clicked close button */
  g_signal_emit_by_name(self, "close", NULL);
}

static void
hildon_find_toolbar_emit_invalid_input(GtkEntry *entry, 
				       GtkInvalidInputType type, 
				       gpointer self)
{
  if(type == GTK_INVALID_INPUT_MAX_CHARS_REACHED)
    g_signal_emit_by_name(self, "invalid_input", NULL);
}

static gboolean
hildon_find_toolbar_entry_key_press (GtkWidget *widget,
                                    GdkEventKey *event,
                                    gpointer user_data)
{
  GtkWidget *find_toolbar = GTK_WIDGET(user_data);
  
  /* on enter we emit search and history_append signals and keep
   * focus by returning true */
  if (event->keyval == GDK_KP_Enter)
    {
      gboolean rb;  
      g_signal_emit_by_name(find_toolbar, "search", NULL);
      g_signal_emit_by_name(find_toolbar, "history_append", &rb, NULL);

      return TRUE;
    }

  return FALSE;      
}

static void
hildon_find_toolbar_class_init(HildonFindToolbarClass *klass)
{
  GObjectClass *object_class;

  g_type_class_add_private(klass, sizeof(HildonFindToolbarPrivate));

  object_class = G_OBJECT_CLASS(klass);

  object_class->get_property = hildon_find_toolbar_get_property;
  object_class->set_property = hildon_find_toolbar_set_property;

  klass->history_append = hildon_find_toolbar_history_append;
  
  g_object_class_install_property(object_class, PROP_LABEL, 
				  g_param_spec_string("label", 
				  "Label", "Displayed name for"
				  " find-toolbar",
                                  _("Ecdg_ti_find_toolbar_label"),
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
hildon_find_toolbar_init(HildonFindToolbar *self)
{
  GtkToolItem *label_container;
  GtkToolItem *entry_combo_box_container;
  
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE(self, \
					   HILDON_TYPE_FIND_TOOLBAR, 
					   HildonFindToolbarPrivate);

  /* Create the label */
  self->priv->label = gtk_label_new(_("Ecdg_ti_find_toolbar_label"));
  
  gtk_misc_set_padding (GTK_MISC(self->priv->label), FIND_LABEL_XPADDING,
                        FIND_LABEL_YPADDING);

  label_container = gtk_tool_item_new();
  gtk_container_add(GTK_CONTAINER(label_container), 
		    self->priv->label);
  gtk_widget_show_all(GTK_WIDGET(label_container));
  gtk_toolbar_insert (GTK_TOOLBAR(self), label_container, -1);
  
  /* ComboBoxEntry for search prefix string / history list */
  self->priv->entry_combo_box = gtk_combo_box_entry_new();
  g_signal_connect(G_OBJECT(gtk_bin_get_child(
		   GTK_BIN(self->priv->entry_combo_box))), 
		   "invalid_input", 
		   G_CALLBACK(hildon_find_toolbar_emit_invalid_input),
		   (gpointer) self);
  entry_combo_box_container = gtk_tool_item_new();
  gtk_tool_item_set_expand(entry_combo_box_container, TRUE);
  gtk_container_add(GTK_CONTAINER(entry_combo_box_container),
		    self->priv->entry_combo_box);
  gtk_widget_show_all(GTK_WIDGET(entry_combo_box_container));
  gtk_toolbar_insert (GTK_TOOLBAR(self), entry_combo_box_container, -1);
  g_signal_connect(GTK_BIN (self->priv->entry_combo_box)->child, 
                    "key-press-event",
                    G_CALLBACK(hildon_find_toolbar_entry_key_press),
                    (gpointer) self);

  /* Find button */
  self->priv->find_button = gtk_tool_button_new (
                              gtk_image_new_from_icon_name ("qgn_toolb_browser_gobutton",
                                                            HILDON_ICON_SIZE_TOOLBAR),
                              "Find");
  g_signal_connect(G_OBJECT(self->priv->find_button), "clicked",
		   G_CALLBACK(hildon_find_toolbar_emit_search), 
		   (gpointer) self);
  gtk_widget_show_all(GTK_WIDGET(self->priv->find_button));
  gtk_toolbar_insert (GTK_TOOLBAR(self), self->priv->find_button, -1);
  if ( GTK_WIDGET_CAN_FOCUS( GTK_BIN(self->priv->find_button)->child) )
      GTK_WIDGET_UNSET_FLAGS(
              GTK_BIN(self->priv->find_button)->child, GTK_CAN_FOCUS);
  
  /* Separator */
  self->priv->separator = gtk_separator_tool_item_new();
  gtk_widget_show(GTK_WIDGET(self->priv->separator));
  gtk_toolbar_insert (GTK_TOOLBAR(self), self->priv->separator, -1);
  
  /* Close button */
  self->priv->close_button = gtk_tool_button_new (
                               gtk_image_new_from_icon_name ("qgn_toolb_gene_close",
                                                             HILDON_ICON_SIZE_TOOLBAR),
                               "Close");
  g_signal_connect(G_OBJECT(self->priv->close_button), "clicked",
		   G_CALLBACK(hildon_find_toolbar_emit_close), 
		   (gpointer) self);
  gtk_widget_show_all(GTK_WIDGET(self->priv->close_button));
  gtk_toolbar_insert (GTK_TOOLBAR(self), self->priv->close_button, -1);
  if ( GTK_WIDGET_CAN_FOCUS( GTK_BIN(self->priv->close_button)->child) )
      GTK_WIDGET_UNSET_FLAGS(
              GTK_BIN(self->priv->close_button)->child, GTK_CAN_FOCUS);
}

/*Public functions*/

/**
 * hildon_find_toolbar_new:
 * @label: label for the find_toolbar, NULL to set the label to 
 *         default "Find".
 * 
 * Returns a new HildonFindToolbar.
 *
 **/

GtkWidget *
hildon_find_toolbar_new(gchar *label)
{
  GtkWidget *findtoolbar;
  
  findtoolbar =  GTK_WIDGET(g_object_new(HILDON_TYPE_FIND_TOOLBAR, NULL));
  if(label != NULL)
    g_object_set(G_OBJECT(findtoolbar), "label", label, NULL);

  return findtoolbar;
}

/**
 * hildon_find_toolbar_new_with_model
 * @label: label for the find_toolbar, NULL to set the label to 
 *         default "Find".
 * @model: A @GtkListStore.
 * @column: Indicating which column the search histry list will 
 *          retreive string from.
 * 
 * Returns a new HildonFindToolbar, with a model.
 *
 **/
GtkWidget *
hildon_find_toolbar_new_with_model(gchar *label, 
				   GtkListStore *model,
				   gint column)
{
  GtkWidget *findtoolbar;

  findtoolbar = hildon_find_toolbar_new(label);
  g_object_set(G_OBJECT(findtoolbar), "list", model, 
	       "column", column, NULL);

  return findtoolbar;
}

/**
 * hildon_find_toolbar_highlight_entry
 * @HildonFindToolbar: Find Toolbar whose entry is to be highlighted.
 * @get_focus: if user passes TRUE to this value, then the text in
 * the entry will not only get highlighted, but also get focused.
 * 
 * */
void
hildon_find_toolbar_highlight_entry(HildonFindToolbar *ftb,
                                    gboolean get_focus)
{
  GtkWidget *entry = NULL;
  
  if(!HILDON_IS_FIND_TOOLBAR(ftb))
    return;
  
  entry = gtk_bin_get_child(GTK_BIN(ftb->priv->entry_combo_box));
  
  gtk_editable_select_region(GTK_EDITABLE(entry), 0, -1);
  
  if(get_focus)
    gtk_widget_grab_focus(entry);
}
