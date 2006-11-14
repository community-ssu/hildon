/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
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

#include <gtk/gtkhbox.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkicontheme.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xatom.h>

#include "hildon-defines.h"
#include "hildon-banner.h"

/* position relative to the screen */
#define HILDON_BANNER_WINDOW_X            30
#define HILDON_BANNER_WINDOW_Y            73
#define HILDON_BANNER_WINDOW_FULLSCREEN_Y 20

/* max widths */
#define HILDON_BANNER_PROGRESS_WIDTH      104
#define HILDON_BANNER_LABEL_MAX_TIMED     375
#define HILDON_BANNER_LABEL_MAX_PROGRESS  375 /*265*/

/* default timeout */
#define HILDON_BANNER_TIMEOUT             3000

/* default icons */
#define HILDON_BANNER_DEFAULT_ICON               "qgn_note_infoprint"
#define HILDON_BANNER_DEFAULT_PROGRESS_ANIMATION "qgn_indi_pball_a"

enum {
   PROP_PARENT_WINDOW = 1,
   PROP_IS_TIMED
};

struct _HildonBannerPrivate
{
   GtkWidget *main_item;
   GtkWidget *label;
   GtkWidget *layout;
   guint timeout_id;
   gboolean is_timed; 
};

static GtkWidget *global_timed_banner = NULL;

G_DEFINE_TYPE(HildonBanner, hildon_banner, GTK_TYPE_WINDOW)

/* copy/paste from old infoprint implementation: Use matchbox 
   properties to find the topmost application window */
static Window get_current_app_window(void)
{
    unsigned long n;
    unsigned long extra;
    int format;
    int status;

    Atom atom_current_app_window = gdk_x11_get_xatom_by_name ("_MB_CURRENT_APP_WINDOW");
    Atom realType;
    Window win_result = None;
    guchar *data_return = NULL;

    status = XGetWindowProperty(GDK_DISPLAY(), GDK_ROOT_WINDOW(), 
                                atom_current_app_window, 0L, 16L,
                                0, XA_WINDOW, &realType, &format,
                                &n, &extra, 
                                &data_return);

    if ( status == Success && realType == XA_WINDOW 
         && format == 32 && n == 1 && data_return != NULL )
    {
        win_result = ((Window*) data_return)[0];
    } 
        
    if ( data_return ) 
        XFree(data_return);    
    
    return win_result;
}

/* Checks if a window is in fullscreen state or not. This
   information is needed when banners are positioned on screen.
   copy/paste from old infoprint implementation.  */
static gboolean check_fullscreen_state( Window window )
{
    unsigned long n;
    unsigned long extra;
    int format, status, i; 
    guchar *data_return = NULL;
    
    Atom realType;
    Atom atom_window_state = gdk_x11_get_xatom_by_name ("_NET_WM_STATE");
    Atom atom_fullscreen   = gdk_x11_get_xatom_by_name ("_NET_WM_STATE_FULLSCREEN");
    
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
                return TRUE;
            }
    }

    if (data_return) 
        XFree(data_return);

    return FALSE;
}

static GQuark hildon_banner_timed_quark(void)
{
    static GQuark quark = 0;

    if (G_UNLIKELY(quark == 0))
        quark = g_quark_from_static_string("hildon-banner-timed");

    return quark;
}

/* Set the label name to make the correct rc-style attached into it */
static void hildon_banner_bind_label_style(HildonBanner *self,
                                           const gchar  *name)
{
   GtkWidget *label = self->priv->label;

   /* Too bad that we cannot really reset the widget name */
   gtk_widget_set_name(label, name ? name : g_type_name(GTK_WIDGET_TYPE(label)));
}

/* In timeout function we automatically destroy timed banners */
static gboolean hildon_banner_timeout(gpointer data)
{
   GtkWidget *widget;
   GdkEvent *event;
   gboolean continue_timeout = FALSE;

   GDK_THREADS_ENTER ();
   
   g_assert(HILDON_IS_BANNER(data));

   widget = GTK_WIDGET(data);
   g_object_ref(widget);

   /* If the banner is currently visible (it normally should), 
      we simulate clicking the close button of the window.
      This allows applications to reuse the banner by prevent
      closing it etc */
   if (GTK_WIDGET_DRAWABLE(widget))
   {
      event = gdk_event_new (GDK_DELETE);
      event->any.window = g_object_ref(widget->window);
      event->any.send_event = FALSE;
      continue_timeout = gtk_widget_event (widget, event);
      gdk_event_free(event);
   }
   
   if (!continue_timeout)
      gtk_widget_destroy (widget);

   g_object_unref(widget);

   GDK_THREADS_LEAVE ();
   
   return continue_timeout;
}

static gboolean hildon_banner_clear_timeout(HildonBanner *self)
{
   g_assert(HILDON_IS_BANNER(self));

   if (self->priv->timeout_id != 0) {
      g_source_remove(self->priv->timeout_id);
      self->priv->timeout_id = 0;
      return TRUE;
   }

   return FALSE;
}

static void hildon_banner_ensure_timeout(HildonBanner *self)
{
   g_assert(HILDON_IS_BANNER(self));

   if (self->priv->timeout_id == 0 && self->priv->is_timed)
      self->priv->timeout_id = g_timeout_add(HILDON_BANNER_TIMEOUT, 
         hildon_banner_timeout, self);
}

static void hildon_banner_set_property(GObject      *object,
                                       guint         prop_id,
                                       const GValue *value,
                                       GParamSpec   *pspec)
{
   GtkWidget *window;
   GdkGeometry geom;
   HildonBannerPrivate *priv = HILDON_BANNER(object)->priv;

   switch (prop_id) {
      case PROP_IS_TIMED:
         priv->is_timed = g_value_get_boolean(value);

         /* Timed and progress notifications have different
            pixel size values for text. 
            We force to use requisition size for timed banners 
            in order to avoid resize problems when reusing the
            window (see bug #24339) */
         geom.max_width = priv->is_timed ? -1
                                         : HILDON_BANNER_LABEL_MAX_PROGRESS;
         geom.max_height = -1;
         gtk_window_set_geometry_hints(GTK_WINDOW(object), 
                                       priv->label, &geom, GDK_HINT_MAX_SIZE);
         break;

      case PROP_PARENT_WINDOW:
         window = g_value_get_object(value);         

         gtk_window_set_transient_for(GTK_WINDOW(object), (GtkWindow *) window);

         if (window)
            gtk_window_set_destroy_with_parent(GTK_WINDOW(object), TRUE);

         break;

      default:
         G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
         break;
  }
}

static void hildon_banner_get_property(GObject    *object,
                                       guint       prop_id,
                                       GValue     *value,
                                       GParamSpec *pspec)
{
   HildonBanner *self = HILDON_BANNER(object);

   switch (prop_id)
   {
      case PROP_IS_TIMED:
         g_value_set_boolean(value, self->priv->is_timed);
         break;

      case PROP_PARENT_WINDOW:
         g_value_set_object(value, gtk_window_get_transient_for(GTK_WINDOW(object)));
         break;

      default:
         G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
         break;
  }
}

static void hildon_banner_destroy(GtkObject *object)
{
   HildonBanner *self;
   GObject *parent_window;

   g_assert(HILDON_IS_BANNER(object));
   self = HILDON_BANNER(object);

   /* Drop possible global pointer. That can hold reference to us */
   if ((gpointer) object == (gpointer) global_timed_banner) {
      global_timed_banner = NULL;
      g_object_unref(object);
   }

   /* Remove the data from parent window for timed banners. Those hold reference */
   if (self->priv->is_timed && 
      (parent_window = (GObject *) gtk_window_get_transient_for(
             GTK_WINDOW(object))) != NULL)
      g_object_set_qdata(parent_window, hildon_banner_timed_quark(), NULL);

   (void) hildon_banner_clear_timeout(self);

   if (GTK_OBJECT_CLASS(hildon_banner_parent_class)->destroy)
      GTK_OBJECT_CLASS(hildon_banner_parent_class)->destroy(object);
}

/* Search a previous banner instance */
static GObject *hildon_banner_real_get_instance(GObject *window, gboolean timed)
{
   g_assert(window == NULL || GTK_IS_WINDOW(window));

   if (timed) {
      /* If we have a parent window, the previous instance is stored there */
      if (window) 
         return g_object_get_qdata(window, hildon_banner_timed_quark());

      /* System notification instance is stored into global pointer */
      return (GObject *) global_timed_banner;
   }

   /* Non-timed banners are normal (non-singleton) objects */
   return NULL;
}

/* By overriding constructor we force timed banners to be
   singletons for each window */
static GObject* hildon_banner_constructor (GType type,
                                           guint n_construct_params,
                                           GObjectConstructParam *construct_params)
{
   GObject *banner, *window = NULL;
   gboolean timed = FALSE;
   guint i;

   /* Search banner type information from parameters in order
      to locate the possible previous banner instance. */
   for (i = 0; i < n_construct_params; i++)
   {
      if (strcmp(construct_params[i].pspec->name, "parent-window") == 0)
         window = g_value_get_object(construct_params[i].value);       
      else if (strcmp(construct_params[i].pspec->name, "is-timed") == 0)
         timed = g_value_get_boolean(construct_params[i].value);
   }

   /* Try to get a previous instance if such exists */
   banner = hildon_banner_real_get_instance(window, timed);
   if (!banner)
   {
      /* We have to create a new banner */
      banner = G_OBJECT_CLASS(hildon_banner_parent_class)->constructor(
                  type, n_construct_params, construct_params);

      /* Store the newly created singleton instance either into parent 
         window data or into global variables. */
      if (timed) {
         if (window) {
            g_object_set_qdata_full(G_OBJECT(window), hildon_banner_timed_quark(), 
               g_object_ref(banner), g_object_unref); 
         } else {
            g_assert(global_timed_banner == NULL);
            global_timed_banner = g_object_ref(banner);
         }
      }
   }
   else {
      /* FIXME: This is a hack! We have to manually freeze
         notifications. This is normally done by g_object_init, but we
         are not going to call that. g_object_newv will otherwise give
         a critical like this:

            GLIB CRITICAL ** GLib-GObject - g_object_notify_queue_thaw: 
            assertion `nqueue->freeze_count > 0' failed */

      g_object_freeze_notify(banner);
   }

   /* We restart possible timeouts for each new timed banner request */
   if (timed && hildon_banner_clear_timeout(HILDON_BANNER(banner)))
      hildon_banner_ensure_timeout(HILDON_BANNER(banner));

   return banner;
}

/* We start the timer for timed notifications after the window appears on screen */
static gboolean hildon_banner_map_event(GtkWidget *widget, GdkEventAny *event)
{
   gboolean result = FALSE;

   if (GTK_WIDGET_CLASS(hildon_banner_parent_class)->map_event)
      result = GTK_WIDGET_CLASS(hildon_banner_parent_class)->map_event(widget, event);

   hildon_banner_ensure_timeout(HILDON_BANNER(widget));

   return result;
}  


/* force to wrap truncated label by setting explicit size request
 * see N#27000 and G#329646 */
static void force_to_wrap_truncated(HildonBanner *banner){
  GtkLabel *label;
  PangoLayout *layout;
  int width_text, width_max;
  int width = -1;

  label = GTK_LABEL(banner->priv->label);

  layout = gtk_label_get_layout(label);
  width_text  = PANGO_PIXELS(pango_layout_get_width(layout));
  /* = width to which the lines of the PangoLayout should be wrapped */

  width_max = banner->priv->is_timed ? HILDON_BANNER_LABEL_MAX_TIMED
                                     : HILDON_BANNER_LABEL_MAX_PROGRESS;

  if(width_text >= width_max){
    /* explicitly request maximum size to force wrapping */
    PangoRectangle logical;

    pango_layout_set_width (layout, width_max * PANGO_SCALE);
    pango_layout_get_extents (layout, NULL, &logical);

    width = PANGO_PIXELS (logical.width);
  }

  /* use fixed width when wrapping or natural one otherwise */
  gtk_widget_set_size_request (GTK_WIDGET (label),
			       width, -1);
}


static void hildon_banner_check_position(GtkWidget *widget)
{
   gint x, y;
   GtkRequisition req;

   force_to_wrap_truncated(HILDON_BANNER(widget)); /* see N#27000 and G#329646 */

   gtk_widget_size_request(widget, &req);

   if (req.width == 0)
   {
     return;
   }

   x = gdk_screen_width() - HILDON_BANNER_WINDOW_X - req.width;
   y = check_fullscreen_state(get_current_app_window()) ? 
         HILDON_BANNER_WINDOW_FULLSCREEN_Y : HILDON_BANNER_WINDOW_Y;

   gtk_window_move(GTK_WINDOW(widget), x, y);
}

static void hildon_banner_realize(GtkWidget *widget)
{
   /* We let the parent to init widget->window before we need it */
   if (GTK_WIDGET_CLASS(hildon_banner_parent_class)->realize)
      GTK_WIDGET_CLASS(hildon_banner_parent_class)->realize(widget);

   /* We use special hint to turn the banner into information notification. */
   gdk_window_set_type_hint(widget->window, GDK_WINDOW_TYPE_HINT_MESSAGE);

   hildon_banner_check_position(widget);
}

static void hildon_banner_class_init(HildonBannerClass *klass)
{
   GObjectClass *object_class;
   GtkWidgetClass *widget_class;

   object_class = G_OBJECT_CLASS(klass);
   widget_class = GTK_WIDGET_CLASS(klass);

   /* Append private structure to class. This is more elegant than
      on g_new based approach */
   g_type_class_add_private(klass, sizeof(HildonBannerPrivate));

   /* Override virtual methods */
   object_class->constructor = hildon_banner_constructor;
   object_class->set_property = hildon_banner_set_property;
   object_class->get_property = hildon_banner_get_property;
   GTK_OBJECT_CLASS(klass)->destroy = hildon_banner_destroy;
   widget_class->map_event = hildon_banner_map_event;
   widget_class->realize = hildon_banner_realize;

   /* Install properties.
      We need construct properties for singleton purposes */
   g_object_class_install_property(object_class, PROP_PARENT_WINDOW,
      g_param_spec_object("parent-window",
                          "Parent window",
                          "The window for which the banner will be singleton",
                          GTK_TYPE_WINDOW, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

   g_object_class_install_property(object_class, PROP_IS_TIMED,
      g_param_spec_boolean("is-timed",
                           "Is timed",
                           "Whether or not the notification goes away automatically "
                           "after the specified time has passed",
                           FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void hildon_banner_init(HildonBanner *self)
{
   HildonBannerPrivate *priv;

   /* Make private data available through cached pointer */
   self->priv = priv = G_TYPE_INSTANCE_GET_PRIVATE(self, 
                                                   HILDON_TYPE_BANNER,
                                                   HildonBannerPrivate);

   /* Initialize the common layout inside banner */
   self->priv->layout = gtk_hbox_new(FALSE, HILDON_MARGIN_DEFAULT);

   priv->label = g_object_new(GTK_TYPE_LABEL, NULL);
   gtk_label_set_line_wrap(GTK_LABEL(priv->label), TRUE);

   gtk_container_set_border_width(GTK_CONTAINER(self->priv->layout), HILDON_MARGIN_DEFAULT);
   gtk_container_add(GTK_CONTAINER(self), self->priv->layout);
   gtk_box_pack_start(GTK_BOX(self->priv->layout), priv->label, TRUE, TRUE, 0);

   gtk_window_set_accept_focus(GTK_WINDOW(self), FALSE);
}

/* Makes sure that icon/progress item contains the desired type
   of item. If possible, tries to avoid creating a new widget but
   reuses the existing one */
static void hildon_banner_ensure_child(HildonBanner *self, 
                                       GtkWidget    *user_widget,
                                       guint         pos,
                                       GType         type,
                                       const gchar  *first_property, ...)
{
   GtkWidget *widget;
   va_list args;

   g_assert(HILDON_IS_BANNER(self));
   g_assert(user_widget == NULL || GTK_IS_WIDGET(user_widget));

   widget = self->priv->main_item;
   va_start(args, first_property);

   /* Reuse existing widget if possible */
   if (!user_widget && G_TYPE_CHECK_INSTANCE_TYPE(widget, type))
   {
      g_object_set_valist(G_OBJECT(widget), first_property, args);
   }
   else
   {
      /* We have to abandon old content widget */
      if (widget)
         gtk_container_remove(GTK_CONTAINER(self->priv->layout), widget);

      /* Use user provided widget or create a new one */
      self->priv->main_item = widget = user_widget ? 
         user_widget : GTK_WIDGET(g_object_new_valist(type, first_property, args));
      gtk_box_pack_start(GTK_BOX(self->priv->layout), widget, TRUE, TRUE, 0);
   }

   /* We make sure that the widget exists in desired position. Different
      banners place this child widget to different places */
   gtk_box_reorder_child(GTK_BOX(self->priv->layout), widget, pos);
   va_end(args);
}

/* Creates a new banner instance or uses an existing one */
static HildonBanner *hildon_banner_get_instance_for_widget(GtkWidget *widget, gboolean timed)
{
   GtkWidget *window;

   g_return_val_if_fail(widget == NULL || GTK_IS_WIDGET(widget), NULL);
   window = widget ? gtk_widget_get_ancestor(widget, GTK_TYPE_WINDOW) : NULL;
   return g_object_new(HILDON_TYPE_BANNER, "parent-window", window, "is-timed", timed, NULL);
}





/* Public functions **************************************************************/

/**
 * hildon_banner_show_information:
 * @widget: the #GtkWidget that wants to display banner
 * @icon_name: the name of icon to use. Can be %NULL for default icon.
 * @text: Text to display
 *
 * This function creates and displays an information banner that
 * automatically goes away after certain time period. For each window
 * in your application there can only be one timed banner, so if you
 * spawn a new banner before the earlier one has timed out, the
 * previous one will be replaced.
 *
 * Since: 0.12.2
 */
void hildon_banner_show_information(GtkWidget *widget, 
                                    const gchar *icon_name,
                                    const gchar *text)
{
   HildonBanner *banner;

   g_return_if_fail(widget == NULL || GTK_IS_WIDGET(widget));
   g_return_if_fail(icon_name == NULL || icon_name[0] != 0);
   g_return_if_fail(text != NULL);

   /* Prepare banner */
   banner = hildon_banner_get_instance_for_widget(widget, TRUE);
   hildon_banner_ensure_child(banner, NULL, 0, GTK_TYPE_IMAGE, 
      "pixel-size", HILDON_ICON_PIXEL_SIZE_NOTE, 
      "icon-name", icon_name ? icon_name : HILDON_BANNER_DEFAULT_ICON,
      "yalign", 0.0, 
      NULL);
   hildon_banner_set_text(banner, text);
   hildon_banner_bind_label_style(banner, NULL);

   /* Show the banner, since caller cannot do that */
   gtk_widget_show_all(GTK_WIDGET(banner));
}

/**
 * hildon_banner_show_information_with_markup:
 * @widget: the #GtkWidget that wants to display banner
 * @icon_name: the name of icon to use. Can be %NULL for default icon.
 * @markup: a markup string to display (see <link linkend="PangoMarkupFormat">Pango markup format</link>)
 *
 * This function creates and displays an information banner that
 * automatically goes away after certain time period. For each window
 * in your application there can only be one timed banner, so if you
 * spawn a new banner before the earlier one has timed out, the
 * previous one will be replaced.
 *
 * Since: 0.12.8
 */
void hildon_banner_show_information_with_markup(GtkWidget *widget, 
   const gchar *icon_name, const gchar *markup)
{
   HildonBanner *banner;

   g_return_if_fail(widget == NULL || GTK_IS_WIDGET(widget));
   g_return_if_fail(icon_name == NULL || icon_name[0] != 0);
   g_return_if_fail(markup != NULL);

   /* Prepare banner */
   banner = hildon_banner_get_instance_for_widget(widget, TRUE);

   hildon_banner_ensure_child(banner, NULL, 0, GTK_TYPE_IMAGE, 
      "pixel-size", HILDON_ICON_PIXEL_SIZE_NOTE, 
      "icon-name", icon_name ? icon_name : HILDON_BANNER_DEFAULT_ICON,
      "yalign", 0.0, 
      NULL);
   hildon_banner_set_markup(banner, markup);
   hildon_banner_bind_label_style(banner, NULL);

   /* Show the banner, since caller cannot do that */
   gtk_widget_show_all(GTK_WIDGET(banner));
}

/**
 * hildon_banner_show_animation:
 * @widget: the #GtkWidget that wants to display banner
 * @animation_name: The progress animation to use. You usually can just
 *                  pass %NULL for the default animation.
 * @text: the text to display.
 *
 * Shows an animated progress notification. It's recommended not to try
 * to show more than one progress notification at a time, since
 * they will appear on top of each other. You can use progress
 * notifications with timed banners. In this case the banners are
 * located so that you can somehow see both.
 *
 * Please note that banners are destroyed automatically once the
 * window they are attached to is closed. The pointer that you
 * receive with this function do not contain additional references,
 * so it can become invalid without warning (this is true for
 * all toplevel windows in gtk). To make sure that the banner do not disapear
 * automatically, you can separately ref the return value (this
 * doesn't prevent the banner from disappearing, but the object it just
 * not finalized). In this case you have to call both #gtk_widget_destroy 
 * followed by #g_object_unref (in this order).
 * 
 * Returns: a #HildonBanner widget. You must call #gtk_widget_destroy
 *          once you are ready with the banner.
 *
 * Since: 0.12.2
 */
GtkWidget *hildon_banner_show_animation(GtkWidget *widget, 
   const gchar *animation_name, const gchar *text)
{
   HildonBanner *banner;
   GtkIconTheme *theme; 
   GtkIconInfo *info;
   GtkWidget *image_widget;
   const gchar *filename;

   g_return_val_if_fail(widget == NULL || GTK_IS_WIDGET(widget), NULL);
   g_return_val_if_fail(animation_name == NULL || animation_name[0] != 0, NULL);
   g_return_val_if_fail(text != NULL, NULL);

   /* Find out which animation to use */
   theme = gtk_icon_theme_get_default();
   info = gtk_icon_theme_lookup_icon(theme, animation_name ?   /* TODO: consider using: gtk_icon_theme_load_icon() */
            animation_name : HILDON_BANNER_DEFAULT_PROGRESS_ANIMATION,
            HILDON_ICON_SIZE_NOTE, 0);

   /* Try to load animation. One could try to optimize this 
      to avoid loading the default animation during each call */
   if (info) {
   	  filename = gtk_icon_info_get_filename(info);
      image_widget = gtk_image_new_from_file(filename);
      gtk_icon_info_free(info);
   } else {
      g_warning("icon theme lookup for icon failed!");
      image_widget = NULL;
   }

   /* Prepare banner */
   banner = hildon_banner_get_instance_for_widget(widget, FALSE);
   hildon_banner_ensure_child(banner, image_widget, 0,
      GTK_TYPE_IMAGE, "yalign", 0.0, NULL);
   hildon_banner_set_text(banner, text);
   hildon_banner_bind_label_style(banner, NULL);

   /* And show it */
   gtk_widget_show_all(GTK_WIDGET(banner));

   return GTK_WIDGET(banner);
}

/**
 * hildon_banner_show_progress:
 * @widget: the #GtkWidget that wants to display banner
 * @bar: Progressbar to use. You usually can just pass %NULL, unless
 *       you want somehow customized progress bar.
 * @text: text to display.
 *
 * Shows progress notification. See #hildon_banner_show_animation
 * for more information.
 * 
 * Returns: a #HildonBanner widget. You must call #gtk_widget_destroy
 *          once you are ready with the banner.
 *
 * Since: 0.12.2
 */
GtkWidget *hildon_banner_show_progress(GtkWidget *widget, 
   GtkProgressBar *bar, const gchar *text)
{
   HildonBanner *banner;

   g_return_val_if_fail(widget == NULL || GTK_IS_WIDGET(widget), NULL);
   g_return_val_if_fail(bar == NULL || GTK_IS_PROGRESS_BAR(bar), NULL);
   g_return_val_if_fail(text != NULL, NULL);

   /* Prepare banner */
   banner = hildon_banner_get_instance_for_widget(widget, FALSE);
   hildon_banner_ensure_child(banner, (GtkWidget *) bar, -1, GTK_TYPE_PROGRESS_BAR, NULL);
   gtk_widget_set_size_request(banner->priv->main_item,
      HILDON_BANNER_PROGRESS_WIDTH, -1);
   hildon_banner_set_text(banner, text);
   hildon_banner_bind_label_style(banner, NULL);

   /* Show the banner */
   gtk_widget_show_all(GTK_WIDGET(banner));

   return GTK_WIDGET(banner);   
}

/**
 * hildon_banner_set_text:
 * @self: a #HildonBanner widget
 * @text: a new text to display in banner
 *
 * Sets the text that is displayed in the banner.
 *
 * Since: 0.12.2
 */
void hildon_banner_set_text(HildonBanner *self, const gchar *text)
{
   GtkLabel *label;

   g_return_if_fail(HILDON_IS_BANNER(self));

   label = GTK_LABEL(self->priv->label);
   gtk_label_set_text(label, text);

   hildon_banner_check_position(GTK_WIDGET(self));
}

/**
 * hildon_banner_set_markup:
 * @self: a #HildonBanner widget
 * @markup: a new text with Pango markup to display in the banner
 *
 * Sets the text with markup that is displayed in the banner.
 *
 * Since: 0.12.8
 */
void hildon_banner_set_markup(HildonBanner *self, const gchar *markup)
{
   GtkLabel *label;

   g_return_if_fail(HILDON_IS_BANNER(self));

   label = GTK_LABEL(self->priv->label);
   gtk_label_set_markup(label, markup);

   hildon_banner_check_position(GTK_WIDGET(self));
}

/**
 * hildon_banner_set_fraction:
 * @self: a #HildonBanner widget
 * @fraction: #gdouble
 *
 * The fraction is the completion of progressbar, 
 * the scale is from 0.0 to 1.0.
 * Sets the amount of fraction the progressbar has.
 *
 * Since: 0.12.2
 */
void hildon_banner_set_fraction(HildonBanner *self, gdouble fraction)
{
   g_return_if_fail(HILDON_IS_BANNER(self));
   g_return_if_fail(GTK_IS_PROGRESS_BAR(self->priv->main_item));
   gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(self->priv->main_item), fraction);
}





/**
 * Deprecated: really, do NOT use.
 */
void _hildon_gtk_label_set_text_n_lines(GtkLabel *label, const gchar *text, gint max_lines)
{
/* Forces the wrapping of text into several lines and ellipsizes the rest. 
   Similar to combination of gtk_label_set_wrap and pango ellipzation. 
   We cannot just use those directly, since ellipzation always wins wrapping.

   This means that we have to:
     * First wrap the text
     * Insert forced linebreaks into text
     * Truncate the result

   NOTE! This will not work with pango markup!

   FIXME: luc: DO NOT TRUNCATE the text. Use as many lines as needed.
          Lenth of the text is under applications' responsibility.
          Widget does not have to enforce this.
*/
   PangoLayout *layout;
   PangoLayoutLine *line;
   GtkRequisition req;
   GString *wrapped_text;
   gchar *line_data;
   gint lines, i;

   g_return_if_fail(GTK_IS_LABEL(label));
   g_return_if_fail(max_lines >= 1);

   /* Setup the label to contain the new data */
   gtk_label_set_text(label, text);
   gtk_label_set_line_wrap(label, TRUE);
   gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_NONE);
   
   /* We really want to recalculate the size, not use some old values */
   gtk_widget_size_request(GTK_WIDGET(label), &req);
   layout = gtk_label_get_layout(label);
   lines = pango_layout_get_line_count(layout);

   /* Now collect the wrapped text. */
   wrapped_text = g_string_new(NULL);

   for (i = 0; i < lines; i++)
   {
      /* Append the next line into wrapping buffer, but 
         avoid adding extra whitespaces at the end, since those
         can cause other lines to be ellipsized as well. */
      line = pango_layout_get_line(layout, i);
      line_data = g_strndup(pango_layout_get_text(layout) + line->start_index, 
         line->length);
      g_strchomp(line_data);
      g_string_append(wrapped_text, line_data);

      /* Append forced linebreaks, until we have the desired
         amount of lines. After that we put the rest to the
         last line to make ellipzation to happen */
      if (i < lines - 1)
      {
         if (i < max_lines - 1)
            g_string_append_c(wrapped_text, '\n');
         else
            g_string_append_c(wrapped_text, ' ');
      }

      g_free(line_data);
   }

   /* Now update the label to use wrapped text. Use builtin
      ellipzation as well. */
   gtk_widget_set_size_request(GTK_WIDGET(label), req.width, -1);
   gtk_label_set_text(label, wrapped_text->str);
   gtk_label_set_ellipsize(label, PANGO_ELLIPSIZE_END);
   gtk_label_set_line_wrap(label, FALSE);

   g_string_free(wrapped_text, TRUE);
}
