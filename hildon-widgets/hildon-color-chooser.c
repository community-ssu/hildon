/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License or any later version.
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
 * SECTION:hildon-color-chooser
 * @short_description: A widget to choose a color.
 * @see_also: #HildonColorChooserDialog, #HildonColorButton
 *
 * HildonColorChooser is a widget to choose a color.
 */
#include <gtk/gtk.h>


#include "hildon-color-chooser.h"

#include "hildon-plugin-widget.h"


enum {
  COLOR_CHANGED,
  LAST_SIGNAL
};


static HildonPluginWidgetInfo *global_plugin = NULL;


static guint color_chooser_signals[LAST_SIGNAL] = { 0 };


static void hildon_color_chooser_init(HildonColorChooser *sel);
static void hildon_color_chooser_class_init(HildonColorChooserClass *klass);


GtkType hildon_color_chooser_get_type ()
{
  static GtkType chooser_type = 0;

  if (!chooser_type)
    {
      static const GtkTypeInfo chooser_info =
      {
	"HildonColorChooser",
	sizeof (HildonColorChooser),
	sizeof (HildonColorChooserClass),
	(GtkClassInitFunc) hildon_color_chooser_class_init,
	(GtkObjectInitFunc) hildon_color_chooser_init,
	/* reserved_1 */ NULL,
	/* reserved_1 */ NULL,
	(GtkClassInitFunc) NULL
      };

      chooser_type = gtk_type_unique (GTK_TYPE_WIDGET, &chooser_info);
    }

  return chooser_type;
}


static void hildon_color_chooser_init(HildonColorChooser *sel)
{
  sel->color.red   = 0x0000;
  sel->color.green = 0x0000;
  sel->color.blue  = 0x0000;
  sel->color.pixel = 0x00000000;
}

static void hildon_color_chooser_class_init(HildonColorChooserClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  GtkObjectClass *object_class = GTK_OBJECT_CLASS(klass);

  klass->set_color = NULL;


  gtk_widget_class_install_style_property(widget_class,
                                          g_param_spec_boxed("outer_border",
                                                             "Outer border",
                                                             "Size of outer border",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));

  color_chooser_signals[COLOR_CHANGED] = g_signal_new("color-changed", G_OBJECT_CLASS_TYPE (object_class),
                                                      G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET (HildonColorChooserClass, color_changed),
                                                      NULL, NULL, g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1, GDK_TYPE_COLOR);
}

/**
 * hildon_color_chooser_new:
 *
 * Creates a new color chooser widget. The dialog is created through
 * HildonPluginWidget API and is loaded from plugin. The initially selected
 * color can be anything, so it's recommended to call
 * hildon_color_chooser_dialog_set_color () after creating the widget.
 *
 * Returns: a new color chooser widget
 */
GtkWidget *hildon_color_chooser_new()
{
  if(!global_plugin) {
    global_plugin = hildon_plugin_info_initialize(HILDON_TYPE_COLOR_CHOOSER, NULL);
  }


  return hildon_plugin_info_construct_widget(global_plugin);
}

/**
 * hildon_color_chooser_set_color:
 * @chooser: a #HildonColorChooser
 * @color: a color to be set
 *
 * Sets the color selected in the widget.
 */
void hildon_color_chooser_set_color(HildonColorChooser *chooser, GdkColor *color)
{
  HildonColorChooserClass *klass = HILDON_COLOR_CHOOSER_CLASS(G_OBJECT_GET_CLASS(chooser));


  chooser->color = *color;

  if(klass->set_color) {
    klass->set_color(chooser, color);
  }
}

/**
 * hildon_color_chooser_get_color:
 * @chooser: a #HildonColorChooser
 * @color: a pointer to #GdkColor to be filled by the function
 *
 * Gets the color selected in the widget.
 */
void hildon_color_chooser_get_color(HildonColorChooser *chooser, GdkColor *color)
{
  *color = chooser->color;
}


void hildon_color_chooser_emit_color_changed(HildonColorChooser *chooser)
{
  g_signal_emit(chooser, color_chooser_signals[COLOR_CHANGED], 0, &chooser->color);
}
