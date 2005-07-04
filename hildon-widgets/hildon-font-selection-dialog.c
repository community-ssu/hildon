/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
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

/*
 * @file hildon-font-selection-dialog.c
 *
 * This file implements the HildonFontSelectionDialog widget
 *
 * A modification from the gtk_font_selection_dialog
*/

#include <stdlib.h>
#include <string.h>

#include <gtk/gtkstock.h>
#include <gtk/gtkcombobox.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtkcheckbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkliststore.h>
#include <gtk/gtknotebook.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "hildon-font-selection-dialog.h"
#include <hildon-widgets/hildon-caption.h>
#include <hildon-widgets/hildon-color-selector.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

#define PREVIEW_RESPONSE_ID 12345

/*
 * These are what we use as the standard font sizes, for the size list.
 */
static const guint16 font_sizes[] = {
    8, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 24, 26, 28,
    32, 36, 40, 48, 56, 64, 72
};

#define HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
        HILDON_TYPE_FONT_SELECTION_DIALOG, HildonFontSelectionDialogPrivate))

typedef struct
_HildonFontSelectionDialogPrivate HildonFontSelectionDialogPrivate;
struct _HildonFontSelectionDialogPrivate {
    GtkNotebook *notebook;
    GtkWidget *vbox_tab[3];
    GtkWidget *caption_control;
    GtkSizeGroup *group;

    gchar *preview_text;
    guint keysnooper;

/*Tab one*/
    GtkWidget *cbx_font_type;
    GtkWidget *cbx_font_size;
    GtkWidget *font_color_box;
    GtkWidget *font_color_button;
    GdkColor c_table;
    GtkWidget *e_table;

/*Tab two*/
    GtkWidget *chk_bold;
    GtkWidget *chk_italic;
    GtkWidget *chk_underline;

/*Tab three*/
    GtkWidget *chk_strikethrough;
    GtkWidget *cbx_positioning;

    gboolean both;      /* Most of the fonts has a bold-italic font type.
                           (Not just Bold and Italic BUT --> Bold-italic) */
/*	gboolean auto_resize;*/
/*	gint dialog_width;*/

    HildonPositioning positioning;      /* Current positioning */
    PangoFontFamily *family;    /* Current family */
    HildonFontFaceType face;    /* Current face number */
    gint size;  /* Current size */

/*Components for the preview dialog*/
    GtkWidget *dlg_preview;
    GtkWidget *lbl_preview;

/*Every family*/
    PangoFontFamily **families;
    gint n_families;
/*The faces for current family*/
    PangoFontFace **faces;
/*There's different names for the faces (Bold == Demi, etc.) -
  So we have "a layer" to determine the correct one*/
    HildonFontFaceType facetypes[4];
};

static void hildon_font_preview_dialog_close(GtkDialog * dialog,
                                             gint response, gpointer data);
static gboolean
hildon_font_selection_dialog_preview_key_press(GtkWidget * widget,
                                               GdkEventKey * event,
                                               gpointer data);

/*Some tools from gtk_font_selection*/
static int cmp_families(const void *a, const void *b);
static int faces_sort_func(const void *a, const void *b);
static int compare_font_descriptions(const PangoFontDescription * a,
                                     const PangoFontDescription * b); 
				     

static void 
hildon_font_selection_dialog_check_button_clicked(GtkToggleButton *chk,
		gpointer data);
static void
hildon_font_selection_dialog_show_preview(HildonFontSelectionDialog *
                                          fontsel);
static void
hildon_font_selection_dialog_set_font_for_preview(HildonFontSelectionDialog
                                                  * fontsel);
static PangoAttrList
    *hildon_font_selection_dialog_create_font(HildonFontSelectionDialog *
                                              fontsel);

static void
hildon_font_selection_dialog_show_available_positionings
(HildonFontSelectionDialog * fontsel);
static void
hildon_font_selection_dialog_show_available_fonts(HildonFontSelectionDialog
                                                  * fontseldiag);
static void
hildon_font_selection_dialog_show_available_styles
(HildonFontSelectionDialog * fontsel);
static void
hildon_font_selection_dialog_show_available_sizes(HildonFontSelectionDialog
                                                  * fontsel,
                                                  gboolean first_time);
static void
hildon_font_selection_dialog_class_init(HildonFontSelectionDialogClass *
                                        klass);
static void hildon_font_selection_dialog_init(HildonFontSelectionDialog *
                                              fontseldiag);
static void hildon_font_selection_dialog_finalize(GObject * object);

static void hildon_font_selection_dialog_preview_click(GtkDialog * dialog,
                                                       gint response,
                                                       gpointer data);
static void hildon_font_selection_dialog_font_type_changed(GtkComboBox *
                                                           cbox,
                                                           gpointer
                                                           userdata);
static void
hildon_font_selection_dialog_check_button_toggled(GtkToggleButton * chk,
                                                  gpointer user_data);

static void show_selector(GtkWidget * widget, GtkButton * b,
                          gpointer data);
static void set_event_box_color(GtkWidget * b, GdkColor * color);
static gboolean color_key_press(GtkWidget * widget, GdkEventKey * event,
                                gpointer data);
static gint hildon_font_selection_dialog_key_snooper(GtkWidget * swidget,
                                                     GdkEventKey * event,
                                                     GtkWidget * widget);

static GtkDialogClass *font_selection_dialog_parent_class = NULL;

/* property setter and getter */
static void hildon_font_selection_dialog_set_property(GObject * object,
		guint property_id,
		const GValue * value,
		GParamSpec * pspec);
static void hildon_font_selection_dialog_get_property(GObject * object,
		guint property_id,
		GValue * value,
		GParamSpec * pspec);

enum {
	PROP_SELECTOR_BOLD = 1,
	PROP_SELECTOR_ITALIC,	/* add all needed properties here */
	PROP_SELECTOR_UNDERLINE,
	PROP_SELECTOR_STRIKETHROUGH
};

enum {
	PROP_SET,
	PROP_INTERMEDIATE,
	PROP_DIMMED,
	PROP_UNSET
};


GType hildon_font_selection_dialog_get_type(void)
{
    static GType font_selection_dialog_type = 0;

    if (!font_selection_dialog_type) {
        static const GTypeInfo fontsel_diag_info = {
            sizeof(HildonFontSelectionDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_font_selection_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonFontSelectionDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_font_selection_dialog_init,
        };

        font_selection_dialog_type =
            g_type_register_static(GTK_TYPE_DIALOG,
                                   "HildonFontSelectionDialog",
                                   &fontsel_diag_info, 0);
    }

    return font_selection_dialog_type;
}

static void
hildon_font_selection_dialog_class_init(HildonFontSelectionDialogClass *
                                        klass)
{
    GObjectClass *gobject_class;

    font_selection_dialog_parent_class = g_type_class_peek_parent(klass);
    gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = hildon_font_selection_dialog_finalize;

    /* property stuff */
    gobject_class->set_property = hildon_font_selection_dialog_set_property;
    gobject_class->get_property = hildon_font_selection_dialog_get_property;

    g_object_class_install_property (gobject_class, PROP_SELECTOR_BOLD, 
		    g_param_spec_boolean ("is_bold_face", "BoldFace", 
                                    "Whether the text is boldface or not",
			    FALSE, G_PARAM_CONSTRUCT|G_PARAM_READWRITE));

    g_type_class_add_private(klass,
        sizeof(struct _HildonFontSelectionDialogPrivate));
}


static void hildon_font_selection_dialog_init(HildonFontSelectionDialog *
                                              fontseldiag)
{
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontseldiag);
    int i;
    for (i = 0; i < 4; ++i){
	    priv->facetypes[i] = -1;
    }
    
    priv->positioning = HILDON_POSITIONING_NORMAL;
    priv->size = 10;
    priv->face = 0;
    priv->families = NULL;
    priv->notebook = GTK_NOTEBOOK(gtk_notebook_new());

    for (i = 0; i < 3; i++)
        priv->vbox_tab[i] = gtk_vbox_new(FALSE, 0);

    priv->group =
        GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));

/*Tab one*/
    priv->cbx_font_type = gtk_combo_box_new_text();
    hildon_font_selection_dialog_show_available_fonts(fontseldiag);
    priv->caption_control = hildon_caption_new(priv->group,
                                               _("ecdg_fi_font_font"),
                                               priv->cbx_font_type,
                                               NULL,
                                               HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[0]), priv->caption_control,
                       FALSE, FALSE, 0);

    priv->cbx_font_size = gtk_combo_box_new_text();
    hildon_font_selection_dialog_show_available_sizes(fontseldiag, TRUE);
    priv->caption_control = hildon_caption_new(priv->group,
                                               _("ecdg_fi_font_size"),
                                               priv->cbx_font_size,
                                               NULL,
                                               HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[0]), priv->caption_control,
                       FALSE, FALSE, 0);

    priv->font_color_box = gtk_hbox_new(FALSE, 0);
    g_object_set(G_OBJECT(priv->font_color_box), "can-focus", TRUE, NULL);
    priv->font_color_button = gtk_event_box_new();
    g_object_set(G_OBJECT(priv->font_color_button), "can-focus", TRUE,
                 NULL);
    g_signal_connect_swapped(GTK_WIDGET(priv->font_color_button),
                             "button-release-event",
                             G_CALLBACK(show_selector),
                             (HildonFontSelectionDialog *) fontseldiag);
    g_signal_connect_swapped(GTK_WIDGET(priv->font_color_box),
                             "key-press-event",
                             G_CALLBACK(color_key_press), fontseldiag);
    gtk_box_pack_start(GTK_BOX(priv->font_color_box),
                       priv->font_color_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(priv->font_color_box), gtk_label_new(""),
                       TRUE, TRUE, 0);

    gtk_widget_set_size_request(GTK_WIDGET(priv->font_color_button), 20,
                                20);
    priv->c_table.red = 0;      /* (0,0,0) is black */
    priv->c_table.green = 0;
    priv->c_table.blue = 0;
    set_event_box_color(GTK_WIDGET(priv->font_color_button),
                        &priv->c_table);
    priv->caption_control =
        hildon_caption_new(priv->group, _("ecdg_fi_font_color_selector"),
                           priv->font_color_box,
                           NULL, HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[0]), priv->caption_control,
                       FALSE, FALSE, 0);

/*Tab two*/
    priv->chk_bold = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold), FALSE);
    priv->caption_control = hildon_caption_new(priv->group,
                                               _("ecdg_fi_font_bold"),
                                               priv->chk_bold,
                                               NULL,
                                               HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[1]), priv->caption_control,
                       FALSE, FALSE, 0);

    priv->chk_italic = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_italic),
                                 FALSE);
    priv->caption_control =
        hildon_caption_new(priv->group, _("ecdg_fi_font_italic"),
                           priv->chk_italic,
                           NULL, HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[1]), priv->caption_control,
                       FALSE, FALSE, 0);

    priv->chk_underline = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_underline),
                                 FALSE);
    priv->caption_control =
        hildon_caption_new(priv->group, _("ecdg_fi_font_underline"),
                           priv->chk_underline, NULL,
                           HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[1]), priv->caption_control,
                       FALSE, FALSE, 0);

    /* Sets bold and italic checkboxes */
    hildon_font_selection_dialog_show_available_styles(fontseldiag);

/*Tab three*/
    priv->chk_strikethrough = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                 (priv->chk_strikethrough), FALSE);
    priv->caption_control =
        hildon_caption_new(priv->group, _("ecdg_fi_font_strikethrough"),
                           priv->chk_strikethrough, NULL,
                           HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[2]), priv->caption_control,
                       FALSE, FALSE, 0);

    priv->cbx_positioning = gtk_combo_box_new_text();
    hildon_font_selection_dialog_show_available_positionings(fontseldiag);
    priv->caption_control =
        hildon_caption_new(priv->group, _("ecdg_fi_font_special"),
                           priv->cbx_positioning, NULL,
                           HILDON_CAPTION_OPTIONAL);
    gtk_box_pack_start(GTK_BOX(priv->vbox_tab[2]), priv->caption_control,
                       FALSE, FALSE, 0);

    /* Populate notebook */
    gtk_notebook_insert_page(priv->notebook, priv->vbox_tab[0], NULL, 0);
    gtk_notebook_insert_page(priv->notebook, priv->vbox_tab[1], NULL, 1);
    gtk_notebook_insert_page(priv->notebook, priv->vbox_tab[2], NULL, 2);
    gtk_notebook_set_tab_label_text(priv->notebook, priv->vbox_tab[0],
                                    _("ecdg_ti_font_dialog_style"));
    gtk_notebook_set_tab_label_text(priv->notebook, priv->vbox_tab[1],
                                    _("ecdg_ti_font_dialog_format"));
    gtk_notebook_set_tab_label_text(priv->notebook, priv->vbox_tab[2],
                                    _("ecdg_ti_font_dialog_other"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fontseldiag)->vbox),
                       GTK_WIDGET(priv->notebook), TRUE, TRUE, 0);
    gtk_widget_show(GTK_WIDGET(priv->notebook));

    /* Add dialog buttons */
    gtk_dialog_add_button(GTK_DIALOG(fontseldiag),
                          _("ecdg_bd_font_dialog_ok"),
                          GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(fontseldiag),
                          _("ecdg_bd_font_dialog_preview"),
                          PREVIEW_RESPONSE_ID);
    gtk_dialog_add_button(GTK_DIALOG(fontseldiag),
                          _("ecdg_bd_font_dialog_cancel"),
                          GTK_RESPONSE_CANCEL);

/*Set default preview text*/
    priv->preview_text = g_strdup(_("ecdg_fi_preview_font_preview_text"));

/*Set response-handler*/
    g_signal_connect(G_OBJECT(fontseldiag), "response",
                     G_CALLBACK
                     (hildon_font_selection_dialog_preview_click), NULL);

/*If we change the font, we have to check all the possible styles for it
  --> connecting signal*/
    g_signal_connect(G_OBJECT(priv->cbx_font_type), "changed",
                     G_CALLBACK
                     (hildon_font_selection_dialog_font_type_changed),
                     NULL);

/*If we have a font, where is Bold and italic styles,
  BUT the font does NOT have Bold-Italic style --> connecting signals*/
    g_signal_connect(G_OBJECT(priv->chk_bold), "clicked",
                     G_CALLBACK
                     (hildon_font_selection_dialog_check_button_clicked),
                     NULL);
    g_signal_connect(G_OBJECT(priv->chk_italic), "clicked",
                     G_CALLBACK
                     (hildon_font_selection_dialog_check_button_clicked),
                     NULL);

    g_signal_connect(G_OBJECT(priv->chk_underline), "clicked",
                     G_CALLBACK
                     (hildon_font_selection_dialog_check_button_clicked),
                     NULL);
    g_signal_connect(G_OBJECT(priv->chk_strikethrough), "clicked",
                     G_CALLBACK
                     (hildon_font_selection_dialog_check_button_clicked),
                     NULL);

    
    g_signal_connect(G_OBJECT(priv->chk_bold), "toggled",
                     G_CALLBACK
                     (hildon_font_selection_dialog_check_button_toggled),
                     NULL);
    g_signal_connect(G_OBJECT(priv->chk_italic), "toggled",
                     G_CALLBACK
                     (hildon_font_selection_dialog_check_button_toggled),
                     NULL);

/*Preview dialog init*/
    priv->dlg_preview =
        gtk_dialog_new_with_buttons(_("ecdg_ti_preview_font"), NULL,
                                    GTK_DIALOG_MODAL |
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_STOCK_OK,
                                    GTK_RESPONSE_ACCEPT,
                                    NULL);
    priv->lbl_preview = gtk_label_new(priv->preview_text);

    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(priv->dlg_preview)->vbox),
                      priv->lbl_preview);
    gtk_dialog_set_has_separator(GTK_DIALOG(priv->dlg_preview), FALSE);
    gtk_label_set_use_markup(GTK_LABEL(priv->lbl_preview), FALSE);
    gtk_label_set_use_underline(GTK_LABEL(priv->lbl_preview), FALSE);

/*Connect a signal to hide the preview dialog*/
    g_signal_connect(G_OBJECT(priv->dlg_preview), "response",
                     G_CALLBACK(hildon_font_preview_dialog_close),
                     fontseldiag);
/* set keypress handler (ESC hardkey) */
    g_signal_connect(G_OBJECT(priv->dlg_preview), "key-press-event",
                     G_CALLBACK
                     (hildon_font_selection_dialog_preview_key_press),
                     NULL);

    gtk_window_set_title(GTK_WINDOW(fontseldiag), _("ecdg_ti_font"));


    priv->keysnooper =
        (guint) (gtk_key_snooper_install(
            (GtkKeySnoopFunc) (hildon_font_selection_dialog_key_snooper),
            GTK_WIDGET (fontseldiag)));
    
    
    
    
    
    for (i = 0; i < 3; i++)
        gtk_widget_show_all(GTK_WIDGET(priv->vbox_tab[i]));
}

static void hildon_font_selection_dialog_set_property(GObject * object,
		guint property_id,
		const GValue * value,
		GParamSpec * pspec)
{
	gboolean bold;
	switch (property_id){
		case PROP_SELECTOR_BOLD: /* add other cases and handling of them */
			bold = g_value_get_boolean (value);	
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
			break;
	}
}

static void hildon_font_selection_dialog_get_property(GObject * object,
		guint property_id,
		GValue * value,
		GParamSpec * pspec)
{
	switch (property_id){
		case PROP_SELECTOR_BOLD:
			g_value_set_boolean(value, g_value_get_boolean(value)); 
			/* should set priv->bold? */
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
	}
}

static void hildon_font_preview_dialog_close(GtkDialog * dialog,
                                             gint response, gpointer data)
{
    HildonFontSelectionDialog *d;
    HildonFontSelectionDialogPrivate *priv;

    g_return_if_fail(HILDON_IS_FONT_SELECTION_DIALOG(data));
    d = HILDON_FONT_SELECTION_DIALOG(data);
    priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(d);
    gtk_widget_hide(GTK_WIDGET(dialog));
    gtk_window_present(GTK_WINDOW(data));
}

static void set_event_box_color(GtkWidget * b, GdkColor * color)
{
    GdkColor c = { 0, 32000, 32000, 32000 };

    g_return_if_fail(b);

    if (!color) {
        color = &c;
    }

    gtk_widget_modify_bg(b, GTK_STATE_NORMAL, color);
    gtk_widget_modify_bg(b, GTK_STATE_ACTIVE, color);
    gtk_widget_modify_bg(b, GTK_STATE_PRELIGHT, color);
    gtk_widget_modify_bg(b, GTK_STATE_SELECTED, color);
    gtk_widget_modify_bg(b, GTK_STATE_INSENSITIVE, color);
}

static gboolean color_key_press(GtkWidget * widget,
                                GdkEventKey * event, gpointer data)
{
    if (event->keyval == GDK_Return || event->keyval == GDK_KP_Enter)
        show_selector(widget, NULL, NULL);
    return FALSE;
}

static void show_selector(GtkWidget * widget, GtkButton * b, gpointer data)
{
    GtkWidget *selector;

    gint result;
    HildonFontSelectionDialogPrivate *priv;
    HildonFontSelectionDialog *fontseldiag;
    GdkColor *color;

    g_return_if_fail(widget);

    selector = hildon_color_selector_new(NULL);

    fontseldiag = HILDON_FONT_SELECTION_DIALOG(widget);
    priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontseldiag);
    hildon_color_selector_set_color(HILDON_COLOR_SELECTOR(selector),
                                    &priv->c_table);
    result = gtk_dialog_run(GTK_DIALOG(selector));

    color =
        hildon_color_selector_get_color(HILDON_COLOR_SELECTOR(selector));

    gtk_widget_destroy(selector);
    gtk_window_present(GTK_WINDOW(fontseldiag));

    if (result != GTK_RESPONSE_OK)
        return;

    priv->c_table = *color;
    set_event_box_color(GTK_WIDGET(priv->font_color_button),
                        &priv->c_table);
    gtk_widget_modify_fg(priv->lbl_preview, GTK_STATE_NORMAL,
                         &priv->c_table);
    gtk_widget_modify_fg(priv->lbl_preview, GTK_STATE_ACTIVE,
                         &priv->c_table);
    gtk_widget_modify_fg(priv->lbl_preview, GTK_STATE_PRELIGHT,
                         &priv->c_table);
    gtk_widget_modify_fg(priv->lbl_preview, GTK_STATE_SELECTED,
                         &priv->c_table);
    gtk_widget_modify_fg(priv->lbl_preview, GTK_STATE_INSENSITIVE,
                         &priv->c_table);

}

static void hildon_font_selection_dialog_finalize(GObject * object)
{
    HildonFontSelectionDialogPrivate *priv;
    HildonFontSelectionDialog *fontsel;

    g_return_if_fail(HILDON_IS_FONT_SELECTION_DIALOG(object));
    fontsel = HILDON_FONT_SELECTION_DIALOG(object);

    priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

    gtk_key_snooper_remove(priv->keysnooper);

    if (priv->faces)
        g_free(priv->faces);
    if (priv->preview_text)
        g_free(priv->preview_text);
    if (priv->families)
        g_free(priv->families);

    if (G_OBJECT_CLASS(font_selection_dialog_parent_class)->finalize)
        G_OBJECT_CLASS(font_selection_dialog_parent_class)->
            finalize(object);
}

static void 
hildon_font_selection_dialog_check_button_clicked(GtkToggleButton *chk,
		gpointer data){

	gboolean inconsistent = FALSE;

	g_object_get(G_OBJECT(chk), "inconsistent", &inconsistent, NULL);
	if (inconsistent)
		g_object_set(G_OBJECT(chk), "active", TRUE, "inconsistent", FALSE, NULL);
}

/*When we toggle either the bold or the italic togglebutton*/
static void
hildon_font_selection_dialog_check_button_toggled(GtkToggleButton * chk,
                                                  gpointer user_data)
{
    HildonFontSelectionDialog *fontsel;
    HildonFontSelectionDialogPrivate *priv;
    gboolean bold, italic;

    bold = italic = TRUE;

    fontsel =
        HILDON_FONT_SELECTION_DIALOG(gtk_widget_get_ancestor
                                     (GTK_WIDGET(chk),
                                      HILDON_TYPE_FONT_SELECTION_DIALOG));
    priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);
	
    if (priv->facetypes[HILDON_FONT_FACE_BOLD] == -1)
	    bold = FALSE;
    if (priv->facetypes[HILDON_FONT_FACE_ITALIC] == -1)
	    italic = FALSE;
    
    if (!priv->both) {
        if (GTK_WIDGET(chk) == priv->chk_bold) {
            if (gtk_toggle_button_get_active(chk)) {
                priv->face = HILDON_FONT_FACE_BOLD;
		gtk_widget_set_sensitive(priv->chk_italic, FALSE);
            } else {
                priv->face = HILDON_FONT_FACE_NORMAL;
		if (italic){
		       	gtk_widget_set_sensitive(priv->chk_italic, TRUE);
		}
            }
        } else {
            if (gtk_toggle_button_get_active(chk)) {
                priv->face = HILDON_FONT_FACE_ITALIC;
                gtk_widget_set_sensitive(priv->chk_bold, FALSE);
            } else {
                priv->face = HILDON_FONT_FACE_NORMAL;
                if (bold){
		       	gtk_widget_set_sensitive(priv->chk_bold, TRUE);
		}
            }
        }

    } else
        if (gtk_toggle_button_get_active
            (GTK_TOGGLE_BUTTON(priv->chk_bold)))
        if (gtk_toggle_button_get_active
            (GTK_TOGGLE_BUTTON(priv->chk_italic)))
            priv->face = HILDON_FONT_FACE_BOLD_ITALIC;
        else
            priv->face = HILDON_FONT_FACE_BOLD;
    else if (gtk_toggle_button_get_active
             (GTK_TOGGLE_BUTTON(priv->chk_italic)))
        priv->face = HILDON_FONT_FACE_ITALIC;
    else
        priv->face = HILDON_FONT_FACE_NORMAL;
}

/*Some gtk_fontsel general pango functions -- BEGIN*/

static int compare_font_descriptions(const PangoFontDescription * a,
                                     const PangoFontDescription * b)
{
    int val = strcmp(pango_font_description_get_family(a),
                     pango_font_description_get_family(b));

    if (val != 0)
        return val;

    if (pango_font_description_get_weight(a) !=
        pango_font_description_get_weight(b))
        return pango_font_description_get_weight(a) -
            pango_font_description_get_weight(b);

    if (pango_font_description_get_style(a) !=
        pango_font_description_get_style(b))
        return pango_font_description_get_style(a) -
            pango_font_description_get_style(b);

    if (pango_font_description_get_stretch(a) !=
        pango_font_description_get_stretch(b))
        return pango_font_description_get_stretch(a) -
            pango_font_description_get_stretch(b);

    if (pango_font_description_get_variant(a) !=
        pango_font_description_get_variant(b))
        return pango_font_description_get_variant(a) -
            pango_font_description_get_variant(b);

    return 0;
}
static int faces_sort_func(const void *a, const void *b)
{
    PangoFontDescription *desc_a;
    PangoFontDescription *desc_b;
    int ord;

    desc_a = pango_font_face_describe(*(PangoFontFace **) a);
    desc_b = pango_font_face_describe(*(PangoFontFace **) b);
    ord = compare_font_descriptions(desc_a, desc_b);

    pango_font_description_free(desc_a);
    pango_font_description_free(desc_b);

    return ord;
}
static int cmp_families(const void *a, const void *b)
{
    const char *a_name =
        pango_font_family_get_name(*(PangoFontFamily **) a);
    const char *b_name =
        pango_font_family_get_name(*(PangoFontFamily **) b);

    return g_utf8_collate(a_name, b_name);
}

/*The general pango functions -- END*/

/*Run only once in the init*/
static void hildon_font_selection_dialog_preview_click(GtkDialog * dialog,
                                                       gint response,
                                                       gpointer data)
{
    if (response == PREVIEW_RESPONSE_ID) {
        g_signal_stop_emission_by_name(G_OBJECT(dialog), "response");
        hildon_font_selection_dialog_show_preview
            (HILDON_FONT_SELECTION_DIALOG(dialog));
    }
}

static gboolean
hildon_font_selection_dialog_preview_key_press(GtkWidget * widget,
                                               GdkEventKey * event,
                                               gpointer data)
{
    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(event, FALSE);

    if (event->keyval == GDK_Escape) {
        gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_CANCEL);
        return TRUE;
    }

    return FALSE;
}

/*If the font type is changed -> update the components
  (... We may not have a sensitive bold or/and italic toggle button)*/
static void hildon_font_selection_dialog_font_type_changed(GtkComboBox *
                                                           cbox,
                                                           gpointer
                                                           userdata)
{
    gint i;
    GtkWidget *fontsel =
        gtk_widget_get_ancestor(GTK_WIDGET(cbox),
                                HILDON_TYPE_FONT_SELECTION_DIALOG);
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE
        (HILDON_FONT_SELECTION_DIALOG(fontsel));
    priv->family = priv->families[gtk_combo_box_get_active(cbox)];
    for (i = 0; i < 4; ++i) {
	    priv->facetypes[i] = -1;
    }
    hildon_font_selection_dialog_show_available_styles
        (HILDON_FONT_SELECTION_DIALOG(fontsel));
}


static PangoAttrList
    *hildon_font_selection_dialog_create_font(HildonFontSelectionDialog *
                                              fontsel)
{
    PangoFontDescription *font_desc;
    PangoAttrList *list;
    PangoAttribute *attr;
    int len;
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

/*Setting size to a temporar variable*/
    priv->size =
        font_sizes[gtk_combo_box_get_active
                   (GTK_COMBO_BOX(priv->cbx_font_size))];

    list = pango_attr_list_new();
    gtk_label_set_text(GTK_LABEL(priv->lbl_preview), priv->preview_text);
    len = strlen(priv->preview_text);

/*Create a font_desc attribute for the font*/

    font_desc = pango_font_face_describe(priv->faces[priv->facetypes[priv->face]]);
    pango_font_description_set_size(font_desc, priv->size * PANGO_SCALE);
    pango_font_description_set_family(font_desc,
                                      pango_font_family_get_name(priv->
                                                                 family));

    attr = pango_attr_font_desc_new(font_desc);
    attr->start_index = 0;
    attr->end_index = len;
    pango_attr_list_insert(list, attr);

/*Create an underline attribute for the font*/
    if (gtk_toggle_button_get_active
        (GTK_TOGGLE_BUTTON(priv->chk_underline))) {
        attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
        attr->start_index = 0;
        attr->end_index = len;
        pango_attr_list_insert(list, attr);
    }

/*Create a striketrough attribute for the font*/
    if (gtk_toggle_button_get_active
        (GTK_TOGGLE_BUTTON(priv->chk_strikethrough))) {
        attr = pango_attr_strikethrough_new(TRUE);
        attr->start_index = 0;
        attr->end_index = len;
        pango_attr_list_insert(list, attr);
    }

/*Create a positioning attribute for the font*/
    attr = NULL;
    switch (gtk_combo_box_get_active
            (GTK_COMBO_BOX(priv->cbx_positioning))) {
    case HILDON_POSITIONING_SUPER:
        attr = pango_attr_rise_new(priv->size * (PANGO_SCALE / 2));
        break;
    case HILDON_POSITIONING_SUB:
        attr = pango_attr_rise_new(-priv->size * (PANGO_SCALE / 2));
        break;
    };

    if (attr) {
        attr->start_index = 0;
        attr->end_index = len;
        pango_attr_list_insert(list, attr);
    }
/*Create a color attribute for the font*/
    attr =
        pango_attr_foreground_new(priv->c_table.red, priv->c_table.green,
                                  priv->c_table.blue);
    attr->start_index = 0;
    attr->end_index = len;
    pango_attr_list_insert(list, attr);

    return list;
}

static void
hildon_font_selection_dialog_set_font_for_preview(HildonFontSelectionDialog
                                                  * fsd)
{
/*set the attribute list for the preview label*/
    HildonFontSelectionDialogPrivate *priv;
    PangoAttrList *attr = hildon_font_selection_dialog_create_font(fsd);

    priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
    gtk_label_set_attributes(GTK_LABEL(priv->lbl_preview), attr);
    pango_attr_list_unref(attr);
}

/*When the user clicks the preview button*/
static void
hildon_font_selection_dialog_show_preview(HildonFontSelectionDialog *
                                          fontsel)
{
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

/*Set the font*/
    hildon_font_selection_dialog_set_font_for_preview(fontsel);

/*Set a correct size for the label and for the window*/
/*First we minimize the window, if we resize from larger font to smaller*/
    gtk_window_resize(GTK_WINDOW(priv->dlg_preview), 1, 1);
    gtk_widget_set_size_request(priv->lbl_preview, -1, priv->size*4);

/*And show the dialog*/
    gtk_widget_show_all(priv->dlg_preview);
    gtk_dialog_run(GTK_DIALOG(priv->dlg_preview));
    gtk_widget_hide(priv->dlg_preview);
}


/*Run only once in the init --> Collect all the available fonts*/
static void
hildon_font_selection_dialog_show_available_fonts(HildonFontSelectionDialog
                                                  * fontsel)
{
    PangoFontFamily *match_family = NULL;
    gint i, match_row;
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);
    match_row = 0;

    pango_context_list_families(gtk_widget_get_pango_context
                                (GTK_WIDGET(fontsel)), &priv->families,
                                &priv->n_families);
    qsort(priv->families, priv->n_families, sizeof(PangoFontFamily *),
          cmp_families);


    for (i = 0; i < priv->n_families; i++) {
        const gchar *name = pango_font_family_get_name(priv->families[i]);

        gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_font_type),
                                  name);

        if (i == 0 || !g_ascii_strcasecmp(name, "sans")) {
            match_family = priv->families[i];
            match_row = i;
        }
    }

    priv->family = match_family;
    gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_type),
                             match_row);
}


static void
hildon_font_selection_dialog_show_available_positionings
(HildonFontSelectionDialog * fontsel)
{
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

    gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_positioning),
                              _("ecdg_va_font_printpos_1"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_positioning),
                              _("ecdg_va_font_printpos_2"));
    gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_positioning),
                              _("ecdg_va_font_printpos_3"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_positioning), 0);
}


/*Run after the hildon_font_selection_dialog_show_available_fonts
  -function*/
static void
hildon_font_selection_dialog_show_available_styles
(HildonFontSelectionDialog * fontsel)
{
    gint n_faces, i;
    PangoFontFace **faces;
    gchar *familyname;
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);
    faces = priv->faces;

    if (faces)
        g_free(faces);

    familyname = (gchar *)pango_font_family_get_name (priv->family);
	    
/*Fetch and sort faces*/
    pango_font_family_list_faces(priv->family, &faces, &n_faces);

    qsort(faces, n_faces, sizeof(PangoFontFace *), faces_sort_func); 

    priv->faces = faces;

/*Set the checkbuttons to false ->
  If we have italic or/and bold, we'll set that to true*/
    gtk_widget_set_sensitive(priv->chk_bold, FALSE);
    gtk_widget_set_sensitive(priv->chk_italic, FALSE);

    priv->both = FALSE;

/*Check all the faces we have for the selected font type*/
    for (i = 0; i < n_faces; i++) {
        const gchar *str = pango_font_face_get_face_name(faces[i]);
	
        
	if (!(strcmp(str, "Bold Italic")) || !(strcmp(str, "Demi Italic"))
            || !(strcmp(str, "Bold Oblique"))
            || !(strcmp(str, "Demi Bold Italic"))) {
            priv->facetypes[HILDON_FONT_FACE_BOLD_ITALIC] = i;
            gtk_widget_set_sensitive(priv->chk_bold, TRUE);
            gtk_widget_set_sensitive(priv->chk_italic, TRUE);
            priv->both = TRUE;
            i = n_faces;        /* IHOPE -- Regular/Normal (what ever name 
                                   it has) is assumed to be first one */
        } else if (g_str_has_prefix(str, "Bold")
                   || g_str_has_prefix(str, "Demi")) {
            priv->facetypes[HILDON_FONT_FACE_BOLD] = i;
            gtk_widget_set_sensitive(priv->chk_bold, TRUE);
        } else if (g_str_has_suffix(str, "Italic") ||
                   g_str_has_suffix(str, "Oblique"))
        {
            priv->facetypes[HILDON_FONT_FACE_ITALIC] = i;
            gtk_widget_set_sensitive(priv->chk_italic, TRUE);
        } 	
	else if (i == 0)  {    /* First one is Regular, Normal, etc. */
            priv->facetypes[HILDON_FONT_FACE_NORMAL] = i;
	}
    }

/*If we had only one legal face (Regular), both are same ->
  change both to TRUE*/
    if (i == 1)
        priv->both = TRUE;

/*And then.. If a chk_button is not sensitive -> un_check it*/
    if (!GTK_WIDGET_IS_SENSITIVE(priv->chk_bold))
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold),
                                     FALSE);

    if (!GTK_WIDGET_IS_SENSITIVE(priv->chk_italic))
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_italic),
                                     FALSE);

    if (priv->both) {
        if (!GTK_WIDGET_IS_SENSITIVE(priv->chk_bold))
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                         (priv->chk_italic), FALSE);
        else if (!GTK_WIDGET_IS_SENSITIVE(priv->chk_italic))
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold),
                                         FALSE);
    }
}


/*Loads the sizes from a pre-allocated table*/
static void
hildon_font_selection_dialog_show_available_sizes(HildonFontSelectionDialog
                                                  * fontsel,
                                                  gboolean first_time)
{
    gint i;
    GtkListStore *model;
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

    if (first_time) {
        model = gtk_list_store_new(1, G_TYPE_INT);
        gtk_combo_box_set_model(GTK_COMBO_BOX(priv->cbx_font_size),
                                GTK_TREE_MODEL(model));
        for (i = 0; i < G_N_ELEMENTS(font_sizes); i++) {
            GtkTreeIter iter;

            gtk_list_store_append(model, &iter);
            /* 0 == First column in the combobox */
            gtk_list_store_set(model, &iter, 0, font_sizes[i], -1);

            if (i == 0 || font_sizes[i] * PANGO_SCALE == priv->size)
                gtk_combo_box_set_active_iter(GTK_COMBO_BOX
                                              (priv->cbx_font_size),
                                              &iter);
        }
    }
}

/*******************/
/*Public functions*/
/*******************/

/**
 * hildon_font_selection_dialog_new:
 * @parent: the parent window.
 * @title: the title of font selection dialog.
 *
 * if the title is left to %NULL, the title will be left
 * to DEFAULT == "Font".
 *
 * Return value: a new #HildonFontSelectionDialog.
 */
GtkWidget *hildon_font_selection_dialog_new(GtkWindow * parent,
                                            const gchar * title)
{
    HildonFontSelectionDialog *fontseldiag;

    fontseldiag = g_object_new(HILDON_TYPE_FONT_SELECTION_DIALOG,
                               "has-separator", FALSE, NULL);

    if (title)
        gtk_window_set_title(GTK_WINDOW(fontseldiag), title);

    if (parent)
        gtk_window_set_transient_for(GTK_WINDOW(fontseldiag), parent);

    return GTK_WIDGET(fontseldiag);
}

/**
 * hildon_font_selection_dialog_get_preview_text:
 * @fsd: the font selection dialog.
 *
 * Gets the text in preview dialog.
 * The returned string must be freed by the user.
 *
 * Returns: a string pointer.
 */
gchar *
hildon_font_selection_dialog_get_preview_text(HildonFontSelectionDialog * fsd)
{
    HildonFontSelectionDialogPrivate *priv;

    g_return_val_if_fail(HILDON_IS_FONT_SELECTION_DIALOG(fsd), FALSE);
    priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
    return g_strdup(priv->preview_text);
}

/**
 * hildon_font_selection_dialog_set_preview_text:
 * @fsd: the font selection dialog.
 * @text: the text to be set to the preview dialog.
 *
 * Sets the text to the preview dialog.
 * DEFAULT == "The quick brown fox jumped over the lazy dogs"
 */
void
hildon_font_selection_dialog_set_preview_text(HildonFontSelectionDialog *
                                              fsd, const gchar * text)
{
    if (HILDON_IS_FONT_SELECTION_DIALOG(fsd)) ;
    {
        HildonFontSelectionDialogPrivate *priv =
            HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
        g_free(priv->preview_text);
        priv->preview_text = g_strdup(text);
    }
}

/**
 * hildon_font_selection_dialog_get_text_tag:
 * @fsd: the font selection dialog.
 *
 * Get the #GtkTextTag for selections.
 * 
 * Returns: a #GtkTextTag representing the choises selected (eg. font name, font size, bolding, italic, underlinening etc.)
 */ 
GtkTextTag * hildon_font_selection_dialog_get_text_tag (
		HildonFontSelectionDialog *fsd)
{
	GtkTextTag *tag = NULL;
	gboolean bold, italic, strikethrough, underline;
	gint i=-1, fontsize = 0, rise=0;
	gchar *fontname;
	HildonFontSelectionDialogPrivate *priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
	bold=italic=strikethrough=underline=FALSE;

	g_object_get(G_OBJECT(priv->chk_bold), "active", &bold, NULL);
	g_object_get(G_OBJECT(priv->chk_italic), "active", &italic, NULL);
	g_object_get(G_OBJECT(priv->chk_underline), "active", &underline, NULL);
	g_object_get(G_OBJECT(priv->chk_strikethrough), "active", &strikethrough, NULL);
	
	i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_font_type));
	fontname = (gchar *) pango_font_family_get_name(priv->families[i]);
	
	fontsize = font_sizes[gtk_combo_box_get_active (GTK_COMBO_BOX(priv->cbx_font_size))];

	switch (gtk_combo_box_get_active (GTK_COMBO_BOX(priv->cbx_positioning))) {
		case HILDON_POSITIONING_SUPER:
		     rise = (priv->size * (PANGO_SCALE / 2));
		     break;
		case HILDON_POSITIONING_SUB:
		     rise = (-priv->size * (PANGO_SCALE / 2));
	             break;
		case HILDON_POSITIONING_NORMAL:
	    	     rise = 0; break;
	}; 
	
        tag = gtk_text_tag_new(NULL);
	if (fontname)
		g_object_set(G_OBJECT(tag), "font", fontname, NULL);             	
	g_object_set(G_OBJECT(tag), "size", fontsize*PANGO_SCALE, NULL);	 	
	if (bold)
		g_object_set(G_OBJECT(tag), "weight", PANGO_WEIGHT_BOLD, NULL);
	if (italic)
		g_object_set(G_OBJECT(tag), "style", PANGO_STYLE_ITALIC, NULL);
	if (underline)
		g_object_set(G_OBJECT(tag), "underline", TRUE, NULL);
	if (strikethrough)
		g_object_set(G_OBJECT(tag), "strikethrough", TRUE, NULL);
	if (rise)
		g_object_set(G_OBJECT(tag), "rise", rise, NULL);
	
	g_object_set(G_OBJECT(tag), "foreground-gdk", &priv->c_table, NULL);
	return tag;
}

/** 
 * hildon_font_selection_dialog_set_buffer:
 * @fsd: the font selection dialog.
 * @buffer: a #GtkTextBuffer containing the text to which the selections will be applied. Applying is responsibility of application.
 *
 * Sets the textbuffer under editing. The dialog will look what selections (bolding/italic/font/fontsize etc.) are present in the 
 * buffer text and make these selections be default in the dialog.
 * 
 */
void hildon_font_selection_dialog_set_buffer (		 
		HildonFontSelectionDialog *fsd,
		GtkTextBuffer *buffer)
{
	GtkTextIter begin, end;
	GtkStyle *wstyle = NULL; 
	gint size = 0, i;
	gchar *font = NULL;	
	gboolean bold_on, bold_off, italic_on, italic_off, underline_on, underline_off, 
		 strike_through_off, strike_through_on, superscript_on, superscript_off, subscript_on, subscript_off;
	HildonFontSelectionDialogPrivate *priv;

	priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
	
	/* bold_on and bold_off and the likes are used to recognize conflicts so that the checkbuttons can be put to 'inconsistent' state 
	 * NOTE: they are set initially to TRUE! */
	bold_on = bold_off = italic_on = italic_off = underline_on = underline_off = TRUE;
	strike_through_off = strike_through_on = superscript_on = superscript_off = subscript_on = subscript_off = TRUE;

	/* go through the selection */
	gtk_text_buffer_get_selection_bounds (buffer, &begin, &end);
	priv->preview_text = gtk_text_buffer_get_slice(buffer, &begin, &end, FALSE);

	if (!gtk_text_iter_equal(&begin, &end)) /* this fails if no selection */
	{
		gboolean tags_found = FALSE; /* this is needed so we know we need to get the default font */
		GtkTextIter iter = begin;
		
		/* loop letter by letter */
		while (!gtk_text_iter_equal(&iter, &end))
		{
			GSList *tags;
			gboolean bset, iset, stset, ulset, rset, sset, fgset; /* these are flags for whether the property is set or not */
			
			gboolean strike_through, underline; /* these are the */
			gint weight=0, rise=0;		    /* values of the */
			PangoStyle style;		    /* properties of one tag */
			
			gboolean b, i, st, ul, r, l, s, c; /* these booleans tell whether or not there was at least one tags in the
								  tags list with the corresponding property set */
			
			GdkColor *color;
			b = i = st = ul = r = l =s= c=FALSE;
			size = 0;
			bset=iset=strike_through=underline=stset=ulset=rset=FALSE; /* all are assumed to FALSE */
			
			tags = gtk_text_iter_get_tags(&iter); /* TODO: free the list! */
			if (tags) tags_found=TRUE;
			while (tags){
				/* TODO: positioning?? foreground color?? */

				g_object_get(G_OBJECT(tags->data), 
						"weight-set", &bset, 
						"style-set", &iset,
						"strikethrough-set", &stset,
						"underline-set", &ulset,
						"rise-set", &rset,
						"family", &font,
						"foreground-set", &fgset, 
						"size-set", &sset, 
						NULL);
				
				if (bset){
					g_object_get(G_OBJECT(tags->data), "weight", &weight, NULL);
			       		if (weight > PANGO_WEIGHT_NORMAL)	
						b = TRUE;
				}
				if (iset){
					g_object_get(G_OBJECT(tags->data), "style", &style, NULL);
					if (style == PANGO_STYLE_ITALIC)
						i = TRUE;
				}
				if (stset){
					g_object_get(G_OBJECT(tags->data), "strikethrough", &strike_through, NULL);
					if (strike_through)
						st = TRUE;
				}
				if (ulset){
					g_object_get(G_OBJECT(tags->data), "underline", &underline, NULL);
					if (underline)
						ul = TRUE;
				}
				if (rset){
					g_object_get(G_OBJECT(tags->data), "rise", &rise, NULL);
					if (rise > 0) r=TRUE; 
					else if (rise<0) l=TRUE; 
				}			
				if (sset){
					g_object_get(G_OBJECT(tags->data), "size", &size, NULL);
					s = TRUE;
				}
				if (fgset){
					g_object_get(G_OBJECT(tags->data), "foreground-gdk", &color, NULL);
					c = TRUE;
				}
								
				tags = tags->next;
			}
			if (!b) bold_on = FALSE; /* if no tag in the tag list for this letter had bolding then bold_on should be false
						    the idea is that if the next (or some letter later has bolding the we have
						    bold_on = bolf_off = FALSE and thus we're in inconsistent state */
			else bold_off = FALSE;
			if (!i) italic_on = FALSE;
			else italic_off = FALSE;
			if (!st) strike_through_on = FALSE;
			else strike_through_off = FALSE;
			if (!ul) underline_on = FALSE;
			else underline_off = FALSE;
			if (!r) superscript_on = FALSE;
			else superscript_off = FALSE;
			if (!l) subscript_on = FALSE;
			else subscript_off = FALSE;
			
			if (!bold_on && !bold_off){
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_bold), 
						"inconsistent", TRUE, 
						NULL);
			}
			else {
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_bold), 
						"inconsistent", FALSE,
						"active", bold_on, 
						NULL);
				if (!priv->both) gtk_widget_set_sensitive (GTK_WIDGET(priv->chk_italic), FALSE);
			}
			if (!italic_on && !italic_off){
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_italic), 
						"inconsistent", TRUE, 
						NULL);
			}
			else {
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_italic), 
						"inconsistent", FALSE,
						"active", italic_on, 
						NULL);
				if (!priv->both) gtk_widget_set_sensitive (GTK_WIDGET(priv->chk_bold), FALSE);
			}
			if (!underline_on && !underline_off){
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_underline), 
						"inconsistent", TRUE, 
						NULL);
			}
			else {
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_underline), 
						"inconsistent", FALSE,
						"active", underline_on,              			
						NULL);
			}
			if (!strike_through_on && !strike_through_off) {
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_strikethrough),
						"inconsistent", TRUE,
						NULL);
			}
			else{
				g_object_set(GTK_TOGGLE_BUTTON(priv->chk_strikethrough),
						"inconsistent", FALSE,
						"active", strike_through_on,
						NULL);
			}
			if ((!superscript_on && !superscript_off)||(!subscript_on && !subscript_off));
			else{
				if (superscript_on)
					g_object_set(G_OBJECT(priv->cbx_positioning), "active", 1, NULL);
				else if (subscript_on)
					g_object_set(G_OBJECT(priv->cbx_positioning), "active", 2, NULL);
				else 
					g_object_set(G_OBJECT(priv->cbx_positioning), "active", 0, NULL);
			}
			if (c){
				priv->c_table = *color;
				set_event_box_color(priv->font_color_button, color);
			}
			if (!s){
				wstyle = gtk_rc_get_style_by_paths(gtk_settings_get_default(), NULL, NULL, GTK_TYPE_TEXT_VIEW);
				size = pango_font_description_get_size(wstyle->font_desc);
				wstyle = NULL;
			}
			gtk_text_iter_forward_cursor_position(&iter);
			
		}
		if (!tags_found){
			wstyle = gtk_rc_get_style_by_paths(gtk_settings_get_default(), NULL, NULL, GTK_TYPE_TEXT_VIEW);
		}
	}
	else wstyle = gtk_rc_get_style_by_paths(gtk_settings_get_default(), NULL, NULL, GTK_TYPE_TEXT_VIEW);
	if (wstyle)                                                         		
	{                                                                               	
		font = (gchar *) pango_font_description_get_family(wstyle->font_desc);	
		size = pango_font_description_get_size(wstyle->font_desc);  	
	}

	for (i = 0; i < G_N_ELEMENTS(font_sizes); i++) {
		if ((font_sizes[i]*PANGO_SCALE)==size)
			gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_size), i);
	}
	for (i = 0; i < priv->n_families; i++) {
		const gchar *name = pango_font_family_get_name(priv->families[i]);
		if (font && name && !strcmp(font, name))
			gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_type), i);
	}

}		

/**
 * hildon_font_selection_dialog_get_font:
 * @fsd: the font selection dialog.
 *
 * Gets the font from the dialog.
 *
 * Return value: pointer to @PangoAttrList.
 */
PangoAttrList
    *hildon_font_selection_dialog_get_font(HildonFontSelectionDialog * fsd)
{
    g_return_val_if_fail(HILDON_IS_FONT_SELECTION_DIALOG(fsd), FALSE);
    return hildon_font_selection_dialog_create_font(fsd);
}

/**
 * hildon_font_selection_dialog_set_font:
 * @fsd: the font selection dialog.
 * @list: the pango attribute list.
 *
 * Sets the font to the dialog.
 */
void hildon_font_selection_dialog_set_font(HildonFontSelectionDialog * fsd,
                                           PangoAttrList * list)
{
    if (HILDON_IS_FONT_SELECTION_DIALOG(fsd)) {
        gint i, tmp;
        gchar *str;
        PangoAttribute *attr;
        PangoFontDescription *desc;
        gboolean is_italic, is_bold;
        PangoAttrIterator *iter;
        HildonFontSelectionDialogPrivate *priv =
            HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
        iter = pango_attr_list_get_iterator(list);
        attr = pango_attr_iterator_get(iter, PANGO_ATTR_FONT_DESC);
        desc = ((PangoAttrFontDesc *) attr)->desc;
        str = (gchar *) g_strdup(pango_font_description_get_family(desc));

        /* Set family */
        for (i = 0; i < priv->n_families; i++) {
            if (g_ascii_strcasecmp
                (pango_font_family_get_name(priv->families[i]), str) == 0) {
                priv->family = priv->families[i];
                gtk_combo_box_set_active(GTK_COMBO_BOX
                                         (priv->cbx_font_type), i);
                i = priv->n_families;
            }
        }

        is_italic = is_bold = FALSE;

        /* If it's not normal -> it's italic */
        if (pango_font_description_get_style(desc) != PANGO_STYLE_NORMAL) {
            is_italic = TRUE;
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                         (priv->chk_italic), TRUE);
        } else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                         (priv->chk_italic), FALSE);

        if (pango_font_description_get_weight(desc) > PANGO_WEIGHT_NORMAL) {
            is_bold = TRUE;
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold),
                                         TRUE);
        } else
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold),
                                         FALSE);

        if (is_bold) {
            if (is_italic)
                priv->face = HILDON_FONT_FACE_BOLD_ITALIC;
            else
                priv->face = HILDON_FONT_FACE_BOLD;
        } else if (is_italic)
            priv->face = HILDON_FONT_FACE_ITALIC;
        else
            priv->face = HILDON_FONT_FACE_NORMAL;

        /* Set size */
        priv->size = pango_font_description_get_size(desc);
        tmp = priv->size / PANGO_SCALE;
        for (i = 0; i < G_N_ELEMENTS(font_sizes); i++) {
            if (tmp == font_sizes[i]) {
                gtk_combo_box_set_active(GTK_COMBO_BOX
                                         (priv->cbx_font_size), i);
                i = G_N_ELEMENTS(font_sizes) + 1;
            }
        }
        /* If we went trough the whole for-loop -> set the size to 10 */
        if (i == G_N_ELEMENTS(font_sizes))
            gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_size),
                                     10);

        /* Set underline */
        attr = pango_attr_iterator_get(iter, PANGO_ATTR_UNDERLINE);
        if (attr) {
            if (((PangoAttrInt *) attr)->value)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_underline), TRUE);
            else
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_underline), FALSE);
        }
        /* Set striketrought */
        attr = pango_attr_iterator_get(iter, PANGO_ATTR_STRIKETHROUGH);
        if (attr) {
            if (((PangoAttrInt *) attr)->value)
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_strikethrough),
                                             TRUE);
            else
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_strikethrough),
                                             FALSE);
        }

        /* Set positioning */
        attr = pango_attr_iterator_get(iter, PANGO_ATTR_RISE);
        if (attr) {
            if (((PangoAttrInt *) attr)->value > 0)
                gtk_combo_box_set_active(GTK_COMBO_BOX
                                         (priv->cbx_positioning),
                                         HILDON_POSITIONING_SUPER);
            else if (((PangoAttrInt *)
                      pango_attr_iterator_get(iter,
                                              PANGO_ATTR_RISE))->value < 0)
                gtk_combo_box_set_active(GTK_COMBO_BOX
                                         (priv->cbx_positioning),
                                         HILDON_POSITIONING_SUB);
        }

        /* Set color */
        attr = pango_attr_iterator_get(iter, PANGO_ATTR_FOREGROUND);
        if (attr) {
            priv->c_table.red =
                (guint) (((PangoAttrColor *) attr)->color).red;
            priv->c_table.green =
                (guint) (((PangoAttrColor *) attr)->color).green;
            priv->c_table.blue =
                (guint) (((PangoAttrColor *) attr)->color).blue;
            set_event_box_color(GTK_WIDGET(priv->font_color_button),
                                &priv->c_table);
        }
        pango_attr_iterator_destroy(iter);
    }
}

static gint hildon_font_selection_dialog_key_snooper(GtkWidget * swidget,
                                                     GdkEventKey * event,
                                                     GtkWidget * widget)
{
    HildonFontSelectionDialog *dialog =
        HILDON_FONT_SELECTION_DIALOG(widget);
    HildonFontSelectionDialogPrivate *priv =
        HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(dialog);
    if (event->type == GDK_KEY_RELEASE)
        switch (event->keyval) {
        case GDK_P:
        case GDK_p:
            gtk_dialog_response(GTK_DIALOG(widget), PREVIEW_RESPONSE_ID);
            break;
        case GDK_B:
        case GDK_b:
        {
            gboolean state =
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_bold));
            if (GTK_WIDGET_IS_SENSITIVE (priv->chk_bold))
              gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold),
                                           state ? FALSE : TRUE);
            break;
        }
        case GDK_I:
        case GDK_i:
        {
            gboolean state =
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_italic));
            if (GTK_WIDGET_IS_SENSITIVE (priv->chk_italic))
              gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                           (priv->chk_italic),
                                           state ? FALSE : TRUE);
            break;
        }
        case GDK_U:
        case GDK_u:
        {
            gboolean state =
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
                                             (priv->chk_underline));
            if (GTK_WIDGET_IS_SENSITIVE (priv->chk_underline))
              gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                           (priv->chk_underline),
                                           state ? FALSE : TRUE);
            break;
        }
        }
    return FALSE;
}
