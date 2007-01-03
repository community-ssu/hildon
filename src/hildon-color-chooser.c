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

#include                                        <gtk/gtk.h>
#include                                        "hildon-color-chooser.h"
#include                                        "hildon-color-chooser-private.h"

static GtkWidgetClass*                          parent_class = NULL;

/* "crosshair" is hardcoded for now */
static gchar crosshair[64]                      = { 0, 0, 0, 2, 2, 0, 0, 0,
                                                    0, 2, 2, 3, 3, 2, 2, 0,
                                                    0, 2, 3, 0, 0, 3, 2, 0,
                                                    2, 3, 0, 0, 0, 0, 3, 2,
                                                    2, 3, 0, 0, 0, 0, 3, 2,
                                                    0, 2, 3, 0, 0, 3, 2, 0,
                                                    0, 2, 2, 3, 3, 2, 2, 0,
                                                    0, 0, 0, 2, 2, 0, 0, 0};

static void 
hildon_color_chooser_init                       (HildonColorChooser *self);

static void 
hildon_color_chooser_class_init                 (HildonColorChooserClass *klass);

static void 
hildon_color_chooser_dispose                    (HildonColorChooser *self);

static void 
hildon_color_chooser_size_request               (GtkWidget *widget, 
                                                 GtkRequisition *req);

static void 
hildon_color_chooser_size_allocate              (GtkWidget *widget, 
                                                 GtkAllocation *alloc);

static void
hildon_color_chooser_realize                    (GtkWidget *widget);

static void 
hildon_color_chooser_map                        (GtkWidget *widget);

static void 
hildon_color_chooser_unmap                      (GtkWidget *widget);

static gboolean 
hildon_color_chooser_expose                     (GtkWidget *widget, 
                                                 GdkEventExpose *event);

static gboolean
hildon_color_chooser_button_press               (GtkWidget *widget, 
                                                 GdkEventButton *event);

static gboolean
hildon_color_chooser_button_release             (GtkWidget *widget, 
                                                 GdkEventButton *event);

static gboolean 
hildon_color_chooser_pointer_motion             (GtkWidget *widget, 
                                                 GdkEventMotion *event);

static void 
get_border                                      (GtkWidget *w, 
                                                 char *name, 
                                                 GtkBorder *b);

static void 
init_borders                                    (GtkWidget *w, 
                                                 GtkBorder *inner, 
                                                 GtkBorder *outer);

inline void 
inline_clip_to_alloc                            (void *s, 
                                                 GtkAllocation *a);

inline void 
inline_sub_times                                (GTimeVal *result, 
                                                 GTimeVal *greater, 
                                                 GTimeVal *lesser);

inline void 
inline_limited_expose                           (HildonColorChooser *self);

inline void 
inline_draw_hue_bar                             (GtkWidget *widget, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h, 
                                                 int sy, 
                                                 int sh);

inline void
inline_draw_hue_bar_dimmed                      (GtkWidget *widget, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h, 
                                                 int sy, 
                                                 int sh);

inline void 
inline_draw_sv_plane                            (HildonColorChooser *self, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h);

inline void 
inline_draw_sv_plane_dimmed                     (HildonColorChooser *self, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h);

inline void 
inline_draw_crosshair                           (unsigned char *buf, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h);

inline void 
inline_h2rgb                                    (unsigned short hue, 
                                                 unsigned long *rgb);

static gboolean
hildon_color_chooser_expose_timer               (gpointer data);

#define                                         EXPOSE_INTERVAL 50000

#define                                         FULL_COLOR8 0xff

#define                                         FULL_COLOR 0x00ffffff

enum 
{
    COLOR_CHANGED,
    LAST_SIGNAL
};

static guint                                    color_chooser_signals [LAST_SIGNAL] = { 0 };

GType G_GNUC_CONST
hildon_color_chooser_get_type                   (void)
{
    static GType chooser_type = 0;

    if (!chooser_type) {
        static const GTypeInfo chooser_info =
        {
            sizeof (HildonColorChooserClass),
            NULL,
            NULL,
            (GClassInitFunc) hildon_color_chooser_class_init,
            NULL,
            NULL,
            sizeof (HildonColorChooser),
            0,
            (GInstanceInitFunc) hildon_color_chooser_init,
            NULL
        };

        chooser_type = g_type_register_static (GTK_TYPE_WIDGET,
                "HildonColorChooser",
                &chooser_info, 0);
    }

    return chooser_type;
}

static void
hildon_color_chooser_init                       (HildonColorChooser *sel)
{

    GTK_WIDGET_SET_FLAGS (sel, GTK_NO_WINDOW);
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);
    g_assert (priv);

    priv->currhue = 0;
    priv->currsat = 0;
    priv->currval = 0;

    priv->mousestate = 0;
    priv->mousein = FALSE;

    g_get_current_time (&priv->expose_info.last_expose_time);

    priv->expose_info.last_expose_hue = priv->currhue;
    priv->expose_info.expose_queued = 0;

    priv->dimmed_plane = NULL;
    priv->dimmed_bar = NULL;
}

static void
hildon_color_chooser_class_init                 (HildonColorChooserClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    parent_class = g_type_class_peek_parent (klass);
    
    object_class->dispose = (gpointer) hildon_color_chooser_dispose;

    widget_class->size_request          = hildon_color_chooser_size_request;
    widget_class->size_allocate         = hildon_color_chooser_size_allocate;
    widget_class->realize               = hildon_color_chooser_realize;
    widget_class->map                   = hildon_color_chooser_map;
    widget_class->unmap                 = hildon_color_chooser_unmap;
    widget_class->expose_event          = hildon_color_chooser_expose;
    widget_class->button_press_event    = hildon_color_chooser_button_press;
    widget_class->button_release_event  = hildon_color_chooser_button_release;
    widget_class->motion_notify_event   = hildon_color_chooser_pointer_motion;

    gtk_widget_class_install_style_property (widget_class,
            g_param_spec_boxed ("inner_size",
                "Inner sizes",
                "Sizes of SV plane, H bar and spacing",
                GTK_TYPE_BORDER,
                G_PARAM_READABLE));

    gtk_widget_class_install_style_property (widget_class,
            g_param_spec_boxed ("graphic_border",
                "Graphical borders",
                "Size of graphical border",
                GTK_TYPE_BORDER,
                G_PARAM_READABLE));

    color_chooser_signals[COLOR_CHANGED] = g_signal_new("color-changed", 
            G_OBJECT_CLASS_TYPE (object_class),
            G_SIGNAL_RUN_FIRST, 
            G_STRUCT_OFFSET (HildonColorChooserClass, color_changed),
            NULL, 
            NULL, 
            g_cclosure_marshal_VOID__VOID, 
            G_TYPE_NONE, 
            0);
}

static void
hildon_color_chooser_dispose                    (HildonColorChooser *sel)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);
    g_assert (priv);

    if (priv->dimmed_bar != NULL) {
        g_object_unref (priv->dimmed_bar);
        priv->dimmed_bar = NULL;
    }

    if (priv->dimmed_plane != NULL) {
        g_object_unref (priv->dimmed_plane);
        priv->dimmed_plane = NULL;
    }

    G_OBJECT_CLASS (parent_class)->dispose (G_OBJECT (sel));
}

static void
hildon_color_chooser_size_request               (GtkWidget *widget, 
                                                 GtkRequisition *req)
{
    GtkBorder inner, outer;

    init_borders (widget, &inner, &outer);

    req->width = inner.left + inner.top + inner.bottom + outer.left + outer.right;
    req->height = inner.right + outer.top + outer.bottom;
}

static void 
hildon_color_chooser_size_allocate              (GtkWidget *widget, 
                                                 GtkAllocation *alloc)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);
    GtkBorder outer, inner;

    g_assert (priv);

    widget->allocation = *alloc;
    
    init_borders(widget, &inner, &outer);

    priv->hba.height = alloc->height - outer.top - outer.bottom;
    priv->hba.y = alloc->y + outer.top;
    priv->hba.width = inner.top;
    priv->hba.x = alloc->x + alloc->width - outer.right - inner.top;

    priv->spa.x = alloc->x + outer.left;
    priv->spa.y = alloc->y + outer.top;
    priv->spa.height = alloc->height - outer.top - outer.bottom;
    priv->spa.width = alloc->width - outer.left - outer.right - inner.top - inner.bottom;

    if (GTK_WIDGET_REALIZED (widget)) {
        gdk_window_move_resize (priv->event_window, 
                widget->allocation.x, 
                widget->allocation.y, 
                widget->allocation.width, 
                widget->allocation.height);
    }
}


static void
hildon_color_chooser_realize                    (GtkWidget *widget)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    g_assert (priv);
    GdkWindowAttr attributes;
    gint attributes_mask;

    attributes.x = widget->allocation.x;
    attributes.y = widget->allocation.y;
    attributes.width = widget->allocation.width;
    attributes.height = widget->allocation.height;
    attributes.wclass = GDK_INPUT_ONLY;
    attributes.window_type = GDK_WINDOW_CHILD;

    attributes.event_mask = gtk_widget_get_events (widget) | 
        GDK_BUTTON_PRESS_MASK           | 
        GDK_BUTTON_RELEASE_MASK         | 
        GDK_POINTER_MOTION_MASK         |
        GDK_POINTER_MOTION_HINT_MASK    | 
        GDK_BUTTON_MOTION_MASK          |
        GDK_BUTTON1_MOTION_MASK;

    attributes.visual = gtk_widget_get_visual (widget);
    attributes.colormap = gtk_widget_get_colormap (widget);

    attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_WMCLASS;
    priv->event_window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);


    gdk_window_set_user_data (priv->event_window, widget);
    widget->window = gtk_widget_get_parent_window (widget);

    widget->style = gtk_style_attach (widget->style, widget->window);

    GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
}

static void 
hildon_color_chooser_map                        (GtkWidget *widget)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    g_assert (priv);
    GTK_WIDGET_CLASS(parent_class)->map(widget);

    if (priv->event_window) {
        gdk_window_show (priv->event_window);
    }
}

static void 
hildon_color_chooser_unmap                      (GtkWidget *widget)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    g_assert (priv);

    if (priv->event_window) {
        gdk_window_hide (priv->event_window);
    }

    GTK_WIDGET_CLASS (parent_class)->unmap (widget);
}

inline void 
inline_clip_to_alloc                            (void *s, 
                                                 GtkAllocation *a)
{
    struct {
        int x, y, w, h;
    } *area = s;


    if (area->x < a->x) {
        area->w -= a->x - area->x;
        area->x = a->x;
    } if (area->y < a->y) {
        area->h -= a->y - area->y;
        area->y = a->y;
    }
    if (area->x + area->w > a->x + a->width) 
        area->w = a->width - (area->x - a->x);

    if (area->y + area->h > a->y + a->height) 
        area->h = a->height - (area->y - a->y);
}

static gboolean 
hildon_color_chooser_expose                     (GtkWidget *widget, 
                                                 GdkEventExpose *event)
{
    HildonColorChooser *sel = HILDON_COLOR_CHOOSER (widget);
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    g_assert (priv);

    GtkBorder graphical_border;

    struct {
        int x, y, w, h;
    } area;


    if(! GTK_WIDGET_REALIZED (widget)) {
        return FALSE;
    }

    get_border (widget, "graphic_border", &graphical_border);
    
    if (event->area.width || event->area.height) {

        gdk_draw_rectangle (widget->window,
                widget->style->black_gc,
                FALSE,
                priv->hba.x - 2, 
                priv->hba.y - 2, 
                priv->hba.width + 3,
                priv->hba.height + 3);

        gdk_draw_rectangle (widget->window,
                widget->style->black_gc,
                FALSE,
                priv->spa.x - 2, 
                priv->spa.y - 2, 
                priv->spa.width + 3,
                priv->spa.height + 3);
    }

    if (priv->expose_info.expose_queued) {
        if (GTK_WIDGET_SENSITIVE (widget)) {
            inline_draw_hue_bar (widget, priv->hba.x, priv->hba.y, priv->hba.width, priv->hba.height, priv->hba.y, priv->hba.height);

            inline_draw_sv_plane (sel, priv->spa.x, priv->spa.y, priv->spa.width, priv->spa.height);
        } else {
            inline_draw_hue_bar_dimmed (widget, priv->hba.x, priv->hba.y, priv->hba.width, priv->hba.height, priv->hba.y, priv->hba.height);

            inline_draw_sv_plane_dimmed (sel, priv->spa.x, priv->spa.y, priv->spa.width, priv->spa.height);
        }

        priv->expose_info.expose_queued = 0;

        g_get_current_time (&priv->expose_info.last_expose_time);

    } else {
        /* clip hue bar region */
        area.x = event->area.x;
        area.y = event->area.y;
        area.w = event->area.width;
        area.h = event->area.height;

        inline_clip_to_alloc (&area, &priv->hba);

        if(GTK_WIDGET_SENSITIVE (widget)) {
            inline_draw_hue_bar (widget, area.x, area.y, area.w, area.h, priv->hba.y, priv->hba.height);
        } else {
            inline_draw_hue_bar_dimmed (widget, area.x, area.y, area.w, area.h, priv->hba.y, priv->hba.height);
        }
        
        area.x = event->area.x;
        area.y = event->area.y;
        area.w = event->area.width;
        area.h = event->area.height;

        inline_clip_to_alloc (&area, &priv->spa);

        if (GTK_WIDGET_SENSITIVE (widget)) {
            inline_draw_sv_plane (sel, area.x, area.y, area.w, area.h);
        } else {
            inline_draw_sv_plane_dimmed (sel, area.x, area.y, area.w, area.h);
        }
    }

    return FALSE;
}


inline void 
inline_sub_times                                (GTimeVal *result, 
                                                 GTimeVal *greater, 
                                                 GTimeVal *lesser)
{
    result->tv_sec = greater->tv_sec - lesser->tv_sec;
    result->tv_usec = greater->tv_usec - lesser->tv_usec;

    if (result->tv_usec < 0) {
        result->tv_sec--;
        result->tv_usec += 1000000;
    }
}

inline void 
inline_limited_expose                           (HildonColorChooser *sel)
{
    GTimeVal curr_time, result;
    GdkEventExpose event;
    HildonColorChooserPrivate *priv; 
   
    if (! GTK_WIDGET_REALIZED (GTK_WIDGET (sel))) {
        return;
    }

    priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);
    g_assert (priv);

    if(priv->currhue == priv->expose_info.last_expose_hue) {
        return; /* no need to redraw */
    }

    priv->expose_info.last_expose_hue = priv->currhue;

    g_get_current_time (&curr_time);

    inline_sub_times (&result, &curr_time, &priv->expose_info.last_expose_time);

    if(result.tv_sec != 0 || result.tv_usec >= EXPOSE_INTERVAL) {

        priv->expose_info.expose_queued = 1;

        event.type = GDK_EXPOSE;
        event.area.width = 0;
        event.area.height = 0;
        event.window = GTK_WIDGET(sel)->window;

        gtk_widget_send_expose(GTK_WIDGET(sel), (GdkEvent *)&event);

    } else if(! priv->expose_info.expose_queued) {
        priv->expose_info.expose_queued = 1;
        g_timeout_add ((EXPOSE_INTERVAL - result.tv_usec) / 1000, hildon_color_chooser_expose_timer, sel);
    }
}

static gboolean 
hildon_color_chooser_button_press               (GtkWidget *widget, 
                                                 GdkEventButton *event)
{
    HildonColorChooser *sel = HILDON_COLOR_CHOOSER (widget);
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    g_assert (priv);
    int x, y, tmp;

    x = (int) event->x + widget->allocation.x;
    y = (int) event->y + widget->allocation.y;

    if (x >= priv->spa.x && x <= priv->spa.x + priv->spa.width &&
        y >= priv->spa.y && y <= priv->spa.y + priv->spa.height) {

        tmp = y - priv->spa.y;
        priv->currsat = tmp * 0xffff / priv->spa.height;
        tmp = x - priv->spa.x;
        priv->currval = tmp * 0xffff / priv->spa.width;

        g_signal_emit (sel, color_chooser_signals[COLOR_CHANGED], 0);
        gtk_widget_queue_draw (widget);

        priv->mousestate = 1;
        priv->mousein = TRUE;

        gtk_grab_add(widget);

    } else if (x >= priv->hba.x && x <= priv->hba.x + priv->hba.width &&
               y >= priv->hba.y && y <= priv->hba.y + priv->hba.height) {

        tmp = y - priv->hba.y;
        priv->currhue = tmp * 0xffff / priv->hba.height;

        g_signal_emit (sel, color_chooser_signals[COLOR_CHANGED], 0);
        inline_limited_expose (sel);

        priv->mousestate = 2;
        priv->mousein = TRUE;

        gtk_grab_add (widget);
    }

    return FALSE;
}

static gboolean
hildon_color_chooser_button_release             (GtkWidget *widget, 
                                                 GdkEventButton *event)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    g_assert (priv);

    if (priv->mousestate) {
        gtk_grab_remove (widget);
    }

    priv->mousestate = 0;
    priv->mousein = FALSE;

    return FALSE;
}

static gboolean 
hildon_color_chooser_pointer_motion             (GtkWidget *widget, 
                                                 GdkEventMotion *event)
{
    HildonColorChooser *sel = HILDON_COLOR_CHOOSER (widget);
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    GdkModifierType mods;
    gint x, y, tmp;

    g_assert (priv);

    if (event->is_hint || (event->window != widget->window))
        gdk_window_get_pointer (widget->window, &x, &y, &mods);

    if (priv->mousestate == 1) {
        if (x >= priv->spa.x && x <= priv->spa.x + priv->spa.width &&
            y >= priv->spa.y && y <= priv->spa.y + priv->spa.height) {

            priv->currsat = (((long)(y - priv->spa.y)) * 0xffff) / priv->spa.height;
            priv->currval = (((long)(x - priv->spa.x)) * 0xffff) / priv->spa.width;

            g_signal_emit (sel, color_chooser_signals[COLOR_CHANGED], 0);
            gtk_widget_queue_draw(widget);

        } else if (priv->mousein == TRUE) {
        }

    } else if (priv->mousestate == 2) {
        if (x >= priv->hba.x && x <= priv->hba.x + priv->hba.width &&
            y >= priv->hba.y && y <= priv->hba.y + priv->hba.height) {
            tmp = y - priv->hba.y;
            tmp *= 0xffff;
            tmp /= priv->hba.height;

            if(tmp != priv->currhue) {
                priv->currhue = tmp;

                g_signal_emit (sel, color_chooser_signals[COLOR_CHANGED], 0);
                inline_limited_expose (sel);
            }

        } else if (priv->mousein == TRUE) {
        }
    }

    return FALSE;
}

static void 
get_border                                      (GtkWidget *w, 
                                                 char *name, 
                                                 GtkBorder *b)
{
    GtkBorder *tb;

    gtk_widget_style_get (w, name, &tb, NULL);

    if(tb) {
        *b = *tb;
        g_free (tb);
    } else {
        b->left = 0;
        b->right = 0;
        b->top = 0;
        b->bottom = 0;
    }
}

static void 
init_borders                                    (GtkWidget *w, 
                                                 GtkBorder *inner, 
                                                 GtkBorder *outer)
{
    GtkBorder *tb;

    get_border (w, "outer_border", outer);

    gtk_widget_style_get (w, "inner_size", &tb, NULL);

    if (tb) {
        *inner = *tb;
        g_free (tb);
    } else {
        inner->left = 64;
        inner->right = 64;
        inner->top = 12;
        inner->bottom = 2;
    }

    if (inner->left < 2) inner->left = 2;
    if (inner->right < 2) inner->right = 2;
    if (inner->top < 2) inner->top = 2;
}

/**
 * hildon_color_chooser_set_color:
 * @chooser: a #HildonColorChooser
 * @color: a color to be set
 *
 * Sets the color selected in the widget.
 */
void 
hildon_color_chooser_set_color                  (HildonColorChooser *sel, 
                                                 GdkColor *color)
{
    unsigned short hue, sat, val;
    unsigned long min, max;
    signed long tmp, diff;
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);

    g_assert (priv);

    /* ugly nesting */
    min = MIN (MIN (color->red, color->green), color->blue);
    max = MAX (MAX (color->red, color->green), color->blue);
    diff = max - min;
    
    val = max;

    if (val > 0 && diff != 0) {
        sat = (diff * 0x0000ffff) / max;

        if (color->red == max) {
            tmp = (signed) color->green - (signed) color->blue;
            tmp *= 10922;
            tmp /= diff;
            if(tmp < 0) {
                tmp += 65532;
            }
            hue = tmp;
        } else if (color->green == max) {
            hue = (((signed long) color->blue - (signed long)color->red) * 10922 / diff) + 21844;
        } else {
            hue = (((signed long) color->red -(signed long) color->green) * 10922 / diff) + 43688;
        }
    } else {
        hue = 0;
        sat = 0;
    }

    priv->currhue = hue;
    priv->currsat = sat;
    priv->currval = val;

    inline_limited_expose (sel);
    g_signal_emit (sel, color_chooser_signals[COLOR_CHANGED], 0);
}

inline void
inline_h2rgb                                    (unsigned short hue, 
                                                 unsigned long *rgb)
{
    unsigned short hue_rotation, hue_value;

    hue_rotation  = hue / 10922;
    hue_value     = hue % 10922;

    switch (hue_rotation) {

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

static void 
intern_h2rgb8                                   (unsigned short hue, 
                                                 unsigned char *rgb)
{
    unsigned short hue_rotation, hue_value;

    hue >>= 8;
    hue_rotation  = hue / 42;
    hue_value     = hue % 42;

    switch (hue_rotation) {
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
inline void 
inline_draw_hue_bar                             (GtkWidget *widget, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h, 
                                                 int sy, 
                                                 int sh)
{
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (widget);

    unsigned short hvec, hcurr;
    unsigned char *buf, *ptr, tmp[3];
    int i, j, tmpy;
    g_assert (priv);

    if (w <= 0 || h <= 0) {
        return;
    }

    buf = (unsigned char *) g_malloc (w * h * 3);

    hvec = 65535 / sh;
    hcurr = hvec * (y - sy);
    
    ptr = buf;

    for (i = 0; i < h; i++) {
        intern_h2rgb8 (hcurr, tmp);

        for (j = 0; j < w; j++) {
            ptr[0] = tmp[0];
            ptr[1] = tmp[1];
            ptr[2] = tmp[2];
            ptr += 3;
        }

        hcurr += hvec;
    }


    gdk_draw_rgb_image (widget->parent->window, 
            widget->style->fg_gc[0], 
            x, y, 
            w, h, 
            GDK_RGB_DITHER_NONE, buf, w * 3);

    tmpy = priv->hba.y + (priv->currhue * priv->hba.height / 0xffff);
    gdk_draw_line (widget->parent->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], priv->hba.x, tmpy, priv->hba.x + priv->hba.width - 1, tmpy);

    if ((((priv->currhue * priv->hba.height) & 0xffff) > 0x8000) && (tmpy < (priv->hba.y + priv->hba.height))) {
        gdk_draw_line (widget->parent->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], 
                priv->hba.x, tmpy+1, priv->hba.x + priv->hba.width - 1, tmpy+1);
    } else if (tmpy > priv->hba.y) {
        gdk_draw_line(widget->parent->window, widget->style->fg_gc[GTK_WIDGET_STATE(widget)], priv->hba.x, 
                tmpy-1, priv->hba.x + priv->hba.width - 1, tmpy-1);
    }

    g_free(buf);
}

inline void 
inline_draw_hue_bar_dimmed                      (GtkWidget *widget, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h, 
                                                 int sy, 
                                                 int sh)
{
    HildonColorChooser *sel = HILDON_COLOR_CHOOSER (widget);
    HildonColorChooserPrivate *priv;

    if (w <= 0 || h <= 0) {
        return;
    }

    priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);
    g_assert (priv);

    /* We need to create (and cache) the pixbuf if we don't 
     * have it yet */
    if (priv->dimmed_bar == NULL) {
        int i, j;
        unsigned short hvec, hcurr, avg;
        unsigned char *buf, *ptr, tmp[3];
        buf = (unsigned char *) g_malloc (w * h * 3);

        hvec = 65535 / sh;
        hcurr = hvec * (y - sy);
        ptr = buf;

        for (i = 0; i < h; i++) {
            intern_h2rgb8 (hcurr, tmp);

            for(j = 0; j < w; j++) {
                avg = ((unsigned short) tmp[0]*3 + (unsigned short) tmp[1]*2 + (unsigned short) tmp[2])/6;
                ptr[0] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
                ptr[1] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
                ptr[2] = ((((i % 2) + j) % 2) == 0) ? MIN ((avg * 0.7) + 180, 255) : MIN ((avg * 0.7) + 120, 255);
                ptr += 3;
            }

            hcurr += hvec;
        }

        priv->dimmed_bar = gdk_pixbuf_new_from_data (buf, GDK_COLORSPACE_RGB, FALSE, 8, w, h, w * 3, (gpointer) g_free, buf);
    }

    gdk_draw_pixbuf (widget->parent->window, widget->style->fg_gc [0], priv->dimmed_bar, 0, 0, x, y, w, h, GDK_RGB_DITHER_NONE, 0, 0);
}

inline void 
inline_draw_crosshair                           (unsigned char *buf, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h)
{
    int i, j, sx, sy;

    /* bad "clipping", clip the loop to save cpu */
    for(i = 0; i < 8; i++) {
        for(j = 0; j < 8; j++) {
            sx = j + x; sy = i + y;

            if (sx >= 0 && sx < w && sy >= 0 && sy < h) {
                if (crosshair[j + 8*i]) {
                    if (crosshair[j + 8*i] & 0x1) {
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

inline void 
inline_draw_sv_plane                            (HildonColorChooser *sel, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h)
{
    GtkWidget *widget = GTK_WIDGET (sel);
    unsigned char *buf, *ptr;
    unsigned long rgbx[3] = { 0x00ffffff, 0x00ffffff, 0x00ffffff }, rgbtmp[3];
    signed long rgby[3];
    HildonColorChooserPrivate *priv;
    int i, j;
    int tmp;

    if (w <= 0 || h <= 0) {
        return;
    }

    priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);
    g_assert (priv);
    tmp = priv->spa.width * priv->spa.height;

    buf = (unsigned char *) g_malloc (w * h * 3);
    ptr = buf;

    inline_h2rgb (priv->currhue, rgbtmp);

    rgby[0] = rgbtmp[0] - rgbx[0];
    rgby[1] = rgbtmp[1] - rgbx[1];
    rgby[2] = rgbtmp[2] - rgbx[2];

    rgbx[0] /= priv->spa.width;
    rgbx[1] /= priv->spa.width;
    rgbx[2] /= priv->spa.width;

    rgby[0] /= tmp;
    rgby[1] /= tmp;
    rgby[2] /= tmp;

    rgbx[0] += (y - priv->spa.y) * rgby[0];
    rgbx[1] += (y - priv->spa.y) * rgby[1];
    rgbx[2] += (y - priv->spa.y) * rgby[2];

    for(i = 0; i < h; i++) {
        rgbtmp[0] = rgbx[0] * (x - priv->spa.x);
        rgbtmp[1] = rgbx[1] * (x - priv->spa.x);
        rgbtmp[2] = rgbx[2] * (x - priv->spa.x);

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

    inline_draw_crosshair (buf, 
            (priv->spa.width * priv->currval / 0xffff) - x + priv->spa.x - 4, 
            (priv->spa.height * priv->currsat / 0xffff) - y + priv->spa.y - 4, 
            w, h);

    gdk_draw_rgb_image (widget->parent->window, widget->style->fg_gc[0], x, y, w, h, GDK_RGB_DITHER_NONE, buf, w * 3);
    g_free(buf);
}

inline void 
inline_draw_sv_plane_dimmed                     (HildonColorChooser *sel, 
                                                 int x, 
                                                 int y, 
                                                 int w, 
                                                 int h)
{
    GtkWidget *widget = GTK_WIDGET (sel);
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);

    g_assert (priv);

    if (w <= 0 || h <= 0) {
        return;
    }

    /* We need to create (and cache) the pixbuf if we don't 
     * have it yet */
    if (priv->dimmed_plane == NULL) {
        unsigned char *buf, *ptr;
        unsigned long rgbx[3] = { 0x00ffffff, 0x00ffffff, 0x00ffffff }, rgbtmp[3];
        unsigned long avg;
        signed long rgby[3];
        int tmp = priv->spa.width * priv->spa.height, i, j;

        buf = (unsigned char *) g_malloc (w * h * 3);

        ptr = buf;

        /* possibe optimization: as we are drawing grayscale plane, there might
           be some simpler algorithm to do this*/
        rgbtmp[0] = 0x00ffffff;
        rgbtmp[1] = 0x00000000;
        rgbtmp[2] = 0x00000000;

        rgby[0] = rgbtmp[0] - rgbx[0];
        rgby[1] = rgbtmp[1] - rgbx[1];
        rgby[2] = rgbtmp[2] - rgbx[2];

        rgbx[0] /= priv->spa.width;
        rgbx[1] /= priv->spa.width;
        rgbx[2] /= priv->spa.width;

        rgby[0] /= tmp;
        rgby[1] /= tmp;
        rgby[2] /= tmp;

        rgbx[0] += (y - priv->spa.y) * rgby[0];
        rgbx[1] += (y - priv->spa.y) * rgby[1];
        rgbx[2] += (y - priv->spa.y) * rgby[2];

        for(i = 0; i < h; i++) {
            rgbtmp[0] = rgbx[0] * (x - priv->spa.x);
            rgbtmp[1] = rgbx[1] * (x - priv->spa.x);
            rgbtmp[2] = rgbx[2] * (x - priv->spa.x);

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

        priv->dimmed_plane = gdk_pixbuf_new_from_data (buf, GDK_COLORSPACE_RGB, FALSE, 8, w, h, w * 3, (gpointer) g_free, buf);
    }

    gdk_draw_pixbuf (widget->parent->window, widget->style->fg_gc [0], priv->dimmed_plane, 0, 0, x, y, w, h, GDK_RGB_DITHER_NONE, 0, 0);
}


static gboolean 
hildon_color_chooser_expose_timer               (gpointer data)
{
    HildonColorChooser *sel = HILDON_COLOR_CHOOSER (data);
    HildonColorChooserPrivate *priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);

    g_assert (priv);

    if (priv->expose_info.expose_queued) {
        gtk_widget_queue_draw (GTK_WIDGET (data));
    }

    return FALSE;
}

void
hildon_color_chooser_get_color                  (HildonColorChooser *sel, 
                                                 GdkColor *color)
{
    HildonColorChooserPrivate *priv;
    GdkVisual *system_visual = gdk_visual_get_system ();
    unsigned long rgb[3], rgb2[3];

    g_return_if_fail (HILDON_IS_COLOR_CHOOSER (sel));
    g_return_if_fail (color != NULL);

    priv = HILDON_COLOR_CHOOSER_GET_PRIVATE (sel);
    g_assert (priv);

    inline_h2rgb (priv->currhue, rgb);

    rgb2[0] = 0xffffff - rgb[0];
    rgb2[1] = 0xffffff - rgb[1];
    rgb2[2] = 0xffffff - rgb[2];

    color->red   = ((rgb[0] >> 8) + ((rgb2[0] >> 8) * (0xffff - priv->currsat) / 0xffff)) * priv->currval / 0xffff;
    color->green = ((rgb[1] >> 8) + ((rgb2[1] >> 8) * (0xffff - priv->currsat) / 0xffff)) * priv->currval / 0xffff;
    color->blue  = ((rgb[2] >> 8) + ((rgb2[2] >> 8) * (0xffff - priv->currsat) / 0xffff)) * priv->currval / 0xffff;

    color->pixel = ((color->red >> (16 - system_visual->red_prec)) << system_visual->red_shift) |
        ((color->green >> (16 - system_visual->green_prec)) << system_visual->green_shift) |
        ((color->blue >> (16 - system_visual->blue_prec)) << system_visual->blue_shift);
}
