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
 
#ifndef                                         __HILDON_CAPTION_H__
#define                                         __HILDON_CAPTION_H__

#include                                        <glib.h>
#include                                        <glib-object.h>
#include                                        <gtk/gtkeventbox.h>
#include                                        <gtk/gtksizegroup.h>
#include                                        "hildon-enum-types.h"

G_BEGIN_DECLS

typedef struct                                  _HildonCaption HildonCaption;

typedef struct                                  _HildonCaptionClass HildonCaptionClass;

#define                                         HILDON_TYPE_CAPTION (hildon_caption_get_type())

#define                                         HILDON_CAPTION(obj) \
                                                (GTK_CHECK_CAST (obj, HILDON_TYPE_CAPTION, HildonCaption))

#define                                         HILDON_CAPTION_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass),\
                                                HILDON_TYPE_CAPTION, HildonCaptionClass))

#define                                         HILDON_IS_CAPTION(obj) \
                                                (GTK_CHECK_TYPE (obj, HILDON_TYPE_CAPTION))

#define                                         HILDON_IS_CAPTION_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_CAPTION))
    
/**
 * HildonCaptionStatus:
 * @HILDON_CAPTION_OPTIONAL: Optional.
 * @HILDON_CAPTION_MANDATORY: Mandatory.
 *
 * Keys to set the #HildonCaption to be optional or mandatory.
 */
typedef enum
{
    HILDON_CAPTION_OPTIONAL = 0,
    HILDON_CAPTION_MANDATORY
}                                               HildonCaptionStatus;

/**
 * HildonCaptionIconPosition:
 * @HILDON_CAPTION_POSITION_LEFT: Show the icon on the left side.
 * @HILDON_CAPTION_POSITION_RIGHT: Show the icon on the right side.
 *
 * Keys to set the icon placement in #HildonCaption.
 *
 */
typedef enum
{
    HILDON_CAPTION_POSITION_LEFT = 0,
    HILDON_CAPTION_POSITION_RIGHT
}                                               HildonCaptionIconPosition;


struct                                          _HildonCaption
{
    GtkEventBox parent;
};


struct                                          _HildonCaptionClass
{
    GtkEventBoxClass parent_class;
    void (*activate) (HildonCaption *widget);
};

GType G_GNUC_CONST
hildon_caption_get_type                         (void);

GtkWidget* 
hildon_caption_new                              (GtkSizeGroup *group, 
                                                 const gchar *value,
                                                 GtkWidget *control, 
                                                 GtkWidget *icon,
                                                 HildonCaptionStatus flag);

GtkSizeGroup*
hildon_caption_get_size_group                   (const HildonCaption *caption);

void 
hildon_caption_set_size_group                   (const HildonCaption *caption,
                                                 GtkSizeGroup *new_group );

gboolean 
hildon_caption_is_mandatory                     (const HildonCaption *caption);

void 
hildon_caption_set_status                       (HildonCaption *caption,
                                                 HildonCaptionStatus flag);

HildonCaptionStatus
hildon_caption_get_status                       (const HildonCaption *caption);

void 
hildon_caption_set_icon_position                (HildonCaption *caption,
                                                 HildonCaptionIconPosition pos );

HildonCaptionIconPosition
hildon_caption_get_icon_position                (const HildonCaption *caption);

void
hildon_caption_set_icon_image                   (HildonCaption *caption, 
                                                 GtkWidget *icon);

GtkWidget*
hildon_caption_get_icon_image                   (const HildonCaption *caption);

void
hildon_caption_set_label                        (HildonCaption *caption, 
                                                 const gchar *label );

gchar*
hildon_caption_get_label                        (const HildonCaption *caption);

void 
hildon_caption_set_separator                    (HildonCaption *caption, 
                                                 const gchar *separator);

gchar*
hildon_caption_get_separator                    (const HildonCaption *caption);

void 
hildon_caption_set_label_alignment              (HildonCaption *caption, 
                                                 gfloat alignment);

gfloat 
hildon_caption_get_label_alignment              (HildonCaption *caption);

void 
hildon_caption_set_child_expand                 (HildonCaption *caption, 
                                                 gboolean expand);

gboolean 
hildon_caption_get_child_expand                 (const HildonCaption *caption);

void
hildon_caption_set_label_markup                 (HildonCaption *caption,
                                                 const gchar *markup);

G_END_DECLS

#endif                                          /* __HILDON_CAPTION_H__ */
