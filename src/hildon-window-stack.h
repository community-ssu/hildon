/*
 * This file is a part of hildon
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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

#ifndef                                         __HILDON_WINDOW_STACK_H__
#define                                         __HILDON_WINDOW_STACK_H__

#include                                        "hildon-stackable-window.h"

G_BEGIN_DECLS

#define                                         HILDON_TYPE_WINDOW_STACK \
                                                (hildon_window_stack_get_type())

#define                                         HILDON_WINDOW_STACK(obj) \
                                                (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                HILDON_TYPE_WINDOW_STACK, \
                                                HildonWindowStack))

#define                                         HILDON_WINDOW_STACK_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_WINDOW_STACK, \
                                                HildonWindowStackClass))

#define                                         HILDON_IS_WINDOW_STACK(obj) \
                                                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                HILDON_TYPE_WINDOW_STACK))

#define                                         HILDON_IS_WINDOW_STACK_CLASS(klass) \
                                                (G_TYPE_CHECK_CLASS_TYPE ((klass), \
                                                HILDON_TYPE_WINDOW_STACK))

#define                                         HILDON_WINDOW_STACK_GET_CLASS(obj) \
                                                (G_TYPE_INSTANCE_GET_CLASS ((obj), \
                                                HILDON_TYPE_WINDOW_STACK, \
                                                HildonWindowStackClass))

typedef struct                                  _HildonWindowStackPrivate HildonWindowStackPrivate;

#ifndef _TYPEDEF_HILDON_WINDOW_STACK_
#define _TYPEDEF_HILDON_WINDOW_STACK_
typedef struct                                  _HildonWindowStack HildonWindowStack;
#endif
typedef struct                                  _HildonWindowStackClass HildonWindowStackClass;

struct                                          _HildonWindowStack
{
    GObject parent;

    /* private */
    HildonWindowStackPrivate *priv;
};

struct                                          _HildonWindowStackClass
{
    GObjectClass parent_class;

    /* Padding for future extension */
    void (*_hildon_reserved1)(void);
    void (*_hildon_reserved2)(void);
    void (*_hildon_reserved3)(void);
    void (*_hildon_reserved4)(void);
};

GType
hildon_window_stack_get_type                    (void) G_GNUC_CONST;

HildonWindowStack *
hildon_window_stack_get_default                 (void);

HildonWindowStack *
hildon_window_stack_new                         (void);

gint
hildon_window_stack_size                        (HildonWindowStack *stack);

GtkWidget *
hildon_window_stack_peek                        (HildonWindowStack *stack);

void
hildon_window_stack_push                        (HildonWindowStack     *stack,
                                                 HildonStackableWindow *win1,
                                                 ...);

void
hildon_window_stack_push_list                   (HildonWindowStack *stack,
                                                 GList             *list);

void
hildon_window_stack_push_1                      (HildonWindowStack     *stack,
                                                 HildonStackableWindow *win);

void
hildon_window_stack_pop                         (HildonWindowStack  *stack,
                                                 gint                nwindows,
                                                 GList             **popped_windows);

GtkWidget *
hildon_window_stack_pop_1                       (HildonWindowStack *stack);

void
hildon_window_stack_pop_and_push                (HildonWindowStack      *stack,
                                                 gint                    nwindows,
                                                 GList                 **popped_windows,
                                                 HildonStackableWindow  *win1,
                                                 ...);

void
hildon_window_stack_pop_and_push_list           (HildonWindowStack  *stack,
                                                 gint                nwindows,
                                                 GList             **popped_windows,
                                                 GList              *list);

G_END_DECLS

#endif                                          /* __HILDON_WINDOW_STACK_H__ */
