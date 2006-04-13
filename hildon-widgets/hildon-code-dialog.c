/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2006 Nokia Corporation.
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


#include "hildon-code-dialog.h"
#include "hildon-defines.h"
#include "gtk-infoprint.h"

#include <gdk/gdkkeysyms.h>

#include<gtk/gtkbutton.h>
#include<gtk/gtkentry.h>
#include<gtk/gtkicontheme.h>
#include<gtk/gtktable.h>
#include<gtk/gtkvbox.h>
#include<gtk/gtkbbox.h>
#include<gtk/gtklabel.h>
#include<gtk/gtkalignment.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>

#define HEIGHT (38-HILDON_MARGIN_DEFAULT)
#define WIDTH  (60-HILDON_MARGIN_DEFAULT)
#define BACKSPACE_ICON    "qgn_calculator_backspace"

#define _(String) dgettext(PACKAGE, String)
#define DEVICELOCK_OK     _("secu_enter_lock_code_dialog_ok")
#define DEVICELOCK_CANCEL _("secu_enter_lock_code_dialog_cancel")
#define DEVICELOCK_TITLE  _("secu_application_title")


#define MAX_PINCODE_LEN   (10)



static void hildon_code_dialog_class_init (HildonCodeDialogClass *cd_class);
static void hildon_code_dialog_init (HildonCodeDialog *self);
static void hildon_code_dialog_finalize (GObject *self);
static void hildon_code_dialog_button_clicked (GtkButton *buttonm,
                                               gpointer user_data);
static void hildon_code_dialog_insert_text (GtkEditable *editable,
                                            gchar *new_text,
                                            gint new_text_length,
                                            gint *position,
                                            gpointer user_data);

static gboolean hildon_code_dialog_key_press_event (GtkWidget *widget,
                                                    GdkEventKey *event,
                                                    gpointer user_data);


GtkDialogClass *parent_class;

struct _HildonCodeDialogPrivate
{
    GtkWidget *entry;
    GtkWidget *buttons[5][3]; 
    GtkWidget *help_text;
};

GType hildon_code_dialog_get_type (void)
{
    static GType type = 0;

    if (!type)
    {
        static const GTypeInfo info = 
        {
            sizeof(HildonCodeDialogClass),
            NULL, /* base_init */
            NULL, /* base_finalize */
            (GClassInitFunc) hildon_code_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonCodeDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_code_dialog_init
        };
        type = g_type_register_static (GTK_TYPE_DIALOG,
                "HildonCodeDialog", &info, 0);
    }
    return type;
}

static void hildon_code_dialog_class_init (HildonCodeDialogClass *cd_class)
{
    /* Get convenience variables */
    GObjectClass *object_class = G_OBJECT_CLASS (cd_class);

    parent_class = GTK_DIALOG_CLASS (g_type_class_peek_parent (cd_class));

    object_class->finalize = hildon_code_dialog_finalize;

}

static void hildon_code_dialog_init (HildonCodeDialog *dialog)
{
    HildonCodeDialogPrivate *priv;
    gint i, x, y;
    GtkWidget *dialog_vbox1 = NULL;
    GtkWidget *table = NULL;
    GtkWidget *alignment = NULL;
    GtkWidget *vbox1 = NULL;
    GtkWidget *image1 = NULL;
    GtkWidget *dialog_action_area1 = NULL;
    GdkGeometry hints;
    GtkWidget *okButton;
    GtkWidget *cancelButton;

    priv = dialog->priv = (HildonCodeDialogPrivate*) g_malloc0 
        (sizeof (HildonCodeDialogPrivate));

    const gchar* numstrs[10] = {
        "0","1","2","3","4","5","6","7","8","9"
    };

    GdkPixbuf* pixbuf = NULL;
    GtkIconTheme* icon_theme = NULL;
    GtkIconInfo *icon_info = NULL;
    gint base_size=0;

    /* Set default title */
    gtk_window_set_title (GTK_WINDOW (dialog), DEVICELOCK_TITLE);

    gtk_window_set_type_hint(GTK_WINDOW (dialog), GDK_WINDOW_TYPE_HINT_DIALOG);

    hints.min_width  = -1;
    hints.min_height = -1;
    hints.max_width  = -1;
    hints.max_height = -1;

    gtk_window_set_geometry_hints(GTK_WINDOW(dialog), GTK_WIDGET(dialog), 
            &hints,
            GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);

    table = gtk_table_new (4, 3, FALSE);
    gtk_table_set_row_spacings (GTK_TABLE (table), HILDON_MARGIN_DEFAULT );
    gtk_table_set_col_spacings (GTK_TABLE (table), HILDON_MARGIN_DEFAULT );

    dialog_vbox1 = GTK_DIALOG (dialog)->vbox;
    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_box_set_spacing (GTK_BOX(vbox1),HILDON_MARGIN_DOUBLE);

    priv->help_text= gtk_label_new ("");
    alignment=gtk_alignment_new(0.5,0,1,1);
    gtk_container_add(GTK_CONTAINER(alignment),priv->help_text);

    priv->entry = gtk_entry_new ();
    
    GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(priv->entry),GTK_CAN_FOCUS);
    gtk_entry_set_invisible_char (GTK_ENTRY(priv->entry), g_utf8_get_char ("*"));
    gtk_entry_set_alignment (GTK_ENTRY(priv->entry),1.0);

    gtk_editable_set_editable (GTK_EDITABLE(priv->entry),FALSE);
    gtk_entry_set_visibility (GTK_ENTRY(priv->entry), FALSE);

    gtk_box_pack_start (GTK_BOX (vbox1),alignment,TRUE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (vbox1),priv->entry,TRUE,FALSE,0);
    gtk_box_pack_start (GTK_BOX (vbox1),table,FALSE,TRUE,0);

    gtk_box_pack_start (GTK_BOX (dialog_vbox1), vbox1,FALSE,TRUE,0);


    for(i=1;i<=3;i++) {
        priv->buttons[0][i-1] = gtk_button_new_with_mnemonic(numstrs[i]);
        gtk_widget_set_size_request(priv->buttons[0][i-1],WIDTH,HEIGHT);
        gtk_table_attach_defaults(GTK_TABLE(table), priv->buttons[0][i-1],
                i-1,i,0,1);
    }

    for(i=4;i<=6;i++) {
        priv->buttons[1][i-4] = gtk_button_new_with_mnemonic (numstrs[i]);
        gtk_widget_set_size_request(priv->buttons[1][i-4],WIDTH,HEIGHT);
        gtk_table_attach_defaults(GTK_TABLE(table),priv->buttons[1][i-4],
                i-4,i-3,1,2);
    }

    for(i=7;i<=9;i++) {
        priv->buttons[2][i-7] = gtk_button_new_with_mnemonic (numstrs[i]);
        gtk_widget_set_size_request(priv->buttons[2][i-7], WIDTH,HEIGHT);
        gtk_table_attach_defaults(GTK_TABLE(table), priv->buttons[2][i-7],
                i-7,i-6,2,3);
    }

    priv->buttons[3][0] = priv->buttons[3][1] = 
        gtk_button_new_with_mnemonic (numstrs[0]);
    gtk_widget_set_size_request (priv->buttons[3][0], WIDTH,HEIGHT);
    gtk_table_attach (GTK_TABLE(table), priv->buttons[3][0],
            0,2,3,4, (GtkAttachOptions) (GTK_FILL),
            (GtkAttachOptions) (0), 0, 0);

    priv->buttons[3][2] = gtk_button_new ();
    gtk_widget_set_size_request(priv->buttons[3][2], WIDTH,HEIGHT);
    gtk_table_attach_defaults(GTK_TABLE(table), priv->buttons[3][2],
            2,3,3,4);

    icon_theme = gtk_icon_theme_get_default ();

    icon_info = gtk_icon_theme_lookup_icon(icon_theme, BACKSPACE_ICON, 1,
            GTK_ICON_LOOKUP_NO_SVG);
    base_size = gtk_icon_info_get_base_size(icon_info);
    gtk_icon_info_free(icon_info);
    icon_info = NULL;
    pixbuf = gtk_icon_theme_load_icon (icon_theme,
            BACKSPACE_ICON, base_size,
            GTK_ICON_LOOKUP_NO_SVG,
            NULL);
    image1 = gtk_image_new_from_pixbuf (pixbuf);
    g_object_unref (G_OBJECT(pixbuf));
    gtk_container_add (GTK_CONTAINER (priv->buttons[3][2]), image1);
    dialog_action_area1 = GTK_DIALOG (dialog)->action_area;
    gtk_button_box_set_layout (GTK_BUTTON_BOX(dialog_action_area1),
            GTK_BUTTONBOX_END);

    okButton = gtk_dialog_add_button (GTK_DIALOG(dialog),DEVICELOCK_OK,
            GTK_RESPONSE_OK);
    cancelButton =
        gtk_dialog_add_button (GTK_DIALOG(dialog),DEVICELOCK_CANCEL,
                GTK_RESPONSE_CANCEL);
    
    gtk_widget_set_sensitive (okButton, FALSE);

    priv->buttons[4][0] = priv->buttons[4][1] = okButton;
    priv->buttons[4][2] = cancelButton;

    /*
       Connect signals.
    */
    g_signal_connect (G_OBJECT(priv->entry), "insert_text",
            G_CALLBACK (hildon_code_dialog_insert_text), dialog);
    
    gtk_entry_set_max_length(GTK_ENTRY(priv->entry),MAX_PINCODE_LEN);

    for (x=0; x<3; x++)
    {
        for (y=0; y<3 ;y++)
        {
            g_signal_connect (G_OBJECT (priv->buttons[x][y]), "clicked",
                G_CALLBACK (hildon_code_dialog_button_clicked), dialog);
            g_signal_connect (G_OBJECT (priv->buttons[x][y]), "key-press-event",
                G_CALLBACK (hildon_code_dialog_key_press_event), dialog);
        }
    }
            
    g_signal_connect (G_OBJECT (priv->buttons[3][0]), "clicked",
                G_CALLBACK (hildon_code_dialog_button_clicked), dialog);
    g_signal_connect (G_OBJECT (priv->buttons[3][0]), "key-press-event",
                G_CALLBACK (hildon_code_dialog_key_press_event), dialog);
    
    g_signal_connect (G_OBJECT (priv->buttons[3][2]), "clicked",
                G_CALLBACK (hildon_code_dialog_button_clicked), dialog);
    g_signal_connect (G_OBJECT (priv->buttons[3][2]), "key-press-event",
                G_CALLBACK (hildon_code_dialog_key_press_event), dialog);
        
    g_signal_connect (G_OBJECT (okButton), "key-press-event",
                G_CALLBACK(hildon_code_dialog_key_press_event), dialog);
    
    g_signal_connect (G_OBJECT (cancelButton), "key-press-event",
                G_CALLBACK (hildon_code_dialog_key_press_event), dialog);

}

static void hildon_code_dialog_finalize (GObject *self)
{
    HildonCodeDialog *dialog = HILDON_CODE_DIALOG (self);

    fprintf( stderr, "called destroy\n" );

    g_free (dialog->priv);

    G_OBJECT_CLASS (parent_class)->finalize (self);
}

/* Signal handlers */
void hildon_code_dialog_button_clicked (GtkButton *button, gpointer user_data)
{
    HildonCodeDialog *dialog = HILDON_CODE_DIALOG (user_data);
    HildonCodeDialogPrivate *priv = dialog->priv;
    const char *number = gtk_button_get_label (button);

    if (number && *number )
    {
        gtk_entry_append_text (GTK_ENTRY (priv->entry), number);
    }
    else
    {
        /* Backspace */
        gchar *text = g_strdup (gtk_entry_get_text (GTK_ENTRY (priv->entry)));
        gchar *pos;
        
        pos = text;

        while (*pos != '\0')
        {
            pos ++;
        }

        pos = g_utf8_find_prev_char (text, pos);

        if (pos)
        {
            *pos=0;
        }

        gtk_entry_set_text (GTK_ENTRY (priv->entry), text);

        if (*text == 0)
        {
            gtk_widget_set_sensitive (priv->buttons[4][0], FALSE);
        }

        g_free (text);
    }

}

static void hildon_code_dialog_insert_text (GtkEditable *editable,
                                            gchar *new_text,
                                            gint new_text_length,
                                            gint *position,
                                            gpointer user_data)
{
    HildonCodeDialog *dialog = HILDON_CODE_DIALOG (user_data);
    HildonCodeDialogPrivate *priv = dialog->priv;
    gchar * text = g_strdup(gtk_entry_get_text (GTK_ENTRY (priv->entry)));
    glong length = g_utf8_strlen (text, -1);
    g_free (text);

    if (length == MAX_PINCODE_LEN)
    {
        gtk_infoprint(NULL, "ckdg_ib_maximum_characters_reached");
    }

    else if (!length)
    { 
        /* make the Ok button sensitive */
        gtk_widget_set_sensitive(priv->buttons[4][0], TRUE);
    }
}

static gboolean hildon_code_dialog_key_press_event (GtkWidget *widget,
                                                    GdkEventKey *event,
                                                    gpointer user_data)
{
    HildonCodeDialog *dialog = HILDON_CODE_DIALOG (user_data);
    HildonCodeDialogPrivate *priv = dialog->priv;
    GtkWidget *new_widget = widget;
    gint x, y;

    for (x = 0; x < 5; x++)
    {
        for (y = 0; y < 3; y++)
        {
            if (priv->buttons[x][y] == widget)
                goto found;
        }
    }
    return FALSE;

found:

    while (new_widget == widget)
    {
        switch (event->keyval)
        {
            case GDK_Up:
                x = (x+4)%5;
                break;

            case GDK_Down:
                x = (x+1)%5;
                break;

            case GDK_Left:
                y = (y+2)%3;
                break;

            case GDK_Right:
                y = (y+1)%3;
                break;

            default:
                return FALSE;
        }
                
        new_widget = priv->buttons[x][y];
    }

    gtk_widget_grab_focus (new_widget);

    return TRUE;
}

/* Public methods */

/**
 * hildon_code_dialog_new:
 *
 * Use this function to create a new HildonCodeDialog.
 *
 * Return value: A @HildonCodeDialog.
 **/
GtkWidget *hildon_code_dialog_new()
{
    HildonCodeDialog *dialog = g_object_new (HILDON_TYPE_CODE_DIALOG, NULL);

    return GTK_WIDGET (dialog);
}

/**
 * hildon_code_dialog_get_code:
 * @dialog: The #HildonCodeDialog from which to get the entered code
 *
 * Use this function to access the code entered by the user.
 *
 * Return value: The entered code.
 **/
const gchar *hildon_code_dialog_get_code (HildonCodeDialog *dialog)
{
    g_return_val_if_fail (HILDON_IS_CODE_DIALOG (dialog), NULL);
    return gtk_entry_get_text (GTK_ENTRY (dialog->priv->entry));
}

/**
 * hildon_code_dialog_clear_clode:
 * @dialog: The #HildonCodeDialog whose entry should be cleared:
 *
 * Use this function to clear the user entered code.
 **/
void hildon_code_dialog_clear_code (HildonCodeDialog *dialog)
{
    g_return_if_fail (HILDON_IS_CODE_DIALOG (dialog));
    gtk_entry_set_text (GTK_ENTRY (dialog->priv->entry), "");
    gtk_widget_set_sensitive (dialog->priv->buttons[4][0], FALSE);
}

/**
 * hildon_code_dialog_set_help_text:
 * @dialog: The #HildonCodeDialog whose entry should be cleared:
 * @text: The text to use in the help label.
 *
 * Use this function to set the text that will be displayd in the
 * help label
 **/
void hildon_code_dialog_set_help_text (HildonCodeDialog *dialog, 
                                       const gchar *text)
{
    g_return_if_fail (HILDON_IS_CODE_DIALOG (dialog));
    gtk_label_set_text (GTK_LABEL (dialog->priv->help_text), text);
}
