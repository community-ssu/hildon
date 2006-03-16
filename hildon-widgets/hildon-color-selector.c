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

/*
 * @file hildon-color-selector.c
 *
 * This file contains the implementation of Hildon Color Selector
 * widget containing the base color and custom color palette selector 
 * and popup for selecting different colors based on RGB values.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtktable.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkdrawingarea.h>
#include <gtk/gtkbutton.h>
#include <gdk/gdkkeysyms.h>
#include <gconf/gconf-client.h> 

#include "hildon-color-selector.h"
#include "hildon-color-popup.h"

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

/* Color amounts */
#define HILDON_BASE_COLOR_NUM             16
#define HILDON_CUSTOM_COLOR_NUM            8
#define HILDON_TOTAL_COLOR_NUM (HILDON_BASE_COLOR_NUM + HILDON_CUSTOM_COLOR_NUM)
#define BLACKIND                           0
#define GREYIND                            6
#define WHITEIND                           9

/* Pixel sizes */
#define HILDON_COLOR_SELECTOR_BOX_W       27
#define HILDON_COLOR_SELECTOR_BOX_H       27
#define HILDON_COLOR_SELECTOR_BOX_BORDER   2
#define HILDON_COLOR_SELECTOR_COLS         8
#define HILDON_COLOR_SELECTOR_ROWS         3
#define HILDON_COLOR_PALETTE_SIZE        120
#define HILDON_COLOR_CONTROLBAR_MAX       31
#define HILDON_COLOR_CONTROLBAR_MIN        0 
#define HILDON_COLOR_LABELS_LEFT_PAD      35
#define HILDON_COLOR_PALETTE_POS_PAD      45
#define HILDON_COLOR_BAR_WIDTH           449

/* gconf definitions */
#define HILDON_COLOR_GCONF_PATH  "/system/osso/af/color_selector"
#define HILDON_COLOR_GCONF_KEYS  "/system/osso/af/color_selector/custom_colors"
/*
 * Pointer parent class 
 */
static GtkDialogClass *parent_class;

struct _HildonColorSelectorPriv 
{
    GConfClient *client;
    GtkWidget *drawing_area;
    GtkWidget *modify_button;
    gint index;
    guint notify_id;

    /* one extra place for the modified base color */
    GdkColor color[HILDON_TOTAL_COLOR_NUM + 1];
};

enum 
{
  PROP_NONE,
  PROP_COLOR
};

/* 
 * Private function prototype definitions 
 */
static void
hildon_color_selector_class_init (HildonColorSelectorClass * selector_class);

static void 
hildon_color_selector_init (HildonColorSelector * selector);

static gboolean
hildon_color_selector_expose  (GtkWidget      * widget,
                               GdkEventExpose * event, 
                               gpointer         data);

static gboolean key_pressed   (GtkWidget      * widget,
                               GdkEventKey    * event);

static gboolean color_pressed (GtkWidget      * widget,
                               GdkEventButton * event,
                               gpointer         user_data);

static void     select_color  (HildonColorSelector * selector,
                               int      event_x,
                               int      event_y,
                               gboolean motion);

static gboolean color_moved   (GtkWidget      * widget,
                               GdkEventMotion * event,
                               gpointer         data);

static void
modify_button_clicked         (GtkWidget           * button,
                               HildonColorSelector * selector);

static void modify_selected(HildonColorSelector * colselector);

static void
hildon_color_selector_set_property(GObject      *object,
                                   guint         param_id,
                                   const GValue *value,
                                   GParamSpec   *pspec);
static void
hildon_color_selector_get_property(GObject    *object,
                                   guint       param_id,
                                   GValue     *value,
                                   GParamSpec *pspec);

GType
hildon_color_selector_get_type(void)
{
    static GType selector_type = 0;

    if (!selector_type) 
    {
        static const GTypeInfo selector_info = 
            {
                sizeof(HildonColorSelectorClass),
                NULL,       /* base_init      */
                NULL,       /* base_finalize  */
                (GClassInitFunc) hildon_color_selector_class_init,
                NULL,       /* class_finalize */
                NULL,       /* class_data     */
                sizeof(HildonColorSelector),
                0,          /* n_preallocs    */
                (GInstanceInitFunc) hildon_color_selector_init,
            };

        selector_type = g_type_register_static(GTK_TYPE_DIALOG,
                                               "HildonColorSelector",
                                               &selector_info, 0);
    }
    return selector_type;
}

static void
hildon_color_selector_destroy(GtkObject *obj)
{
  HildonColorSelectorPriv *priv = HILDON_COLOR_SELECTOR(obj)->priv;

  if (priv->client)
  {
    gconf_client_notify_remove(priv->client, priv->notify_id);
    g_object_unref(priv->client);
    priv->client = NULL;
  }

  GTK_OBJECT_CLASS(parent_class)->destroy(obj);
}

static void
hildon_color_selector_class_init(HildonColorSelectorClass * selector_class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(selector_class);
    GObjectClass *gobject_class = G_OBJECT_CLASS (selector_class);

    parent_class = g_type_class_peek_parent(selector_class);

    widget_class->key_press_event = key_pressed;

    g_type_class_add_private(selector_class,
                             sizeof(HildonColorSelectorPriv));

    GTK_OBJECT_CLASS(selector_class)->destroy = hildon_color_selector_destroy;
  
    gobject_class->get_property = hildon_color_selector_get_property;
    gobject_class->set_property = hildon_color_selector_set_property;
  
    /**
     * HildonColorSelector:color:
     *
     * The selected color.
     */
    g_object_class_install_property (gobject_class, PROP_COLOR,
	    g_param_spec_boxed ("color",
                "Current Color",
                "The selected color",
                GDK_TYPE_COLOR,
                G_PARAM_READWRITE));
}


/**
 * hildon_color_selector_new:
 * @parent:  The parent window. The X window ID of the parent window
 *           has to be the same as the X window ID of the application.
 * @returns: new #HildonColorSelector.
 *
 * Creates a new #HildonColorSelector dialog with 3x8 layout of 
 * Windows base colors and 'OK', 'More..' and 'Cancel' buttons.
 **/
GtkWidget *hildon_color_selector_new(GtkWindow * parent)
{
    GtkWidget *dialog = g_object_new(HILDON_TYPE_COLOR_SELECTOR, NULL);

    g_return_val_if_fail(dialog, NULL);

    if (parent) 
    {
        gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    }

    return dialog;
}

static void
hildon_color_selector_set_custom_colors(
  HildonColorSelector *selector,
  GConfValue *value)
{
  GSList *list;
  int i;

  g_assert(HILDON_IS_COLOR_SELECTOR(selector));

  /* We have to be really careful. At least gconftool's
     stress test may generate unexpected value setups */
  if (value == NULL
      || value->type != GCONF_VALUE_LIST
      || gconf_value_get_list_type(value) != GCONF_VALUE_STRING)
    list = NULL;
  else
    list = gconf_value_get_list(value);

  /* Use list to fill in the selector's color property */
  for ( i = 0; i < HILDON_CUSTOM_COLOR_NUM; ++i)
  {
    const gchar *color_string = NULL;

    if (list) {
      color_string = gconf_value_get_string(list->data);
      list = list->next;
    } else {
      color_string = "#FFFFFF";
    }

/*    g_print("custom_color: %s\n", color_string);  */

    selector->priv->color[i].pixel = 0;
    gdk_color_parse (color_string, 
       &(selector->priv->color[HILDON_BASE_COLOR_NUM+i]));
  }
}

static void
gconf_notify_func(GConfClient *client,
                             guint cnxn_id,
                             GConfEntry *entry,
                             gpointer data)
{
  hildon_color_selector_set_custom_colors(HILDON_COLOR_SELECTOR(data), 
    gconf_entry_get_value(entry));
  gtk_widget_queue_draw(GTK_WIDGET(data));
}

static void 
hildon_color_selector_init(HildonColorSelector * selector)
{
    guint i;
    GtkWidget *hbox;
    GConfValue *value;

    /* 16 standard Windows colors */
    static char *base_colours[HILDON_BASE_COLOR_NUM] = {
        "#000000", "#FFFFFF", "#FF0000", "#660000", "#0000FF", "#000066",
        "#FF33FF", "#660066", "#33CC33", "#006600", "#FFCC00", "#CC9900",
        "#999999", "#666666", "#00CCCC", "#006666"
    };

    selector->priv =
        G_TYPE_INSTANCE_GET_PRIVATE(selector,
                                    HILDON_TYPE_COLOR_SELECTOR,
                                    HildonColorSelectorPriv);
    
    /*  ***********test GConf***********   */
    selector->priv->client = gconf_client_get_default ();
    gconf_client_set_error_handling (selector->priv->client, 
      GCONF_CLIENT_HANDLE_UNRETURNED);
    
    /* Add our directory to the list of directories the GConfClient will
       watch. */
    gconf_client_add_dir (selector->priv->client, HILDON_COLOR_GCONF_PATH,
                              GCONF_CLIENT_PRELOAD_NONE,
                              NULL);
    
    /* Use the value directed by GConfValue to set the color */
    value = gconf_client_get(selector->priv->client, 
      HILDON_COLOR_GCONF_KEYS, NULL);    

    hildon_color_selector_set_custom_colors(selector, value);    

    if (value) {
      gconf_value_free(value);
    }
    
    /* Listen to changes to our key. */
    selector->priv->notify_id = gconf_client_notify_add (selector->priv->client, 
        HILDON_COLOR_GCONF_KEYS, gconf_notify_func, selector, NULL, NULL); 

    /* ************************************  */

    selector->priv->index = GREYIND;
    
    /* init base colors for color boxes */
    for (i = 0; i < HILDON_BASE_COLOR_NUM; ++i) 
    {
        selector->priv->color[i].pixel = 0;
        gdk_color_parse (base_colours[i], &(selector->priv->color[i]));
    }
    
    gtk_dialog_set_has_separator(GTK_DIALOG(selector), FALSE);

    /* create drawing area */
    hbox = gtk_hbox_new(TRUE, 0);
    selector->priv->drawing_area = gtk_drawing_area_new();
    
    /* receive focus from dialog buttons */
    GTK_WIDGET_SET_FLAGS (selector->priv->drawing_area, GTK_CAN_FOCUS);

    gtk_widget_add_events (selector->priv->drawing_area,
                          GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK);

    /* Arrange size of the drawing area */ 
    gtk_widget_set_size_request (selector->priv->drawing_area,
                                 (HILDON_COLOR_SELECTOR_BOX_W *
                                  HILDON_COLOR_SELECTOR_COLS) +
                                 (HILDON_COLOR_SELECTOR_BOX_BORDER *
                                  2*HILDON_COLOR_SELECTOR_COLS),
                                 (HILDON_COLOR_SELECTOR_BOX_H *
                                  HILDON_COLOR_SELECTOR_ROWS) +
                                 HILDON_COLOR_SELECTOR_BOX_BORDER *
                                 2 * HILDON_COLOR_SELECTOR_ROWS);

    gtk_box_pack_start (GTK_BOX(GTK_DIALOG(selector)->vbox), 
                       hbox, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(hbox), selector->priv->drawing_area,
                       FALSE, FALSE, 0);

    /* Register callback functions for the drawing area */
    g_signal_connect (selector->priv->drawing_area, "expose_event",
                      G_CALLBACK(hildon_color_selector_expose), selector);
    g_signal_connect (selector->priv->drawing_area, "button_press_event", 
                      G_CALLBACK(color_pressed), selector);
    g_signal_connect (selector->priv->drawing_area, "motion-notify-event", 
                      G_CALLBACK(color_moved), selector);

    gtk_dialog_add_button (GTK_DIALOG(selector), 
                           _("ecdg_bd_colour_selector_ok"), GTK_RESPONSE_OK);

    selector->priv->modify_button =
        gtk_button_new_with_label(_("ecdg_bd_colour_selector_modify"));
    gtk_widget_set_sensitive(selector->priv->modify_button, FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(selector)->action_area),
                       selector->priv->modify_button, FALSE, TRUE, 0);

    g_signal_connect(selector->priv->modify_button, "clicked",
                     G_CALLBACK(modify_button_clicked), selector);

    gtk_dialog_add_button (GTK_DIALOG(selector),
                           _("ecdg_bd_colour_selector_cancel"),
                           GTK_RESPONSE_CANCEL);

    gtk_dialog_set_default_response (GTK_DIALOG(selector), GTK_RESPONSE_OK);
    
    gtk_window_set_title ( GTK_WINDOW(selector),  
                          _("ecdg_ti_colour_selector") );
    gtk_widget_show_all (GTK_DIALOG(selector)->vbox);
}

static gboolean 
hildon_color_selector_expose(GtkWidget * widget,
                             GdkEventExpose * event,
                             gpointer data)
{
    HildonColorSelector *selector;
    GdkColor color;
    GdkGC *gc;
    gint x, y;

    g_return_val_if_fail (GTK_IS_WIDGET(widget), FALSE);
    g_return_val_if_fail (event, FALSE);
    g_return_val_if_fail (HILDON_IS_COLOR_SELECTOR(data), FALSE);

    if (!GTK_WIDGET_DRAWABLE(widget))
      return FALSE;

    selector = HILDON_COLOR_SELECTOR(data);
    gc = gdk_gc_new (widget->window); 

    /* draw the color boxes and a focus for one of them */
    for (x = 0; x < HILDON_COLOR_SELECTOR_COLS; ++x) 
    {
        for (y = 0; y < HILDON_COLOR_SELECTOR_ROWS; ++y) 
        {
            color.pixel = color.red = color.green = color.blue = 0;
            gdk_gc_set_rgb_fg_color(gc, &color);

            /* focus around the selected color box */
            if ( (y * HILDON_COLOR_SELECTOR_COLS + x) ==
                 selector->priv->index) 
            {
                /* focus box */
                gdk_draw_rectangle(widget->window, gc, TRUE,
                                   (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
                                    HILDON_COLOR_SELECTOR_BOX_W) * x,
                                   (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
                                    HILDON_COLOR_SELECTOR_BOX_H) * y,
                                   HILDON_COLOR_SELECTOR_BOX_W +
                                   HILDON_COLOR_SELECTOR_BOX_BORDER * 2,
                                   HILDON_COLOR_SELECTOR_BOX_H +
                                   HILDON_COLOR_SELECTOR_BOX_BORDER * 2); 
            }
            
            /* frames on color box */
            gdk_draw_rectangle(widget->window, gc, FALSE,
                               (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
                                HILDON_COLOR_SELECTOR_BOX_W) * x + 1,
                               (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
                                HILDON_COLOR_SELECTOR_BOX_H) * y + 1,
                               HILDON_COLOR_SELECTOR_BOX_W + 1,
                               HILDON_COLOR_SELECTOR_BOX_H + 1);
            
            gdk_gc_set_rgb_fg_color(gc, 
              &(selector->priv->color[y * HILDON_COLOR_SELECTOR_COLS + x]));

            /* color box */
            gdk_draw_rectangle(widget->window, gc,
                               TRUE,    /* filled */
                               HILDON_COLOR_SELECTOR_BOX_BORDER +
                               (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
                                HILDON_COLOR_SELECTOR_BOX_W) * x,
                               HILDON_COLOR_SELECTOR_BOX_BORDER +
                               (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
                                HILDON_COLOR_SELECTOR_BOX_H) * y,
                               HILDON_COLOR_SELECTOR_BOX_W,
                               HILDON_COLOR_SELECTOR_BOX_H);
        }
    }

    g_object_unref(gc);
    return TRUE;
}

/**
 * hildon_color_selector_get_color:
 * @selector: a #HildonColorSelector
 * 
 * Gets the currently selected base color as #GdkColor.
 * 
 * Returns: The currently selected #GdkColor. The returned pointer must 
 * not be freed.
 */
G_CONST_RETURN GdkColor *hildon_color_selector_get_color(HildonColorSelector * selector)
{
    g_return_val_if_fail(HILDON_IS_COLOR_SELECTOR(selector), NULL);    
    return &(selector->priv->color[selector->priv->index]);
}

/**
 * hildon_color_selector_set_color:
 * @selector: #HildonColorSelector
 * @color: #Gdkcolor to set.
 * 
 * Select the color specified. Does nothing if the color does not 
 * exists among the standard colors.
 */
void hildon_color_selector_set_color(HildonColorSelector * selector,
                                     GdkColor * color)
{
    gint i;

    g_return_if_fail(HILDON_IS_COLOR_SELECTOR(selector));
    g_return_if_fail(color);

    /* Select the specified color */
    for (i = 0; 
         i < (HILDON_BASE_COLOR_NUM + HILDON_CUSTOM_COLOR_NUM);
         ++i) 
    {
        if (selector->priv->color[i].red   == color->red &&
            selector->priv->color[i].green == color->green &&
            selector->priv->color[i].blue  == color->blue) 
        {
            selector->priv->index = i;

            /* The modify button is active if the color index is bigger than
             * the number of base colours.
             */
            gtk_widget_set_sensitive(selector->priv->modify_button,
                                     selector->priv->index >= HILDON_BASE_COLOR_NUM);
            gtk_widget_queue_draw(selector->priv->drawing_area);
            break;
        }
    }

    /* Notify the color selector that the color has changed */  
    g_object_notify (G_OBJECT (selector), "color");
}

static gboolean
color_pressed(GtkWidget * widget, GdkEventButton * event,
              gpointer user_data)
{
    select_color(HILDON_COLOR_SELECTOR(user_data), event->x, event->y, FALSE);
    return TRUE;
}

/* Handle key press of right, left, up, down and return */
static gboolean key_pressed(GtkWidget * widget,
                            GdkEventKey * event)
{
    HildonColorSelector *selector;
    gint index;

    g_assert(widget);

    selector = HILDON_COLOR_SELECTOR(widget);
    index = selector->priv->index;

    /* if dialog buttons has the focus */
    if (GTK_WIDGET_HAS_FOCUS(selector->priv->drawing_area) == FALSE)
        return GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);

    /* Since wrapping is not allowed,
     * move only if the next index is a valid one.
     */
    switch (event->keyval) {
    case GDK_KP_Right:
    case GDK_Right:
        if (index == (HILDON_BASE_COLOR_NUM + HILDON_CUSTOM_COLOR_NUM - 1)
            || index == (HILDON_CUSTOM_COLOR_NUM - 1)
            || index == (2*HILDON_CUSTOM_COLOR_NUM - 1)) 
        {
            return TRUE;
        }
        index++;
        break;
    case GDK_KP_Left:
    case GDK_Left:
        if (index == 0 
            || index == (HILDON_CUSTOM_COLOR_NUM)
            || index == (2*HILDON_CUSTOM_COLOR_NUM)) 
        {
            return TRUE;
        }
        index--;
        break;
    case GDK_KP_Up:
    case GDK_Up:
        if (index > (HILDON_COLOR_SELECTOR_COLS - 1)) 
        {
            index -= HILDON_COLOR_SELECTOR_COLS;
        } 
        else 
        {
            return GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);
        }
        break;
    case GDK_KP_Down:
    case GDK_Down:
        if (index < (HILDON_COLOR_SELECTOR_COLS + HILDON_CUSTOM_COLOR_NUM)) 
        {
            index += HILDON_COLOR_SELECTOR_COLS;  
        } 
        else 
        {
            return GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);
        }
        break;
    case GDK_KP_Enter:
    case GDK_Return:
        if (index < HILDON_BASE_COLOR_NUM)
            return GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);

        modify_selected(selector);
    default:
        return GTK_WIDGET_CLASS(parent_class)->key_press_event(widget, event);
    }

    if (index < (HILDON_BASE_COLOR_NUM + HILDON_CUSTOM_COLOR_NUM)) 
    {
        selector->priv->index = index;
    } 
    else 
    {
        selector->priv->index = 
            HILDON_BASE_COLOR_NUM + HILDON_CUSTOM_COLOR_NUM - 1;
    }
    /* The modify button is active if the color index is bigger than
     * the number of base colours.
     */
    gtk_widget_set_sensitive(selector->priv->modify_button,
                             selector->priv->index >= HILDON_BASE_COLOR_NUM);

    gtk_widget_queue_draw(selector->priv->drawing_area);
    
    return TRUE;
}

static void
select_color(HildonColorSelector * selector, int event_x, int event_y,
             gboolean motion)
{
    gint x, y;

    g_assert(HILDON_IS_COLOR_SELECTOR(selector));

    /* Get the selection coordinates */ 
    x = ( (event_x - HILDON_COLOR_SELECTOR_BOX_BORDER) /
          (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
           HILDON_COLOR_SELECTOR_BOX_W)
        );
    y = ( (event_y -
           HILDON_COLOR_SELECTOR_BOX_BORDER) /
          (HILDON_COLOR_SELECTOR_BOX_BORDER * 2 +
           HILDON_COLOR_SELECTOR_BOX_H)
        );

    /* Get the row and column numbers for the selected color */ 
    if (x > (HILDON_COLOR_SELECTOR_COLS + HILDON_CUSTOM_COLOR_NUM - 1)) 
    {
        x = HILDON_COLOR_SELECTOR_COLS + HILDON_CUSTOM_COLOR_NUM - 1;
    } 
    else if (x < 0) 
    {
        x = 0;
    }
    if (y > (HILDON_COLOR_SELECTOR_ROWS - 1)) 
    {
        y = HILDON_COLOR_SELECTOR_ROWS - 1;
    } 
    else if (y < 0) 
    {
        y = 0;
    }

    /* If a custom colour is selected, open a popup to modify the colour */ 
    if (!motion &&
        selector->priv->index >= HILDON_BASE_COLOR_NUM &&
        selector->priv->index == (x + y * HILDON_COLOR_SELECTOR_COLS))
        modify_selected(selector);

    selector->priv->index = (x + y * HILDON_COLOR_SELECTOR_COLS);
    gtk_widget_set_sensitive(selector->priv->modify_button,
                             selector->priv->index >= HILDON_BASE_COLOR_NUM);
    
    gtk_widget_queue_draw(selector->priv->drawing_area);
}

static gboolean
color_moved(GtkWidget * widget, GdkEventMotion * event, gpointer data)
{
    if ( event->state &
         (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK | GDK_BUTTON3_MASK) ) 
    {
        select_color(HILDON_COLOR_SELECTOR(data), event->x, event->y, TRUE);
        return TRUE;
    }
    return FALSE;
}

static void
modify_button_clicked(GtkWidget * button, HildonColorSelector * selector)
{
    modify_selected (selector);
}

static void
modify_selected(HildonColorSelector * colselector)
{
    HildonColorPopup popupdata;
    GtkWidget *popup;
    
    /* Create a HildonColorPopup dialog */     
    popup = hildon_color_popup_new(GTK_WINDOW(colselector), 
		    hildon_color_selector_get_color(colselector), &popupdata);
    
    if ( gtk_dialog_run(GTK_DIALOG(popup) ) == GTK_RESPONSE_OK) 
    {        
      GdkColor *color;

      /* We cannot modify a base color */
      if (colselector->priv->index < HILDON_BASE_COLOR_NUM)
      {
        colselector->priv->color[HILDON_TOTAL_COLOR_NUM] = 
          colselector->priv->color[colselector->priv->index];
        colselector->priv->index = HILDON_TOTAL_COLOR_NUM;
      }

      color = hildon_color_selector_get_color(colselector);

      /* Get the current color value */
      hildon_color_popup_set_color_from_sliders(color, &popupdata);

      /* If we modified a base color we just accept the dialog */      
      if( colselector->priv->index >=  HILDON_TOTAL_COLOR_NUM)
      {          
        gtk_dialog_response(GTK_DIALOG(colselector), GTK_RESPONSE_OK); 
      }
      else /* If we mofied custom colors we have to save to gconf */
      {
        GConfValue *value;
        GSList * list;
        int i;  

        value = gconf_value_new(GCONF_VALUE_LIST);
        gconf_value_set_list_type(value, GCONF_VALUE_STRING);
        list = NULL;

        for ( i = HILDON_BASE_COLOR_NUM; i < HILDON_TOTAL_COLOR_NUM; i++) 
        {
            GConfValue *item;
            char buffer[32];
                  
            g_snprintf(buffer, sizeof(buffer), "#%.2X%.2X%.2X",
                            (colselector->priv->color[i].red>>8)&0xFF,
                            (colselector->priv->color[i].green>>8)&0xFF,
                            (colselector->priv->color[i].blue>>8)&0xFF ); 
  
            item = gconf_value_new(GCONF_VALUE_STRING);
            gconf_value_set_string(item, buffer);
            list = g_slist_append (list, item);
        } 
        
        gconf_value_set_list_nocopy(value, list);

        /* gconf client handles the possible error */
        gconf_client_set(colselector->priv->client, 
          HILDON_COLOR_GCONF_KEYS, value, NULL);
    
        gconf_value_free(value);
      }  
    }

    gtk_widget_destroy (popup); 
    gtk_window_present (GTK_WINDOW(colselector));
}

static void
hildon_color_selector_set_property(GObject *object, guint param_id,
			                           const GValue *value, GParamSpec *pspec)
{
  HildonColorSelector *selector = HILDON_COLOR_SELECTOR(object);

  switch (param_id) 
    {
    case PROP_COLOR:
      hildon_color_selector_set_color(selector, (GdkColor*)g_value_get_boxed(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
    }
}

static void
hildon_color_selector_get_property(GObject *object, guint param_id,
			                           GValue *value, GParamSpec *pspec)
{
  HildonColorSelector *selector = HILDON_COLOR_SELECTOR(object);

  switch (param_id) 
    {
    case PROP_COLOR:
      g_value_set_boxed(value, (gconstpointer)hildon_color_selector_get_color(selector));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
    }
}

