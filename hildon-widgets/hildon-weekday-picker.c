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
 * TODO:
 * - Must check if graphics should be added to widgets weekday
 *   buttons
 *
 */

/* HILDON DOC
 * @shortdesc: Weekday Picker is a widget for selecting weekdays
 * @longdesc: Weekday Picker is a widget for selecting weekdays. 
 * It shows seven toggle buttons that can have state on or off.
 *
 * @seealso: #HildonTimePicker
 */

 /* GDate numbers days from 1 to 7 and G_DATE_MONDAY is 1st day. However
    according to locale settings first day is sunday. To get around this
    problem, we addjust GDate days numbering to be same as locale
    numbering */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>
#include <libintl.h>
#include <langinfo.h>
#include <time.h>
#include <gtk/gtksignal.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtktogglebutton.h>
#include <gtk/gtksizegroup.h>
#include <gtk/gtkwindow.h>
#include "hildon-weekday-picker.h"
#include "hildon-composite-widget.h"

#define _(String) gettext (String)

#define HILDON_WEEKDAY_PICKER_GET_PRIVATE(obj) \
    (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
     HILDON_WEEKDAY_PICKER_TYPE, HildonWeekdayPickerPrivate));

static GtkContainerClass *parent_class;

typedef struct _HildonWeekdayPickerPrivate HildonWeekdayPickerPrivate;

static void
hildon_weekday_picker_class_init(HildonWeekdayPickerClass * picker_class);

static void 
hildon_weekday_picker_init(HildonWeekdayPicker * picker);

static void
hildon_weekday_picker_size_allocate(GtkWidget * widget,
                                    GtkAllocation * allocation);

static void
hildon_weekday_picker_size_request(GtkWidget * widget,
                                   GtkRequisition * requisition);

static void
hildon_weekday_picker_forall(GtkContainer * container,
                             gboolean include_internals,
                             GtkCallback callback, gpointer callback_data);

static void
hildon_weekday_picker_destroy(GtkObject * self);

static gboolean
hildon_weekday_picker_mnemonic_activate(GtkWidget *widget,
                                        gboolean group_cycling);

static void
button_toggle(GtkToggleButton * togglebutton, gpointer data);

struct _HildonWeekdayPickerPrivate {
    guint dayorder[8];  /* Lookup table for weekdays */
    GtkWidget *buttons[8];      /* weekday buttons */
    guint days;
    guint last_index;
};

enum {
  SELECTION_CHANGED_SIGNAL,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

GType hildon_weekday_picker_get_type(void)
{
    static GType picker_type = 0;

    if (!picker_type) {
        static const GTypeInfo picker_info = {
            sizeof(HildonWeekdayPickerClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_weekday_picker_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonWeekdayPicker),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_weekday_picker_init,
        };
        picker_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                             "HildonWeekdayPicker",
                                             &picker_info, 0);
    }
    return picker_type;
}

static void
hildon_weekday_picker_class_init(HildonWeekdayPickerClass * picker_class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(picker_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(picker_class);
    GObjectClass *object_class = G_OBJECT_CLASS(picker_class);

    parent_class = g_type_class_peek_parent(picker_class);

    g_type_class_add_private(picker_class,
                             sizeof(HildonWeekdayPickerPrivate));

    widget_class->mnemonic_activate = hildon_weekday_picker_mnemonic_activate;
    widget_class->size_request = hildon_weekday_picker_size_request;
    widget_class->size_allocate = hildon_weekday_picker_size_allocate;
    widget_class->focus = hildon_composite_widget_focus;
    container_class->forall = hildon_weekday_picker_forall;
    GTK_OBJECT_CLASS(picker_class)->destroy =
        hildon_weekday_picker_destroy;

    signals[SELECTION_CHANGED_SIGNAL] = g_signal_new("selection_changed",
                                                G_OBJECT_CLASS_TYPE
                                                (object_class),
                                                G_SIGNAL_RUN_LAST |
                                                G_SIGNAL_ACTION,
                                                G_STRUCT_OFFSET
                                                (HildonWeekdayPickerClass,
                                                 selection_changed), NULL, NULL,
                                                gtk_marshal_VOID__INT,
                                                G_TYPE_NONE, 1, G_TYPE_INT);
}

static void
hildon_weekday_picker_init(HildonWeekdayPicker * picker)
{
    HildonWeekdayPickerPrivate *priv;
    gint i, day;
    const gchar *wdays[8];
    GtkSizeGroup *sgroup;

    sgroup = gtk_size_group_new(GTK_SIZE_GROUP_BOTH);

    /* NOTE: we query locales with "first_weekday" parameter, but this
       seems to be broken on some installations. e.g. on my system I
       receive "1" (Sunday) to be first day of the week on "fi_FI"
       locale, when it should be "2" (Monday). */

    /* Check our first weekday */
    day = *nl_langinfo(_NL_TIME_FIRST_WEEKDAY);

    wdays[1] = nl_langinfo(ABDAY_1);
    wdays[2] = nl_langinfo(ABDAY_2);
    wdays[3] = nl_langinfo(ABDAY_3);
    wdays[4] = nl_langinfo(ABDAY_4);
    wdays[5] = nl_langinfo(ABDAY_5);
    wdays[6] = nl_langinfo(ABDAY_6);
    wdays[7] = nl_langinfo(ABDAY_7);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    for (i = 1; i <= 7; i++) {
        priv->buttons[i] = gtk_toggle_button_new_with_label(wdays[day]);
        priv->dayorder[i] = day++;

        if (day > 7)
            day = 1;

        g_signal_connect(GTK_WIDGET(priv->buttons[i]),
                         "toggled", G_CALLBACK(button_toggle), picker);

        priv->last_index = i;
        gtk_size_group_add_widget(sgroup, priv->buttons[i]);

        gtk_widget_set_parent(priv->buttons[i], GTK_WIDGET(picker));
        gtk_widget_show(priv->buttons[i]);
    }

    GTK_WIDGET_SET_FLAGS(picker, GTK_NO_WINDOW);
}

/**
 * hildon_weekday_picker_new:
 *
 * Creates a new #HildonWeekdayPicker.
 *
 * Return value: Pointer to a new @HildonWeekdayPicker widget.
 */
GtkWidget *hildon_weekday_picker_new(void)
{
    return g_object_new(HILDON_WEEKDAY_PICKER_TYPE, NULL);
}

static gboolean
hildon_weekday_picker_mnemonic_activate(GtkWidget *widget,
                                        gboolean group_cycling)
{
  HildonWeekdayPickerPrivate *priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(widget);
  gtk_widget_grab_focus(priv->buttons[1]);
  return TRUE;
}

static void 
hildon_weekday_picker_forall(GtkContainer * container,
                             gboolean include_internals, GtkCallback callback,
                             gpointer callback_data)
{
    HildonWeekdayPicker *picker;
    HildonWeekdayPickerPrivate *priv;
    gint i;

    g_return_if_fail(container);
    g_return_if_fail(callback);

    picker = HILDON_WEEKDAY_PICKER(container);
    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    if (!include_internals)
        return;

    for (i = 1; i <= 7; ++i) {
        (*callback) (priv->buttons[i], callback_data);
    }

}

static void 
hildon_weekday_picker_destroy(GtkObject * self)
{
    HildonWeekdayPickerPrivate *priv;
    gint i;

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(self);

    for (i = 1; i <= 7; ++i) {
      if (priv->buttons[i])
      {
        gtk_widget_unparent(priv->buttons[i]);
        priv->buttons[i] = NULL;
      }
    }

    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        GTK_OBJECT_CLASS(parent_class)->destroy(self);

}

static void 
hildon_weekday_picker_size_request(GtkWidget * widget,
                                   GtkRequisition *requisition)
{
    HildonWeekdayPicker *picker;
    HildonWeekdayPickerPrivate *priv;
    gint i;
    GtkRequisition req;

    picker = HILDON_WEEKDAY_PICKER(widget);
    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);
    requisition->width = 0;
    requisition->height = 0;

    for (i = 1; i <= 7; ++i) {
        gtk_widget_size_request(priv->buttons[i], &req);
        requisition->width += req.width;
        if (req.height > requisition->height)
            requisition->height = req.height;

    }
}

static void 
hildon_weekday_picker_size_allocate(GtkWidget * widget,
                                                GtkAllocation * allocation)
{
    HildonWeekdayPicker *picker;
    HildonWeekdayPickerPrivate *priv;
    gint i;
    GtkAllocation alloc;
    GtkRequisition child_requisition;
    gint header_x;
    guint sval;
    GtkTextDirection direction;

    g_return_if_fail(widget);
    g_return_if_fail(allocation);

    /* Check orientation */
    direction = gtk_widget_get_direction(widget);

    picker = HILDON_WEEKDAY_PICKER(widget);
    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);
    header_x = allocation->x;
    widget->allocation = *allocation;

    if (direction == GTK_TEXT_DIR_LTR || direction == GTK_TEXT_DIR_NONE)
        sval = 1;
    else
        sval = 7;
    for (i = 1; i <= 7; ++i) {
        gtk_widget_get_child_requisition(priv->buttons[sval],
                                         &child_requisition);

        alloc.x = header_x;
        alloc.y = allocation->y;
        alloc.width = child_requisition.width;
        alloc.height = child_requisition.height;
        header_x += alloc.width;
        gtk_widget_size_allocate(priv->buttons[sval], &alloc);
        if (direction == GTK_TEXT_DIR_RTL)
            sval--;
        else
            sval++;
    }
}

static void
button_toggle(GtkToggleButton * button, gpointer data)
{
    HildonWeekdayPicker *picker;
    HildonWeekdayPickerPrivate *priv;
    gint i;
    static guint wdays[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    g_return_if_fail(button);
    g_return_if_fail(data);

    picker = HILDON_WEEKDAY_PICKER(data);
    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    for (i = 1; i <= 7; ++i) {
        if (GTK_WIDGET(button) == priv->buttons[i]) {
            priv->days ^= wdays[i];
            g_signal_emit (GTK_WIDGET(picker), signals[SELECTION_CHANGED_SIGNAL],
                   0, priv->dayorder[i]);
            break;
        }
    }
}

/**
 * hildon_weekday_picker_set_day:
 * @picker: the @HildonWeekdayPicker widget
 * @day: Day to be set
 *
 * Select specified weekday.
 */
void 
hildon_weekday_picker_set_day(HildonWeekdayPicker * picker,
                                   GDateWeekday day)
{
    HildonWeekdayPickerPrivate *priv;
    static guint wdays[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    guint i;

    g_return_if_fail(picker);
    g_return_if_fail(day >= 1 && day <= 7);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    if (day == G_DATE_SUNDAY)
        day = G_DATE_MONDAY;
    else
        day++;

    for (i = 1; i <= 7; i++) {
        if (priv->dayorder[i] == day) {
            if (!(priv->days & wdays[i]))
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                             (priv->buttons[i]), TRUE);
        }
    }
}

/**
 * hildon_weekday_picker_unset_day:
 * @picker: the @HildonWeekdayPicker widget
 * @day: Day to be unset
 *
 * Unselect specified weekday.
 */
void 
hildon_weekday_picker_unset_day(HildonWeekdayPicker * picker,
                                     GDateWeekday day)
{
    HildonWeekdayPickerPrivate *priv;
    static guint wdays[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
    guint i;

    g_return_if_fail(picker);
    g_return_if_fail(day >= 1 && day <= 7);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    if (day == G_DATE_SUNDAY)
        day = G_DATE_MONDAY;
    else
        day++;

    for (i = 1; i <= 7; i++) {
        if (priv->dayorder[i] == day) {
            if (priv->days & wdays[i])
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                             (priv->buttons[i]), FALSE);
        }
    }
}

/**
 * hildon_weekday_picker_toggle_day:
 * @picker: the @HildonWeekdayPicker widget
 * @day: Day to be toggled
 *
 * Toggle current status of the specified weekday.
 */
void 
hildon_weekday_picker_toggle_day(HildonWeekdayPicker * picker,
                                      GDateWeekday day)
{
    HildonWeekdayPickerPrivate *priv;
    guint i;
    gboolean is_active;
   
    g_return_if_fail(picker);
    g_return_if_fail(day >= 1 && day <= 7);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    if (day == G_DATE_SUNDAY)
        day = G_DATE_MONDAY;
    else
        day++;

    for (i = 1; i <= 7; i++) {
        if (priv->dayorder[i] == day) {
            is_active = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
					(priv->buttons[i]));
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                         (priv->buttons[i]),
                                         is_active ? FALSE : TRUE);
        }
    }
}

/**
 * hildon_weekday_picker_set_all:
 * @picker: the @HildonWeekdayPicker widget
 *
 * Set all weekdays.
 */
void 
hildon_weekday_picker_set_all(HildonWeekdayPicker * picker)
{
    HildonWeekdayPickerPrivate *priv;
    gint i;
    static guint wdays[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    g_return_if_fail(picker);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    for (i = 1; i <= 7; i++) {
        if (!(priv->days & wdays[i]))
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                         (priv->buttons[i]), TRUE);
    }
}

/**
 * hildon_weekday_picker_unset_all:
 * @picker: the @HildonWeekdayPicker widget
 *
 * Unset all weekdays.
 */
void 
hildon_weekday_picker_unset_all(HildonWeekdayPicker * picker)
{
    HildonWeekdayPickerPrivate *priv;
    gint i;
    static guint wdays[] = { 1, 2, 4, 8, 16, 32, 64, 128 };

    g_return_if_fail(picker);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    for (i = 1; i <= 7; i++) {
        if (priv->days & wdays[i])
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
                                         (priv->buttons[i]), FALSE);
    }
}

/**
 * hildon_weekday_picker_set_all:
 * @picker: the @HildonWeekdayPicker widget
 * @day: Day to be checked.
 *
 * Check if the specified weekday is set.
 *
 * Return value: TRUE if the day is set. 
 */
gboolean 
hildon_weekday_picker_isset_day(HildonWeekdayPicker * picker,
                                         GDateWeekday day)
{
    HildonWeekdayPickerPrivate *priv;
    guint i;

    g_return_val_if_fail(picker, FALSE);
    g_return_val_if_fail(day >= 1 && day <= 7, FALSE);

    priv = HILDON_WEEKDAY_PICKER_GET_PRIVATE(picker);

    if (day == G_DATE_SUNDAY)
        day = G_DATE_MONDAY;
    else
        day++;

    for (i = 1; i <= 7; i++) {
        if ( priv->dayorder[i] == day && 
             gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
                                         (priv->buttons[i]))) {
             return TRUE;
        }
    }
    return FALSE;
}
