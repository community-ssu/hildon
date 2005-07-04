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
 * @file hildon-time-picker.c
 * 
 * This file implements the HildonTimePicker widget. This widget
 * is used with HildonTimeEditor widget to set time.
 */

/* HILDON DOC
 * @shortdesc: TimePicker is a widget for setting a time.
 * @longdesc: The TimePicker contains two fields for setting the
 * time. Arrow buttons can be used to increment and decrement time. If
 * specified in systems locale setting AM/PM button is displayed.
 * 
 * @seealso: #HildonTimeEditor
 */

#include <hildon-widgets/hildon-time-picker.h>
#include <hildon-widgets/hildon-defines.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdk.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <langinfo.h>
#include <libintl.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _(String) dgettext(PACKAGE, String)

#define HILDON_TIME_PICKER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     HILDON_TYPE_TIME_PICKER, HildonTimePickerPriv));

typedef struct _HildonTimePickerPriv HildonTimePickerPriv;

/* 
 * Pointer parent class 
 */
static GtkDialogClass *parent_class;

/*< private >*/
struct _HildonTimePickerPriv
{
    GtkWidget *ht_label;
    GtkWidget *hs_label;
    GtkWidget *mt_label;
    GtkWidget *ms_label;
    GtkWidget *h_frame;
    GtkWidget *m_frame;
    GtkWidget *ampm_frame;
    GtkWidget *h_ebox;
    GtkWidget *mt_ebox;
    GtkWidget *ms_ebox;
    GtkWidget *h_up;
    GtkWidget *h_down;
    GtkWidget *mt_up;
    GtkWidget *mt_down;
    GtkWidget *ms_up;
    GtkWidget *ms_down;
    GtkWidget *d_label;

    /* Localisation stuff */
    GtkWidget *ampm_button;   /* Eventbox to hold label */
    GtkWidget *ampm_label;    /* Label to hold am or pm symbol */

    gboolean ampm_pos_after;  /* Position eventbox before of after clock */
    gboolean clock_24h; /* 12h or 24h time */
    gboolean am;        /* Indicates before or after noon */

    gchar *am_symbol;   /* Local symbol for am */
    gchar *pm_symbol;   /* Local symbol for pm */

    /* These are for timeout */
    guint id;   /* key repeat timer id */
    guint start_id;     /* key repeat start timer id */
    GtkWidget *press_button;
};

/* 
 * Private function prototype definitions 
 */
static void
hildon_time_picker_class_init (HildonTimePickerClass *picker_class);

static void
hildon_time_picker_init (HildonTimePicker *picker);

static void
hildon_time_picker_finalize (GObject *obj_self);

static gboolean
hildon_time_picker_key_press (GtkWidget *widget, GdkEventKey *event,
                              gpointer data);

static void
hildon_time_picker_clicked (GtkButton *button, gpointer data);

static void
hildon_time_picker_add_style (void);

static gboolean
hildon_time_picker_timeout (gpointer data);

static gboolean
hildon_time_picker_start_timer (gpointer data);

static gboolean
hildon_time_picker_button_press (GtkWidget *widget, GdkEventButton *event,
                                 gpointer data);

static gboolean
hildon_time_picker_button_release (GtkWidget *widget, GdkEventButton *event,
                                   gpointer data);
static gboolean
hildon_time_picker_focus_in (GtkWidget *widget, GdkEventFocus *event,
                             gpointer data);
static void
hildon_time_picker_handle_hours (GtkButton *button, HildonTimePicker *tp);

static gboolean
hildon_time_picker_check_locale (HildonTimePicker *picker);

static gboolean
hildon_time_picker_ampm_clicked (GtkWidget *widget, GdkEventButton *event,
                                 gpointer data);

static gboolean
hildon_time_picker_change_ampm_label (HildonTimePicker *picker);

/* 
 * Public functions start here
 */
GType hildon_time_picker_get_type(void)
{
    static GType picker_type = 0;

    if (!picker_type)
      {
        static const GTypeInfo picker_info =
          {
            sizeof (HildonTimePickerClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_time_picker_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof (HildonTimePicker),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_time_picker_init,
          };
        picker_type = g_type_register_static (GTK_TYPE_DIALOG,
                                             "HildonTimePicker",
                                             &picker_info, 0);
      }
    return picker_type;
}

/**
 * hildon_time_picker_new:
 * @parent: parent window.
 *
 * Hildon Time Picker shows time picker dialog. Close button is placed
 * in dialog's action area and time picker is placed in dialogs vbox.
 * Actual time picker consists two #GtkLabel fields one for hour and
 * one for minutes, arrow buttons and an AM/PM button. A : is placed
 * between hour and minute fields.
 *
 * Return value: pointer to a new #HildonTimePicker widget.
 **/
GtkWidget *hildon_time_picker_new(GtkWindow * parent)
{
    GtkWidget *widget = g_object_new (HILDON_TYPE_TIME_PICKER, NULL);

    if (parent)
      {
        gtk_window_set_transient_for (GTK_WINDOW(widget), parent);
      }

    return GTK_WIDGET (widget);
}

/**
 * hildon_time_picker_set_time:
 * @picker: the #HildonTimePicker widget.
 * @hours: hours
 * @minutes: minutes
 *
 * Sets the time of the #HildonTimePicker widget.
 **/
void hildon_time_picker_set_time(HildonTimePicker * picker,
                                 guint hours, guint minutes)
{
    HildonTimePickerPriv *priv;
    gchar h_str[3], m_str[3], mt_str[2], ms_str[2], ht_str[2], hs_str[2];

    g_return_if_fail (picker);

    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    if ((hours >= 0) && (hours < 24) && (minutes >= 0) && (minutes < 60))
      {
        /* If we are using 12h clock, convert the time and check if it's
           after or before noon */
        if (!priv->clock_24h)
          {
            if (hours > 12)
              {
                priv->am = FALSE;
                hours = hours - 12;
              }
            else if (hours < 12)
              {
                priv->am = TRUE;
                if (hours == 0)
                  hours = 12;
              }
            else if (hours == 12)
                priv->am = FALSE;
            gtk_label_set_text (GTK_LABEL (priv->ampm_label),
                                priv->am ? priv->am_symbol : priv->pm_symbol);
          }

        sprintf (h_str, "%02d", hours);
        sprintf (m_str, "%02d", minutes);

        mt_str[0] = m_str[0];
        ms_str[0] = m_str[1];
        ht_str[0] = h_str[0];
        hs_str[0] = h_str[1];
        mt_str[1] = ms_str[1] = ht_str[1] = hs_str[1] = '\0';

        gtk_label_set_text (GTK_LABEL (priv->ht_label), ht_str);
        gtk_label_set_text (GTK_LABEL (priv->hs_label), hs_str);
        gtk_label_set_text (GTK_LABEL (priv->mt_label), mt_str);
        gtk_label_set_text (GTK_LABEL (priv->ms_label), ms_str);
      }
}

/**
 * hildon_time_picker_get_time:
 * @picker: the #HildonTimePicker widget.
 * @hours: hours
 * @minutes: minutes
 *
 * Gets the time of the #HildonTimePicker widget.
 **/
void hildon_time_picker_get_time(HildonTimePicker * picker,
                                 guint * hours, guint * minutes)
{
    HildonTimePickerPriv *priv;

    g_return_if_fail (picker);
    g_return_if_fail (hours);
    g_return_if_fail (minutes);

    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    *hours = 10 * atoi (gtk_label_get_text (GTK_LABEL (priv->ht_label))) +
        atoi (gtk_label_get_text (GTK_LABEL (priv->hs_label)));
    *minutes = 10 * atoi (gtk_label_get_text (GTK_LABEL (priv->mt_label))) +
        atoi (gtk_label_get_text (GTK_LABEL (priv->ms_label)));

    /* If we use 12h clock, we convert time to 24h time, because all time
       related APIs use 24h clock */
    if (!priv->clock_24h && !priv->am)
      {
        if (*hours != 12)
          *hours += 12;
      }
    if (!priv->clock_24h && *hours == 12 && priv->am)
      *hours = 0;
}

/*
 * Private functions start here
 */

static void
hildon_time_picker_class_init(HildonTimePickerClass * picker_class)
{
    GObjectClass *object_class = G_OBJECT_CLASS (picker_class);
    parent_class = g_type_class_peek_parent (picker_class);
    object_class->finalize = hildon_time_picker_finalize;

    g_type_class_add_private (picker_class, sizeof (HildonTimePickerPriv));
    hildon_time_picker_add_style();
}

static void hildon_time_picker_init(HildonTimePicker * picker)
{
    HildonTimePickerPriv *priv;
    GtkDialog *dlg;
    GtkRequisition req;
    PangoFontDescription *font;
    GtkWidget *h_box, *hl_box, *ml_box, *mmain_box, *mup_box, *mmid_box,
              *mdown_box, *w_box, *ampm_box, *event_box, *event_label;
    GtkWidget *hu_img, *hd_img, *mtu_img, *mtd_img, *msu_img, *msd_img;
    GtkSizeGroup *sgroup;
    struct tm *local;
    time_t stamp;

    gint px, py;

    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    priv->ampm_pos_after = TRUE;
    priv->press_button = NULL;
    priv->id = 0;

    dlg = GTK_DIALOG (picker);
    gtk_dialog_set_has_separator (dlg, FALSE);

    priv->ht_label = gtk_label_new(NULL);
    priv->hs_label = gtk_label_new(NULL);
    priv->mt_label = gtk_label_new(NULL);
    priv->ms_label = gtk_label_new(NULL);
    priv->d_label = gtk_label_new (":");
    priv->ampm_label = gtk_label_new (NULL);
    event_label = gtk_label_new (NULL);

/* +3/+1 -- Hildon specifications --
 * Forcing to the specification pixel cordinates
 */
    px = py = 0;
    gtk_misc_set_padding (GTK_MISC (priv->ht_label), 3, py + 1);
    gtk_misc_set_padding (GTK_MISC (priv->hs_label), 3, py + 1);

    gtk_misc_set_padding (GTK_MISC (priv->mt_label), 3, py + 1);
    gtk_misc_set_padding (GTK_MISC (priv->ms_label), 3, py + 1);

    gtk_misc_set_padding (GTK_MISC (priv->d_label), 0, py + 1);

    priv->h_frame = gtk_frame_new (NULL);
    priv->m_frame = gtk_frame_new (NULL);
    priv->ampm_frame = gtk_frame_new (NULL);

    priv->h_ebox = gtk_event_box_new();
    priv->mt_ebox = gtk_event_box_new();
    priv->ms_ebox = gtk_event_box_new();
    priv->ampm_button = gtk_event_box_new();

    GTK_WIDGET_SET_FLAGS (priv->h_ebox, GTK_CAN_FOCUS);
    GTK_WIDGET_SET_FLAGS (priv->mt_ebox, GTK_CAN_FOCUS);
    GTK_WIDGET_SET_FLAGS (priv->ms_ebox, GTK_CAN_FOCUS);
    GTK_WIDGET_SET_FLAGS (priv->ampm_button, GTK_CAN_FOCUS);

    gtk_widget_set_name (priv->ht_label, "timepickerlabel");
    gtk_widget_set_name (priv->hs_label, "timepickerlabel");
    gtk_widget_set_name (priv->mt_label, "timepickerlabel");
    gtk_widget_set_name (priv->ms_label, "timepickerlabel");

    gtk_widget_set_name (priv->h_ebox, "timepickerebox");
    gtk_widget_set_name (priv->mt_ebox, "timepickerebox");
    gtk_widget_set_name (priv->ms_ebox, "timepickerebox");
    gtk_widget_set_name (priv->ampm_button, "timepickerebox");

    gtk_widget_set_name (priv->h_frame, "numberframe");
    gtk_widget_set_name (priv->m_frame, "numberframe");
    
    gtk_widget_set_name (priv->ampm_frame, "am_pm_frame");

    hl_box = gtk_hbox_new (FALSE, 0);
    ml_box = gtk_hbox_new (FALSE, 0);
    ampm_box = gtk_vbox_new (FALSE, 0);

    /* Hour box */
    gtk_box_pack_start (GTK_BOX (hl_box), priv->ht_label, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (hl_box), priv->hs_label, FALSE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (priv->h_frame), hl_box);
    gtk_container_add (GTK_CONTAINER (priv->h_ebox), priv->h_frame);

    /* Minutes box */
    gtk_box_pack_start (GTK_BOX (ml_box), priv->mt_ebox, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (ml_box), priv->ms_ebox, FALSE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (priv->mt_ebox), priv->mt_label);
    gtk_container_add (GTK_CONTAINER (priv->ms_ebox), priv->ms_label);

    gtk_container_add (GTK_CONTAINER (priv->m_frame), ml_box);

    /* am/pm box */
    gtk_box_pack_start (GTK_BOX (ampm_box), priv->ampm_label, FALSE, FALSE,
                        0);

    gtk_container_add (GTK_CONTAINER (priv->ampm_frame), ampm_box);
    gtk_container_add (GTK_CONTAINER (priv->ampm_button), priv->ampm_frame);

    /* Check locale settings */
    priv->clock_24h = hildon_time_picker_check_locale (picker);

    stamp = time(NULL);
    local = localtime(&stamp);
    hildon_time_picker_set_time(picker, local->tm_hour, local->tm_min);

    sgroup = gtk_size_group_new (GTK_SIZE_GROUP_VERTICAL);

    priv->h_up = gtk_button_new();
    priv->h_down = gtk_button_new();
    priv->mt_up = gtk_button_new();
    priv->mt_down = gtk_button_new();
    priv->ms_up = gtk_button_new();
    priv->ms_down = gtk_button_new();

    hu_img = 
        gtk_image_new_from_stock ("hildon-picker-up", HILDON_ICON_SIZE_WIDG);
    hd_img =
        gtk_image_new_from_stock ("hildon-picker-down", HILDON_ICON_SIZE_WIDG);
    mtu_img =
        gtk_image_new_from_stock ("hildon-picker-up", HILDON_ICON_SIZE_WIDG);
    mtd_img =
        gtk_image_new_from_stock ("hildon-picker-down", HILDON_ICON_SIZE_WIDG);
    msu_img =
        gtk_image_new_from_stock ("hildon-picker-up", HILDON_ICON_SIZE_WIDG);
    msd_img =
        gtk_image_new_from_stock ("hildon-picker-down", HILDON_ICON_SIZE_WIDG);

    gtk_container_add (GTK_CONTAINER (priv->h_up), hu_img);
    gtk_container_add (GTK_CONTAINER (priv->h_down), hd_img);
    gtk_container_add (GTK_CONTAINER (priv->mt_up), mtu_img);
    gtk_container_add (GTK_CONTAINER (priv->mt_down), mtd_img);
    gtk_container_add (GTK_CONTAINER (priv->ms_up), msu_img);
    gtk_container_add (GTK_CONTAINER (priv->ms_down), msd_img);

    gtk_widget_set_name (priv->h_up, "hildon-arrow-item");
    gtk_widget_set_name (priv->h_down, "hildon-arrow-item");
    gtk_widget_set_name (priv->mt_up, "hildon-arrow-item");
    gtk_widget_set_name (priv->mt_down, "hildon-arrow-item");
    gtk_widget_set_name (priv->ms_up, "hildon-arrow-item");
    gtk_widget_set_name (priv->ms_down, "hildon-arrow-item");

    h_box = gtk_vbox_new (FALSE, 0);
    mmain_box = gtk_vbox_new (FALSE, 0);
    mup_box = gtk_hbox_new (TRUE, 0);
    mmid_box = gtk_hbox_new (FALSE, 0);
    mdown_box = gtk_hbox_new (TRUE, 0);
    event_box = gtk_vbox_new (FALSE, 0);
    w_box = gtk_hbox_new (FALSE, 0);

    /* Add widgets to same group... makes am/pm eventbox look good */
    gtk_size_group_add_widget (sgroup, priv->h_up);
    gtk_size_group_add_widget (sgroup, priv->mt_up);
    gtk_size_group_add_widget (sgroup, priv->ms_up);
    gtk_size_group_add_widget (sgroup, event_label);

    gtk_box_pack_start (GTK_BOX (h_box), priv->h_up, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (h_box), priv->h_ebox, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (h_box), priv->h_down, FALSE, FALSE, 0);

    gtk_box_pack_start (GTK_BOX (mmain_box), mup_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (mmain_box), mmid_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (mmain_box), mdown_box, FALSE, FALSE, 0);

    gtk_box_pack_start (GTK_BOX (event_box), event_label, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (event_box), priv->ampm_button, FALSE, FALSE, 
                        0);

    gtk_box_pack_start (GTK_BOX (mup_box), priv->mt_up, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (mup_box), priv->ms_up, FALSE, FALSE, 0);

    gtk_box_pack_start (GTK_BOX (mmid_box), priv->m_frame, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (mdown_box), priv->mt_down, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (mdown_box), priv->ms_down, FALSE, FALSE, 0);

    /* Pack am/pm eventbox before time fields if locale says so */
    if (!priv->clock_24h && !priv->ampm_pos_after)
        gtk_box_pack_start (GTK_BOX (w_box), event_box, FALSE, FALSE, 12);

    /* Pack other stuff */
    gtk_box_pack_start (GTK_BOX (w_box), h_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (w_box), priv->d_label, FALSE, FALSE, 1);
    gtk_box_pack_start (GTK_BOX (w_box), mmain_box, FALSE, FALSE, 0);

    /* Pack am/pm eventbox after time fields if locale says so */
    if (!priv->clock_24h && priv->ampm_pos_after)
        gtk_box_pack_start (GTK_BOX (w_box), event_box, FALSE, FALSE, 12);

    gtk_box_pack_start (GTK_BOX (dlg->vbox), w_box, FALSE, FALSE, 0);

    gtk_widget_show_all (dlg->vbox);
    gtk_dialog_add_button (dlg, _("ecdg_bd_time_picker_close"),
                           GTK_RESPONSE_OK);

    g_signal_connect (GTK_OBJECT (priv->h_up), "clicked",
                      G_CALLBACK (hildon_time_picker_clicked), picker);
    g_signal_connect (GTK_OBJECT (priv->h_up), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->h_up), "button-release-event",
                      G_CALLBACK (hildon_time_picker_button_release), picker);

    g_signal_connect (GTK_OBJECT (priv->h_down), "clicked",
                      G_CALLBACK (hildon_time_picker_clicked), picker);
    g_signal_connect (GTK_OBJECT (priv->h_down), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->h_down), "button-release-event",
                      G_CALLBACK (hildon_time_picker_button_release), picker);

    g_signal_connect (GTK_OBJECT (priv->mt_up), "clicked",
                      G_CALLBACK (hildon_time_picker_clicked), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_up), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_up), "button-release-event",
                      G_CALLBACK (hildon_time_picker_button_release), picker);

    g_signal_connect (GTK_OBJECT (priv->mt_down), "clicked",
                      G_CALLBACK (hildon_time_picker_clicked), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_down), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_down), "button-release-event",
                      G_CALLBACK (hildon_time_picker_button_release), picker);

    g_signal_connect (GTK_OBJECT (priv->ms_up), "clicked",
                      G_CALLBACK (hildon_time_picker_clicked), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_up), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_up), "button-release-event",
                      G_CALLBACK (hildon_time_picker_button_release), picker);

    g_signal_connect (GTK_OBJECT (priv->ms_down), "clicked",
                      G_CALLBACK (hildon_time_picker_clicked), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_down), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_down), "button-release-event",
                      G_CALLBACK (hildon_time_picker_button_release),picker);

    g_signal_connect (GTK_OBJECT (priv->h_ebox), "focus-in-event",
                      G_CALLBACK (hildon_time_picker_focus_in), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_ebox), "focus-in-event",
                      G_CALLBACK (hildon_time_picker_focus_in), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_ebox), "focus-in-event",
                      G_CALLBACK (hildon_time_picker_focus_in), picker);
    g_signal_connect (GTK_OBJECT (priv->ampm_button), "focus-in-event",
                      G_CALLBACK (hildon_time_picker_focus_in), picker);

    g_signal_connect (GTK_OBJECT (priv->h_ebox), "key-press-event",
                      G_CALLBACK (hildon_time_picker_key_press), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_ebox), "key-press-event",
                      G_CALLBACK (hildon_time_picker_key_press), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_ebox), "key-press-event",
                      G_CALLBACK (hildon_time_picker_key_press), picker);
    g_signal_connect (GTK_OBJECT (priv->ampm_button), "key-press-event",
                      G_CALLBACK (hildon_time_picker_key_press), picker);

    g_signal_connect (GTK_OBJECT (priv->h_ebox), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->mt_ebox), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->ms_ebox), "button-press-event",
                      G_CALLBACK (hildon_time_picker_button_press), picker);
    g_signal_connect (GTK_OBJECT (priv->ampm_button), "button-press-event",
                      G_CALLBACK (hildon_time_picker_ampm_clicked), picker);

    gtk_widget_realize (GTK_WIDGET (dlg));
    gdk_window_set_decorations (GTK_WIDGET (dlg)->window, GDK_DECOR_BORDER);
    gtk_window_set_modal (GTK_WINDOW (dlg), FALSE);

    font = pango_font_description_copy (priv->ht_label->style->font_desc);

    pango_font_description_set_size (font, 34 * PANGO_SCALE);
    gtk_widget_modify_font (priv->d_label, font);

    pango_font_description_set_size (font, 42 * PANGO_SCALE);
    gtk_widget_modify_font (priv->mt_label, font);
    gtk_widget_modify_font (priv->ms_label, font);
    gtk_widget_modify_font (priv->ht_label, font);
    gtk_widget_modify_font (priv->hs_label, font);

    gtk_widget_size_request (priv->ht_label, &req);
    req.height += 1;
    gtk_widget_set_size_request (priv->ht_label, req.width, req.height);

    gtk_widget_size_request (priv->hs_label, &req);
    req.height += 1;
    gtk_widget_set_size_request (priv->hs_label, req.width, req.height);

    gtk_widget_size_request (priv->mt_label, &req);
    req.height += 1;
    gtk_widget_set_size_request (priv->mt_label, req.width, req.height);

    gtk_widget_size_request (priv->ms_label, &req);
    req.height += 1;
    gtk_widget_set_size_request (priv->ms_label, req.width, req.height);

    pango_font_description_free (font);

    gtk_widget_grab_focus (priv->h_ebox);

}

static void
hildon_time_picker_finalize (GObject *obj_self)
{
    HildonTimePicker *picker;
    HildonTimePickerPriv *priv;

    picker = HILDON_TIME_PICKER (obj_self);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    /* free timers */
    if (priv->start_id)
        g_source_remove (priv->start_id);

    if (priv->id)
        g_source_remove (priv->id);

    g_free (priv->am_symbol);
    g_free (priv->pm_symbol);

    /* Call parent class finalize, if have one */
    if (G_OBJECT_CLASS (parent_class)->finalize)
        G_OBJECT_CLASS (parent_class)->finalize(obj_self);
}

static gboolean
hildon_time_picker_check_locale(HildonTimePicker *picker)
{
    HildonTimePickerPriv *priv;

    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    /* Now am/pm symbols always contain a copy. Value returned by
        nl_langinfo must not be modified. */
    priv->am_symbol = g_ascii_strdown (nl_langinfo (AM_STR), -1);
    priv->pm_symbol = g_ascii_strdown (nl_langinfo (PM_STR), -1);

    if (priv->am_symbol[0] == 0)
        return TRUE;

    /* Check what format am/pm time should be */
    if (!strncmp (nl_langinfo (T_FMT_AMPM), "%p", 2))
      priv->ampm_pos_after = FALSE;

    return FALSE;
}

static gboolean
hildon_time_picker_ampm_clicked(GtkWidget *widget, GdkEventButton *event,
                                gpointer data)
{
    HildonTimePicker *picker;
    HildonTimePickerPriv *priv;

    g_return_val_if_fail (widget, FALSE);
    g_return_val_if_fail (data, FALSE);

    picker = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    gtk_widget_grab_focus (widget);
    hildon_time_picker_change_ampm_label (picker);

    return FALSE;
}

static gboolean
hildon_time_picker_change_ampm_label(HildonTimePicker * picker)
{
    HildonTimePickerPriv *priv;

    g_return_val_if_fail (picker, FALSE);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (picker);

    if (priv->am)
      {
        gtk_label_set_text (GTK_LABEL (priv->ampm_label), priv->pm_symbol);
        priv->am = FALSE;
      }
    else
      {
        gtk_label_set_text (GTK_LABEL (priv->ampm_label), priv->am_symbol);
        priv->am = TRUE;
      }
    return TRUE;
}

static gboolean
hildon_time_picker_key_press (GtkWidget *widget, GdkEventKey *event,
                             gpointer data)
{
    HildonTimePicker *tp;
    HildonTimePickerPriv *priv;

    g_return_val_if_fail (data, FALSE);
    g_return_val_if_fail (event, FALSE);
    g_return_val_if_fail (widget, FALSE);

    tp = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (tp);

    /* If ESC pressed we should close dialog and not select value */

    /* We don't want wrap */
    if ((event->keyval == GDK_KP_Left || event->keyval == GDK_Left)
        && (widget == priv->h_ebox))
      {
        return TRUE;
      }
    else if ((event->keyval == GDK_KP_Right || event->keyval == GDK_Right)
            && (widget == priv->ampm_button))
      {
        return TRUE;
      }

    /* Handle ampm button */
    if ((event->keyval == GDK_Up || event->keyval == GDK_Down)
        && widget == priv->ampm_button)
      {
        hildon_time_picker_change_ampm_label (tp);
        return TRUE;
      }

    switch (event->keyval)
    {
      case GDK_Left:
        if (widget == priv->h_ebox)
            gtk_widget_grab_focus (priv->ampm_button);
        else if (widget == priv->ampm_button)
            gtk_widget_grab_focus (priv->ms_ebox);
        else if (widget == priv->ms_ebox)
            gtk_widget_grab_focus (priv->mt_ebox);
        else
            gtk_widget_grab_focus (priv->h_ebox);

        return TRUE;

      case GDK_Right:
        if (widget == priv->h_ebox)
            gtk_widget_grab_focus (priv->mt_ebox);
        else if (widget == priv->mt_ebox)
            gtk_widget_grab_focus (priv->ms_ebox);
        else if (widget == priv->ms_ebox)
            gtk_widget_grab_focus (priv->ampm_button);
        else
            gtk_widget_grab_focus (priv->h_ebox);

        return TRUE;

      case GDK_Up:
        if (widget == priv->h_ebox)
            hildon_time_picker_clicked (GTK_BUTTON (priv->h_up), data);
        if (widget == priv->mt_ebox)
            hildon_time_picker_clicked (GTK_BUTTON (priv->mt_up), data);
        if (widget == priv->ms_ebox)
            hildon_time_picker_clicked (GTK_BUTTON (priv->ms_up), data);

        return TRUE;

      case GDK_Down:
        if (widget == priv->h_ebox)
            hildon_time_picker_clicked (GTK_BUTTON (priv->h_down), data);
        if (widget == priv->mt_ebox)
            hildon_time_picker_clicked (GTK_BUTTON (priv->mt_down), data);
        if (widget == priv->ms_ebox)
            hildon_time_picker_clicked (GTK_BUTTON (priv->ms_down), data);

        return TRUE;

      case GDK_Escape:
        gtk_dialog_response (GTK_DIALOG (data), GTK_RESPONSE_REJECT);
        return TRUE;

      case GDK_KP_Enter:
      case GDK_Return:
        gtk_dialog_response (GTK_DIALOG (data), GTK_RESPONSE_ACCEPT);
        return TRUE;

      default:
        return TRUE;
    }

    return FALSE;
}

static void
hildon_time_picker_clicked (GtkButton *button, gpointer data)
{
    gchar new_val[3];
    gint val, tval;
    HildonTimePicker *tp;
    HildonTimePickerPriv *priv;

    g_return_if_fail (button);
    g_return_if_fail (data);

    tp = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (tp);

    if (button == GTK_BUTTON (priv->h_up) ||
        button == GTK_BUTTON (priv->h_down))
      {
        hildon_time_picker_handle_hours (button, tp);
      }

    if (button == GTK_BUTTON (priv->mt_up))
      {
        val = atoi (gtk_label_get_text (GTK_LABEL (priv->mt_label)));
        val++;
        if (val > 5)
          {
            val = 0;
            hildon_time_picker_handle_hours (GTK_BUTTON (priv->h_up), tp);
          }
        sprintf (new_val, "%d", val);
        gtk_label_set_text (GTK_LABEL (priv->mt_label), new_val);
        gtk_widget_grab_focus (priv->mt_ebox);
      }

    if (button == GTK_BUTTON (priv->mt_down))
      {
        val = atoi (gtk_label_get_text (GTK_LABEL (priv->mt_label)));
        val--;

        if (val < 0)
          {
            val = 5;
            hildon_time_picker_handle_hours (GTK_BUTTON (priv->h_down), tp);
          }

        sprintf (new_val, "%d", val);
        gtk_label_set_text (GTK_LABEL (priv->mt_label), new_val);
        gtk_widget_grab_focus (priv->mt_ebox);
      }

    if (button == GTK_BUTTON (priv->ms_up))
      {
        tval = atoi (gtk_label_get_text (GTK_LABEL (priv->mt_label)));
        val = atoi (gtk_label_get_text (GTK_LABEL (priv->ms_label)));
        val++;

        if (val > 9)
          {
            val = 0;
            tval++;

            if (tval > 5)
              {
                tval = 0;
                hildon_time_picker_handle_hours (GTK_BUTTON (priv->h_up), tp);
              }
            sprintf (new_val, "%d", tval);
            gtk_label_set_text (GTK_LABEL (priv->mt_label), new_val);
          }

        sprintf (new_val, "%d", val);
        gtk_label_set_text (GTK_LABEL (priv->ms_label), new_val);
        gtk_widget_grab_focus (priv->ms_ebox);
      }

    if (button == GTK_BUTTON (priv->ms_down))
      {
        tval = atoi(gtk_label_get_text (GTK_LABEL (priv->mt_label)));
        val = atoi(gtk_label_get_text (GTK_LABEL (priv->ms_label)));
        val--;

        if (val < 0)
          {
            val = 9;
            tval--;

            if (tval < 0)
              {
                tval = 5;
                hildon_time_picker_handle_hours (GTK_BUTTON (priv->h_down),
                                                 tp);
              }
            sprintf (new_val, "%d", tval);
            gtk_label_set_text (GTK_LABEL (priv->mt_label), new_val);
          }

        sprintf (new_val, "%d", val);
        gtk_label_set_text (GTK_LABEL (priv->ms_label), new_val);
        gtk_widget_grab_focus (priv->ms_ebox);
      }
}

static void
hildon_time_picker_handle_hours (GtkButton *button, HildonTimePicker *tp)
{
    gchar new_val[3];
    gint val;
    HildonTimePickerPriv *priv;

    g_return_if_fail (button);

    priv = HILDON_TIME_PICKER_GET_PRIVATE (tp);

    if (button == GTK_BUTTON (priv->h_up))
      {
        val = 10 * atoi (gtk_label_get_text (GTK_LABEL (priv->ht_label))) +
            atoi (gtk_label_get_text (GTK_LABEL (priv->hs_label)));
        val++;

        if (priv->clock_24h && val > 23)
            val = 0;
        else if (!priv->clock_24h && val == 12 && priv->am)
          {
            gtk_label_set_text (GTK_LABEL (priv->ampm_label),
                               priv->pm_symbol);
            priv->am = FALSE;
          }
        else if (!priv->clock_24h && val == 12 && !priv->am)
          {
            gtk_label_set_text (GTK_LABEL (priv->ampm_label),
                               priv->am_symbol);
            priv->am = TRUE;
          }
        else if (!priv->clock_24h && val > 12)
          val = 1;

        sprintf (new_val, "%02d", val);
        gtk_label_set_text (GTK_LABEL (priv->hs_label), &new_val[1]);
        new_val[1] = '\0';
        gtk_label_set_text (GTK_LABEL (priv->ht_label), new_val);
        gtk_widget_grab_focus (priv->h_ebox);
      }

    if (button == GTK_BUTTON (priv->h_down))
      {
        val = 10 * atoi (gtk_label_get_text (GTK_LABEL (priv->ht_label))) +
            atoi (gtk_label_get_text (GTK_LABEL (priv->hs_label)));
        val--;

        if (priv->clock_24h && val < 0)
            val = 23;
        else if (!priv->clock_24h && val == 11 && priv->am)
          {
            gtk_label_set_text (GTK_LABEL (priv->ampm_label),
                                priv->pm_symbol);
            priv->am = FALSE;
          }
        else if (!priv->clock_24h && val == 11 && !priv->am)
          {
            gtk_label_set_text (GTK_LABEL (priv->ampm_label),
                                priv->am_symbol);
            priv->am = TRUE;
          }
        else if (!priv->clock_24h && val < 1)
            val = 12;

        sprintf (new_val, "%02d", val);
        gtk_label_set_text (GTK_LABEL (priv->hs_label), &new_val[1]);
        new_val[1] = '\0';
        gtk_label_set_text (GTK_LABEL (priv->ht_label), new_val);
        gtk_widget_grab_focus (priv->h_ebox);
      }
}

static void
hildon_time_picker_add_style (void)
{
    gtk_rc_parse_string ("  style \"hildon-arrow-item\" {"
                         "    xthickness = 0"
                         "    ythickness = 0"
                         "    engine \"pixmap\" {"
                         "      image {"
                         "        function = BOX"
                         "      }"
                         "    }"
                         "  }"
                         "  widget \"*.hildon-arrow-item\""
                         "    style \"hildon-arrow-item\"");
}

static gboolean
hildon_time_picker_timeout (gpointer data)
{
    HildonTimePicker *p;
    HildonTimePickerPriv *priv;
    GtkWidget *b;

    if (!data)
        return FALSE;

    p = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (p);

    b = priv->press_button;

    if (b)
      {
        hildon_time_picker_clicked (GTK_BUTTON(b), data);
        return TRUE;
      }

    priv->id = 0;
    return FALSE;
}

static gboolean
hildon_time_picker_start_timer (gpointer data)
{
    HildonTimePicker *p;
    HildonTimePickerPriv *priv;
    GtkSettings *settings;
    guint interval;

    if (!data)
        return FALSE;

    p = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (p);

    settings = gtk_settings_get_default();
    g_object_get (settings, "gtk-update-timeout", &interval, NULL);

    if ((priv->press_button) && (priv->id == 0))
        priv->id = g_timeout_add (interval, hildon_time_picker_timeout, data);

    priv->start_id = 0;

    return FALSE;
}

static gboolean
hildon_time_picker_button_press (GtkWidget *widget, GdkEventButton *event,
                                 gpointer data)
{
    HildonTimePicker *p;
    HildonTimePickerPriv *priv;

    g_return_val_if_fail (widget, FALSE);
    g_return_val_if_fail (data, FALSE);

    p = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (p);

    if ((widget == priv->h_ebox) || (widget == priv->ms_ebox) ||
        (widget == priv->mt_ebox))
      {
        gtk_widget_grab_focus(widget);
        return FALSE;
      }

    if ((widget == priv->h_up) || (widget == priv->h_down)
        || (widget == priv->mt_up) || (widget == priv->mt_down)
        || (widget == priv->ms_up) || (priv->ms_down))
      {
        GtkSettings *settings;
        guint interval;

        settings = gtk_settings_get_default();
        g_object_get (settings, "gtk-initial-timeout", &interval, NULL);

        priv->press_button = widget;
        if (priv->start_id != 0)
            g_source_remove (priv->start_id);
        priv->start_id =
            g_timeout_add (interval, hildon_time_picker_start_timer, data);
      }

    if ((widget == priv->h_up) || (widget == priv->h_down))
        gtk_widget_grab_focus (priv->h_ebox);
    else if ((widget == priv->mt_up) || (widget == priv->mt_down))
        gtk_widget_grab_focus (priv->mt_ebox);
    else if ((widget == priv->ms_up) || (widget == priv->ms_down))
        gtk_widget_grab_focus (priv->ms_ebox);

    return FALSE;
}

static gboolean
hildon_time_picker_button_release (GtkWidget *widget, GdkEventButton *event,
                                   gpointer data)
{
    HildonTimePicker *p;
    HildonTimePickerPriv *priv;

    g_return_val_if_fail (widget, FALSE);
    g_return_val_if_fail (data, FALSE);

    p = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE( p);

    /* Setting this to NULL will result hanler to fail and return FALSE.
       Timeout will be removed if FALSE is returned. */
    priv->press_button = NULL;

    return FALSE;
}

static gboolean
hildon_time_picker_focus_in (GtkWidget *widget, GdkEventFocus *event,
                             gpointer data)
{
    HildonTimePicker *p;
    HildonTimePickerPriv *priv;

    g_return_val_if_fail (widget, FALSE);
    g_return_val_if_fail (data, FALSE);

    p = HILDON_TIME_PICKER (data);
    priv = HILDON_TIME_PICKER_GET_PRIVATE (p);

    if (widget == priv->h_ebox)
      {
        gtk_widget_set_state (priv->h_ebox, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->ht_label, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->hs_label, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->mt_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ms_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->mt_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ms_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ampm_button, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ampm_label, GTK_STATE_NORMAL);
      }
    if (widget == priv->mt_ebox)
      {
        gtk_widget_set_state (priv->h_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ht_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->hs_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->mt_ebox, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->ms_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->mt_label, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->ms_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ampm_button, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ampm_label, GTK_STATE_NORMAL);
      }
    if (widget == priv->ms_ebox)
      {
        gtk_widget_set_state (priv->h_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ht_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->hs_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->mt_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ms_ebox, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->mt_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ms_label, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->ampm_button, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ampm_label, GTK_STATE_NORMAL);
      }
    if (widget == priv->ampm_button)
      {
        gtk_widget_set_state (priv->h_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ht_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->hs_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->mt_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ms_ebox, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->mt_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ms_label, GTK_STATE_NORMAL);
        gtk_widget_set_state (priv->ampm_button, GTK_STATE_SELECTED);
        gtk_widget_set_state (priv->ampm_label, GTK_STATE_SELECTED);
      }

  return FALSE;
}
