/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation.
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

/**
 * SECTION:hildon-font-selection-dialog
 * @short_description: A widget used to allow users to select a font 
 * with certain properties
 *
 * Font selection can be made using this widget. Users can select font name, 
 * size, style, etc. Users can also preview text in the selected font.
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
#include <gtk/gtk.h>
#include <glib.h>
#include <gdk/gdkkeysyms.h>

#include "hildon-font-selection-dialog.h"
#include <hildon-widgets/hildon-caption.h>
#include <hildon-widgets/hildon-color-selector.h>
#include <hildon-widgets/hildon-color-button.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#define _(String) dgettext(PACKAGE, String)

#define SUPERSCRIPT_RISE 3333
#define SUBSCRIPT_LOW   -3333
#define ON_BIT  0x01
#define OFF_BIT 0x02

/*
 * These are what we use as the standard font sizes, for the size list.
 */
static const guint16 font_sizes[] = 
{
  6, 8, 10, 12, 16, 24, 32
};

#define HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(obj) \
(G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
			      HILDON_TYPE_FONT_SELECTION_DIALOG, \
			      HildonFontSelectionDialogPrivate))

/*None of designed api function works, so now it all comes down to 
 *use properties to achieve what we are supposed to achieve*/
enum
{
  PROP_FAMILY = 1,
  PROP_FAMILY_SET,
  PROP_SIZE,
  PROP_SIZE_SET,
  PROP_COLOR,
  PROP_COLOR_SET,
  PROP_BOLD,
  PROP_BOLD_SET,
  PROP_ITALIC,
  PROP_ITALIC_SET,
  PROP_UNDERLINE,
  PROP_UNDERLINE_SET,
  PROP_STRIKETHROUGH,
  PROP_STRIKETHROUGH_SET,
  PROP_POSITION,
  PROP_POSITION_SET,
  PROP_PREVIEW_TEXT,
  PROP_FONT_SCALING
};

typedef struct
_HildonFontSelectionDialogPrivate HildonFontSelectionDialogPrivate;

struct _HildonFontSelectionDialogPrivate 
{  
  GtkNotebook *notebook;

  gchar *preview_text;

  /*Tab one*/
  GtkWidget *cbx_font_type;
  GtkWidget *cbx_font_size;
  GtkWidget *font_color_button;

  /*Tab two*/
  GtkWidget *chk_bold;
  GtkWidget *chk_italic;
  GtkWidget *chk_underline;

  /*Tab three*/
  GtkWidget *chk_strikethrough;
  GtkWidget *cbx_positioning;

  /*Every family*/
  PangoFontFamily **families;
  gint n_families;

  /*color_set is used to show whether the color is inconsistent
   * The handler id is used to block the signal emission
   * when we change the color setting*/
  
  gboolean color_set;

  /* font_scaling is the scaling factor applied to font
   * scale in the preview dialog */

  gdouble font_scaling;
  gulong color_modified_signal_handler;
};

/*combo box active row indicator -2--inconsistent, -1--undefined 
 * please make sure that you use settings_init settings_apply
 * and settings_destroy, dont even try to touch this structure 
 * without using the three above interface functions, of course
 * if you know what you are doing, do as you please ;-)*/
typedef struct
{
  HildonFontSelectionDialog
               *fsd; /*pointer to our font selection dialog*/
  
  gint         family; /*combo box indicator*/
  gint         size; /*combo box indicator*/
  GdkColor     *color; /*free after read the setting*/
  gboolean     color_inconsist;
  gint         weight; /*bit mask*/
  gint         style;  /*bit mask*/
  gint         underline; /*bit mask*/
  gint         strikethrough; /*bit mask*/
  gint         position; /*combo box indicator*/

}HildonFontSelectionDialogSettings;

static gboolean
              hildon_font_selection_dialog_preview_key_press
                                            (GtkWidget * widget,
                                             GdkEventKey * event,
                                             gpointer unused);

/*Some tools from gtk_font_selection*/
static int    cmp_families                   (const void *a, const void *b);

static void   hildon_font_selection_dialog_show_preview
                                             (HildonFontSelectionDialog 
				              *fontsel);
					     
static PangoAttrList*
              hildon_font_selection_dialog_create_attrlist
	                                     (HildonFontSelectionDialog 
					      *fontsel, guint start_index,
					      guint len);

static void   hildon_font_selection_dialog_show_available_positionings
                                             (HildonFontSelectionDialogPrivate
					      *priv);
						 
static void   hildon_font_selection_dialog_show_available_fonts
                                             (HildonFontSelectionDialog
				              *fontsel);
						 
static void   hildon_font_selection_dialog_show_available_sizes
                                             (HildonFontSelectionDialogPrivate
					      *priv);

static void   hildon_font_selection_dialog_class_init
                                             (HildonFontSelectionDialogClass 
					      *klass);
						 
static void   hildon_font_selection_dialog_init
                                             (HildonFontSelectionDialog 
					      *fontseldiag);

static void   hildon_font_selection_dialog_finalize
                                             (GObject * object);

static void   hildon_font_selection_dialog_construct_notebook  
                                             (HildonFontSelectionDialog
				              *fontsel);
					     
static void   color_modified_cb              (HildonColorButton *button,
					      GParamSpec *pspec,
					      gpointer fsd_priv);

static void   check_tags                     (gpointer data,
					      gpointer user_data);

static void   settings_init                  (HildonFontSelectionDialogSettings
					      *setttings,
					      HildonFontSelectionDialog
					      *fsd);

static void   settings_apply                 (HildonFontSelectionDialogSettings
					      *setttings);

static void   settings_destroy               (HildonFontSelectionDialogSettings
					      *setttings);

static void   bit_mask_toggle                (gint mask, GtkToggleButton*
					      button, GObject *object, 
					      const gchar *prop, 
					      const gchar *prop_set);

static void   combo_active                   (gint active, GtkComboBox *box,
					      GObject *object, 
					      const gchar *prop,
					      const gchar *prop_set);

static void   add_preview_text_attr          (PangoAttrList *list, 
					      PangoAttribute *attr, 
					      guint start, 
					      guint len);

static void   toggle_clicked                 (GtkButton *button, 
					      gpointer unused);
	
					     
					     
static GtkDialogClass *font_selection_dialog_parent_class = NULL;

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
hildon_font_selection_dialog_get_property (GObject      *object,
					   guint         prop_id,
					   GValue       *value,
					   GParamSpec   *pspec)
{
  gint i;
  GdkColor *color = NULL;
  
  HildonFontSelectionDialogPrivate *priv =
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(
	HILDON_FONT_SELECTION_DIALOG(object));
  
  
  switch (prop_id)
    {
    case PROP_FAMILY:
      i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_font_type));
      if(i >= 0 && i < priv->n_families)
	g_value_set_string(value, 
			   pango_font_family_get_name(priv->families[i]));
      else
	g_value_set_string(value, "Sans");
      break;
      
    case PROP_FAMILY_SET:
      i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_font_type));
      if(i >= 0 && i < priv->n_families)
	g_value_set_boolean(value, TRUE);
      else
	g_value_set_boolean(value, FALSE);
      break;
      
    case PROP_SIZE:
      i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_font_size));
      if(i >= 0 && i < G_N_ELEMENTS(font_sizes))
	g_value_set_int(value, font_sizes[i]);
      else
	g_value_set_int(value, 16);
      break;
      
    case PROP_SIZE_SET:
      i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_font_size));
      if(i >= 0 && i < G_N_ELEMENTS(font_sizes))
	g_value_set_boolean(value, TRUE);
      else
	g_value_set_boolean(value, FALSE);
      break;

    case PROP_COLOR:
      color = hildon_color_button_get_color
	(HILDON_COLOR_BUTTON(priv->font_color_button));
      g_value_set_boxed(value, (gconstpointer) color);
      if(color != NULL)
	gdk_color_free(color);
      break;
      
    case PROP_COLOR_SET:
      g_value_set_boolean(value, priv->color_set);
      break;

    case PROP_BOLD:
      g_value_set_boolean(value, 
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(priv->chk_bold)));
      break;

    case PROP_BOLD_SET:
      g_value_set_boolean(value,
	!gtk_toggle_button_get_inconsistent
	(GTK_TOGGLE_BUTTON(priv->chk_bold)));
      break;
      
    case PROP_ITALIC:
      g_value_set_boolean(value, 
	gtk_toggle_button_get_active
	(GTK_TOGGLE_BUTTON(priv->chk_italic)));
      break;

    case PROP_ITALIC_SET:
      g_value_set_boolean(value,
	!gtk_toggle_button_get_inconsistent
	(GTK_TOGGLE_BUTTON(priv->chk_italic)));
      break;
      
    case PROP_UNDERLINE:
      g_value_set_boolean(value, 
	gtk_toggle_button_get_active
	(GTK_TOGGLE_BUTTON(priv->chk_underline)));
      break;

    case PROP_UNDERLINE_SET:
      g_value_set_boolean(value,
	!gtk_toggle_button_get_inconsistent
	(GTK_TOGGLE_BUTTON(priv->chk_underline)));
      break;
      
    case PROP_STRIKETHROUGH:
      g_value_set_boolean(value, 
	gtk_toggle_button_get_active
	(GTK_TOGGLE_BUTTON(priv->chk_strikethrough)));
      break;

    case PROP_STRIKETHROUGH_SET:
      g_value_set_boolean(value,
	!gtk_toggle_button_get_inconsistent
	(GTK_TOGGLE_BUTTON(priv->chk_strikethrough)));
      break;

    case PROP_POSITION:
      i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_positioning));
      if(i == 1)/*super*/
	g_value_set_int(value, 1);
      else if(i == 2)/*sub*/
	g_value_set_int(value, -1);
      else
	g_value_set_int(value, 0);
      break;
    
    case PROP_FONT_SCALING:
	g_value_set_double(value, priv->font_scaling);
      break;
  
    case PROP_POSITION_SET:
      i = gtk_combo_box_get_active(GTK_COMBO_BOX(priv->cbx_positioning));
      if(i >= 0 && i < 3)
	g_value_set_boolean(value, TRUE);
      else
	g_value_set_boolean(value, FALSE);
      break;
    
    case PROP_PREVIEW_TEXT:
	g_value_set_string(value,
		hildon_font_selection_dialog_get_preview_text(
		    HILDON_FONT_SELECTION_DIALOG(object)));
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void 
hildon_font_selection_dialog_set_property (GObject         *object,
					   guint            prop_id,
					   const GValue    *value,
					   GParamSpec      *pspec)
{
  gint i, size;
  const gchar *str;
  gboolean b;
  GdkColor *color = NULL;
  GdkColor black;
  
  HildonFontSelectionDialogPrivate *priv =
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(
	HILDON_FONT_SELECTION_DIALOG(object));
  black.red = black.green = black.blue = 0;
  
  switch (prop_id)
    {
    case PROP_FAMILY:
      str = g_value_get_string(value);
      for(i = 0; i < priv->n_families; i++)
	{
	  if(strcmp(str, pango_font_family_get_name(priv->families[i]))
	     == 0)
	    {
	      gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_type), i);
	      break;
	    }
	}
      break;
      
    case PROP_FAMILY_SET:
      b = g_value_get_boolean(value);
      if(!b)
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_type), -1);
      break;
    
    case PROP_SIZE:
      size = g_value_get_int(value);
      for(i = 0; i < G_N_ELEMENTS(font_sizes); i++)
	{
	  if(size == font_sizes[i])
	    {
	      gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_size), i);
	      break;
	    }
	}
      break;
      
    case PROP_SIZE_SET:
      b = g_value_get_boolean(value);
      if(!b)
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_font_size), -1);
      break;

    case PROP_COLOR:
      color = (GdkColor *) g_value_get_boxed(value);
      if(color != NULL)
	hildon_color_button_set_color(HILDON_COLOR_BUTTON
				      (priv->font_color_button),
				      color);
      else
	hildon_color_button_set_color(HILDON_COLOR_BUTTON
				      (priv->font_color_button),
				      &black);
      break;

    case PROP_COLOR_SET:
      priv->color_set = g_value_get_boolean(value);
      if(!priv->color_set)
	{
	  /*set color to black, but block our signal handler*/
	  g_signal_handler_block((gpointer) priv->font_color_button,
				 priv->color_modified_signal_handler);
	  
	  hildon_color_button_set_color(HILDON_COLOR_BUTTON
					(priv->font_color_button), 
					&black);
	  
	  g_signal_handler_unblock((gpointer) priv->font_color_button,
				 priv->color_modified_signal_handler);
	}
      break;

    case PROP_BOLD:
      /*this call will make sure that we dont get extra clicked signal*/
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(priv->chk_bold),
					 FALSE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_bold),
				   g_value_get_boolean(value));
      break;

    case PROP_BOLD_SET:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(priv->chk_bold),
					 !g_value_get_boolean(value));
      break;
      
    case PROP_ITALIC:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(priv->chk_italic),
					 FALSE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_italic),
				   g_value_get_boolean(value));
      break;

    case PROP_ITALIC_SET:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(priv->chk_italic),
					 !g_value_get_boolean(value));
      break;
      
    case PROP_UNDERLINE:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON
					 (priv->chk_underline),
					 FALSE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_underline),
				   g_value_get_boolean(value));
      break;

    case PROP_UNDERLINE_SET:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON(priv->chk_underline),
					 !g_value_get_boolean(value));
      break;
  
    case PROP_STRIKETHROUGH:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON
					 (priv->chk_strikethrough),
					 FALSE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->chk_strikethrough),
				   g_value_get_boolean(value));
      break;

    case PROP_STRIKETHROUGH_SET:
      gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON
					 (priv->chk_strikethrough),
					 !g_value_get_boolean(value));
      break;

    case PROP_POSITION:
      i = g_value_get_int(value);
      if( i == 1 )
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_positioning), 1);
      else if(i == -1)
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_positioning), 2);
      else
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_positioning), 0);
      break;

    case PROP_FONT_SCALING:
      priv->font_scaling = g_value_get_double(value);
      break;
      
    case PROP_POSITION_SET:
      b = g_value_get_boolean(value);
      if(!b)
	gtk_combo_box_set_active(GTK_COMBO_BOX(priv->cbx_positioning), -1);
      break;

    case PROP_PREVIEW_TEXT:
      hildon_font_selection_dialog_set_preview_text(
	      HILDON_FONT_SELECTION_DIALOG(object),
	      g_value_get_string(value));
      break;
    
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
hildon_font_selection_dialog_class_init(HildonFontSelectionDialogClass *
					klass)
{
  GObjectClass *gobject_class;

  font_selection_dialog_parent_class = g_type_class_peek_parent(klass);
  gobject_class = G_OBJECT_CLASS(klass);
  gobject_class->finalize = hildon_font_selection_dialog_finalize;
  gobject_class->get_property = hildon_font_selection_dialog_get_property;
  gobject_class->set_property = hildon_font_selection_dialog_set_property;

  /* Install property to the class */
  g_object_class_install_property(gobject_class, PROP_FAMILY,
				  g_param_spec_string("family",
				  "Font family", "String defines"
				  " the font family", "Sans",
				  G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_FAMILY_SET,
				  g_param_spec_boolean ("family-set",
				  "family inconsistent state",
				  "Whether the family property"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_SIZE,
				   g_param_spec_int ("size",
				   "Font size",
				   "Font size in Pt",
				   6, 32, 16,
				   G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_SIZE_SET,
				  g_param_spec_boolean ("size-set",
				  "size inconsistent state",
				  "Whether the size property"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_COLOR,
				  g_param_spec_boxed ("color",
				  "text color",
				  "gdk color for the text",
				  GDK_TYPE_COLOR,
				  G_PARAM_READWRITE));

  g_object_class_install_property(gobject_class, PROP_COLOR_SET,
				  g_param_spec_boolean ("color-set",
				  "color inconsistent state",
				  "Whether the color property"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));

  g_object_class_install_property(gobject_class, PROP_BOLD,
				  g_param_spec_boolean ("bold",
				  "text weight",
				  "Whether the text is bold",
				  FALSE,
				  G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_BOLD_SET,
				  g_param_spec_boolean ("bold-set",
				  "bold inconsistent state",
				  "Whether the bold"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_ITALIC,
				  g_param_spec_boolean ("italic",
				  "text style",
				  "Whether the text is italic",
				  FALSE,
				  G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_ITALIC_SET,
				  g_param_spec_boolean ("italic-set",
				  "italic inconsistent state",
				  "Whether the italic"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_UNDERLINE,
				  g_param_spec_boolean ("underline",
				  "text underline",
				  "Whether the text is underlined",
				  FALSE,
				  G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_UNDERLINE_SET,
				  g_param_spec_boolean ("underline-set",
				  "underline inconsistent state",
				  "Whether the underline"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_STRIKETHROUGH,
				  g_param_spec_boolean ("strikethrough",
				  "strikethroughed text",
				  "Whether the text is strikethroughed",
				  FALSE,
				  G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_STRIKETHROUGH_SET,
				  g_param_spec_boolean ("strikethrough-set",
				  "strikethrough inconsistent state",
				  "Whether the strikethrough"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_POSITION,
				   g_param_spec_int ("position",
				   "Font position",
				   "Font position super or subscript",
				   -1, 1, 0,
				   G_PARAM_READWRITE));

  /* FIXME This was introduced in 0.14.1. We don't have documentation for 
   * properties anyways, but once it's there it needs to be marked as 
   * Since: 0.14.1 */
  g_object_class_install_property(gobject_class, PROP_FONT_SCALING,
				   g_param_spec_double ("font-scaling",
				   "Font scaling",
				   "Font scaling for the preview dialog",
				   0, 10, 1,
				   G_PARAM_READWRITE));
  
  g_object_class_install_property(gobject_class, PROP_POSITION_SET,
				  g_param_spec_boolean ("position-set",
				  "position inconsistent state",
				  "Whether the position"
				  " is inconsistent", FALSE,
				  G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  
  g_object_class_install_property(gobject_class, PROP_PREVIEW_TEXT,
				  g_param_spec_string("preview-text",
				  "Preview Text", 
				  "the text in preview dialog, which does" 
				  "not include the reference text",
				  "",
				  G_PARAM_READWRITE));
  

  g_type_class_add_private(klass,
			   sizeof(struct _HildonFontSelectionDialogPrivate));
}


static void 
hildon_font_selection_dialog_init(HildonFontSelectionDialog *fontseldiag)
{
  HildonFontSelectionDialogPrivate *priv =
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontseldiag);
  GtkWidget *preview_button;
  
  priv->notebook = GTK_NOTEBOOK(gtk_notebook_new());

  hildon_font_selection_dialog_construct_notebook(fontseldiag);
  
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fontseldiag)->vbox),
		     GTK_WIDGET(priv->notebook), TRUE, TRUE, 0);
  
  /* Add dialog buttons */
  gtk_dialog_add_button(GTK_DIALOG(fontseldiag),
			_("ecdg_bd_font_dialog_ok"),
			GTK_RESPONSE_OK);
  
  preview_button = gtk_button_new_with_label(_("ecdg_bd_font_dialog_preview"));
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(fontseldiag)->action_area), 
		     preview_button, FALSE, TRUE, 0);
  g_signal_connect_swapped(preview_button, "clicked",
			   G_CALLBACK
			   (hildon_font_selection_dialog_show_preview),
			   fontseldiag);
  gtk_widget_show(preview_button);

  gtk_dialog_add_button(GTK_DIALOG(fontseldiag),
			_("ecdg_bd_font_dialog_cancel"),
			GTK_RESPONSE_CANCEL);

  /*Set default preview text*/
  priv->preview_text = g_strdup(_("ecdg_fi_preview_font_preview_text"));

  gtk_window_set_title(GTK_WINDOW(fontseldiag), _("ecdg_ti_font"));
  /*here is the line to make sure that notebook has the default focus*/
  gtk_container_set_focus_child(GTK_CONTAINER(GTK_DIALOG(fontseldiag)->vbox),
				GTK_WIDGET(priv->notebook));
}

static void 
hildon_font_selection_dialog_construct_notebook (HildonFontSelectionDialog
				                 *fontsel)
{
  gint i;
  GtkWidget *vbox_tab[3];
  GtkWidget *font_color_box;
  GtkWidget *caption_control;
  GtkSizeGroup *group;
  
  HildonFontSelectionDialogPrivate *priv =
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

  for (i = 0; i < 3; i++)
    vbox_tab[i] = gtk_vbox_new(TRUE, 0);

  group =
    GTK_SIZE_GROUP(gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL));
  
  /* Build the first page of the GtkNotebook: font style */
  priv->cbx_font_type = gtk_combo_box_new_text();
  hildon_font_selection_dialog_show_available_fonts(fontsel);
  caption_control = hildon_caption_new(group,
				       _("ecdg_fi_font_font"),
				       priv->cbx_font_type,
				       NULL,
				       HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[0]), caption_control,
		     FALSE, FALSE, 0);

  priv->cbx_font_size = gtk_combo_box_new_text();
  hildon_font_selection_dialog_show_available_sizes(priv);
  caption_control = hildon_caption_new(group,
				       _("ecdg_fi_font_size"),
				       priv->cbx_font_size,
				       NULL,
				       HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[0]), caption_control,
		     FALSE, FALSE, 0);

  font_color_box = gtk_hbox_new(FALSE, 0);
  priv->font_color_button = hildon_color_button_new();
  priv->color_set = FALSE;
  priv->font_scaling = 1.0;
  priv->color_modified_signal_handler = 
    g_signal_connect(G_OBJECT(priv->font_color_button), "notify::color",
		     G_CALLBACK(color_modified_cb), (gpointer) priv);
  gtk_box_pack_start(GTK_BOX(font_color_box),
		     priv->font_color_button, FALSE, FALSE, 0);
  
  caption_control =
    hildon_caption_new(group, _("ecdg_fi_font_colour_selector"),
		       font_color_box,
		       NULL, HILDON_CAPTION_OPTIONAL);
  
  gtk_box_pack_start(GTK_BOX(vbox_tab[0]), caption_control,
		     FALSE, FALSE, 0);

  /* Build the second page of the GtkNotebook: font formatting */ 
  priv->chk_bold = gtk_check_button_new();
  caption_control = hildon_caption_new(group,
				       _("ecdg_fi_font_bold"),
				       priv->chk_bold,
				       NULL,
				       HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[1]), caption_control,
		     FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(priv->chk_bold), "clicked", 
		   G_CALLBACK(toggle_clicked), NULL);

  priv->chk_italic = gtk_check_button_new();
  caption_control =
    hildon_caption_new(group, _("ecdg_fi_font_italic"),
		       priv->chk_italic,
		       NULL, HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[1]), caption_control,
		     FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(priv->chk_italic), "clicked", 
		   G_CALLBACK(toggle_clicked), NULL);

  priv->chk_underline = gtk_check_button_new();
  caption_control =
    hildon_caption_new(group, _("ecdg_fi_font_underline"),
		       priv->chk_underline, NULL,
		       HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[1]), caption_control,
		     FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(priv->chk_underline), "clicked", 
		   G_CALLBACK(toggle_clicked), NULL);

  /* Build the third page of the GtkNotebook: other font properties */
  priv->chk_strikethrough = gtk_check_button_new();
  caption_control =
    hildon_caption_new(group, _("ecdg_fi_font_strikethrough"),
		       priv->chk_strikethrough, NULL,
		       HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[2]), caption_control,
		     FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(priv->chk_strikethrough), "clicked", 
		   G_CALLBACK(toggle_clicked), NULL);

  priv->cbx_positioning = gtk_combo_box_new_text();
  hildon_font_selection_dialog_show_available_positionings(priv);
  caption_control =
    hildon_caption_new(group, _("ecdg_fi_font_special"),
		       priv->cbx_positioning, NULL,
		       HILDON_CAPTION_OPTIONAL);
  gtk_box_pack_start(GTK_BOX(vbox_tab[2]), caption_control,
		     FALSE, FALSE, 0);
  
  /* Populate notebook */
  gtk_notebook_insert_page(priv->notebook, vbox_tab[0], NULL, 0);
  gtk_notebook_insert_page(priv->notebook, vbox_tab[1], NULL, 1);
  gtk_notebook_insert_page(priv->notebook, vbox_tab[2], NULL, 2);
  gtk_notebook_set_tab_label_text(priv->notebook, vbox_tab[0],
				  _("ecdg_ti_font_dialog_style"));
  gtk_notebook_set_tab_label_text(priv->notebook, vbox_tab[1],
				  _("ecdg_ti_font_dialog_format"));
  gtk_notebook_set_tab_label_text(priv->notebook, vbox_tab[2],
				  _("ecdg_ti_font_dialog_other"));
  
  gtk_widget_show_all(GTK_WIDGET(priv->notebook));
}

static void 
color_modified_cb(HildonColorButton *button, 
                  GParamSpec *pspec, 
                  gpointer fsd_priv)
{
  HildonFontSelectionDialogPrivate *priv = 
          (HildonFontSelectionDialogPrivate *) fsd_priv;

  priv->color_set = TRUE;
}

static void 
hildon_font_selection_dialog_finalize(GObject * object)
{
  HildonFontSelectionDialogPrivate *priv;
  HildonFontSelectionDialog *fontsel;

  g_assert(HILDON_IS_FONT_SELECTION_DIALOG(object));
  fontsel = HILDON_FONT_SELECTION_DIALOG(object);

  priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);
  
  g_free(priv->preview_text);
  g_free(priv->families);

  if (G_OBJECT_CLASS(font_selection_dialog_parent_class)->finalize)
    G_OBJECT_CLASS(font_selection_dialog_parent_class)->finalize(object);
}

static int 
cmp_families(const void *a, const void *b)
{
  const char *a_name =
    pango_font_family_get_name(*(PangoFontFamily **) a);
  const char *b_name =
    pango_font_family_get_name(*(PangoFontFamily **) b);

  return g_utf8_collate(a_name, b_name);
}

/* Exits the preview dialog with GTK_RESPONSE_CANCEL if Esc key
 * was pressed */
static gboolean
hildon_font_selection_dialog_preview_key_press(GtkWidget   * widget,
                                               GdkEventKey * event,
                                               gpointer      unused)
{
  g_assert(widget);
  g_assert(event);
  
  if (event->keyval == GDK_Escape)
    {
      gtk_dialog_response(GTK_DIALOG(widget), GTK_RESPONSE_CANCEL);
      return TRUE;
    }

  return FALSE;
}

static void
add_preview_text_attr(PangoAttrList *list, PangoAttribute *attr, 
		      guint start, guint len)
{
  attr->start_index = start;
  attr->end_index = start + len;
  pango_attr_list_insert(list, attr);
}

static PangoAttrList*
hildon_font_selection_dialog_create_attrlist(HildonFontSelectionDialog *
					 fontsel, guint start_index, guint len)
{
  PangoAttrList *list;
  PangoAttribute *attr;
  gint size, position;
  gboolean family_set, size_set, color_set, bold, bold_set,
           italic, italic_set, underline, underline_set,
	   strikethrough, strikethrough_set, position_set;
  GdkColor *color = NULL;
  gchar *family = NULL;
  gdouble font_scaling = 1.0;

  list = pango_attr_list_new();
 
  g_object_get(G_OBJECT(fontsel),
	       "family", &family, "family-set", &family_set,
	       "size", &size, "size-set", &size_set,
	       "color", &color, "color-set", &color_set,
	       "bold", &bold, "bold-set", &bold_set,
	       "italic", &italic, "italic-set", &italic_set,
	       "underline", &underline, "underline-set", &underline_set,
	       "strikethrough", &strikethrough, "strikethrough-set", 
	       &strikethrough_set, "position", &position, 
	       "position-set", &position_set, 
               "font-scaling", &font_scaling,
               NULL);

  /*family*/
  if(family_set)
    {
      attr = pango_attr_family_new(family);
      add_preview_text_attr(list, attr, start_index, len);
    }
  g_free(family);
  
  /*size*/
  if(size_set)
    {
      attr = pango_attr_size_new(size * PANGO_SCALE);
      add_preview_text_attr(list, attr, start_index, len);
    }
  
  /*color*/
  if(color_set)
    {
      attr = pango_attr_foreground_new(color->red, color->green, color->blue);
      add_preview_text_attr(list, attr, start_index, len);
    }
  
  if(color != NULL)
    gdk_color_free(color);
  
  /*weight*/
  if(bold_set)
    {
      if(bold)
        attr = pango_attr_weight_new(PANGO_WEIGHT_BOLD);
      else
	attr = pango_attr_weight_new(PANGO_WEIGHT_NORMAL);

      add_preview_text_attr(list, attr, start_index, len);
    }
  
  /*style*/
  if(italic_set)
    {
      if(italic)
        attr = pango_attr_style_new(PANGO_STYLE_ITALIC);
      else
	attr = pango_attr_style_new(PANGO_STYLE_NORMAL);

      add_preview_text_attr(list, attr, start_index, len);
    }
  
  /*underline*/
  if(underline_set)
    {
      if(underline)
        attr = pango_attr_underline_new(PANGO_UNDERLINE_SINGLE);
      else
	attr = pango_attr_underline_new(PANGO_UNDERLINE_NONE);

      add_preview_text_attr(list, attr, start_index, len);
    }
  
  /*strikethrough*/
  if(strikethrough_set)
    {
      if(strikethrough)
        attr = pango_attr_strikethrough_new(TRUE);
      else
	attr = pango_attr_strikethrough_new(FALSE);

      add_preview_text_attr(list, attr, start_index, len);
    }
  
  /*position*/
  if(position_set)
    {
      switch(position)
	{
	case 1: /*super*/
	  attr = pango_attr_rise_new(SUPERSCRIPT_RISE);
	  break;
	case -1: /*sub*/
	  attr = pango_attr_rise_new(SUBSCRIPT_LOW);
	  break;
	default: /*normal*/
	  attr = pango_attr_rise_new(0);
	  break;
	}

      add_preview_text_attr(list, attr, start_index, len);
    }

  /*font scaling for preview*/
  if(font_scaling)
    {
      attr = pango_attr_scale_new(font_scaling);
      add_preview_text_attr(list, attr, 0, len + start_index);
    }
   
  return list;
}

static void
hildon_font_selection_dialog_show_preview(HildonFontSelectionDialog *
					  fontsel)
{
  HildonFontSelectionDialogPrivate *priv =
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);
  gint size;
  gboolean family_set, size_set;
  PangoAttribute *attr;
  PangoAttrList *list;
  GtkWidget *preview_dialog;
  GtkWidget *preview_label;
  gchar *str = NULL;
  
  /*Preview dialog init*/
  preview_dialog=
    gtk_dialog_new_with_buttons(_("ecdg_ti_preview_font"), NULL,
				GTK_DIALOG_MODAL |
				GTK_DIALOG_DESTROY_WITH_PARENT |
				GTK_DIALOG_NO_SEPARATOR,
				_("ecdg_bd_font_dialog_ok"),
				GTK_RESPONSE_ACCEPT,
				NULL);

  str = g_strconcat(_("ecdg_fi_preview_font_preview_reference"),
                    priv->preview_text, 0);

  preview_label = gtk_label_new(str);
  gtk_label_set_line_wrap(GTK_LABEL(preview_label), TRUE);

  g_free(str);
  str = NULL;

  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(preview_dialog)->vbox),
		    preview_label);

  
  /* set keypress handler (ESC hardkey) */
  g_signal_connect(G_OBJECT(preview_dialog), "key-press-event",
                  G_CALLBACK(hildon_font_selection_dialog_preview_key_press),
                  NULL);
  
  
  /*Set the font*/
  list = hildon_font_selection_dialog_create_attrlist(fontsel, 
				strlen(_("ecdg_fi_preview_font_preview_reference")),
				strlen(priv->preview_text));

  g_object_get(G_OBJECT(fontsel), "family", &str, "family-set",
	       &family_set, "size", &size, "size-set", &size_set,
	       NULL);
  /*make reference text to have the same fontface and size*/
  if(family_set)
    {
      attr = pango_attr_family_new(str);
      add_preview_text_attr(list, attr, 0, strlen(_("ecdg_fi_preview_font_preview_reference")));
    }
  g_free(str);
  
  /*size*/
  if(size_set)
    {
      attr = pango_attr_size_new(size * PANGO_SCALE);
      add_preview_text_attr(list, attr, 0, strlen(_("ecdg_fi_preview_font_preview_reference")));
    }
  
  gtk_label_set_attributes(GTK_LABEL(preview_label), list);
  pango_attr_list_unref(list);
  
  /*And show the dialog*/
  gtk_window_set_transient_for(GTK_WINDOW(preview_dialog), 
			       GTK_WINDOW(fontsel));
  gtk_widget_show_all(preview_dialog);
  gtk_dialog_run(GTK_DIALOG(preview_dialog));
  gtk_widget_destroy(preview_dialog);
}


static gboolean is_internal_font(const gchar * name){
  return strcmp(name, "DeviceSymbols") == 0
      || strcmp(name, "Nokia Smiley" ) == 0
      || strcmp(name, "NewCourier" ) == 0
      || strcmp(name, "NewTimes" ) == 0
      || strcmp(name, "SwissA" ) == 0
      || strcmp(name, "Nokia Sans"   ) == 0
      || strcmp(name, "Nokia Sans Cn") == 0;
}

static void filter_out_internal_fonts(PangoFontFamily **families, int *n_families){
  int i;
  int n; /* counts valid fonts */
  const gchar * name = NULL;

  for(i = 0, n = 0; i < *n_families; i++){

    name = pango_font_family_get_name(families[i]);
        
    if(!is_internal_font(name)){

      if(i!=n){ /* there are filtered out families */
	families[n] = families[i]; /* shift the current family */
      }

      n++; /* count one more valid */
    }
  }/* foreach font family */

  *n_families = n;  
}


static void
hildon_font_selection_dialog_show_available_fonts(HildonFontSelectionDialog 
						  *fontsel)

{
  gint i;
  
  HildonFontSelectionDialogPrivate *priv =
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fontsel);

  pango_context_list_families(gtk_widget_get_pango_context
			      (GTK_WIDGET(fontsel)), &priv->families,
			      &priv->n_families);

  filter_out_internal_fonts(priv->families, &priv->n_families);

  qsort(priv->families, priv->n_families, sizeof(PangoFontFamily *),
	cmp_families);


  for (i = 0; i < priv->n_families; i++) 
    {
      const gchar *name = pango_font_family_get_name(priv->families[i]);

      gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_font_type),
				name);
    }
}


static void
hildon_font_selection_dialog_show_available_positionings
                                             (HildonFontSelectionDialogPrivate
					      *priv)
{
  gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_positioning),
			    _("ecdg_va_font_printpos_1"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_positioning),
			    _("ecdg_va_font_printpos_2"));
  gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_positioning),
			    _("ecdg_va_font_printpos_3"));
}

/*Loads the sizes from a pre-allocated table*/
static void
hildon_font_selection_dialog_show_available_sizes
                                             (HildonFontSelectionDialogPrivate
					      *priv)
{
  gchar *size_str;
  gint i;

  for (i = 0; i < G_N_ELEMENTS(font_sizes); i++) 
    {
      size_str = g_strdup_printf ("%i %s",
				  font_sizes[i],
				  _("ecdg_va_font_size_trailer"));

      gtk_combo_box_append_text(GTK_COMBO_BOX(priv->cbx_font_size),
				size_str);
      g_free (size_str);
    }
}

/* WARNING: This function is called only from deprecated API */
static
void check_tags(gpointer data, gpointer user_data)
{
  gchar *font_family;
  GdkColor *fore_color =  NULL;
  gint p_size, p_weight, p_style, p_underline, p_rise;
  gboolean b_st, ff_s, size_s, fgc_s, w_s, ss_s, u_s, sth_s, r_s;
  
  GtkTextTag *tag = (GtkTextTag*) data;
  HildonFontSelectionDialogSettings *settings = 
    (HildonFontSelectionDialogSettings *) user_data;
  HildonFontSelectionDialogPrivate *priv = 
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(settings->fsd);
  
  /*get all the properties*/
  g_object_get(G_OBJECT(tag),
	       "family", &font_family, "family-set", &ff_s,
	       "size", &p_size, "size-set", &size_s,
	       "foreground-gdk", &fore_color, "foreground-set", &fgc_s,
	       "weight", &p_weight, "weight-set", &w_s,
	       "style", &p_style, "style-set", &ss_s,
	       "underline", &p_underline, "underline-set", &u_s,
	       "strikethrough", &b_st, "strikethrough-set", &sth_s, 
	       "rise", &p_rise, "rise-set", & r_s,
	       NULL);
  
  /* Check that the given values are valid. 
   * If not, set the combobox row indicator to 'inconsistent' */
  if(ff_s)
    {
      gint new_f = -1;
      gint i;
      
      for(i = 0; i < priv->n_families; i++)
	{
	  if(strcmp(font_family, 
		    pango_font_family_get_name(priv->families[i])) == 0)
	    {
	      new_f = i;
	      break;
	    }
	}
      
      if(settings->family == -1)
	settings->family = new_f;
      else if(settings->family != -2 && 
	      settings->family != new_f)
	settings->family = -2;/*inconsist*/

      g_free(font_family);
    }
  
  if(size_s)
    {
      gint new_size = -1;
      gint i;
      
      for(i = 0; i < G_N_ELEMENTS(font_sizes); i++)
	{
	  if(p_size == font_sizes[i] * PANGO_SCALE)
	    {
	      new_size = i;
	      break;
	    }
	}
      
      if(settings->size == -1)
	settings->size = new_size;
      else if(settings->size != -2 && 
	      settings->size != new_size)
	settings->size = -2;/*inconsist*/
    }
  
  if(fgc_s && settings->color == NULL 
     && !settings->color_inconsist)
	settings->color = fore_color;
  else if(fore_color != NULL)
    {
      if(!gdk_color_equal(fore_color, settings->color) 
	 && fgc_s)
	settings->color_inconsist = TRUE;
      
      gdk_color_free(fore_color);
    }

  if(w_s)
    settings->weight |= p_weight == PANGO_WEIGHT_NORMAL ? OFF_BIT : ON_BIT;
  
  if(ss_s)
    settings->style |= p_style == PANGO_STYLE_NORMAL ? OFF_BIT : ON_BIT;
  
  if(u_s)
    settings->underline |= 
      p_underline == PANGO_UNDERLINE_NONE ? OFF_BIT : ON_BIT;
  
  if(sth_s)
    settings->strikethrough |= b_st ? ON_BIT : OFF_BIT;

  if(r_s)
    {
      gint new_rs = -1;
      
      if(p_rise == 0)
	new_rs = 0;/*normal*/
      else if (p_rise > 0)
	new_rs = 1;/*super*/
      else
	new_rs = 2;/*sub*/

      if(settings->position == -1)
	settings->position = new_rs;
      else if(settings->position != -2 && 
	      settings->position != new_rs)
	settings->position = -2;/*inconsist*/
    }
}

/* WARNING: This function is called only from deprecated API */
static
void check_attrs(gpointer data, gpointer user_data)
{
  PangoAttribute *attr = (PangoAttribute *) data;
  HildonFontSelectionDialogSettings *settings = 
    (HildonFontSelectionDialogSettings *) user_data;
  HildonFontSelectionDialogPrivate *priv = 
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(settings->fsd);
  
  gchar *family;
  GdkColor color;
  gint i;
  gint size, weight, style, underline, strikethrough, rise;
  gint new_f = -1, new_size = -1, new_rise = -1;

  /* Check that the given values are valid.
   * If not, set the combobox row indicator to 'inconsistent' */
  switch(attr->klass->type)
    {
    case PANGO_ATTR_FAMILY:
      family = ((PangoAttrString *) attr)->value;

      for(i = 0; i < priv->n_families; i++)
	{
	  if(strcmp(family, 
		    pango_font_family_get_name(priv->families[i])) == 0)
	    {
	      new_f = i;
	      break;
	    }
	}

      if(settings->family == -1)
	settings->family = new_f;
      else if(settings->family != -2 && 
	      settings->family != new_f)
	settings->family = -2;/*inconsist*/
      
      break;
    case PANGO_ATTR_SIZE:
      size = ((PangoAttrInt *) attr)->value;
      
      for(i = 0; i < G_N_ELEMENTS(font_sizes); i++)
	{
	  if(size == font_sizes[i] * PANGO_SCALE)
	    {
	      new_size = i;
	      break;
	    }
	}
      
      if(settings->size == -1)
	settings->size = new_size;
      else if(settings->size != -2 && 
	      settings->size != new_size)
	settings->size = -2;/*inconsist*/

      break;
    case PANGO_ATTR_FOREGROUND:
      color.red = ((PangoAttrColor *) attr)->color.red;
      color.green = ((PangoAttrColor *) attr)->color.green;
      color.blue = ((PangoAttrColor *) attr)->color.blue;

      if(!settings->color_inconsist &&  settings->color == NULL)
	settings->color = gdk_color_copy(&color);
      else if(settings->color != NULL && 
	      !gdk_color_equal(&color, settings->color))
	settings->color_inconsist = TRUE;

      break;
    case PANGO_ATTR_WEIGHT:
      weight = ((PangoAttrInt *) attr)->value;

      settings->weight |= weight == PANGO_WEIGHT_NORMAL ? OFF_BIT : ON_BIT;
      
      break;
    case PANGO_ATTR_STYLE:
      style = ((PangoAttrInt *) attr)->value;

      settings->style |= style == PANGO_STYLE_NORMAL ? OFF_BIT : ON_BIT; 
      
      break;
    case PANGO_ATTR_UNDERLINE:
      underline = ((PangoAttrInt *) attr)->value;

      settings->underline |= 
	underline == PANGO_UNDERLINE_NONE ? OFF_BIT : ON_BIT;
  
      break;
    case PANGO_ATTR_STRIKETHROUGH:
      strikethrough = ((PangoAttrInt *) attr)->value;
      
      settings->strikethrough |= strikethrough ? ON_BIT : OFF_BIT;

      break;
    case PANGO_ATTR_RISE:
      rise = ((PangoAttrInt *) attr)->value;
      
      if(rise == 0)
	new_rise = 0;/*normal*/
      else if (rise > 0)
	new_rise = 1;/*super*/
      else
	new_rise = 2;/*sub*/

      if(settings->position == -1)
	settings->position = new_rise;
      else if(settings->position != -2 && 
	      settings->position != new_rise)
	settings->position = -2;/*inconsist*/

      break;
    default:
      break;
    }

  pango_attribute_destroy(attr);
}

/* WARNING: This function is called only from deprecated API */
static void
settings_init(HildonFontSelectionDialogSettings *settings,
	      HildonFontSelectionDialog  *fsd)
{
  settings->fsd = fsd;
  settings->family = -1;
  settings->size = -1;
  settings->color = NULL;
  settings->color_inconsist = FALSE;
  settings->weight = 0;
  settings->style = 0;
  settings->underline = 0;
  settings->strikethrough = 0;
  settings->position = -1;
}

/* WARNING: This function is called only from deprecated API */
static void
bit_mask_toggle(gint mask, GtkToggleButton *button,
		GObject *object, const gchar *prop,
		const gchar *prop_set)
{
  
  if(mask == 3)
    gtk_toggle_button_set_inconsistent(button, TRUE);
  else
    {
      gtk_toggle_button_set_inconsistent(button, FALSE);

      if(mask == 1)
	gtk_toggle_button_set_active(button, TRUE);
      else
	gtk_toggle_button_set_active(button, FALSE);

      g_object_notify(object, prop);
    }

  g_object_notify(object, prop_set);
}

/* WARNING: This function is called only from deprecated API */
static void
combo_active(gint active, GtkComboBox *box, 
	     GObject *object, const gchar *prop, const gchar *prop_set)
{
  /*probaly not the best function, but we need all these
   * parameters to keep things together*/
 
  
  if(active >= 0)
    {
      gtk_combo_box_set_active(box, active);
      g_object_notify(object, prop);
    }
  else
    gtk_combo_box_set_active(box, -1);

  g_object_notify(object, prop_set);
}

/* WARNING: This function is called only from deprecated API */
static void
settings_apply(HildonFontSelectionDialogSettings *settings)
{

  HildonFontSelectionDialogPrivate *priv = 
    HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(settings->fsd);
  
  /*family*/
  combo_active(settings->family, GTK_COMBO_BOX(priv->cbx_font_type),
	       G_OBJECT(settings->fsd), "family", "family-set");
  
  /*size*/
  combo_active(settings->size, GTK_COMBO_BOX(priv->cbx_font_size),
	       G_OBJECT(settings->fsd), "size", "size-set");
  
  /*block our signal handler indicating color has been changed by
   * the user before set the color, and unblock it after setting
   * is done*/
  
  if(settings->color == NULL || settings->color_inconsist)
    {
      GdkColor black;

      black.red = black.green = black.blue = 0;
      g_signal_handler_block((gpointer) priv->font_color_button,
			     priv->color_modified_signal_handler);
      
      g_object_set(G_OBJECT(settings->fsd), "color", &black, "color-set", 
		   FALSE, NULL);

      g_signal_handler_unblock((gpointer) priv->font_color_button,
			       priv->color_modified_signal_handler);
    }
  else 
      g_object_set(G_OBJECT(settings->fsd), "color", settings->color, NULL);
  
  /*weight*/
  bit_mask_toggle(settings->weight, GTK_TOGGLE_BUTTON(priv->chk_bold),
		  G_OBJECT(settings->fsd), "bold", "bold-set");
  
  /*style*/
  bit_mask_toggle(settings->style, GTK_TOGGLE_BUTTON(priv->chk_italic),
		  G_OBJECT(settings->fsd), "italic", "italic-set");
  
  /*underline*/
  bit_mask_toggle(settings->underline, 
		  GTK_TOGGLE_BUTTON(priv->chk_underline), 
		  G_OBJECT(settings->fsd), "underline", "underline-set");
  
  /*strikethrough*/
  bit_mask_toggle(settings->strikethrough, 
		  GTK_TOGGLE_BUTTON(priv->chk_strikethrough),
		  G_OBJECT(settings->fsd), "strikethrough", 
		  "strikethrough-set");

  /*position*/
  combo_active(settings->position, GTK_COMBO_BOX(priv->cbx_positioning),
	       G_OBJECT(settings->fsd), "position", "position-set");
}

static void
settings_destroy(HildonFontSelectionDialogSettings *settings)
{
  if(settings->color != NULL)
    gdk_color_free(settings->color);
}

static void
toggle_clicked(GtkButton *button, gpointer unused)
{
  GtkToggleButton *t_b = GTK_TOGGLE_BUTTON(button);

  /*we have to remove the inconsistent state ourselves*/
  if(gtk_toggle_button_get_inconsistent(t_b))
    {
      gtk_toggle_button_set_inconsistent(t_b, FALSE);
      gtk_toggle_button_set_active(t_b, FALSE);
    }
}

/*******************/
/*Public functions*/
/*******************/

/**
 * hildon_font_selection_dialog_new:
 * @parent: the parent window
 * @title: the title of font selection dialog
 *
 * If NULL is passed for title, then default title
 * "Font" will be used.
 *
 * Returns: a new #HildonFontSelectionDialog
 */
GtkWidget *
hildon_font_selection_dialog_new(GtkWindow * parent,
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
 * @fsd: the font selection dialog
 *
 * Gets the text in preview dialog, which does not include the 
 * reference text. The returned string must be freed by the user.
 *
 * Returns: a string pointer
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
 * @fsd: the font selection dialog
 * @text: the text to be displayed in the preview dialog
 *
 * The default preview text is
 * "The quick brown fox jumped over the lazy dogs"
 */
void
hildon_font_selection_dialog_set_preview_text(HildonFontSelectionDialog *
					      fsd, const gchar * text)
{
   HildonFontSelectionDialogPrivate *priv = NULL;
   
   g_return_if_fail(HILDON_IS_FONT_SELECTION_DIALOG(fsd));
   g_return_if_fail(text);
  
   priv = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
   
   g_free(priv->preview_text);
   priv->preview_text = g_strdup(text);
   g_object_notify (G_OBJECT (fsd), "preview-text");
}

/**
 * hildon_font_selection_dialog_get_text_tag:
 * @fsd: the font selection dialog
 *
 * Get the #GtkTextTag for selections. This function
 * is deprecated. The best way to use
 * the text tags is to reuse them as much as possible.
 * The recommended way is to get the properties of the font
 * selection dialog on GTK_RESPONSE_OK, and according to
 * these properties use the tags that you have pre-created.
 * 
 * Returns: a #GtkTextTag having corresponding properties
 * set 
 */ 
GtkTextTag * 
hildon_font_selection_dialog_get_text_tag (HildonFontSelectionDialog *fsd)
{
  GtkTextTag *tag;
  gint size, position;
  gboolean family_set, size_set, color_set, bold, bold_set,
           italic, italic_set, underline, underline_set,
	   strikethrough, strikethrough_set, position_set;
  GdkColor *color = NULL;
  gchar *family = NULL;

  tag = gtk_text_tag_new(NULL);
  
  g_object_get(G_OBJECT(fsd),
	       "family", &family, "family-set", &family_set,
	       "size", &size, "size-set", &size_set,
	       "color", &color, "color-set", &color_set,
	       "bold", &bold, "bold-set", &bold_set,
	       "italic", &italic, "italic-set", &italic_set,
	       "underline", &underline, "underline-set", &underline_set,
	       "strikethrough", &strikethrough, "strikethrough-set", 
	       &strikethrough_set, "position", &position, 
	       "position-set", &position_set, NULL);
  /*family*/
  if(family_set)
    g_object_set(G_OBJECT(tag), "family",
		 family, "family-set", TRUE, NULL);
  else
    g_object_set(G_OBJECT(tag), "family-set", FALSE, NULL);

  g_free(family);
  
  /*size*/
  if(size_set)
    g_object_set(G_OBJECT(tag), "size", size * PANGO_SCALE, 
		 "size-set", TRUE, NULL);
  else
    g_object_set(G_OBJECT(tag), "size-set", FALSE, NULL);
  
  /*color*/
  if(color_set)
    g_object_set(G_OBJECT(tag), "foreground-gdk", color, 
		 "foreground-set", TRUE ,NULL);
  else
    g_object_set(G_OBJECT(tag), "foreground-set", FALSE, NULL);

  if(color != NULL)
    gdk_color_free(color);
  
  /*weight*/
  if(bold_set)
    {
      if(bold)
        g_object_set(G_OBJECT(tag), "weight", PANGO_WEIGHT_BOLD, NULL);
      else
        g_object_set(G_OBJECT(tag), "weight", PANGO_WEIGHT_NORMAL, NULL);
	
      g_object_set(G_OBJECT(tag), "weight-set", TRUE, NULL);
    }
  else
    g_object_set(G_OBJECT(tag), "weight-set", FALSE, NULL);
  
  /*style*/
  if(italic_set)
    {
      if(italic)
        g_object_set(G_OBJECT(tag), "style", PANGO_STYLE_ITALIC, NULL);
      else
        g_object_set(G_OBJECT(tag), "style", PANGO_STYLE_NORMAL, NULL);
	
      g_object_set(G_OBJECT(tag), "style-set", TRUE, NULL);
    }
  else
    g_object_set(G_OBJECT(tag), "style-set", FALSE, NULL);
  
  /*underline*/
  if(underline_set)
    {
      if(underline)
        g_object_set(G_OBJECT(tag), "underline", PANGO_UNDERLINE_SINGLE, NULL);
      else
        g_object_set(G_OBJECT(tag), "underline", PANGO_UNDERLINE_NONE, NULL);
	
      g_object_set(G_OBJECT(tag), "underline-set", TRUE, NULL);
    }
  else
    g_object_set(G_OBJECT(tag), "underline-set", FALSE, NULL);
  
  /*strikethrough*/
  if(strikethrough_set)
    {
      if(strikethrough)
        g_object_set(G_OBJECT(tag), "strikethrough", TRUE, NULL);
      else
        g_object_set(G_OBJECT(tag), "strikethrough", FALSE, NULL);
	
      g_object_set(G_OBJECT(tag), "strikethrough-set", TRUE, NULL);
    }
  else
    g_object_set(G_OBJECT(tag), "strikethrough-set", FALSE, NULL);
  
  /*position*/
  if(position_set)
    {
      switch(position)
	{
	case 1: /*super*/
	  g_object_set(G_OBJECT(tag), "rise", SUPERSCRIPT_RISE, NULL);
	  break;
	case -1: /*sub*/
	  g_object_set(G_OBJECT(tag), "rise", SUBSCRIPT_LOW, NULL);
	  break;
	case 0: /*normal*/
	  g_object_set(G_OBJECT(tag), "rise", 0, NULL);
	  break;
	}
      g_object_set(G_OBJECT(tag), "rise-set", TRUE, NULL);
    }
  else
    g_object_set(G_OBJECT(tag), "rise-set", FALSE, NULL);
  
  return tag;
}

/** 
 * hildon_font_selection_dialog_set_buffer:
 * @fsd: the font selection dialog
 * @buffer: a #GtkTextBuffer containing the text to which the selections will 
 * be applied. Applying is responsibility of application.
 *
 * This is deprecated. GtkTextBuffer is not enough
 * to get the attributes of currently selected text. Please 
 * inspect the attributes yourself, and set the properties of
 * font selection dialog to reflect your inspection.
 */
void 
hildon_font_selection_dialog_set_buffer (HildonFontSelectionDialog *fsd,
					 GtkTextBuffer *buffer)
{
  GtkTextIter begin, end, iter;
  HildonFontSelectionDialogSettings settings;

  gtk_text_buffer_get_selection_bounds(buffer, &begin, &end);
  
  settings_init(&settings, fsd);
  
  iter = begin;

  /* Keep original settings if the selection includes nothing */ 
  if(gtk_text_iter_compare(&iter, &end) == 0)
    {
      GSList *slist;
      
      slist = gtk_text_iter_get_tags(&iter);
      g_slist_foreach(slist, check_tags, (gpointer) &settings);
      g_slist_free(slist);
    }

  /* Apply the user settings to the selected text */
  while(gtk_text_iter_compare(&iter, &end) < 0)
    {
      GSList *slist;
      
      slist = gtk_text_iter_get_tags(&iter);
      g_slist_foreach(slist, check_tags, (gpointer) &settings);
      g_slist_free(slist);
      
      if(!gtk_text_iter_forward_cursor_position(&iter))
	break;
    }

  settings_apply(&settings);
  settings_destroy(&settings);
}

/**
 * hildon_font_selection_dialog_get_font:
 * @fsd: the font selection dialog
 *
 * This is deprecated. @PangoAttrList needs
 * starting index, and end index on construction.
 *
 * Returns: pointer to @PangoAttrList
 */
PangoAttrList
*hildon_font_selection_dialog_get_font(HildonFontSelectionDialog * fsd)
{
  HildonFontSelectionDialogPrivate *priv
    = HILDON_FONT_SELECTION_DIALOG_GET_PRIVATE(fsd);
  
  g_return_val_if_fail(HILDON_IS_FONT_SELECTION_DIALOG(fsd), FALSE);
  /*an approve of none working api, should have ask for start_index,
   * and length in bytes of the string, currently using preview_text 
   * length, KLUDGE!*/
  
  return hildon_font_selection_dialog_create_attrlist(fsd, 
				0, strlen(priv->preview_text));
}

/**
 * hildon_font_selection_dialog_set_font:
 * @fsd: the font selection dialog
 * @list: the pango attribute list
 *
 * This is a deprecated.
 * 
 * Sets the font to the dialog.
 */
void 
hildon_font_selection_dialog_set_font(HildonFontSelectionDialog * fsd,
				      PangoAttrList * list)
{
  PangoAttrIterator *iter;
  HildonFontSelectionDialogSettings settings;

  iter = pango_attr_list_get_iterator(list);
  
  settings_init(&settings, fsd);
  
  while(iter != NULL)
    {
      GSList *slist;
      
      slist = pango_attr_iterator_get_attrs(iter);
      g_slist_foreach(slist, check_attrs, (gpointer) &settings);
      g_slist_free(slist);
      
      if(!pango_attr_iterator_next(iter))
	break;
    }

  pango_attr_iterator_destroy(iter);

  settings_apply(&settings);
  settings_destroy(&settings);
}
