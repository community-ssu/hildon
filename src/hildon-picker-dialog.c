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
 * SECTION:hildon-picker-dialog
 * @short_description: A utility widget that shows a #HildonTouchPicker widget
 *
 * HildonPickerDialog is a utility widget that shows a #HildonTouchPicker widget in
 * a new dialog (see #GtkDialog for more information about this issue)
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include <libintl.h>
#include <gtk/gtkmarshal.h>

#include "hildon-touch-picker.h"
#include "hildon-picker-dialog.h"

#define _(String)  dgettext("hildon-libs", String)

#define HILDON_PICKER_DIALOG_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_PICKER_DIALOG, HildonPickerDialogPrivate))

G_DEFINE_TYPE (HildonPickerDialog, hildon_picker_dialog, GTK_TYPE_DIALOG)

struct _HildonPickerDialogPrivate
{
  GtkWidget *picker;
  GtkWidget *separator;
  GtkWidget *button;

  GtkWidget *title_label;

  gulong signal_id;
};

/* properties */
enum
{
  PROP_0,
  PROP_DONE_BUTTON_TEXT,
  PROP_LAST
};

enum
{
  RESPONSE,
  LAST_SIGNAL
};

#define DEFAULT_DONE_BUTTON_TEXT        "Done"

static void
hildon_picker_dialog_set_property (GObject * object,
                                   guint prop_id,
                                   const GValue * value,
                                   GParamSpec * pspec);

static void
hildon_picker_dialog_get_property (GObject * object,
                                   guint prop_id,
                                   GValue * value, GParamSpec * pspec);
/* gtkwidget */
static void hildon_picker_dialog_realize (GtkWidget * widget);

/* private functions */
static gboolean requires_done_button (HildonPickerDialog * dialog);

static void
_select_on_picker_changed_cb (HildonTouchPicker * dialog,
                              gint column, gpointer data);

static gboolean
_hildon_picker_dialog_set_picker (HildonPickerDialog * dialog,
                                  HildonTouchPicker * picker);

static void
_update_title_on_picker_changed_cb (HildonTouchPicker * picker,
                                    gint column, gpointer data);

static void
hildon_picker_dialog_class_init (HildonPickerDialogClass * class)
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
  gobject_class->set_property = hildon_picker_dialog_set_property;
  gobject_class->get_property = hildon_picker_dialog_get_property;

  /* GtkWidget */
  widget_class->realize = hildon_picker_dialog_realize;

  /* HildonPickerDialog */
  class->set_picker = _hildon_picker_dialog_set_picker;

  /* signals */

  /* properties */
  /**
   * HildonPickerDialog
   *
   * Button label
   */
  g_object_class_install_property (gobject_class,
                                   PROP_DONE_BUTTON_TEXT,
                                   g_param_spec_string ("done-button-text",
                                                        "Done Button Label",
                                                        "Done Button Label",
                                                        DEFAULT_DONE_BUTTON_TEXT,
                                                        G_PARAM_READABLE |
                                                        G_PARAM_WRITABLE |
                                                        G_PARAM_CONSTRUCT));

  g_type_class_add_private (object_class, sizeof (HildonPickerDialogPrivate));
}


static void
hildon_picker_dialog_init (HildonPickerDialog * dialog)
{
  GtkWidget *separator = NULL;

  dialog->priv = HILDON_PICKER_DIALOG_GET_PRIVATE (dialog);

  dialog->priv->picker = NULL;
  dialog->priv->button =
    gtk_dialog_add_button (GTK_DIALOG (dialog), "", GTK_RESPONSE_OK);

  dialog->priv->title_label = gtk_label_new ("default value");
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
                      dialog->priv->title_label, FALSE, FALSE, 0);
  separator = gtk_hseparator_new ();
  gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
                      separator, FALSE, FALSE, 0);

  dialog->priv->signal_id = 0;

/*   gtk_widget_show (dialog->priv->title_label); */
/*   gtk_widget_show (separator); */
}


static void
hildon_picker_dialog_set_property (GObject * object,
                                   guint param_id,
                                   const GValue * value, GParamSpec * pspec)
{
  HildonPickerDialogPrivate *priv = HILDON_PICKER_DIALOG_GET_PRIVATE (object);
  g_assert (priv);

  switch (param_id) {
  case PROP_DONE_BUTTON_TEXT:
    hildon_picker_dialog_set_done_label (HILDON_PICKER_DIALOG (object),
                                         g_value_get_string (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
    break;
  }
}

static void
hildon_picker_dialog_get_property (GObject * object,
                                   guint param_id,
                                   GValue * value, GParamSpec * pspec)
{
  HildonPickerDialog *dialog;
  HildonPickerDialogPrivate *priv;

  dialog = HILDON_PICKER_DIALOG (object);
  priv = HILDON_PICKER_DIALOG_GET_PRIVATE (object);

  switch (param_id) {
  case PROP_DONE_BUTTON_TEXT:
    g_value_set_string (value, hildon_picker_dialog_get_done_label (dialog));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
    break;
  }
}


static void
hildon_picker_dialog_realize (GtkWidget * widget)
{
  GTK_WIDGET_CLASS (hildon_picker_dialog_parent_class)->realize (widget);

/*   gdk_window_set_decorations (widget->window, GDK_DECOR_BORDER); */
}

/* ------------------------------ PRIVATE METHODS ---------------------------- */
static void
_select_on_picker_changed_cb (HildonTouchPicker * picker,
                              gint column, gpointer data)
{
  HildonPickerDialog *dialog = NULL;

  g_return_if_fail (HILDON_IS_PICKER_DIALOG (data));

  dialog = HILDON_PICKER_DIALOG (data);

  gtk_dialog_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
}

static void
_update_title_on_picker_changed_cb (HildonTouchPicker * picker,
                                    gint column, gpointer data)
{
  HildonPickerDialog *dialog = NULL;
  gchar *new_title = NULL;

  g_return_if_fail (HILDON_IS_PICKER_DIALOG (data));

  dialog = HILDON_PICKER_DIALOG (data);

  new_title = hildon_touch_picker_get_current_text (picker);

  if (dialog->priv->title_label != NULL) {
/*       gtk_label_set_text (GTK_LABEL(picker->priv->title_label), new_title); */
  }
  gtk_window_set_title (GTK_WINDOW (dialog), new_title);

  g_free (new_title);
}


void
hildon_picker_dialog_set_done_label (HildonPickerDialog * dialog,
                                     const gchar * label)
{
  HildonPickerDialogPrivate *priv;

  g_return_if_fail (HILDON_IS_PICKER_DIALOG (dialog));
  g_return_if_fail (label != NULL);

  priv = HILDON_PICKER_DIALOG_GET_PRIVATE (dialog);

  gtk_button_set_label (GTK_BUTTON (priv->button), label);
}

const gchar *
hildon_picker_dialog_get_done_label (HildonPickerDialog * dialog)
{
  HildonPickerDialogPrivate *priv;

  g_return_val_if_fail (HILDON_IS_PICKER_DIALOG (dialog), NULL);

  priv = HILDON_PICKER_DIALOG_GET_PRIVATE (dialog);

  return gtk_button_get_label (GTK_BUTTON (priv->button));
}

static gboolean
requires_done_button (HildonPickerDialog * dialog)
{
  gint n_columns = 0;
  HildonTouchPickerSelectionMode mode =
    HILDON_TOUCH_PICKER_SELECTION_MODE_SINGLE;

  n_columns =
    hildon_touch_picker_get_num_columns (HILDON_TOUCH_PICKER
                                         (dialog->priv->picker));
  mode =
    hildon_touch_picker_get_column_selection_mode (HILDON_TOUCH_PICKER
                                                   (dialog->priv->picker));

  return ((n_columns > 1)
          || (mode == HILDON_TOUCH_PICKER_SELECTION_MODE_MULTIPLE));
}


/* ------------------------------ PUBLIC METHODS ---------------------------- */

/**
 * hildon_picker_dialog_new:
 * @:
 *
 * Creates a new #HildonPickerDialog
 *
 * Returns: a new #HildonPickerDialog
 **/
GtkWidget *
hildon_picker_dialog_new (GtkWindow * parent)
{
  GtkDialog *dialog = NULL;

  dialog = g_object_new (HILDON_TYPE_PICKER_DIALOG, NULL);

  if (parent) {
    gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
  }

  return GTK_WIDGET (dialog);
}


static gboolean
_hildon_picker_dialog_set_picker (HildonPickerDialog * dialog,
                                  HildonTouchPicker * picker)
{
  if (dialog->priv->picker != NULL) {
    gtk_container_remove (GTK_CONTAINER (GTK_DIALOG (dialog)->vbox),
                          dialog->priv->picker);
    g_object_unref (dialog->priv->picker);
    dialog->priv->picker = NULL;
  }

  dialog->priv->picker = GTK_WIDGET (picker);

  if (dialog->priv->picker != NULL) {
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox),
                        dialog->priv->picker, TRUE, TRUE, 0);
    gtk_widget_show (dialog->priv->picker);
    g_object_ref (dialog->priv->picker);
  }

  if (dialog->priv->signal_id) {
    g_signal_handler_disconnect (dialog->priv->picker,
                                 dialog->priv->signal_id);
  }

  if (requires_done_button (dialog)) {
    gtk_dialog_set_has_separator (GTK_DIALOG (dialog), TRUE);
    gtk_widget_show (GTK_DIALOG (dialog)->action_area);
    /* update the title */
    dialog->priv->signal_id =
      g_signal_connect (G_OBJECT (dialog->priv->picker), "changed",
                        G_CALLBACK (_update_title_on_picker_changed_cb),
                        dialog);
  } else {
    gtk_dialog_set_has_separator (GTK_DIALOG (dialog), FALSE);
    gtk_widget_hide (GTK_DIALOG (dialog)->action_area);
    dialog->priv->signal_id =
      g_signal_connect (G_OBJECT (dialog->priv->picker), "changed",
                        G_CALLBACK (_select_on_picker_changed_cb), dialog);
  }

  return TRUE;
}

gboolean
hildon_picker_dialog_set_picker (HildonPickerDialog * dialog,
                                 HildonTouchPicker * picker)
{
  g_return_val_if_fail (HILDON_IS_PICKER_DIALOG (dialog), FALSE);
  g_return_val_if_fail (HILDON_IS_TOUCH_PICKER (picker), FALSE);

  return HILDON_PICKER_DIALOG_GET_CLASS (dialog)->set_picker (dialog, picker);
}

HildonTouchPicker *
hildon_picker_dialog_get_picker (HildonPickerDialog * dialog)
{
  g_return_val_if_fail (HILDON_IS_PICKER_DIALOG (dialog), NULL);

  return HILDON_TOUCH_PICKER (dialog->priv->picker);
}
