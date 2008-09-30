/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version. or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef                                         __HILDON_TOUCH_SELECTOR_COLUMN_H__
#define                                         __HILDON_TOUCH_SELECTOR_COLUMN_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_TOUCH_SELECTOR_COLUMN \
                                                (hildon_touch_selector_column_get_type ())

#define                                         HILDON_TOUCH_SELECTOR_COLUMN(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_TOUCH_SELECTOR_COLUMN, HildonTouchSelectorColumn))

#define                                         HILDON_TOUCH_SELECTOR_COLUMN_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_TOUCH_SELECTOR_COLUMN, HildonTouchSelectorColumnClass))

#define                                         HILDON_IS_TOUCH_SELECTOR_COLUMN(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                HILDON_TYPE_TOUCH_SELECTOR_COLUMN))

#define                                         HILDON_IS_TOUCH_SELECTOR_COLUMN_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                HILDON_TYPE_TOUCH_SELECTOR_COLUMN))

#define                                         HILDON_TOUCH_SELECTOR_COLUMN_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_TOUCH_SELECTOR_COLUMN, HildonTouchSelectorColumnClass))

typedef struct                                  _HildonTouchSelectorColumn HildonTouchSelectorColumn;
typedef struct                                  _HildonTouchSelectorColumnClass HildonTouchSelectorColumnClass;
typedef struct                                  _HildonTouchSelectorColumnPrivate HildonTouchSelectorColumnPrivate;

struct                                         _HildonTouchSelectorColumn
{
  GObject parent;

  /* < private > */
  HildonTouchSelectorColumnPrivate *priv;
};

struct                                          _HildonTouchSelectorColumnClass
{
  GObjectClass parent_class;
};

GType G_GNUC_CONST
hildon_touch_selector_column_get_type           (void);

G_END_DECLS


#endif /*__HILDON_TOUCH_SELECTOR_COLUMN_H__ */
