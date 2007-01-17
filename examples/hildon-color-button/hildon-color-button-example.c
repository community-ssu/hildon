#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon-color-button.h"
#include "hildon-program.h"
#include "hildon-window.h"

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    GtkDialog *dialog = GTK_DIALOG (gtk_dialog_new ());
    GtkWidget *button = hildon_color_button_new ();
    GtkWidget *label = gtk_label_new ("Pick the color:");
    GtkWidget *hbox = gtk_hbox_new (FALSE, 12);

    gtk_box_pack_start (GTK_BOX (hbox), label, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (dialog->vbox), hbox, TRUE, TRUE, 0);

    gtk_dialog_add_button (dialog, "Close", GTK_RESPONSE_CLOSE);

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);
    
    return 0;
}


