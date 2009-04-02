/*
 * This file is a part of hildon examples
 *
 * Copyright (C) 2008 Nokia Corporation, all rights reserved.
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

#include                                        "hildon.h"

static gint                                     global_stack_count = 1;

static void
add_window                                      (GtkWidget             *button,
                                                 HildonStackableWindow *parent);

static void
push_windows                                    (GtkWidget     *button,
                                                 GtkSpinButton *spin);

static void
pop_windows                                     (GtkWidget     *button,
                                                 GtkSpinButton *spin);

static GtkWidget*
new_window                                      (HildonStackableWindow *parent)
{
    GtkWidget *window, *hbbox, *vbox, *label, *add, *new;
    GtkWidget *spin1hbox, *spin1label1, *spin1, *spin1label2, *pushbtn, *align1;
    GtkWidget *spin2hbox, *spin2label1, *spin2, *spin2label2, *popbtn, *align2;
    gint stack_number, win_number;
    gchar *text;

    window = hildon_stackable_window_new ();

    if (parent) {
        stack_number = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (parent), "stack-number"));
        win_number = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (parent), "win-number")) + 1;
    } else {
        stack_number = global_stack_count++;
        win_number = 1;
    }
    g_object_set_data (G_OBJECT (window), "stack-number", GINT_TO_POINTER (stack_number));
    g_object_set_data (G_OBJECT (window), "win-number", GINT_TO_POINTER (win_number));

    /* Window title */
    text = g_strdup_printf ("Stack number %d - window %d", stack_number, win_number);
    gtk_window_set_title (GTK_WINDOW (window), text);
    g_free (text);

    /* Marked up window title */
    text = g_strdup_printf ("Stack number <i>%d</i> - window <i>%d</i>", stack_number, win_number);
    hildon_window_set_markup (HILDON_WINDOW (window), text);
    g_free (text);

    /* Main label */
    text = g_strdup_printf ("Stack number %d\nWindow number %d", stack_number, win_number);
    label = gtk_label_new (text);
    g_free (text);

    hbbox = gtk_hbutton_box_new ();

    /* Button to push a window to the current stack */
    add = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (add), "Add a window to this stack");
    gtk_box_pack_start (GTK_BOX (hbbox), add, FALSE, FALSE, 0);
    g_signal_connect (G_OBJECT (add), "clicked", G_CALLBACK (add_window), window);

    /* Button to create a new stack */
    new = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (new), "Add a window to a new stack");
    gtk_box_pack_start (GTK_BOX (hbbox), new, FALSE, FALSE, 0);
    g_signal_connect (G_OBJECT (new), "clicked", G_CALLBACK (add_window), NULL);

    /* Spinbox and button to push many windows */
    spin1hbox = gtk_hbox_new (FALSE, 0);
    spin1label1 = gtk_label_new ("Push");
    spin1 = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (2, 2, 5, 1, 1, 1)), 1, 0);
    gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin1), TRUE);
    spin1label2 = gtk_label_new ("windows");
    pushbtn = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (pushbtn), "Push windows");
    gtk_box_pack_start (GTK_BOX (spin1hbox), spin1label1, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (spin1hbox), spin1, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (spin1hbox), spin1label2, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (spin1hbox), pushbtn, FALSE, FALSE, 10);
    align1 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (align1), spin1hbox);
    g_signal_connect (G_OBJECT (pushbtn), "clicked", G_CALLBACK (push_windows), spin1);

    /* Spinbox and button to pop many windows */
    spin2hbox = gtk_hbox_new (FALSE, 0);
    spin2label1 = gtk_label_new ("Pop");
    spin2 = gtk_spin_button_new (GTK_ADJUSTMENT (gtk_adjustment_new (2, 2, 5, 1, 1, 1)), 1, 0);
    gtk_spin_button_set_numeric (GTK_SPIN_BUTTON (spin2), TRUE);
    spin2label2 = gtk_label_new ("windows");
    popbtn = hildon_gtk_button_new (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH);
    gtk_button_set_label (GTK_BUTTON (popbtn), "Pop windows");
    gtk_box_pack_start (GTK_BOX (spin2hbox), spin2label1, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (spin2hbox), spin2, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (spin2hbox), spin2label2, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (spin2hbox), popbtn, FALSE, FALSE, 10);
    align2 = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (align2), spin2hbox);
    g_signal_connect (G_OBJECT (popbtn), "clicked", G_CALLBACK (pop_windows), spin2);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), label, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), hbbox, FALSE, FALSE, 10);
    gtk_box_pack_start (GTK_BOX (vbox), align1, FALSE, FALSE, 10);
    gtk_box_pack_start (GTK_BOX (vbox), align2, FALSE, FALSE, 10);

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_widget_show_all (vbox);

    return window;
}

static void
add_window                                      (GtkWidget             *button,
                                                 HildonStackableWindow *parent)
{
    HildonWindowStack *stack = NULL;
    GtkWidget *window;

    if (parent) {
        stack = hildon_stackable_window_get_stack (parent);
    } else {
        stack = hildon_window_stack_new ();
    }

    window = new_window (parent);

    if (!stack) {
        stack = hildon_window_stack_get_default ();
    }
    hildon_window_stack_push_1 (stack, HILDON_STACKABLE_WINDOW (window));
}

static void
push_windows                                    (GtkWidget     *button,
                                                 GtkSpinButton *spin)
{
    GList *l = NULL;
    HildonWindowStack *stack = NULL;
    HildonStackableWindow *parent;
    gint nwindows = gtk_spin_button_get_value_as_int (spin);

    parent = HILDON_STACKABLE_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (spin)));
    stack = hildon_stackable_window_get_stack (parent);

    while (nwindows > 0) {
        parent = HILDON_STACKABLE_WINDOW (new_window (parent));
        l = g_list_append (l, parent);
        nwindows--;
    }
    hildon_window_stack_push_list (stack, l);
    g_list_free (l);
}

static void
pop_windows                                     (GtkWidget     *button,
                                                 GtkSpinButton *spin)
{
    HildonWindowStack *stack = NULL;
    HildonStackableWindow *win;
    gint nwindows = gtk_spin_button_get_value_as_int (spin);

    win = HILDON_STACKABLE_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (spin)));
    stack = hildon_stackable_window_get_stack (win);

    hildon_window_stack_pop (stack, nwindows, NULL);
}

int
main                                            (int    argc,
                                                 char **argv)
{
    GtkWidget *window;

    hildon_gtk_init (&argc, &argv);

    g_set_application_name ("hildon-stackable-window-example");

    window = new_window (NULL);

    g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (gtk_main_quit), NULL);

    gtk_widget_show (window);

    gtk_main ();

    return 0;
}
