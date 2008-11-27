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
 * @short_description: A selector widget with several columns.
 *
 * #HildonTouchSelector is a selector widget, that allows users to
 * select items from one to many predefined lists. It is very similar
 * to #GtkComboBox, but with several individual pannable columns.
 *
 * Normally, you would use #HildonTouchSelector together with a
 * #HildonPickerDialog activated from a button. For the most common
 * cases, you should use #HildonPickerButton.
 *
 * The composition of each column in the selector is represented by a
 * #GtkTreeModel. To add a new column to a #HildonTouchSelector, use
 * hildon_touch_selector_append_column(). If you want to add a
 * text-only column, without special attributes, use
 * hildon_touch_selector_append_text_column().
 *
 * It is highly recommended that you use only one column
 * #HildonTouchSelector<!-- -->s.
 * If you only need a text only, one column selector, you can create it with
 * hildon_touch_selector_new_text() and populate with
 * hildon_touch_selector_append_text(), hildon_touch_selector_prepend_text(),
 * and hildon_touch_selector_insert_text().
 *
 * If you need a selector widget that also accepts user inputs, you
 * can use #HildonTouchSelectorEntry.
 *
 * The current selection has a string representation. In the most common cases,
 * each column model will contain a text column. You can configure
 * which column in particular using the #HildonTouchSelectorColumn property
 * #HildonTouchSelectorColumn:text-column
 *
 * You can get this string representation using
 * hildon_touch_selector_get_current_text().
 * You can configure how the selection is printed with
 * hildon_touch_selector_set_print_func(), that sets the current hildon touch
 * selector print function. The widget has a default print function, that
 * uses the #HildonTouchSelectorColumn:text-column property on each
 * #HildonTouchSelectorColumn to compose the final representation.
 *
 * If you create the selector using hildon_touch_selector_new_text() you
 * don't need to take care of this property, as the model is created internally.
 * If you create the selector using hildon_touch_selector_new(), you
 * need to specify properly the property for your custom model in order to get a
 * non-empty string representation, or define your custom print function.
 *
 */

/**
 * SECTION:hildon-touch-selector-column
 * @short_description: A visible column in a #HildonTouchSelector
 *
 * #HildonTouchSelectorColumn object represents a visible column in
 * #HildonTouchSelector. It allows to manage the cell renderers related to each
 * column.
 */

#undef HILDON_DISABLE_DEPRECATED

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include "hildon-gtk.h"

#include "hildon-pannable-area.h"
#include "hildon-touch-selector.h"

#define HILDON_TOUCH_SELECTOR_GET_PRIVATE(obj)                          \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_TOUCH_SELECTOR, HildonTouchSelectorPrivate))

G_DEFINE_TYPE (HildonTouchSelector, hildon_touch_selector, GTK_TYPE_VBOX)

/*
 * IMPLEMENTATION NOTES:
 * Struct to maintain the data of each column. The columns are the elements
 * of the widget that belongs properly to the selection behaviour. Although
 * internally the columns are arranged in a private #GtkHBox, as the selector
 * itself is a #GtkVBox, you can add more widgets, like buttons etc., so
 * you finally could have a widget with more elements that the columns, but
 * this doesn't belongs to the selection logic
 */
struct _HildonTouchSelectorColumnPrivate
{
  HildonTouchSelector *parent;    /* the selector that contains this column */
  GtkTreeModel *model;
  gint text_column;
  GtkTreeView *tree_view;

  GtkWidget *panarea;           /* the pannable widget */
};

struct _HildonTouchSelectorPrivate
{
  GSList *columns;              /* the selection columns */
  GtkWidget *hbox;              /* the container for the selector's columns */
  gboolean initial_scroll;      /* whether initial fancy scrolling to selection */

  HildonTouchSelectorPrintFunc print_func;
};

enum
{
  PROP_HAS_MULTIPLE_SELECTION = 1,
  PROP_INITIAL_SCROLL
};

enum
{
  CHANGED,
  LAST_SIGNAL
};

static gint hildon_touch_selector_signals[LAST_SIGNAL] = { 0 };

static void
hildon_touch_selector_get_property              (GObject * object,
                                                 guint prop_id,
                                                 GValue * value,
                                                 GParamSpec * pspec);
static void
hildon_touch_selector_set_property              (GObject *object,
                                                 guint prop_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec);
/* gtkwidget */

/* gtkcontainer */
static void hildon_touch_selector_remove        (GtkContainer * container,
                                                 GtkWidget * widget);
/* private functions */
static void _selection_changed_cb               (GtkTreeSelection * selection,
                                                 gpointer user_data);
static gchar *_default_print_func               (HildonTouchSelector * selector);

static HildonTouchSelectorColumn *_create_new_column (HildonTouchSelector * selector,
                                                 GtkTreeModel * model,
                                                 GtkCellRenderer * renderer,
                                                 va_list args);
static gboolean
_hildon_touch_selector_center_on_selected_items (GtkWidget *widget,
                                                 gpointer data);

static void
_hildon_touch_selector_set_model                (HildonTouchSelector * selector,
                                                 gint num_column,
                                                 GtkTreeModel * model);
static gboolean
_hildon_touch_selector_has_multiple_selection   (HildonTouchSelector * selector);

/* GtkCellLayout implementation (HildonTouchSelectorColumn)*/
static void hildon_touch_selector_column_cell_layout_init         (GtkCellLayoutIface      *iface);

static void hildon_touch_selector_column_cell_layout_pack_start   (GtkCellLayout         *cell_layout,
                                                                   GtkCellRenderer       *cell,
                                                                   gboolean               expand);
static void hildon_touch_selector_column_cell_layout_pack_end     (GtkCellLayout         *cell_layout,
                                                                   GtkCellRenderer       *cell,
                                                                   gboolean               expand);
static void hildon_touch_selector_column_cell_layout_clear        (GtkCellLayout         *cell_layout);
static void hildon_touch_selector_column_cell_layout_add_attribute(GtkCellLayout         *cell_layout,
                                                                   GtkCellRenderer       *cell,
                                                                   const gchar           *attribute,
                                                                   gint                   column);
static void hildon_touch_selector_column_cell_layout_set_cell_data_func (GtkCellLayout         *cell_layout,
                                                                         GtkCellRenderer       *cell,
                                                                         GtkCellLayoutDataFunc  func,
                                                                         gpointer               func_data,
                                                                         GDestroyNotify         destroy);
static void hildon_touch_selector_column_cell_layout_clear_attributes   (GtkCellLayout         *cell_layout,
                                                                         GtkCellRenderer       *cell);
static void hildon_touch_selector_column_cell_layout_reorder       (GtkCellLayout         *cell_layout,
                                                                    GtkCellRenderer       *cell,
                                                                    gint                   position);
static GList *hildon_touch_selector_column_cell_layout_get_cells   (GtkCellLayout         *cell_layout);


static void
hildon_touch_selector_class_init (HildonTouchSelectorClass * class)
{
  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  gobject_class = G_OBJECT_CLASS (class);
  object_class = GTK_OBJECT_CLASS (class);
  widget_class = GTK_WIDGET_CLASS (class);
  container_class = GTK_CONTAINER_CLASS (class);

  /* GObject */
  gobject_class->get_property = hildon_touch_selector_get_property;
  gobject_class->set_property = hildon_touch_selector_set_property;

  /* GtkWidget */

  /* GtkContainer */
  container_class->remove = hildon_touch_selector_remove;

  /* HildonTouchSelector */
  class->changed = NULL;
  class->set_model = _hildon_touch_selector_set_model;

  class->has_multiple_selection = _hildon_touch_selector_has_multiple_selection;

  /* signals */
  /**
   * HildonTouchSelector::changed:
   * @column: the concrete column being modified
   * @widget: the object which received the signal
   *
   * The changed signal is emitted when the active item on any column is changed.
   * This can be due to the user selecting a different item from the list, or
   * due to a call to hildon_touch_selector_select_iter() on one of the columns.
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

  g_object_class_install_property (gobject_class, PROP_HAS_MULTIPLE_SELECTION,
                                   g_param_spec_boolean ("has-multiple-selection",
                                                         "has multiple selection",
                                                         "Whether the widget has multiple "
                                                         "selection (like multiple columns, "
                                                         "multiselection mode, or multiple "
                                                         "internal widgets) and therefore "
                                                         "it may need a confirmation button, "
                                                         "for instance.",
                                                         FALSE,
                                                         G_PARAM_READABLE));

  g_object_class_install_property (G_OBJECT_CLASS (gobject_class),
                                   PROP_INITIAL_SCROLL,
                                   g_param_spec_boolean ("initial-scroll",
                                                         "Initial scroll",
                                                         "Whether to scroll to the"
                                                         "current selection when"
                                                         "the selector is first"
                                                         "shown",
                                                         TRUE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /* style properties */
  /* We need to ensure fremantle mode for the treeview in order to work
     properly. This is not about the appearance, this is about behaviour */
  gtk_rc_parse_string ("style \"fremantle-htst\" {\n"
                       "  GtkWidget::hildon-mode = 1\n"
                       "} widget \"*.fremantle-htst\" style \"fremantle-htst\""
                       "widget_class \"*<HildonPannableArea>.GtkTreeView\" style :highest \"fremantle-htst\"");

  g_type_class_add_private (object_class, sizeof (HildonTouchSelectorPrivate));
}

static void
hildon_touch_selector_get_property (GObject * object,
                                    guint prop_id,
                                    GValue * value, GParamSpec * pspec)
{
  HildonTouchSelectorPrivate *priv = HILDON_TOUCH_SELECTOR (object)->priv;

  switch (prop_id) {
  case PROP_HAS_MULTIPLE_SELECTION:
    g_value_set_boolean (value,
                         hildon_touch_selector_has_multiple_selection (HILDON_TOUCH_SELECTOR (object)));
    break;
  case PROP_INITIAL_SCROLL:
    g_value_set_boolean (value, priv->initial_scroll);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
hildon_touch_selector_set_property (GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec)
{
  HildonTouchSelectorPrivate *priv = HILDON_TOUCH_SELECTOR (object)->priv;

  switch (prop_id) {
  case PROP_INITIAL_SCROLL:
    priv->initial_scroll = g_value_get_boolean (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}


static void
hildon_touch_selector_init (HildonTouchSelector * selector)
{
  selector->priv = HILDON_TOUCH_SELECTOR_GET_PRIVATE (selector);

  GTK_WIDGET_SET_FLAGS (GTK_WIDGET (selector), GTK_NO_WINDOW);
  gtk_widget_set_redraw_on_allocate (GTK_WIDGET (selector), FALSE);

  selector->priv->columns = NULL;

  selector->priv->print_func = NULL;
  selector->priv->initial_scroll = TRUE;
  selector->priv->hbox = gtk_hbox_new (FALSE, 0);

  gtk_box_pack_end (GTK_BOX (selector), selector->priv->hbox,
                    TRUE, TRUE, 0);
  gtk_widget_show (selector->priv->hbox);
}

/*
 * IMPLEMENTATION NOTES:
 * Some people sent questions regarding a missing dispose/finalize function on
 * this widget that could lead to leak memory, so we will clarify this topic.
 *
 * This is not required as #HildonTouchSelector extends #GtkContainer. When the
 * widget is freed, the #GtkContainer freeing memory functions are called. This
 * process includes remove each widget individually, so all the widgets are
 * properly freed.
 *
 * In the same way, this widget redefines gtk_container->remove function, in
 * order to free the column related information if it is required.
 *
 * Please take a look to hildon_touch_selector_remove for more information.
 */

/*------------------------------ GtkContainer ------------------------------ */

/*
 * Required in order to free the column at the columns list
 */
static void
hildon_touch_selector_remove (GtkContainer * container, GtkWidget * widget)
{
  HildonTouchSelector *selector = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (container));
  selector = HILDON_TOUCH_SELECTOR (container);

  /* Remove the extra data related to the columns, if required. */
  if (widget == selector->priv->hbox) {
    g_slist_foreach (selector->priv->columns, (GFunc) g_object_unref, NULL);

    g_slist_free (selector->priv->columns);

    selector->priv->columns = NULL;
  } else {
    g_debug ("Freeing a widget outside the columns logic");
  }

  /* Now remove the widget itself from the container */
  GTK_CONTAINER_CLASS (hildon_touch_selector_parent_class)->remove (container, widget);
}

/* ------------------------------ PRIVATE METHODS ---------------------------- */
/**
 * default_print_func:
 * @selector: a #HildonTouchSelector
 *
 * Default print function
 *
 * Returns: a new string that represents the selected items
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
  gint text_column = -1; 
  HildonTouchSelectorColumn *column = NULL;

  num_columns = hildon_touch_selector_get_num_columns (selector);

  mode = hildon_touch_selector_get_column_selection_mode (selector);

  if ((mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE)
      && (num_columns > 0)) {
    /* In this case we get the first column first */
    selected_rows = hildon_touch_selector_get_selected_rows (selector, 0);
    model = hildon_touch_selector_get_model (selector, 0);
    column = hildon_touch_selector_get_column (selector, 0);
    g_object_get (G_OBJECT(column), "text-column", &text_column, NULL);

    result = g_strdup_printf ("(");
    i = 0;
    for (item = selected_rows; item; item = g_list_next (item)) {
      current_path = item->data;
      gtk_tree_model_get_iter (model, &iter, current_path);

      if (text_column != -1) {
        gtk_tree_model_get (model, &iter, text_column, &current_string, -1);
      }

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
    column = hildon_touch_selector_get_column (selector, i);
    g_object_get (G_OBJECT(column), "text-column", &text_column, NULL);

    if (hildon_touch_selector_get_selected (selector, i, &iter)) {
      if (text_column == -1 ) {
        g_warning ("Trying to use the default print function in HildonTouchSelector, but "
                   "\"text-column\" property is not set for HildonTouchSelectorColumn %p.", column);
        current_string = NULL;
      } else {
        gtk_tree_model_get (model, &iter, text_column, &current_string, -1);
      }

      if (i != 0) {
        aux = g_strconcat (result, ":", current_string, NULL);
        g_free (result);
        result = aux;
      } else {
        result = current_string;
      }
    }
  }

  return result;
}

static void
_selection_changed_cb (GtkTreeSelection * selection, gpointer user_data)
{
  HildonTouchSelector *selector = NULL;
  HildonTouchSelectorColumn *column = NULL;
  gint num_column = -1;

  column = HILDON_TOUCH_SELECTOR_COLUMN (user_data);
  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (column->priv->parent));

  selector = column->priv->parent;

  num_column = g_slist_index (selector->priv->columns, column);

  g_signal_emit (selector, hildon_touch_selector_signals[CHANGED], 0, num_column);
}


static HildonTouchSelectorColumn *
_create_new_column (HildonTouchSelector * selector,
                    GtkTreeModel * model,
                    GtkCellRenderer * renderer, va_list args)
{
  HildonTouchSelectorColumn *new_column = NULL;
  GtkTreeViewColumn *tree_column = NULL;
  GtkTreeView *tv = NULL;
  GtkWidget *panarea = NULL;
  GtkTreeSelection *selection = NULL;
  GtkTreeIter iter;
  gchar *attribute;
  gint value;

  tree_column = gtk_tree_view_column_new ();

  if (renderer != NULL) {
    gtk_tree_view_column_pack_start (tree_column, renderer, TRUE);

    attribute = va_arg (args, gchar *);
    while (attribute != NULL) {
      value = va_arg (args, gint);
      gtk_tree_view_column_add_attribute (tree_column, renderer, attribute,
                                          value);
      attribute = va_arg (args, gchar *);
    }
  }

#ifdef MAEMO_GTK
  tv = GTK_TREE_VIEW (hildon_gtk_tree_view_new (HILDON_UI_MODE_EDIT));
#else
  tv = GTK_TREE_VIEW (gtk_tree_view_new ());
#endif /* MAEMO_GTK */

  gtk_tree_view_set_enable_search (tv, FALSE);

  gtk_tree_view_set_model (tv, model);
  gtk_tree_view_set_rules_hint (tv, TRUE);

  gtk_tree_view_append_column (GTK_TREE_VIEW (tv), tree_column);

  new_column = g_object_new (HILDON_TYPE_TOUCH_SELECTOR_COLUMN, NULL);
  new_column->priv->parent = selector;

  panarea = hildon_pannable_area_new ();

  g_object_set (G_OBJECT (panarea),
                "initial-hint", FALSE, NULL);

  gtk_container_add (GTK_CONTAINER (panarea), GTK_WIDGET (tv));

  new_column->priv->model = model;
  new_column->priv->tree_view = tv;
  new_column->priv->panarea = panarea;

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

  g_signal_connect_after (G_OBJECT (panarea), "realize",
                          G_CALLBACK (_hildon_touch_selector_center_on_selected_items),
                          new_column);

  return new_column;
}


/* ------------------------ HildonTouchSelectorColumn implementation ---------------------- */
G_DEFINE_TYPE_WITH_CODE (HildonTouchSelectorColumn, hildon_touch_selector_column, G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_CELL_LAYOUT,
						hildon_touch_selector_column_cell_layout_init))

enum
{
  PROP_TEXT_COLUMN = 1
};

static void
hildon_touch_selector_column_class_init (HildonTouchSelectorColumnClass *klass);

static void
hildon_touch_selector_column_get_property (GObject *object, guint property_id,
                                           GValue *value, GParamSpec *pspec);

static void
hildon_touch_selector_column_set_property  (GObject *object, guint property_id,
                                            const GValue *value, GParamSpec *pspec);

static void
hildon_touch_selector_column_set_text_column (HildonTouchSelectorColumn *column,
                                              gint text_column);
static gint
hildon_touch_selector_column_get_text_column (HildonTouchSelectorColumn *column);


static void
hildon_touch_selector_column_class_init (HildonTouchSelectorColumnClass *klass)
{
  GObjectClass *gobject_class = NULL;

  gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (HildonTouchSelectorColumnPrivate));

  /* GObject */
  gobject_class->get_property = hildon_touch_selector_column_get_property;
  gobject_class->set_property = hildon_touch_selector_column_set_property;

  g_object_class_install_property (G_OBJECT_CLASS(klass),
                                   PROP_TEXT_COLUMN,
                                   g_param_spec_int ("text-column",
                                                     "Text Column",
                                                     "A column in the data source model to get the strings from.",
                                                     -1,
                                                     G_MAXINT,
                                                     -1,
                                                     G_PARAM_READWRITE));
}

static void
hildon_touch_selector_column_init (HildonTouchSelectorColumn *column)
{
  column->priv = G_TYPE_INSTANCE_GET_PRIVATE (column, HILDON_TYPE_TOUCH_SELECTOR_COLUMN,
                                              HildonTouchSelectorColumnPrivate);
  column->priv->text_column = -1;
}

static void
hildon_touch_selector_column_set_text_column (HildonTouchSelectorColumn *column,
                                              gint text_column)
{
  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (column));
  g_return_if_fail (text_column >= -1);

  column->priv->text_column = text_column;

  g_object_notify (G_OBJECT (column), "text-column");
}

static gint
hildon_touch_selector_column_get_text_column (HildonTouchSelectorColumn *column)
{
  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (column), -1);

  return column->priv->text_column;
}

static void
hildon_touch_selector_column_get_property (GObject *object, guint property_id,
                                           GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  case PROP_TEXT_COLUMN:
    g_value_set_int (value,
                     hildon_touch_selector_column_get_text_column (HILDON_TOUCH_SELECTOR_COLUMN (object)));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
hildon_touch_selector_column_set_property (GObject *object, guint property_id,
                                           const GValue *value, GParamSpec *pspec)
{
  switch (property_id) {
  case PROP_TEXT_COLUMN:
    hildon_touch_selector_column_set_text_column (HILDON_TOUCH_SELECTOR_COLUMN (object),
                                                  g_value_get_int (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

/* ------------------------ GtkCellLayout implementation -------------------- */
static void
hildon_touch_selector_column_cell_layout_init (GtkCellLayoutIface      *iface)
{
  iface->pack_start         = hildon_touch_selector_column_cell_layout_pack_start;
  iface->pack_end           = hildon_touch_selector_column_cell_layout_pack_end;
  iface->clear              = hildon_touch_selector_column_cell_layout_clear;
  iface->add_attribute      = hildon_touch_selector_column_cell_layout_add_attribute;
  iface->set_cell_data_func = hildon_touch_selector_column_cell_layout_set_cell_data_func;
  iface->clear_attributes   = hildon_touch_selector_column_cell_layout_clear_attributes;
  iface->reorder            = hildon_touch_selector_column_cell_layout_reorder;
  iface->get_cells          = hildon_touch_selector_column_cell_layout_get_cells;
}

static void
hildon_touch_selector_column_cell_layout_pack_start (GtkCellLayout         *cell_layout,
                                               GtkCellRenderer       *cell,
                                               gboolean               expand)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT(view_column), cell, expand);

}

static void
hildon_touch_selector_column_cell_layout_pack_end (GtkCellLayout         *cell_layout,
                                             GtkCellRenderer       *cell,
                                             gboolean               expand)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_pack_end (GTK_CELL_LAYOUT(view_column), cell, expand);
}

static void
hildon_touch_selector_column_cell_layout_clear (GtkCellLayout         *cell_layout)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_clear (GTK_CELL_LAYOUT(view_column));
}

static void
hildon_touch_selector_column_cell_layout_add_attribute (GtkCellLayout         *cell_layout,
                                                  GtkCellRenderer       *cell,
                                                  const gchar           *attribute,
                                                  gint                   column)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_add_attribute (GTK_CELL_LAYOUT(view_column), cell, attribute, column);
}


static void
hildon_touch_selector_column_cell_layout_set_cell_data_func (GtkCellLayout         *cell_layout,
                                                       GtkCellRenderer       *cell,
                                                       GtkCellLayoutDataFunc  func,
                                                       gpointer               func_data,
                                                       GDestroyNotify         destroy)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_set_cell_data_func (GTK_CELL_LAYOUT(view_column), cell, func,
                                      func_data, destroy);
}

static void
hildon_touch_selector_column_cell_layout_clear_attributes (GtkCellLayout         *cell_layout,
                                                     GtkCellRenderer       *cell)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_clear_attributes (GTK_CELL_LAYOUT (view_column), cell);
}

static void
hildon_touch_selector_column_cell_layout_reorder (GtkCellLayout         *cell_layout,
                                            GtkCellRenderer       *cell,
                                            gint                   position)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout));
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  gtk_cell_layout_reorder (GTK_CELL_LAYOUT(view_column), cell, position);
}

static GList*
hildon_touch_selector_column_cell_layout_get_cells (GtkCellLayout         *cell_layout)
{
  HildonTouchSelectorColumn *sel_column = NULL;
  GtkTreeViewColumn *view_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR_COLUMN (cell_layout), NULL);
  sel_column = HILDON_TOUCH_SELECTOR_COLUMN (cell_layout);

  view_column = gtk_tree_view_get_column (sel_column->priv->tree_view, 0);

  return gtk_cell_layout_get_cells (GTK_CELL_LAYOUT(view_column));
}

/* ------------------------------ PUBLIC METHODS ---------------------------- */

/**
 * hildon_touch_selector_new:
 *
 * Creates a new empty #HildonTouchSelector.
 *
 * Returns: a new #HildonTouchSelector.
 **/
GtkWidget *
hildon_touch_selector_new (void)
{
  return g_object_new (HILDON_TYPE_TOUCH_SELECTOR, NULL);
}

/**
 * hildon_touch_selector_new_text:
 *
 * Creates a #HildonTouchSelector with a single text column that
 * can be populated conveniently through hildon_touch_selector_append_text(),
 * hildon_touch_selector_prepend_text(), hildon_touch_selector_insert_text().
 *
 * Returns: A new #HildonTouchSelector
 **/
GtkWidget *
hildon_touch_selector_new_text (void)
{
  GtkWidget *selector;
  GtkListStore *store;
  HildonTouchSelectorColumn *column = NULL;

  selector = hildon_touch_selector_new ();
  store = gtk_list_store_new (1, G_TYPE_STRING);

  column = hildon_touch_selector_append_text_column (HILDON_TOUCH_SELECTOR (selector),
                                                     GTK_TREE_MODEL (store), TRUE);

  g_object_set (G_OBJECT (column), "text-column", 0, NULL);

  return selector;
}

/**
 * hildon_touch_selector_append_text:
 * @selector: A #HildonTouchSelector.
 * @text: a non %NULL text string.
 *
 * Appends a new entry in a #HildonTouchSelector created with
 * hildon_touch_selector_new_text().
 **/
void
hildon_touch_selector_append_text (HildonTouchSelector * selector,
                                   const gchar * text)
{
  GtkTreeIter iter;
  GtkTreeModel *model;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (text != NULL);

  model = hildon_touch_selector_get_model (HILDON_TOUCH_SELECTOR (selector), 0);

  g_return_if_fail (GTK_IS_LIST_STORE (model));

  gtk_list_store_append (GTK_LIST_STORE (model), &iter);
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, text, -1);
}

/**
 * hildon_touch_selector_prepend_text:
 * @selector: A #HildonTouchSelector.
 * @text: a non %NULL text string.
 *
 * Prepends a new entry in a #HildonTouchSelector created with
 * hildon_touch_selector_new_text().
 **/
void
hildon_touch_selector_prepend_text (HildonTouchSelector * selector,
                                    const gchar * text)
{
  GtkTreeIter iter;
  GtkTreeModel *model;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (text != NULL);

  model = hildon_touch_selector_get_model (HILDON_TOUCH_SELECTOR (selector), 0);

  g_return_if_fail (GTK_IS_LIST_STORE (model));

  gtk_list_store_prepend (GTK_LIST_STORE (model), &iter);
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, text, -1);
}

/**
 * hildon_touch_selector_insert_text:
 * @selector: a #HildonTouchSelector.
 * @position: the position to insert @text.
 * @text: A non %NULL text string.
 *
 * Inserts a new entry in particular position of a #HildoTouchSelector created
 * with hildon_touch_selector_new_text().
 *
 **/
void
hildon_touch_selector_insert_text (HildonTouchSelector * selector,
                                   gint position, const gchar * text)
{
  GtkTreeIter iter;
  GtkTreeModel *model;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (text != NULL);
  g_return_if_fail (position >= 0);

  model = hildon_touch_selector_get_model (HILDON_TOUCH_SELECTOR (selector), 0);

  g_return_if_fail (GTK_IS_LIST_STORE (model));

  gtk_list_store_insert (GTK_LIST_STORE (model), &iter, position);
  gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, text, -1);
}

/**
 * hildon_touch_selector_append_column
 * @selector: a #HildonTouchSelector
 * @model: the #GtkTreeModel with the data of the column
 * @cell_renderer: The #GtkCellRenderer where to draw each row contents.
 * @Varargs: a %NULL-terminated pair of attributes and column numbers.
 *
 * This functions adds a new column to the widget, whose data will
 * be obtained from the model. Only widgets added this way should used on
 * the selection logic, i.e., the print function, the #HildonTouchPicker::changed
 * signal, etc.
 *
 * You can optionally pass a #GtkCellRenderer in @cell_renderer,
 * together with a %NULL-terminated list of pairs property/value, in
 * the same way you would use gtk_tree_view_column_set_attributes().
 * This will pack @cell_renderer at the start of the column, expanded by default.
 * If you prefer not to add it this way, you can simply pass %NULL to @cell_renderer
 * and use the #GtkCellLayout interface on the returned #HildonTouchSelectorColumn
 * to set your renderers.
 *
 * There is a prerequisite to be considered on models used: text data must
 * be in the first column.
 *
 * This method basically adds a #GtkTreeView to the widget, using the model and
 * the data received.
 *
 * Returns: the new column added added, %NULL otherwise.
 **/

HildonTouchSelectorColumn*
hildon_touch_selector_append_column (HildonTouchSelector * selector,
                                     GtkTreeModel * model,
                                     GtkCellRenderer * cell_renderer, ...)
{
  va_list args;
  HildonTouchSelectorColumn *new_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  g_return_val_if_fail (GTK_IS_TREE_MODEL (model), NULL);

  if (model != NULL) {

    va_start (args, cell_renderer);
    new_column = _create_new_column (selector, model, cell_renderer, args);
    g_object_ref_sink (new_column);
    va_end (args);

    selector->priv->columns = g_slist_append (selector->priv->columns,
                                              new_column);
    gtk_box_pack_start (GTK_BOX (selector->priv->hbox),
                        new_column->priv->panarea,
                        TRUE, TRUE, 6);

    gtk_widget_show_all (new_column->priv->panarea);
  } else {
    return NULL;
  }

  return new_column;
}

/**
 * hildon_touch_selector_append_text_column
 * @selector: a #HildonTouchSelector
 * @model: a #GtkTreeModel with data for the column
 * @center: whether to center the text on the column
 *
 * Equivalent to hildon_touch_selector_append_column(), but using a
 * default text cell renderer. This is the most common use case of the
 * widget.
 *
 * Returns: the new column added, NULL otherwise.
 **/
HildonTouchSelectorColumn*
hildon_touch_selector_append_text_column (HildonTouchSelector * selector,
                                          GtkTreeModel * model, gboolean center)
{
  GtkCellRenderer *renderer = NULL;
  GValue val = { 0, };

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  g_return_val_if_fail (GTK_IS_TREE_MODEL (model), NULL);

  if (model != NULL) {
    renderer = gtk_cell_renderer_text_new ();

    if (center) {
      g_value_init (&val, G_TYPE_FLOAT);
      g_value_set_float (&val, 0.5);
      g_object_set_property (G_OBJECT (renderer), "xalign", &val);
    }

    return hildon_touch_selector_append_column (selector, model, renderer,
                                                "text", 0, NULL);
  } else {
    return NULL;
  }
}

/**
 * hildon_touch_selector_remove_column:
 * @selector: a #HildonTouchSelector
 * @column: the position of the column to be removed
 *
 * Removes a column from @selector.
 *
 * Returns: %TRUE if the column was removed, %FALSE otherwise
 **/
gboolean
hildon_touch_selector_remove_column (HildonTouchSelector * selector, gint column)
{
  HildonTouchSelectorColumn *current_column = NULL;
  HildonTouchSelectorPrivate *priv;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);
  g_return_val_if_fail (column <
                        hildon_touch_selector_get_num_columns (selector), FALSE);

  priv = HILDON_TOUCH_SELECTOR_GET_PRIVATE (selector);
  current_column = g_slist_nth_data (priv->columns, column);

  gtk_container_remove (GTK_CONTAINER (priv->hbox), current_column->priv->panarea);
  priv->columns = g_slist_remove (priv->columns, current_column);
  g_object_unref (current_column);

  return TRUE;
}

/**
 * hildon_touch_selector_set_column_attributes:
 * @selector: a #HildonTouchSelector
 * @num_column: the number of the column whose attributes we're setting
 * @cell_renderer: the #GtkCellRendere we're setting the attributes of
 * @Varargs: A %NULL-terminated list of attributes.
 *
 * Sets the attributes for the given column. The attributes must be given
 * in attribute/column pairs, just like in gtk_tree_view_column_set_attributes().
 * All existing attributes are removed and replaced with the new ones.
 *
 * Deprecated: #HildonTouchSelectorColumn implements #GtkCellLayout, use this
 *             interface instead. See
 *             hildon_touch_selector_get_column().
 *
 **/
void
hildon_touch_selector_set_column_attributes (HildonTouchSelector * selector,
                                             gint num_column,
                                             GtkCellRenderer * cell_renderer,
                                             ...)
{
  va_list args;
  GtkTreeViewColumn *tree_column = NULL;
  HildonTouchSelectorColumn *current_column = NULL;
  gchar *attribute = NULL;
  gint value = 0;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (num_column <
                    hildon_touch_selector_get_num_columns (selector));

  current_column = g_slist_nth_data (selector->priv->columns, num_column);

  tree_column = gtk_tree_view_get_column (current_column->priv->tree_view, 0);
  gtk_tree_view_remove_column (current_column->priv->tree_view, tree_column);

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

  gtk_tree_view_append_column (current_column->priv->tree_view, tree_column);
}

/**
 * hildon_touch_selector_get_num_columns:
 * @selector: a #HildonTouchSelector
 *
 * Gets the number of columns in the #HildonTouchSelector.
 *
 * Returns: the number of columns in @selector.
 **/
gint
hildon_touch_selector_get_num_columns (HildonTouchSelector * selector)
{
  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), -1);

  return g_slist_length (selector->priv->columns);
}

/**
 * hildon_touch_selector_get_column_selection_mode:
 * @selector: a #HildonTouchSelector
 *
 * Gets the selection mode of @selector.
 *
 * Returns: one of #HildonTouchSelectorSelectionMode
 **/
HildonTouchSelectorSelectionMode
hildon_touch_selector_get_column_selection_mode (HildonTouchSelector * selector)
{
  HildonTouchSelectorSelectionMode result =
    HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE;
  GtkSelectionMode treeview_mode = GTK_SELECTION_BROWSE;
  HildonTouchSelectorColumn *column = NULL;
  GtkTreeSelection *selection = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), result);
  g_return_val_if_fail (hildon_touch_selector_get_num_columns (selector) > 0,
                        result);

  column = HILDON_TOUCH_SELECTOR_COLUMN (selector->priv->columns->data);

  selection = gtk_tree_view_get_selection (column->priv->tree_view);
  treeview_mode = gtk_tree_selection_get_mode (selection);


  if (treeview_mode == GTK_SELECTION_MULTIPLE) {
    result = HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE;
  } else {
    result = HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE;
  }

  return result;
}

/**
 * hildon_touch_selector_set_column_selection_mode:
 * @selector: a #HildonTouchSelector
 * @mode: the #HildonTouchSelectorMode for @selector
 *
 * Sets the selection mode for @selector. See #HildonTouchSelectorSelectionMode.
 **/
void
hildon_touch_selector_set_column_selection_mode (HildonTouchSelector * selector,
                                                 HildonTouchSelectorSelectionMode mode)
{
  GtkTreeView *tv = NULL;
  HildonTouchSelectorColumn *column = NULL;
  GtkTreeSelection *selection = NULL;
  GtkSelectionMode treeview_mode = GTK_SELECTION_MULTIPLE;
  GtkTreeIter iter;
  HildonTouchSelectorSelectionMode current_mode;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (hildon_touch_selector_get_num_columns (selector) > 0);

  current_mode = hildon_touch_selector_get_column_selection_mode (selector);

  if (current_mode == mode) {
    return;
  }

  column = HILDON_TOUCH_SELECTOR_COLUMN ((g_slist_nth (selector->priv->columns, 0))->data);
  tv = column->priv->tree_view;

  if (tv) {
    switch (mode) {
    case HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE:
      treeview_mode = GTK_SELECTION_BROWSE;
      break;
    case HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE:
      treeview_mode = GTK_SELECTION_MULTIPLE;
      break;
    }

    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tv));
    gtk_tree_selection_set_mode (selection, treeview_mode);

    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (tv));
    gtk_tree_model_get_iter_first (column->priv->model, &iter);
    gtk_tree_selection_unselect_all (selection);
    gtk_tree_selection_select_iter (selection, &iter);
  }

}

/**
 * hildon_touch_selector_set_print_func:
 * @selector: a #HildonTouchSelector
 * @func: a #HildonTouchSelectorPrintFunc function
 *
 * Sets the function to be used by hildon_touch_selector_get_current_text()
 * to produce a text representation of the currently selected items in @selector.
 * The default function will return a concatenation of comma separated items
 * selected in each column in @selector. Use this to override this method if you
 * need a particular representation for your application.
 *
 **/
void
hildon_touch_selector_set_print_func (HildonTouchSelector * selector,
                                      HildonTouchSelectorPrintFunc func)
{
  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));

  selector->priv->print_func = func;
}

/**
 * hildon_touch_selector_get_print_func:
 * @selector: a #HildonTouchSelector
 *
 * Gets the #HildonTouchSelectorPrintFunc currently used. See
 * hildon_touch_selector_set_print_func().
 *
 * Returns: a #HildonTouchSelectorPrintFunc or %NULL if the default
 * one is currently used.
 **/
HildonTouchSelectorPrintFunc
hildon_touch_selector_get_print_func (HildonTouchSelector * selector)
{
  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);

  return selector->priv->print_func;
}

/**
 * hildon_touch_selector_set_active:
 * @selector: a #HildonTouchSelector
 * @column: column number
 * @index: the index of the item to select, or -1 to have no active item
 *
 * Sets the active item of the #HildonTouchSelector to @index. The
 * column number is taken from @column.
 *
 * @selector must be in %HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE
 **/
void
hildon_touch_selector_set_active                (HildonTouchSelector *selector,
                                                 gint                 column,
                                                 gint                 index)
{
  GtkTreeSelection *selection = NULL;
  HildonTouchSelectorColumn *current_column = NULL;
  HildonTouchSelectorSelectionMode mode;
  GtkTreePath *path;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (column < hildon_touch_selector_get_num_columns (selector));
  mode = hildon_touch_selector_get_column_selection_mode (selector);
  g_return_if_fail (mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE);

  current_column = g_slist_nth_data (selector->priv->columns, column);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (current_column->priv->tree_view));
  path = gtk_tree_path_new_from_indices (index, -1);
  gtk_tree_selection_unselect_all (selection);
  if (index != -1)
    gtk_tree_selection_select_path (selection, path);

  gtk_tree_path_free (path);
}

/**
 * hildon_touch_selector_get_active:
 * @selector: a #HildonTouchSelector
 * @column: column number
 *
 * Returns the index of the currently active item in column number
 * @column, or -1 if there's no active item.
 *
 * @selector must be in %HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE
 *
 * Returns: an integer which is the index of the currently active
 * item, or -1 if there's no active item.
 **/
gint
hildon_touch_selector_get_active                (HildonTouchSelector *selector,
                                                 gint                 column)
{
  GtkTreeSelection *selection = NULL;
  HildonTouchSelectorColumn *current_column = NULL;
  HildonTouchSelectorSelectionMode mode;
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreePath *path;
  gint index;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), -1);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector), -1);
  mode = hildon_touch_selector_get_column_selection_mode (selector);
  g_return_val_if_fail (mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE, -1);

  current_column = g_slist_nth_data (selector->priv->columns, column);

  selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (current_column->priv->tree_view));
  model = gtk_tree_view_get_model (GTK_TREE_VIEW (current_column->priv->tree_view));

  gtk_tree_selection_get_selected (selection, NULL, &iter);
  path = gtk_tree_model_get_path (model, &iter);
  index = (gtk_tree_path_get_indices (path))[0];

  gtk_tree_path_free (path);

  return index;
}

/**
 * hildon_touch_selector_get_selected:
 * @selector: a #HildonTouchSelector
 * @column: the column number we want to get the element
 * @iter: #GtkTreeIter currently selected
 *
 * Sets @iter to the currently selected node on the nth-column, if selection is
 * set to %HILDON_TOUCH_SELECTOR_SINGLE or %HILDON_TOUCH_SELECTOR_MULTIPLE with
 * a column different that the first one. @iter may be %NULL if you just want to
 * test if selection has any selected items.
 *
 * This function will not work if selection is in
 * %HILDON_TOUCH_SELECTOR_MULTIPLE mode and the column is the first one.
 *
 * See gtk_tree_selection_get_selected() for more information.
 *
 * Returns: %TRUE if @iter was correctly set, %FALSE otherwise
 **/
gboolean
hildon_touch_selector_get_selected (HildonTouchSelector * selector,
                                    gint column, GtkTreeIter * iter)
{
  GtkTreeSelection *selection = NULL;
  HildonTouchSelectorColumn *current_column = NULL;
  HildonTouchSelectorSelectionMode mode;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector),
                        FALSE);
  mode = hildon_touch_selector_get_column_selection_mode (selector);
  g_return_val_if_fail
    ((mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE) ||
     ((mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE)&&(column>0)),
     FALSE);

  current_column = g_slist_nth_data (selector->priv->columns, column);

  selection =
    gtk_tree_view_get_selection (GTK_TREE_VIEW (current_column->priv->tree_view));

  return gtk_tree_selection_get_selected (selection, NULL, iter);
}

/**
 * hildon_touch_selector_select_iter
 * @selector: a #HildonTouchSelector
 * @column:   the column to selects
 * @iter:     the #GtkTreeIter to be selected
 * @scroll_to: whether to smoothly scroll to the item
 *
 * Sets the currently selected item in the column @column to the one pointed by @iter,
 * optionally smoothly scrolling to it.
 *
 **/
void
hildon_touch_selector_select_iter (HildonTouchSelector * selector,
                                   gint column, GtkTreeIter * iter,
                                   gboolean scroll_to)
{
  GtkTreePath *path;
  GtkTreeModel *model;
  GdkRectangle rect;
  HildonTouchSelectorColumn *current_column = NULL;
  GtkTreeView *tv = NULL;
  GtkTreeSelection *selection = NULL;
  gint y;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (column < hildon_touch_selector_get_num_columns (selector));

  current_column = g_slist_nth_data (selector->priv->columns, column);

  tv = current_column->priv->tree_view;
  selection = gtk_tree_view_get_selection (tv);
  model = gtk_tree_view_get_model (tv);
  path = gtk_tree_model_get_path (model, iter);

  gtk_tree_selection_select_iter (selection, iter);

  if (scroll_to) {
    gtk_tree_view_get_background_area (tv,
                                       path, NULL, &rect);
    gtk_tree_view_convert_bin_window_to_tree_coords (tv,
                                                     0, rect.y, NULL, &y);
    hildon_pannable_area_scroll_to (HILDON_PANNABLE_AREA (current_column->priv->panarea),
                                    -1, y);
  }
  gtk_tree_path_free (path);
}

/**
 * hildon_touch_selector_unselect_iter
 * @selector: a #HildonTouchSelector
 * @column:   the column to unselects from
 * @iter:     the #GtkTreeIter to be unselected
 *
 * Unselect the item pointed by @iter in the column @column
 *
 **/

void hildon_touch_selector_unselect_iter (HildonTouchSelector * selector,
                                          gint column,
                                          GtkTreeIter * iter)
{
  HildonTouchSelectorColumn *current_column = NULL;
  GtkTreeSelection *selection = NULL;

  g_return_if_fail (HILDON_IS_TOUCH_SELECTOR (selector));
  g_return_if_fail (column < hildon_touch_selector_get_num_columns (selector));

  current_column = g_slist_nth_data (selector->priv->columns, column);
  selection = gtk_tree_view_get_selection (current_column->priv->tree_view);
  gtk_tree_selection_unselect_iter (selection, iter);
}

/**
 * hildon_touch_selector_get_selected_rows:
 * @selector: a #HildonTouchSelector
 * @column: the position of the column to get the selected rows from
 *
 * Creates a list of #GtkTreePath<!-- -->s of all selected rows in a column. Additionally,
 * if you to plan to modify the model after calling this function, you may
 * want to convert the returned list into a list of GtkTreeRowReferences. To do this,
 * you can use gtk_tree_row_reference_new().
 *
 * See gtk_tree_selection_get_selected_rows() for more information.
 *
 * Returns: A new #GList containing a #GtkTreePath for each selected row in the column @column.
 *
 **/
GList *
hildon_touch_selector_get_selected_rows (HildonTouchSelector * selector,
                                         gint column)
{
  GList *result = NULL;
  HildonTouchSelectorColumn *current_column = NULL;
  GtkTreeSelection *selection = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector),
                        NULL);

  current_column = g_slist_nth_data (selector->priv->columns, column);
  selection = gtk_tree_view_get_selection (current_column->priv->tree_view);

  result = gtk_tree_selection_get_selected_rows (selection, NULL);

  return result;
}

/**
 * hildon_touch_selector_get_model:
 * @selector: a #HildonTouchSelector
 * @column: the position of the column in @selector
 *
 * Gets the model of a column of @selector.
 *
 * Returns: the #GtkTreeModel for the column @column of @selector.
 **/
GtkTreeModel *
hildon_touch_selector_get_model (HildonTouchSelector * selector, gint column)
{
  HildonTouchSelectorColumn *current_column = NULL;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  g_return_val_if_fail (column < hildon_touch_selector_get_num_columns (selector),
                        NULL);

  current_column = g_slist_nth_data (selector->priv->columns, column);

  return current_column->priv->model;
}

static void
_hildon_touch_selector_set_model (HildonTouchSelector * selector,
                                  gint column, GtkTreeModel * model)
{
  HildonTouchSelectorColumn *current_column = NULL;

  current_column =
    HILDON_TOUCH_SELECTOR_COLUMN (g_slist_nth_data (selector->priv->columns, column));

  current_column->priv->model = model;
  gtk_tree_view_set_model (current_column->priv->tree_view,
                           current_column->priv->model);
}

/**
 * hildon_touch_selector_set_model:
 * @selector: a #HildonTouchSelector
 * @column: the position of the column to set the model to
 * @model: a #GtkTreeModel
 *
 * Sets the #GtkTreeModel for a particular column in @model.
 **/
void
hildon_touch_selector_set_model (HildonTouchSelector * selector,
                                 gint column, GtkTreeModel * model)
{
  g_return_if_fail (HILDON_TOUCH_SELECTOR (selector));
  g_return_if_fail (column < hildon_touch_selector_get_num_columns (selector));

  HILDON_TOUCH_SELECTOR_GET_CLASS (selector)->set_model (selector, column, model);
}

/**
 * hildon_touch_selector_get_current_text:
 * @selector: a #HildonTouchSelector
 *
 * Returns a string representing the currently selected items for
 * each column of @selector. See hildon_touch_selector_set_print_func().
 *
 * Returns: a newly allocated string.
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
_hildon_touch_selector_center_on_selected_items (GtkWidget *widget,
                                                 gpointer data)
{
  HildonTouchSelector *selector = NULL;
  HildonTouchSelectorColumn *column = NULL;
  GtkTreeIter iter;
  GtkTreePath *path = NULL;
  GdkRectangle rect;
  gint y;
  gint num_column = -1;
  HildonTouchSelectorSelectionMode selection_mode;

  column = HILDON_TOUCH_SELECTOR_COLUMN (data);
  selector = column->priv->parent;

  if (!selector->priv->initial_scroll) {
    return FALSE;
  }
  selection_mode = hildon_touch_selector_get_column_selection_mode (selector);
  num_column = g_slist_index (selector->priv->columns, column);

  if ((num_column == 0)
      && (selection_mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE)) {

    return FALSE;
  }

  if (hildon_touch_selector_get_selected (selector, num_column, &iter)) {
    path = gtk_tree_model_get_path (column->priv->model, &iter);
    gtk_tree_view_get_background_area (GTK_TREE_VIEW
                                       (column->priv->tree_view), path, NULL,
                                       &rect);

    gtk_tree_view_convert_bin_window_to_tree_coords (GTK_TREE_VIEW
                                                     (column->priv->tree_view), 0,
                                                     rect.y, NULL, &y);

    hildon_pannable_area_scroll_to (HILDON_PANNABLE_AREA
                                    (column->priv->panarea), -1, y);

    gtk_tree_path_free (path);
  }

  return FALSE;
}

static gboolean
_hildon_touch_selector_has_multiple_selection (HildonTouchSelector * selector)
{
  HildonTouchSelectorSelectionMode mode = HILDON_TOUCH_SELECTOR_SELECTION_MODE_SINGLE;
  gint n_columns = 0;

  n_columns = hildon_touch_selector_get_num_columns (selector);
  mode = hildon_touch_selector_get_column_selection_mode (selector);

  return ((n_columns > 1) || (mode == HILDON_TOUCH_SELECTOR_SELECTION_MODE_MULTIPLE));
}

/**
 * hildon_touch_selector_has_multiple_selection:
 * @selector: A #HildonTouchSelector
 *
 * Determines whether @selector is complex enough to actually require an
 * extra selection step than only picking an item. This is normally %TRUE
 * if @selector has multiple columns, multiple selection, or when it is a
 * more complex widget, like #HildonTouchSelectorEntry.
 *
 * This information is useful for widgets containing a #HildonTouchSelector,
 * like #HildonPickerDialog, that could need a "Done" button, in case that
 * its internal #HildonTouchSelector has multiple columns, for instance.
 *
 * Returns: %TRUE if @selector requires multiple selection steps.
 **/
gboolean
hildon_touch_selector_has_multiple_selection (HildonTouchSelector * selector)
{
  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), FALSE);

  return HILDON_TOUCH_SELECTOR_GET_CLASS (selector)->has_multiple_selection (selector);
}


/**
 * hildon_touch_selector_get_column:
 * @selector: A #HildonTouchSelector
 * @column: a column number
 *
 * Use this method to retrieve a #HildonTouchSelectorColumn. Then, you can use
 * the #GtkCellLayout interface to set up the layout of the column.
 *
 * Returns: the @column<!-- -->-th #HildonTouchSelectorColumn in @selector
 **/
HildonTouchSelectorColumn *
hildon_touch_selector_get_column (HildonTouchSelector * selector,
                                  gint column)
{
  gint num_columns = -1;

  g_return_val_if_fail (HILDON_IS_TOUCH_SELECTOR (selector), NULL);
  num_columns = hildon_touch_selector_get_num_columns (selector);
  g_return_val_if_fail (column < num_columns && column >= 0, NULL);

  return g_slist_nth_data (selector->priv->columns, column);
}
