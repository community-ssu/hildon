/*
 * This file is a part of hildon
 *
 * Copyright (C) 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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

#undef HILDON_DISABLE_DEPRECATED

#include "hildon-bread-crumb-widget.h"
#include "hildon-defines.h"

#define HILDON_BREAD_CRUMB_WIDGET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), HILDON_TYPE_BREAD_CRUMB_WIDGET, HildonBreadCrumbWidgetPrivate))

struct _HildonBreadCrumbWidgetPrivate
{
  GtkWidget *hbox;
  GtkWidget *label;
  GtkWidget *icon;
  GtkWidget *arrow;
  gchar *text;

  GtkPositionType icon_position;
  gboolean constructed;
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
  PROP_ICON,
  PROP_ICON_POSITION,
  PROP_SHOW_SEPARATOR
};

/*
static guint bread_crumb_item_signals[LAST_SIGNAL] = { 0 };
*/

/* GType methods */

static void hildon_bread_crumb_widget_finalize (GObject *object);
static void hildon_bread_crumb_widget_set_property (GObject *object, guint prop_id,
                                             const GValue *value, GParamSpec *pspec);
static void hildon_bread_crumb_widget_get_property (GObject *object, guint prop_id,
                                             GValue *value, GParamSpec *pspec);
static GObject*	hildon_bread_crumb_widget_constructor (GType                  type,
                                                       guint                  n_construct_properties,
                                                       GObjectConstructParam *construct_params);
static void hildon_bread_crumb_widget_set_contents (HildonBreadCrumbWidget *bread_crumb);

static void hildon_bread_crumb_widget_clicked (GtkButton *button);

static void hildon_bread_crumb_widget_bread_crumb_init (HildonBreadCrumbIface *iface);

static void hildon_bread_crumb_widget_get_natural_size (HildonBreadCrumb *bread_crumb,
                                                        gint *width,
                                                        gint *height);

G_DEFINE_TYPE_WITH_CODE (HildonBreadCrumbWidget, hildon_bread_crumb_widget, GTK_TYPE_BUTTON,
                         G_IMPLEMENT_INTERFACE (HILDON_TYPE_BREAD_CRUMB,
                                                hildon_bread_crumb_widget_bread_crumb_init))

static void
hildon_bread_crumb_widget_class_init (HildonBreadCrumbWidgetClass *klass)
{
  GObjectClass *gobject_class = (GObjectClass*)klass;
  GtkButtonClass *button_class = (GtkButtonClass*)klass;

    /* GObject signals */
  gobject_class->constructor = hildon_bread_crumb_widget_constructor;
  gobject_class->finalize = hildon_bread_crumb_widget_finalize;
  gobject_class->set_property = hildon_bread_crumb_widget_set_property;
  gobject_class->get_property = hildon_bread_crumb_widget_get_property;

  /* GtkButton signals */
  button_class->clicked = hildon_bread_crumb_widget_clicked;

  /* Properties */
  g_object_class_install_property (gobject_class,
                                   PROP_TEXT,
                                   g_param_spec_string ("text",
                                                        "Text",
                                                        "Text of the label widget inside the bread crumb",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (gobject_class,
                                   PROP_ICON,
                                   g_param_spec_object ("icon",
                                                        "Icon",
                                                        "Image that will appear next to the bread crumb text",
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (gobject_class,
                                   PROP_ICON_POSITION,
                                   g_param_spec_enum ("icon-position",
                                                      "Icon position",
                                                      "The position of the image relative to the text",
                                                      GTK_TYPE_POSITION_TYPE,
                                                      GTK_POS_LEFT,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (gobject_class,
                                   PROP_SHOW_SEPARATOR,
                                   g_param_spec_boolean ("show-separator",
                                                         "Show separator",
                                                         "Show the separator attached to the item",
                                                         TRUE,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  /* Private data */
  g_type_class_add_private (gobject_class, sizeof (HildonBreadCrumbWidgetPrivate));
}

static void
hildon_bread_crumb_widget_bread_crumb_init (HildonBreadCrumbIface *iface)
{
  iface->get_natural_size = hildon_bread_crumb_widget_get_natural_size;
}

static GObject*
hildon_bread_crumb_widget_constructor (GType type,
                                       guint n_construct_properties,
                                       GObjectConstructParam *construct_params)
{
  GObject *object;
  HildonBreadCrumbWidget *bread_crumb;
  HildonBreadCrumbWidgetPrivate *priv;

  object = (* G_OBJECT_CLASS (hildon_bread_crumb_widget_parent_class)->constructor) (type,
                                                                              n_construct_properties,
                                                                              construct_params);

  bread_crumb = HILDON_BREAD_CRUMB_WIDGET (object);
  priv = bread_crumb->priv;
  priv->constructed = TRUE;

  priv->hbox = gtk_hbox_new (FALSE, 6);
  gtk_container_add (GTK_CONTAINER (bread_crumb), priv->hbox);

  /* Separator */
  priv->arrow = gtk_arrow_new (GTK_ARROW_RIGHT, GTK_SHADOW_NONE);
  gtk_widget_set_name (priv->arrow, "hildon-bread-crumb-separator-arrow");
  gtk_box_pack_start (GTK_BOX (priv->hbox), priv->arrow, FALSE, FALSE, 0);

  if (priv->show_separator)
    gtk_widget_show (priv->arrow);

  gtk_widget_set_no_show_all (priv->arrow, TRUE);

  /* Contents base container */
  bread_crumb->contents = gtk_hbox_new (FALSE, HILDON_MARGIN_DEFAULT);
  gtk_container_add (GTK_CONTAINER (priv->hbox), bread_crumb->contents);
  gtk_widget_show (bread_crumb->contents);

  if (priv->text || priv->icon)
    hildon_bread_crumb_widget_set_contents (bread_crumb);

  /* Show everything */
  gtk_widget_show (priv->hbox);
  
  return object;
}

static void
hildon_bread_crumb_widget_init (HildonBreadCrumbWidget *item)
{
  HildonBreadCrumbWidgetPrivate *priv = HILDON_BREAD_CRUMB_WIDGET_GET_PRIVATE (item);

  item->priv = priv;

  item->contents = NULL;

  priv->constructed = FALSE;
  priv->text = NULL;
  priv->icon = NULL;
  priv->icon_position = GTK_POS_LEFT;
  priv->show_separator = TRUE;
}

static void
hildon_bread_crumb_widget_finalize (GObject *object)
{
  HildonBreadCrumbWidgetPrivate *priv = HILDON_BREAD_CRUMB_WIDGET (object)->priv;

  g_free (priv->text);

  G_OBJECT_CLASS (hildon_bread_crumb_widget_parent_class)->finalize (object);
}

static void
hildon_bread_crumb_widget_clicked (GtkButton *button)
{
  hildon_bread_crumb_activated (HILDON_BREAD_CRUMB (button));
}

static void
hildon_bread_crumb_widget_set_contents (HildonBreadCrumbWidget *bread_crumb)
{
  GtkWidget *icon = NULL;
  HildonBreadCrumbWidgetPrivate *priv = bread_crumb->priv;

  if (!priv->constructed)
    return;

  if (!priv->text && !priv->icon)
    return;

  /* If the icon exists, keep it */
  if (priv->icon)
    {
      icon = g_object_ref (priv->icon);
      if (icon->parent)
        gtk_container_remove (GTK_CONTAINER (icon->parent), icon);
      priv->icon = NULL;
    }

  /* Reset contents */
  if (bread_crumb->contents)
    gtk_container_remove (GTK_CONTAINER (priv->hbox),
                          bread_crumb->contents);

  if (icon)
    {
      priv->icon = icon;
      if (priv->icon_position == GTK_POS_LEFT ||
          priv->icon_position == GTK_POS_RIGHT)
          bread_crumb->contents = gtk_hbox_new (FALSE, HILDON_MARGIN_DEFAULT);
      else
          bread_crumb->contents = gtk_vbox_new (FALSE, HILDON_MARGIN_DEFAULT);

      if (priv->icon_position == GTK_POS_LEFT ||
          priv->icon_position == GTK_POS_TOP)
        gtk_box_pack_start (GTK_BOX (bread_crumb->contents), priv->icon,
                            FALSE, FALSE, 0);
      else
        gtk_box_pack_end (GTK_BOX (bread_crumb->contents), priv->icon,
                          FALSE, FALSE, 0);
        
      if (priv->text)
        {
          priv->label = gtk_label_new (priv->text);
          g_object_set (G_OBJECT (priv->label), "xalign", 0.0, NULL);
          gtk_label_set_ellipsize (GTK_LABEL (priv->label),
                                   PANGO_ELLIPSIZE_END);

          if (priv->icon_position == GTK_POS_RIGHT ||
              priv->icon_position == GTK_POS_BOTTOM)
            gtk_box_pack_start (GTK_BOX (bread_crumb->contents), priv->label,
                                TRUE, TRUE, 0);
          else
            gtk_box_pack_end (GTK_BOX (bread_crumb->contents), priv->label,
                              TRUE, TRUE, 0);
          
        }

      gtk_box_pack_start (GTK_BOX (priv->hbox), bread_crumb->contents,
                          TRUE, TRUE, 0);
      gtk_widget_show_all (bread_crumb->contents);

      g_object_unref (icon);
    }
  else
    {
      /* Only text */
      bread_crumb->contents = gtk_hbox_new (FALSE, 0);
      gtk_box_pack_start (GTK_BOX (priv->hbox), bread_crumb->contents,
                          TRUE, TRUE, 0);

      priv->label = gtk_label_new (priv->text);
      g_object_set (G_OBJECT (priv->label), "xalign", 0.0, NULL);
      gtk_label_set_ellipsize (GTK_LABEL (priv->label),
                               PANGO_ELLIPSIZE_END);
      gtk_box_pack_start (GTK_BOX (bread_crumb->contents), priv->label, TRUE, TRUE, 0);

      gtk_widget_show_all (bread_crumb->contents);
    }
}

static void
hildon_bread_crumb_widget_set_property (GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec)
{
  HildonBreadCrumbWidget *item = HILDON_BREAD_CRUMB_WIDGET (object);

  switch (prop_id)
    {
    case PROP_TEXT:
      _hildon_bread_crumb_widget_set_text (item, g_value_get_string (value));
      break;
    case PROP_ICON:
      _hildon_bread_crumb_widget_set_icon (item, (GtkWidget*)g_value_get_object (value));
      break;
    case PROP_ICON_POSITION:
      _hildon_bread_crumb_widget_set_icon_position (item, g_value_get_enum (value));
      break;
    case PROP_SHOW_SEPARATOR:
      _hildon_bread_crumb_widget_set_show_separator (item, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hildon_bread_crumb_widget_get_property (GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec)
{
  HildonBreadCrumbWidget *item = HILDON_BREAD_CRUMB_WIDGET (object);

  switch (prop_id)
    {
    case PROP_TEXT:
      g_value_set_string (value, item->priv->text);
      break;
    case PROP_ICON:
      g_value_set_object (value, (GObject *)item->priv->icon);
      break;
    case PROP_ICON_POSITION:
      g_value_set_enum (value, item->priv->icon_position);
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
_hildon_bread_crumb_widget_set_text (HildonBreadCrumbWidget *item,
                                     const gchar *text)
{
  HildonBreadCrumbWidgetPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB_WIDGET (item));

  priv = item->priv;

  if (priv->text)
    g_free (priv->text);

  priv->text = g_strdup (text);

  hildon_bread_crumb_widget_set_contents (item);

  g_object_notify (G_OBJECT (item), "text");
}

const gchar*
_hildon_bread_crumb_widget_get_text (HildonBreadCrumbWidget *item)
{
  HildonBreadCrumbWidgetPrivate *priv;

  g_return_val_if_fail (HILDON_IS_BREAD_CRUMB_WIDGET (item), NULL);

  priv = item->priv;

  return priv->text;
}

void
_hildon_bread_crumb_widget_set_show_separator (HildonBreadCrumbWidget *item,
                                               gboolean show_separator)
{
  HildonBreadCrumbWidgetPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB_WIDGET (item));

  priv = item->priv;

  if (priv->show_separator == show_separator)
    return;
  
  priv->show_separator = show_separator;

  if (!priv->constructed)
    return;

  if (show_separator)
    gtk_widget_show (priv->arrow);
  else
    gtk_widget_hide (priv->arrow);

  g_object_notify (G_OBJECT (item), "show-separator");
}

static void
hildon_bread_crumb_widget_get_natural_size (HildonBreadCrumb *bread_crumb,
                                            gint *natural_width,
                                            gint *natural_height)
{
  GtkRequisition widget_req, label_req;
  gint width, height;
  PangoLayout *layout;
  HildonBreadCrumbWidget *item;
  HildonBreadCrumbWidgetPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB_WIDGET (bread_crumb));

  item = HILDON_BREAD_CRUMB_WIDGET (bread_crumb);
  priv = item->priv;

  gtk_widget_size_request (GTK_WIDGET (item), &widget_req);

  layout = gtk_widget_create_pango_layout (priv->label, priv->text);
  pango_layout_get_pixel_size (layout, &width, &height);
  g_object_unref (layout);

  if (natural_width)
    {
      *natural_width = widget_req.width;
      /* Substract the size request of the label */
      gtk_widget_size_request (priv->label, &label_req);
      *natural_width -= label_req.width;

      /* Add the "natural" width for the label */
      *natural_width += width;
      *natural_width += GTK_CONTAINER (item)->border_width * 2;
    }

  if (natural_height)
    {
      *natural_height = widget_req.height;
      *natural_height += GTK_CONTAINER (item)->border_width * 2;
    }
}

GtkWidget*
_hildon_bread_crumb_widget_new ()
{
  return GTK_WIDGET (g_object_new (HILDON_TYPE_BREAD_CRUMB_WIDGET, NULL));
}

GtkWidget*
_hildon_bread_crumb_widget_new_with_text (const gchar *text)
{
  return GTK_WIDGET (g_object_new (HILDON_TYPE_BREAD_CRUMB_WIDGET,
                                   "text", text,
                                   NULL));
}

GtkWidget*
_hildon_bread_crumb_widget_new_with_icon (GtkWidget *icon, const gchar *text)
{
  return GTK_WIDGET (g_object_new (HILDON_TYPE_BREAD_CRUMB_WIDGET,
                                   "icon", icon,
                                   "text", text,
                                   NULL));
}

void
_hildon_bread_crumb_widget_set_icon (HildonBreadCrumbWidget *bread_crumb,
                                     GtkWidget *icon)
{
  HildonBreadCrumbWidgetPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB_WIDGET (bread_crumb));

  priv = bread_crumb->priv;

  priv->icon = icon;

  hildon_bread_crumb_widget_set_contents (bread_crumb);

  g_object_notify (G_OBJECT (bread_crumb), "icon");
}

void
_hildon_bread_crumb_widget_set_icon_position (HildonBreadCrumbWidget *bread_crumb,
                                              GtkPositionType icon_position)
{
  HildonBreadCrumbWidgetPrivate *priv;

  g_return_if_fail (HILDON_IS_BREAD_CRUMB_WIDGET (bread_crumb));

  priv = bread_crumb->priv;

  if (priv->icon_position == icon_position)
    return;

  priv->icon_position = icon_position;

  hildon_bread_crumb_widget_set_contents (bread_crumb);

  g_object_notify (G_OBJECT (bread_crumb), "icon-position");
}
