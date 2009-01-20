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

#ifndef                                         __HILDON_TIME_SELECTOR_H__
#define                                         __HILDON_TIME_SELECTOR_H__

#include                                        "hildon-touch-selector.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_TIME_SELECTOR \
                                                (hildon_time_selector_get_type ())

#define                                         HILDON_TIME_SELECTOR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_TIME_SELECTOR, HildonTimeSelector))

#define                                         HILDON_TIME_SELECTOR_CLASS(vtable) \
                                                (G_TYPE_CHECK_CLASS_CAST ((vtable), \
                                                HILDON_TYPE_TIME_SELECTOR, HildonTimeSelectorClass))

#define                                         HILDON_IS_TIME_SELECTOR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_TIME_SELECTOR))

#define                                         HILDON_IS_TIME_SELECTOR_CLASS(vtable) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((vtable), HILDON_TYPE_TIME_SELECTOR))

#define                                         HILDON_TIME_SELECTOR_GET_CLASS(inst) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((inst), \
                                                HILDON_TYPE_TIME_SELECTOR, HildonTimeSelectorClass))

typedef struct                                  _HildonTimeSelector HildonTimeSelector;
typedef struct                                  _HildonTimeSelectorClass HildonTimeSelectorClass;
typedef struct                                  _HildonTimeSelectorPrivate HildonTimeSelectorPrivate;

struct                                          _HildonTimeSelector
{
  HildonTouchSelector parent_instance;

  /*< private > */
  HildonTimeSelectorPrivate *priv;
};

struct                                          _HildonTimeSelectorClass
{
  HildonTouchSelectorClass parent_class;

  /* signals */
};


/* construction */
GType
hildon_time_selector_get_type                   (void) G_GNUC_CONST;

GtkWidget*
hildon_time_selector_new                        (void);

GtkWidget*
hildon_time_selector_new_step                   (guint minutes_step);

/* time management */
gboolean
hildon_time_selector_set_time                   (HildonTimeSelector *selector,
                                                 guint               hours,
                                                 guint               minutes);

void
hildon_time_selector_get_time                   (HildonTimeSelector *selector,
                                                 guint              *hours,
                                                 guint              *minutes);

G_END_DECLS

#endif /* __HILDON_TIME_SELECTOR_H__ */
