/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

#ifndef                                         __HILDON_FIND_TOOLBAR_H__
#define                                         __HILDON_FIND_TOOLBAR_H__

#include                                        <gtk/gtktoolbar.h>
#include                                        <gtk/gtkliststore.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_FIND_TOOLBAR (hildon_find_toolbar_get_type())

#define                                         HILDON_FIND_TOOLBAR(object) \
                                                (G_TYPE_CHECK_INSTANCE_CAST((object), \
                                                HILDON_TYPE_FIND_TOOLBAR, \
                                                HildonFindToolbar))

#define                                         HILDON_FIND_TOOLBAR_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST((klass), HILDON_TYPE_FIND_TOOLBAR, \
                                                HildonFindToolbarClass))

#define                                         HILDON_IS_FIND_TOOLBAR(object) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE((object), \
                                                HILDON_TYPE_FIND_TOOLBAR))

#define                                         HILDON_IS_FIND_TOOLBAR_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST((klass), \
                                                HILDON_TYPE_FIND_TOOLBAR))

#define                                         HILDON_FIND_TOOLBAR_GET_CLASS(object) \
                                                (G_TYPE_INSTANCE_GET_CLASS((object), \
                                                HILDON_TYPE_FIND_TOOLBAR, \
                                                HildonFindToolbarClass))

typedef struct                                  _HildonFindToolbar HildonFindToolbar;

typedef struct                                  _HildonFindToolbarClass HildonFindToolbarClass;

struct _HildonFindToolbar
{
  GtkToolbar parent;
};

struct _HildonFindToolbarClass
{
  GtkToolbarClass parent_class;

  void (*search) (HildonFindToolbar *toolbar);
  void (*close) (HildonFindToolbar *toolbar);
  void (*invalid_input) (HildonFindToolbar *toolbar);
  gboolean (*history_append) (HildonFindToolbar *tooblar);
};

GType G_GNUC_CONST
hildon_find_toolbar_get_type                    (void);

GtkWidget*      
hildon_find_toolbar_new                         (const gchar *label);

GtkWidget*      
hildon_find_toolbar_new_with_model              (const gchar *label,
                                                 GtkListStore* model,
                                                 gint column);

void 
hildon_find_toolbar_highlight_entry             (HildonFindToolbar *ftb,
                                                 gboolean get_focus);

G_END_DECLS

#endif                                          /* __HILDON_FIND_TOOLBAR_H__ */

