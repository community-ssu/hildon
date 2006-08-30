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


#include <memory.h>
#include <string.h>

#include <libintl.h>

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include <gtk/gtk.h>

#include <gconf/gconf-client.h>


#include <hildon-widgets/hildon-color-chooser-dialog.h>
#include <hildon-widgets/hildon-color-chooser.h>

#include <hildon-widgets/hildon-plugin-widget.h>

#include <hildon-widgets/hildon-defines.h>

#include <hildon-widgets/hildon-banner.h>


#define _(String) dgettext("hildon-libs", String)


const char *parent_name = "HildonColorChooserDialog";
const char *plugin_name = "HSV color chooser dialog";

GType export_type(void);


static HildonPluginWidgetInfo *chooser_plugin = NULL;

static HildonColorChooserDialogClass *parent_klass = NULL;


/* darkened EGA palette to be used as predefined colors if style doesn't
   define anything else (darker colors are darkened 0x8000 -> 0x6666) */
static GdkColor hardcoded_colors[16] = {{0, 0x0000, 0x0000, 0x0000},
                                        {0, 0x6666, 0x6666, 0x6666},
                                        {0, 0x6666, 0x0000, 0x0000},
                                        {0, 0x0000, 0x6666, 0x0000},
                                        {0, 0x0000, 0x0000, 0x6666},
                                        {0, 0x6666, 0x6666, 0x0000},
                                        {0, 0x6666, 0x0000, 0x6666},
                                        {0, 0x0000, 0x6666, 0x6666},
                                        {0, 0xffff, 0xffff, 0xffff},
                                        {0, 0xc000, 0xc000, 0xc000},
                                        {0, 0xffff, 0x0000, 0x0000},
                                        {0, 0x0000, 0xffff, 0x0000},
                                        {0, 0x0000, 0x0000, 0xffff},
                                        {0, 0xffff, 0xffff, 0x0000},
                                        {0, 0xffff, 0x0000, 0xffff},
                                        {0, 0x0000, 0xffff, 0xffff}};


typedef struct {
  GtkBorder radio_sizes;
  GtkBorder cont_sizes;
  GtkBorder num_buttons;
  GtkBorder last_num_buttons;

  GdkColor default_color;
} HildonColorChooserStyleInfo;


typedef struct _HildonColorChooserDialogHSV HildonColorChooserDialogHSV;
typedef struct _HildonColorChooserDialogHSVClass HildonColorChooserDialogHSVClass;


struct _HildonColorChooserDialogHSV {
  HildonColorChooserDialog parent;

  GtkWidget *hbox;
  GtkWidget *vbox;

  GtkWidget *align_custom, *align_defined;
  GtkWidget *area_custom, *area_defined;
  GtkWidget *separator;

  GtkWidget *chooser;

  GdkColor *colors_custom, *colors_defined;
  GdkGC **gc_array;

  gint selected;

  HildonColorChooserStyleInfo style_info;


  gint has_style;

  GdkColor pending_color;


  struct {
    GConfClient *client;
  } gconf_data;
};

struct _HildonColorChooserDialogHSVClass {
  HildonColorChooserDialogClass parent_klass;
};


#define HILDON_COLOR_CHOOSER_DIALOG_HSV(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), export_type(), HildonColorChooserDialogHSV))
#define HILDON_COLOR_CHOOSER_DIALOG_HSV_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), export_type(), HildonColorChooserDialogHSVClass))


GtkType export_type(void);


static void hildon_color_chooser_dialog_hsv_init(HildonColorChooserDialogHSV *object);
static void hildon_color_chooser_dialog_hsv_class_init(HildonColorChooserDialogHSVClass *klass);

static void hildon_color_chooser_dialog_hsv_size_request(GtkWidget *widget, GtkRequisition *req);
static void hildon_color_chooser_dialog_hsv_size_allocate(GtkWidget *widget, GtkAllocation *alloc);

static void hildon_color_chooser_dialog_hsv_realize(GtkWidget *widget);
static void hildon_color_chooser_dialog_hsv_unrealize(GtkWidget *widget);

static void hildon_color_chooser_dialog_hsv_style_set(GtkWidget *widget, GtkStyle *previous_style);

static void hildon_color_chooser_dialog_hsv_show(GtkWidget *widget);
static void hildon_color_chooser_dialog_hsv_show_all(GtkWidget *widget);

static gboolean hildon_color_chooser_dialog_hsv_key_press_event(GtkWidget *widget, GdkEventKey *event);
static gboolean hildon_color_chooser_dialog_hsv_key_release_event(GtkWidget *widget, GdkEventKey *event);


static void hildon_color_chooser_dialog_hsv_destroy(GtkObject *object);


static void hildon_color_chooser_dialog_hsv_set_color(HildonColorChooserDialog *dialog, GdkColor *color);


static gboolean hildon_color_chooser_dialog_hsv_area_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data);

static gboolean hildon_color_chooser_dialog_hsv_area_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data);


static void hildon_color_chooser_dialog_hsv_chooser_color_changed(HildonColorChooser *chooser, GdkColor *color, gpointer data);

static void hildon_color_chooser_dialog_hsv_chooser_insensitive_press(GtkWidget *widget, gpointer data);


static void hildon_color_chooser_dialog_hsv_refresh_style_info(HildonColorChooserDialogHSV *dialog);

static void hildon_color_chooser_dialog_hsv_set_color_num(HildonColorChooserDialogHSV *dialog, gint num);

static void hildon_color_chooser_dialog_hsv_ascii_hex_to_color(gchar *s, GdkColor *color);
static void hildon_color_chooser_dialog_hsv_color_to_ascii_hex(gchar *s, GdkColor *color);


GType export_type()
{
  static GType dialog_type = 0;

  if (!dialog_type) {
    static const GTypeInfo dialog_info =
    {
      sizeof (HildonColorChooserDialogHSVClass),
      NULL,
      NULL,
      (GClassInitFunc) hildon_color_chooser_dialog_hsv_class_init,
      NULL,
      NULL,
      sizeof (HildonColorChooserDialogHSV),
      0,
      (GInstanceInitFunc) hildon_color_chooser_dialog_hsv_init,
      NULL
    };

    dialog_type = g_type_register_static (HILDON_TYPE_COLOR_CHOOSER_DIALOG, "HildonColorChooserDialogHSV", &dialog_info, 0);
  }

  return dialog_type;
}


static void hildon_color_chooser_dialog_hsv_init(HildonColorChooserDialogHSV *object)
{
  if(!chooser_plugin) {
    chooser_plugin = hildon_plugin_info_initialize(HILDON_TYPE_COLOR_CHOOSER, "hsv");
  }


  gtk_dialog_set_has_separator(GTK_DIALOG(object), FALSE);

  gtk_window_set_title(GTK_WINDOW(object), _("ecdg_ti_colour_selector"));


  object->chooser = hildon_plugin_info_construct_widget(chooser_plugin);

  object->hbox = gtk_hbox_new(FALSE, 0);
  object->vbox = gtk_vbox_new(FALSE, 0);

  gtk_box_pack_start(GTK_BOX(object->hbox), object->chooser, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(object->hbox), object->vbox, FALSE, FALSE, 0);


  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(object)->vbox), object->hbox, TRUE, TRUE, 0);


  object->align_custom = gtk_alignment_new(0.5, 1.0, 0.0, 0.0);
  object->align_defined = gtk_alignment_new(0.5, 1.0, 0.0, 0.0);

  object->area_custom = gtk_drawing_area_new();
  object->area_defined = gtk_drawing_area_new();


  gtk_container_add(GTK_CONTAINER(object->align_custom), object->area_custom);
  gtk_container_add(GTK_CONTAINER(object->align_defined), object->area_defined);


  object->separator = gtk_hseparator_new();


  gtk_box_pack_start(GTK_BOX(object->vbox), object->align_defined, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(object->vbox), object->separator,     FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(object->vbox), object->align_custom,  FALSE, FALSE, 0);


  gtk_dialog_add_button(GTK_DIALOG(object), _("ecdg_bd_colour_selector_ok"), GTK_RESPONSE_OK);
  gtk_dialog_add_button(GTK_DIALOG(object), _("ecdg_bd_colour_selector_cancel"), GTK_RESPONSE_CANCEL);


  g_signal_connect(G_OBJECT(object->chooser), "insensitive-press", G_CALLBACK(hildon_color_chooser_dialog_hsv_chooser_insensitive_press), object);


  g_signal_connect(G_OBJECT(object->area_custom), "expose-event", G_CALLBACK(hildon_color_chooser_dialog_hsv_area_expose), object);
  g_signal_connect(G_OBJECT(object->area_defined), "expose-event", G_CALLBACK(hildon_color_chooser_dialog_hsv_area_expose), object);

  g_signal_connect(G_OBJECT(object->area_custom), "button-press-event", G_CALLBACK(hildon_color_chooser_dialog_hsv_area_button_press), object);
  g_signal_connect(G_OBJECT(object->area_defined), "button-press-event", G_CALLBACK(hildon_color_chooser_dialog_hsv_area_button_press), object);

  gtk_widget_add_events(object->area_custom, GDK_BUTTON_PRESS_MASK);
  gtk_widget_add_events(object->area_defined, GDK_BUTTON_PRESS_MASK);


  object->selected = 0;


  g_signal_connect(G_OBJECT(object->chooser), "color-changed", G_CALLBACK(hildon_color_chooser_dialog_hsv_chooser_color_changed), object);


  object->gconf_data.client = gconf_client_get_default();


  memset(&object->style_info, 0, sizeof(HildonColorChooserStyleInfo));


  object->colors_custom = NULL;
  object->colors_defined = NULL;

  object->gc_array = NULL;


  object->has_style = 0;
}

static void hildon_color_chooser_dialog_hsv_class_init(HildonColorChooserDialogHSVClass *klass)
{
  GtkWidgetClass *widget_klass = GTK_WIDGET_CLASS(klass);
  GtkObjectClass *object_klass = GTK_OBJECT_CLASS(klass);
  HildonColorChooserDialogClass *dialog_klass = HILDON_COLOR_CHOOSER_DIALOG_CLASS(klass);
  gchar tmp[32];
  gint i;


  widget_klass->size_request = hildon_color_chooser_dialog_hsv_size_request;
  widget_klass->size_allocate = hildon_color_chooser_dialog_hsv_size_allocate;

  widget_klass->realize = hildon_color_chooser_dialog_hsv_realize;
  widget_klass->unrealize = hildon_color_chooser_dialog_hsv_unrealize;

  widget_klass->style_set = hildon_color_chooser_dialog_hsv_style_set;

  widget_klass->show = hildon_color_chooser_dialog_hsv_show;
  widget_klass->show_all = hildon_color_chooser_dialog_hsv_show_all;

  widget_klass->key_press_event = hildon_color_chooser_dialog_hsv_key_press_event;
  widget_klass->key_release_event = hildon_color_chooser_dialog_hsv_key_release_event;


  object_klass->destroy = hildon_color_chooser_dialog_hsv_destroy;


  dialog_klass->set_color = hildon_color_chooser_dialog_hsv_set_color;


  parent_klass = g_type_class_peek_parent(klass);


  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("container_sizes",
                                                             "Container sizes",
                                                             "Container specific sizes",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));

  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("radio_sizes",
                                                             "Color radio sizes",
                                                             "Color radio specific sizes",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));

  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("num_buttons",
                                                             "Number of buttons",
                                                             "Number of color store buttons",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));


  gtk_widget_class_install_style_property(widget_klass,
                                          g_param_spec_boxed("default_color", "Default color",
                                                             "Default color for nonpainted custom colors",
                                                             GDK_TYPE_COLOR,
                                                             G_PARAM_READABLE));


  for(i = 0; i < 32; i++) {
    memset(tmp, 0, 32);
    g_snprintf(tmp, 32, "defined_color%d", i);

    gtk_widget_class_install_style_property(widget_klass,
                                            g_param_spec_boxed(tmp, "Defined color",
                                                               "Pre-defined colors for the dialog",
                                                               GDK_TYPE_COLOR,
                                                               G_PARAM_READABLE));
  }
}


static void hildon_color_chooser_dialog_hsv_size_request(GtkWidget *widget, GtkRequisition *req)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);


  gtk_container_set_border_width(GTK_CONTAINER(dialog->hbox), dialog->style_info.cont_sizes.left);

  gtk_box_set_spacing(GTK_BOX(dialog->hbox), dialog->style_info.cont_sizes.right);
  gtk_box_set_spacing(GTK_BOX(dialog->vbox), dialog->style_info.cont_sizes.top);
  gtk_box_set_spacing(GTK_BOX(GTK_DIALOG(widget)->vbox), dialog->style_info.cont_sizes.bottom);


  gtk_widget_set_size_request(dialog->area_custom,
                              (dialog->style_info.radio_sizes.left + 2*dialog->style_info.radio_sizes.bottom)*(dialog->style_info.num_buttons.top) +
                              (dialog->style_info.num_buttons.top-1)*dialog->style_info.radio_sizes.top,
                              (dialog->style_info.radio_sizes.right + 2*dialog->style_info.radio_sizes.bottom)*(dialog->style_info.num_buttons.bottom) +
                              (dialog->style_info.num_buttons.bottom-1)*dialog->style_info.radio_sizes.top);
  gtk_widget_set_size_request(dialog->area_defined,
                              (dialog->style_info.radio_sizes.left + 2*dialog->style_info.radio_sizes.bottom)*(dialog->style_info.num_buttons.left) +
                              (dialog->style_info.num_buttons.left-1)*dialog->style_info.radio_sizes.top,
                              (dialog->style_info.radio_sizes.right + 2*dialog->style_info.radio_sizes.bottom)*(dialog->style_info.num_buttons.right) +
                              (dialog->style_info.num_buttons.right-1)*dialog->style_info.radio_sizes.top);


  GTK_WIDGET_CLASS(parent_klass)->size_request(widget, req);
}

static void hildon_color_chooser_dialog_hsv_size_allocate(GtkWidget *widget, GtkAllocation *alloc)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);
  GdkRectangle rect;
  int i, tmp, tmp2;


  GTK_WIDGET_CLASS(parent_klass)->size_allocate(widget, alloc);


  if(GTK_WIDGET_REALIZED(widget)) {
    tmp  = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);
    tmp2 = (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);

    for(i = 0; i < tmp; i++) {
      rect.x = ((i % dialog->style_info.num_buttons.left) * (dialog->style_info.radio_sizes.left + dialog->style_info.radio_sizes.top +
               2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
      rect.y = ((i / dialog->style_info.num_buttons.left) * (dialog->style_info.radio_sizes.right + dialog->style_info.radio_sizes.top +
               2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
      rect.width = dialog->style_info.radio_sizes.left;
      rect.height = dialog->style_info.radio_sizes.right;

      gdk_gc_set_clip_rectangle(dialog->gc_array[i], &rect);
    }

    for(i = 0; i < tmp2; i++) {
      rect.x = ((i % dialog->style_info.num_buttons.top) * (dialog->style_info.radio_sizes.left + dialog->style_info.radio_sizes.top +
               2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
      rect.y = ((i / dialog->style_info.num_buttons.top) * (dialog->style_info.radio_sizes.right + dialog->style_info.radio_sizes.top +
               2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
      rect.width = dialog->style_info.radio_sizes.left;
      rect.height = dialog->style_info.radio_sizes.right;

      gdk_gc_set_clip_rectangle(dialog->gc_array[i + tmp], &rect);
    }
  }
}


static void hildon_color_chooser_dialog_hsv_realize(GtkWidget *widget)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);
  GdkRectangle rect;
  int i, tmp, tmp2;


  GTK_WIDGET_CLASS(parent_klass)->realize(widget);


  tmp = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right) +
        (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);

  for(i = 0; i < tmp; i++) {
    dialog->gc_array[i] = gdk_gc_new(widget->window);
  }



  tmp  = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);
  tmp2 = (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);

  for(i = 0; i < tmp; i++) {
    gdk_gc_set_rgb_fg_color(dialog->gc_array[i], &dialog->colors_defined[i]);

    rect.x = ((i % dialog->style_info.num_buttons.left) * (dialog->style_info.radio_sizes.left + dialog->style_info.radio_sizes.top +
             2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
    rect.y = ((i / dialog->style_info.num_buttons.left) * (dialog->style_info.radio_sizes.right + dialog->style_info.radio_sizes.top +
             2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
    rect.width = dialog->style_info.radio_sizes.left;
    rect.height = dialog->style_info.radio_sizes.right;

    gdk_gc_set_clip_rectangle(dialog->gc_array[i], &rect);
  }

  for(i = 0; i < tmp2; i++) {
    gdk_gc_set_rgb_fg_color(dialog->gc_array[i + tmp], &dialog->colors_custom[i]);

    rect.x = ((i % dialog->style_info.num_buttons.top) * (dialog->style_info.radio_sizes.left + dialog->style_info.radio_sizes.top +
             2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
    rect.y = ((i / dialog->style_info.num_buttons.top) * (dialog->style_info.radio_sizes.right + dialog->style_info.radio_sizes.top +
             2*dialog->style_info.radio_sizes.bottom)) + dialog->style_info.radio_sizes.bottom;
    rect.width = dialog->style_info.radio_sizes.left;
    rect.height = dialog->style_info.radio_sizes.right;

    gdk_gc_set_clip_rectangle(dialog->gc_array[i + tmp], &rect);
  }
}

static void hildon_color_chooser_dialog_hsv_unrealize(GtkWidget *widget)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);
  int i, tmp;


  tmp = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right) +
        (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);

  for(i = 0; i < tmp; i++) {
    g_object_unref(dialog->gc_array[i]);
  }


  GTK_WIDGET_CLASS(parent_klass)->unrealize(widget);
}


static void hildon_color_chooser_dialog_hsv_style_set(GtkWidget *widget, GtkStyle *previous_style)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);
  GdkColor *tmpcolor;
  gchar tmp[32], key[128], *val;
  int i, tmpn, setcolor = 0;


  if(!dialog->has_style)
    setcolor = 1;

  dialog->has_style = 1;


  gtk_widget_style_get(widget, "default_color", &tmpcolor, NULL);
  if(tmpcolor) {
    dialog->style_info.default_color = *tmpcolor;
  } else {
    dialog->style_info.default_color.red   = 0x0000;
    dialog->style_info.default_color.green = 0x0000;
    dialog->style_info.default_color.blue  = 0x0000;
    dialog->style_info.default_color.pixel = 0x00000000;
  }


  hildon_color_chooser_dialog_hsv_refresh_style_info(dialog);


  if(memcmp(&dialog->style_info.num_buttons, &dialog->style_info.last_num_buttons, sizeof(GtkBorder))) {
    if(dialog->colors_custom) {
      g_free(dialog->colors_custom);
    } if(dialog->colors_defined) {
      g_free(dialog->colors_defined);
    } if(dialog->gc_array) {
      if(GTK_WIDGET_REALIZED(widget)) {
        tmpn = (dialog->style_info.last_num_buttons.left * dialog->style_info.last_num_buttons.right) +
               (dialog->style_info.last_num_buttons.top * dialog->style_info.last_num_buttons.bottom);

        for(i = 0; i < tmpn; i++) {
          g_object_unref(dialog->gc_array[i]);
        }
      }

      g_free(dialog->gc_array);
    }

    dialog->colors_custom  = (GdkColor *)g_malloc0(sizeof(GdkColor) * (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom));
    dialog->colors_defined = (GdkColor *)g_malloc0(sizeof(GdkColor) * (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right));


    tmpn = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right) +
           (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);
    dialog->gc_array = (GdkGC **)g_malloc0(sizeof(GdkGC *) * tmpn);


    if(dialog->gconf_data.client) {
      for(i = 0; i < (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom); i++) {
        memset(key, 0, 128);
        g_snprintf(key, 128, "/system/osso/af/color_chooser/custom_color%d", i);
        val = gconf_client_get_string(dialog->gconf_data.client, key, NULL);
        if(val) {
          hildon_color_chooser_dialog_hsv_ascii_hex_to_color(val, &dialog->colors_custom[i]);
          g_free(val);
        } else {
          dialog->colors_custom[i] = dialog->style_info.default_color;
        }
      }
    } else {
      for(i = 0; i < (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom); i++) {
        dialog->colors_custom[i] = dialog->style_info.default_color;
      }
    }
  }


  tmpn = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);

  hildon_color_chooser_set_color(HILDON_COLOR_CHOOSER(dialog->chooser),
                                 (dialog->selected < tmpn) ? &dialog->colors_defined[dialog->selected] : &dialog->colors_custom[dialog->selected - tmpn]);


  for(i = 0; i < (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right); i++) {
    memset(tmp, 0, 32);
    g_snprintf(tmp, 32, "defined_color%d", i);

    gtk_widget_style_get(widget, tmp, &tmpcolor, NULL);

    if(tmpcolor) {
      dialog->colors_defined[i] = *tmpcolor;
    } else {
      if(i < 16) {
        dialog->colors_defined[i] = hardcoded_colors[i];
      } else { /* fallback to prevent segfault */
        dialog->colors_defined[i].red = 0x0000;
        dialog->colors_defined[i].green = 0x0000;
        dialog->colors_defined[i].blue = 0x0000;
        dialog->colors_defined[i].pixel = 0x00000000;
      }
    }
  }


  if(GTK_WIDGET_REALIZED(widget)) {
    for(i = 0; i < (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right); i++) {
      gdk_gc_set_rgb_fg_color(dialog->gc_array[i], &dialog->colors_defined[i]);
    }
  }


  if(setcolor)
    hildon_color_chooser_dialog_hsv_set_color(HILDON_COLOR_CHOOSER_DIALOG(dialog), &dialog->pending_color);


  gtk_widget_queue_resize(widget);


  GTK_WIDGET_CLASS(parent_klass)->style_set(widget, previous_style);
}


static void hildon_color_chooser_dialog_hsv_show(GtkWidget *widget)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);


  gtk_widget_show(dialog->hbox);
  gtk_widget_show(dialog->vbox);

  gtk_widget_show(dialog->chooser);

  gtk_widget_show(dialog->align_custom);
  gtk_widget_show(dialog->align_defined);

  gtk_widget_show(dialog->separator);

  gtk_widget_show(dialog->area_custom);
  gtk_widget_show(dialog->area_defined);


  GTK_WIDGET_CLASS(parent_klass)->show(widget);
}

static void hildon_color_chooser_dialog_hsv_show_all(GtkWidget *widget)
{
  hildon_color_chooser_dialog_hsv_show(widget);
}


static gboolean hildon_color_chooser_dialog_hsv_key_press_event(GtkWidget *widget, GdkEventKey *event)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(widget);
  int tmp, tot, sel;


  if(event->keyval == HILDON_HARDKEY_UP || event->keyval == HILDON_HARDKEY_DOWN ||
     event->keyval == HILDON_HARDKEY_LEFT || event->keyval == HILDON_HARDKEY_RIGHT) {
    tmp = (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);
    tot = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right) + tmp;

    switch(event->keyval) {
    case HILDON_HARDKEY_UP:
      if(dialog->selected >= dialog->style_info.num_buttons.top) {
        if(dialog->selected - dialog->style_info.num_buttons.left >= tmp) {
          sel = dialog->selected - dialog->style_info.num_buttons.left;
        } else {
          sel = dialog->selected - dialog->style_info.num_buttons.top;
        }

        hildon_color_chooser_dialog_hsv_set_color_num(dialog, sel);
      }
      break;
    case HILDON_HARDKEY_DOWN:
      if(dialog->selected < tot - dialog->style_info.num_buttons.left) {
        if(dialog->selected < tmp) {
          sel = dialog->selected + dialog->style_info.num_buttons.top;
        } else {
          sel = dialog->selected + dialog->style_info.num_buttons.left;
        }

        hildon_color_chooser_dialog_hsv_set_color_num(dialog, sel);
      }
      break;
    case HILDON_HARDKEY_LEFT:
      if((dialog->selected < tmp ? (dialog->selected % dialog->style_info.num_buttons.top) : ((dialog->selected - tmp) % dialog->style_info.num_buttons.left)) > 0) {
        sel = dialog->selected - 1;

        hildon_color_chooser_dialog_hsv_set_color_num(dialog, sel);
      }
      break;
    case HILDON_HARDKEY_RIGHT:
      if((dialog->selected < tmp) ? (dialog->selected % dialog->style_info.num_buttons.top < dialog->style_info.num_buttons.top - 1) :
         ((dialog->selected - tmp) % dialog->style_info.num_buttons.left < dialog->style_info.num_buttons.left - 1)) {
        sel = dialog->selected + 1;

        hildon_color_chooser_dialog_hsv_set_color_num(dialog, sel);
      }
      break;
    default:
      break;
    }

    return FALSE;
  }

  return GTK_WIDGET_CLASS(parent_klass)->key_press_event(widget, event);
}

static gboolean hildon_color_chooser_dialog_hsv_key_release_event(GtkWidget *widget, GdkEventKey *event)
{
  if(event->keyval == HILDON_HARDKEY_UP || event->keyval == HILDON_HARDKEY_DOWN ||
     event->keyval == HILDON_HARDKEY_LEFT || event->keyval == HILDON_HARDKEY_RIGHT) {
    return FALSE;
  }

  return GTK_WIDGET_CLASS(parent_klass)->key_press_event(widget, event);
}


static void hildon_color_chooser_dialog_hsv_destroy(GtkObject *object)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(object);
  gchar key[128], color[13];
  int i, tmp;


  if(dialog->gconf_data.client) {
    memset(color, 0, 13);

    tmp = (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);

    for(i = 0; i < tmp; i++) {
      memset(key, 0, 128);
      g_snprintf(key, 128, "/system/osso/af/color_chooser/custom_color%d", i);
      hildon_color_chooser_dialog_hsv_color_to_ascii_hex(color, &dialog->colors_custom[i]);
      gconf_client_set_string(dialog->gconf_data.client, key, color, NULL);
    }

    g_object_unref(dialog->gconf_data.client);
    dialog->gconf_data.client = NULL;
  }


  if(dialog->gc_array) {
    g_free(dialog->gc_array);
    dialog->gc_array = NULL;
  } if(dialog->colors_defined) {
    g_free(dialog->colors_defined);
    dialog->colors_defined = NULL;
  } if(dialog->colors_custom) {
    g_free(dialog->colors_custom);
    dialog->colors_custom = NULL;
  }


  GTK_OBJECT_CLASS(parent_klass)->destroy(object);
}


static void hildon_color_chooser_dialog_hsv_set_color(HildonColorChooserDialog *dialog, GdkColor *color)
{
  HildonColorChooserDialogHSV *dialog_hsv = HILDON_COLOR_CHOOSER_DIALOG_HSV(dialog);
  int i, found = -1, tmp, tmp2;


  if(!dialog_hsv->has_style) {
    dialog_hsv->pending_color = *color;
    return;
  }


  tmp  = (dialog_hsv->style_info.num_buttons.left * dialog_hsv->style_info.num_buttons.right);
  tmp2 = (dialog_hsv->style_info.num_buttons.top * dialog_hsv->style_info.num_buttons.bottom);

  for(i = 0; i < tmp; i++) {
    if(dialog_hsv->colors_defined[i].red == color->red &&
       dialog_hsv->colors_defined[i].green == color->green &&
       dialog_hsv->colors_defined[i].blue == color->blue) {
      found = i;
      break;
    }
  }

  if(found == -1) {
    for(i = 0; i < tmp2; i++) {
      if(dialog_hsv->colors_custom[i].red == color->red &&
         dialog_hsv->colors_custom[i].green == color->green &&
         dialog_hsv->colors_custom[i].blue == color->blue) {
        found = i + tmp;
        break;
      }
    }
  }


  if(found == -1) {
    dialog_hsv->colors_custom[tmp2-1] = *color;
    if(GTK_WIDGET_REALIZED(GTK_WIDGET(dialog))) {
      gdk_gc_set_rgb_fg_color(dialog_hsv->gc_array[tmp2-1], color);
    }
    hildon_color_chooser_dialog_hsv_set_color_num(dialog_hsv, tmp2-1);
  } else {
    hildon_color_chooser_dialog_hsv_set_color_num(dialog_hsv, found);
  }
}


static gboolean hildon_color_chooser_dialog_hsv_area_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(data);
  int i, num_selected, tot_w, tot_h, spacing, brd, x, y;
  GdkGC **start_gc;
  int tmp, w, h;


  tmp = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);

  if(widget == dialog->area_custom) {
    num_selected = dialog->selected - tmp;
    start_gc = dialog->gc_array + tmp;
    tmp = (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);
    w = dialog->style_info.num_buttons.top; h = dialog->style_info.num_buttons.bottom;
  } else { /* widget == dialog->area_defined */
    num_selected = dialog->selected;
    start_gc = dialog->gc_array;
    w = dialog->style_info.num_buttons.left; h = dialog->style_info.num_buttons.right;
  }

  spacing = dialog->style_info.radio_sizes.top;
  brd = dialog->style_info.radio_sizes.bottom;
  tot_w = dialog->style_info.radio_sizes.left + 2*brd;
  tot_h = dialog->style_info.radio_sizes.right + 2*brd;


  for(i = 0; i < tmp; i++) {
    x = ((i % w) * (tot_w + spacing));
    y = ((i / w) * (tot_h + spacing));

    if(brd) {
      gtk_paint_box(gtk_widget_get_style(GTK_WIDGET(dialog)), widget->window, (i == num_selected) ? GTK_STATE_SELECTED : GTK_STATE_NORMAL,
                    (i == num_selected) ? GTK_SHADOW_IN : GTK_SHADOW_OUT, &event->area, GTK_WIDGET(dialog), "color-radio", x, y, tot_w, tot_h);
    }

    gdk_draw_rectangle(widget->window, start_gc[i], TRUE, event->area.x, event->area.y, event->area.width, event->area.height);
  }


  return FALSE;
}


static gboolean hildon_color_chooser_dialog_hsv_area_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(data);
  int i, hskip, vskip, brd, selection = -1;
  int x, y, tmp, tmp2, w;


  x = event->x;
  y = event->y;


  brd = dialog->style_info.radio_sizes.bottom;
  hskip = dialog->style_info.radio_sizes.left  + dialog->style_info.radio_sizes.top + 2*brd;
  vskip = dialog->style_info.radio_sizes.right + dialog->style_info.radio_sizes.top + 2*brd;


  tmp  = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);
  tmp2 = (dialog->style_info.num_buttons.top * dialog->style_info.num_buttons.bottom);


  if(widget == dialog->area_defined) {
    w = dialog->style_info.num_buttons.left;

    for(i = 0; i < tmp; i++) {
      if(x >= hskip*(i % w) + brd && x < hskip*(i % w) + brd + dialog->style_info.radio_sizes.left &&
         y >= vskip*(i / w) + brd && y < hskip*(i / w) + brd + dialog->style_info.radio_sizes.right) {
        selection = i;
        break;
      }
    }
  } else {
    w = dialog->style_info.num_buttons.top;
    for(i = 0; i < tmp2; i++) {
      if(x >= hskip*(i % w) + brd && x < hskip*(i % w) + brd + dialog->style_info.radio_sizes.left &&
         y >= vskip*(i / w) + brd && y < hskip*(i / w) + brd + dialog->style_info.radio_sizes.right) {
         selection = i + tmp;
         break;
      }
    }
  }


  if(selection != -1) {
    hildon_color_chooser_dialog_hsv_set_color_num(dialog, selection);
  }


  return FALSE;
}


static void hildon_color_chooser_dialog_hsv_chooser_color_changed(HildonColorChooser *chooser, GdkColor *color, gpointer data)
{
  HildonColorChooserDialogHSV *dialog = HILDON_COLOR_CHOOSER_DIALOG_HSV(data);
  HildonColorChooserDialog *dia = HILDON_COLOR_CHOOSER_DIALOG(data);
  char key[128], color_str[13];
  int tmp;


  dia->color = *color;


  tmp = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);


  if(dialog->selected >= tmp) {
    dialog->colors_custom[dialog->selected - tmp] = *color;

    gdk_gc_set_rgb_fg_color(dialog->gc_array[dialog->selected], &dialog->colors_custom[dialog->selected - tmp]);

    gtk_widget_queue_draw(dialog->area_custom);


    if(dialog->gconf_data.client) {
      memset(key, 0, 128);
      memset(color_str, 0, 13);
      g_snprintf(key, 128, "/system/osso/af/color_chooser/custom_color%d", dialog->selected - tmp);
      hildon_color_chooser_dialog_hsv_color_to_ascii_hex(color_str, &dialog->colors_custom[dialog->selected - tmp]);
      gconf_client_set_string(dialog->gconf_data.client, key, color_str, NULL);
    }
  }
}


static void hildon_color_chooser_dialog_hsv_chooser_insensitive_press(GtkWidget *widget, gpointer data)
{
  hildon_banner_show_information(widget, NULL, _("ecdg_ib_colour_selector_predefined"));
}

  /* function has size defaults */
static void hildon_color_chooser_dialog_hsv_refresh_style_info(HildonColorChooserDialogHSV *dialog)
{
  GtkBorder *tmp1, *tmp2, *tmp3;


  gtk_widget_style_get(GTK_WIDGET(dialog), "container_sizes", &tmp1,
                       "radio_sizes", &tmp2, "num_buttons", &tmp3, NULL);


  dialog->style_info.last_num_buttons = dialog->style_info.num_buttons;


  if(tmp1) {
    dialog->style_info.cont_sizes = *tmp1;
    g_free(tmp1);
  } else {
    dialog->style_info.cont_sizes.left = 0;
    dialog->style_info.cont_sizes.right = 8;
    dialog->style_info.cont_sizes.top = 4;
    dialog->style_info.cont_sizes.bottom = 0;
  }

  if(tmp2) {
    dialog->style_info.radio_sizes = *tmp2;
    g_free(tmp2);
  } else {
    dialog->style_info.radio_sizes.left = 16;
    dialog->style_info.radio_sizes.right = 16;
    dialog->style_info.radio_sizes.top = 4;
    dialog->style_info.radio_sizes.bottom = 2;
  }

  if(tmp3) {
    dialog->style_info.num_buttons = *tmp3;
    g_free(tmp3);
  } else {
    dialog->style_info.num_buttons.left = 8;
    dialog->style_info.num_buttons.right = 2;
    dialog->style_info.num_buttons.top = 8;
    dialog->style_info.num_buttons.bottom = 2;
  }
}


static void hildon_color_chooser_dialog_hsv_set_color_num(HildonColorChooserDialogHSV *dialog, gint num)
{
  HildonColorChooserDialog *dia = HILDON_COLOR_CHOOSER_DIALOG(dialog);
  int tmp;


  tmp = (dialog->style_info.num_buttons.left * dialog->style_info.num_buttons.right);


  if(num < tmp) {
    gtk_widget_set_sensitive(dialog->chooser, FALSE);
  } else {
    gtk_widget_set_sensitive(dialog->chooser, TRUE);
  }


  dialog->selected = num;

  gtk_widget_queue_draw(dialog->area_custom);
  gtk_widget_queue_draw(dialog->area_defined);

  dia->color = (num < tmp) ? dialog->colors_defined[num] : dialog->colors_custom[num - tmp];

  hildon_color_chooser_set_color(HILDON_COLOR_CHOOSER(dialog->chooser), (num < tmp) ? &dialog->colors_defined[num] : &dialog->colors_custom[num - tmp]);
}


static void hildon_color_chooser_dialog_hsv_ascii_hex_to_color(gchar *s, GdkColor *color)
{
  int vals[12], i;


  for(i = 0; i < 12; i++) {
    if(s[i] >= '0' && s[i] <= '9') {
      vals[i] = s[i] - 0x30;
    } else if(s[i] >= 'a' && s[i] <= 'f') {
      vals[i] = s[i] - 0x57;
    } else {
      vals[i] = 0;
    }
  }


  color->red   = (vals[0] << 12) | (vals[1] <<  8) | (vals[2 ] <<  4) | (vals[3 ]      );
  color->green = (vals[4] << 12) | (vals[5] <<  8) | (vals[6 ] <<  4) | (vals[7 ]      );
  color->blue  = (vals[8] << 12) | (vals[9] <<  8) | (vals[10] <<  4) | (vals[11]      );
}

static void hildon_color_chooser_dialog_hsv_color_to_ascii_hex(gchar *s, GdkColor *color)
{
  g_snprintf(s, 12, "%x%x%x%x%x%x%x%x%x%x%x%x",
             (color->red >> 12) & 0xf, (color->red >>  8) & 0xf,
             (color->red >>  4) & 0xf, (color->red      ) & 0xf,
             (color->green >> 12) & 0xf, (color->green >>  8) & 0xf,
             (color->green >>  4) & 0xf, (color->green      ) & 0xf,
             (color->blue >> 12) & 0xf, (color->blue >>  8) & 0xf,
             (color->blue >>  4) & 0xf, (color->blue      ) & 0xf);
}
