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

/**
 * SECTION:hildon-color-button
 * @short_description: A widget to open HildonColorChooserDialog.
 * @see_also: #HildonColorChooserDialog, #HildonColorPopup
 *
 * HildonColorButton is a widget to open a HildonColorChooserDialog.
 * The selected color is shown in the button.
 * The selected color is a property of the button.
 * The property name is "color" and its type is GtkColor.
 * 
 * <example>
 * <title>HildonColorButton example</title>
 * <programlisting>
 * HildonColorButton *cbutton;
 * GtkColor *color;
 * <!-- -->
 * cbutton = hildon_color_button_new();
 * gtk_object_get( GTK_OBJECT(cbutton), "color", color );
 * </programlisting>
 * </example>
 * 
 */

#undef                                          HILDON_DISABLE_DEPRECATED

#ifdef                                          HAVE_CONFIG_H
#include                                        <config.h>
#endif

#include                                        <gdk/gdkkeysyms.h>

#include                                        "hildon-color-button.h"
#include                                        "hildon-defines.h"
#include                                        "hildon-color-chooser-dialog.h"
#include                                        "hildon-color-button-private.h"

#define                                         COLOR_FILLED_HEIGHT 22

#define                                         COLOR_FILLED_WIDTH 22

#define                                         COLOR_BUTTON_WIDTH 52

#define                                         COLOR_BUTTON_HEIGHT 48

#define                                         OUTER_BORDER_RED 0

#define                                         OUTER_BORDER_BLUE 0

#define                                         OUTER_BORDER_GREEN 0

#define                                         OUTER_BORDER_THICKNESS 1

#define                                         INNER_BORDER_RED 65535

#define                                         INNER_BORDER_BLUE 65535

#define                                         INNER_BORDER_GREEN 65535

#define                                         INNER_BORDER_THICKNESS 2

enum
{
    SETUP_DIALOG,
    LAST_SIGNAL
};

enum
{
    PROP_0,
    PROP_COLOR,
    PROP_POPUP_SHOWN
};

static void
hildon_color_button_class_init                  (HildonColorButtonClass *klass);

static void
hildon_color_button_init                        (HildonColorButton *color_button);

static void
hildon_color_button_finalize                    (GObject *object);

static void
hildon_color_button_set_property                (GObject *object, 
                                                 guint param_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec);

static void
hildon_color_button_get_property                (GObject *object, 
                                                 guint param_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec);

static void
hildon_color_button_realize                     (GtkWidget *widget);

static void
hildon_color_button_unrealize                   (GtkWidget *widget);

static void
hildon_color_button_clicked                     (GtkButton *button);

static gboolean
hildon_color_button_key_pressed                 (GtkWidget *button, 
                                                 GdkEventKey *event,
                                                 gpointer data);

static gint
hildon_color_field_expose_event                 (GtkWidget *widget, 
                                                 GdkEventExpose *event,
                                                 HildonColorButton *cb);

static gboolean
hildon_color_button_mnemonic_activate           (GtkWidget *widget,
                                                 gboolean group_cycling);

static void
draw_grid                                       (GdkDrawable *drawable, 
                                                 GdkGC *gc, 
                                                 int x, 
                                                 int y, 
                                                 gint w, 
                                                 gint h);

static gpointer                                 parent_class = NULL;
static guint                                    signals [LAST_SIGNAL] = { 0, };

/**
 * hildon_color_button_get_type:
 *
 * Initializes and returns the type of a hildon color button.
 *
 * @Returns: GType of #HildonColorButton.
 */
GType G_GNUC_CONST
hildon_color_button_get_type                    (void)
{
    static GType color_button_type = 0;

    if (! color_button_type)
    {
        static const GTypeInfo color_button_info =
        {
            sizeof (HildonColorButtonClass),
            NULL,           /* base_init */
            NULL,           /* base_finalize */
            (GClassInitFunc) hildon_color_button_class_init,
            NULL,           /* class_finalize */
            NULL,           /* class_data */
            sizeof (HildonColorButton),
            0,              /* n_preallocs */
            (GInstanceInitFunc) hildon_color_button_init,
        };

        color_button_type = g_type_register_static (GTK_TYPE_BUTTON, "HildonColorButton",
                &color_button_info, 0);
    }

    return color_button_type;
}

static void
hildon_color_button_class_init                  (HildonColorButtonClass *klass)
{
    GObjectClass *gobject_class;
    GtkButtonClass *button_class;
    GtkWidgetClass *widget_class;

    gobject_class = G_OBJECT_CLASS (klass);
    button_class = GTK_BUTTON_CLASS (klass);
    widget_class = GTK_WIDGET_CLASS (klass);
    
    parent_class = g_type_class_peek_parent (klass);

    gobject_class->get_property     = hildon_color_button_get_property;
    gobject_class->set_property     = hildon_color_button_set_property;
    gobject_class->finalize         = hildon_color_button_finalize;
    widget_class->realize           = hildon_color_button_realize;
    widget_class->unrealize         = hildon_color_button_unrealize;
    button_class->clicked           = hildon_color_button_clicked;
    widget_class->mnemonic_activate = hildon_color_button_mnemonic_activate;

   signals[SETUP_DIALOG] =
       g_signal_new ("setup-dialog",
                     G_TYPE_FROM_CLASS (klass),
                     G_SIGNAL_RUN_LAST,
                     0,
                     NULL, NULL,
                     g_cclosure_marshal_VOID__OBJECT,
                     G_TYPE_NONE, 1,
                     HILDON_TYPE_COLOR_CHOOSER_DIALOG);

    /**
     * HildonColorButton:color:
     *
     * The currently selected color.
     */
    g_object_class_install_property (gobject_class, PROP_COLOR,
            g_param_spec_boxed ("color",
                "Current Color",
                "The selected color",
                GDK_TYPE_COLOR,
                G_PARAM_READWRITE));

    /**
     * HildonColorButton:popup-shown:
     *
     * If the color selection dialog is currently popped-up (visible)
     */
    g_object_class_install_property (gobject_class, PROP_POPUP_SHOWN,
            g_param_spec_boolean ("popup-shown",
                "IsPopped",
                "If the color selection dialog is popped up",
                FALSE,
                G_PARAM_READABLE));

    g_type_class_add_private (gobject_class, sizeof (HildonColorButtonPrivate));
}

/* FIXME Draw a dotted grid over the specified area to make it look 
 * insensitive. Actually, we should generate that pixbuf once and 
 * just render it over later... */
static void
draw_grid                                       (GdkDrawable *drawable, 
                                                 GdkGC *gc, 
                                                 int x, 
                                                 int y,
                                                 gint w, 
                                                 gint h)
{
    int currentx;
    int currenty;
    for (currenty = y; currenty <= h; currenty++)
        for (currentx = ((currenty % 2 == 0) ? x : x + 1); currentx <= w; currentx += 2)
            gdk_draw_point (drawable, gc, currentx, currenty);
}

/* Handle exposure events for the color picker's drawing area */
static gint
hildon_color_field_expose_event                 (GtkWidget *widget, 
                                                 GdkEventExpose *event,
                                                 HildonColorButton *cb)
{
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (cb);
    GdkColor outer_border, inner_border;

    g_assert (priv);

    /* Create the outer border color */
    outer_border.pixel = 0;
    outer_border.red   = OUTER_BORDER_RED;
    outer_border.blue  = OUTER_BORDER_BLUE;
    outer_border.green = OUTER_BORDER_GREEN;

    /* Create the inner border color */
    inner_border.pixel = 0;
    inner_border.red   = INNER_BORDER_RED;
    inner_border.blue  = INNER_BORDER_BLUE;
    inner_border.green = INNER_BORDER_GREEN;

    /* serve the outer border color to the Graphic Context */
    gdk_gc_set_rgb_fg_color (priv->gc, &outer_border);
    /* draw the outer border as a filled rectangle */
    gdk_draw_rectangle (widget->window,
            (GTK_WIDGET_IS_SENSITIVE (widget)) ?  priv->gc : widget->style->bg_gc [GTK_STATE_INSENSITIVE],
            TRUE,
            0, 
            0,
            widget->allocation.width,
            widget->allocation.height);

    /* serve the inner border color to the Graphic Context */
    gdk_gc_set_rgb_fg_color (priv->gc, &inner_border);

    /* draw the inner border as a filled rectangle */
    gdk_draw_rectangle (widget->window,
            priv->gc,
            TRUE,
            OUTER_BORDER_THICKNESS, 
            OUTER_BORDER_THICKNESS,
            widget->allocation.width - (OUTER_BORDER_THICKNESS * 2),
            widget->allocation.height - (OUTER_BORDER_THICKNESS * 2));

    /* serve the actual color to the Graphic Context */
    gdk_gc_set_rgb_fg_color(priv->gc, &priv->color);

    /* draw the actual rectangle */
    gdk_draw_rectangle(widget->window,
            priv->gc,
            TRUE,
            INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS,
            INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS,
            widget->allocation.width - ((INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS)*2),
            widget->allocation.height - ((INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS)*2));

    if (! GTK_WIDGET_IS_SENSITIVE (widget)) {
        draw_grid (GDK_DRAWABLE (widget->window), widget->style->bg_gc [GTK_STATE_INSENSITIVE], 
                INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS,
                INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS,
                widget->allocation.width  - ((INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS)*2) + 2,
                widget->allocation.height - ((INNER_BORDER_THICKNESS + OUTER_BORDER_THICKNESS)*2) + 2);
    }

    return FALSE;
}

static void
hildon_color_button_init                        (HildonColorButton *cb)
{
    GtkWidget *align;
    GtkWidget *drawing_area;
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (cb);

    priv->dialog = NULL;
    priv->gc = NULL;
    priv->popped = FALSE;

    gtk_widget_push_composite_child ();

    /* create widgets and pixbuf */
    align = gtk_alignment_new (0.5, 0.5, 0, 0); /* composite widget */

    drawing_area = gtk_drawing_area_new (); /* composite widget */

    /* setting minimum sizes */
    gtk_widget_set_size_request (GTK_WIDGET (cb), COLOR_BUTTON_WIDTH,
            COLOR_BUTTON_HEIGHT);

    gtk_widget_set_size_request (GTK_WIDGET(drawing_area),
            COLOR_FILLED_WIDTH, COLOR_FILLED_HEIGHT);

    /* Connect the callback function for exposure event */
    g_signal_connect (drawing_area, "expose-event",
            G_CALLBACK (hildon_color_field_expose_event), cb);

    /* Connect to callback function for key press event */
    g_signal_connect (G_OBJECT(cb), "key-press-event",
            G_CALLBACK(hildon_color_button_key_pressed), cb);

    /* packing */
    gtk_container_add (GTK_CONTAINER (align), drawing_area);
    gtk_container_add (GTK_CONTAINER (cb), align);

    gtk_widget_show_all (align);

    gtk_widget_pop_composite_child ();
}

/* Free memory used by HildonColorButton */
static void
hildon_color_button_finalize                    (GObject *object)
{
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (object);
    g_assert (priv);

    if (priv->dialog)
    {
        gtk_widget_destroy (priv->dialog);
        priv->dialog = NULL;
    }

    if (G_OBJECT_CLASS (parent_class)->finalize)
        G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
hildon_color_button_realize                     (GtkWidget *widget)
{
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (widget);
    g_assert (priv);

    GTK_WIDGET_CLASS (parent_class)->realize (widget);

    priv->gc = gdk_gc_new (widget->window);
}

static void
hildon_color_button_unrealize                   (GtkWidget *widget)
{
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (widget);
    g_assert (priv);

    if (priv->gc != NULL) { 
        g_object_unref (priv->gc);
        priv->gc = NULL;
    }

    GTK_WIDGET_CLASS (parent_class)->unrealize (widget);
}

/* Make the widget sensitive with the keyboard event */
static gboolean
hildon_color_button_mnemonic_activate           (GtkWidget *widget,
                                                 gboolean group_cycling)
{
    gtk_widget_grab_focus (widget);
    return TRUE;
}

/* Popup a color selector dialog on button click */
static void
hildon_color_button_clicked                     (GtkButton *button)
{
    HildonColorButton *cb = HILDON_COLOR_BUTTON (button);
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (button);
    HildonColorChooserDialog *cs_dialog;
    
    g_assert (priv);
    
    cs_dialog = (HildonColorChooserDialog *) priv->dialog;

    /* Popup the color selector dialog */
    if (! cs_dialog)
    {
        /* The dialog hasn't been created yet, do it */
        GtkWidget *parent = gtk_widget_get_toplevel (GTK_WIDGET(cb));
        priv->dialog = hildon_color_chooser_dialog_new ();
        cs_dialog = HILDON_COLOR_CHOOSER_DIALOG (priv->dialog);
        if (parent)
            gtk_window_set_transient_for (GTK_WINDOW (cs_dialog), GTK_WINDOW (parent));

        g_signal_emit (button, signals[SETUP_DIALOG], 0, priv->dialog);
    }

    /* Set the initial color for the color selector dialog */
    hildon_color_chooser_dialog_set_color (cs_dialog, &priv->color);

    /* Update the color for color button if selection was made */
    priv->popped = TRUE;
    if (gtk_dialog_run (GTK_DIALOG (cs_dialog)) == GTK_RESPONSE_OK)
    {
        hildon_color_chooser_dialog_get_color (cs_dialog, &priv->color);
        hildon_color_button_set_color (HILDON_COLOR_BUTTON (button), &priv->color);
        // FIXME A queue-draw should be enough here (not set needed)
    } 

    gtk_widget_hide (GTK_WIDGET(cs_dialog));
    priv->popped = FALSE;
}

/* Popup a color selector dialog on hardkey Select press.
 * FIXME This is a bit hacky, should work without thi. Check. */
static gboolean
hildon_color_button_key_pressed                 (GtkWidget *button, 
                                                 GdkEventKey *event,
                                                 gpointer data)
{
    g_return_val_if_fail (HILDON_IS_COLOR_BUTTON (button), FALSE);

    if (event->keyval == HILDON_HARDKEY_SELECT)
    {
        hildon_color_button_clicked (GTK_BUTTON (button));
        return TRUE;
    }

    return FALSE;
}

static void
hildon_color_button_set_property                (GObject *object, 
                                                 guint param_id,
                                                 const GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonColorButton *cb = HILDON_COLOR_BUTTON (object);
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (cb);
    g_assert (priv);

    switch (param_id) 
    {

        case PROP_COLOR:
            priv->color = *(GdkColor *) g_value_get_boxed (value); 
            gtk_widget_queue_draw (GTK_WIDGET (cb));
            break;

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

static void
hildon_color_button_get_property                (GObject *object, 
                                                 guint param_id,
                                                 GValue *value, 
                                                 GParamSpec *pspec)
{
    HildonColorButton *cb = HILDON_COLOR_BUTTON (object);
    HildonColorButtonPrivate *priv = HILDON_COLOR_BUTTON_GET_PRIVATE (cb);
    g_assert (priv);

    switch (param_id) 
    {

        case PROP_COLOR:
            g_value_set_boxed (value, &priv->color);
            break;

        case PROP_POPUP_SHOWN:
            g_value_set_boolean (value, priv->popped);

        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
            break;
    }
}

/**
 * hildon_color_button_new:
 *
 * Creates a new color button. This returns a widget in the form of a
 * small button containing a swatch representing the selected color.
 * When the button is clicked, a color-selection dialog will open,
 * allowing the user to select a color. The swatch will be updated to
 * reflect the new color when the user finishes.
 *
 * Returns: a new color button
 */
GtkWidget*
hildon_color_button_new                         (void)
{
    return g_object_new (HILDON_TYPE_COLOR_BUTTON, NULL);
}

/**
 * hildon_color_button_new_with_color:
 * @color: a #GdkColor for the initial color
 *
 * Creates a new color button with @color as the initial color. 
 *
 * Returns: a new color button
 */
GtkWidget*
hildon_color_button_new_with_color              (const GdkColor *color)
{
    return g_object_new (HILDON_TYPE_COLOR_BUTTON, "color", color, NULL);
}

/**
 * hildon_color_button_set_color:
 * @button: a #HildonColorButton
 * @color: a color to be set
 *
 * Sets the color selected by the button.
 */
void
hildon_color_button_set_color                   (HildonColorButton *button, 
                                                 GdkColor *color)
{
    g_return_if_fail (HILDON_IS_COLOR_BUTTON (button));

    g_object_set (G_OBJECT (button), "color", color, NULL);
}

/**
 * hildon_color_button_get_popup_shown
 * @button: a #HildonColorButton
 *
 * This function checks if the color button has the color 
 * selection dialog currently popped-up. 
 * 
 * Returns: TRUE if the dialog is popped-up (visible to user).
 *
 */
gboolean
hildon_color_button_get_popup_shown             (HildonColorButton *button)
{
    HildonColorButtonPrivate *priv = NULL; 
    g_return_val_if_fail (HILDON_IS_COLOR_BUTTON (button), FALSE);

    priv = HILDON_COLOR_BUTTON_GET_PRIVATE (button);
    g_assert (priv);

    return priv->popped;
}

/**
 * hildon_color_button_popdown
 * @button: a #HildonColorButton
 *
 * If the color selection dialog is currently popped-up (visible)
 * it will be popped-down (hidden).
 *
 */
void
hildon_color_button_popdown                     (HildonColorButton *button)
{
    HildonColorButtonPrivate *priv = NULL; 
    g_return_if_fail (HILDON_IS_COLOR_BUTTON (button));

    priv = HILDON_COLOR_BUTTON_GET_PRIVATE (button);
    g_assert (priv);

    if (priv->popped && priv->dialog) {
        gtk_dialog_response (GTK_DIALOG (priv->dialog), GTK_RESPONSE_CANCEL);
    }
}

/**
 * hildon_color_button_get_color:
 * @button: a #HildonColorButton
 * @color: a color #GdkColor to be fillled with the current color
 *
 */
void
hildon_color_button_get_color                   (HildonColorButton *button, 
                                                 GdkColor *color)
{
    HildonColorButtonPrivate *priv = NULL; 
    g_return_if_fail (HILDON_IS_COLOR_BUTTON (button));
    g_return_if_fail (color != NULL);
   
    priv = HILDON_COLOR_BUTTON_GET_PRIVATE (button);
    g_assert (priv);

    color->red = priv->color.red;
    color->green = priv->color.green;
    color->blue = priv->color.blue;
    color->pixel = priv->color.pixel;
}

