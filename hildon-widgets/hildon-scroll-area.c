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

/* hildon-scroll-area.c
 *
 * The reason why this is not made as a widget:
 *  We can not create a widget which could return the correct child.
 *  (ie. by gtk_bin_get_child)
 *
 * There is a problem with signals. Signals are emited pretty requently
 * and so these handlers are runned pretty often. That has made some problems.
 * There is tons of different situations where the signals are emited
 * and to make a code which works in every situation and does not break
 * any other situation -> pretty crocky.
 *
*/


#include "hildon-scroll-area.h"
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkfixed.h>
#include <gtk/gtkadjustment.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkeventbox.h>
#include <string.h>

#define DEBUG_TEXT 0

typedef struct {
    GtkWidget *swouter;
    GtkWidget *fixed;
    GtkWidget *swinner;
    GtkWidget *child;
    GtkWidget *ebox;

    GtkAdjustment *outadj;
    GtkAdjustment *inadj;

    gdouble last_value_diff;
    gdouble last_page_size;
    gdouble last_upper;
    gdouble last_value;
    gdouble out_last_value;

    gint inner_changed:1;
    gint inner_value_changed:1;

} HildonScrollArea;


static void hildon_scroll_area_outer_value_changed(GtkAdjustment *adjustment,
                                                   HildonScrollArea *sc);
static void hildon_scroll_area_inner_changed(GtkAdjustment *adjustment,
                                             HildonScrollArea *sc);
static void hildon_scroll_area_inner_value_changed(GtkAdjustment *adjustment,
                                                   HildonScrollArea *sc);
static gboolean hildon_scroll_area_event_box_press(GtkWidget *ebox,
                                                   GdkEvent *event,
                                                   HildonScrollArea *sc);
static void hildon_scroll_area_size_allocate(GtkWidget *widget,
                                            GtkAllocation *allocation,
                                            HildonScrollArea *sc);

/**
 * hildon_scroll_area_new:
 * @sw: #GtkWidget - #GtkScrolledWindow
 * @child: #GtkWidget - Child to be place inside the sw
 *
 * This is not a widget. It's a helper function to create
 * hildon specified scrolling method for Device.
 * Puts and connects the @child to the @sw.
 *
 * Returns: a @GtkFixed
 */
GtkWidget *hildon_scroll_area_new(GtkWidget *sw, GtkWidget *child)
{
    GtkWidget *swi;
    GtkWidget *fixed;
    HildonScrollArea *sc;

    g_return_val_if_fail(GTK_IS_SCROLLED_WINDOW(sw)
                         && GTK_IS_WIDGET(child), NULL);

    swi = gtk_scrolled_window_new(NULL, NULL);
    fixed = gtk_fixed_new();
    sc = g_malloc(sizeof(HildonScrollArea));
    memset(sc, 0, sizeof(HildonScrollArea));

    sc->ebox = gtk_event_box_new();
    gtk_widget_set_events(sc->ebox, GDK_BUTTON_PRESS_MASK);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swi),
                                   GTK_POLICY_NEVER, GTK_POLICY_NEVER);

    gtk_container_add(GTK_CONTAINER(swi), child);
    gtk_fixed_put(GTK_FIXED(fixed), sc->ebox, 0, 0);
    gtk_fixed_put(GTK_FIXED(fixed), swi, 0, 0);

    sc->swouter = sw;
    sc->fixed = fixed;
    sc->swinner = swi;
    sc->child = child;
    sc->outadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(sw));
    sc->inadj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(swi));

    g_signal_connect(G_OBJECT(sc->outadj), "value_changed",
                     G_CALLBACK(hildon_scroll_area_outer_value_changed), sc);
    g_signal_connect(G_OBJECT(sc->inadj), "changed",
                     G_CALLBACK(hildon_scroll_area_inner_changed), sc);
    g_signal_connect(G_OBJECT(sc->inadj), "value_changed",
                     G_CALLBACK(hildon_scroll_area_inner_value_changed), sc);
    g_signal_connect(G_OBJECT(sc->ebox), "button-press-event",
                     G_CALLBACK(hildon_scroll_area_event_box_press), sc);
    g_signal_connect_after(G_OBJECT(sw), "size-allocate",
                     G_CALLBACK(hildon_scroll_area_size_allocate), sc);

    gtk_widget_show_all(sw);
    
    gtk_widget_queue_resize(sw);
    gtk_widget_queue_draw(sw);

    return fixed;
}


static void hildon_scroll_area_outer_value_changed(GtkAdjustment *adjustment,
                                                   HildonScrollArea *sc)
{
    if ((adjustment->value + adjustment->page_size) >
        sc->fixed->allocation.y
        && (sc->fixed->allocation.y + sc->fixed->allocation.height) >
        adjustment->value) {
        /* If the fixed is on the screen, we have to update it */
        gtk_adjustment_value_changed(sc->inadj);
    }
}


static void hildon_scroll_area_inner_changed(GtkAdjustment *adjustment,
                                             HildonScrollArea *sc)
{
    GtkRequisition req;
    gboolean refress = FALSE;
    gboolean mid = FALSE;

    if (sc->inner_changed)
    {
      #if DEBUG_TEXT
      g_print("A\n");
      #endif
      sc->inner_changed = FALSE;
      return;
    }

    /* If child wants less space than we have allocated */
    gtk_widget_size_request(sc->child, &req);
    if (req.height < sc->swinner->allocation.height)
    {
      #if DEBUG_TEXT
      g_print("B");
      #endif
      adjustment->upper = adjustment->page_size = (gdouble) req.height;
    }


    /* If the amount of content is changed */
    if (sc->last_upper != adjustment->upper)
    {
      #if DEBUG_TEXT
      g_print("C");
      #endif
      if (adjustment->upper < adjustment->page_size)
      {
        #if DEBUG_TEXT
        g_print("D");
        #endif
        /* Content is getting smaller than our page -> Shrinking page */
        adjustment->page_size = adjustment->upper;
      }
      sc->last_upper = adjustment->upper;
      refress = TRUE;
    }

    /* If the page size is changed */
    if (sc->last_page_size != adjustment->page_size)
    {
      #if DEBUG_TEXT
      g_print("E");
      #endif
        if (sc->outadj->value <= sc->fixed->allocation.y)
        {
        #if DEBUG_TEXT
        g_print("F");
        #endif
            /* We are before the start of fixed */
            adjustment->value = 0;
            adjustment->page_size = sc->outadj->page_size -
                (sc->fixed->allocation.y - sc->outadj->value);

            if ((sc->fixed->allocation.y + sc->fixed->allocation.height) <
                (sc->outadj->value + sc->outadj->page_size))
            {
              #if DEBUG_TEXT
              g_print("G");
              #endif
                /* The fixed is at the center of screen */
                mid = TRUE;
            }
        }
        else if (sc->outadj->value >= sc->fixed->allocation.y)
        {
          #if DEBUG_TEXT
          g_print("H");
          #endif
            if ((sc->fixed->allocation.y + sc->fixed->allocation.height) <
                (sc->outadj->value + sc->outadj->page_size) &&
                (adjustment->page_size - sc->last_page_size) <
                MAX(sc->last_value_diff, 0))
            {
              #if DEBUG_TEXT
              g_print("I");
              #endif
                /* We are over the end of fixed */
                adjustment->value =
                    sc->outadj->value - sc->fixed->allocation.y;
                adjustment->page_size =
                    (sc->fixed->allocation.y +
                     sc->fixed->allocation.height) - sc->outadj->value;
            }
            else
            {
              #if DEBUG_TEXT
              g_print("J");
              #endif
                /* We are wondering to the middle (This should happen only 
                   when "getting IN" to the fixed) */
                adjustment->value =
                    sc->outadj->value - sc->fixed->allocation.y;
                adjustment->page_size = sc->outadj->page_size;
            }
        }
        refress = TRUE;
        sc->last_page_size = adjustment->page_size;
    }


    /* Last time checks ->
       Yes, system is working so randomly that this is needed
       This is a hack in a way.. But so it shall be. */
    if (adjustment->page_size > adjustment->upper)
    {
      #if DEBUG_TEXT
      g_print("K");
      #endif
      adjustment->page_size = adjustment->upper;
    }

    /* If we made changes -> Refress */
    if (refress)
    {
        gint new_width;
        #if DEBUG_TEXT
        g_print("L");
        #endif
        sc->inner_changed = TRUE;

        if (sc->swinner->allocation.width <= sc->fixed->allocation.width &&
            sc->fixed->allocation.width != 1)
        {
          #if DEBUG_TEXT
          g_print("M");
          #endif
            new_width = sc->fixed->allocation.width;
            if( !GTK_WIDGET_REALIZED(sc->swinner) )
              gtk_widget_realize(sc->swinner);
        }
        else
            new_width = -1;

        if (adjustment->page_size <= 0)
        {
          #if DEBUG_TEXT
          g_print("N");
          #endif
            gtk_widget_set_size_request(sc->swinner, new_width, -1);
        }
        else if( mid )
        {
            gtk_widget_set_size_request( sc->swinner, new_width,
                                         sc->fixed->allocation.height );
        }
        else
        {
          #if DEBUG_TEXT
          g_print("O");
          #endif
            gtk_widget_set_size_request(sc->swinner, new_width,
                                        (gint) adjustment->page_size);
        }

        if (adjustment->upper <= 0)
        {
          #if DEBUG_TEXT
          g_print("P");
          #endif
            gtk_widget_set_size_request(sc->fixed, -1, -1);
        }
        else
            gtk_widget_set_size_request(sc->fixed, -1,
                                        (gint) adjustment->upper);

        if (mid)
        {
          #if DEBUG_TEXT
          g_print("Q");
          #endif
            gtk_widget_set_size_request(sc->ebox, new_width,
                                        sc->fixed->allocation.height);
        }

        gtk_adjustment_changed(adjustment);
        gtk_adjustment_value_changed(adjustment);
    }
  #if DEBUG_TEXT
  g_print("\n");
  #endif
}


static void hildon_scroll_area_inner_value_changed(GtkAdjustment *adjustment,
                                                   HildonScrollArea *sc)
{
    gboolean refress = FALSE;

    if (sc->inner_value_changed)
    {
        #if DEBUG_TEXT
        g_print("1\n");
        #endif
        sc->inner_value_changed = FALSE;
        return;
    }

    if (sc->out_last_value != sc->outadj->value)
    {
        #if DEBUG_TEXT
        g_print("2");
        #endif
        /* Mouse movement comes here */
        if (sc->outadj->value < sc->fixed->allocation.y)
        {
          #if DEBUG_TEXT
          g_print("3");
          #endif
            /* We are before the top of fixed-> The value is 0 every time
               But we may have to update the page size */
            adjustment->value = 0;
            if ((sc->outadj->value + sc->outadj->page_size) <
                (sc->fixed->allocation.y + sc->fixed->allocation.height))
            {
              #if DEBUG_TEXT
              g_print("4");
              #endif

                if ((sc->outadj->value + sc->outadj->page_size) <
                    (sc->fixed->allocation.y))
                {
                  #if DEBUG_TEXT
                  g_print("5");
                  #endif
                    /* The fixed is not on the screen -> go get it boy! */
                    sc->outadj->value = sc->fixed->allocation.y +
                        adjustment->value;
                }


                /* The top of fixed is positioned on the screen We have to 
                   change the page size */
                adjustment->page_size = sc->outadj->page_size -
                    (sc->fixed->allocation.y - sc->outadj->value);
            }
            else
            {
              #if DEBUG_TEXT
              g_print("6");
              #endif
                /* The fixed is small and it fits
                   complitely on the screen */
                adjustment->page_size = sc->outadj->page_size -
                    ((sc->fixed->allocation.y - sc->outadj->value) +
                     ((sc->outadj->value + sc->outadj->page_size) -
                      (sc->fixed->allocation.y +
                       sc->fixed->allocation.height)));
            }
        }
        else if ((sc->outadj->value + sc->outadj->page_size) >
                   (sc->fixed->allocation.y +
                    sc->fixed->allocation.height))
        {
          #if DEBUG_TEXT
          g_print("7");
          #endif
            if ((sc->outadj->value) >
                (sc->fixed->allocation.y + sc->fixed->allocation.height))
            {
              #if DEBUG_TEXT
              g_print("8");
              #endif
                /* The fixed is not on the screen */
                sc->outadj->value = sc->fixed->allocation.y +
                    adjustment->value;
            }


            /* We are after the bottom of fixed-> We have to change the
               values */
            adjustment->page_size =
                (sc->fixed->allocation.y + sc->fixed->allocation.height) -
                sc->outadj->value;
            adjustment->value =
                sc->fixed->allocation.height - adjustment->page_size;
        }
        else
        {
          #if DEBUG_TEXT
          g_print("9");
          #endif
            /* We are at the center of fixed -> The page_size is always
               same as the outers one */
            adjustment->page_size = sc->outadj->page_size;
            adjustment->value =
                sc->outadj->value - sc->fixed->allocation.y;
        }
        sc->last_value = adjustment->value;
        refress = TRUE;
    }

    if (sc->last_value != adjustment->value)
    {
        #if DEBUG_TEXT
        g_print("-10-");
        #endif
        /* We have changed from the last time */
        if (adjustment->upper > sc->outadj->page_size)
        {
          #if DEBUG_TEXT
          g_print("-11-");
          #endif
            /* If amount of content is greater than the page size, it will
               be informed here. */
            if (adjustment->page_size != sc->outadj->page_size)
            {
              #if DEBUG_TEXT
              g_print("-12-");
              #endif
                if (sc->outadj->value < sc->fixed->allocation.y)
                {
                  #if DEBUG_TEXT
                  g_print("-13-");
                  #endif
                    /* We are over the top of fixed */
                    sc->outadj->value +=
                        adjustment->value - sc->last_value;
                    adjustment->page_size +=
                        adjustment->value - sc->last_value;

                    if (adjustment->page_size > sc->outadj->page_size)
                    {
                      #if DEBUG_TEXT
                      g_print("-14-");
                      #endif
                      /* We see top of the widget, and keypress is asking
                         to move out from the screen -> changing values. */
                        adjustment->value = adjustment->page_size -
                            sc->outadj->page_size;
                        sc->outadj->value = sc->fixed->allocation.y +
                            adjustment->value;
                        adjustment->page_size = sc->outadj->page_size;
                    }
                    else
                        adjustment->value = 0;

                }
                else if ((sc->outadj->value + sc->outadj->page_size) >
                           (sc->fixed->allocation.y +
                            sc->fixed->allocation.height))
                {
                  #if DEBUG_TEXT
                  g_print("-15-");
                  #endif
                    /* The bottom of fixed is on the screen */
                    if (adjustment->value < sc->last_value)
                    {
                      #if DEBUG_TEXT
                      g_print("-16-");
                      #endif
                        adjustment->page_size +=
                            sc->last_value - adjustment->value;
                        sc->outadj->value +=
                            adjustment->value - sc->last_value;
                    }
                    else
                    {
                        adjustment->page_size = sc->outadj->page_size -
                            ((sc->outadj->value + sc->outadj->page_size) -
                             (sc->fixed->allocation.y +
                              adjustment->upper));


                        if (adjustment->page_size >
                            sc->outadj->page_size)
                        {
                          #if DEBUG_TEXT
                          g_print("-17-");
                          #endif
                            sc->outadj->value +=
                                adjustment->page_size -
                                sc->outadj->page_size;
                            adjustment->page_size = sc->outadj->page_size;
                        }
                        sc->last_page_size = adjustment->page_size;
                        adjustment->value = sc->outadj->value -
                            sc->fixed->allocation.y;
                    }

                    if (adjustment->page_size > sc->outadj->page_size)
                    {
                      #if DEBUG_TEXT
                      g_print("-18-");
                      #endif
                        /* We went to the middle of fixed(?) */
                        adjustment->page_size +=
                            adjustment->value - sc->last_value;
                        sc->outadj->value = sc->fixed->allocation.y +
                            adjustment->value;
                        adjustment->value =
                            (sc->outadj->value - sc->fixed->allocation.y);

                        adjustment->page_size = sc->outadj->page_size;
                    }
                }
                else
                {
                  #if DEBUG_TEXT
                  g_print("-19-");
                  #endif
                    /* We are in the center now */
                    sc->outadj->value = sc->fixed->allocation.y +
                        adjustment->value;
                    adjustment->page_size = sc->outadj->page_size;
                }
            }
            else
            {
              #if DEBUG_TEXT
              g_print("-20-");
              #endif
                sc->outadj->value += adjustment->value - sc->last_value;
            }
            refress = TRUE;
        }
        else
        {
          #if DEBUG_TEXT
          g_print("-21-");
          #endif
            if ((sc->outadj->value + sc->outadj->page_size) >
                (sc->fixed->allocation.y + sc->fixed->allocation.height))
            {
              #if DEBUG_TEXT
              g_print("-22-");
              #endif
                /* We are not going over the screen, from the bottom. */
                if (sc->outadj->value < sc->fixed->allocation.y)
                {
                  #if DEBUG_TEXT
                  g_print("-23-");
                  #endif
                    /* Whole fixed is inside the screen borders. -> We
                       allocate some more space */
                    sc->outadj->value +=
                        adjustment->value - sc->last_value;
                    adjustment->page_size += adjustment->value;
                    adjustment->value = 0;
                }
                else
                {
                  #if DEBUG_TEXT
                  g_print("-24-");
                  #endif
                    /* The fixed is at the top of screen, partly visible,
                       partly out of the screen */
                    sc->outadj->value += adjustment->value - sc->last_value;
                    adjustment->page_size += sc->last_value - adjustment->value;
                }
                refress = TRUE;
            }
            else
            {    /* We are going over the screen from the bottom */
              #if DEBUG_TEXT
              g_print("-25-");
              #endif
                if (sc->outadj->value < sc->fixed->allocation.y)
                {
                  #if DEBUG_TEXT
                  g_print("-26-");
                  #endif
                    /* Top is not going over */
                    sc->outadj->value += adjustment->value - sc->last_value;
                    adjustment->page_size += adjustment->value;
                    adjustment->value = 0;
                    refress = TRUE;
                }
            }
        }
        sc->last_value_diff = adjustment->value - sc->last_value;
        sc->last_value = adjustment->value;
    }

    if (refress)
    {
      #if DEBUG_TEXT
      g_print("-27-");
      #endif
        sc->out_last_value = sc->outadj->value;
        gtk_fixed_move(GTK_FIXED(sc->fixed), sc->swinner, 0,
                       adjustment->value);

        gtk_adjustment_changed(adjustment);
        sc->inner_value_changed = TRUE;
        gtk_adjustment_value_changed(adjustment);
    }
#if DEBUG_TEXT
g_print("-27-");
#endif
}

static gboolean hildon_scroll_area_event_box_press(GtkWidget *ebox,
                                                   GdkEvent *event,
                                                   HildonScrollArea *sc)
{
    gtk_widget_grab_focus(sc->child);
    return FALSE;
}

static void hildon_scroll_area_size_allocate(GtkWidget *widget,
                                            GtkAllocation *allocation,
                                            HildonScrollArea *sc)
{
  g_signal_handlers_disconnect_by_func(widget, 
                                       (void*)hildon_scroll_area_size_allocate,
                                       sc );
  gtk_widget_set_size_request(sc->child, sc->fixed->allocation.width, -1);
}

