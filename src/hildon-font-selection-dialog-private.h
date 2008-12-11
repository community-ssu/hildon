/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
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

#ifndef                                         __HILDON_FONT_SELECTION_DIALOG_PRIVATE_H__
#define                                         __HILDON_FONT_SELECTION_DIALOG_PRIVATE_H__

G_BEGIN_DECLS

#define                                         HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(obj) \
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
			                        HILDON_TYPE_FONT_SELECTION_DIALOG, \
			                        HildonFontSelectionDialogPrivate))

typedef struct                                  _HildonFontSelectionDialogPrivate HildonFontSelectionDialogPrivate;

struct                                          _HildonFontSelectionDialogPrivate 
{  
    GtkNotebook *notebook;

    gchar *preview_text;

    /* Tab one */
    GtkWidget *cbx_font_type;
    GtkWidget *cbx_font_size;
    GtkWidget *font_color_button;

    /* Tab two */
    GtkWidget *chk_bold;
    GtkWidget *chk_italic;
    GtkWidget *chk_underline;

    /* Tab three */
    GtkWidget *chk_strikethrough;
    GtkWidget *cbx_positioning;

    /* Every family */
    PangoFontFamily **families;
    gint n_families;

    /* color_set is used to show whether the color is inconsistent
     * The handler id is used to block the signal emission
     * when we change the color setting */
    gboolean color_set;

    /* font_scaling is the scaling factor applied to font
     * scale in the preview dialog */
    gdouble font_scaling;

    gulong color_modified_signal_handler;
};

G_END_DECLS

#endif                                          /* __HILDON_FONT_SELECTION_DIALOG_PRIVATE_H__ */
