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

/**
 * SECTION:hildon-telephone-editor.c
 * @short_description: A widget which allows users to enter telephone numbers 
 *
 * A single-line editor which allows user to enter a telephone number. 
 * There are two modes to choose from (coerce/free format). 
 * Coerce format has three fields which are placed horizontally. 
 * The fields are: country code, area code and number. When nothing is 
 * entered in the fields, assisting text is displayed. Tapping on the 
 * field highlights the field and allows users to input numbers.
 *
 * The widget is used where a user should enter a phone number. Free format 
 * should be used wherever possible as it enables the user to enter the 
 * phone number in the format she likes. Free format also supports DTMF 
 * strings as a part of the phone number. The format can not be changed 
 * at runtime.  
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <libintl.h>
#include <string.h>
#include "hildon-telephone-editor.h"
#include "hildon-composite-widget.h"

#define _(String) gettext (String)
#define HILDON_TELEPHONE_EDITOR_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
        HILDON_TELEPHONE_EDITOR_TYPE, HildonTelephoneEditorPriv));

#define AREA_LEN 4
#define COUNTRY_LEN 7
#define NUMBER_LEN 10

#define AREA_STR "Area"
#define COUNTRY_STR "Country"
#define NUMBER_STR "Number"

typedef struct _HildonTelephoneEditorPriv HildonTelephoneEditorPriv;

enum {
    PROP_0,
    PROP_COUNTRY_STR,
    PROP_AREA_STR,
    PROP_NUMBER_STR,
    PROP_SHOW_BORDER,
    PROP_SHOW_PLUS,
    PROP_FORMAT
};

/*< private >*/
struct _HildonTelephoneEditorPriv {
    HildonTelephoneEditorFormat format; /* format of the editor */

    gboolean edited_country;    /* is the country edited */
    gboolean edited_area;       /* is the area edited */
    gboolean edited_number;     /* is the number edited */

    gboolean editable_country;  /* is the country editable */
    gboolean editable_area;     /* is the area editable */
    gboolean editable_number;   /* is the number editable */

    gboolean show_plus; /* show '+' sign in country */
    gboolean show_border;       /* show border around the widget */

    GtkWidget *frame;   /* frame for border lines */
    GtkWidget *plus;    /* + in front of country code */
    GtkWidget *plus_event;      /* event box for the + -label */
    GtkWidget *country; /* country code */
    GtkWidget *area;    /* area code */
    GtkWidget *number;  /* telephone number */

    gint j;
};


static GtkContainerClass *parent_class;

static void
hildon_telephone_editor_class_init(HildonTelephoneEditorClass *
                                   editor_class);

static void 
hildon_telephone_editor_init(HildonTelephoneEditor * editor);

static void
hildon_telephone_editor_forall(GtkContainer * container,
                               gboolean include_internals,
                               GtkCallback callback,
                               gpointer callback_data);
static void 
hildon_telephone_editor_destroy(GtkObject * self);

static void
hildon_telephone_editor_size_allocate(GtkWidget * widget,
                                      GtkAllocation * allocation);

static void
hildon_telephone_editor_size_request(GtkWidget * widget,
                                     GtkRequisition * requisition);

static gboolean
hildon_telephone_editor_button_release(GtkWidget * widget,
                                     GdkEventButton * event,
                                     gpointer data);
static void
hildon_telephone_editor_focus_dest_entry(GtkWidget * widget,
                                         gboolean edited,
                                         gboolean move_left);
static gboolean
hildon_telephone_editor_entry_keypress(GtkWidget * widget,
                                       GdkEventKey * event, gpointer data);

static gboolean
hildon_telephone_editor_mnemonic_activate( GtkWidget *widget,
                                           gboolean group_cycling);

static void
hildon_telephone_editor_set_property(GObject * object,
                                     guint prop_id,
                                     const GValue * value,
                                     GParamSpec * pspec);
static void
hildon_telephone_editor_get_property(GObject * object,
                                     guint prop_id,
                                     GValue * value, GParamSpec * pspec);

GType hildon_telephone_editor_get_type(void)
{
    static GType editor_type = 0;

    if (!editor_type) {
        static const GTypeInfo editor_info = {
            sizeof(HildonTelephoneEditorClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_telephone_editor_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonTelephoneEditor),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_telephone_editor_init,
        };
        editor_type = g_type_register_static(GTK_TYPE_CONTAINER,
                                             "HildonTelephoneEditor",
                                             &editor_info, 0);
    }
    return editor_type;
}

static void
hildon_telephone_editor_class_init(HildonTelephoneEditorClass *
                                   editor_class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(editor_class);
    GtkContainerClass *container_class = GTK_CONTAINER_CLASS(editor_class);
    GObjectClass *object_class = G_OBJECT_CLASS(editor_class);

    parent_class = g_type_class_peek_parent(editor_class);

    g_type_class_add_private(editor_class, sizeof(HildonTelephoneEditorPriv));

    widget_class->size_request = hildon_telephone_editor_size_request;
    widget_class->size_allocate = hildon_telephone_editor_size_allocate;
    container_class->forall = hildon_telephone_editor_forall;
    widget_class->mnemonic_activate = hildon_telephone_editor_mnemonic_activate;
    widget_class->focus = hildon_composite_widget_focus;
    object_class->set_property = hildon_telephone_editor_set_property;
    object_class->get_property = hildon_telephone_editor_get_property;

    GTK_OBJECT_CLASS(editor_class)->destroy = hildon_telephone_editor_destroy;

    /* Install properties */
    g_object_class_install_property(object_class, PROP_COUNTRY_STR,
        g_param_spec_string("country",
                            ("Country string"),
                            ("Country string"),
                            COUNTRY_STR, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_AREA_STR,
        g_param_spec_string("area",
                            ("Area string"),
                            ("Area string"),
                            AREA_STR, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_NUMBER_STR,
        g_param_spec_string("number",
                            ("Number string"),
                            ("Number string"),
                            NUMBER_STR, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_SHOW_BORDER,
        g_param_spec_boolean ("show-border",
			      "Show Border",
			      "Wether to show the border around the widget",
			      TRUE, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_SHOW_PLUS,
        g_param_spec_boolean ("show-plus",
			      "Show Plus",
			      "Wether to show the plus sign in front of"
			      " coerce format's country field",
			      TRUE, G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_FORMAT,
        g_param_spec_int("set-format",
                         ("Format"),
                         ("Sets telephoneditor format"),
                         HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
                         HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA,
                         0, G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));
}

static void
hildon_telephone_editor_init(HildonTelephoneEditor * editor)
{
    HildonTelephoneEditorPriv *priv;
    gboolean use_frames = FALSE;       /* in entries, for debug purposes */

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    priv->frame = gtk_frame_new(NULL);
    priv->plus = gtk_label_new("+");
    priv->plus_event = gtk_event_box_new();
    priv->country = gtk_entry_new();
    priv->area = gtk_entry_new();
    priv->number = gtk_entry_new();
    priv->edited_country = FALSE;
    priv->edited_area = FALSE;
    priv->edited_number = FALSE;
    priv->show_plus = TRUE;
    priv->show_border = TRUE;
    priv->editable_country = TRUE;
    priv->editable_area = TRUE;
    priv->editable_number = TRUE;

    gtk_container_add(GTK_CONTAINER(priv->plus_event), priv->plus);

    gtk_entry_set_has_frame(GTK_ENTRY(priv->country), use_frames);
    gtk_entry_set_has_frame(GTK_ENTRY(priv->area), use_frames);
    gtk_entry_set_has_frame(GTK_ENTRY(priv->number), use_frames);

    gtk_widget_set_parent(priv->frame, GTK_WIDGET(editor));
    gtk_widget_set_parent(priv->plus_event, GTK_WIDGET(editor));
    
    hildon_telephone_editor_set_widths(editor, COUNTRY_LEN, AREA_LEN,
                                       NUMBER_LEN);

    /* set signals for country entry */
    gtk_signal_connect(GTK_OBJECT(priv->country), "button_release_event",
                       G_CALLBACK(hildon_telephone_editor_button_release),
                       editor);

    gtk_signal_connect(GTK_OBJECT(priv->country), "key-press-event",
                       G_CALLBACK(hildon_telephone_editor_entry_keypress),
                       editor);

    /* set signals for plus label */
    gtk_signal_connect(GTK_OBJECT(priv->plus_event), "button_press_event",
                       G_CALLBACK(hildon_telephone_editor_button_release),
                       editor);

    /* set signals for area entry */
    gtk_signal_connect(GTK_OBJECT(priv->area), "button_release_event",
                       G_CALLBACK(hildon_telephone_editor_button_release),
                       editor);

    gtk_signal_connect(GTK_OBJECT(priv->area), "key-press-event",
                       G_CALLBACK(hildon_telephone_editor_entry_keypress),
                       editor);

    /* set signals for number entry */
    gtk_signal_connect(GTK_OBJECT(priv->number), "button_release_event",
                       G_CALLBACK(hildon_telephone_editor_button_release),
                       editor);

    gtk_signal_connect(GTK_OBJECT(priv->number), "key-press-event",
                       G_CALLBACK(hildon_telephone_editor_entry_keypress),
                       editor);

    GTK_WIDGET_SET_FLAGS(editor, GTK_NO_WINDOW);

    gtk_widget_show(priv->number);
    gtk_widget_show(priv->area);
    gtk_widget_show(priv->country);
    gtk_widget_show_all(priv->frame);
    gtk_widget_show_all(priv->plus_event);
}

/**
 * hildon_telephone_editor_new:
 * @format: format of the editor
 *
 * Creates a new #HildonTelephoneEditor. The editor can be in a free
 * format where the user can type in country code, area code and the
 * phone number and can type in other characters than just digits. In
 * coerse format the editor has fields where the user can input only
 * digits. See #HildonTelephoneEditorFormat for the different coerse
 * formats.
 * 
 * Returns: new #HildonTelephoneEditor
 */
GtkWidget *
hildon_telephone_editor_new(HildonTelephoneEditorFormat format)
{

    HildonTelephoneEditor *editor;

    editor =
        g_object_new(HILDON_TELEPHONE_EDITOR_TYPE, "set-format", format,
                     "set-country", _(COUNTRY_STR), "set-area",
                     _(AREA_STR), "set-number", _(NUMBER_STR), NULL);

    return GTK_WIDGET(editor);
}

/**
 * hildon_telephone_editor_new_with_strings:
 * @format: format of the editor
 * @country: default text for the country field
 * @area: default text for the area field
 * @number: default text for the number field
 *
 * Creates a new #HildonTelephoneEditor. See hildon_telephone_editor_new
 * for details.
 * 
 * Returns: new #HildonTelephoneEditor
 */
GtkWidget *
hildon_telephone_editor_new_with_strings(HildonTelephoneEditorFormat
                                              format,
                                              const gchar * country,
                                              const gchar * area,
                                              const gchar * number)
{
    HildonTelephoneEditor *editor;

    editor =
        g_object_new(HILDON_TELEPHONE_EDITOR_TYPE, "set-format", format,
                     "set-country", country, "set-area", area,
                     "set-number", number, NULL);

    return GTK_WIDGET(editor);
}

static void 
hildon_telephone_editor_get_property(GObject * object,
                                                 guint prop_id,
                                                 GValue * value,
                                                 GParamSpec * pspec)
{
    HildonTelephoneEditor *editor;
    HildonTelephoneEditorPriv *priv;

    editor = HILDON_TELEPHONE_EDITOR (object);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_COUNTRY_STR:
        g_value_set_string(value,
		hildon_telephone_editor_get_country(editor));
        break;
    case PROP_AREA_STR:
        g_value_set_string(value, 
		hildon_telephone_editor_get_area(editor));
        break;
    case PROP_NUMBER_STR:
        g_value_set_string(value,
		    hildon_telephone_editor_get_number(editor));
        break;
    case PROP_SHOW_BORDER:
        g_value_set_boolean(value, 
		hildon_telephone_editor_get_show_border(editor));
        break;
    case PROP_SHOW_PLUS:
        g_value_set_boolean(value, 
		hildon_telephone_editor_get_show_plus(editor));
        break;
    case PROP_FORMAT:
        g_value_set_int(value, priv->format);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void 
hildon_telephone_editor_set_property(GObject * object,
                                                 guint prop_id,
                                                 const GValue * value,
                                                 GParamSpec * pspec)
{
    HildonTelephoneEditor *editor;
    HildonTelephoneEditorPriv *priv;
   
    editor = HILDON_TELEPHONE_EDITOR (object);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(object);

    switch (prop_id) {
    case PROP_COUNTRY_STR:
        hildon_telephone_editor_set_country(editor,
                           _(g_value_get_string(value)));
        break;
    case PROP_AREA_STR:
        hildon_telephone_editor_set_area(editor,
                           _(g_value_get_string(value)));
        break;
    case PROP_NUMBER_STR:
        hildon_telephone_editor_set_number(editor,
                           _(g_value_get_string(value)));
        break;
    case PROP_SHOW_BORDER:
        hildon_telephone_editor_set_show_border(
		editor, g_value_get_boolean(value));
        break;
    case PROP_SHOW_PLUS:
        hildon_telephone_editor_set_show_plus(
		editor, g_value_get_boolean(value));
        break;
    case PROP_FORMAT:
        priv->format = g_value_get_int(value);
        if (priv->format != HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
            gtk_entry_set_text(GTK_ENTRY(priv->number), _(NUMBER_STR));

        /* set proper fields according to selected format */
        switch (priv->format) {
        case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
            gtk_widget_set_parent(priv->number, GTK_WIDGET(object));
        case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
            gtk_widget_set_parent(priv->area, GTK_WIDGET(object));
        case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
            gtk_widget_set_parent(priv->country, GTK_WIDGET(object));
            break;
        case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
            gtk_widget_set_parent(priv->number, GTK_WIDGET(object));
            break;
        default:
            g_assert_not_reached();
        }
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static gboolean
hildon_telephone_editor_mnemonic_activate( GtkWidget *widget,
                                  gboolean group_cycling)
{
  HildonTelephoneEditorPriv *priv;
  priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(widget);

  if (!GTK_CONTAINER(widget)->focus_child)
  {
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
    {
      gtk_widget_grab_focus(priv->number);

      if (priv->editable_number && !priv->edited_number)
        gtk_editable_select_region(GTK_EDITABLE(priv->number), 0, -1);
    }
    else
    {
      if (priv->editable_country)
      {
        gtk_widget_grab_focus(priv->country);
        if (!priv->edited_country)
          gtk_editable_select_region(GTK_EDITABLE(priv->country), 0, -1);
      }
      else if ((priv->editable_area) && (priv->format !=
               HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY))
      {
        gtk_widget_grab_focus(priv->area);
        if (!priv->edited_area)
          gtk_editable_select_region(GTK_EDITABLE(priv->area), 0, -1);
      }
      else if ((priv->editable_number) && (priv->format ==
               HILDON_TELEPHONE_EDITOR_FORMAT_COERCE))
      {
        gtk_widget_grab_focus(priv->number);
        if (!priv->edited_number)
          gtk_editable_select_region(GTK_EDITABLE(priv->number), 0, -1);
      }
      else
        gtk_widget_grab_focus(priv->country);
    }
  }
  return TRUE;
}

static void 
hildon_telephone_editor_forall(GtkContainer * container,
                                           gboolean include_internals,
                                           GtkCallback callback,
                                           gpointer callback_data)
{
    HildonTelephoneEditor *editor;
    HildonTelephoneEditorPriv *priv;

    editor = HILDON_TELEPHONE_EDITOR(container);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    g_return_if_fail(container);
    g_return_if_fail(callback);

    if (!include_internals)
        return;

    switch (priv->format) {
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
        (*callback) (priv->number, callback_data);
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
        (*callback) (priv->area, callback_data);
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
        (*callback) (priv->country, callback_data);
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
        (*callback) (priv->number, callback_data);
        break;
    default:
        g_return_if_reached();
    }

    if (priv->show_plus)
        (*callback) (priv->plus_event, callback_data);

    if (priv->show_border)
        (*callback) (priv->frame, callback_data);

}

static void 
hildon_telephone_editor_destroy(GtkObject * self)
{
    HildonTelephoneEditorPriv *priv;

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(self);

    switch (priv->format) {
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
        if (priv->number) {
            gtk_widget_unparent(priv->number);
            priv->number = NULL;
        }
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
        if (priv->area) {
            gtk_widget_unparent(priv->area);
            priv->area = NULL;
        }
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
        if (priv->country) {
            gtk_widget_unparent(priv->country);
            priv->country = NULL;
        }
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
        if (priv->number) {
            gtk_widget_unparent(priv->number);
            priv->number = NULL;
        }
        break;
    default:
        g_return_if_reached();
    }

    if (priv->plus_event) {
        gtk_widget_unparent(priv->plus_event);
        priv->plus_event = NULL;
    }
    if (priv->frame) {
        gtk_widget_unparent(priv->frame);
        priv->frame = NULL;
    }
    if (GTK_OBJECT_CLASS(parent_class)->destroy)
        GTK_OBJECT_CLASS(parent_class)->destroy(self);
}

static void 
hildon_telephone_editor_size_request(GtkWidget *widget,
                                     GtkRequisition *requisition)
{
    HildonTelephoneEditor *editor;
    HildonTelephoneEditorPriv *priv;
    GtkRequisition req;

    g_return_if_fail(requisition);

    editor = HILDON_TELEPHONE_EDITOR(widget);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    if (priv->show_border)
        gtk_widget_size_request(priv->frame, requisition);

    requisition->width = 0;
    requisition->height = 0;

    switch (priv->format)
    {
      case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
        gtk_widget_size_request(priv->number, &req);
        requisition->width += req.width;
        if (requisition->height < req.height)
            requisition->height = req.height;

      case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
        gtk_widget_size_request(priv->area, &req);
        requisition->width += req.width;
        if (requisition->height < req.height)
            requisition->height = req.height;

      case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
        gtk_widget_size_request(priv->country, &req);
        requisition->width += req.width;
        if (requisition->height < req.height)
            requisition->height = req.height;

        break;
      case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
        gtk_widget_size_request(priv->number, &req);
        requisition->width += req.width;
        if (requisition->height < req.height)
            requisition->height = req.height;

        break;
      default:
        g_return_if_reached();
    }

    if (priv->show_plus)
    {
        gtk_widget_size_request(priv->plus_event, &req);
        requisition->width += req.width;
        if (requisition->height < req.height)
            requisition->height = req.height;
    }

    requisition->width += GTK_CONTAINER(priv->frame)->border_width * 2;
    requisition->height += GTK_CONTAINER(priv->frame)->border_width * 2;
}

static void 
hildon_telephone_editor_size_allocate(GtkWidget *widget,
                                      GtkAllocation *allocation)
{
    HildonTelephoneEditor *editor;
    HildonTelephoneEditorPriv *priv;
    gint header_x = 6, frame_w = 12;
    GtkAllocation alloc, country_alloc, area_alloc, number_alloc;
    GtkRequisition requisition, country_requisition, area_requisition,
        number_requisition;

    g_return_if_fail(widget);
    g_return_if_fail(allocation);

    if (allocation->height < 0 )
      allocation->height = 0;
    
    widget->allocation = *allocation;
    
    header_x += allocation->x;
    editor = HILDON_TELEPHONE_EDITOR(widget);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);


    if (priv->show_plus &&
        priv->format != HILDON_TELEPHONE_EDITOR_FORMAT_FREE) {
        gtk_widget_get_child_requisition(priv->plus_event, &requisition);

        alloc.x = header_x;
        alloc.y = allocation->y + 1;
        alloc.width = requisition.width;
        alloc.height = requisition.height;

        header_x += alloc.width;
        gtk_widget_size_allocate(priv->plus_event, &alloc);
        frame_w += alloc.width;
    }
    
    gtk_widget_get_child_requisition(priv->number, &number_requisition);

    number_alloc.width = number_requisition.width;

    number_alloc.height = allocation->height - 4 -
                          GTK_CONTAINER(priv->frame)->border_width * 2;

    /* get sizes */
    switch (priv->format) {
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
        gtk_widget_get_child_requisition(priv->area, &area_requisition);

        area_alloc.width = area_requisition.width;
        area_alloc.height = number_alloc.height;
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
        gtk_widget_get_child_requisition(priv->country, &country_requisition);

        country_alloc.width = country_requisition.width;
        country_alloc.height = number_alloc.height;
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
        break;
    default:
        g_return_if_reached();
    }


    /* allocate in coerce formats */
    switch (priv->format) {
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
        number_alloc.x = header_x + country_alloc.width + area_alloc.width;
        number_alloc.y = allocation->y + 1;

        gtk_widget_size_allocate(priv->number, &number_alloc);
        frame_w += number_alloc.width;
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
        area_alloc.x = header_x + country_alloc.width;
        area_alloc.y = allocation->y + 1;

        gtk_widget_size_allocate(priv->area, &area_alloc);
        frame_w += area_alloc.width;
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
        country_alloc.x = header_x;
        country_alloc.y = allocation->y + 1;

        gtk_widget_size_allocate(priv->country, &country_alloc);
        frame_w += country_alloc.width;
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
        number_alloc.x = header_x;
        number_alloc.y = allocation->y + 1;

        gtk_widget_size_allocate(priv->number, &number_alloc);
        frame_w += number_alloc.width;
        break;
    default:
        g_return_if_reached();
    }

    if (priv->show_border)
    {
        alloc.x = allocation->x;
        alloc.y = allocation->y;

        alloc.width = frame_w - 4;

        alloc.height = allocation->height;
        gtk_widget_size_allocate( GTK_WIDGET(priv->frame), &alloc );
    }
}

static gboolean
hildon_telephone_editor_button_release(GtkWidget * widget,
                                     GdkEventButton * event, gpointer data)
{
  gtk_editable_select_region(GTK_EDITABLE(widget), 0, -1);
  return FALSE;
}

static void 
hildon_telephone_editor_focus_dest_entry(GtkWidget * widget,
                                                     gboolean edited,
                                                     gboolean move_left)
{
    /* full coerse mode, wrap to field */
    gtk_widget_grab_focus(widget);

    if (move_left)
        gtk_editable_set_position(GTK_EDITABLE(widget), -1);
    else
        gtk_editable_set_position(GTK_EDITABLE(widget), 0);
    /* new field not yet edited, select all */
    if (!edited)
        gtk_editable_select_region(GTK_EDITABLE(widget), 0, -1);
}

static gboolean
hildon_telephone_editor_entry_keypress(GtkWidget * widget,
                                       GdkEventKey * event, gpointer data)
{
    HildonTelephoneEditor *editor;
    HildonTelephoneEditorPriv *priv;
    gint pos, width;
    gint cpos, start, end;

    g_return_val_if_fail(widget, FALSE);
    g_return_val_if_fail(event, FALSE);
    g_return_val_if_fail(data, FALSE);

    editor = HILDON_TELEPHONE_EDITOR(data);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);
    cpos = gtk_editable_get_position(GTK_EDITABLE(widget));
    gtk_editable_get_selection_bounds(GTK_EDITABLE(widget), &start, &end);

    if( event->keyval == GDK_Up || event->keyval == GDK_KP_Up ||
        event->keyval == GDK_Down || event->keyval == GDK_KP_Down )
      return FALSE;

    /* Wrap around should not happen; that's why following code */
    switch (priv->format) {
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE:
        if (((event->keyval == GDK_Left || event->keyval == GDK_KP_Left) &&
             (widget == priv->country) && ((cpos == 0) ||
             ((start == 0 && end == strlen(GTK_ENTRY(priv->country)->text)))))
            ||
            ((event->keyval == GDK_Right || event->keyval == GDK_KP_Right)
             && (widget == priv->number)
             && (cpos >= strlen(GTK_ENTRY(priv->number)->text))))
            return TRUE;
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA:
        if (((event->keyval == GDK_Left || event->keyval == GDK_KP_Left) &&
             (widget == priv->country) && ((cpos == 0) ||
                                           ((start == 0
                                             && end ==
                                             strlen(GTK_ENTRY
                                                    (priv->country)->
                                                    text)))))
            ||
            ((event->keyval == GDK_Right || event->keyval == GDK_KP_Right)
             && (widget == priv->area)
             && (cpos >= strlen(GTK_ENTRY(priv->area)->text))))
            return TRUE;
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY:
        break;
    case HILDON_TELEPHONE_EDITOR_FORMAT_FREE:
        break;
    }

    /* valid key pressed */
    if (event->keyval == GDK_Left || event->keyval == GDK_KP_Left ||
        event->keyval == GDK_Right || event->keyval == GDK_KP_Right ||
        event->keyval == GDK_BackSpace || event->keyval == GDK_Delete ||
        event->keyval == GDK_KP_Delete ||
        (event->keyval >= GDK_0 && event->keyval <= GDK_9) ||
        (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE &&
         (event->keyval == GDK_slash || event->keyval == GDK_space ||
          event->keyval == GDK_parenleft || event->keyval == GDK_parenright
          || event->keyval == GDK_period || event->keyval == GDK_hyphen
          || event->keyval == GDK_plus || event->keyval == GDK_asterisk
          || event->keyval == GDK_KP_Multiply
          || event->keyval == GDK_KP_Add || event->keyval == GDK_numbersign
          || event->keyval == GDK_question || event->keyval == GDK_KP_Subtract
          || event->keyval == GDK_minus)))
    {
        if ((event->keyval >= GDK_0 && event->keyval <= GDK_9) ||
            (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE &&
             (event->keyval == GDK_slash || event->keyval == GDK_space ||
              event->keyval == GDK_parenleft ||
              event->keyval == GDK_parenright ||
              event->keyval == GDK_period || event->keyval == GDK_hyphen ||
              event->keyval == GDK_plus || event->keyval == GDK_asterisk ||
              event->keyval == GDK_KP_Add ||
              event->keyval == GDK_KP_Multiply ||
              event->keyval == GDK_numbersign ||
              event->keyval == GDK_question ||
              event->keyval == GDK_KP_Subtract || 
              event->keyval == GDK_minus)))
        {
            if (widget == priv->country)
                priv->edited_country = TRUE;
            if (widget == priv->area)
                priv->edited_area = TRUE;
            if (widget == priv->number)
                priv->edited_number = TRUE;
        }

        pos = gtk_editable_get_position(GTK_EDITABLE(widget));

        /* going left */
        if (event->keyval == GDK_Left || event->keyval == GDK_KP_Left) {
            /* not yet on the left edge and the widget is edited */
            if (pos != 0 &&
                ((widget == priv->country && priv->edited_country == TRUE)
                 || (widget == priv->area && priv->edited_area == TRUE)
                 || (widget == priv->number
                     && priv->edited_number == TRUE)))
                return FALSE;

            /* left edge of number field */
            if (widget == priv->number) {
                /* Stop signal handler, if only number field exists */
                if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
                    return TRUE;
                else {
                    /* Grab the focus to the area field, if it is editable 
                     */
                    if (priv->editable_area) {
                        hildon_telephone_editor_focus_dest_entry
                            (priv->area, priv->edited_area, TRUE);
                    }
                    /* Grab the focus to the country field, if it is
                       editable */
                    else if (priv->editable_country) {
                        hildon_telephone_editor_focus_dest_entry
                            (priv->country, priv->edited_country, TRUE);
                    } else
                        return TRUE;
                }
            }

            /* left edge of area field */
            if (widget == priv->area) { /* grab the focus to the country
                                           field, if it is editable */
                if (priv->editable_country) {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->country, priv->edited_country, TRUE);
                } else
                    return TRUE;
            }

            /* left edge of country field, let's wrap */
            if (widget == priv->country) {
                /* Stop the signal handler, if only country field exists */
                if (priv->format ==
                    HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY)
                    return TRUE;
                /* wrap to area, if in AREA mode */
                else if (priv->format ==
                         HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA) {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->area, priv->edited_area, TRUE);
                }

                else {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->number, priv->edited_number, TRUE);
                }
            }
        }
        /* going right */
        else if (event->keyval == GDK_Right || event->keyval == GDK_KP_Right) {
            width = g_utf8_strlen(gtk_entry_get_text(GTK_ENTRY(widget)), -1);

            /* not yet on the rigth edge and the widget is edited */
            if (pos != width &&
                ((widget == priv->country && priv->edited_country == TRUE)
                 || (widget == priv->area && priv->edited_area == TRUE)
                 || (widget == priv->number
                     && priv->edited_number == TRUE)))
                return FALSE;

            /* rigth edge of number field */
            if (widget == priv->number) {
                /* Stop signal handler, if only number field exists */
                if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
                    return TRUE;
                else {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->country, priv->edited_country, FALSE);
                }
            }

            /* rigth edge of area field */
            if (widget == priv->area) {
                /* area mode, wrap to country field */
                if (priv->format ==
                    HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA) {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->country, priv->edited_country, FALSE);
                }
                /* grab the focus to the number field, if it is editable */
                else if (priv->editable_number) {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->number, priv->edited_number, FALSE);
                } else
                    return TRUE;
            }

            /* rigth edge of country field */
            if (widget == priv->country) {
                /* wrap around, if only country field exists */
                if (priv->format ==
                    HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY)
                    return TRUE;

                /* area mode, move to area field */
                else if (priv->format ==
                         HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA ||
                         priv->format ==
                         HILDON_TELEPHONE_EDITOR_FORMAT_COERCE) {
                    /* grab the focus to the area field, if it is editable 
                     */
                    if (priv->editable_area) {
                        hildon_telephone_editor_focus_dest_entry
                            (priv->area, priv->edited_area, FALSE);
                    }
                    /* grab the focus to the area field, if it is editable 
                     */
                    else if ((priv->format ==
                              HILDON_TELEPHONE_EDITOR_FORMAT_COERCE) &&
                             priv->editable_number) {
                        hildon_telephone_editor_focus_dest_entry
                            (priv->number, priv->edited_number, FALSE);
                    } else
                        return TRUE;
                }

                else {
                    hildon_telephone_editor_focus_dest_entry
                        (priv->number, priv->edited_number, FALSE);
                }
            }
        }
        return FALSE;
    }
    return TRUE;
}

/**
 * hildon_telephone_editor_set_editable:
 * @hte: #HildonTelephoneEditor
 * @country: set the editable status of the country field in coarce format
 * @area: set the editable status of the area field in coarce format
 * @number: set the editable status of the number field in coarce format
 * 
 * Specify wheter the fields in coerse format are editable or
 * not. This function is ignored if the editor is in free mode. The
 * number or area and number parameters are ignored if the editor is
 * in HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA or
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY formats,
 * respectively. 
 */
void 
hildon_telephone_editor_set_editable(HildonTelephoneEditor * editor,
                                          gboolean country,
                                          gboolean area, gboolean number)
{
    HildonTelephoneEditorPriv *priv;

    g_return_if_fail(editor);

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    priv->editable_country = country;
    priv->editable_area = area;
    priv->editable_number = number;

    if (priv->country)
    {
        gtk_editable_set_editable(GTK_EDITABLE(priv->country), country);
        gtk_widget_set_sensitive(GTK_WIDGET(priv->plus), country);
    }

    if (priv->area)
        gtk_editable_set_editable(GTK_EDITABLE(priv->area), area);

    if (priv->number)
        gtk_editable_set_editable(GTK_EDITABLE(priv->number), number);
}

/**
 * hildon_telephone_editor_set_widths:
 * @hte: #HildonTelephoneEditor
 * @country: width (characters) of the country field in coarce mode
 * @area: width (characters) of the area field in coarce mode
 * @number: width (characters) of the number field in coarce mode
 *
 * Set widths of the fields in coecse format. Country and area parameters
 * are ignored, if the editor is in free mode. The number or area and number
 * parameters are ignored if the editor is in
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA or
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY mode, respectively.
 */
void 
hildon_telephone_editor_set_widths(HildonTelephoneEditor * editor,
                                        guint country,
                                        guint area, guint number)
{
    HildonTelephoneEditorPriv *priv;

    g_return_if_fail(editor);

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    if (priv->country) {
        /*gtk_entry_set_max_length(GTK_ENTRY(priv->country), country);*/
        gtk_entry_set_width_chars(GTK_ENTRY(priv->country), country);
    }

    if (priv->area) {
        /*gtk_entry_set_max_length(GTK_ENTRY(priv->area), area);*/
        gtk_entry_set_width_chars(GTK_ENTRY(priv->area), area);
    }

    if (priv->number) {
        /*gtk_entry_set_max_length(GTK_ENTRY(priv->number), number);*/
        gtk_entry_set_width_chars(GTK_ENTRY(priv->number), number);
    }

}

/**
 * hildon_telephone_editor_set_show_plus:
 * @hte: #HildonTelephoneEditor
 * @show: show the plus sign
 *
 * Show or hide the plus sign in front of coerce format's country field. 
 */
void 
hildon_telephone_editor_set_show_plus(HildonTelephoneEditor * editor,
                                           gboolean show)
{
    HildonTelephoneEditorPriv *priv;

    g_return_if_fail(editor);

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    /* do nothing in FREE format */
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
        return;

    if (show && !priv->show_plus) {
        priv->show_plus = TRUE;
        gtk_widget_set_parent(priv->plus_event, GTK_WIDGET(editor));
        gtk_widget_show(priv->plus_event);
    } else if (!show && priv->show_plus) {
        gtk_widget_hide(priv->plus_event);
        gtk_widget_unparent(priv->plus_event);
        priv->show_plus = FALSE;
    }
}

/**
 * hildon_telephone_editor_get_show_plus:
 * @hte: #HildonTelephoneEditor
 *
 * Get the visibility status of the plus sign in
 * front of coerce format's country field.
 *
 * Returns: gboolean
 */
gboolean 
hildon_telephone_editor_get_show_plus(HildonTelephoneEditor *
                                               editor)
{
    HildonTelephoneEditorPriv *priv;
    g_return_val_if_fail(editor, FALSE);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    /* no plus in FREE format */
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
        return FALSE;
    return priv->show_plus;
}

/**
 * hildon_telephone_editor_set_show_border:
 * @hte: #HildonTelephoneEditor
 * @show: show the border
 *
 * Show or hide the border around the widget
 */
void 
hildon_telephone_editor_set_show_border(HildonTelephoneEditor *
                                             editor, gboolean show)
{
    HildonTelephoneEditorPriv *priv;
    g_return_if_fail(editor);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    if (show && !priv->show_border) {
        priv->show_border = TRUE;
        gtk_widget_set_parent(priv->frame, GTK_WIDGET(editor));
        gtk_widget_show(priv->frame);
    } else if (!show && priv->show_border) {
        gtk_widget_hide(priv->frame);
        gtk_widget_unparent(priv->frame);
        priv->show_border = FALSE;
    }
}

/**
 * hildon_telephone_editor_get_show_border:
 * @hte: #HildonTelephoneEditor
 *
 * Get the visibility status of the border around the widget.
 *
 * Returns: gboolean
 */
gboolean
hildon_telephone_editor_get_show_border(HildonTelephoneEditor * editor)
{
    HildonTelephoneEditorPriv *priv;
    g_return_val_if_fail(editor, FALSE);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    return priv->show_border;
}

/**
 * hildon_telephone_editor_get_country:
 * @hte: #HildonTelephoneEditor
 *
 * Get text in the country field in coarse format. This function must
 * not be called if in free mode.
 *
 * Returns: pointer to the text in the country field. It must not
 * be changed or freed.
 */
const gchar *
hildon_telephone_editor_get_country(HildonTelephoneEditor *
                                                 editor)
{
    HildonTelephoneEditorPriv *priv;
    g_return_val_if_fail(editor, NULL);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    /* return NULL if in FREE format */
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
        return NULL;

    return gtk_entry_get_text(GTK_ENTRY(priv->country));
}

/**
 * hildon_telephone_editor_set_country:
 * @hte: #HildonTelephoneEditor
 * @country: text to be set in country field
 *
 * Set text in the country field in coarse format. This function must
 * not be called if in free mode.
 */
void 
hildon_telephone_editor_set_country(HildonTelephoneEditor * editor,
                                         const gchar * country)
{
    HildonTelephoneEditorPriv *priv;
    const gchar *p;
    gunichar u;

    g_return_if_fail(editor);
    g_return_if_fail(country);

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    /* do nothing in FREE format */
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
        return;

    if (!priv->country)
        return;

    p = country;
    g_return_if_fail(g_utf8_validate(p, -1, NULL));

    /* allow only digits */
    do {
        u = g_utf8_get_char(p);
        if (!g_unichar_isdigit(*p))
            return;

        p = g_utf8_next_char(p);
    } while (*p);

    gtk_entry_set_text(GTK_ENTRY(priv->country), country);
}

/**
 * hildon_telephone_editor_get_area:
 * @hte: #HildonTelephoneEditor
 *
 * Get text in the area field in coarse format. This function must not
 * be called if in free mode or in
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY mode.
 *
 * Returns: pointer to the text in the area field. It must not be
 * changed or freed.
 */
const gchar *
hildon_telephone_editor_get_area(HildonTelephoneEditor *
                                              editor)
{
    HildonTelephoneEditorPriv *priv;
    g_return_val_if_fail(editor, NULL);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    /* return NULL if in FREE format or if in COERCE_COUNTRY format */
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
        return NULL;
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY)
        return NULL;

    return gtk_entry_get_text(GTK_ENTRY(priv->area));
}

/**
 * hildon_telephone_editor_set_area:
 * @hte: #HildonTelephoneEditor
 * @area: text to be set in area field
 *
 * Set text in the area field in coarse format. This function must not
 * be called if in free mode or in
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY mode.
 */
void
hildon_telephone_editor_set_area(HildonTelephoneEditor * editor,
                                      const gchar * area)
{
    HildonTelephoneEditorPriv *priv;
    const gchar *p;
    gunichar u;

    g_return_if_fail(editor);
    g_return_if_fail(area);

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    /* do nothing in FREE format */
    if (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE)
        return;

    if (!priv->area)
        return;

    p = area;
    g_return_if_fail(g_utf8_validate(p, -1, NULL));

    /* allow only digits */
    do {
        u = g_utf8_get_char(p);
        if (!g_unichar_isdigit(u))
            return;

        p = g_utf8_next_char(p);
    } while (*p);

    gtk_entry_set_text(GTK_ENTRY(priv->area), area);
}

/**
 * hildon_telephone_editor_get_number:
 * @hte: #HildonTelephoneEditor
 *
 * Get text in the number field in all formats. In free mode, this
 * functions returns the whole phone number. In coarce mode, it returns
 * only the number field. This function must not be called if
 * the editor is in HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA or
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY modes.
 *
 * Returns: pointer to text in the number field. It must not be
 * changed or freed.
 */
const gchar *
hildon_telephone_editor_get_number(HildonTelephoneEditor *
                                                editor)
{
    HildonTelephoneEditorPriv *priv;
    g_return_val_if_fail(editor, NULL);
    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    return gtk_entry_get_text(GTK_ENTRY(priv->number));
}

/**
 * hildon_telephone_editor_set_number:
 * @hte: #HildonTelephoneEditor
 * @number: text to be set to number field
 *
 * Set text in the number field in all formats. In free mode this
 * functions sets the whole phone number. In coerce mode, it sets
 * only the number field. This function must not be called if
 * the editor is in HILDON_TELEPHONE_EDITOR_FORMAT_COERSE_AREA or
 * HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY modes.
 */
void 
hildon_telephone_editor_set_number(HildonTelephoneEditor * editor,
                                        const gchar * number)
{
    HildonTelephoneEditorPriv *priv;
    const gchar *p;
    gunichar u;

    g_return_if_fail(editor);
    g_return_if_fail(number);

    priv = HILDON_TELEPHONE_EDITOR_GET_PRIVATE(editor);

    if (!priv->number)
        return;

    p = number;
    g_return_if_fail(g_utf8_validate(p, -1, NULL));

    /* allow only digits in coerce format or some other in free format */
    do {
        u = g_utf8_get_char(p);
        if (g_unichar_isdigit(u) ||
            (priv->format == HILDON_TELEPHONE_EDITOR_FORMAT_FREE &&
             (u == '+' ||
              u == 'p' ||
              u == 'w' ||
              u == '(' ||
              u == ')' ||
              u == '/' ||
              u == ' ' ||
              u == '.' || u == '-' || u == '*' || u == '#' || u == '?')))
            p = g_utf8_next_char(p);
        else
            return;

    } while (*p);

    gtk_entry_set_text(GTK_ENTRY(priv->number), number);
}
