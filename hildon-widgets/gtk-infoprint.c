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

/**
 * SECTION:gtk-infoprint
 * @short_description: deprecated widget. Use #HildonBanner instead
 *
 * This widget is deprecated. Use #HildonBanner instead
 */

#include "gtk-infoprint.h"
#include "hildon-banner.h"

/* This is a helper function that searches the banner for
   given window. This is needed to provide backwards
   compatibility. */
static GtkWidget *find_banner_for_parent(GtkWindow *parent)
{
   GList *toplevels, *iter;
   GtkWidget *result = NULL;
   gboolean is_timed;

   toplevels = gtk_window_list_toplevels();
   for (iter = toplevels; iter; iter = iter->next)
      if (HILDON_IS_BANNER(iter->data) && 
          gtk_window_get_transient_for(GTK_WINDOW(iter->data)) == parent)
      {
         g_object_get(iter->data, "is-timed", &is_timed, NULL);

         /* We do not want to touch timed infoprints */
         if (!is_timed) {
           result = iter->data;
           break;
         }
      }

   g_list_free(toplevels);
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
 *
 * Deprecated: Use #hildon_banner_show_information instead.
 */
void gtk_infoprint(GtkWindow * parent, const gchar * text)
{
    hildon_banner_show_information((GtkWidget *) parent, NULL, text);    
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
 *
 * Deprecated: Use #hildon_banner_show_information instead.
 */
void
gtk_infoprint_with_icon_stock(GtkWindow * parent,
                              const gchar * text, const gchar * stock_id)
{
   hildon_banner_show_information((GtkWidget *) parent, NULL, text);
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
 *
 * Deprecated: Use #hildon_banner_show_information instead.
 */
void
gtk_infoprint_with_icon_name(GtkWindow * parent,
                              const gchar * text, const gchar * icon_name)
{
   hildon_banner_show_information((GtkWidget *) parent, icon_name, text);
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
 *
 * Deprecated: Use #hildon_banner_show_information instead.
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
 *
 * Currently it does nothing.
 *
 * Deprecated: 
 */
void gtk_infoprint_temporarily_disable_wrap(void)
{
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
 *
 * Deprecated: Use #hildon_banner_show_information instead.
 */
void
gtk_confirmation_banner(GtkWindow * parent, const gchar * text,
                        const gchar * stock_id)
{
  gchar *s;
  s = g_strdup_printf("<b>%s</b>", text);

  hildon_banner_show_information_with_markup((GtkWidget *) parent, NULL, s);

  g_free(s);
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
 *
 * Deprecated: Use #hildon_banner_show_information instead.
 */
void
gtk_confirmation_banner_with_icon_name(GtkWindow * parent, const gchar * text,
                        const gchar * icon_name)
{
   hildon_banner_show_information((GtkWidget *) parent, icon_name, text);
}

/**
 * gtk_banner_show_animation:
 * @parent: #GtkWindow
 * @text: #const gchar *
 *
 * The @text is the text shown in banner.
 * Creates a new banner with the animation.
 *
 * Deprecated: Use #hildon_banner_show_animation instead.
 */
void gtk_banner_show_animation(GtkWindow * parent, const gchar * text)
{
   (void) hildon_banner_show_animation((GtkWidget *) parent, NULL, text);
}

/**
 * gtk_banner_show_bar
 * @parent: #GtkWindow
 * @text: #const gchar *
 *
 * The @text is the text shown in banner.
 * Creates a new banner with the progressbar.
 *
 * Deprecated: Use #hildon_banner_show_progress instead.
 */
void gtk_banner_show_bar(GtkWindow * parent, const gchar * text)
{
   (void) hildon_banner_show_progress((GtkWidget *) parent, NULL, text);
}

/**
 * gtk_banner_set_text
 * @parent: #GtkWindow
 * @text: #const gchar *
 *
 * The @text is the text shown in banner.
 * Sets the banner text.
 *
 * Deprecated: Use #hildon_banner_set_text instead.
 */
void gtk_banner_set_text(GtkWindow * parent, const gchar * text)
{
   GtkWidget *banner;

   g_return_if_fail(GTK_IS_WINDOW(parent) || parent == NULL);

   banner = find_banner_for_parent(parent);
   if (banner)
      hildon_banner_set_text(HILDON_BANNER(banner), text);
}

/**
 * gtk_banner_set_fraction:
 * @parent: #GtkWindow
 * @fraction: #gdouble
 *
 * The fraction is the completion of progressbar, 
 * the scale is from 0.0 to 1.0.
 * Sets the amount of fraction the progressbar has.
 *
 * Deprecated: Use #hildon_banner_set_fraction instead.
 */
void gtk_banner_set_fraction(GtkWindow * parent, gdouble fraction)
{
   GtkWidget *banner;

   g_return_if_fail(GTK_IS_WINDOW(parent) || parent == NULL);

   banner = find_banner_for_parent(parent);
   if (banner)
      hildon_banner_set_fraction(HILDON_BANNER(banner), fraction);
}

/**
 * gtk_banner_close:
 * @parent: #GtkWindow
 *
 * Destroys the banner
 *
 * Deprecated:
 */
void gtk_banner_close(GtkWindow * parent)
{
   GtkWidget *banner;

   g_return_if_fail(GTK_IS_WINDOW(parent) || parent == NULL);

   banner = find_banner_for_parent(parent);
   if (banner)
      gtk_widget_destroy(banner);
}

/**
 * gtk_banner_temporarily_disable_wrap
 * 
 * Will disable wrapping for the next shown banner. This only
 * affects next banner shown in this application.
 *
 * Currently it does nothing.
 *
 * Deprecated:
 **/
void gtk_banner_temporarily_disable_wrap(void)
{
}
