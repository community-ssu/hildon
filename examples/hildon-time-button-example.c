#include                                        <glib.h>
#include                                        <gtk/gtk.h>
#include                                        "hildon.h"

static void
on_picker_value_changed (HildonPickerButton * button, gpointer data)
{
  g_print ("Newly selected value: %s\n",
           hildon_button_get_value (HILDON_BUTTON (button)));
}

static void
on_change_policy_clicked (GtkButton *button,
                          gpointer data)
{
  static HildonTimeSelectorFormatPolicy policy = HILDON_TIME_SELECTOR_FORMAT_POLICY_AUTOMATIC;
  HildonPickerButton *time_button = NULL;
  HildonTouchSelector *selector = NULL;

  policy = (policy + 1)%3;

  switch (policy)
    {
    case HILDON_TIME_SELECTOR_FORMAT_POLICY_AMPM:
      hildon_button_set_value (HILDON_BUTTON (button),
                               "AMPM");
      break;
    case HILDON_TIME_SELECTOR_FORMAT_POLICY_24H:
      hildon_button_set_value (HILDON_BUTTON (button),
                               "24H");
      break;
    case HILDON_TIME_SELECTOR_FORMAT_POLICY_AUTOMATIC:
      hildon_button_set_value (HILDON_BUTTON (button),
                               "Automatic");
      break;
    }

  time_button = HILDON_PICKER_BUTTON (data);
  selector = hildon_picker_button_get_selector (time_button);

  g_object_set (G_OBJECT (selector),
                "time-format-policy", policy, NULL);
}

int
main (int argc, char **argv)
{
  HildonProgram *program = NULL;
  GtkWidget *window = NULL;
  GtkWidget *button;
  GtkWidget *hbox;
  GtkWidget *button2;

  hildon_gtk_init (&argc, &argv);

  program = hildon_program_get_instance ();
  g_set_application_name ("hildon-time-button-example");

  window = hildon_stackable_window_new ();
  gtk_window_set_default_size (GTK_WINDOW (window), 300, 200);
  hildon_program_add_window (program, HILDON_WINDOW (window));

  button = hildon_time_button_new_step (HILDON_SIZE_AUTO, HILDON_BUTTON_ARRANGEMENT_VERTICAL, 5);
  g_signal_connect (G_OBJECT (button), "value-changed",
                    G_CALLBACK (on_picker_value_changed), NULL);

  button2 = hildon_button_new_with_text (HILDON_SIZE_AUTO_WIDTH |
                                         HILDON_SIZE_AUTO_HEIGHT,
                                         HILDON_BUTTON_ARRANGEMENT_HORIZONTAL,
                                         "Change Policy", "Automatic");

  g_signal_connect (G_OBJECT (button2), "clicked",
                    G_CALLBACK (on_change_policy_clicked), button);

  hbox = gtk_hbox_new (FALSE, 10);

  gtk_container_add (GTK_CONTAINER (hbox), button);
  gtk_container_add (GTK_CONTAINER (hbox), button2);
  gtk_container_add (GTK_CONTAINER (window), hbox);

  g_signal_connect (G_OBJECT (window), "delete-event",
                    G_CALLBACK (gtk_main_quit), NULL);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
