/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
 *
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
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


#ifndef __HILDON_COLOR_CHOOSER_BUTTON_H__
#define __HILDON_COLOR_CHOOSER_BUTTON_H__


#include <gdk/gdkcolor.h>

#include <gtk/gtkwidget.h>


#define HILDON_TYPE_COLOR_CHOOSER_BUTTON             (hildon_color_chooser_button_get_type())

#define HILDON_COLOR_CHOOSER_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_COLOR_CHOOSER_BUTTON, HildonColorChooserButton))
#define HILDON_COLOR_CHOOSER_BUTTON_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HILDON_TYPE_COLOR_CHOOSER_BUTTON, HildonColorChooserButtonClass))
#define HILDON_IS_COLOR_CHOOSER_BUTTON(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_COLOR_CHOOSER_BUTTON))


typedef struct HildonColorChooserButton_            HildonColorChooserButton;
typedef struct HildonColorChooserButtonClass_       HildonColorChooserButtonClass;


struct HildonColorChooserButton_
{
  GtkButton parent;

  GdkColor color;

  GtkWidget *area;

  GdkGC *color_gc;


  struct {
    GtkBorder outer;
    GtkBorder inner;
    GtkBorder min;
  } style_info;
};

struct HildonColorChooserButtonClass_
{
  GtkButtonClass parent;

  void (*color_changed) (HildonColorChooserButton *button, GdkColor *color);

  void (*set_color) (HildonColorChooserButton *, GdkColor *);
};


GtkType hildon_color_chooser_button_get_type();
GtkWidget *hildon_color_chooser_button_new();

void hildon_color_chooser_button_set_color(HildonColorChooserButton *button, GdkColor *color);
void hildon_color_chooser_button_get_color(HildonColorChooserButton *button, GdkColor *color);


#endif /* __HILDON_COLOR_CHOOSER_BUTTON_H__ */
