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

#ifndef __HILDON_TELEPHONE_EDITOR_H__
#define __HILDON_TELEPHONE_EDITOR_H__

#include <gtk/gtkcontainer.h>

G_BEGIN_DECLS
/**
 * HILDON_TELEPHONE_EDITOR_TYPE:
 *
 * Macro for getting type of telephone editor.
 */
#define HILDON_TELEPHONE_EDITOR_TYPE \
  ( hildon_telephone_editor_get_type() )
#define HILDON_TELEPHONE_EDITOR(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TELEPHONE_EDITOR_TYPE, \
  HildonTelephoneEditor))
#define HILDON_TELEPHONE_EDITOR_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TELEPHONE_EDITOR_TYPE, \
  HildonTelephoneEditorClass))
#define HILDON_IS_TELEPHONE_EDITOR(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TELEPHONE_EDITOR_TYPE))
#define HILDON_IS_TELEPHONE_EDITOR_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TELEPHONE_EDITOR_TYPE))
/**
 * HildonTelephoneEditorFormat:
 * @HILDON_TELEPHONE_EDITOR_FORMAT_FREE: Free format.
 * @HILDON_TELEPHONE_EDITOR_FORMAT_COERCE: Coerce format, three fields.
 * @HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY: Coerce format, only
 * country field.
 * @HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA: Coerce format, country
 * and area fields.
 * 
 * Define all possible format modes for the HildonTelephoneEditor.
 *
 */
    typedef enum {
    HILDON_TELEPHONE_EDITOR_FORMAT_FREE = 0,
    HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
    HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY,
    HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA
} HildonTelephoneEditorFormat;


/**
 * HildonTelephoneEditor:
 *
 * Internal struct for telephone editor.
 */
typedef struct _HildonTelephoneEditor HildonTelephoneEditor;
typedef struct _HildonTelephoneEditorClass HildonTelephoneEditorClass;

/*< private >*/
struct _HildonTelephoneEditor {
    GtkContainer parent;
};

struct _HildonTelephoneEditorClass {
    GtkContainerClass parent_class;
};

GType hildon_telephone_editor_get_type(void);

GtkWidget *hildon_telephone_editor_new(HildonTelephoneEditorFormat format);
GtkWidget
    *hildon_telephone_editor_new_with_strings(HildonTelephoneEditorFormat
                                              format,
                                              const gchar * country,
                                              const gchar * area,
                                              const gchar * number);
void hildon_telephone_editor_set_editable(HildonTelephoneEditor * hte,
                                          gboolean country, gboolean area,
                                          gboolean number);

void hildon_telephone_editor_set_show_plus(HildonTelephoneEditor * hte,
                                           gboolean show);

gboolean hildon_telephone_editor_get_show_plus(HildonTelephoneEditor *
                                               hte);

void hildon_telephone_editor_set_show_border(HildonTelephoneEditor * hte,
                                             gboolean show);

gboolean hildon_telephone_editor_get_show_border(HildonTelephoneEditor *
                                                 hte);


void hildon_telephone_editor_set_widths(HildonTelephoneEditor * hte,
                                        guint country,
                                        guint area, guint number);

const gchar *hildon_telephone_editor_get_country(HildonTelephoneEditor *
                                                 hte);

void hildon_telephone_editor_set_country(HildonTelephoneEditor * hte,
                                         const gchar * country);

const gchar *hildon_telephone_editor_get_area(HildonTelephoneEditor * hte);

void hildon_telephone_editor_set_area(HildonTelephoneEditor * hte,
                                      const gchar * area);

const gchar *hildon_telephone_editor_get_number(HildonTelephoneEditor *
                                                hte);

void hildon_telephone_editor_set_number(HildonTelephoneEditor * hte,
                                        const gchar * number);

G_END_DECLS
#endif /* __HILDON_TELEPHONE_EDITOR_H__ */
