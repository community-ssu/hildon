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

#ifndef                                         __HILDON_DATE_SELECTOR_H__
#define                                         __HILDON_DATE_SELECTOR_H__

#include                                        "hildon-touch-selector.h"

G_BEGIN_DECLS
#define                                         HILDON_TYPE_DATE_SELECTOR \
                                                (hildon_date_selector_get_type ())

#define                                         HILDON_DATE_SELECTOR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_DATE_SELECTOR, HildonDateSelector))

#define                                         HILDON_DATE_SELECTOR_CLASS(vtable) \
                                                (G_TYPE_CHECK_CLASS_CAST ((vtable), \
                                                HILDON_TYPE_DATE_SELECTOR, HildonDateSelectorClass))

#define                                         HILDON_IS_DATE_SELECTOR(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HILDON_TYPE_DATE_SELECTOR))

#define                                         HILDON_IS_DATE_SELECTOR_CLASS(vtable) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((vtable), HILDON_TYPE_DATE_SELECTOR))

#define                                         HILDON_DATE_SELECTOR_GET_CLASS(inst) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((inst), \
                                                HILDON_TYPE_DATE_SELECTOR, HildonDateSelectorClass))

typedef struct                                  _HildonDateSelector HildonDateSelector;
typedef struct                                  _HildonDateSelectorClass HildonDateSelectorClass;
typedef struct                                  _HildonDateSelectorPrivate HildonDateSelectorPrivate;

struct                                          _HildonDateSelector
{
  HildonTouchSelector parent_instance;

  /*< private > */
  HildonDateSelectorPrivate *priv;
};

struct                                          _HildonDateSelectorClass
{
  HildonTouchSelectorClass parent_class;

  /* signals */
};


/* construction */
GType
hildon_date_selector_get_type                   (void) G_GNUC_CONST;

GtkWidget*
hildon_date_selector_new                        (void);

GtkWidget *
hildon_date_selector_new_with_year_range        (gint min_year,
                                                 gint max_year);

/* date management */
gboolean
hildon_date_selector_select_month               (HildonDateSelector *selector,
                                                 guint               month,
                                                 guint               year);

void
hildon_date_selector_select_day                 (HildonDateSelector *selector,
                                                 guint               day);

gboolean
hildon_date_selector_select_current_date        (HildonDateSelector *selector,
                                                 guint               year,
                                                 guint               month,
                                                 guint               day);

void
hildon_date_selector_get_date                   (HildonDateSelector *selector,
                                                 guint              *year,
                                                 guint              *month,
                                                 guint              *day);

G_END_DECLS

#endif /* __HILDON_DATE_SELECTOR_H__ */
