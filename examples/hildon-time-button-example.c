#include                                        <glib.h>
#include                                        <gtk/gtk.h>
#include                                        "hildon.h"

static void
on_picker_value_changed (HildonPickerButton * button, gpointer data)
{
  g_print ("Newly selected value: %s\n",
           hildon_button_get_value (HILDON_BUTTON (button)));
}

int
main (int argc, char **argv)
{
  HildonProgram *program = NULL;
  GtkWidget *window = NULL;
  GtkWidget *button;

  hildon_gtk_init (&argc, &argv);

  program = hildon_program_get_instance ();
  g_set_application_name ("hildon-time-button-example");

  window = hildon_stackable_window_new ();
  gtk_window_set_default_size (GTK_WINDOW (window), 300, 200);
  hildon_program_add_window (program, HILDON_WINDOW (window));

  button = hildon_time_button_new_step (HILDON_SIZE_AUTO, HILDON_BUTTON_ARRANGEMENT_VERTICAL, 5);
  g_signal_connect (G_OBJECT (button), "value-changed",
                    G_CALLBACK (on_picker_value_changed), NULL);

  gtk_container_add (GTK_CONTAINER (window), button);

  g_signal_connect (G_OBJECT (window), "delete-event", 
                    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
