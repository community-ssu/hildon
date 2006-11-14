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

#ifndef __HILDON_COLOR_BUTTON_H__
#define __HILDON_COLOR_BUTTON_H__

#include <gtk/gtkbutton.h>

G_BEGIN_DECLS

#define HILDON_TYPE_COLOR_BUTTON             (hildon_color_button_get_type ())
#define HILDON_COLOR_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HILDON_TYPE_COLOR_BUTTON, HildonColorButton))
#define HILDON_COLOR_BUTTON_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_COLOR_BUTTON, HildonColorButtonClass))
#define HILDON_IS_COLOR_BUTTON(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_COLOR_BUTTON))
#define HILDON_IS_COLOR_BUTTON_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_COLOR_BUTTON))
#define HILDON_COLOR_BUTTON_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HILDON_TYPE_COLOR_BUTTON, HildonColorButtonClass))

typedef struct _HildonColorButton          HildonColorButton;
typedef struct _HildonColorButtonClass     HildonColorButtonClass;
typedef struct _HildonColorButtonPrivate   HildonColorButtonPrivate;

struct _HildonColorButton
{
  GtkButton button;
  HildonColorButtonPrivate *priv;
};

struct _HildonColorButtonClass
{
  GtkButtonClass parent_class;

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      
hildon_color_button_get_type( void ) G_GNUC_CONST;

GtkWidget *
hildon_color_button_new( void );

GtkWidget *
hildon_color_button_new_with_color( const GdkColor *color );

GdkColor *
hildon_color_button_get_color( HildonColorButton *button );

void
hildon_color_button_set_color( HildonColorButton *button, GdkColor *color );

G_END_DECLS

#endif  /* __HILDON_COLOR_BUTTON_H__ */




