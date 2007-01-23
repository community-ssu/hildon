/*
 * This file is a part of hildon tests
 *
 * Copyright (C) 2006, 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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

/*

 gcc `pkg-config --cflags gtk+-2.0 hildon-libs` `pkg-config --libs gtk+-2.0 hildon-libs`  -o tc_banner_truncate tc_banner_truncate.c && DISPLAY=:2 run-standalone.sh  ./tc_banner_truncate 1

 Usage 1: tc_banner_truncate <case #>
 Usage 2: tc_banner_truncate <1|2|3> the string you like to be displayed
                              1 -> information banner
                              2 -> progress banner
                              3 -> animation banner
*/

#include <stdio.h>
#include <gtk/gtk.h>
#include <hildon-widgets/hildon-banner.h>

#define WAIT g_timeout_add(3000, (GSourceFunc)(gtk_main_quit), NULL);

#define TC_INFO(string) hildon_banner_show_information(window,  NULL, string); WAIT;
#define TC_ANIM(string) banner = hildon_banner_show_animation(NULL, NULL, string);
#define TC_PROG(string) banner = hildon_banner_show_progress (NULL, NULL, string);

static void get_argv_string(int argc, char ** argv, char *text);

int main(int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *banner;
  int n;
  char text[10000];

  if(argc <  2){ fprintf(stderr, "Add case # as parameter.\n");  return 1;}
  if(argc >= 2){ if(sscanf(argv[1], "%d", &n) == 0) n = 1;}
  if(argc >  2){ get_argv_string(argc, argv, text);}

  gtk_init(&argc, &argv);
  
  window = gtk_dialog_new();
  
  if(argc > 2){
    /* text is given in the command line */
    switch (n){
    case 1: TC_INFO(text); break;
    case 2: TC_PROG(text); break;
    case 3: TC_ANIM(text); break;
    default:TC_INFO(text); break;
    }
    n = -99999;
  }

  switch (n) {
  /* NOTE: comments (*) are given for 0.12.12, with default theme (orange)
   *       (*) "truncated", WRAPPED", etc.
   */

  case 0: TC_INFO(""); break;
  case 1: TC_INFO("loading..."); break;

  case 11: TC_INFO("mmmmmmmmmmmmmmmmmm m.."       ); break; //1 line
  case 12: TC_INFO("mmmmmmmmmmmmmmmmmm m..."      ); break; //truncated
  case 13: TC_INFO("mmmmmmmmmmmmmmmmmm m...."     ); break; //truncated
  case 14: TC_INFO("mmmmmmmmmmmmmmmmmm m....."    ); break; //truncated
  case 15: TC_INFO("mmmmmmmmmmmmmmmmmm m......"   ); break; //truncated
  case 16: TC_INFO("mmmmmmmmmmmmmmmmmm m......."  ); break; //truncated
  case 17: TC_INFO("mmmmmmmmmmmmmmmmmm m........" ); break; //truncated
  case 18: TC_INFO("mmmmmmmmmmmmmmmmmm m........."); break; //WRAPPED

  case 21: TC_INFO("maximum number of characters xxxxxxxx");  break; //WRAPPED
  case 22: TC_INFO("maximum number of characters xxxxxxxi");  break; //truncated

  case 23: TC_INFO("Maximum number of characters reache"   ); break; //1 line
  case 24: TC_INFO("Maximum number of characters reached"  ); break; //truncated
  case 25: TC_INFO("Maximum number of characters reached." ); break; //truncated
  case 26: TC_INFO("Maximum number of characters reached " ); break; //truncated
  case 27: TC_INFO("Maximum number of characters reached a"); break; //WRAPPED

  /* progress banners */
  case 101: TC_PROG(""               ); break;
  case 102: TC_PROG("hello"          ); break;
  case 103: TC_PROG("hello progress" ); break;

  /* Bugzilla related TC */
  case -1: case 25305: TC_INFO("Maximum number of characters reached");      break;
  case -2: case 23430: TC_INFO("Unable to manage folders during searching"); break; //PASSED
  case -3: case 26624: TC_ANIM("Connection Manager - Loading");              break;
  case -4: case 26161: TC_ANIM("Painel de Controle - carregando");           break;
  case -5: case 26203: TC_ANIM("Caricamento di Pannello di controllo");      break;
  case -6: case 27875: TC_ANIM("Instant messaging - loading");               break;
  case -7: case 28760: TC_ANIM("Teach Handwriting- Loading");                break;

  /* word longer than max width */
  case 201: TC_INFO("mmmmmmmmmmmmmmmmmmmmmmmmmmmmm 1 2 3 4");  break;

  /* misc */
  case 202: TC_INFO("Maximum number of characters reached osaidfh galds hgdslf helius grkdsz vfliesdfrjaesfrszri os o ozx oyli l ilij ero worjw aeowaijf osijf a jfaowijef aoweijf s faoij flsaijf sajfd oaijfe aosi e 1 2 3 4"); break; /* TRUNCATED after last 'e' */


  /* TC for plain GTK (no theme) */
  case -1031: TC_INFO("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiil i."); break; //1 line
  case -1032: TC_INFO("iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiil i."); break; //WRAPPED

  case -1033: TC_INFO("mmmmmmmmmmmmmmmmmm m................................................i" ); break; //1 line
  case -1034: TC_INFO("mmmmmmmmmmmmmmmmmm m.................................................i"); break; //WRAPPED


  default:
    WAIT;
  }

  gtk_main();
  return 0;
}








/* ############################################################################## */
/* ############################################################################## */
/* ############################################################################## */

static void get_argv_string(int argc, char ** argv, char *text){
  int i,j;
  char * s;
  s = text;
  for(i=2; i<argc; i++){
    for(j=0; argv[i][j] != '\0'; j++){
      *s = argv[i][j];
      s++;
    }
    if(i<(argc-1)){
      *s=' ';
      s++;
    }
  }
  /* hem */
}
