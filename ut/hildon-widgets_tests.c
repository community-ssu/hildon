/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include <gtk/gtk.h>

#include "hildon-controlbar.h"
#include "hildon-seekbar.h"
#include "hildon-note.h"
#include <hildon-widgets/gtk-infoprint.h>
#include <hildon-widgets/hildon-weekday-picker.h>
#include <hildon-widgets/hildon-telephone-editor.h>
#include <hildon-widgets/hildon-range-editor.h>
#include <hildon-widgets/hildon-sort-dialog.h>
#include <hildon-widgets/hildon-get-password-dialog.h>
#include <hildon-widgets/hildon-set-password-dialog.h>
#include <hildon-widgets/hildon-hvolumebar.h>
#include <hildon-widgets/hildon-vvolumebar.h>
#include <hildon-widgets/hildon-font-selection-dialog.h>
#include <hildon-widgets/hildon-insert-object-dialog.h>
#include <hildon-widgets/hildon-number-editor.h>
#include <hildon-widgets/hildon-color-selector.h>
#include <hildon-widgets/hildon-file-handling-note.h>
#include <hildon-widgets/hildon-add-home-dialog.h>
#include <hildon-widgets/hildon-grid-item.h>
#include <hildon-widgets/hildon-grid.h>
#include <hildon-widgets/hildon-time-editor.h>
#include <hildon-widgets/hildon-name-password-dialog.h>
#include <hildon-widgets/hildon-caption.h>
#include <outo.h>

/* Icon which must exist (HildonGridItem). */
#define VALID_ICON  "Help"

/*prototypes to keep the compiler happy*/
testcase *get_tests(void);
void init_test(void);

     /* the testfuncs */
int test1a(void);
int test1b(void);
int test1c(void);
int test1d(void);
int test1e(void);
int test1f(void);
int test1g(void);
int test1h(void);
int test1i(void);
int test2a(void);
int test2b(void);
int test2c(void);
int test2d(void);
int test2e(void);
int test3a(void);
int test3b(void);
int test3c(void);
int test3d(void);
int test3e(void);
int test3f(void);
int test3g(void);
int test3h(void);
int test3i(void);
int test3j(void);
int test3k(void);
/*int test3l(void);*/
int test3m(void);
int test3n(void);
int test3o(void);
int test3p(void);
int test3q(void);
int test3r(void);
int test3s(void);
int test3t(void);
int test3u(void);
int test3v(void);

/* notes */
int test4a(void);
int test4b(void);
int test4c(void);
int test4d(void);
int test4e(void);
int test4f(void);
int test4g(void);
int test4h(void);
int test4i(void);
int test4j(void);
int test4k(void);
int test4l(void);

/* volumebar */
int test6a(void);
int test6b(void);
int test6c(void);
int test6d(void);
int test6e(void);

int test8a(void);
int test8b(void);
int test8c(void);
int test8d(void);
int test8e(void);
int test8f(void);
int test8g(void);
int test8h(void);
int test8i(void);
int test8j(void);
int test8k(void);

/* weekday picker */
int test10a(void);
int test11a(void);
int test11b(void);
int test11c(void);
int test11d(void);
int test12a(void);
int test12b(void);
int test12c(void);

/* telephone editor */
int test13a(void);
int test13b(void);
int test14a(void);
int test14b(void);
int test15a(void);
int test15b(void);
int test15c(void);
int test15d(void);
int test15e(void);
int test15f(void);
int test16a(void);
int test16b(void);
int test17a(void);
int test17b(void);
int test17c(void);
int test17d(void);
int test18a(void);
int test18b(void);
int test18c(void);
int test18d(void);
int test19a(void);
int test19b(void);
int test19c(void);
int test19d(void);
int test19e(void);

#if 0
/* find object dialog */
int test20a(void);
int test20b(void);
int test20c(void);
int test20d(void);
#endif

/* find object options dialog */
/*int test21a(void);
int test21b(void);
int test21c(void);
int test21d(void);
int test21e(void);
int test21f(void);
int test21g(void);
int test21h(void);
int test21i(void);
int test21j(void);*/

/* range editor */
int test22a(void);
int test22b(void);
int test22c(void);
int test22d(void);
int test22e(void);
int test22f(void);
int test22g(void);
int test22h(void);
int test22i(void);
int test22j(void);
int test22k(void);
int test22l(void);
int test22m(void);

/* sort dialog */
int test23a(void);
int test23b(void);
int test23c(void);
int test23d(void);
int test23e(void);
int test23f(void);
int test23g(void);

/* get / get old password dialog */
int test24a(void);
int test24b(void);
int test24c(void);
int test24d(void);

/* set / change password dialog */
int test25a(void);
int test25b(void);
int test25c(void);
int test25d(void);
int test25e(void);

/*FontSelectionDialog*/
#ifndef HILDON_DISABLE_DEPRECATED
int fsd_set_font(void);
int fsd_get_font(void);
#endif
int fsd_set_pretext(void);
int fsd_get_pretext(void);

int test27a(void);
int test27b(void);
int test27c(void);

/* NumberEditor */
int test28a(void);
int test28b(void);
int test28c(void);
int test28d(void);

/* ColorSelector */
int test29a(void);
int test29b(void);

/* FontHandlingNote */
int test30a(void);
int test30b(void);
int test30c(void);
int test30d(void);

/* Add to Home / Rename dialog */
int test31a(void);
int test31b(void);
int test32a(void);
int test32b(void);

/* HildonGridItem */
int test33a1(void);
int test33a2(void);
int test33b1(void);
int test33b2(void);
int test33b3(void);
int test33b4(void);
int test33c1(void);
int test33c2(void);
int test33c3(void);
int test33c4(void);
int test33d1(void);
int test33d2(void);
int test33d3(void);
int test33e(void);

/* HildonGrid */
int test34a(void);
int test34b1(void);
int test34b2(void);
int test34b3(void);
int test34b4(void);
int test34c1(void);
int test34c2(void);
int test34c3(void);
int test34d1(void);
int test34d2(void);
int test34e1(void);
int test34e2(void);
int test34f1(void);
int test34f2(void);
int test34f3(void);
int test34f4(void);
int test34f5(void);
int test34g1(void);
int test34g2(void);
int test34g3(void);
int test34g4(void);
int test34g5(void);
int test34g6(void);
int test34h1(void);
int test34h2(void);
int test34h3(void);
int test34h4(void);
int test34i1(void);
int test34i2(void);
int test34j1(void);
int test34j2(void);

/* Time/Duration Editor */
int test35a(void);
int test35b(void);
int test35c(void);
int test35d(void);
int test35e(void);
int test35f(void);
int test35g(void);
int test35h(void);

#if 0
int test36a(void);
int test36b(void);
int test36c(void);
int test36d(void);
#endif

/* New infoprint/banner functionalities. */
int test37a(void);
int test37b(void);
int test38a(void);

/* namepassworddialog */
int test39a(void);
int test39b(void);

/* file details dialog */
#ifndef HILDON_DISABLE_DEPRECATED
int test41a(void);
#endif

int test42 (void);

/* this has to be like this (not static). outo
   calls for this! */
testcase *get_tests(void);
void init_test( void )
{
    int plop = 0;
    gtk_init (&plop, NULL);
}

int test1a()
{
    HildonControlbar *controlBar;
    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());

    assert(controlBar);
    return 1;
}

int test1b()
{
    HildonControlbar *controlBar;
    gint max=500;
    
    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    hildon_controlbar_set_max(controlBar, max);
    assert(hildon_controlbar_get_max(controlBar) == max);
    return 1;
}

int test1c()
{
    HildonControlbar *controlBar;
    gint max=500;
    gint value=250;
    
    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    hildon_controlbar_set_max(controlBar, max);
    hildon_controlbar_set_value(controlBar, value);
    assert(hildon_controlbar_get_value(controlBar) == value);
    return 1;
}

int test1d()
{
    HildonControlbar *controlBar;
    
    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    gtk_widget_destroy(GTK_WIDGET(controlBar));
    return 1;
}

int test1e()
{
    gint getmin;
    HildonControlbar *controlBar;

    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    hildon_controlbar_set_min(controlBar, 5);
    getmin = hildon_controlbar_get_min (controlBar);
    assert (getmin == 5);
    return 1;
}

int test1f()
{
   gint min = 10;
   gint max = 50;

   HildonControlbar *controlBar;

   controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
   hildon_controlbar_set_range (controlBar, min, max);
   return 1;
}

int test1g()
{
    gint minval;
    gint maxval;

    HildonControlbar *controlBar;

    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    hildon_controlbar_set_range (controlBar, 57, 219);
    minval = hildon_controlbar_get_min (controlBar);
    maxval = hildon_controlbar_get_max (controlBar);
    assert (minval == 57);
    assert (maxval == 219);
    return 1;
}

int test1h()
{
    gint value;

    HildonControlbar *controlBar;

    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    hildon_controlbar_set_range (controlBar, 250, 550);
    hildon_controlbar_set_value (controlBar, 100);
   
    value = hildon_controlbar_get_value (controlBar);

    assert (value == 250);
    return 1;
}

int test1i()
{
    gint value;

    HildonControlbar *controlBar;

    controlBar = HILDON_CONTROLBAR(hildon_controlbar_new());
    hildon_controlbar_set_range (controlBar, 250, 550);
    hildon_controlbar_set_value (controlBar, 650);
    
    value = hildon_controlbar_get_value (controlBar);

    assert (value == 550);
    return 1;
}

int test2a()
{
    GtkWidget *seekBar;
    
    seekBar = hildon_seekbar_new();
    assert(seekBar);
    return 1;
}

int test2b()
{
    GtkWidget *seekBar;
    gint totalTime=500;
    
    seekBar = hildon_seekbar_new();
    hildon_seekbar_set_total_time(HILDON_SEEKBAR(seekBar), totalTime);
    assert(hildon_seekbar_get_total_time(HILDON_SEEKBAR(seekBar)) == totalTime);
    return 1;
}

int test2c()
{
    GtkWidget *seekBar;
    gint totalTime=500;
    gint position=250;
    
    seekBar = hildon_seekbar_new();
    hildon_seekbar_set_total_time(HILDON_SEEKBAR(seekBar), totalTime);
    hildon_seekbar_set_position(HILDON_SEEKBAR(seekBar), position);
    assert(hildon_seekbar_get_position(HILDON_SEEKBAR(seekBar)) == position);
    return 1;
}

int test2d()
{
    GtkWidget *seekBar;
    
    seekBar = hildon_seekbar_new();
    gtk_widget_destroy(GTK_WIDGET(seekBar));
    return 1;
}

int test2e()
{
    GtkWidget *seekBar;
    gint totalTime=500;
    
    seekBar = hildon_seekbar_new();
    hildon_seekbar_set_total_time(HILDON_SEEKBAR(seekBar), totalTime);
    hildon_seekbar_set_fraction(HILDON_SEEKBAR(seekBar), 200);
    assert(hildon_seekbar_get_fraction(HILDON_SEEKBAR(seekBar)) == 200);
    return 1;
}

int test3a()
{
    
    gtk_infoprint(NULL,"infoprint");
    return 1;
}

int test3b()
{
    
    gtk_infoprint(NULL, NULL);
    return 1;
}

int test3c()
{
    
    gtk_infoprint_with_icon_stock(NULL, "infoprint", GTK_STOCK_DIALOG_QUESTION);
    return 1;
}

int test3d()
{
    
    gtk_infoprint_with_icon_stock(NULL, "infoprint", "");
    return 1;
}

int test3e()
{

    gtk_infoprint_with_icon_stock(NULL, "infoprint", NULL);
    return 1;
}

int test3f()
{
    
    gtk_infoprintf (NULL, "mambo %d", 2+3);
    return 1;
}

int test3g()
{
    
    gtk_infoprintf (NULL, NULL);
    return 1;
}


int test3h()
{
    gtk_confirmation_banner (NULL, "Insert our favourite testing text here.", NULL);
    return 1;
}

int test3i()
{
    gtk_confirmation_banner (NULL, NULL, NULL);
    return 1;
}

int test3j()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_animation (win, "Insert our favourite testing text here.");
    gtk_banner_set_text (win, "Insert our second favourite testing text here.");
    gtk_banner_set_text (win, "");
    gtk_banner_set_text (win, NULL); 
    gtk_banner_close (win);
    gtk_widget_destroy (GTK_WIDGET (win));
    return 1;
}

int test3k()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_animation (win, "Insert our favourite testing text here.");
    gtk_banner_set_fraction (win, 0.5);
    gtk_banner_set_fraction (win, -0.5);
    gtk_banner_set_fraction (win, 1.5);
    gtk_banner_close (win);
    gtk_widget_destroy (GTK_WIDGET (win));
    return 1;
}

/*int test3l()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_animation (win, "Insert our favourite testing text here.");
    gtk_banner_close (win);
    gtk_banner_close (win);
    gtk_widget_destroy (GTK_WIDGET (win));
    return 1;
}*/

int test3m()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_animation (win, "Insert our favourite testing text here.");
    gtk_widget_destroy (GTK_WIDGET (win));
    return 1;
}

int test3n()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_bar (win, "Insert our favourite testing text here.");
    gtk_banner_set_text (win, "Insert our second favourite testing text here.");
    gtk_banner_set_text (win, "");
    gtk_banner_set_text (win, NULL);
    gtk_banner_set_fraction (win, 0.5);
    gtk_banner_close (win);
    return 1;
}

int test3o()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_bar (win, "Insert our favourite testing text here.");
    gtk_banner_set_text (win, "Insert our second favourite testing text here.");
    gtk_banner_set_text (win, "");
    gtk_banner_set_text (win, NULL);
    gtk_banner_set_fraction (win, 0.5);
    gtk_widget_destroy (GTK_WIDGET (win));
    return 1;
}

int test3p()
{
    gtk_banner_show_bar (NULL, "Insert our favourite testing text here.");
    gtk_banner_close (NULL);
    return 1;
}

int test3q()
{
    gtk_banner_set_text (NULL, "Insert our second favourite testing text here.");
    return 1;
}

int test3r()
{
    gtk_banner_set_fraction (NULL, 0.5);
    return 1;
}

int test3s()
{
    gtk_banner_close (NULL);
    return 1;
}

int test3t()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_bar (win, "Insert our favourite testing text here.");
    gtk_banner_set_fraction (win, -0.5);
    gtk_banner_close (win);
    return 1;
}

int test3u()
{
    GtkWindow *win = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
    gtk_banner_show_bar (win, "Insert our favourite testing text here.");
    gtk_banner_set_fraction (win, 1.5);
    gtk_banner_close (win);
    return 1;
}

int test3v()
{
    gtk_banner_show_animation (NULL, "Insert our favourite testing text here.");
    gtk_banner_close (NULL);
    return 1;
}


/* note: confirmation note */
int test4a()
{
    HildonNote *note =  HILDON_NOTE( hildon_note_new_confirmation(  NULL, 
    "Delete the presentation?\nHildon Slides" ));
    assert(note);
    return 1;
}
/* note: confirmation note with icon stock */
int test4b()
{
    HildonNote *note;
    note = HILDON_NOTE( hildon_note_new_confirmation_with_icon_stock(NULL, 
    "Delete the presentation?\nHildon Slides", GTK_STOCK_OPEN));
    assert(note);   
    return 1;
}
/* note: confirmation note with 10 additional buttons */
int test4c()
{
    
    HildonNote *note =  HILDON_NOTE( hildon_note_new_confirmation_add_buttons( NULL,
				     "Delete the presentation?\nHildon Slides", 
				   "add 1", 12345,  "add 2", 13579, 
				   "add 3", 12345,  "add 4", 13579,
                                   "add 5", 12345,  "add 6", 13579, 			
                                   "add 7", 12345,  "add 8", 13579,  		     
				   "add 9", 24680,  "add 9", 24680, NULL) );  
    assert(note);   
    return 1;
}

/* note: confirmation note: Set button text*/
int test4d()
{
    HildonNote *note;
    note = HILDON_NOTE( hildon_note_new_confirmation(NULL, 
    "Delete the presentation?\nHildon Slides"));
    hildon_note_set_button_text( HILDON_NOTE( note ) , "Remove" );
    return 1;
}

/* note: information note */
int test4e()
{
    HildonNote *note;
    note = HILDON_NOTE( hildon_note_new_information(NULL, 
    "Only one button present\nin this information note"));
    return 1;
}

/* note: information note with icon stock */
int test4f()
{
    HildonNote *note;
    note = HILDON_NOTE( hildon_note_new_information_with_icon_stock(NULL, 
    "Only one button present\nin this information note", GTK_STOCK_HARDDISK));
    return 1;
}

/* note: information note: Set button text*/
int test4g()
{
    HildonNote *note;
    note = HILDON_NOTE( hildon_note_new_information(NULL, 
    "Only one button present\nin this information note"));
    hildon_note_set_button_text( HILDON_NOTE( note ) , "Continue" );
    return 1;
}

/* note: new cancel
int test4h()
{
    HildonNote *note;
    
    note = HILDON_NOTE( hildon_note_new_cancel( NULL, 
    "Deleting files.." )); 
    assert(note);
    return 1;
}*/

/* note: new cancel: set button text
int test4i()
{
    HildonNote *note;
    
    note = HILDON_NOTE( hildon_note_new_cancel( NULL, 
    "Deleting files.." )); 
    hildon_note_set_button_text( HILDON_NOTE( note ) , "Stop" );
    return 1;
}*/

/* note: new cancel with progress bar*/
int test4j()
{
  GtkWidget *progbar;
  GtkWidget *progress_note;
    
  progbar = gtk_progress_bar_new();

  gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progbar ), 0.7 );

  progress_note = hildon_note_new_cancel_with_progress_bar( 
  NULL, "Deleting files..", 
  GTK_PROGRESS_BAR( progbar ));
 
  assert(progress_note);
  return 1;
}

/* note: new cancel with progress bar: set button text*/
int test4k()
{
  GtkWidget *progbar;
  GtkWidget *progress_note;
    
  progbar = gtk_progress_bar_new();

  gtk_progress_bar_set_fraction( GTK_PROGRESS_BAR( progbar ), 0.7 );

  progress_note = hildon_note_new_cancel_with_progress_bar( 
  NULL, "Deleting files..", 
  GTK_PROGRESS_BAR( progbar ));
 
  hildon_note_set_button_text( HILDON_NOTE( progress_note ) , "Stop" );
  return 1;
}
/* destroy note widgets */
int test4l()
{
    GtkWidget *conf;
    GtkWidget *inf;
    /*    GtkWidget *cancel;*/
    GtkWidget *progress;
    GtkWidget *progbar;
    GtkWidget *conf_button;
    
    conf = hildon_note_new_confirmation(NULL, NULL);
    assert(conf);
    gtk_widget_destroy(conf);
    
    inf = hildon_note_new_information(NULL, NULL);
    assert(inf);
    gtk_widget_destroy(inf);
    
    /*
    cancel = hildon_note_new_cancel( NULL, NULL ); 
    assert(cancel);
    gtk_widget_destroy(cancel);
    */
    
    progbar = gtk_progress_bar_new();
    progress = hildon_note_new_cancel_with_progress_bar( 
    NULL, NULL, GTK_PROGRESS_BAR( progbar ));
    assert(progress);
    gtk_widget_destroy(progress);

    conf_button =  hildon_note_new_confirmation_add_buttons( NULL, NULL, NULL );
    assert(conf_button);
    gtk_widget_destroy(conf_button);
    
    return 1;
}

int test6a()
{
    HildonHVolumebar *volumeBar;
    
    volumeBar = HILDON_HVOLUMEBAR(hildon_hvolumebar_new());
    assert(volumeBar);
    return 1;
}

int test6b()
{
    HildonVVolumebar *volumeBar;
    
    volumeBar = HILDON_VVOLUMEBAR(hildon_vvolumebar_new());
    assert(volumeBar);
    return 1;
}

int test6c()
{
    HildonHVolumebar *volumeBar;
    gdouble volumeLevel = 0.5;
    
    volumeBar = HILDON_HVOLUMEBAR(hildon_hvolumebar_new());
    hildon_volumebar_set_level(HILDON_VOLUMEBAR(volumeBar), volumeLevel);
    assert(hildon_volumebar_get_level(HILDON_VOLUMEBAR(volumeBar)) == volumeLevel);
    return 1;
}

int test6d()
{
    HildonHVolumebar *volumeBar;
    gboolean mute = FALSE;
    
    volumeBar = HILDON_HVOLUMEBAR(hildon_hvolumebar_new());
    hildon_volumebar_set_mute(HILDON_VOLUMEBAR(volumeBar), mute);
    assert(hildon_volumebar_get_mute(HILDON_VOLUMEBAR(volumeBar)) == mute);
    return 1;
}

int test6e()
{
    HildonVVolumebar *volumeBar;
    
    volumeBar = HILDON_VVOLUMEBAR(hildon_vvolumebar_new());
    gtk_widget_destroy(GTK_WIDGET(volumeBar));
    return 1;
}

/* weekday_picker: new: ok */
int test10a()
{
    GtkWidget *picker;

    
    picker = hildon_weekday_picker_new();
    assert( picker );
    return 1;
}

/* weekday_picker: set/unset/isset: ok */
int test11a()
{
    gint i, j;
    HildonWeekdayPicker *picker;

    
    picker = HILDON_WEEKDAY_PICKER( hildon_weekday_picker_new() );
    assert( picker );
   
    /* set all one by one */
    for( i = 1; i < 8; ++i )
	  hildon_weekday_picker_set_day( picker, i );

    /* check that the proper days are set */
	for( j = 1; j < 8; ++j )
	   assert( hildon_weekday_picker_isset_day( picker, j ) );
    
    /* unset all one by one */
    for( i = 1; i < 8; ++i )
	  hildon_weekday_picker_unset_day( picker, i );

    /* check that the proper days are unset */
	for( j = 1; j < 8; ++j )
	  assert( !hildon_weekday_picker_isset_day( picker, j ) );
    
    return 1;
}

/* weekday_picker: set_day: null */
int test11b()
{
    
    hildon_weekday_picker_set_day( NULL, 0 );
    return 1;
}

/* weekday_picker: unset_day: null */
int test11c()
{
    
    hildon_weekday_picker_unset_day( NULL, 0 );
    return 1;
}

/* weekday_picker: isset_day: null */
int test11d()
{
    
    hildon_weekday_picker_isset_day( NULL, 0 );
    return 1;
}

/* weekday_picker: toggle: ok */
int test12a()
{
    gint i, j;
    HildonWeekdayPicker *picker;

    picker = HILDON_WEEKDAY_PICKER( hildon_weekday_picker_new() );
    assert( picker );
    
    /* set all one by one */
    for( i = 1; i < 8; i++ )
	  hildon_weekday_picker_toggle_day( picker, i );
    
    /* check that the proper days are set */
	for( j = 1; j < 8; j++ )
      assert ( hildon_weekday_picker_isset_day( picker, j ) );
   
    /* unset all one by one */
    for( i = 1; i < 8; ++i )
	  hildon_weekday_picker_toggle_day( picker, i );
    
    /* check that the proper days are unset */
	for( j = 1; j < 8; ++j )
	  assert( !hildon_weekday_picker_isset_day( picker, j ) );
    
    return 1;
}

/* weekday_picker: toggle_day: null */
int test12b()
{
    HildonWeekdayPicker *picker;
    picker = HILDON_WEEKDAY_PICKER( hildon_weekday_picker_new() );
    hildon_weekday_picker_toggle_day( NULL, 0 );
    return 1;
}

/* weekday_picker: set_all/unset_all*/
int test12c()
{
    HildonWeekdayPicker *picker;
    picker = HILDON_WEEKDAY_PICKER( hildon_weekday_picker_new() );
    hildon_weekday_picker_set_all( picker );
    hildon_weekday_picker_unset_all( picker );
    return 1;
}

/* telephone editor: new: ok */
int test13a()
{
    GtkWidget *editor;

    

    editor = hildon_telephone_editor_new( HILDON_TELEPHONE_EDITOR_FORMAT_FREE );
    assert( editor );

    editor = hildon_telephone_editor_new
      ( HILDON_TELEPHONE_EDITOR_FORMAT_COERCE );
    assert( editor );

    editor = hildon_telephone_editor_new
      ( HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY );
    assert( editor );

    editor = hildon_telephone_editor_new
      ( HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA );
    assert( editor );

    return 1;
}

/* telephone editor: new: inv */
int test13b()
{
    GtkWidget *editor;

    

    editor = hildon_telephone_editor_new( 17 );

    return 1;
}


/* telephone editor: set_editable: ok  */
int test14a()
{
    GtkWidget *editor;
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );
	hildon_telephone_editor_set_editable( HILDON_TELEPHONE_EDITOR( editor ),
					      TRUE, TRUE, TRUE );
	hildon_telephone_editor_set_editable( HILDON_TELEPHONE_EDITOR( editor ),
					      FALSE, FALSE, FALSE );
	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set_editable: null  */
int test14b()
{
    
    hildon_telephone_editor_set_editable( NULL, TRUE, TRUE, TRUE );

    return 1;
}
/* telephone editor: set_show_plus: ok  */
int test15a()
{
    GtkWidget *editor;
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );

	assert( editor );

	hildon_telephone_editor_set_show_plus( HILDON_TELEPHONE_EDITOR( editor ),
					       TRUE );

	/* only check if not in free mode (no plus there) */
	if( i )
	  assert( hildon_telephone_editor_get_show_plus( HILDON_TELEPHONE_EDITOR( editor ) ) 
		  == TRUE );
	hildon_telephone_editor_set_show_plus( HILDON_TELEPHONE_EDITOR( editor ),
					       FALSE );
	if( i )
	  assert( hildon_telephone_editor_get_show_plus( HILDON_TELEPHONE_EDITOR( editor ) )
		  == FALSE );
	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set_show_plus: null  */
int test15b()
{
    
    hildon_telephone_editor_set_show_plus( NULL, TRUE );

    return 1;
}

/* telephone editor: get_show_plus: null  */
int test15c()
{
    
    hildon_telephone_editor_get_show_plus( NULL );

    return 1;
}

/* telephone editor: set_show_border: ok  */
int test15d()
{
    GtkWidget *editor;
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );

	assert( editor );

	hildon_telephone_editor_set_show_border( HILDON_TELEPHONE_EDITOR( editor ),
						 TRUE );
	assert( hildon_telephone_editor_get_show_border
		( HILDON_TELEPHONE_EDITOR( editor ) ) == TRUE );

	hildon_telephone_editor_set_show_border( HILDON_TELEPHONE_EDITOR( editor ),
						 FALSE );
	assert( hildon_telephone_editor_get_show_border( HILDON_TELEPHONE_EDITOR( editor ) )
		== FALSE );

	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set_show_border: null  */
int test15e()
{
    
    hildon_telephone_editor_set_show_border( NULL, TRUE );

    return 1;
}

/* telephone editor: get_show_border: null  */
int test15f()
{
    
    hildon_telephone_editor_get_show_border( NULL );

    return 1;
}

/* telephone editor: set_widths: ok  */
int test16a()
{
    GtkWidget *editor;
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );
	hildon_telephone_editor_set_widths( HILDON_TELEPHONE_EDITOR( editor ),
					    0, 0, 0 );
	hildon_telephone_editor_set_widths( HILDON_TELEPHONE_EDITOR( editor ),
					    10, 10, 10 );
	hildon_telephone_editor_set_widths( HILDON_TELEPHONE_EDITOR( editor ),
					    100, 100, 100 );
	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set_widths: null  */
int test16b()
{
    
    hildon_telephone_editor_set_widths( NULL, 1, 1, 1 );

    return 1;
}

/* telephone editor: set/get_country: ok  */
int test17a()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "123456";
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );
	hildon_telephone_editor_set_country( HILDON_TELEPHONE_EDITOR( editor ),
					     teststr );
	c = hildon_telephone_editor_get_country
	    ( HILDON_TELEPHONE_EDITOR( editor ) );

	/* format != FREE */
	if( i )
	{
	    g_assert( c );
	    g_assert( strcmp( teststr, c ) == 0 );
	}

	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set/get_country: inv  */
int test17b()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "123456p";

    
    
    editor = hildon_telephone_editor_new( 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY );
    hildon_telephone_editor_set_country( HILDON_TELEPHONE_EDITOR( editor ),
					 teststr );
    c = hildon_telephone_editor_get_country
	( HILDON_TELEPHONE_EDITOR( editor ) );

    g_assert( c );
    g_assert( strcmp( teststr, c ) != 0 );

    gtk_widget_destroy( editor );

    return 1;
}

/* telephone editor: set_country: null  */
int test17c()
{
    
    hildon_telephone_editor_set_country( NULL, "123" );

    return 1;
}

/* telephone editor: get_country: null  */
int test17d()
{
    
    hildon_telephone_editor_get_country( NULL );

    return 1;
}

/* telephone editor: set/get_area: ok  */
int test18a()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "123456";
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );
	hildon_telephone_editor_set_area( HILDON_TELEPHONE_EDITOR( editor ),
					     teststr );
	c = hildon_telephone_editor_get_area
	    ( HILDON_TELEPHONE_EDITOR( editor ) );

	/* format != FREE nor COUNTRY */
	if( i && i != 2 )
	{
	    g_assert( c );
	    
	    g_assert( strncmp( teststr, c, strlen( c ) ) == 0 );
	}

	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set/get_area: inv  */
int test18b()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "123456p";

    
    
    editor = hildon_telephone_editor_new( 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA );
    hildon_telephone_editor_set_area( HILDON_TELEPHONE_EDITOR( editor ),
					 teststr );
    c = hildon_telephone_editor_get_area
	( HILDON_TELEPHONE_EDITOR( editor ) );

    g_assert( c );
    g_assert( strcmp( teststr, c ) != 0 );

    gtk_widget_destroy( editor );

    return 1;
}

/* telephone editor: set_area: null  */
int test18c()
{
    
    hildon_telephone_editor_set_area( NULL, "123" );

    return 1;
}

/* telephone editor: get_area: null  */
int test18d()
{
    
    hildon_telephone_editor_get_area( NULL );

    return 1;
}
 
/* telephone editor: set/get_number: ok  */
int test19a()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "123456";
    HildonTelephoneEditorFormat fmt[4] =  {
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE,
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_COUNTRY, 
	HILDON_TELEPHONE_EDITOR_FORMAT_COERCE_AREA };
    gint i;

    
    
    for( i = 0; i < 4; ++i )
    {
	editor = hildon_telephone_editor_new( fmt[i] );
	hildon_telephone_editor_set_number( HILDON_TELEPHONE_EDITOR( editor ),
					     teststr );
	c = hildon_telephone_editor_get_number
	    ( HILDON_TELEPHONE_EDITOR( editor ) );

	/* free != COUNTRY nor AREA */
	if( i != 2 && i != 3 )
	{
	    g_assert( c );
	    g_assert( strcmp( teststr, c ) == 0 );
	}

	gtk_widget_destroy( editor );
    }

    return 1;
}

/* telephone editor: set/get_number: ok in FREE format  */
int test19b()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "()+?*pw1234567890";
    
    editor = hildon_telephone_editor_new( 
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE );
    hildon_telephone_editor_set_number( HILDON_TELEPHONE_EDITOR( editor ),
					 teststr );
    c = hildon_telephone_editor_get_number
	( HILDON_TELEPHONE_EDITOR( editor ) );

    g_assert( c );
    g_assert( strncmp( teststr, c, strlen(c) ) == 0 );

    gtk_widget_destroy( editor );

    return 1;
}

/* telephone editor: set/get_number: inv  */
int test19c()
{
    GtkWidget *editor;
    const gchar *c;
    gchar teststr[] = "123456f";

    
    
    editor = hildon_telephone_editor_new( 
	HILDON_TELEPHONE_EDITOR_FORMAT_FREE );
    hildon_telephone_editor_set_number( HILDON_TELEPHONE_EDITOR( editor ),
					 teststr );
    c = hildon_telephone_editor_get_number
	( HILDON_TELEPHONE_EDITOR( editor ) );

    g_assert( c );
    g_assert( strcmp( teststr, c ) != 0 );

    gtk_widget_destroy( editor );

    return 1;
}

/* telephone editor: set_number: null  */
int test19d()
{   
    hildon_telephone_editor_set_number( NULL, "123" );

    return 1;
}

/* telephone editor: get_number: null  */
int test19e()
{    
    hildon_telephone_editor_get_number( NULL );

    return 1;
}

/* find object options dialog: new */
/*int test21a(void){
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  g_assert(dialog);
  return 1;
}*/

/* find object options dialog: hide_location */
/*int test21b(void){
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  hildon_find_object_options_dialog_hide_location(
            HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog));
  gtk_widget_destroy(dialog);
  
  return 1;
}*/

/* find object options dialog: hide_find_in */
/*int test21c(void){
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  hildon_find_object_options_dialog_hide_find_in(
            HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog));

  return 1;  
}*/

/* find object options dialog: get_location */
/*int test21d(void){
  const gchar *retString;
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  retString = hildon_find_object_options_dialog_get_location(
				 HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog));
  
  g_assert(retString);

  return 1;
}*/

/* find object options dialog: get_find_type */
/*int test21e(void){
  HildonFindInType find_type;
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  
  find_type = hildon_find_object_options_dialog_get_find_type(
                     HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog));

  return 1;
}*/

/* find object options dialog: get_time_period_type */
/*int test21f(void){
  HildonTimePeriodType type;
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  type = hildon_find_object_options_dialog_get_time_period_type(
                     HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog));
  return 1;
}*/

/* find object options dialog: get_start_date */
/*int test21g(void){
  GtkWidget *dialog;
  gint year, month, day;

  dialog = hildon_find_object_options_dialog_new(NULL);

  hildon_find_object_options_dialog_get_start_date(HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog),
						   &year, &month, &day);

  g_assert(year);
  g_assert(month);
  g_assert(day);

  return 1;
}*/

/* find object options dialog: get_end_date */
/*int test21h(void){
  GtkWidget *dialog;
  gint year, month, day;

  dialog = hildon_find_object_options_dialog_new(NULL);

  hildon_find_object_options_dialog_get_end_date(HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog), 
						 &year, &month, &day);

  g_assert(year);
  g_assert(month);
  g_assert(day);

  return 1;
}*/

/* find object options dialog: add_custom_options */
/*int test21i(void){
  GtkWidget *dialog;
  GtkWidget *widget = gtk_entry_new();

  dialog = hildon_find_object_options_dialog_new(NULL);

  hildon_find_object_options_dialog_add_custom_option(
      HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog), "New widget", widget);

  return 1;
}*/

/* find object options dialog: date_matches */
/*int test21j(void){
  GtkWidget *dialog;
  dialog = hildon_find_object_options_dialog_new(NULL);
  
  g_assert(hildon_find_object_dialog_date_matches(HILDON_FIND_OBJECT_OPTIONS_DIALOG(dialog),
						  1, 1, 1979));

  return 1;
}*/
 
/* range editor: new range editor: ok parameters */
int test22a()
{
  HildonRangeEditor *range_editor;

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new());
  assert(range_editor);
  return 1;
}

/* range editor: new range editor with separator: ok parameters */
int test22b()
{
  HildonRangeEditor *range_editor;
  
  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new_with_separator(
                                     "String"));
  assert(range_editor);
  return 1;
}

/* range editor: new range editor with separator: NULL parameters */
int test22c()
{
  HildonRangeEditor *range_editor;

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new_with_separator(NULL));
  assert(range_editor);
  return 1;
}

/* range editor: get range: ok parameters */
int test22d()
{
  HildonRangeEditor *range_editor;
  gint start, end;

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new());
  hildon_range_editor_get_range (range_editor, &start, &end);

  assert (start == -999);
  assert (end == 999);

  return 1;
}

/* range editor: get range: null range editor */
int test22e()
{
  HildonRangeEditor *range_editor = NULL;
  gint start, end;

  hildon_range_editor_get_range (range_editor, &start, &end);

  return 1;
}

/* range editor: get range: null end */
int test22f()
{
  HildonRangeEditor *range_editor = NULL;
  gint start;

  hildon_range_editor_get_range (range_editor, &start, NULL);

  return 1;
}

/* range editor: get range: null start */
 int test22g()
{
  HildonRangeEditor *range_editor = NULL;
  gint end;

  hildon_range_editor_get_range (range_editor, NULL, &end);

  return 1;
}

/* range editor: get range: null start, end */
int test22h()
{
  HildonRangeEditor *range_editor = NULL;

  hildon_range_editor_get_range (range_editor, NULL, NULL);

  return 1;
}

/* range editor: set range: ok parameters */
int test22i()
{
  HildonRangeEditor *range_editor;
  gint start = 5, end = 6;

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new());

  hildon_range_editor_set_range (range_editor, start, end);
  hildon_range_editor_get_range (range_editor, &start, &end);

  assert (start   == 5);
  assert (end == 6);

  return 1;
}

/* range editor: set range: null range editor*/
int test22j()
{
  HildonRangeEditor *range_editor = NULL;
  gint start = 5, end = 6;

  hildon_range_editor_set_range (range_editor, start, end);

  return 1;
}

/* range editor: set limits: ok parameters */
int test22k()
{
  HildonRangeEditor *range_editor;
  gint start = 0, end = 0;

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new());

  hildon_range_editor_set_limits (range_editor, start, end);
  hildon_range_editor_get_range (range_editor, &start, &end);

  assert (start == 0);
  assert (end == 0);

  return 1;
}

/* range editor: set limits: set range over start minimum and end maximum */
int test22l()
{
  HildonRangeEditor *range_editor;
  gint start = -1, end = 1;

  range_editor = HILDON_RANGE_EDITOR(hildon_range_editor_new());

  hildon_range_editor_set_limits (range_editor, 0, 0);
  hildon_range_editor_set_range (range_editor, start, end);
  hildon_range_editor_get_range (range_editor, &start, &end);

  assert (start  == 0);
  assert (end == 0);

  return 1;
}

/* range editor: set limits: NULL editor */
int test22m()
{
  HildonRangeEditor *range_editor = NULL;
  gint start = 0, end = 0;

  hildon_range_editor_set_limits (range_editor, start, end);

  return 1;
}


/* sort dialog:  */
int test23a(void)
{

  HildonSortDialog *sort_dialog;

  sort_dialog = HILDON_SORT_DIALOG(hildon_sort_dialog_new(NULL));
  assert(sort_dialog);
  return 1;
}

/* sort dialog: Get sort key. '0' is the default index value */
int test23b(void)
{

  HildonSortDialog *sort_dialog;
  gint default_key = 0;
    
  sort_dialog = HILDON_SORT_DIALOG(hildon_sort_dialog_new(NULL));
  assert(hildon_sort_dialog_get_sort_key( sort_dialog ) == default_key );
  return 1;
}

/* sort dialog: Set sort key */
int test23c(void)
{

  HildonSortDialog *sort_dialog;
  gint key = 1;
   
  sort_dialog = HILDON_SORT_DIALOG(hildon_sort_dialog_new(NULL));
  hildon_sort_dialog_set_sort_key( sort_dialog, key );
  assert(hildon_sort_dialog_get_sort_key( sort_dialog ) == 1 );
  return 1;
}

/* sort dialog: add sort key */
int test23d(void)
{

  HildonSortDialog *sort_dialog;
  gint key = 1;
  gint added_key_index = 0;
   
  sort_dialog = HILDON_SORT_DIALOG (hildon_sort_dialog_new(NULL));
  hildon_sort_dialog_add_sort_key (sort_dialog, "Random");
  added_key_index = hildon_sort_dialog_add_sort_key (sort_dialog,
						     "Date");
                                                     
  hildon_sort_dialog_set_sort_key( sort_dialog, added_key_index );
  assert(hildon_sort_dialog_get_sort_key( sort_dialog ) == key );
  return 1;
}

/* sort dialog: get sort order. '0' is the default index value */
int test23e(void)
{

  HildonSortDialog *sort_dialog;
  GtkSortType type = 0;

  sort_dialog = HILDON_SORT_DIALOG(hildon_sort_dialog_new(NULL));
  assert(hildon_sort_dialog_get_sort_order( sort_dialog ) == type);
  return 1;
}

/* sort dialog: set sort order */
int test23f(void)
{

  HildonSortDialog *sort_dialog;
  GtkSortType type = 1;

  sort_dialog = HILDON_SORT_DIALOG(hildon_sort_dialog_new(NULL));
  hildon_sort_dialog_set_sort_order( sort_dialog , type);
  assert(hildon_sort_dialog_get_sort_order( sort_dialog ) == 1);
  return 1;
}

/* sort dialog: gtk_widget_destroy  */
int test23g()
{
    GtkWidget *sort_dialog;
     
    sort_dialog = hildon_sort_dialog_new(FALSE);
    gtk_widget_destroy(GTK_WIDGET(sort_dialog));
        
    return 1;
}

/* Get password dialog */
int test24a(void)
{

  HildonGetPasswordDialog *get_dialog;
 
  get_dialog = HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new(NULL,
                                          FALSE));
  assert(get_dialog);
  return 1;
}

/* Get Old password dialog */
int test24b(void)
{

  HildonGetPasswordDialog *get_old_dialog;
 
  get_old_dialog = HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new(NULL,
                                              TRUE));
  assert(get_old_dialog);
  return 1;
}

/* Get / Get Old password dialog: get password  */
int test24c(void)
{
  HildonGetPasswordDialog *get_dialog;
  HildonGetPasswordDialog *get_old_dialog;
  const gchar *get_string;
  const gchar *get_old_string;
 
  get_dialog = HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new(NULL, 
                                          FALSE));
  get_string = hildon_get_password_dialog_get_password( get_dialog);
  assert(get_string);
  
  get_old_dialog = HILDON_GET_PASSWORD_DIALOG(hildon_get_password_dialog_new(NULL,
                                              TRUE));
  get_old_string = hildon_get_password_dialog_get_password( get_old_dialog);
  assert(get_old_string);
  return 1;
}

/* get / get old password dialog: gtk_widget_destroy  */
int test24d()
{
    GtkWidget *get_dialog;
    GtkWidget *get_old_dialog;
    
    get_dialog = hildon_get_password_dialog_new(NULL, FALSE);
    gtk_widget_destroy(GTK_WIDGET(get_dialog));
    
    get_old_dialog = hildon_get_password_dialog_new(NULL, TRUE);
    gtk_widget_destroy(GTK_WIDGET(get_old_dialog));
    
    return 1;
}

/* set password dialog */
int test25a(void)
{

  HildonSetPasswordDialog *set_dialog;
 
  set_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(NULL, 
                                          FALSE));
  assert(set_dialog);
  return 1;
}

/* change password dialog */
int test25b(void)
{

  HildonSetPasswordDialog *change_dialog;
 
  change_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(
                                             NULL, TRUE));
  assert(change_dialog);
  return 1;
}

/* set / change password dialog: get password  */
int test25c(void)
{
  HildonSetPasswordDialog *set_dialog;
  HildonSetPasswordDialog *change_dialog;
  const gchar *set_string;
  const gchar *change_string;
 
  set_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(
                                          NULL, FALSE));
  set_string = hildon_set_password_dialog_get_password( set_dialog);
  assert(set_string);
  
  change_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(
                                             NULL, TRUE));
  change_string = hildon_set_password_dialog_get_password( change_dialog);
  assert(change_string);
  return 1;
}

/* set / change password dialog: get protected  */
int test25d(void)
{                                     
  HildonSetPasswordDialog *set_dialog;
  HildonSetPasswordDialog *change_dialog;
  gboolean set;
  gboolean change;
 
  set_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(
                                          NULL, FALSE));
  set = hildon_set_password_dialog_get_protected(set_dialog);
  assert(set);
  
  change_dialog = HILDON_SET_PASSWORD_DIALOG(hildon_set_password_dialog_new(
                                             NULL, TRUE));
  change = hildon_set_password_dialog_get_protected(change_dialog);
  assert(change);
  return 1; 
} 
/* set / change password dialog: gtk_widget_destroy  */
int test25e()
{
    GtkWidget *set_dialog;
    GtkWidget *change_dialog;
    
    set_dialog = hildon_set_password_dialog_new(NULL, FALSE);
    gtk_widget_destroy(GTK_WIDGET(set_dialog));
    
    change_dialog = hildon_set_password_dialog_new(NULL, TRUE);
    gtk_widget_destroy(GTK_WIDGET(change_dialog));
    
    return 1;
}

#ifndef HILDON_DISABLE_DEPRECATED
int fsd_get_font(void)
{
	GtkWidget *fsd;
	PangoAttrList *list = NULL;
	fsd = hildon_font_selection_dialog_new( NULL, "fsd" );
	list = hildon_font_selection_dialog_get_font(HILDON_FONT_SELECTION_DIALOG(fsd));
	assert( list!=NULL );
	return 1;
}

int fsd_set_font(void)
{
	GtkWidget *fsd;
	PangoAttrList *list = NULL;
	fsd = hildon_font_selection_dialog_new( NULL, "fsd" );
	list = hildon_font_selection_dialog_get_font(HILDON_FONT_SELECTION_DIALOG(fsd));
	hildon_font_selection_dialog_set_font(HILDON_FONT_SELECTION_DIALOG(fsd), list);
	return 1;
}
#endif
int fsd_set_pretext(void)
{
	GtkWidget *fsd;
	char *str = "Mopo karkas käsistä";
	fsd = hildon_font_selection_dialog_new( NULL, "fsd" );
	hildon_font_selection_dialog_set_preview_text(HILDON_FONT_SELECTION_DIALOG(fsd),str);
	return 1;
}

int fsd_get_pretext(void)
{
	GtkWidget *fsd;
	char *str = NULL;
	fsd = hildon_font_selection_dialog_new( NULL, "fsd" );
	str = (char*)hildon_font_selection_dialog_get_preview_text(
	    HILDON_FONT_SELECTION_DIALOG(fsd));

	assert( str!=NULL );
	return 1;
}
/* insert object dialog: new */
int test27a(void){
  GtkWidget *dialog;
  dialog = hildon_insert_object_dialog_new(NULL);
  g_assert(dialog);
  return 1;  
}

/* insert object dialog: get_name */
int test27b(void){
  const gchar *string;
  GtkWidget *dialog;

  dialog = hildon_insert_object_dialog_new(NULL);
  string = hildon_insert_object_dialog_get_name(HILDON_INSERT_OBJECT_DIALOG(dialog));
  g_assert(string);
  return 1;
}

/* insert object dialog: get_mime_type */
int test27c(void){
  const gchar *string;
  GtkWidget *dialog;

  dialog = hildon_insert_object_dialog_new(NULL);
  string = hildon_insert_object_dialog_get_mime_type(HILDON_INSERT_OBJECT_DIALOG(dialog));
  g_assert(string);
  return 1;
}

/* hildon_number_editor_new */
int test28a(void){
  GtkWidget *editor;
  gint min = -120;
  gint max = 900;

  editor = hildon_number_editor_new (min, max);
  g_assert(editor);

  return 1;
}

/* hildon_number_editor_get_value  */
int test28b(void){
  GtkWidget *editor;
  gint min = 450;
  gint max = 7500;
  gint value;

  editor = hildon_number_editor_new (min, max);
  value = hildon_number_editor_get_value (HILDON_NUMBER_EDITOR(editor));
  g_assert (value == max);

  return 1;
}

/* hildon_number_editor_set_range  */
int test28c(void){
  GtkWidget *editor;
  gint min = 450;
  gint max = 7500;
  gint value;

  editor = hildon_number_editor_new (min, max);
  hildon_number_editor_set_range (HILDON_NUMBER_EDITOR (editor), 30, 80);
  value = hildon_number_editor_get_value (HILDON_NUMBER_EDITOR(editor));
  g_assert (value == 80);

  return 1;
}

/* hildon_number_editor_set_value */
int test28d(void){
  GtkWidget *editor;
  gint min = 450;
  gint max = 7500;
  gint value;

  editor = hildon_number_editor_new (min, max);
  hildon_number_editor_set_value (HILDON_NUMBER_EDITOR (editor), 3450);
  value = hildon_number_editor_get_value (HILDON_NUMBER_EDITOR(editor));
  g_assert (value == 3450);

return 1;
}

/* Hildon-Color-Selector: 
    hildon_color_selector_new() 
    with base color
 */ 
int test29a(void){
  GtkWidget *colselector;
  GdkColor redcolor = {0, 65535, 0, 0 };
  GdkColor *color;

  colselector = hildon_color_selector_new( NULL );

  hildon_color_selector_set_color( HILDON_COLOR_SELECTOR(colselector), &(redcolor) );
  color = hildon_color_selector_get_color( HILDON_COLOR_SELECTOR(colselector) );

  g_assert( colselector );
  g_assert( redcolor.pixel == color->pixel );
  g_assert( redcolor.red   == color->red   );
  g_assert( redcolor.green == color->green );
  g_assert( redcolor.blue  == color->blue  );

  g_free(color);
  gtk_widget_destroy ( colselector );   

  return 1;
}
/* Hildon-Color-Selector: 
   HildonColorSelector with non-base color
 */ 
int test29b(void){

  HildonColorSelector *colorsel;
  GtkWidget *obj_self;
  GdkColor mixedcolor = { 0, 7281, 21845, 65535 };   
  GdkColor *color;

  obj_self =  hildon_color_selector_new( NULL );
  colorsel = HILDON_COLOR_SELECTOR ( obj_self  );

  hildon_color_selector_set_color( colorsel, &mixedcolor );
  color = hildon_color_selector_get_color( colorsel ); 	

  g_assert( colorsel );
  g_assert( color->pixel == 0 );
  g_assert( color->red   == 7281 );
  g_assert( color->green == 21845 );
  g_assert( color->blue  == 65535 ); 

  g_free( color ); 
  g_object_unref ( obj_self );
  gtk_widget_destroy( GTK_WIDGET(colorsel) );
  
  return 1;
}

/* hildon_file_handling_note_new_moving */
int test30a(void){

   GtkWidget *file_note;
  
   file_note = hildon_file_handling_note_new_moving (NULL);
   hildon_file_handling_note_set_fraction (HILDON_FILE_HANDLING_NOTE(file_note), 0.7);

   g_assert (file_note);
  
   return 1;
}

/* hildon_file_handling_note_new_deleting */
int test30b(void){
    
   GtkWidget *file_note;
  
   file_note = hildon_file_handling_note_new_deleting (NULL);
   hildon_file_handling_note_set_counter_and_name (HILDON_FILE_HANDLING_NOTE (file_note), 
						   1, 2, "bar.txt");
   hildon_file_handling_note_set_counter_and_name (HILDON_FILE_HANDLING_NOTE (file_note), 
						   2, 2, "foo.txt"); 
   g_assert (file_note);
  
   return 1;
}

/* hildon_file_handling_note_new_opening */
int test30c(void){
    
   GtkWidget *file_note;
  
   file_note = hildon_file_handling_note_new_opening (NULL);
   hildon_file_handling_note_set_name (HILDON_FILE_HANDLING_NOTE (file_note),
   "baz.txt");
   g_assert (file_note);
  
   return 1;
}

/* hildon_file_handling_note_new_saving */
int test30d(void){
    
   GtkWidget *file_note;
  
   file_note = hildon_file_handling_note_new_saving (NULL);
   hildon_file_handling_note_set_fraction (HILDON_FILE_HANDLING_NOTE (file_note), 0.7);
   g_assert (file_note);
  
   return 1;
}

/* Add to Home dialog new: ok parameters */
int test31a(void){
    GtkWidget *d;
    
    d = hildon_add_home_dialog_new( NULL, "name" , NULL);
    g_assert( d );

    return 1;
}

/* Add to Home dialog new: NULL parameter (is ok) */
int test31b(void){
    GtkWidget *d;
    
    d = hildon_add_home_dialog_new( NULL, NULL, NULL );
    g_assert( d );

    return 1;
}

/* Rename dialog new: ok parameters */
int test32a(void){
    GtkWidget *d;
    
    d = hildon_add_home_dialog_new( NULL, "name", "newname" );
    g_assert( d );

    return 1;
}

/* Rename dialog new: NULL parameter (is ok) */
int test32b(void){
    GtkWidget *d;
    
    d = hildon_add_home_dialog_new( NULL, NULL, "newname" );
    g_assert( d );

    return 1;
}

/* Create item with icon basename. */
int test33a1 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    item = hildon_grid_item_new (VALID_ICON);
    g_assert (item);
    return 1;
}

/* Create item without icon basename. */
int test33a2 (void)
{
    GtkWidget *item;
    item = hildon_grid_item_new (NULL);
    g_assert (item);
    return 1;
}

/* Create item with icon basename and label. */
int test33b1 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    item = hildon_grid_item_new_with_label (VALID_ICON, "foo");
    g_assert (item);
    return 1;
}

/* Create item with icon basename but with no label. */
int test33b2 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    item = hildon_grid_item_new_with_label (VALID_ICON, NULL);
    g_assert (item);
    return 1;
}
int test33b3 (void)
{
    GtkWidget *item;
    item = hildon_grid_item_new_with_label (NULL, "foo");
    g_assert (item);
    return 1;
}

/* Create item without icon basename and without label. */
int test33b4 (void)
{
    GtkWidget *item;
    item = hildon_grid_item_new_with_label (NULL, NULL);
    g_assert (item);
    return 1;
}

/* Set emblem type. Non-NULL item, non-NULL emblem. */
int test33c1 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    item = hildon_grid_item_new (VALID_ICON);
    hildon_grid_item_set_emblem_type (HILDON_GRID_ITEM (item), VALID_ICON);
    return 1;
}

/* Set emblem type. Non-NULL item, NULL emblem. */
int test33c2 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    item = hildon_grid_item_new (VALID_ICON);
    hildon_grid_item_set_emblem_type (HILDON_GRID_ITEM (item), NULL);
    return 1;
}

/* Set emblem type. NULL item, non-NULL emblem. */
int test33c3 (void)
{
    hildon_grid_item_set_emblem_type (NULL, VALID_ICON);
    return 1;
}

/* Set emblem type. NULL item, NULL emblem. */
int test33c4 (void)
{
    hildon_grid_item_set_emblem_type (NULL, NULL);
    return 1;
}

/* Get emblem type from existing item - has emblem. */
int test33d1 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    const gchar *type;
    item = hildon_grid_item_new (VALID_ICON);
    hildon_grid_item_set_emblem_type (HILDON_GRID_ITEM (item), VALID_ICON);
    type = hildon_grid_item_get_emblem_type (HILDON_GRID_ITEM (item));
    g_assert (type);
    return 1;
}

/* Get emblem type from existing item - has no emblem. */
int test33d2 (void)
{
    /* If test fails, first see if icon exists. */
    GtkWidget *item;
    const gchar *type;
    item = hildon_grid_item_new (VALID_ICON);
    type = hildon_grid_item_get_emblem_type (HILDON_GRID_ITEM (item));
    g_assert (type != NULL);
    return 1;
}

/* Get emblem type from non-existing item. */
int test33d3 (void)
{
    const gchar *type;
    type = hildon_grid_item_get_emblem_type (NULL);
    g_assert (type != NULL);
    return 1;
}

/* Destroy griditem. */
int test33e(void)
{
    GtkWidget *item;
    item = hildon_grid_item_new (VALID_ICON);
    gtk_widget_destroy (item);
    return 1;
}

/* Create new grid. */
int test34a (void)
{
    GtkWidget *grid;
    grid = hildon_grid_new ();
    g_assert (grid);
    return 1;
}

/* Set grid style. */
int test34b1 (void)
{
    GtkWidget *grid;
    grid = hildon_grid_new ();
    hildon_grid_set_style (HILDON_GRID (grid), "bar");
    return 1;
}

/* Set grid style (no style). */
int test34b2 (void)
{
    GtkWidget *grid;
    grid = hildon_grid_new ();
    hildon_grid_set_style (HILDON_GRID (grid), NULL);
    return 1;
}

/* Set grid style. Invalid grid. */
int test34b3 (void)
{
    hildon_grid_set_style (NULL, "bar");
    return 1;
}

/* Set grid style. Invalid grid, no style. */
int test34b4 (void)
{
    hildon_grid_set_style (NULL, NULL);
    return 1;
}

/* Get grid style. Valid grid, has style. */
int test34c1 (void)
{
    GtkWidget *grid;
    const gchar *style;
    grid = hildon_grid_new ();
    hildon_grid_set_style (HILDON_GRID (grid), "foo");
    style = hildon_grid_get_style (HILDON_GRID (grid));
    g_assert (style);
    return 1;
}

/* Get grid style. Valid grid, has no style. */
int test34c2 (void)
{
    GtkWidget *grid;
    const gchar *style;
    grid = hildon_grid_new ();
    hildon_grid_set_style (HILDON_GRID (grid), NULL);
    style = hildon_grid_get_style (HILDON_GRID (grid));
    g_assert (style != NULL && strcmp (style, "HildonGrid") == 0);
    return 1;
}

/* Get grid style. Invalid grid. */
int test34c3 (void)
{
    const gchar *style;
    style = hildon_grid_get_style (NULL);
    g_assert (style);
    return 1;
}

/* Set grid scrollbar position. Valid grid. */
int test34d1 (void)
{
    GtkWidget *grid;
    grid = hildon_grid_new ();
    hildon_grid_set_scrollbar_pos (HILDON_GRID (grid), 0);
    return 1;
}

/* Set grid scrollbar position. Invalid grid. */
int test34d2 (void)
{
    hildon_grid_set_scrollbar_pos (NULL, 0);
    return 1;
}

/* Get grid scrollbar position. Valid grid. */
int test34e1 (void)
{
    GtkWidget *grid;
    gint pos;
    grid = hildon_grid_new ();
    pos = hildon_grid_get_scrollbar_pos (HILDON_GRID (grid));
    return 1;
}

/* Get grid scrollbar position. Inalid grid. */
int test34e2 (void)
{
    gint pos;
    pos = hildon_grid_get_scrollbar_pos (NULL);
    return 1;
}

/* Add item to existing grid. */
int test34f1 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    return 1;
}

/* Add NULL to existing grid. */
int test34f2 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), NULL);
    return 1;
}

/* Add something to existing grid. */
int test34f3 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = gtk_button_new ();
    gtk_container_add (GTK_CONTAINER (grid), item);
    return 1;
}

/* Add something to non-existing grid. */
int test34f4 (void)
{
    GtkWidget *item;
    item = gtk_button_new ();
    gtk_container_add (NULL, item);
    return 1;
}

/* Add something to invalid grid. */
int test34f5 (void)
{
    GtkWidget *item;
    item = gtk_button_new ();
    gtk_container_add (GTK_CONTAINER (item), item);
    return 1;
}

/* Remove item from grid. */
int test34g1 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_container_remove (GTK_CONTAINER (grid), item);
    return 1;
}

/* Remove non-existing item from grid. */
int test34g2 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_remove (GTK_CONTAINER (grid), item);
    return 1;
}

/* Remove NULL from grid. */
int test34g3 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_container_remove (GTK_CONTAINER (grid), NULL);
    return 1;
}

/* Remove something from grid. */
int test34g4 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = gtk_button_new ();
    gtk_container_remove (GTK_CONTAINER (grid), item);
    return 1;
}

/* Remove something from NULL grid. */
int test34g5 (void)
{
    GtkWidget *item;
    item = gtk_button_new ();
    gtk_container_remove (NULL, item);
    return 1;
}

/* Remove something from invalid grid. */
int test34g6 (void)
{
    GtkWidget *item0, *item1;
    item0 = gtk_button_new ();
    item1 = gtk_button_new ();
    gtk_container_remove (GTK_CONTAINER (item0), item1);
    return 1;
}

/* Set focus. */
int test34h1 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_container_set_focus_child (GTK_CONTAINER (grid), item);
    return 1;
}

/* Set focus to non-existing. */
int test34h2 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_container_set_focus_child (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_set_focus_child (GTK_CONTAINER (grid), item);
    return 1;
}

/* Set focus to NULL. */
int test34h3 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_container_set_focus_child (GTK_CONTAINER (grid), item);
    return 1;
}

/* Set focus to something. */
int test34h4 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = gtk_button_new ();
    gtk_container_set_focus_child (GTK_CONTAINER (grid), item);
    return 1;
}

/* Get focus. */
int test34i1 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_container_set_focus_child (GTK_CONTAINER (grid), item);
    g_assert (GTK_CONTAINER (grid)->focus_child == item);
    return 1;
}

/* Get focus (no focused). */
int test34i2 (void)
{
    GtkWidget *grid;
    grid = hildon_grid_new ();
    g_assert (GTK_CONTAINER (grid)->focus_child == NULL);
    return 1;
}

/* Destroy grid. Has items. */
int test34j1 (void)
{
    GtkWidget *grid;
    grid = hildon_grid_new ();
    gtk_widget_destroy (grid);
    return 1;
}

/* Destroy grid. No items. */
int test34j2 (void)
{
    GtkWidget *grid;
    GtkWidget *item;
    grid = hildon_grid_new ();
    item = hildon_grid_item_new (VALID_ICON);
    gtk_container_add (GTK_CONTAINER (grid), item);
    gtk_widget_destroy (grid);
    return 1;
}

/* HildonTimeEditor: create editor widget */
int test35a (void){
    GtkWidget *editor;
    editor = hildon_time_editor_new ();
    g_assert (editor);
    return 1;
}

/* HildonTimeEditor: set time and check if returned
 * time value is same 
 */
int test35b (void){
    GtkWidget *editor;
    guint hour, min, sec;
    editor = hildon_time_editor_new ();
    hildon_time_editor_set_time (HILDON_TIME_EDITOR (editor), 
                                 21, 39, 45);
    hildon_time_editor_get_time (HILDON_TIME_EDITOR (editor),
                                 &hour, &min, &sec);
    g_assert (hour == 21);
    g_assert (min == 39);
    g_assert (sec ==45);
    return 1;
}

/* HildonTimeEditor: enable/disable seconds */
int test35c (void){
    GtkWidget *editor;
    editor = hildon_time_editor_new ();
#ifndef HILDON_DISABLE_DEPRECATED    
    hildon_time_editor_show_seconds (HILDON_TIME_EDITOR (editor), 1);
    hildon_time_editor_show_seconds (HILDON_TIME_EDITOR (editor), 0);
#else
    hildon_time_editor_set_show_seconds (HILDON_TIME_EDITOR (editor), 1);
    hildon_time_editor_set_show_seconds (HILDON_TIME_EDITOR (editor), 0);
#endif
    return 1;
}

/* HildonTimeEditor: enable duration mode */
int test35d (void){
    GtkWidget *editor;
    editor = hildon_time_editor_new ();
#ifndef HILDON_DISABLE_DEPRECATED    
    hildon_time_editor_enable_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
#else
    hildon_time_editor_set_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
#endif
    return 1;
}

/* HildonTimeEditor: set duration range and check values
 * against returned values 
 */
int test35e (void){
    GtkWidget *editor;
    guint min_seconds, max_seconds;
    editor = hildon_time_editor_new ();
#ifndef HILDON_DISABLE_DEPRECATED    
    hildon_time_editor_enable_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
#else
    hildon_time_editor_set_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
#endif
    hildon_time_editor_set_duration_range (HILDON_TIME_EDITOR (editor),
                                           3600, 14500);
    hildon_time_editor_get_duration_range (HILDON_TIME_EDITOR (editor),
                                           &min_seconds, &max_seconds);
    g_assert (min_seconds == 3600);
    g_assert (max_seconds == 14500);
    return 1;
}

/* HildonTimeEditor: enable and disable duration modes */
int test35f (void){
    GtkWidget *editor;
    editor = hildon_time_editor_new ();
#ifndef HILDON_DISABLE_DEPRECATED    
    hildon_time_editor_enable_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
    hildon_time_editor_enable_duration_mode (HILDON_TIME_EDITOR (editor),
                                             0);
#else
    hildon_time_editor_set_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
    hildon_time_editor_set_duration_mode (HILDON_TIME_EDITOR (editor),
                                             0);
#endif
    return 1;
}

/* HildonTimeEditor: set time and check against
 * foo values
 */
int test35g (void){
    GtkWidget *editor;
    guint hour, min, sec;
    editor = hildon_time_editor_new ();
    hildon_time_editor_set_time (HILDON_TIME_EDITOR (editor), 
                                 21, 39, 45);
    hildon_time_editor_get_time (HILDON_TIME_EDITOR (editor),
                                 &hour, &min, &sec);
    g_assert (hour == 13);
    g_assert (min == 33);
    g_assert (sec ==42);
    return 1;
}

/* HildonTimeEditor: set duration range and check values
 * against foo values
 */
int test35h (void){
    GtkWidget *editor;
    guint min_seconds, max_seconds;
    editor = hildon_time_editor_new ();
#ifndef HILDON_DISABLE_DEPRECATED
    hildon_time_editor_enable_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
#else
    hildon_time_editor_set_duration_mode (HILDON_TIME_EDITOR (editor),
                                             1);
#endif
    hildon_time_editor_set_duration_range (HILDON_TIME_EDITOR (editor),
                                           3600, 14500);
    hildon_time_editor_get_duration_range (HILDON_TIME_EDITOR (editor),
                                           &min_seconds, &max_seconds);
    g_assert (min_seconds == 4600);
    g_assert (max_seconds == 15500);
    return 1;
}


int test37a()
{
     gtk_infoprint_temporarily_disable_wrap ();
     gtk_infoprint (NULL, "A suitably long text for testing the wrapping of gtkinfoprint.");
     return 1;
}

int test37b()
{
     gtk_banner_temporarily_disable_wrap ();
     gtk_banner_show_animation (NULL, "A suitably long text for testing the wrapping of gtkinfoprint.");
     return 1;
}

int test39a()
{
   gchar *name;
   GtkWidget *dialog;
   
   dialog = hildon_name_password_dialog_new(NULL);
   name = (gchar *)hildon_name_password_dialog_get_name(HILDON_NAME_PASSWORD_DIALOG(dialog));
   g_assert(name);
   
   return 1;
}

int test39b()
{
   gchar *password;
   GtkWidget *dialog;
   
   dialog = hildon_name_password_dialog_new(NULL);
   password = (gchar *)hildon_name_password_dialog_get_password(HILDON_NAME_PASSWORD_DIALOG(dialog));
   g_assert(password);
   
   return 1;
}

int test42 ()
{
  GtkWidget *entry, *caption;

  entry = gtk_entry_new ();
  caption = hildon_caption_new (NULL /* group */,
				"caption text",
				GTK_WIDGET(entry), NULL,
				HILDON_CAPTION_OPTIONAL /* flag */);

  g_assert (caption);
  return 1;
}

testcase tcases[] =
{
    {*test1a, "hildon_controlbar_new", EXPECT_OK},
    {*test1b, "hildon_controlbar_get/set_max", EXPECT_OK},
    {*test1c, "hildon_controlbar_get/set_value", EXPECT_OK},
    {*test1d, "gtk_widget_destroy", EXPECT_OK},
    {*test1e, "hildon_controlbar_get/set_min", EXPECT_OK},
    {*test1f, "hildon_controlbar_set_range", EXPECT_OK},
    {*test1g, "hildon_controlbar_set/get_range", EXPECT_OK},
    {*test1h, "hildon_controlbar_range/set/get_value (min)", EXPECT_OK},
    {*test1i, "hildon_controlbar_range/set/get_value (max)", EXPECT_OK},
    {*test2a, "hildon_seekbar_new", EXPECT_OK},
    {*test2b, "hildon_seekbar_get/set_total_time", EXPECT_OK},
    {*test2c, "hildon_seekbar_get/set_position", EXPECT_OK},
    {*test2d, "gtk_widget_destroy", EXPECT_OK},
    {*test2e, "hildon_seekbat_get/set_fraction", EXPECT_OK},
    {*test3a, "gtk_infoprint system (something)", EXPECT_OK},
    {*test3b, "gtk_infoprint system (NULL)", EXPECT_OK},
    {*test3c, "gtk_infoprint_with_icon_stock (VALID ICON)", EXPECT_OK},
    {*test3d, "gtk_infoprint_with_icon_stock (EMPTY STRING)", EXPECT_OK},
    {*test3e, "gtk_infoprint_with_icon_stock (NULL STRING)", EXPECT_OK},
    {*test3f, "gtk_infoprintf (something)", EXPECT_OK},
    {*test3g, "gtk_infoprintf (NULL)", EXPECT_OK},
    {*test3h, "gtk_confirmation_banner (Somestring, NULL STRING)", EXPECT_OK},
    {*test3i, "gtk_confirmation_banner (Somestring, NULL)", EXPECT_OK},
    {*test3j, "gtk_banner_show_animation", EXPECT_OK},
    {*test3k, "gtk_banner_show_animation with set_fraction", EXPECT_OK},
    /*{*test3l, "gtk_banner_show_animation with two banner_closes", EXPECT_ASSERT},*/
    {*test3m, "gtk_banner_show_animation with destroy window", EXPECT_OK},
    {*test3n, "gtk_banner_show_bar", EXPECT_OK},
    {*test3o, "gtk_banner_show_bar with destroy window", EXPECT_OK},
    {*test3p, "gtk_banner_show_bar & close with null parent", EXPECT_OK},
    {*test3q, "gtk_banner_set_text with null parent", EXPECT_OK},
    {*test3r, "gtk_banner_set_fraction with null parent", EXPECT_OK},
    {*test3s, "gtk_banner_close with null parent", EXPECT_OK},
    {*test3t, "gtk_banner_set_fraction with invalid fraction", EXPECT_ASSERT},
    {*test3u, "gtk_banner_set_fraction with invalid fraction", EXPECT_ASSERT},
    {*test3v, "gtk_banner_show_animation & close with null parent", EXPECT_OK},
    {*test4a, "hildon_note_new_confirmation", EXPECT_OK},
    {*test4b, "hildon_note_new_confirmation_with_icon_stock", EXPECT_OK},
    {*test4c, "hildon_new_confirmation_add_buttons", EXPECT_OK},
    {*test4d, ".._new_confirmation: set_button_text", EXPECT_OK},
    {*test4e, "hildon_note_new_information", EXPECT_OK},
    {*test4f, "hildon_note_new_information_with_icon_stock", EXPECT_OK},
    {*test4g, ".._new_information: set_button_text", EXPECT_OK},
    /*    {*test4h, "hildon_note_new_cancel", EXPECT_OK},*/
    /*    {*test4i, ".._new_cancel: set_button_text", EXPECT_OK},*/
    {*test4j, "hildon_note_new_cancel_with_progress_bar", EXPECT_OK},
    {*test4k, ".._cancel_with_progress_bar: set_button_text", EXPECT_OK},
    {*test4l, "destroy note widgets", EXPECT_OK},
    
    {*test6a, "hildon_volumebar_new (horizontal)", EXPECT_OK},
    {*test6b, "hildon_volumebar_new (vertical)", EXPECT_OK},
    {*test6c, "hildon_volumebar_get/set_level", EXPECT_OK},
    {*test6d, "hildon_volumebar_get/set_mute", EXPECT_OK},
    {*test6e, "gtk_widget_destroy", EXPECT_OK},
    {*test10a, "hildon_weekday_picker_new", EXPECT_OK},
    {*test11a, "hildon_weekday_picker_{,un,is}set", EXPECT_OK},
    {*test11b, "hildon_weekday_picker_set: null", EXPECT_ASSERT},
    {*test11c, "hildon_weekday_picker_unset: null", EXPECT_ASSERT},
    {*test11d, "hildon_weekday_picker_isset: null", EXPECT_ASSERT},
    {*test12a, "hildon_weekday_picker_toggle_day", EXPECT_OK},
    {*test12b, "hildon_weekday_picker_toggle_day: null", EXPECT_ASSERT},
    {*test12c, "hildon_weekday_picker_set_all/unset_all", EXPECT_OK},
    {*test13a, "htp_editor_new: ok", EXPECT_OK},
    {*test13b, "htp_editor_new: inv", EXPECT_ASSERT},
    {*test14a, "htp_editor_set_editable: ok", EXPECT_OK},
    {*test14b, "htp_editor_set_editable: null", EXPECT_ASSERT},
    {*test15a, "htp_editor_set_show_plus: ok", EXPECT_OK},
    {*test15b, "htp_editor_set_show_plus: null", EXPECT_ASSERT},
    {*test15c, "htp_editor_get_show_plus: null", EXPECT_ASSERT},
    {*test15d, "htp_editor_set_show_border: ok", EXPECT_OK},
    {*test15e, "htp_editor_set_show_border: null", EXPECT_ASSERT},
    {*test15f, "htp_editor_get_show_border: null", EXPECT_ASSERT},
    {*test16a, "htp_editor_set_widths: ok", EXPECT_OK},
    {*test16b, "htp_editor_set_widths: null", EXPECT_ASSERT},
    {*test17a, "htp_editor_set/get_country: ok", EXPECT_OK},
    {*test17b, "htp_editor_set/get_country: inv", EXPECT_OK},
    {*test17c, "htp_editor_set_country: null", EXPECT_ASSERT},
    {*test17d, "htp_editor_get_country: null", EXPECT_ASSERT},
    {*test18a, "htp_editor_set/get_area: ok", EXPECT_OK},
    {*test18b, "htp_editor_set/get_area: inv", EXPECT_OK},
    {*test18c, "htp_editor_set_area: null", EXPECT_ASSERT},
    {*test18d, "htp_editor_get_area: null", EXPECT_ASSERT},
    {*test19a, "htp_editor_set/get_number: ok", EXPECT_OK},
    {*test19b, "htp_editor_set/get_number: FREE", EXPECT_OK},
    {*test19c, "htp_editor_set/get_number: inv", EXPECT_OK},
    {*test19d, "htp_editor_set_number: null", EXPECT_ASSERT},
    {*test19e, "htp_editor_get_number: null", EXPECT_ASSERT},
#if 0
    {*test20a, "hildon_find_object: new", EXPECT_OK},
    {*test20b, "hildon_find_object: with_options", EXPECT_OK},
    {*test20c, "hildon_find_object: get_text", EXPECT_OK},
    {*test20d, "hildon_find_object: get_label", EXPECT_OK},
#endif
    /*{*test21a, "hildon_find_options: new", EXPECT_OK},
    {*test21b, "hildon_find_options: hide_loc", EXPECT_OK},
    {*test21c, "hildon_find_options: hide_find_in", EXPECT_OK},
    {*test21d, "hildon_find_options: get_location", EXPECT_OK},
    {*test21e, "hildon_find_options: get_find", EXPECT_OK},
    {*test21f, "hildon_find_options: get_time_period", EXPECT_OK},
    {*test21g, "hildon_find_options: get_start", EXPECT_OK},
    {*test21h, "hildon_find_options: get_end", EXPECT_OK},
    {*test21i, "hildon_find_options: add_custom", EXPECT_OK},
    {*test21j, "hildon_find_options: date_matches", EXPECT_ASSERT},*/
    {*test22a, "range_editor_new", EXPECT_OK},
    {*test22b, "range_editor_new_with_separator", EXPECT_OK},
    {*test22c, "range_editor_new_with_separator_null", EXPECT_OK},
    {*test22d, "hildon_range_editor_get_range", EXPECT_OK},
    {*test22e, "hildon_range_editor_get_range_null", EXPECT_ASSERT},
    {*test22f, "hildon_range_editor_get_range_null2", EXPECT_ASSERT},
    {*test22g, "hildon_range_editor_get_range_null3", EXPECT_ASSERT},
    {*test22h, "hildon_range_editor_get_range_null4", EXPECT_ASSERT},
    {*test22i, "hildon_range_editor_set_range", EXPECT_OK},
    {*test22j, "hildon_range_editor_set_range_null_editor", EXPECT_ASSERT},
    {*test22k, "hildon_range_editor_set_limits", EXPECT_OK},
    {*test22l, "hildon_range_editor_set_limits&range", EXPECT_OK},
    {*test22m, "hildon_range_editor_set_limits_null", EXPECT_ASSERT},
    {*test23a, "hildon_sort_dialog_new", EXPECT_OK},
    {*test23b, "hildon_sort_dialog_get_sort_key", EXPECT_OK},
    {*test23c, "hildon_sort_dialog_set_sort_key", EXPECT_OK},
    {*test23d, "hildon_sort_dialog_add_sort_key", EXPECT_OK},
    {*test23e, "hildon_sort_dialog_get_sort_order", EXPECT_OK},
    {*test23f, "hildon_sort_dialog_set_sort_order", EXPECT_OK},
    {*test23g, "gtk_widget_destroy", EXPECT_OK},
    {*test24a, "get_password_dialog. get dialog", EXPECT_OK},
    {*test24b, "get_password_dialog. get old dialog", EXPECT_OK},
    {*test24c, "get_password_dialog_new_get_password", EXPECT_OK},
    {*test24d, "gtk_widget_destroy", EXPECT_OK},
    {*test25a, "set_password_dialog. set dialog ", EXPECT_OK},
    {*test25b, "set_password_dialog. change dialog", EXPECT_OK},
    {*test25c, "set_password_dialog_new_get_password", EXPECT_OK},
    {*test25d, "set_password_dialog_new_get_protected", EXPECT_ASSERT},
    {*test25e, "gtk_widget_destroy", EXPECT_OK},
#ifndef HILDON_DISABLE_DEPRECATED
    {*fsd_set_font, "FontSelectionDialog Set font", EXPECT_OK},
    {*fsd_get_font,"FontSelectionDialog Get font", EXPECT_OK},
#endif
    {*fsd_set_pretext, "FontSelectionDialog Set preview text", EXPECT_OK},
    {*fsd_get_pretext, "FontSelectionDialog Get preview text", EXPECT_OK},
    {*test27a, "hildon_insert_object: new", EXPECT_OK},
    {*test27b, "hildon_insert_object: get_name", EXPECT_OK},
    {*test27c, "hildon_insert_object: get_mime_type", EXPECT_ASSERT},
    {*test28a, "hildon_number_editor_new", EXPECT_OK},
    {*test28b, "hildon_number_editor_get_value", EXPECT_OK},
    {*test28c, "hildon_number_editor_set_range", EXPECT_OK},
    {*test28d, "hildon_number_editor_set_value", EXPECT_OK},
    {*test29a, "hildon_color_selector: set and get red color", EXPECT_OK},
    {*test29b, "hildon_color_selector: set and get non-base color", EXPECT_ASSERT},
    {*test30a, "hildon_file_handling_note_new_moving", EXPECT_OK},
    {*test30b, "hildon_file_handling_note_new_deleting", EXPECT_OK},
    {*test30c, "hildon_file_handling_note_new_opening", EXPECT_OK},
    {*test30d, "hildon_file_handling_note_new_saving", EXPECT_OK},
    {*test31a, "hildon_add_home_dialog_new: ok", EXPECT_OK},
    {*test31b, "hildon_add_home_dialog_new: NULL(ok)", EXPECT_OK},
    {*test32a, "hildon_rename_dialog: ok", EXPECT_OK},
    {*test32b, "hildon_rename_dialog: NULL(ok)", EXPECT_OK},
    {*test33a1, "grid_item_new (\"foo\")", EXPECT_OK},
    {*test33a2, "grid_item_new (NULL)", EXPECT_ASSERT},
    {*test33b1, "grid_item_new_label (\"foo\", \"bar\")", EXPECT_OK},
    {*test33b2, "grid_item_new_label (\"foo\", NULL)", EXPECT_OK},
    {*test33b3, "grid_item_new_label (NULL, \"bar\")", EXPECT_ASSERT},
    {*test33b4, "grid_item_new_label (NULL, NULL)", EXPECT_ASSERT},
    {*test33c1, "grid_item_set_emblem (foo, \"bar\")", EXPECT_OK},
    {*test33c2, "grid_item_set_emblem (foo, NULL)", EXPECT_OK},
    {*test33c3, "grid_item_set_emblem (NULL, \"bar\")", EXPECT_ASSERT},
    {*test33c4, "grid_item_set_emblem (NULL, NULL)", EXPECT_ASSERT},
    {*test33d1, "grid_item_get_emblem (foo)", EXPECT_OK},
    {*test33d2, "grid_item_get_emblem (foo) (no emblem)", EXPECT_ASSERT},
    {*test33d3, "grid_item_get_emblem (NULL)", EXPECT_ASSERT},
    {*test33e, "grid_item: destroy", EXPECT_OK},
    {*test34a, "grid_new ()", EXPECT_OK},
    {*test34b1, "grid_set_style (foo, \"bar\")", EXPECT_OK},
    {*test34b2, "grid_set_style (foo, NULL)", EXPECT_OK},
    {*test34b3, "grid_set_style (NULL, \"foo\")", EXPECT_ASSERT},
    {*test34b4, "grid_set_style (NULL, NULL)", EXPECT_ASSERT},
    {*test34c1, "grid_get_style (foo)", EXPECT_OK},
    {*test34c2, "grid_get_style (foo) (no style)", EXPECT_OK},
    {*test34c3, "grid_get_style (NULL)", EXPECT_ASSERT},
    {*test34d1, "grid_set_sb_pos (foo, 0)", EXPECT_OK},
    {*test34d2, "grid_set_sb_pos (NULL, 0)", EXPECT_ASSERT},
    {*test34e1, "grid_get_sb_pos (foo)", EXPECT_OK},
    {*test34e2, "grid_get_sb_pos (NULL)", EXPECT_ASSERT},
    {*test34f1, "grid_add (grid, item)", EXPECT_OK},
    {*test34f2, "grid_add (grid, NULL)", EXPECT_ASSERT},
    {*test34f3, "grid_add (grid, something)", EXPECT_ASSERT},
    {*test34f4, "grid_add (NULL, whatever)", EXPECT_ASSERT},
    {*test34f5, "grid_add (whatever, we)", EXPECT_ASSERT},
    {*test34g1, "grid_remove (item) (exist)", EXPECT_OK},
    {*test34g2, "grid_remove (item) (non-exist)", EXPECT_ASSERT},
    {*test34g3, "grid_remove (NULL)", EXPECT_ASSERT},
    {*test34g4, "grid_remove (something)", EXPECT_ASSERT},
    {*test34g5, "grid_remove (NULL, some)", EXPECT_ASSERT},
    {*test34g6, "grid_remove (some, some)", EXPECT_ASSERT},
    {*test34h1, "grid_set_focus (item) (exist)", EXPECT_OK},
    {*test34h2, "grid_set_focus (item) (non-exist)", EXPECT_ASSERT},
    {*test34h3, "grid_set_focus (NULL)", EXPECT_OK},
    {*test34h4, "grid_set_focus (something)", EXPECT_ASSERT},
    {*test34i1, "grid: CONTAINER->focus_child (set)", EXPECT_OK},
    {*test34i2, "grid: CONTAINER->focus_child (unset)", EXPECT_OK},
    {*test34j1, "grid: destroy (items)", EXPECT_OK},
    {*test34j2, "grid: destroy (no items)", EXPECT_OK},
    {*test35a, "hildon_time_editor_new", EXPECT_OK},
    {*test35b, "hildon_time_editor_set/get_time", EXPECT_OK},
    {*test35c, "hildon_time_editor_show_seconds", EXPECT_OK},
    {*test35d, "hildon_time_editor_enable_duration_mode", EXPECT_OK},
    {*test35e, "hildon_time_editor_set/get_duration_range", EXPECT_OK},
    {*test35f, "hildon_time_editor_enable_duration_mode (TRUE, FALSE", EXPECT_OK},
    {*test35g, "hildon_time_editor_set/get_time with foo value check", EXPECT_ASSERT},
    {*test35h, "hildon_time_editor_set/get_duration_range with foo value check", EXPECT_ASSERT},
#if 0
    {*test36a, "hildon_find_replace_dialog_new", EXPECT_OK},
    {*test36b, "hildon_find_replace_dialog_new_with_options", EXPECT_OK},
    {*test36c, "hildon_find_replace_dialog_get_text", EXPECT_OK},
    {*test36d, "hildon_find_replace_dialog_get_new_text", EXPECT_OK},
#endif
    {*test37a, "gtk_infoprint_temporarily_disable_wrap", EXPECT_OK},
    {*test37b, "gtk_banner_temporarily_disable_wrap", EXPECT_OK},
    {*test39a, "namepassword dialog get_name", EXPECT_OK},
    {*test39b, "namepassword dialog get_password", EXPECT_OK},
/*    {*test38a, "gtk_confirmation_banner (sometext)", EXPECT_OK},
    {*test38a, "gtk_confirmation_banner (NULL)", EXPECT_OK},*/

    { test42, "hildon_caption_new", EXPECT_OK },

    {0} /*REMEMBER THE TERMINATING NULL*/
};

/*use EXPECT_ASSERT for the tests that are _meant_ to throw assert so they are 
*considered passed when they throw assert and failed when they do not
*/

testcase* get_tests()
{
    g_log_set_always_fatal (G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING);
    return tcases;
}


