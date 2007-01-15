#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon-window.h"
#include "hildon-time-picker.h"
#include "hildon-program.h"

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    GtkDialog *dialog = GTK_DIALOG (hildon_time_picker_new (NULL));

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);
    
    return 0;
}

