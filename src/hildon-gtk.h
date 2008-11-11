/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation; version 2 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 *
 */

#ifndef                                         __HILDON_GTK_H__
#define                                         __HILDON_GTK_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum {
   HILDON_SIZE_AUTO_WIDTH                       = 0 << 0, /* set to automatic width */
   HILDON_SIZE_HALFSCREEN_WIDTH                 = 1 << 0, /* set to 50% screen width */
   HILDON_SIZE_FULLSCREEN_WIDTH                 = 2 << 0, /* set to 100% screen width */
   HILDON_SIZE_AUTO_HEIGHT                      = 0 << 2, /* set to automatic height */
   HILDON_SIZE_FINGER_HEIGHT                    = 1 << 2, /* set to finger height */
   HILDON_SIZE_THUMB_HEIGHT                     = 2 << 2, /* set to thumb height */
   HILDON_SIZE_AUTO                             = (HILDON_SIZE_AUTO_WIDTH | HILDON_SIZE_AUTO_HEIGHT)
}                                               HildonSizeType;

void
hildon_gtk_widget_set_theme_size                (GtkWidget      *widget,
                                                 HildonSizeType  size);

GtkWidget *
hildon_gtk_menu_new                             (void);

GtkWidget *
hildon_gtk_button_new                           (HildonSizeType size);

GtkWidget *
hildon_gtk_toggle_button_new                    (HildonSizeType size);

GtkWidget *
hildon_gtk_radio_button_new                     (HildonSizeType  size,
                                                 GSList         *group);

GtkWidget *
hildon_gtk_radio_button_new_from_widget         (HildonSizeType  size,
                                                 GtkRadioButton *radio_group_member);

#ifdef MAEMO_GTK
GtkWidget *
hildon_gtk_tree_view_new                        (HildonUIMode mode);

GtkWidget *
hildon_gtk_tree_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model);

GtkWidget *
hildon_gtk_icon_view_new                        (HildonUIMode mode);

GtkWidget *
hildon_gtk_icon_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model);
#endif /* MAEMO_GTK */

void
hildon_gtk_window_set_progress_indicator        (GtkWindow    *window,
                                                 guint        state);

G_END_DECLS

#endif /* __HILDON_GTK_H__ */
