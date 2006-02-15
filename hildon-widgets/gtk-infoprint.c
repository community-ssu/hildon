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

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/X.h>
#include <X11/Xatom.h>
#include "gtk-infoprint.h"
#include "hildon-defines.h"
#include "hildon-app.h"

#define INFOPRINT_MIN_WIDTH    39
#define INFOPRINT_MAX_WIDTH   334
#define BANNER_MIN_WIDTH       35
#define BANNER_MAX_WIDTH      375
#define DEFAULT_WIDTH          20
#define DEFAULT_HEIGHT         28       /* 44-8-8 = 28 */
#define WINDOW_WIDTH          600
#define MESSAGE_TIMEOUT      2500
#define GTK_INFOPRINT_STOCK "gtk-infoprint"
#define GTK_INFOPRINT_ICON_THEME  "qgn_note_infoprint"

#define BANNER_PROGRESSBAR_MIN_WIDTH 83
#define INFOPRINT_MIN_SIZE           50
#define INFOPRINT_WINDOW_Y           73
#define INFOPRINT_WINDOW_FS_Y        20
#define INFOPRINT_WINDOW_X           15


#define DEFAULT_PROGRESS_ANIMATION "qgn_indi_pball_a"

static gboolean gtk_infoprint_temporal_wrap_disable_flag = FALSE;

typedef struct {
  GtkWindow *parent;
  GtkWidget *window;
  gchar *text;
  GtkWidget *main_item;
  guint timeout;
} InfoprintState;

enum {
    WIN_TYPE = 0,
    WIN_TYPE_MESSAGE,
    MAX_WIN_MESSAGES
};

static GtkWidget *global_banner = NULL;
static GtkWidget *global_infoprint = NULL;

static GQueue *cbanner_queue = NULL;
static InfoprintState *current_ibanner = NULL;
static InfoprintState *current_pbanner = NULL;
static guint pbanner_refs = 0;

static gboolean compare_icons(GtkImage *image1, GtkImage *image2);
static void queue_new_cbanner(GtkWindow *parent,
                              const gchar *text,
                              GtkWidget *image);
static void gtk_msg_window_init(GtkWindow * parent, GQuark type,
                                const gchar * text, GtkWidget * main_item);
static gchar *three_lines_truncate(GtkWindow * parent,
                                   const gchar * str,
                                   gint * max_width, gint * resulting_lines);

static gboolean infoprint_idle_before_timer (GtkWidget *widget, 
                                             GdkEventExpose *event, 
                                             gpointer data);
static gboolean infoprint_start_timer (gpointer data);

/* Getters/initializers for needed quarks */
static GQuark banner_quark(void)
{
    static GQuark quark = 0;

    if (quark == 0)
        quark = g_quark_from_static_string("banner");

    return quark;
}

static GQuark infoprint_quark(void)
{
    static GQuark quark = 0;

    if (quark == 0)
        quark = g_quark_from_static_string("infoprint");

    return quark;
}

static GQuark type_quark(void)
{
    static GQuark quark = 0;

    if (quark == 0)
        quark = g_quark_from_static_string("Message window type");

    return quark;
}

static GQuark label_quark(void)
{
    static GQuark quark = 0;

    if (quark == 0)
        quark = g_quark_from_static_string("Message window text");

    return quark;
}

static GQuark item_quark(void)
{
    static GQuark quark = 0;

    if (quark == 0)
        quark =
            g_quark_from_static_string("Message window graphical item");

    return quark;
}

static GQuark confirmation_banner_quark(void)
{
    static GQuark quark = 0;

    if (quark == 0)
        quark = g_quark_from_static_string("confirmation_banner");

    return quark;
}

/* Returns the infoprint/banner linked to the specific window */
static GtkWindow *gtk_msg_window_get(GtkWindow * parent, GQuark quark)
{
    if (quark == 0)
        return NULL;
    if (parent == NULL) {
        if (quark == banner_quark()) {
            return GTK_WINDOW(global_banner);
        }
        return GTK_WINDOW(global_infoprint);
    }

    return GTK_WINDOW(g_object_get_qdata(G_OBJECT(parent), quark));
}

/* Returns the given widget from banner type of message window. 
   This is used when the banner data is updated in later stage. 
*/
static GtkWidget *gtk_banner_get_widget(GtkWindow * parent,
                                        GQuark widget_quark)
{
    GtkWindow *window = gtk_msg_window_get(parent, banner_quark());

    if (window)
        return
            GTK_WIDGET(g_object_get_qdata(G_OBJECT(window), widget_quark));

    return NULL;
}

/* Timed destroy. Removing this callback is done in other place. */
static gboolean gtk_msg_window_destroy(gpointer pointer)
{
    g_return_val_if_fail(GTK_IS_WINDOW(pointer), TRUE);
    gtk_widget_destroy(GTK_WIDGET(pointer));
    
    return FALSE;
}

static gboolean gtk_msg_window_real_destroy(gpointer pointer)
{
    GObject *parent;
    GQuark quark;

    g_return_val_if_fail(GTK_IS_WINDOW(pointer), TRUE);

    parent = G_OBJECT(gtk_window_get_transient_for(GTK_WINDOW(pointer)));
    quark = (GQuark) g_object_get_qdata((GObject *) pointer, type_quark());

    if (quark == infoprint_quark() && current_ibanner) {
      gtk_widget_unref(current_ibanner->main_item);
      g_free(current_ibanner->text);
      g_free(current_ibanner);
      current_ibanner = NULL;
    } else if (quark == banner_quark() && current_pbanner) {
      /*
       * If the destroy signal is not emited via gkt_banner_close() 
       * (for example when the banner is being destroyed with the parent)
       * the reference counter will have a value larger than 0 and the
       * reference counter should be decremented.
       */
      
      if (pbanner_refs > 0) 
        --pbanner_refs;
      
      gtk_widget_unref(current_pbanner->main_item);
      g_free(current_pbanner->text);
      g_free(current_pbanner);
      current_pbanner = NULL;
    } else if (quark == confirmation_banner_quark() &&
               !g_queue_is_empty(cbanner_queue)) {
      InfoprintState *cbanner = g_queue_pop_head(cbanner_queue);

      gtk_widget_unref(cbanner->main_item);
      g_free(cbanner->text);
      g_free(cbanner);

      if (!g_queue_is_empty(cbanner_queue)) {
        cbanner = g_queue_peek_head(cbanner_queue);
        gtk_msg_window_init(cbanner->parent, confirmation_banner_quark(),
                            cbanner->text, cbanner->main_item);
      } else {
        g_queue_free(cbanner_queue);
        cbanner_queue = NULL;
      }
    }

    if (parent) {
        g_object_set_qdata(parent, quark, NULL);
    } else {
        if (quark == banner_quark()) {
            global_banner = NULL;
        } else {
            global_infoprint = NULL;
        }
    }

    return FALSE;
}


/* Get window ID of top window from _NET_ACTIVE_WINDOW property */
static Window get_active_window( void )
{
    unsigned long n;
    unsigned long extra;
    int format;
    int status;

    Atom atom_net_active = gdk_x11_get_xatom_by_name ("_NET_ACTIVE_WINDOW");
    Atom realType;
    Window win_result = None;
    guchar *data_return = NULL;

    status = XGetWindowProperty(GDK_DISPLAY(), GDK_ROOT_WINDOW(), 
                                atom_net_active, 0L, 16L,
                                0, XA_WINDOW, &realType, &format,
                                &n, &extra, 
                                &data_return);

    if ( status == Success && realType == XA_WINDOW 
            && format == 32 && n == 1 && data_return != NULL )
    {
        win_result = ((Window*) data_return)[0];
    /*    g_print("_NET_ACTIVE_WINDOW id %d\n", ((gint *)data_return)[0] );*/
    } 
        
    if ( data_return ) 
        XFree(data_return);    
    
    return win_result;
}

/* Checks if a window is in fullscreen state or not */
static gboolean check_fullscreen_state( Window window )
{
    unsigned long n;
    unsigned long extra;
    int           format, status, i; 
    guchar *data_return = NULL;
    
    Atom          realType;
    Atom  atom_window_state = gdk_x11_get_xatom_by_name ("_NET_WM_STATE");
    Atom  atom_fullscreen = gdk_x11_get_xatom_by_name ("_NET_WM_STATE_FULLSCREEN");
    
    if ( window == None )
        return FALSE;

    /* in some cases XGetWindowProperty seems to generate BadWindow,
       so at the moment this function does not always work perfectly */
    gdk_error_trap_push();
    status = XGetWindowProperty(GDK_DISPLAY(), window,
                                atom_window_state, 0L, 1000000L,
                                0, XA_ATOM, &realType, &format,
                                &n, &extra, &data_return);
    gdk_flush();
    if (gdk_error_trap_pop())
        return FALSE;

    if (status == Success && realType == XA_ATOM && format == 32 && n > 0)
    {
        for(i=0; i < n; i++)
            if ( ((Atom*)data_return)[i] && 
                 ((Atom*)data_return)[i] == atom_fullscreen)
            {
                if (data_return) XFree(data_return);
                return True;
            }
    }

    if (data_return) 
        XFree(data_return);

    return False;
}


static gboolean
compare_icons(GtkImage *image1, GtkImage *image2)
{
  GtkImageType type = gtk_image_get_storage_type(image1);
  gchar *name1, *name2;
  const gchar *icon_name1, *icon_name2;
  GtkIconSize size1, size2;

  if (gtk_image_get_storage_type(image2) != type)
    return FALSE;

  switch (type) {
  case GTK_IMAGE_STOCK:
    gtk_image_get_stock(image1, &name1, &size1);
    gtk_image_get_stock(image2, &name2, &size2);
    return ((g_utf8_collate(name1, name2) == 0) && (size1 == size2));
  case GTK_IMAGE_ICON_NAME:
    gtk_image_get_icon_name(image1, &icon_name1, &size1);
    gtk_image_get_icon_name(image2, &icon_name2, &size2);
    return ((g_utf8_collate(icon_name1, icon_name2) == 0) && (size1 == size2));
  case GTK_IMAGE_ANIMATION:
    /* there is only one possible animation */
    return TRUE;
  default:
    /* other types of icons are actually not even supported */
    return FALSE;
  }
}

/* confirmation banners are queued so that all of them will
   be shown eventually for the appropriate amount of time */
static void
queue_new_cbanner(GtkWindow *parent, const gchar *text, GtkWidget *image)
{
  InfoprintState *cbanner;

  if (cbanner_queue == NULL)
    cbanner_queue = g_queue_new();

  /* identical consecutive cbanners are collapsed to just one cbanner */
  if ((cbanner = g_queue_peek_tail(cbanner_queue)) != NULL &&
      g_utf8_collate(cbanner->text, text) == 0 &&
      compare_icons(GTK_IMAGE(image), GTK_IMAGE(cbanner->main_item))) {
    g_source_remove(cbanner->timeout);

    cbanner->timeout = g_timeout_add(MESSAGE_TIMEOUT,
                                     gtk_msg_window_destroy,
                                     cbanner->window);
    g_signal_connect_swapped(cbanner->window, "destroy",
                             G_CALLBACK(g_source_remove),
                             GUINT_TO_POINTER(cbanner->timeout));
    gtk_object_sink(GTK_OBJECT(image));
    return;
  }

  cbanner = g_new0(InfoprintState, 1);
  cbanner->parent = parent;
  cbanner->text = g_strdup(text);
  cbanner->main_item = image;
  gtk_widget_ref(cbanner->main_item);

  g_queue_push_tail(cbanner_queue, cbanner);

  if (g_queue_get_length(cbanner_queue) == 1)
    gtk_msg_window_init(parent, confirmation_banner_quark(), text, image);
}


/* gtk_msg_window_init
 *
 * @parent -- The parent window
 * @type   -- The enumerated type of message window
 * @text   -- The displayed text
 * @item   -- The item to be loaded, or NULL if default 
 *             (used only in INFOPRINT_WITH_ICON)
 */
static void
gtk_msg_window_init(GtkWindow * parent, GQuark type,
                    const gchar * text, GtkWidget * main_item)
{
    GtkWidget *window;
    GtkWidget *hbox;
    GtkWidget *label;

    gchar *str = NULL;
    gint max_width = 0;

    g_return_if_fail((GTK_IS_WINDOW(parent) || parent == NULL));

    if (type == banner_quark())
      pbanner_refs++;

    /* information banners: just reset the timeout if trying
       to recreate the currently visible information banner */
    if (type == infoprint_quark() && current_ibanner) {
     
      /* caller is trying to recreate current information banner */
      if (g_utf8_collate(current_ibanner->text, text) == 0 &&
          compare_icons(GTK_IMAGE(main_item),
                        GTK_IMAGE(current_ibanner->main_item))) {
        /* If previous timer has been created, replace it with a new one */
        if (current_ibanner->timeout > 0) {
          g_source_remove(current_ibanner->timeout);
          current_ibanner->timeout = g_timeout_add(MESSAGE_TIMEOUT,
                                                   gtk_msg_window_destroy,
                                                   current_ibanner->window);

          g_signal_connect_swapped(current_ibanner->window, "destroy",
                                   G_CALLBACK(g_source_remove),
                                   GUINT_TO_POINTER(current_ibanner->timeout));
        }
        gtk_object_sink(GTK_OBJECT(main_item));
        return;
      }
     
      /* If the timer has already been set -> remove it */
      if (current_ibanner->timeout > 0)
        g_source_remove(current_ibanner->timeout);

      gtk_msg_window_destroy(current_ibanner->window);
    }

    if (type == banner_quark() && current_pbanner) {
      if (g_utf8_collate(current_pbanner->text, text) == 0) {
        if (GTK_IS_PROGRESS_BAR(main_item) &&
            GTK_IS_PROGRESS_BAR(current_pbanner->main_item)) {
          gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(current_pbanner->main_item), 0.0);
          gtk_object_sink(GTK_OBJECT(main_item));
          return;
        } else if (GTK_IS_IMAGE(main_item) &&
                   GTK_IS_IMAGE(current_pbanner->main_item) &&
                   compare_icons(GTK_IMAGE(main_item),
                                 GTK_IMAGE(current_pbanner->main_item))) {
          gtk_object_sink(GTK_OBJECT(main_item));
          return;
        }
      }
    }

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_accept_focus(GTK_WINDOW(window), FALSE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_msg_window_real_destroy), window);

    hbox = gtk_hbox_new(FALSE, 5);

    if (current_pbanner && type == banner_quark()) {
      /*
       * The destroy substracts one from the reference counter,
       * so adding one to keep the counter working
       */
      ++pbanner_refs;
      gtk_msg_window_destroy(current_pbanner->window);
    }

    if (parent) {
        gtk_window_set_transient_for(GTK_WINDOW(window), parent);
        gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);

        g_object_set_qdata(G_OBJECT(parent), type, (gpointer) window);
    } else {
        if (type == banner_quark()) {
            global_banner = window;
        } else {
            global_infoprint = window;
        }
    }

    gtk_widget_realize(window);

    if ((type == confirmation_banner_quark()) || (type == banner_quark()))
        gtk_infoprint_temporal_wrap_disable_flag = TRUE;

    if (type == banner_quark()) {
        max_width = BANNER_MAX_WIDTH;
    } else {
        max_width = INFOPRINT_MAX_WIDTH;
    }

    if (parent == NULL) {
      gdk_window_set_transient_for(GDK_WINDOW(window->window),
                                   GDK_WINDOW(gdk_get_default_root_window()));
      str = three_lines_truncate(GTK_WINDOW(window), text, &max_width, NULL);
    } else {
      str = three_lines_truncate(parent, text, &max_width, NULL);
    }

    gtk_infoprint_temporal_wrap_disable_flag = FALSE;

    label = gtk_label_new(str);
    g_free(str);

    if (max_width < INFOPRINT_MIN_WIDTH) {
        gtk_widget_set_size_request(GTK_WIDGET(label),
                                    (max_width < INFOPRINT_MIN_WIDTH) ?
                                    INFOPRINT_MIN_WIDTH : -1,
                                    -1);
    }

    if ((type == confirmation_banner_quark()) || (type == banner_quark()))
        gtk_widget_set_name(label, "hildon-banner-label");

    g_object_set_qdata(G_OBJECT(window), type_quark(), (gpointer) type);
    g_object_set_qdata(G_OBJECT(window), label_quark(), (gpointer) label);
    g_object_set_qdata(G_OBJECT(window), item_quark(),
                       (gpointer) main_item);

    gtk_container_add(GTK_CONTAINER(window), hbox);

    if (type == banner_quark()) {
        gtk_box_pack_start_defaults(GTK_BOX(hbox), label);
        if (main_item) {
            if (GTK_IS_PROGRESS_BAR(main_item)) {
                gtk_widget_set_size_request(GTK_WIDGET(main_item),
                                            BANNER_PROGRESSBAR_MIN_WIDTH,
                                            -1);
            }
            gtk_box_pack_start_defaults(GTK_BOX(hbox), main_item);
        }
    } else {
        if (main_item) {
            GtkAlignment *ali =
                GTK_ALIGNMENT(gtk_alignment_new(0, 0, 0, 0));
            gtk_widget_set_size_request(GTK_WIDGET(main_item),
                                        INFOPRINT_MIN_SIZE,
                                        INFOPRINT_MIN_SIZE);
            gtk_container_add(GTK_CONTAINER(ali), GTK_WIDGET(main_item));
            gtk_box_pack_start_defaults(GTK_BOX(hbox), GTK_WIDGET(ali));
        }
        gtk_box_pack_start_defaults(GTK_BOX(hbox), label);
    }

    gtk_window_set_default_size(GTK_WINDOW(window),
                                DEFAULT_WIDTH, DEFAULT_HEIGHT);

    /* Positioning of the infoprint */ 
    { 
        gint y = INFOPRINT_WINDOW_Y;
        gint x = gdk_screen_width() + INFOPRINT_WINDOW_X;
        
        /* Check if the active application is in fullscreen */
        if( check_fullscreen_state(get_active_window()) )
            y =  INFOPRINT_WINDOW_FS_Y;  
            /* this should be fixed to use theme dependant infoprint border size */

        gtk_window_move(GTK_WINDOW(window), x, y);
    }

    gdk_window_set_type_hint(window->window, GDK_WINDOW_TYPE_HINT_MESSAGE);

    gtk_widget_show_all(window);

    if (type == infoprint_quark()) {
      current_ibanner = g_new0(InfoprintState, 1);
      current_ibanner->parent = parent;
      current_ibanner->window = window;
      current_ibanner->text = g_strdup(text);
      current_ibanner->main_item = main_item;
      gtk_widget_ref(current_ibanner->main_item);
    }
    else if (type == banner_quark()) {
      current_pbanner = g_new0(InfoprintState, 1);
      current_pbanner->parent = parent;
      current_pbanner->window = window;
      current_pbanner->text = g_strdup(text);
      current_pbanner->main_item = main_item;
      gtk_widget_ref(current_pbanner->main_item);
    }

    /* If the type is an infoprint we set the timer after the expose-event */
    if (type == infoprint_quark()) {
      g_signal_connect_after(window, "expose-event", 
                             G_CALLBACK(infoprint_idle_before_timer), NULL);
    }
    else if (type == confirmation_banner_quark()) {
      InfoprintState *current_cbanner = g_queue_peek_head(cbanner_queue);

      current_cbanner->window = window;
      current_cbanner->timeout = g_timeout_add(MESSAGE_TIMEOUT,
                                               gtk_msg_window_destroy,
                                               current_cbanner->window);
      g_signal_connect_swapped(window, "destroy",
                               G_CALLBACK(g_source_remove),
                               GUINT_TO_POINTER(current_cbanner->timeout));
    }
}

static gchar *three_lines_truncate(GtkWindow * parent, const gchar * str,
                                   gint * max_width, gint * resulting_lines)
{
    gchar *result = NULL;
    PangoLayout *layout;
    PangoContext *context;

    if (!str)
        return g_strdup("");

    if (GTK_IS_WIDGET(parent)) {
        context = gtk_widget_get_pango_context(GTK_WIDGET(parent));
    } else {
        if (gdk_screen_get_default() != NULL) {
            context = gdk_pango_context_get_for_screen
                (gdk_screen_get_default());
        } else {
            g_print("GtkInfoprint : Could not get default screen.\n");
            return NULL;
        }
    }

    {
        gchar *line1 = NULL;
        gchar *line2 = NULL;
        gchar *line3 = NULL;

        layout = pango_layout_new(context);
        pango_layout_set_text(layout, str, -1);
        if (gtk_infoprint_temporal_wrap_disable_flag == FALSE) {
            pango_layout_set_width(layout, *max_width * PANGO_SCALE);
        } else {
            pango_layout_set_width(layout, -1);
        }
        pango_layout_set_wrap(layout, PANGO_WRAP_WORD_CHAR);

        if (pango_layout_get_line_count(layout) >= 2) {
            PangoLayoutLine *line = pango_layout_get_line(layout, 0);

            if (line != NULL)
                line1 = g_strndup(str, line->length);

            line = pango_layout_get_line(layout, 1);

            if (line != NULL)
                line2 = g_strndup((gchar *) ((gint) str + line->start_index),
                                  line->length);

            line = pango_layout_get_line(layout, 2);

            if (line != NULL)
                line3 = g_strdup((gchar *) ((gint) str + line->start_index));

            g_object_unref(layout);
            layout = pango_layout_new(context);
            pango_layout_set_text(layout, line3 ? line3 : "", -1);

            {
                gint index = 0;

                if (pango_layout_get_line_count(layout) > 1) {
                    gchar *templine = NULL;

                    line = pango_layout_get_line(layout, 0);
                    templine = g_strndup(line3, line->length);
                    g_free(line3);
                    line3 = g_strconcat(templine, "\342\200\246", NULL);
                    g_free(templine);
                }

                if (pango_layout_xy_to_index(layout,
                                             *max_width * PANGO_SCALE,
                                             0, &index, NULL)
                    == TRUE) {
                    gint ellipsiswidth;
                    gchar *tempresult;
                    PangoLayout *ellipsis = pango_layout_new(context);

                    pango_layout_set_text(ellipsis, "\342\200\246", -1);
                    pango_layout_get_size(ellipsis, &ellipsiswidth, NULL);
                    pango_layout_xy_to_index(layout,
                                             *max_width * PANGO_SCALE -
                                             ellipsiswidth,
                                             0, &index, NULL);
                    g_object_unref(G_OBJECT(ellipsis));
                    tempresult = g_strndup(line3, index);
                    g_free(line3);
                    line3 = g_strconcat(tempresult, "\342\200\246", NULL);
                    g_free(tempresult);
                }
            }

        } else
            line1 = g_strdup(str);

        {
            PangoLayout *templayout = pango_layout_new(context);

            pango_layout_set_text(templayout, line1, -1);
            if (pango_layout_get_line_count(templayout) < 3
                && line3 != NULL) {
                result = g_strconcat(line1, "\n", line2, "\n", line3, NULL);
            } else if (pango_layout_get_line_count(templayout) < 2
                       && line2 != NULL) {
                result = g_strconcat(line1, "\n", line2, line3, NULL);
            } else {
                result = g_strconcat(line1, line2, line3, NULL);
            }
            g_object_unref(templayout);
	}

        g_free(line1);
        g_free(line2);
        g_free(line3);

        g_object_unref(layout);

        if (gtk_infoprint_temporal_wrap_disable_flag == TRUE) {
            gint index = 0;
            PangoLayout *templayout = pango_layout_new(context);

            pango_layout_set_text(templayout, result, -1);

            if (pango_layout_get_line_count(templayout) >= 2) {
                PangoLayoutLine *line =
                    pango_layout_get_line(templayout, 0);
                gchar *templine = g_strndup(result, line->length);

                g_free(result);
                result = g_strconcat(templine, "\342\200\246", NULL);
                g_free(templine);
                pango_layout_set_text(templayout, result, -1);
            }

            if (pango_layout_xy_to_index
                (templayout, *max_width * PANGO_SCALE, 0, &index,
                 NULL) == TRUE) {
                gint ellipsiswidth;
                gchar *tempresult;
                PangoLayout *ellipsis = pango_layout_new(context);

                pango_layout_set_text(ellipsis, "\342\200\246", -1);
                pango_layout_get_size(ellipsis, &ellipsiswidth, NULL);
                pango_layout_xy_to_index(templayout,
                                         *max_width * PANGO_SCALE -
                                         ellipsiswidth, 0, &index, NULL);
                g_object_unref(G_OBJECT(ellipsis));
                tempresult = g_strndup(result, index);
                g_free(result);
                result = g_strconcat(tempresult, "\342\200\246", NULL);
                g_free(tempresult);
            }
            g_object_unref(templayout);
        }
    }

    {
        PangoLayout *templayout = pango_layout_new(context);

        pango_layout_set_text(templayout, result, -1);
        pango_layout_get_size(templayout, max_width, NULL);
        if (resulting_lines != NULL)
          *resulting_lines = pango_layout_get_line_count(templayout);
        g_object_unref(templayout);
    }

    if (result == NULL)
        result = g_strdup(str);

    return result;
}

/**************************************************/
/** Public                                       **/
/**************************************************/

/**
 * gtk_infoprint:
 * @parent: The transient window for the infoprint.
 * @text: The text in infoprint
 *
 * Opens a new infoprint with @text content.
 * 
 * If parent is %NULL, the infoprint is a system infoprint.
 * Normally you should use your application window
 * or dialog as a transient parent and avoid passing %NULL.
 */
void gtk_infoprint(GtkWindow * parent, const gchar * text)
{
    gtk_infoprint_with_icon_name(parent, text, NULL);
}

/**
 * gtk_infoprint_with_icon_stock:
 * @parent: The transient window for the infoprint.
 * @text: The text in infoprint
 * @stock_id: The stock id of the custom icon
 *
 * Opens a new infoprint with @text content.
 * With this function you can also set a custom icon
 * by giving a stock id as last parameter.
 * 
 * If parent is %NULL, the infoprint is a system infoprint.
 * Normally you should use your application window
 * or dialog as a transient parent and avoid passing %NULL.
 */
void
gtk_infoprint_with_icon_stock(GtkWindow * parent,
                              const gchar * text, const gchar * stock_id)
{
    GtkWidget *image;

    if (stock_id) {
        image = gtk_image_new_from_stock(stock_id, HILDON_ICON_SIZE_NOTE);
    } else {
        image = gtk_image_new_from_stock(GTK_INFOPRINT_STOCK,
                                         HILDON_ICON_SIZE_NOTE);
    }

    gtk_msg_window_init(parent, infoprint_quark(), text, image);
}

/**
 * gtk_infoprint_with_icon_name:
 * @parent: The transient window for the infoprint.
 * @text: The text in infoprint
 * @icon_name: The name of the icon
 *
 * Opens a new infoprint with @text content.
 * With this function you can also set a custom icon
 * by giving a icon name as last parameter.
 * 
 * If parent is %NULL, the infoprint is a system infoprint.
 * Normally you should use your application window
 * or dialog as a transient parent and avoid passing %NULL.
 */
void
gtk_infoprint_with_icon_name(GtkWindow * parent,
                              const gchar * text, const gchar * icon_name)
{
    GtkWidget *image;

    if (icon_name) {
        image = gtk_image_new_from_icon_name(icon_name, HILDON_ICON_SIZE_NOTE);
    } else {
      image = gtk_image_new_from_icon_name(GTK_INFOPRINT_ICON_THEME, 
					   HILDON_ICON_SIZE_NOTE);
    }

    gtk_msg_window_init(parent, infoprint_quark(), text, image);
}                                                                        

/**
 * gtk_infoprintf:
 * @parent: The transient window for the infoprint.
 * @format: Format of the text.
 * @Varargs: List of parameters.
 *
 * Opens a new infoprint with @text printf-style formatting
 * string and comma-separated list of parameters.
 * 
 * If parent is %NULL, the infoprint is a system infoprint.
 * This version of infoprint allow you to make printf-like formatting
 * easily.
 */
void gtk_infoprintf(GtkWindow * parent, const gchar * format, ...)
{
    gchar *message;
    va_list args;

    va_start(args, format);
    message = g_strdup_vprintf(format, args);
    va_end(args);

    gtk_infoprint(parent, message);

    g_free(message);
}

/**
 * gtk_infoprint_temporarily_disable_wrap:
 * 
 * Will disable wrapping for the next shown infoprint. This only
 * affects next infoprint shown in this application.
 */
void gtk_infoprint_temporarily_disable_wrap(void)
{
    gtk_infoprint_temporal_wrap_disable_flag = TRUE;
}

/**
 * gtk_confirmation_banner:
 * @parent: The transient window for the confirmation banner.
 * @text: The text in confirmation banner
 * @stock_id: The stock id of the custom icon
 *
 * Opens a new confirmation banner with @text content.
 * With this function you can also set a custom icon
 * by giving a stock id as last parameter.
 *
 * If parent is %NULL, the banner is a system banner.
 * Normally you should use your application window
 * or dialog as a transient parent and avoid passing %NULL.
 * 
 * This function is otherwise similar to
 * gtk_infoprint_with_icon_stock except in always restricts
 * the text to one line and the font is emphasized.
 */
void
gtk_confirmation_banner(GtkWindow * parent, const gchar * text,
                        const gchar * stock_id)
{
    GtkWidget *image;

    if (stock_id) {
        image = gtk_image_new_from_stock(stock_id, HILDON_ICON_SIZE_NOTE);
    } else {
        image = gtk_image_new_from_stock(GTK_INFOPRINT_STOCK,
                                         HILDON_ICON_SIZE_NOTE);
    }

    queue_new_cbanner(parent, text, image);
}

/**
 * gtk_confirmation_banner_with_icon_name:
 * @parent: The transient window for the confirmation banner.
 * @text: The text in confirmation banner
 * @icon_name: The name of the custom icon in icon theme
 *
 * Opens a new confirmation banner with @text content.
 * With this function you can also set a custom icon
 * by giving a icon theme's icon name as last parameter.
 *
 * If parent is %NULL, the banner is a system banner.
 * Normally you should use your application window
 * or dialog as a transient parent and avoid passing %NULL.
 * 
 * This function is otherwise similar to
 * gtk_infoprint_with_icon_name except in always restricts
 * the text to one line and the font is emphasized.
 */
void
gtk_confirmation_banner_with_icon_name(GtkWindow * parent, const gchar * text,
                        const gchar * icon_name)
{
    GtkWidget *image;

    if (icon_name) {
        image = gtk_image_new_from_icon_name(icon_name, HILDON_ICON_SIZE_NOTE);
    } else {
        image = gtk_image_new_from_stock(GTK_INFOPRINT_STOCK,
                                         HILDON_ICON_SIZE_NOTE);
    }

    queue_new_cbanner(parent, text, image);
}

/**
 * gtk_banner_show_animation:
 * @parent: #GtkWindow
 * @text: #const gchar *
 *
 * The @text is the text shown in banner.
 * Creates a new banner with the animation.
 */
void gtk_banner_show_animation(GtkWindow * parent, const gchar * text)
{
    GtkWidget *item;
    GtkIconTheme *theme; 
    GtkIconInfo *info;

    theme = gtk_icon_theme_get_default();
    
    info = gtk_icon_theme_lookup_icon(theme, DEFAULT_PROGRESS_ANIMATION,
		    HILDON_ICON_SIZE_NOTE, 0);
    
    if (info) {
	const gchar *filename = gtk_icon_info_get_filename(info);
        item = gtk_image_new_from_file(filename);
    } else {
	g_print("icon theme lookup for icon failed!\n");
        item = gtk_image_new();
    }
    if (info)
        gtk_icon_info_free(info);

    gtk_msg_window_init(parent, banner_quark(), text, item);
}

/**
 * gtk_banner_show_bar
 * @parent: #GtkWindow
 * @text: #const gchar *
 *
 * The @text is the text shown in banner.
 * Creates a new banner with the progressbar.
 */
void gtk_banner_show_bar(GtkWindow * parent, const gchar * text)
{
    gtk_msg_window_init(parent, banner_quark(),
                        text, gtk_progress_bar_new());
}

/**
 * gtk_banner_set_text
 * @parent: #GtkWindow
 * @text: #const gchar *
 *
 * The @text is the text shown in banner.
 * Sets the banner text.
 */
void gtk_banner_set_text(GtkWindow * parent, const gchar * text)
{
    GtkWidget *item;

    g_return_if_fail(GTK_IS_WINDOW(parent) || parent == NULL);

    item = gtk_banner_get_widget(parent, label_quark());

    if (GTK_IS_LABEL(item))
        gtk_label_set_text(GTK_LABEL(item), text);
}

/**
 * gtk_banner_set_fraction:
 * @parent: #GtkWindow
 * @fraction: #gdouble
 *
 * The fraction is the completion of progressbar, 
 * the scale is from 0.0 to 1.0.
 * Sets the amount of fraction the progressbar has.
 */
void gtk_banner_set_fraction(GtkWindow * parent, gdouble fraction)
{
    GtkWidget *item;

    g_return_if_fail(GTK_IS_WINDOW(parent) || parent == NULL);

    item = gtk_banner_get_widget(parent, item_quark());

    if (GTK_IS_PROGRESS_BAR(item))
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(item), fraction);
}

/**
 * gtk_banner_close:
 * @parent: #GtkWindow
 *
 * Destroys the banner
 */
void gtk_banner_close(GtkWindow * parent)
{
    g_return_if_fail(GTK_IS_WINDOW(parent) || parent == NULL);

    if (pbanner_refs > 0) {
        --pbanner_refs;
        if (pbanner_refs == 0 && current_pbanner) {
            gtk_msg_window_destroy(current_pbanner->window);
        }
    }
}

/**
 * gtk_banner_temporarily_disable_wrap
 * 
 * Will disable wrapping for the next shown banner. This only
 * affects next banner shown in this application.
 **/
void gtk_banner_temporarily_disable_wrap(void)
{
    /* The below variable name is intentional. There's no real need for
       having two different variables for this functionality. */
    gtk_infoprint_temporal_wrap_disable_flag = TRUE;
}

/* We want the timer to be launched only after the infoprint is fully drawn.
 * As an approximation, we wait for an idle moment before starting
 * the timer. This method is not exact, since it does not guarantee that
 * the x-server has gotten around to drawing the window. The only way to be 
 * sure would require syncing with the x-server, but this should be close 
 * enough.
 */
static gboolean infoprint_idle_before_timer (GtkWidget *widget,
                                             GdkEventExpose *event,
                                             gpointer data)
{
    g_idle_add(infoprint_start_timer, widget);
    return FALSE;
}

/* Start the actual timer for the infoprint */
static gboolean infoprint_start_timer (gpointer data)
{
    if (GTK_IS_WIDGET (data)) {
      current_ibanner->timeout = g_timeout_add(MESSAGE_TIMEOUT,
              gtk_msg_window_destroy,
              current_ibanner->window);
      g_signal_connect_swapped(GTK_WIDGET(data), "destroy",
              G_CALLBACK(g_source_remove),
              GUINT_TO_POINTER(current_ibanner->timeout));
    }
    return FALSE;
}
