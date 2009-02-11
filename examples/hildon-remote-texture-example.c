#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>
#include "hildon.h"

#define W 800
#define H 480

/* Press and drag to pan around, or tap to move between zoom levels.
 * A file must be specified on the command-line, and it must be > 640x480.
 *
 * This file will be loaded and placed into a shared memory area, which will
 * be read by hildon-desktop.
 * An area 640x480 in the top-left corner will be updated with an animation
 * to show how to modify the area. */

#define ANIM_X 0
#define ANIM_Y 0
#define ANIM_WIDTH 640
#define ANIM_HEIGHT 480
#define ANIM_FRAMES 64

static key_t shm_key = 0xCAFEBEEF;
static gint  bpp, width, height;
static guchar *shm = 0; /* shared mem */
static int anim_frame = 0;

static double scale = 1;
static double scale_smooth = 1;
static double x = 0;
static double y = 0;
static double x_inc = 0;
static double y_inc = 0;
static double cursor_move = 0;

static int last_x, last_y, pressed = 0;

static void timeout_cb (void *obj)
{
    HildonRemoteTexture *actor = HILDON_REMOTE_TEXTURE (obj);

    if (shm) {
      gint x,y;

      for (y=0;y<ANIM_HEIGHT;y++)
        {
          guchar *anim = &shm[(width*(y+ANIM_Y) + ANIM_X)*bpp];
          for (x=0;x<ANIM_WIDTH;x++)
            {
              *anim = ((x+anim_frame)&(ANIM_FRAMES-1)) * 256 / ANIM_FRAMES;
              anim += bpp;
            }
        }

      hildon_remote_texture_update_area(actor,
          ANIM_X, ANIM_Y, ANIM_WIDTH, ANIM_HEIGHT);
      anim_frame++;
    }

    if (!pressed)
      {
        x += x_inc;
        y += y_inc;
        x_inc = x_inc * 0.8;
        y_inc = y_inc * 0.8;
        hildon_remote_texture_set_position (actor, x, y);

        scale_smooth = scale_smooth*0.9 + scale*0.1;
        hildon_remote_texture_set_scale (actor, scale_smooth, scale_smooth);
      }

}

static void press_cb (GtkWidget *widget,
               GdkEventButton *event, gpointer data)
{
  pressed = TRUE;
  last_x = event->x;
  last_y = event->y;
  cursor_move = 0;
}

static void release_cb (GtkWidget *widget,
               GdkEventButton *event, gpointer obj)
{
  pressed = FALSE;
  last_x = event->x;
  last_y = event->y;

  if (cursor_move < 40)
    {
      scale = scale * 1.5;
      if (scale>8) scale=0.125;
      /* scale smooth will actually do the update */
    }
}

static void motion_cb (GtkWidget *widget,
                GdkEventMotion *event, gpointer data)
{
  HildonRemoteTexture *actor = HILDON_REMOTE_TEXTURE(data);

  if (pressed)
    {
      x_inc = event->x - last_x;
      y_inc = event->y - last_y;
      cursor_move += fabs(x_inc) + fabs(y_inc);

      x_inc /= scale;
      y_inc /= scale;

      x += x_inc;
      y += y_inc;
      hildon_remote_texture_set_position (actor, x, y);
    }

  last_x = event->x;
  last_y = event->y;
}

int
main (int argc, char **argv)
{
    gtk_init (&argc, &argv);

    GdkPixbuf        *pixbuf;
    GError           *error = 0;
    guchar           *gpixels;

    if (argc!=2)
      {
        printf("USAGE: hildon-remote-texture-example image.png\n");
        printf("NB. Image should be 640x480 in size or greater\n");
        return EXIT_FAILURE;
      }

    g_debug("Loading Image %s...", argv[1]);
    pixbuf          = gdk_pixbuf_new_from_file (argv[1], &error);
    width           = gdk_pixbuf_get_width (pixbuf);
    height          = gdk_pixbuf_get_height (pixbuf);
    bpp             = gdk_pixbuf_get_n_channels (pixbuf); /* assume 8 bit */
    gpixels         = gdk_pixbuf_get_pixels (pixbuf);
    g_debug("Creating Shared Memory");


    size_t shm_size = width*height*bpp;
    int shmid;

    /*
     * Create the segment, attach it to our data space, and copy in the
     * texture we loaded
     */
    if ((shmid = shmget(shm_key, shm_size, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (guchar *) -1) {
        perror("shmat");
        exit(1);
    }

    memcpy(shm, gpixels, shm_size);

    g_debug("Done.");



    HildonProgram *program = hildon_program_get_instance ();

    HildonWindow *window = HILDON_WINDOW(
        hildon_window_new());
    hildon_program_add_window (program, window);
    gtk_window_set_decorated (GTK_WINDOW (window), FALSE);
    gtk_widget_show (GTK_WIDGET(window));

    HildonRemoteTexture *actor = HILDON_REMOTE_TEXTURE
	(hildon_remote_texture_new ());
    gtk_window_resize(GTK_WINDOW(actor), W, H);

    g_set_application_name ("Animation");

    g_signal_connect (G_OBJECT (actor),
                      "delete_event",
                      G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (G_OBJECT (actor),
                              "button-press-event",
                              G_CALLBACK (press_cb),
                              actor);
    g_signal_connect (G_OBJECT (actor),
                                  "button-release-event",
                                  G_CALLBACK (release_cb),
                                  actor);
    g_signal_connect (G_OBJECT (actor),
                      "motion-notify-event",
                      G_CALLBACK (motion_cb),
                      actor);
    gtk_widget_add_events (GTK_WIDGET(actor),
                           GDK_BUTTON_PRESS_MASK|
                           GDK_BUTTON_MOTION_MASK|
                           GDK_BUTTON_RELEASE_MASK);


    gtk_widget_show_all (GTK_WIDGET (actor));
    gdk_flush ();

    hildon_remote_texture_set_parent(actor, GTK_WINDOW(window));
    hildon_remote_texture_set_image(actor, shm_key, width, height, bpp);
    hildon_remote_texture_set_position (actor, x, y);
    hildon_remote_texture_set_show (actor, 1);

    g_timeout_add (25, (GSourceFunc)timeout_cb, actor);

    printf ("going to gtk_main ()\n");
    gtk_main ();
    return 0;
}
