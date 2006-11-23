#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon-widgets/hildon-window.h"
#include "hildon-widgets/hildon-weekday-picker.h"
#include "hildon-widgets/hildon-program.h"

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    GtkDialog *dialog = GTK_DIALOG (gtk_dialog_new ());

    GtkWidget *picker = hildon_weekday_picker_new ();

    gtk_box_pack_start (GTK_BOX (dialog->vbox), picker, TRUE, TRUE, 0);

    gtk_dialog_add_button (dialog, "Close", GTK_RESPONSE_CLOSE);

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);
    
    return 0;
}


