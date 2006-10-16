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

#include <hildon-widgets/hildon-color-chooser.h>


const char *parent_name = "HildonColorChooser";
const char *plugin_name = "HSV color chooser";

GType export_type(void);


typedef struct {
  HildonColorChooser parent;

  GtkAllocation hba;
  GtkAllocation spa;

  unsigned short currhue;
  unsigned short currsat;
  unsigned short currval;

  int mousestate;
  gboolean mousein;


  GdkWindow *event_window;

  GdkPixbuf *dimmed_plane;
  GdkPixbuf *dimmed_bar;

  struct {
    unsigned short last_expose_hue;

    GTimeVal last_expose_time;

    int expose_queued;
  } expose_info;
} HildonColorChooserHSV;

typedef struct {
  HildonColorChooserClass parent;
} HildonColorChooserHSVClass;


#define HILDON_COLOR_CHOOSER_HSV(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), export_type(), HildonColorChooserHSV))
#define HILDON_COLOR_CHOOSER_HSV_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), export_type(), HildonColorChooserHSVClass))


static HildonColorChooserClass *parent_class = NULL;


  /* "crosshair" is hardcoded for now */
static gchar crosshair[64] = { 0, 0, 0, 2, 2, 0, 0, 0,
                               0, 2, 2, 3, 3, 2, 2, 0,
                               0, 2, 3, 0, 0, 3, 2, 0,
                               2, 3, 0, 0, 0, 0, 3, 2,
                               2, 3, 0, 0, 0, 0, 3, 2,
                               0, 2, 3, 0, 0, 3, 2, 0,
                               0, 2, 2, 3, 3, 2, 2, 0,
                               0, 0, 0, 2, 2, 0, 0, 0};


static void hildon_color_chooser_hsv_init(HildonColorChooserHSV *sel);
static void hildon_color_chooser_hsv_class_init(HildonColorChooserHSVClass *klass);

static void hildon_color_chooser_hsv_dispose(HildonColorChooserHSV *sel);

static void hildon_color_chooser_hsv_size_request(GtkWidget *widget, GtkRequisition *req);
static void hildon_color_chooser_hsv_size_allocate(GtkWidget *widget, GtkAllocation *alloc);

static void hildon_color_chooser_hsv_realize(GtkWidget *widget);

static void hildon_color_chooser_hsv_map(GtkWidget *widget);
static void hildon_color_chooser_hsv_unmap(GtkWidget *widget);

static gboolean hildon_color_chooser_hsv_expose(GtkWidget *widget, GdkEventExpose *event);

static gboolean hildon_color_chooser_button_press(GtkWidget *widget, GdkEventButton *event);
static gboolean hildon_color_chooser_button_release(GtkWidget *widget, GdkEventButton *event);
static gboolean hildon_color_chooser_pointer_motion(GtkWidget *widget, GdkEventMotion *event);

static void hildon_color_chooser_hsv_set_color(HildonColorChooser *sel, GdkColor *color);


static void internal_get_border(GtkWidget *w, char *name, GtkBorder *b);
static void _internal_init_borders(GtkWidget *w, GtkBorder *inner, GtkBorder *outer);

static void internal_invoke_color_changed(HildonColorChooserHSV *sel);


inline void inline_clip_to_alloc(void *s, GtkAllocation *a);

inline void inline_sub_times(GTimeVal *result, GTimeVal *greater, GTimeVal *lesser);

inline void inline_limited_expose(HildonColorChooserHSV *sel);

inline void inline_draw_hue_bar(GtkWidget *widget, int x, int y, int w, int h, int sy, int sh);
inline void inline_draw_hue_bar_dimmed(GtkWidget *widget, int x, int y, int w, int h, int sy, int sh);

inline void inline_draw_sv_plane(HildonColorChooserHSV *sel, int x, int y, int w, int h);
inline void inline_draw_sv_plane_dimmed(HildonColorChooserHSV *sel, int x, int y, int w, int h);

inline void inline_draw_crosshair(unsigned char *buf, int x, int y, int w, int h);


inline void inline_h2rgb(unsigned short hue, unsigned long *rgb);


static gboolean hildon_color_chooser_hsv_expose_timer(gpointer data);


GType export_type()
{
  static GType chooser_type = 0;

  if (!chooser_type) {
    static const GTypeInfo chooser_info =
    {
      sizeof (HildonColorChooserHSVClass),
      NULL,
      NULL,
      (GClassInitFunc) hildon_color_chooser_hsv_class_init,
      NULL,
      NULL,
      sizeof (HildonColorChooserHSV),
      0,
      (GInstanceInitFunc) hildon_color_chooser_hsv_init,
      NULL
    };

    chooser_type = g_type_register_static (HILDON_TYPE_COLOR_CHOOSER,
                                           "HildonColorChooserHSV",
                                           &chooser_info, 0);
  }

  return chooser_type;
}

static void hildon_color_chooser_hsv_init(HildonColorChooserHSV *sel)
{
  GTK_WIDGET_SET_FLAGS (sel, GTK_NO_WINDOW);


  sel->currhue = 0;
  sel->currsat = 0;
  sel->currval = 0;

  sel->mousestate = 0;
  sel->mousein = FALSE;


  g_get_current_time(&sel->expose_info.last_expose_time);

  sel->expose_info.last_expose_hue = sel->currhue;
  sel->expose_info.expose_queued = 0;

  sel->dimmed_plane = NULL;
  sel->dimmed_bar = NULL;
}

static void hildon_color_chooser_hsv_class_init(HildonColorChooserHSVClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  GObjectClass *object_class = G_OBJECT_CLASS(klass);

  HildonColorChooserClass *selection_class = HILDON_COLOR_CHOOSER_CLASS(klass);


  parent_class = g_type_class_peek_parent(klass);


  object_class->dispose = (gpointer) hildon_color_chooser_hsv_dispose;

  widget_class->size_request = hildon_color_chooser_hsv_size_request;
  widget_class->size_allocate = hildon_color_chooser_hsv_size_allocate;

  widget_class->realize = hildon_color_chooser_hsv_realize;

  widget_class->map = hildon_color_chooser_hsv_map;
  widget_class->unmap = hildon_color_chooser_hsv_unmap;

  widget_class->expose_event = hildon_color_chooser_hsv_expose;

  widget_class->button_press_event = hildon_color_chooser_button_press;
  widget_class->button_release_event = hildon_color_chooser_button_release;
  widget_class->motion_notify_event = hildon_color_chooser_pointer_motion;


  selection_class->set_color = hildon_color_chooser_hsv_set_color;


  gtk_widget_class_install_style_property(widget_class,
                                          g_param_spec_boxed("inner_size",
                                                             "Inner sizes",
                                                             "Sizes of SV plane, H bar and spacing",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));
  gtk_widget_class_install_style_property(widget_class,
                                          g_param_spec_boxed("graphic_border",
                                                             "Graphical borders",
                                                             "Size of graphical border",
                                                             GTK_TYPE_BORDER,
                                                             G_PARAM_READABLE));
}


static void hildon_color_chooser_hsv_dispose(HildonColorChooserHSV *sel)
{
  if (sel->dimmed_bar != NULL) {
    g_object_unref (sel->dimmed_bar);
    sel->dimmed_bar = NULL;
  }

  if (sel->dimmed_plane != NULL) {
    g_object_unref (sel->dimmed_plane);
    sel->dimmed_plane = NULL;
  }

  G_OBJECT_CLASS(parent_class)->dispose(sel);
}

static void hildon_color_chooser_hsv_size_request(GtkWidget *widget, GtkRequisition *req)
{
  GtkBorder inner, outer;


  _internal_init_borders(widget, &inner, &outer);


  req->width = inner.left + inner.top + inner.bottom + outer.left + outer.right;
  req->height = inner.right + outer.top + outer.bottom;
}

static void hildon_color_chooser_hsv_size_allocate(GtkWidget *widget, GtkAllocation *alloc)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);
  GtkBorder outer, inner;


  widget->allocation = *alloc;


  _internal_init_borders(widget, &inner, &outer);


  sel->hba.height = alloc->height - outer.top - outer.bottom;
  sel->hba.y = alloc->y + outer.top;
  sel->hba.width = inner.top;
  sel->hba.x = alloc->x + alloc->width - outer.right - inner.top;

  sel->spa.x = alloc->x + outer.left;
  sel->spa.y = alloc->y + outer.top;
  sel->spa.height = alloc->height - outer.top - outer.bottom;
  sel->spa.width = alloc->width - outer.left - outer.right - inner.top - inner.bottom;

  if(GTK_WIDGET_REALIZED(widget)) {
    gdk_window_move_resize(sel->event_window, widget->allocation.x, widget->allocation.y, widget->allocation.width, widget->allocation.height);
  }
}


static void hildon_color_chooser_hsv_realize(GtkWidget *widget)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);
  GdkWindowAttr attributes;
  gint attributes_mask;

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_ONLY;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events(widget) | GDK_BUTTON_PRESS_MASK | 
                          GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
                          GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_MOTION_MASK |
                          GDK_BUTTON1_MOTION_MASK;
  attributes.visual = gtk_widget_get_visual(widget);
  attributes.colormap = gtk_widget_get_colormap(widget);

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_WMCLASS;
  sel->event_window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);


  gdk_window_set_user_data(sel->event_window, widget);


  widget->window = gtk_widget_get_parent_window(widget);


  widget->style = gtk_style_attach(widget->style, widget->window);


  GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);
}


static void hildon_color_chooser_hsv_map(GtkWidget *widget)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);


  GTK_WIDGET_CLASS(parent_class)->map(widget);

  if(sel->event_window) {
    gdk_window_show(sel->event_window);
  }
}

static void hildon_color_chooser_hsv_unmap(GtkWidget *widget)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);


  if(sel->event_window) {
    gdk_window_hide(sel->event_window);
  }

  GTK_WIDGET_CLASS(parent_class)->unmap(widget);
}


inline void inline_clip_to_alloc(void *s, GtkAllocation *a)
{
  struct {
    int x, y, w, h;
  } *area = s;


  if(area->x < a->x) {
    area->w -= a->x - area->x;
    area->x = a->x;
  } if(area->y < a->y) {
    area->h -= a->y - area->y;
    area->y = a->y;
  }
  if(area->x + area->w > a->x + a->width) area->w = a->width - (area->x - a->x);
  if(area->y + area->h > a->y + a->height) area->h = a->height - (area->y - a->y);
}

static gboolean hildon_color_chooser_hsv_expose(GtkWidget *widget, GdkEventExpose *event)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);
  GtkBorder graphical_border;
  struct {
    int x, y, w, h;
  } area;


  if(!GTK_WIDGET_REALIZED(widget)) {
    return FALSE;
  }


  internal_get_border(widget, "graphic_border", &graphical_border);


  if(event->area.width || event->area.height) {
    gdk_draw_rectangle(widget->window,
                       widget->style->black_gc,
                       FALSE,
                       sel->hba.x - 2, 
                       sel->hba.y - 2, 
                       sel->hba.width + 3,
                       sel->hba.height + 3);

    gdk_draw_rectangle(widget->window,
                       widget->style->black_gc,
                       FALSE,
                       sel->spa.x - 2, 
                       sel->spa.y - 2, 
                       sel->spa.width + 3,
                       sel->spa.height + 3);
  }

  if(sel->expose_info.expose_queued) {
    if(GTK_WIDGET_SENSITIVE(widget)) {
      inline_draw_hue_bar(widget, sel->hba.x, sel->hba.y, sel->hba.width, sel->hba.height, sel->hba.y, sel->hba.height);

      inline_draw_sv_plane(sel, sel->spa.x, sel->spa.y, sel->spa.width, sel->spa.height);
    } else {
      inline_draw_hue_bar_dimmed(widget, sel->hba.x, sel->hba.y, sel->hba.width, sel->hba.height, sel->hba.y, sel->hba.height);

      inline_draw_sv_plane_dimmed(sel, sel->spa.x, sel->spa.y, sel->spa.width, sel->spa.height);
    }


    sel->expose_info.expose_queued = 0;

    g_get_current_time(&sel->expose_info.last_expose_time);
  } else {
      /* clip hue bar region */
    area.x = event->area.x;
    area.y = event->area.y;
    area.w = event->area.width;
    area.h = event->area.height;

    inline_clip_to_alloc(&area, &sel->hba);

    if(GTK_WIDGET_SENSITIVE(widget)) {
      inline_draw_hue_bar(widget, area.x, area.y, area.w, area.h, sel->hba.y, sel->hba.height);
    } else {
      inline_draw_hue_bar_dimmed(widget, area.x, area.y, area.w, area.h, sel->hba.y, sel->hba.height);
    }


    area.x = event->area.x;
    area.y = event->area.y;
    area.w = event->area.width;
    area.h = event->area.height;

    inline_clip_to_alloc(&area, &sel->spa);

    if(GTK_WIDGET_SENSITIVE(widget)) {
      inline_draw_sv_plane(sel, area.x, area.y, area.w, area.h);
    } else {
      inline_draw_sv_plane_dimmed(sel, area.x, area.y, area.w, area.h);
    }
  }


  return FALSE;
}


inline void inline_sub_times(GTimeVal *result, GTimeVal *greater, GTimeVal *lesser)
{
  result->tv_sec = greater->tv_sec - lesser->tv_sec;
  result->tv_usec = greater->tv_usec - lesser->tv_usec;

  if(result->tv_usec < 0) {
    result->tv_sec--;
    result->tv_usec += 1000000;
  }
}

#define EXPOSE_INTERVAL 50000
inline void inline_limited_expose(HildonColorChooserHSV *sel)
{
  GTimeVal curr_time, result;
  GdkEventExpose event;


  if(!GTK_WIDGET_REALIZED(GTK_WIDGET(sel))) {
    return;
  }


  if(sel->currhue == sel->expose_info.last_expose_hue) {
    return; /* no need to redraw */
  }


  sel->expose_info.last_expose_hue  = sel->currhue;


  g_get_current_time(&curr_time);

  inline_sub_times(&result, &curr_time, &sel->expose_info.last_expose_time);

  if(result.tv_sec != 0 || result.tv_usec >= EXPOSE_INTERVAL) {
    sel->expose_info.expose_queued = 1;

#if 1
    event.type = GDK_EXPOSE;
    event.area.width = 0;
    event.area.height = 0;
    event.window = GTK_WIDGET(sel)->window;

    gtk_widget_send_expose(GTK_WIDGET(sel), (GdkEvent *)&event);
#else
    gtk_widget_queue_draw(GTK_WIDGET(sel));
#endif
  } else if(!sel->expose_info.expose_queued) {
    sel->expose_info.expose_queued = 1;


    g_timeout_add((EXPOSE_INTERVAL - result.tv_usec)/1000, hildon_color_chooser_hsv_expose_timer, sel);
  }
}

static gboolean hildon_color_chooser_button_press(GtkWidget *widget, GdkEventButton *event)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);
  int x, y, tmp;


  x = (int)event->x + widget->allocation.x;
  y = (int)event->y + widget->allocation.y;


  if(x >= sel->spa.x && x <= sel->spa.x + sel->spa.width &&
     y >= sel->spa.y && y <= sel->spa.y + sel->spa.height) {
    tmp = y - sel->spa.y;
    sel->currsat = tmp * 0xffff / sel->spa.height;
    tmp = x - sel->spa.x;
    sel->currval = tmp * 0xffff / sel->spa.width;

    internal_invoke_color_changed(sel);
    gtk_widget_queue_draw(widget);

    sel->mousestate = 1;
    sel->mousein = TRUE;

    gtk_grab_add(widget);
  } else if(x >= sel->hba.x && x <= sel->hba.x + sel->hba.width &&
            y >= sel->hba.y && y <= sel->hba.y + sel->hba.height) {
    tmp = y - sel->hba.y;
    sel->currhue = tmp * 0xffff / sel->hba.height;

    internal_invoke_color_changed(sel);
    inline_limited_expose(sel);

    sel->mousestate = 2;
    sel->mousein = TRUE;

    gtk_grab_add(widget);
  }


  return FALSE;
}

static gboolean hildon_color_chooser_button_release(GtkWidget *widget, GdkEventButton *event)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);


  if(sel->mousestate) {
    gtk_grab_remove(widget);
  }


  sel->mousestate = 0;
  sel->mousein = FALSE;


  return FALSE;
}

static gboolean hildon_color_chooser_pointer_motion(GtkWidget *widget, GdkEventMotion *event)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);
  GdkModifierType mods;
  gint x, y, tmp;


  if (event->is_hint || (event->window != widget->window))
    gdk_window_get_pointer (widget->window, &x, &y, &mods);


  if(sel->mousestate == 1) {
    if(x >= sel->spa.x && x <= sel->spa.x + sel->spa.width &&
      y >= sel->spa.y && y <= sel->spa.y + sel->spa.height) {
      sel->currsat = (((long)(y - sel->spa.y)) * 0xffff)/sel->spa.height;
      sel->currval = (((long)(x - sel->spa.x)) * 0xffff)/sel->spa.width;

      internal_invoke_color_changed(sel);
      gtk_widget_queue_draw(widget);
    } else if(sel->mousein == TRUE) {
    }
  } else if(sel->mousestate == 2) {
    if(x >= sel->hba.x && x <= sel->hba.x + sel->hba.width &&
      y >= sel->hba.y && y <= sel->hba.y + sel->hba.height) {
      tmp = y - sel->hba.y;
      tmp *= 0xffff;
      tmp /= sel->hba.height;

      if(tmp != sel->currhue) {
        sel->currhue = tmp;

        internal_invoke_color_changed(sel);
        inline_limited_expose(sel);
      }
    } else if(sel->mousein == TRUE) {
    }
  }

  return FALSE;
}


static void internal_get_border(GtkWidget *w, char *name, GtkBorder *b)
{
  GtkBorder *tb;

  gtk_widget_style_get(w, name, &tb, NULL);

  if(tb) {
    *b = *tb;
    g_free(tb);
  } else {
    b->left = 0;
    b->right = 0;
    b->top = 0;
    b->bottom = 0;
  }
}


static void _internal_init_borders(GtkWidget *w, GtkBorder *inner, GtkBorder *outer)
{
  GtkBorder *tb;


  internal_get_border(w, "outer_border", outer);


  gtk_widget_style_get(w, "inner_size", &tb, NULL);

  if(tb) {
    *inner = *tb;
    g_free(tb);
  } else {
    inner->left = 64;
    inner->right = 64;
    inner->top = 12;
    inner->bottom = 2;
  }

  if(inner->left < 2) inner->left = 2;
  if(inner->right < 2) inner->right = 2;
  if(inner->top < 2) inner->top = 2;
}

  /* calculate RGB color & emit signal */
static void internal_invoke_color_changed(HildonColorChooserHSV *sel)
{
  HildonColorChooser *parent_sel = HILDON_COLOR_CHOOSER(sel);
  GdkVisual *system_visual = gdk_visual_get_system();
  unsigned long rgb[3], rgb2[3];


  inline_h2rgb(sel->currhue, rgb);

  rgb2[0] = 0xffffff - rgb[0];
  rgb2[1] = 0xffffff - rgb[1];
  rgb2[2] = 0xffffff - rgb[2];


  parent_sel->color.red   = ((rgb[0] >> 8) + ((rgb2[0] >> 8) * (0xffff - sel->currsat) / 0xffff)) * sel->currval / 0xffff;
  parent_sel->color.green = ((rgb[1] >> 8) + ((rgb2[1] >> 8) * (0xffff - sel->currsat) / 0xffff)) * sel->currval / 0xffff;
  parent_sel->color.blue  = ((rgb[2] >> 8) + ((rgb2[2] >> 8) * (0xffff - sel->currsat) / 0xffff)) * sel->currval / 0xffff;

  parent_sel->color.pixel = ((parent_sel->color.red >> (16 - system_visual->red_prec)) << system_visual->red_shift) |
                            ((parent_sel->color.green >> (16 - system_visual->green_prec)) << system_visual->green_shift) |
                            ((parent_sel->color.blue >> (16 - system_visual->blue_prec)) << system_visual->blue_shift);


  hildon_color_chooser_emit_color_changed(HILDON_COLOR_CHOOSER(sel));
}

  /* do the RGB -> HSV conversion here, not so time critical */
static void hildon_color_chooser_hsv_set_color(HildonColorChooser *sel, GdkColor *color)
{
  HildonColorChooserHSV *sel_hsv = HILDON_COLOR_CHOOSER_HSV(sel);
  unsigned short hue, sat, val;
  unsigned long min, max;
  signed long tmp, diff;

    /* ugly nesting */
  min = MIN(MIN(color->red, color->green), color->blue);
  max = MAX(MAX(color->red, color->green), color->blue);
  diff = max - min;


  val = max;

  if(val > 0 && diff != 0) {
    sat = (diff * 0x0000ffff) / max;

    if(color->red == max) {
      tmp = (signed)color->green - (signed)color->blue;
      tmp *= 10922;
      tmp /= diff;
      if(tmp < 0) {
        tmp += 65532;
      }
      hue = tmp;
    } else if(color->green == max) {
      hue = (((signed long)color->blue - (signed long)color->red)*10922 / diff) + 21844;
    } else {
      hue = (((signed long)color->red - (signed long)color->green)*10922 / diff) + 43688;
    }
  } else {
    hue = 0;
    sat = 0;
  }


  sel_hsv->currhue = hue;
  sel_hsv->currsat = sat;
  sel_hsv->currval = val;


  inline_limited_expose(sel_hsv);
}


#define FULL_COLOR 0x00ffffff
inline void inline_h2rgb(unsigned short hue, unsigned long *rgb)
{
  unsigned short hue_rotation, hue_value;

  hue_rotation  = hue / 10922;
  hue_value     = hue % 10922;


  switch(hue_rotation) {
  case 0:
  case 6:
    rgb[0] = FULL_COLOR;
    rgb[1] = hue_value * 6*256;
    rgb[2] = 0;
    break;
  case 1:
    rgb[0] = FULL_COLOR - (hue_value * 6*256);
    rgb[1] = FULL_COLOR;
    rgb[2] = 0;
    break;
  case 2:
    rgb[0] = 0;
    rgb[1] = FULL_COLOR;
    rgb[2] = hue_value * 6*256;
    break;
  case 3:
    rgb[0] = 0;
    rgb[1] = FULL_COLOR - (hue_value * 6*256);
    rgb[2] = FULL_COLOR;
    break;
  case 4:
    rgb[0] = hue_value * 6*256;
    rgb[1] = 0;
    rgb[2] = FULL_COLOR;
    break;
  case 5:
    rgb[0] = FULL_COLOR;
    rgb[1] = 0;
    rgb[2] = FULL_COLOR - (hue_value * 6*256);
    break;
  default:
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;
    break;
  }
}

#define FULL_COLOR8 0xff
static void intern_h2rgb8(unsigned short hue, unsigned char *rgb)
{
  unsigned short hue_rotation, hue_value;

  hue >>= 8;
  hue_rotation  = hue / 42;
  hue_value     = hue % 42;


  switch(hue_rotation) {
  case 0:
  case 6:
    rgb[0] = FULL_COLOR8;
    rgb[1] = hue_value * 6;
    rgb[2] = 0;
    break;
  case 1:
    rgb[0] = FULL_COLOR8 - (hue_value * 6);
    rgb[1] = FULL_COLOR8;
    rgb[2] = 0;
    break;
  case 2:
    rgb[0] = 0;
    rgb[1] = FULL_COLOR8;
    rgb[2] = hue_value * 6;
    break;
  case 3:
    rgb[0] = 0;
    rgb[1] = FULL_COLOR8 - (hue_value * 6);
    rgb[2] = FULL_COLOR8;
    break;
  case 4:
    rgb[0] = hue_value * 6;
    rgb[1] = 0;
    rgb[2] = FULL_COLOR8;
    break;
  case 5:
    rgb[0] = FULL_COLOR8;
    rgb[1] = 0;
    rgb[2] = FULL_COLOR8 - (hue_value * 6);
    break;
  default:
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;
    break;
  }
}


  /* optimization: do not ask hue for each round but have bilinear vectors */
  /* rethink: benefits from handling data 8 bit? (no shift round) */
inline void inline_draw_hue_bar(GtkWidget *widget, int x, int y, int w, int h, int sy, int sh)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);
  unsigned short hvec, hcurr;
  unsigned char *buf, *ptr, tmp[3];
  int i, j, tmpy;


  if(w <= 0 || h <= 0) {
    return;
  }

  buf = (unsigned char *)g_malloc(w*h*3);

  hvec = 65535/sh;
  hcurr = hvec * (y - sy);

  ptr = buf;


  for(i = 0; i < h; i++) {
    intern_h2rgb8(hcurr, tmp);

    for(j = 0; j < w; j++) {
      ptr[0] = tmp[0];
      ptr[1] = tmp[1];
      ptr[2] = tmp[2];
      ptr += 3;
    }

    hcurr += hvec;
  }


  gdk_draw_rgb_image(widget->parent->window, widget->style->fg_gc[0], x, y, w, h, GDK_RGB_DITHER_NONE, buf, w*3);

  tmpy = sel->hba.y + (sel->currhue * sel->hba.height / 0xffff);
  gdk_draw_line(widget->parent->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], sel->hba.x, tmpy, sel->hba.x + sel->hba.width - 1, tmpy);

  if((((sel->currhue * sel->hba.height) & 0xffff) > 0x8000) && (tmpy < (sel->hba.y + sel->hba.height))) {
    gdk_draw_line(widget->parent->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], sel->hba.x, tmpy+1, sel->hba.x + sel->hba.width - 1, tmpy+1);
  } else if(tmpy > sel->hba.y) {
    gdk_draw_line(widget->parent->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], sel->hba.x, tmpy-1, sel->hba.x + sel->hba.width - 1, tmpy-1);
  }


  g_free(buf);
}

inline void inline_draw_hue_bar_dimmed(GtkWidget *widget, int x, int y, int w, int h, int sy, int sh)
{
   HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(widget);


  if(w <= 0 || h <= 0) {
    return;
  }

  /* We need to create (and cache) the pixbuf if we don't 
   * have it yet */
  if (sel->dimmed_bar == NULL) {
    int i, j;
    unsigned short hvec, hcurr, avg;
    unsigned char *buf, *ptr, tmp[3];
    buf = (unsigned char *)g_malloc(w*h*3);

    hvec = 65535/sh;
    hcurr = hvec * (y - sy);

    ptr = buf;


    for(i = 0; i < h; i++) {
      intern_h2rgb8(hcurr, tmp);

      for(j = 0; j < w; j++) {
        avg = ((unsigned short)tmp[0]*3 + (unsigned short)tmp[1]*2 + (unsigned short)tmp[2])/6;
        ptr[0] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
        ptr[1] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
        ptr[2] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
        ptr += 3;
      }

      hcurr += hvec;
    }

    sel->dimmed_bar = gdk_pixbuf_new_from_data (buf, GDK_COLORSPACE_RGB, FALSE, 8, w, h, w * 3, g_free, buf);
  }

  gdk_draw_pixbuf (widget->parent->window, widget->style->fg_gc [0], sel->dimmed_bar, 0, 0, x, y, w, h, GDK_RGB_DITHER_NONE, 0, 0);
}


inline void inline_draw_crosshair(unsigned char *buf, int x, int y, int w, int h)
{
  int i, j, sx, sy;

    /* bad "clipping", clip the loop to save cpu */
  for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
      sx = j + x; sy = i + y;

      if(sx >= 0 && sx < w && sy >= 0 && sx < h) {
        if(crosshair[j + 8*i]) {
          if(crosshair[j + 8*i] & 0x1) {
            buf[(sx)*3+(sy)*w*3+0] = 255;
            buf[(sx)*3+(sy)*w*3+1] = 255;
            buf[(sx)*3+(sy)*w*3+2] = 255;
          } else {
            buf[(sx)*3+(sy)*w*3+0] = 0;
            buf[(sx)*3+(sy)*w*3+1] = 0;
            buf[(sx)*3+(sy)*w*3+2] = 0;
          }
        }
      }
    }
  }
}


inline void inline_draw_sv_plane(HildonColorChooserHSV *sel, int x, int y, int w, int h)
{
  GtkWidget *widget = GTK_WIDGET(sel);
  unsigned char *buf, *ptr;
  unsigned long rgbx[3] = { 0x00ffffff, 0x00ffffff, 0x00ffffff }, rgbtmp[3];
  signed long rgby[3];
  int tmp = sel->spa.width*sel->spa.height, i, j;


  if(w <= 0 || h <= 0) {
    return;
  }


  buf = (unsigned char *)g_malloc(w*h*3);

  ptr = buf;


  inline_h2rgb(sel->currhue, rgbtmp);

  rgby[0] = rgbtmp[0] - rgbx[0];
  rgby[1] = rgbtmp[1] - rgbx[1];
  rgby[2] = rgbtmp[2] - rgbx[2];

  rgbx[0] /= sel->spa.width;
  rgbx[1] /= sel->spa.width;
  rgbx[2] /= sel->spa.width;

  rgby[0] /= tmp;
  rgby[1] /= tmp;
  rgby[2] /= tmp;


  rgbx[0] += (y - sel->spa.y)*rgby[0];
  rgbx[1] += (y - sel->spa.y)*rgby[1];
  rgbx[2] += (y - sel->spa.y)*rgby[2];


  for(i = 0; i < h; i++) {
    rgbtmp[0] = rgbx[0] * (x - sel->spa.x);
    rgbtmp[1] = rgbx[1] * (x - sel->spa.x);
    rgbtmp[2] = rgbx[2] * (x - sel->spa.x);

    for(j = 0; j < w; j++) {
      ptr[0] = rgbtmp[0] >> 16;
      ptr[1] = rgbtmp[1] >> 16;
      ptr[2] = rgbtmp[2] >> 16;
      rgbtmp[0] += rgbx[0];
      rgbtmp[1] += rgbx[1];
      rgbtmp[2] += rgbx[2];
      ptr += 3;
    }

    rgbx[0] += rgby[0];
    rgbx[1] += rgby[1];
    rgbx[2] += rgby[2];
  }


  inline_draw_crosshair(buf, (sel->spa.width * sel->currval / 0xffff) - x + sel->spa.x - 4, (sel->spa.height * sel->currsat / 0xffff) - y + sel->spa.y - 4, w, h);


  gdk_draw_rgb_image(widget->parent->window, widget->style->fg_gc[0], x, y, w, h, GDK_RGB_DITHER_NONE, buf, w*3);


  g_free(buf);
}

inline void inline_draw_sv_plane_dimmed(HildonColorChooserHSV *sel, int x, int y, int w, int h)
{
  GtkWidget *widget = GTK_WIDGET(sel);

  if(w <= 0 || h <= 0) {
    return;
  }

  /* We need to create (and cache) the pixbuf if we don't 
   * have it yet */
  if (sel->dimmed_plane == NULL) {
    unsigned char *buf, *ptr;
    unsigned long rgbx[3] = { 0x00ffffff, 0x00ffffff, 0x00ffffff }, rgbtmp[3];
    unsigned long avg;
    signed long rgby[3];
    int tmp = sel->spa.width*sel->spa.height, i, j;

    buf = (unsigned char *)g_malloc(w*h*3);

    ptr = buf;

    /* possibe optimization: as we are drawing grayscale plane, there might
       be some simpler algorithm to do this*/
    rgbtmp[0] = 0x00ffffff;
    rgbtmp[1] = 0x00000000;
    rgbtmp[2] = 0x00000000;

    rgby[0] = rgbtmp[0] - rgbx[0];
    rgby[1] = rgbtmp[1] - rgbx[1];
    rgby[2] = rgbtmp[2] - rgbx[2];

    rgbx[0] /= sel->spa.width;
    rgbx[1] /= sel->spa.width;
    rgbx[2] /= sel->spa.width;

    rgby[0] /= tmp;
    rgby[1] /= tmp;
    rgby[2] /= tmp;

    rgbx[0] += (y - sel->spa.y)*rgby[0];
    rgbx[1] += (y - sel->spa.y)*rgby[1];
    rgbx[2] += (y - sel->spa.y)*rgby[2];

    for(i = 0; i < h; i++) {
      rgbtmp[0] = rgbx[0] * (x - sel->spa.x);
      rgbtmp[1] = rgbx[1] * (x - sel->spa.x);
      rgbtmp[2] = rgbx[2] * (x - sel->spa.x);

      for(j = 0; j < w; j++) {
        avg = (rgbtmp[0] + rgbtmp[1] + rgbtmp[2])/3;
        avg >>= 16;
        ptr[0] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
        ptr[1] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
        ptr[2] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
        rgbtmp[0] += rgbx[0];
        rgbtmp[1] += rgbx[1];
        rgbtmp[2] += rgbx[2];
        ptr += 3;
      }

      rgbx[0] += rgby[0];
      rgbx[1] += rgby[1];
      rgbx[2] += rgby[2];
    }
  
    sel->dimmed_plane = gdk_pixbuf_new_from_data (buf, GDK_COLORSPACE_RGB, FALSE, 8, w, h, w * 3, g_free, buf);
  }

  gdk_draw_pixbuf (widget->parent->window, widget->style->fg_gc [0], sel->dimmed_plane, 0, 0, x, y, w, h, GDK_RGB_DITHER_NONE, 0, 0);
}


static gboolean hildon_color_chooser_hsv_expose_timer(gpointer data)
{
  HildonColorChooserHSV *sel = HILDON_COLOR_CHOOSER_HSV(data);


  if(sel->expose_info.expose_queued) {
    gtk_widget_queue_draw(GTK_WIDGET(data));
  }


  return FALSE;
}
