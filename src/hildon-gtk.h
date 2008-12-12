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

void
hildon_gtk_tree_view_set_ui_mode                (GtkTreeView  *treeview,
                                                 HildonUIMode  mode);

GtkWidget *
hildon_gtk_icon_view_new                        (HildonUIMode mode);

GtkWidget *
hildon_gtk_icon_view_new_with_model             (HildonUIMode  mode,
                                                 GtkTreeModel *model);

void
hildon_gtk_icon_view_set_ui_mode                (GtkIconView  *iconview,
                                                 HildonUIMode  mode);
#endif /* MAEMO_GTK */

void
hildon_gtk_window_set_progress_indicator        (GtkWindow    *window,
                                                 guint        state);

GtkWidget*
hildon_gtk_hscale_new                           (void);

G_END_DECLS

#endif /* __HILDON_GTK_H__ */
