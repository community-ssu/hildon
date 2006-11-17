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


#include <gtk/gtk.h>


#include "hildon-color-chooser-button.h"

#include "hildon-color-chooser-dialog.h"


enum {
  COLOR_CHANGED,
  LAST_SIGNAL
};


static guint color_chooser_button_signals[LAST_SIGNAL] = { 0 };


static GtkButtonClass *parent_klass = NULL;


static void hildon_color_chooser_button_init(HildonColorChooserButton *object);
static void hildon_color_chooser_button_class_init(HildonColorChooserButtonClass *klass);


static void hildon_color_chooser_button_size_request(GtkWidget *widget, GtkRequisition *req);
static void hildon_color_chooser_button_size_allocate(GtkWidget *widget, GtkAllocation *alloc);

static void hildon_color_chooser_button_realize(GtkWidget *widget);
static void hildon_color_chooser_button_unrealize(GtkWidget *widget);

static void hildon_color_chooser_button_style_set(GtkWidget *widget, GtkStyle *previous_style);

static void hildon_color_chooser_button_show(GtkWidget *widget);
static void hildon_color_chooser_button_show_all(GtkWidget *widget);

static void hildon_color_chooser_button_virtual_set_color(HildonColorChooserButton *button, GdkColor *color);
static void hildon_color_chooser_button_virtual_color_changed(HildonColorChooserButton *button, GdkColor *color);

static void hildon_color_chooser_button_clicked(GtkButton *button);


static gboolean hildon_color_chooser_button_area_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data);


static void hildon_color_chooser_button_helper_get_style_info(HildonColorChooserButton *button);


GtkType hildon_color_chooser_button_get_type (void)
{
  static GtkType button_type = 0;

  if (!button_type)
    {
      static const GtkTypeInfo button_info =
      {
	"HildonColorChooserButton",
	sizeof (HildonColorChooserButton),
	sizeof (HildonColorChooserButtonClass),
	(GtkClassInitFunc) hildon_color_chooser_button_class_init,
	(GtkObjectInitFunc) hildon_color_chooser_button_init,
	/* reserved_1 */ NULL,
	/* reserved_1 */ NULL,
	(GtkClassInitFunc) NULL
      };

      button_type = gtk_type_unique (GTK_TYPE_BUTTON, &button_info);
    }

  return button_type;
}


  /* initializer functions */
static void hildon_color_chooser_button_init(HildonColorChooserButton *object)
{
  object->color.red =   0x0000;
  object->color.green = 0x0000;
  object->color.blue =  0x0000;
  object->color.pixel = 0x00000000;


  object->area = gtk_drawing_area_new();

  gtk_container_add(GTK_CONTAINER(object), object->area);


  g_signal_connect(G_OBJECT(object->area), "expose-event",
                   G_CALLBACK(hildon_color_chooser_button_area_expose), object);
}

static void hildon_color_chooser_button_class_init(HildonColorChooserButtonClass *klass)
{
  GtkWidgetClass *widget_klass = GTK_WIDGET_CLASS(klass);
  GtkButtonClass *button_klass = GTK_BUTTON_CLASS(klass);
  GtkObjectClass *object_klass = GTK_OBJECT_CLASS(klass);


  parent_klass = g_type_class_peek_parent(klass);


  klass->set_color = hildon_color_chooser_button_virtual_set_color;
  klass->color_changed = hildon_color_chooser_button_virtual_color_changed;


  button_klass->clicked = hildon_color_chooser_button_clicked;


  widget_klass->size_request = hildon_color_chooser_button_size_request;
  widget_klass->size_allocate = hildon_color_chooser_button_size_allocate;

  widget_klass->realize = hildon_color_chooser_button_realize;
  widget_klass->unrealize = hildon_color_chooser_button_unrealize;

  widget_klass->style_set = hildon_color_chooser_button_style_set;

  widget_klass->show = hildon_color_chooser_button_show;
  widget_klass->show_all = hildon_color_chooser_button_show_all;


  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("outer_border",
                                                             "Outer border",
                                                             "Size of the outer border",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));
  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("inner_border",
                                                             "Inner border",
                                                             "Size of the inner border",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));
  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("minimum_size",
                                                             "minimum_size",
                                                             "Minimum size of the color area",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));


  color_chooser_button_signals[COLOR_CHANGED] = g_signal_new("color-changed", G_OBJECT_CLASS_TYPE (object_klass),
                                                             G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET (HildonColorChooserButtonClass, color_changed),
                                                             NULL, NULL, g_cclosure_marshal_VOID__BOXED, G_TYPE_NONE, 1, GDK_TYPE_COLOR);
}


  /* virtual widget functions */
static void hildon_color_chooser_button_size_request(GtkWidget *widget, GtkRequisition *req)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(widget);


  req->width = button->style_info.outer.left + button->style_info.min.left + button->style_info.outer.right;
  req->height = button->style_info.outer.top + button->style_info.min.right + button->style_info.outer.bottom;
}

static void hildon_color_chooser_button_size_allocate(GtkWidget *widget, GtkAllocation *alloc)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(widget);
  GtkAllocation child_alloc;
  GdkRectangle clip_rect;


  GTK_WIDGET_CLASS(parent_klass)->size_allocate(widget, alloc);
  child_alloc = *alloc;


  child_alloc.x += button->style_info.outer.left;
  child_alloc.y += button->style_info.outer.top;

  child_alloc.width  -= (button->style_info.outer.left + button->style_info.outer.right);
  child_alloc.height -= (button->style_info.outer.top + button->style_info.outer.bottom);


  gtk_widget_size_allocate(button->area, &child_alloc);


  if(GTK_WIDGET_REALIZED(widget)) {
    clip_rect.x = button->style_info.inner.left;
    clip_rect.y = button->style_info.inner.top;
    clip_rect.width = button->area->allocation.width - button->style_info.inner.left - button->style_info.inner.right;
    clip_rect.height = button->area->allocation.height - button->style_info.inner.top - button->style_info.inner.bottom;

    gdk_gc_set_clip_rectangle(button->color_gc, &clip_rect);
  }
}


static void hildon_color_chooser_button_realize(GtkWidget *widget)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(widget);
  GdkRectangle clip_rect;


  GTK_WIDGET_CLASS(parent_klass)->realize(widget);


  button->color_gc = gdk_gc_new(widget->window);
  gdk_gc_set_rgb_fg_color(button->color_gc, &button->color);


  clip_rect.x = button->style_info.inner.left;
  clip_rect.y = button->style_info.inner.top;
  clip_rect.width = button->area->allocation.width - button->style_info.inner.left - button->style_info.inner.right;
  clip_rect.height = button->area->allocation.height - button->style_info.inner.top - button->style_info.inner.bottom;

  gdk_gc_set_clip_rectangle(button->color_gc, &clip_rect);
}

static void hildon_color_chooser_button_unrealize(GtkWidget *widget)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(widget);


  g_object_unref(button->color_gc);
  button->color_gc = NULL;


  GTK_WIDGET_CLASS(parent_klass)->unrealize(widget);
}


static void hildon_color_chooser_button_style_set(GtkWidget *widget, GtkStyle *previous_style)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(widget);
  GdkRectangle clip_rect;


  hildon_color_chooser_button_helper_get_style_info(button);


  if(GTK_WIDGET_REALIZED(widget)) {
    clip_rect.x = button->style_info.inner.left;
    clip_rect.y = button->style_info.inner.top;
    clip_rect.width = button->area->allocation.width - button->style_info.inner.left - button->style_info.inner.right;
    clip_rect.height = button->area->allocation.height - button->style_info.inner.top - button->style_info.inner.bottom;

    gdk_gc_set_clip_rectangle(button->color_gc, &clip_rect);
  }
}


static void hildon_color_chooser_button_show(GtkWidget *widget)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(widget);


  gtk_widget_show(button->area);

  GTK_WIDGET_CLASS(parent_klass)->show(widget);
}

static void hildon_color_chooser_button_show_all(GtkWidget *widget)
{
  hildon_color_chooser_button_show(widget);
}


static void hildon_color_chooser_button_virtual_set_color(HildonColorChooserButton *button, GdkColor *color)
{
  button->color = *color;

  if(GTK_WIDGET_REALIZED(button)) {
    gdk_gc_set_rgb_fg_color(button->color_gc, &button->color);

    gtk_widget_queue_draw(button->area);
  }


  g_signal_emit(button, color_chooser_button_signals[COLOR_CHANGED], 0, &button->color);
}

static void hildon_color_chooser_button_virtual_color_changed(HildonColorChooserButton *button, GdkColor *color)
{
}


static void hildon_color_chooser_button_clicked(GtkButton *button)
{
  HildonColorChooserButton *color_button = HILDON_COLOR_CHOOSER_BUTTON(button);
  GtkWidget *dialog;
  GdkColor color;
  gint result = 0;


  dialog = hildon_color_chooser_dialog_new();
  gtk_widget_realize(dialog);
  hildon_color_chooser_dialog_set_color(HILDON_COLOR_CHOOSER_DIALOG(dialog), &color_button->color);
  gtk_widget_show(dialog);


  result = gtk_dialog_run(GTK_DIALOG(dialog));


  if(result == GTK_RESPONSE_OK) {
    hildon_color_chooser_dialog_get_color(HILDON_COLOR_CHOOSER_DIALOG(dialog), &color);
    hildon_color_chooser_button_virtual_set_color(color_button, &color);
  }


/*g_object_unref(G_OBJECT(dialog));*/
  gtk_widget_destroy(dialog);
}


  /* signal handlers */
static gboolean hildon_color_chooser_button_area_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  HildonColorChooserButton *button = HILDON_COLOR_CHOOSER_BUTTON(data);
  GtkWidget *button_widget = GTK_WIDGET(data);


  if(button->style_info.inner.left > 0 || button->style_info.inner.right > 0 ||
     button->style_info.inner.top > 0 || button->style_info.inner.bottom > 0) {
    gtk_paint_box(gtk_widget_get_style(button_widget), widget->window, GTK_WIDGET_STATE(button_widget), GTK_SHADOW_NONE,
                  &event->area, button_widget, "color-button", 0, 0, widget->allocation.width, widget->allocation.height);
  }


  gdk_draw_rectangle(widget->window, button->color_gc, TRUE, event->area.x, event->area.y, event->area.width, event->area.height);


  return FALSE;
}


  /* additional use-only-here functions */
static void hildon_color_chooser_button_helper_get_style_info(HildonColorChooserButton *button)
{
  GtkBorder *in, *out, *min;

  gtk_widget_style_get(GTK_WIDGET(button), "inner_border", &in,
                                           "outer_border", &out,
                                           "minimum_size", &min, NULL);


  if(in) {
    button->style_info.inner = *in;
    g_free(in);
  } else {
    button->style_info.inner.left = 0;
    button->style_info.inner.right = 0;
    button->style_info.inner.top = 0;
    button->style_info.inner.bottom = 0;
  }

  if(out) {
    button->style_info.outer = *out;
    g_free(out);
  } else {
    button->style_info.outer.left = 4;
    button->style_info.outer.right = 4;
    button->style_info.outer.top = 4;
    button->style_info.outer.bottom = 4;
  }

  if(min) {
    button->style_info.min = *min;
    g_free(min);
  } else {
    button->style_info.min.left = 8;
    button->style_info.min.right = 8;
    button->style_info.min.top = 0;
    button->style_info.min.bottom = 0;
  }
}


  /* public API */
GtkWidget *hildon_color_chooser_button_new()
{
  return gtk_type_new(HILDON_TYPE_COLOR_CHOOSER_BUTTON);
}


void hildon_color_chooser_button_set_color(HildonColorChooserButton *button, GdkColor *color)
{
  HILDON_COLOR_CHOOSER_BUTTON_CLASS(G_OBJECT_GET_CLASS(button))->set_color(button, color);
}

void hildon_color_chooser_button_get_color(HildonColorChooserButton *button, GdkColor *color)
{
  *color = button->color;
}
