#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon-widgets/hildon-window.h"
#include "hildon-widgets/hildon-program.h"
#include "hildon-widgets/hildon-note.h"

static gboolean
on_information_clicked (GtkWidget *widget)
{
    HildonNote* note = HILDON_NOTE (hildon_note_new_information (NULL, 
            "This is a really really really long text that should " 
            "get wrapped but never truncated because truncating stuff "
            "automatically is really really bad! Blah blah blah!"));

    gtk_dialog_run (GTK_DIALOG (note));
    gtk_object_destroy (GTK_OBJECT (note));
    
    return TRUE;
}

static gboolean
on_confirmation_clicked (GtkWidget *widget)
{
    HildonNote* note = HILDON_NOTE (hildon_note_new_confirmation (NULL, 
            "Do you want to confirm?!"));

    gtk_dialog_run (GTK_DIALOG (note));
    gtk_object_destroy (GTK_OBJECT (note));
    
    return TRUE;
}

static gboolean
on_progress_clicked (GtkWidget *widget)
{
    GtkProgressBar *bar = GTK_PROGRESS_BAR (gtk_progress_bar_new ());
    HildonNote *note = HILDON_NOTE (hildon_note_new_cancel_with_progress_bar (NULL, 
                "Do you want to foo bar?", bar));

    gtk_dialog_run (GTK_DIALOG (note));
    gtk_object_destroy (GTK_OBJECT (note));

    return TRUE;
}

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    HildonProgram *program = hildon_program_get_instance ();

    GtkWidget *window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));    

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);

    GtkVBox *vbox = GTK_VBOX (gtk_vbox_new (6, FALSE));
    GtkButton *button1 = GTK_BUTTON (gtk_button_new_with_label ("Information note"));
    g_signal_connect (G_OBJECT (button1), "clicked", G_CALLBACK (on_information_clicked), NULL);

    GtkButton *button2 = GTK_BUTTON (gtk_button_new_with_label ("Confirmation note"));
    g_signal_connect (G_OBJECT (button2), "clicked", G_CALLBACK (on_confirmation_clicked), NULL);

    GtkButton *button3 = GTK_BUTTON (gtk_button_new_with_label ("Progress note"));
    g_signal_connect (G_OBJECT (button3), "clicked", G_CALLBACK (on_progress_clicked), NULL);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);

    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (button1), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (button2), TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (button3), TRUE, TRUE, 0);

    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (vbox));

    gtk_widget_show_all (GTK_WIDGET (window));
    
    gtk_main ();

    return 0;
}


