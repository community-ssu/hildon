/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version. or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/**
 * SECTION:hildon-touch-selector
 * @short_description: A selector widget with several columns
 *
 * HildonTouchSelector is a selector widget, very similar to the #GtkComboBox, but with
 * several individual pannable columns
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include "hildon-pannable-area.h"
#include "hildon-touch-selector.h"

#define HILDON_TOUCH_SELECTOR_GET_PRIVATE(obj)                          \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_TOUCH_SELECTOR, HildonTouchSelectorPrivate))

G_DEFINE_TYPE (HildonTouchSelector, hildon_touch_selector, GTK_TYPE_HBOX)

#define CENTER_ON_SELECTED_ITEM_DELAY 50

/**
 * Struct to maintain the data of each column. The columns are the elements
 * of the widget that belongs properly to the selection behaviour. As
 * the selector is a hbox, you can add more widgets, like buttons etc.
 * between the columns, but this doesn't belongs to the selection
 * logic
 */
typedef struct _SelectorColumn SelectorColumn;
struct _SelectorColumn
{
  HildonTouchSelector *parent;    /* the selector that contains this column */
  GtkTreeModel *model;
  GtkTreeView *tree_view;

  GtkWidget *panarea;           /* the pannable widget */
};

struct _HildonTouchSelectorPrivate
{
  GSList *columns;              /* the selection columns */

  HildonTouchSelectorPrintFunc print_func;
};

enum
{
  CHANGED,
  LAST_SIGNAL
};

static gint hildon_touch_selector_signals[LAST_SIGNAL] = { 0 };

/* gtkwidget */
static void hildon_touch_selector_map (GtkWidget * widget);

/* gtkcontainer */
static void hildon_touch_selector_remove (GtkContainer * container,
                                          GtkWidget * widget);
/* private functions */
static void _selection_changed_cb (GtkTreeSelection * selection,
                                   gpointer user_data);
static gchar *_default_print_func (HildonTouchSelector * selector);

static SelectorColumn *_create_new_column (HildonTouchSelector * selector,
                                           GtkTreeModel * model,
                                           GtkCellRenderer * renderer,
                                           va_list args);
static gboolean _hildon_touch_selector_center_on_selected_items (gpointer data);

static void
hildon_touch_selector_class_init (HildonTouchSelectorClass * class)
{
  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  gobject_class = (GObjectClass *) class;
  object_class = (GtkObjectClass *) class;
  widget_class = (GtkWidgetClass *) class;
  container_class = (GtkContainerClass *) class;

  /* GObject */

  /* GtkWidget */
  widget_class->map = hildon_touch_selector_map;

  /* GtkContainer */
  container_class->remove = hildon_touch_selector_remove;


  /* signals */
  /**
   * HildonTouchSelector::changed:
   * @widget: the object which received the signal
   *
   * The changed signal is emitted when the active
   * item is changed. The can be due to the user selecting
   * a different item from the list, or due to a
   * call to hildon_touch_selector_set_active_iter() on
   * one of the columns
   *
   */
  hildon_touch_selector_signals[CHANGED] =
    g_signal_new ("changed",
                  G_OBJECT_CLASS_TYPE (class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (HildonTouchSelectorClass, changed),
                  NULL, NULL,
                  gtk_marshal_NONE__INT, G_TYPE_NONE, 1, G_TYPE_INT);
  /* properties */

  /* style properties */
  g_type_class_add_private (object_class, sizeof (HildonTouchSelectorPrivate));
}


static void
hildon_touch_selector_init (HildonTouchSelector * selector)
{
  selector->priv = HILDON_TOUCH_SELECTOR_GET_PRIVATE (selector);

  GTK_WIDGET_SET_FLAGS (GTK_WIDGET (selector), GTK_NO_WINDOW);
  gtk_widget_set_redraw_on_allocate (GTK_WIDGET (selector), FALSE);

  selector->priv->columns = NULL;

  selector->priv->print_func = NULL;

  /* FIXME: this is the correct height? A fixed height is the correct 
     implementation */
  gtk_widget_set_size_request (GTK_WIDGET (selector), -1, 320);
}

static void
hildon_touch_selector_map (GtkWidget * widget)
{
  GTK_WIDGET_CLASS (hildon_touch_selector_parent_class)->map (widget);

  g_timeout_add (CENTER_ON_SELECTED_ITEM_DELAY,
                 _hildon_touch_selector_center_on_selected_items, widget);
}

/*------------------------------ GtkContainer ------------------------------ */

/*
 * Required in order to free the column at the columns list
 */
static void
hildon_touch_selector_remove (GtkContainer * container, GtkWidget * widget)
{
  HildonTouchSelector *selector = NULL;
  GSList *iter = NULL;
  gint position = 0;
  SelectorColumn *current_column = NULL;
  gint num_columns = 0;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (container));

  selector = HILDON_TOUCH_SELECTOR (container);
  num_columns = hildon_touch_selector_get_num_columns (selector);

  /* Check if the widget is inside a column and remove
     it from the list */
  iter = selector->priv->columns;
  position = 0;
  while (iter) {
    current_column = (SelectorColumn *) iter->data;
    if (widget == current_column->panarea) {
      current_column = g_slist_nth_data (selector->priv->columns, position);

      selector->priv->columns = g_slist_remove (selector->priv->columns,
                                                current_column);
      g_free (current_column);

      break;
    }

    position++;
    iter = g_slist_next (iter);
  }
  if (position >= num_columns) {
    g_debug ("This widget was not inside the selector column");
  }

  GTK_CONTAINER_CLASS (hildon_touch_selector_parent_class)->remove (container, widget);
}

/* ------------------------------ PRIVATE METHODS ---------------------------- */
/**
 * default_print_func:
 * @selector: a #HildonTouchSelector
 *
 * Default print function
 *
 * Returns: a new string that represent the selected items
 **/
static gchar *
_default_print_func (HildonTouchSelector * selector)
{
  gchar *result = NULL;
  gchar *aux = NULL;
  gint num_columns = 0;
  GtkTreeIter iter;
  GtkTreeModel *model = NULL;
  gchar *current_string = NULL;
  gint i;
  HildonTouchSelectorSelectionMode mode;
  GList *item = NULL;
  GtkTreePath *current_path = NULL;
  GList *selected_rows = NULL;
  gint initial_value = 0;

  num_columns = hildon_touch_selector_get_num_columns (selector);

  mode = hildon_touch_selector_get_column_selection_mode (selector);

  if ((mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE)
      && (num_columns > 0)) {
    /* In this case we get the first column first */
    selected_rows = hildon_touch_selector_get_selected_rows (selector, 0);
    model = hildon_touch_selector_get_model (selector, 0);

    result = g_strdup_printf ("(");
    i = 0;
    for (item = selected_rows; item; item = g_list_next (item)) {
      current_path = item->data;
      gtk_tree_model_get_iter (model, &iter, current_path);

      gtk_tree_model_get (model, &iter, 0, &current_string, -1);

      if (i < g_list_length (selected_rows) - 1) {
        aux = g_strconcat (result, current_string, ",", NULL);
        g_free (result);
        result = aux;
      } else {
        aux = g_strconcat (result, current_string, NULL);
        g_free (result);
        result = aux;
      }
      i++;
    }

    aux = g_strconcat (result, ")", NULL);
    g_free (result);
    result = aux;

    g_list_foreach (selected_rows, (GFunc) (gtk_tree_path_free), NULL);
    g_list_free (selected_rows);
    initial_value = 1;
  } else {
    initial_value = 0;
  }

  for (i = initial_value; i < num_columns; i++) {
    model = hildon_touch_selector_get_model (selector, i);
    if (hildon_touch_selector_get_active_iter (selector, i, &iter)) {

      gtk_tree_model_get (model, &iter, 0, &current_string, -1);
      if (i != 0) {
        aux = g_strconcat (result, ":", current_string, NULL);
        g_free (result);
        result = aux;
      } else {
        result = g_strdup_printf ("%s", current_string);
      }
    }
  }

  return result;
}

static void
_selection_changed_cb (GtkTreeSelection * selection, gpointer user_data)
{
  HildonTouchSelector *selector = NULL;
  SelectorColumn *column = NULL;
  gint num_column = -1;

  column = (SelectorColumn *) user_data;
  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (column->parent));

  selector = column->parent;

  num_column = g_slist_index (selector->priv->columns, column);

  g_signal_emit (selector, hildon_touch_selector_signals[CHANGED], 0, num_column);
}


static SelectorColumn *
_create_new_column (HildonTouchSelector * selector,
                    GtkTreeModel * model,
                    GtkCellRenderer * renderer, va_list args)
{
  SelectorColumn *new_column = NULL;
  GtkTreeViewColumn *tree_column = NULL;
  GValue val = { 0, };
  GtkTreeView *tv = NULL;
  GtkWidget *panarea = NULL;
  GtkTreeSelection *selection = NULL;
  GtkTreeIter iter;
  gchar *attribute;
  gint value;

  tree_column = gtk_tree_view_column_new ();
  gtk_tree_view_column_pack_start (tree_column, renderer, TRUE);

  attribute = va_arg (args, gchar *);
  while (attribute != NULL) {
    value = va_arg (args, gint);
    gtk_tree_view_column_add_attribute (tree_column, renderer, attribute,
                                        value);
    attribute = va_arg (args, gchar *);
  }

  tv = g_object_new (GTK_TYPE_TREE_VIEW, "model", model, "name", "fremantle-widget",    /* FIXME: is this always this name? */
                     "hildon-ui-mode", HILDON_UI_MODE_EDIT,
                     "rules-hint", TRUE, NULL);

  gtk_tree_view_append_column (GTK_TREE_VIEW (tv), tree_column);

  new_column = (SelectorColumn *) g_malloc0 (sizeof (SelectorColumn));
  new_column->parent = selector;

  panarea = hildon_pannable_area_new ();

  g_value_init (&val, G_TYPE_INT);
  g_value_set_int (&val, HILDON_PANNABLE_AREA_INDICATOR_MODE_HIDE);
  g_object_set_property (G_OBJECT (panarea), "vindicator-mode", &val);

  g_value_unset (&val);
  g_value_init (&val, G_TYPE_BOOLEAN);
  g_value_set_boolean (&val, FALSE);
  g_object_set_property (G_OBJECT (panarea), "initial-hint", &val);

  gtk_container_add (GTK_CONTAINER (panarea), GTK_WIDGET (tv));

  new_column->model = model;
  new_column->tree_view = tv;
  new_column->panarea = panarea;

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tv));
  gtk_tree_selection_set_mode (selection, GTK_SELECTION_BROWSE);

  /* select the first item */
  if (gtk_tree_model_get_iter_first (model, &iter)) {
    gtk_tree_selection_select_iter (selection, &iter);
  }

  gtk_widget_grab_focus (GTK_WIDGET (tv));

  /* connect to the changed signal connection */
  g_signal_connect (G_OBJECT (selection), "changed",
                    G_CALLBACK (_selection_changed_cb), new_column);

  return new_column;
}

/* ------------------------------ PUBLIC METHODS ---------------------------- */

/**
 * hildon_touch_selector_new:
 * @:
 *
 * Creates a new empty #HildonTouchSelector
 *
 * Returns: a new #HildonTouchSelector
 **/
GtkWidget *
hildon_touch_selector_new ()
{
  return g_object_new (HILDON_TYPE_TOUCH_SELECTOR, NULL);
}

/**
 * hildon_touch_selector_append_text_column
 * @selector: the #HildonTouchSelector widget
 * @model: the #GtkTreeModel with the data of the column
 *
 * This functions adds a new column to the widget, with the data on
 * the model. Only the widgets added in this way should used on
 * the selection logic, ie: the print function, the "changed" signal etc.
 *
 * There are a prerequisite on this model: this append
 * consideres that the text data is in the first column of the model
 *
 * Basically it adds a tree view to the widget, using the model and
 * the data received.
 *
 * Returns: TRUE if a new column were added, FALSE otherside
 **/
gboolean
hildon_touch_selector_append_column (HildonTouchSelector * selector,
                                     GtkTreeModel * model,
                                     GtkCellRenderer * cell_renderer, ...)
{
  va_list args;
  SelectorColumn *new_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);
  g_return_val_if_fail (GTK_IS_TREE_MODEL (model), FALSE);

  if (model != NULL) {

    va_start (args, cell_renderer);
    new_column = _create_new_column (selector, model, cell_renderer, args);
    va_end (args);

    selector->priv->columns = g_slist_append (selector->priv->columns,
                                              new_column);
    gtk_box_pack_start (GTK_BOX (selector), new_column->panarea, TRUE, TRUE, 6);

    gtk_widget_show_all (new_column->panarea);
  } else {
    return FALSE;
  }

  return TRUE;
}

/**
 * hildon_touch_selector_append_text_column
 * @selector: the #HildonTouchSelector widget
 * @model: the #GtkTreeModel with the data of the column
 *
 * Equivalent to hildon_touch_selector_append_column, but using a
 * default text cell renderer. This is the most common use of the
 * widget.
 *
 * Returns: TRUE if a new column were added, FALSE otherside
 **/
gboolean
hildon_touch_selector_append_text_column (HildonTouchSelector * selector,
                                          GtkTreeModel * model)
{
  GtkCellRenderer *renderer = NULL;
  GValue val = { 0, };

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);
  g_return_val_if_fail (GTK_IS_TREE_MODEL (model), FALSE);

  if (model != NULL) {
    renderer = gtk_cell_renderer_text_new ();

    g_value_init (&val, G_TYPE_FLOAT);
    g_value_set_float (&val, 0.5);
    /* FIXME: center the text, this should be configurable */
    g_object_set_property (G_OBJECT (renderer), "xalign", &val);

    return hildon_touch_selector_append_column (selector, model, renderer,
                                                "text", 0, NULL);
  } else {
    return FALSE;
  }
}

/**
 * hildon_touch_selector_remove_column
 * @selector: a #HildonTouchSelector
 * @position: the column position to remove, counting from 0 to (total column number - 1)
 *
 *
 * Returns: TRUE is the column was removed, FALSE otherwise
 **/
gboolean
hildon_touch_selector_remove_column (HildonTouchSelector * selector, gint position)
{
  SelectorColumn *current_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);
  g_return_val_if_fail (position <
                        hildon_touch_selector_get_num_columns (selector), FALSE);

  current_column = g_slist_nth_data (selector->priv->columns, position);

  gtk_container_remove (GTK_CONTAINER (selector), current_column->panarea);

  return TRUE;
}

void
hildon_touch_selector_set_column_attributes (HildonTouchSelector * selector,
                                             gint num_column,
                                             GtkCellRenderer * cell_renderer,
                                             ...)
{
  va_list args;
  GtkTreeViewColumn *tree_column = NULL;
  SelectorColumn *current_column = NULL;
  gchar *attribute = NULL;
  gint value = 0;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (num_column <
                    hildon_touch_selector_get_num_columns (selector));

  current_column = g_slist_nth_data (selector->priv->columns, num_column);

  tree_column = gtk_tree_view_get_column (current_column->tree_view, 0);
  gtk_tree_view_remove_column (current_column->tree_view, tree_column);

  tree_column = gtk_tree_view_column_new ();
  gtk_tree_view_column_pack_start (tree_column, cell_renderer, TRUE);

  va_start (args, cell_renderer);
  attribute = va_arg (args, gchar *);

  gtk_tree_view_column_clear_attributes (tree_column, cell_renderer);

  while (attribute != NULL) {
    value = va_arg (args, gint);
    gtk_tree_view_column_add_attribute (tree_column, cell_renderer,
                                        attribute, value);
    attribute = va_arg (args, gchar *);
  }

  va_end (args);

  gtk_tree_view_append_column (current_column->tree_view, tree_column);
}

gboolean
hildon_touch_selector_insert_column (HildonTouchSelector * selector, gint position)
{
  g_warning ("Un-implemented!");

  return TRUE;
}

gint
hildon_touch_selector_get_num_columns (HildonTouchSelector * selector)
{
  return g_slist_length (selector->priv->columns);
}

HildonTouchSelectorSelectionMode
hildon_touch_selector_get_column_selection_mode (HildonTouchSelector * selector)
{
  HildonTouchSelectorSelectionMode result =
    HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE;
  GtkSelectionMode treeview_mode = GTK_SELECTION_SINGLE;
  SelectorColumn *column = NULL;
  GtkTreeSelection *selection = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), result);
  g_return_val_if_fail (hildon_touch_selector_get_num_columns (selector) > 0,
                        result);

  column = (SelectorColumn *) selector->priv->columns->data;

  selection = gtk_tree_view_get_selection (column->tree_view);
  treeview_mode = gtk_tree_selection_get_mode (selection);


  if (treeview_mode == GTK_SELECTION_MULTIPLE) {
    result = HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE;
  } else {
    result = HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE;
  }

  return result;
}

void
hildon_touch_selector_set_column_selection_mode (HildonTouchSelector * selector,
                                                 HildonTouchSelectorSelectionMode
                                                 mode)
{
  GtkTreeView *tv = NULL;
  SelectorColumn *column = NULL;
  GtkTreeSelection *selection = NULL;
  GtkSelectionMode treeview_mode;
  GtkTreeIter iter;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (hildon_touch_selector_get_num_columns (selector) > 0);

  column = (SelectorColumn *) (g_slist_nth (selector->priv->columns, 0))->data;
  tv = column->tree_view;

  if (tv) {
    switch (mode) {
    case HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE:
      treeview_mode = GTK_SELECTION_SINGLE;
      break;
    case HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE:
      treeview_mode = GTK_SELECTION_MULTIPLE;
      break;
    }

    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tv));
    gtk_tree_selection_set_mode (selection, treeview_mode);

    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tv));
    gtk_tree_model_get_iter_first (column->model, &iter);
    gtk_tree_selection_unselect_all (selection);
    gtk_tree_selection_select_iter (selection, &iter);
  }

}

void
hildon_touch_selector_set_print_func (HildonTouchSelector * selector,
                                      HildonTouchSelectorPrintFunc func)
{
  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));

  selector->priv->print_func = func;
}

HildonTouchSelectorPrintFunc
hildon_touch_selector_get_print_func (HildonTouchSelector * selector)
{
  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);

  return selector->priv->print_func;
}

/**
 * hildon_touch_selector_get_active_iter:
 * @selector: a #HildonTouchSelector
 * @column: the column number we want to get the element
 * @iter: #GtkTreeIter currently selected
 *
 * Sets iter to the currently selected node on the nth-column, if selection is set to
 * HILDON_TOUCH_SELECTOR_SINGLE. iter may be NULL if you just want to test if selection
 * has any selected nodes.
 *
 * This function will not work if you use selection is HILDON_TOUCH_SELECTOR_MULTIPLE.
 *
 * See gtk_tree_selection_get_selected for more information
 *
 * Returns: TRUE if was posible to get the iter, FALSE otherwise
 **/
gboolean
hildon_touch_selector_get_active_iter (HildonTouchSelector * selector,
                                       gint column, GtkTreeIter * iter)
{
  GtkTreeSelection *selection = NULL;
  SelectorColumn *current_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);
  g_return_val_if_fail (hildon_touch_selector_get_column_selection_mode (selector)
                        == HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE, FALSE);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector),
                        FALSE);

  current_column = g_slist_nth_data (selector->priv->columns, column);

  selection =
    gtk_tree_view_get_selection (GTK_TREE_VIEW (current_column->tree_view));

  return gtk_tree_selection_get_selected (selection, NULL, iter);
}

/**
 * hildon_touch_selector_set_active_iter
 * @selector: a #HildonTouchSelector
 * @column:   the column to selects
 * @iter:     the #GtkTreeIter to be selected
 *
 * Sets the current iter
 *
 **/
void
hildon_touch_selector_set_active_iter (HildonTouchSelector * selector,
                                       gint column, GtkTreeIter * iter,
                                       gboolean scroll_to)
{
  GtkTreePath *path;
  GtkTreeModel *model;
  GdkRectangle rect;
  SelectorColumn *current_column = NULL;
  GtkTreeSelection *selection = NULL;
  gint y;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (column < hildon_touch_selector_get_num_columns (selector));

  current_column = g_slist_nth_data (selector->priv->columns, column);

  selection = gtk_tree_view_get_selection (current_column->tree_view);

  gtk_tree_selection_select_iter (selection, iter);

  if (scroll_to) {
    model = gtk_tree_view_get_model (current_column->tree_view);
    path = gtk_tree_model_get_path (model, iter);
    gtk_tree_view_get_background_area (current_column->tree_view,
                                       path, NULL, &rect);
    gtk_tree_view_convert_bin_window_to_tree_coords (current_column->tree_view,
                                                     0, rect.y, NULL, &y);
    hildon_pannable_area_scroll_to (HILDON_PANNABLE_AREA (current_column->panarea),
                                    -1, y);
    gtk_tree_path_free (path);
  }
}

/**
 * hildon_touch_selector_get_selected_rows:
 * @selector: a #HildonTouchSelector
 * @column:
 *
 * Creates a list of path of all selected rows at a concrete column. Additionally,
 * if you are planning on modifying the model after calling this function, you may
 * want to convert the returned list into a list of GtkTreeRowReferences. To do this,
 * you can use gtk_tree_row_reference_new().
 *
 * See #GtkTreeSelection:gtk_tree_selection_get_selected_rows for more information
 *
 * Returns: A new GList containing a GtkTreePath for each selected row in the concrete column
 *
 **/
GList *
hildon_touch_selector_get_selected_rows (HildonTouchSelector * selector,
                                         gint column)
{
  GList *result = NULL;
  SelectorColumn *current_column = NULL;
  GtkTreeSelection *selection = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector),
                        NULL);

  current_column = g_slist_nth_data (selector->priv->columns, column);
  selection = gtk_tree_view_get_selection (current_column->tree_view);

  result = gtk_tree_selection_get_selected_rows (selection, NULL);


  return result;
}

GtkTreeModel *
hildon_touch_selector_get_model (HildonTouchSelector * selector, gint column)
{
  SelectorColumn *current_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector),
                        NULL);

  current_column = g_slist_nth_data (selector->priv->columns, column);

  return current_column->model;
}

void
hildon_touch_selector_set_model (HildonTouchSelector * selector,
                                 gint num_column, GtkTreeModel * model)
{
  SelectorColumn *column = NULL;

  g_return_if_fail (HILDON_TOUCH_SELECTOR (selector));
  g_return_if_fail (num_column <
                    hildon_touch_selector_get_num_columns (selector));

  column =
    (SelectorColumn *) g_slist_nth_data (selector->priv->columns, num_column);

  column->model = model;
  gtk_tree_view_set_model (column->tree_view, column->model);
}

/**
 * hildon_touch_selector_get_active_text
 * @selector: the #HildonTouchSelector
 *
 * It return a new gchar that represents the current element(s) selected,
 * using the current print_func.
 *
 * Returns: a new allocated gchar*
 **/
gchar *
hildon_touch_selector_get_current_text (HildonTouchSelector * selector)
{
  gchar *result = NULL;
  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);

  if (selector->priv->print_func) {
    result = (*selector->priv->print_func) (selector);
  } else {
    result = _default_print_func (selector);
  }

  return result;
}

static gboolean
_hildon_touch_selector_center_on_selected_items (gpointer data)
{
  HildonTouchSelector *selector = NULL;
  SelectorColumn *column = NULL;
  GSList *iter_column = NULL;
  GtkTreeIter iter;
  GtkTreePath *path;
  GdkRectangle rect;
  gint y;
  gint i;
  HildonTouchSelectorSelectionMode selection_mode;

  /* ensure to center on the initial values */
  selector = HILDON_TOUCH_SELECTOR (data);

  selection_mode = hildon_touch_selector_get_column_selection_mode (selector);

  iter_column = selector->priv->columns;
  i = 0;
  while (iter_column) {
    column = (SelectorColumn *) iter_column->data;

    if ((i == 0)
        && (selection_mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE)) {
      break;
    }
    if (hildon_touch_selector_get_active_iter (selector, i, &iter)) {
      path = gtk_tree_model_get_path (column->model, &iter);
      gtk_tree_view_get_background_area (GTK_TREE_VIEW
                                         (column->tree_view), path, NULL,
                                         &rect);

      gtk_tree_view_convert_bin_window_to_tree_coords (GTK_TREE_VIEW
                                                       (column->tree_view), 0,
                                                       rect.y, NULL, &y);

      hildon_pannable_area_scroll_to (HILDON_PANNABLE_AREA
                                      (column->panarea), -1, y);

      gtk_tree_path_free (path);
    }
    iter_column = iter_column->next;
    i++;
  }

  return FALSE;
}
