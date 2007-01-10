#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon-widgets/hildon-window.h"
#include "hildon-widgets/hildon-program.h"
#include "hildon-widgets/hildon-controlbar.h"

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    HildonProgram *program = hildon_program_get_instance ();

    GtkWidget *window = hildon_window_new ();
    hildon_program_add_window (program, HILDON_WINDOW (window));    

    gtk_container_set_border_width (GTK_CONTAINER (window), 6);
    
    HildonControlbar *bar = HILDON_CONTROLBAR (hildon_controlbar_new ());
    hildon_controlbar_set_range (bar, 20, 120);
    hildon_controlbar_set_value (bar, 100);

    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (gtk_main_quit), NULL);
    gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (bar));
    gtk_widget_show_all (GTK_WIDGET (window));
    
    gtk_main ();
    return 0;
}


