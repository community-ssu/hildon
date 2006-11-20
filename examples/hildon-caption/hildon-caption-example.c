#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "hildon-widgets/hildon-window.h"
#include "hildon-widgets/hildon-caption.h"
#include "hildon-widgets/hildon-program.h"

int
main (int argc, char **args)
{
    gtk_init (&argc, &args);
    
    GtkDialog *dialog = GTK_DIALOG (gtk_dialog_new ());

    GtkSizeGroup *size_group = gtk_size_group_new (GTK_SIZE_GROUP_HORIZONTAL);

    GtkWidget *caption1 = hildon_caption_new (size_group, "Caption std", gtk_entry_new (), 
                                              NULL, 0);
    GtkWidget *caption2 = hildon_caption_new (size_group, "Caption left", gtk_entry_new (), 
                                              gtk_image_new_from_stock (GTK_STOCK_OK, GTK_ICON_SIZE_BUTTON), 0);
    GtkWidget *caption3 = hildon_caption_new (size_group, "Caption right", gtk_entry_new (), 
                                              gtk_image_new_from_stock (GTK_STOCK_OK, GTK_ICON_SIZE_BUTTON), 0);

    hildon_caption_set_icon_position (HILDON_CAPTION (caption2), HILDON_CAPTION_POSITION_LEFT);
    hildon_caption_set_icon_position (HILDON_CAPTION (caption3), HILDON_CAPTION_POSITION_RIGHT);
    
    gtk_box_pack_start (GTK_BOX (dialog->vbox), caption1, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (dialog->vbox), caption2, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (dialog->vbox), caption3, FALSE, FALSE, 0);

    gtk_dialog_add_button (dialog, "Close", GTK_RESPONSE_CLOSE);

    gtk_widget_show_all (GTK_WIDGET (dialog));
    gtk_dialog_run (dialog);
    
    return 0;
}


