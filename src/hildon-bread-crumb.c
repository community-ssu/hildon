/*
 * This file is a part of hildon
 *
 * Copyright (C) 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 * Author: Xan Lopez <xan.lopez@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

#include "hildon-bread-crumb.h"

#define HILDON_BREAD_CRUMB_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_BREAD_CRUMB, HildonBreadCrumbPrivate))

struct _HildonBreadCrumbPrivate
{
  GtkWidget *icon;
  GtkWidget *label;
  GtkWidget *arrow;
  gchar *text;

  gboolean show_separator;
};

/* Signals */

enum {
  LAST_SIGNAL
};

/* Properties */

enum {
  PROP_0,
  PROP_TEXT,
  PROP_SHOW_SEPARATOR
};

/*
static guint bread_crumb_item_signals[LAST_SIGNAL] = { 0 };
*/

/* GType methods */

static void hildon_bread_crumb_finalize (GObject *object);
static void hildon_bread_crumb_set_property (GObject *object, guint prop_id,
						  const GValue *value, GParamSpec *pspec);
static void hildon_bread_crumb_get_property (GObject *object, guint prop_id,
						  GValue *value, GParamSpec *pspec);

G_DEFINE_TYPE (HildonBreadCrumb, hildon_bread_crumb, GTK_TYPE_BUTTON)

static void
hildon_bread_crumb_class_init (HildonBreadCrumbClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*)klass;

  /* GObject signals */
  gobject_class->finalize = hildon_bread_crumb_finalize;
  gobject_class->set_property = hildon_bread_crumb_set_property;
  gobject_class->get_property = hildon_bread_crumb_get_property;

  /* Properties */
  g_object_class_install_property (gobject_class,
				   PROP_TEXT,
				   g_param_spec_string ("text",
							"Text",
							"Text displayed as the name of the item",
							NULL,
							G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (gobject_class,
				   PROP_SHOW_SEPARATOR,
				   g_param_spec_boolean ("show-separator",
							 "Show separator",
							 "Show the separator attached to the item",
							 TRUE,
							 G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /* Private data */
  g_type_class_add_private (gobject_class, sizeof (HildonBreadCrumbPrivate));
}

static void
hildon_bread_crumb_init (HildonBreadCrumb *item)
{
  GtkWidget *hbox;
  HildonBreadCrumbPrivate *priv = HILDON_BREAD_CRUMB_GET_PRIVATE (item);

  item->priv = priv;

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (item), hbox);

  priv->arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_NONE);
  gtk_widget_set_name (priv->arrow, "hildon-bread-crumb-separator-arrow");
  gtk_box_pack_start (GTK_BOX (hbox), priv->arrow, FALSE, FALSE, 0);

  if (priv->show_separator)
    gtk_widget_show (priv->arrow);

  priv->label = gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (hbox), priv->label, TRUE, TRUE, 0);
  gtk_widget_show (priv->label);

  gtk_widget_show (hbox);
}

static void
hildon_bread_crumb_finalize (GObject *object)
{
  HildonBreadCrumbPrivate *priv = HILDON_BREAD_CRUMB (object)->priv;

  g_free (priv->text);

  G_OBJECT_CLASS (hildon_bread_crumb_parent_class)->finalize (object);
}

static void
hildon_bread_crumb_set_property (GObject *object, guint prop_id,
				      const GValue *value, GParamSpec *pspec)
{
  HildonBreadCrumb *item = HILDON_BREAD_CRUMB (object);

  switch (prop_id)
    {
    case PROP_TEXT:
      hildon_bread_crumb_set_text (item, g_value_get_string (value));
      break;
    case PROP_SHOW_SEPARATOR:
      hildon_bread_crumb_set_show_separator (item, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hildon_bread_crumb_get_property (GObject *object, guint prop_id,
				      GValue *value, GParamSpec *pspec)
{
  HildonBreadCrumb *item = HILDON_BREAD_CRUMB (object);

  switch (prop_id)
    {
    case PROP_TEXT:
      g_value_set_string (value, item->priv->text);
      break;
    case PROP_SHOW_SEPARATOR:
      g_value_set_boolean (value, item->priv->show_separator);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

void
hildon_bread_crumb_set_text (HildonBreadCrumb *item,
				  const gchar *text)
{
  HildonBreadCrumbPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB (item));

  priv = item->priv;

  if (priv->text)
    g_free (priv->text);

  priv->text = g_strdup (text);

  gtk_label_set_label (GTK_LABEL (priv->label), text);
  gtk_label_set_ellipsize (GTK_LABEL (item->priv->label),
			   PANGO_ELLIPSIZE_START);
  gtk_widget_show (priv->label);

  g_object_notify (G_OBJECT (item), "text");
}

const gchar*
hildon_bread_crumb_get_text (HildonBreadCrumb *item)
{
  HildonBreadCrumbPrivate *priv;

  g_return_val_if_fail (HILDON_IS_BREAD_CRUMB (item), NULL);

  priv = item->priv;

  return priv->text;
}

void
hildon_bread_crumb_set_show_separator (HildonBreadCrumb *item,
					    gboolean show_separator)
{
  HildonBreadCrumbPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB (item));

  priv = item->priv;

  if (priv->show_separator == show_separator)
    return;
  
  priv->show_separator = show_separator;

  if (show_separator)
    gtk_widget_show (priv->arrow);
  else
    gtk_widget_hide (priv->arrow);

  g_object_notify (G_OBJECT (item), "show-separator");
}

void
hildon_bread_crumb_get_natural_size (HildonBreadCrumb *item,
					  gint *natural_width,
					  gint *natural_height)
{
  GtkRequisition req;
  gint width, height;
  PangoLayout *layout;
  HildonBreadCrumbPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB (item));

  priv = item->priv;

  gtk_widget_size_request (GTK_WIDGET (item), &req);
  if (natural_width)
    *natural_width = req.width;

  if (natural_height)
    *natural_height = req.height;
  /* Substract the size request of the label */
  gtk_widget_size_request (priv->label, &req);
  if (natural_width)
    *natural_width -= req.width;
  layout = gtk_widget_create_pango_layout (priv->label, priv->text);
  pango_layout_get_size (layout, &width, &height);

  if (natural_width)
    *natural_width += PANGO_PIXELS (width);
  /* Border width */
  if (natural_width)
    *natural_width += GTK_CONTAINER (item)->border_width * 2;

  if (natural_height)
    *natural_height += GTK_CONTAINER (item)->border_width * 2;
}

GtkWidget*
hildon_bread_crumb_new (const gchar* text)
{
  return GTK_WIDGET (g_object_new (HILDON_TYPE_BREAD_CRUMB,
				   "text", text,
				   NULL));
}

