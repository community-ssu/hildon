#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <hildon-sort-dialog.h>

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    GtkDialog *dialog = GTK_DIALOG (hildon_sort_dialog_new (NULL));

    hildon_sort_dialog_add_sort_key (HILDON_SORT_DIALOG (dialog), "First key");
    hildon_sort_dialog_add_sort_key_reversed (HILDON_SORT_DIALOG (dialog), "Second, key");

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);
    
    return 0;
}


