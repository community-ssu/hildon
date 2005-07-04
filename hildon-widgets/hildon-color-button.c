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

#include <config.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtkbutton.h>
#include <gtk/gtkalignment.h>
#include <gtk/gtkdrawingarea.h>
#include <gtk/gtksignal.h>

#include "hildon-color-button.h"
#include "hildon-color-selector.h"

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

#define HILDON_COLOR_BUTTON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE\
              ((obj), HILDON_TYPE_COLOR_BUTTON, HildonColorButtonPrivate))

#define COLOR_FIELD_HEIGHT 26
#define COLOR_FIELD_WIDTH 26
#define COLOR_BUTTON_WIDTH 48
#define COLOR_BUTTON_HEIGHT 40

struct _HildonColorButtonPrivate 
{
  GtkWidget *dialog;

  GtkWidget *drawing_area;
  GdkPixbuf *pixbuf;
  GdkColor color;
  GdkGC *gc;
};

enum 
{
  PROP_NONE,
  PROP_COLOR
};

static void
hildon_color_button_class_init(HildonColorButtonClass *klass);
static void
hildon_color_button_init(HildonColorButton *color_button);

static void
hildon_color_button_finalize(GObject *object);
static void
hildon_color_button_set_property(GObject *object, guint param_id,
                    					   const GValue *value, GParamSpec *pspec);
static void
hildon_color_button_get_property(GObject *object, guint param_id,
                       				   GValue *value, GParamSpec *pspec);
static void
hildon_color_button_realize(GtkWidget *widget);
static void
hildon_color_button_clicked(GtkButton *button);
static gint
hildon_color_field_expose_event(GtkWidget *widget, GdkEventExpose *event,
                                HildonColorButton *cb);

static gboolean
hildon_color_button_mnemonic_activate( GtkWidget *widget,
                                       gboolean group_cycling );
static void
hildon_color_button_recolor_pixbuf(HildonColorButton *cb);


static gpointer parent_class = NULL;

GType
hildon_color_button_get_type(void)
{
  static GType color_button_type = 0;
  
  if (!color_button_type)
    {
      static const GTypeInfo color_button_info =
      {
        sizeof (HildonColorButtonClass),
        NULL,           /* base_init */
        NULL,           /* base_finalize */
        (GClassInitFunc) hildon_color_button_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (HildonColorButton),
        0,              /* n_preallocs */
        (GInstanceInitFunc) hildon_color_button_init,
      };
      
      color_button_type =
        g_type_register_static (GTK_TYPE_BUTTON, "HildonColorButton",
                                &color_button_info, 0);
    }
  
  return color_button_type;
}

static void
hildon_color_button_class_init(HildonColorButtonClass *klass)
{
  GObjectClass *gobject_class;
  GtkButtonClass *button_class;
  GtkWidgetClass *widget_class;
  
  gobject_class = G_OBJECT_CLASS (klass);
  button_class = GTK_BUTTON_CLASS (klass);
  widget_class = GTK_WIDGET_CLASS (klass);
  
  parent_class = g_type_class_peek_parent (klass);

  gobject_class->get_property = hildon_color_button_get_property;
  gobject_class->set_property = hildon_color_button_set_property;
  gobject_class->finalize = hildon_color_button_finalize;
  widget_class->realize = hildon_color_button_realize;
  button_class->clicked = hildon_color_button_clicked;
  widget_class->mnemonic_activate = hildon_color_button_mnemonic_activate;
  
  /**
   * HildonColorButton:color:
   *
   * The selected color.
   */
  g_object_class_install_property (gobject_class, PROP_COLOR,
                                   g_param_spec_boxed ("color",
                                     "Current Color",
                                     "The selected color",
                                     GDK_TYPE_COLOR, G_PARAM_READABLE | 
                                     G_PARAM_WRITABLE));

  g_type_class_add_private (gobject_class, sizeof (HildonColorButtonPrivate));
}

/* Handle exposure events for the color picker's drawing area */
static gint
hildon_color_field_expose_event(GtkWidget *widget, GdkEventExpose *event,
                                HildonColorButton *cb)
{
  gdk_draw_pixbuf(widget->window, cb->priv->gc,
                  cb->priv->pixbuf, event->area.x,
                  event->area.y, event->area.x,
                  event->area.y, event->area.width,
                  event->area.height, GDK_RGB_DITHER_MAX,
                  event->area.x, event->area.y);
  return FALSE;
}

static void
hildon_color_button_init(HildonColorButton *cb)
{
  GtkWidget *align;
  
  cb->priv = HILDON_COLOR_BUTTON_GET_PRIVATE(cb);

  cb->priv->dialog = NULL;
  cb->priv->gc = NULL;

  gtk_widget_push_composite_child();
  
  align = gtk_alignment_new(0.5, 0.5, 0, 0);

  cb->priv->pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8,
                                    COLOR_FIELD_WIDTH, COLOR_FIELD_HEIGHT);

  cb->priv->drawing_area = gtk_drawing_area_new();

  gtk_widget_set_size_request(GTK_WIDGET(cb), COLOR_BUTTON_WIDTH,
                              COLOR_BUTTON_HEIGHT);
  gtk_widget_set_size_request(GTK_WIDGET(cb->priv->drawing_area),
                              COLOR_FIELD_WIDTH, COLOR_FIELD_HEIGHT);

  g_signal_connect(G_OBJECT(cb->priv->drawing_area), "expose-event",
                   G_CALLBACK(hildon_color_field_expose_event), cb);

  gtk_container_add(GTK_CONTAINER(align), cb->priv->drawing_area);
  gtk_container_add(GTK_CONTAINER(cb), align);
  
  hildon_color_button_recolor_pixbuf(cb);
  
  gtk_widget_show_all(align);
  
  gtk_widget_pop_composite_child();
}

static void
hildon_color_button_finalize(GObject *object)
{
  HildonColorButton *cb = HILDON_COLOR_BUTTON(object);

  if (cb->priv->dialog)
  {
    gtk_widget_destroy(cb->priv->dialog);
    cb->priv->dialog = NULL;
  }

  if (cb->priv->pixbuf)
  {
    g_object_unref(G_OBJECT(cb->priv->pixbuf));
    cb->priv->pixbuf = NULL;
  }

  if (cb->priv->gc)
  {
    g_object_unref(G_OBJECT(cb->priv->gc));
    cb->priv->gc = NULL;
  }
  if( G_OBJECT_CLASS(parent_class)->finalize )
    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void
hildon_color_button_realize(GtkWidget *widget)
{
  HildonColorButton *cb = HILDON_COLOR_BUTTON(widget);

  GTK_WIDGET_CLASS(parent_class)->realize(widget);

  if (!cb->priv->gc)
    cb->priv->gc = gdk_gc_new(widget->window);

  hildon_color_button_recolor_pixbuf(cb);
}

static gboolean
hildon_color_button_mnemonic_activate( GtkWidget *widget,
                                       gboolean group_cycling )
{
  gtk_widget_grab_focus( widget );
  return TRUE;
}

static void
hildon_color_button_recolor_pixbuf(HildonColorButton *cb)
{
  gint i, j, rowstride;
  guchar *pixels;
  guint8 r, g, b;
  GdkPixbuf *pb = cb->priv->pixbuf;
  GdkColor *color = &cb->priv->color;
  
  r = (color->red >> 8);
  g = (color->green >> 8);
  b = (color->blue >> 8);
  
  pixels = gdk_pixbuf_get_pixels(pb);
  rowstride = gdk_pixbuf_get_rowstride(pb);
  
  for (j=0; j<COLOR_FIELD_HEIGHT; j++)
    for (i=0; i<COLOR_FIELD_WIDTH; i++)
    {
      *(j * rowstride + pixels + i * 3)     = r;
      *(j * rowstride + pixels + i * 3 + 1) = g;
      *(j * rowstride + pixels + i * 3 + 2) = b;
    }
  gtk_widget_queue_draw(GTK_WIDGET(cb));
}

static void
hildon_color_button_clicked(GtkButton *button)
{
  gint result;
  HildonColorButton *cb = HILDON_COLOR_BUTTON(button);
  HildonColorSelector *csd = HILDON_COLOR_SELECTOR(cb->priv->dialog);

  if (!csd)
  {
    GtkWidget *parent = gtk_widget_get_toplevel(GTK_WIDGET(cb));
    cb->priv->dialog = hildon_color_selector_new(GTK_WINDOW(parent));
    csd = HILDON_COLOR_SELECTOR(cb->priv->dialog);
    
    if (parent)
      gtk_window_set_transient_for(GTK_WINDOW(csd), GTK_WINDOW(parent));
  }
  
  hildon_color_selector_set_color(csd, &cb->priv->color);
  
  result = gtk_dialog_run(GTK_DIALOG(csd));
  if (result == GTK_RESPONSE_OK)
  {
    cb->priv->color = *hildon_color_selector_get_color(csd);
    g_object_set( G_OBJECT(cb), "color", &cb->priv->color, NULL );
  }
  gtk_widget_hide(GTK_WIDGET(csd));
}

static void
hildon_color_button_set_property(GObject *object, guint param_id,
			                           const GValue *value, GParamSpec *pspec)
{
  HildonColorButton *cb = HILDON_COLOR_BUTTON(object);

  switch (param_id) 
    {
    case PROP_COLOR:
      cb->priv->color = *(GdkColor*)g_value_get_boxed(value);
      hildon_color_button_recolor_pixbuf(cb);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
    }
}

static void
hildon_color_button_get_property(GObject *object, guint param_id,
			                           GValue *value, GParamSpec *pspec)
{
  HildonColorButton *cb = HILDON_COLOR_BUTTON(object);

  switch (param_id) 
    {
    case PROP_COLOR:
      g_value_set_boxed(value, &cb->priv->color);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
    }
}

/**
 * hildon_color_button_new:
 *
 * Creates a new color button. This returns a widget in the form of
 * a small button containing a swatch representing the current selected 
 * color. When the button is clicked, a color-selection dialog will open, 
 * allowing the user to select a color. The swatch will be updated to reflect 
 * the new color when the user finishes.
 *
 * Return value: a new color button.
 */
GtkWidget *
hildon_color_button_new(void)
{
  return g_object_new( HILDON_TYPE_COLOR_BUTTON, NULL );
}

/**
 * hildon_color_button_new_with_color:
 * @color: A #GdkColor to set the current color with.
 *
 * Creates a new color button. 
 *
 * Return value: a new color button.
 */
GtkWidget *
hildon_color_button_new_with_color(const GdkColor *color)
{
  return g_object_new( HILDON_TYPE_COLOR_BUTTON, "color", color, NULL );
}

/**
 * hildon_color_button_set_color:
 * @button: A #HildonColorButton
 * @color: A color to be set
 *
 * Sets a color to the button.
 */
void
hildon_color_button_set_color( HildonColorButton *button, GdkColor *color )
{
  g_object_set( G_OBJECT(button), "color", color, NULL );
}

/**
 * hildon_color_button_get_color:
 * @button: A #HildonColorButton
 *
 * Gets a color from the button.
 *
 * Return value: The color set to a current button.
 */
GdkColor *
hildon_color_button_get_color( HildonColorButton *button )
{
  GdkColor *color = NULL;
  g_object_get( G_OBJECT(button), "color", &color, NULL );
  return color;
}
