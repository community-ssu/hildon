/*
 * Copyright (C) 2006 Nokia Corporation.
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

#include <stdlib.h>
#include <check.h>
#include <gtk/gtkmain.h>
#include <gconf/gconf-client.h>
#include "test_suites.h"
#include "hildon-system-sound.h"

/* This define was copied from the hildon-system-sound.c */
#define ALARM_GCONF_PATH "/apps/osso/sound/system_alert_volume"
#define DEFAULT_BEEP "/usr/share/sounds/ui-default_beep.wav"

/* -------------------- Fixtures -------------------- */

static void
fx_setup_default_defines ()
{
  int argc = 0;

  gtk_init(&argc, NULL);
}

static void
fx_teardown_default_defines ()
{
}
/* -------------------- Test cases -------------------- */

/* ----- Test case for hildon_play_system_sound -----*/

/**
 * Purpose: test playing a system sound
 * Cases considered:
 *    - Play /usr/share/sounds/ui-default_beep.wav with default volume
 *    - Play /usr/share/sounds/ui-default_beep.wav with volume = 0
 *    - Play /usr/share/sounds/ui-default_beep.wav with volume = 1
 *    - Play /usr/share/sounds/ui-default_beep.wav with the value given by gconf
 */
START_TEST (test_hildon_play_system_sound_regular)
{
  GConfClient *client = NULL;
  GError *error = NULL;

  /* Get a gconf client */
  client = gconf_client_get_default();
  fail_if (client == NULL, 
	   "hildon-system-sound: Could not get a GConf client");

  /* Play a system sound */
  hildon_play_system_sound (DEFAULT_BEEP);

  /* Mute the volume and then play */
  gconf_client_set_int (client, ALARM_GCONF_PATH, 0, &error);
  hildon_play_system_sound (DEFAULT_BEEP);

  /* Set the volume to 1 */
  gconf_client_set_int (client, ALARM_GCONF_PATH, 1, &error);
  hildon_play_system_sound (DEFAULT_BEEP);

  /* Unset the value of the key */
  gconf_client_unset (client, ALARM_GCONF_PATH, &error);
  hildon_play_system_sound (DEFAULT_BEEP);
}
END_TEST

/**
 * Purpose: test playing a sound with invalid parameters
 * Cases considered:
 *    - Play a NULL path
 *    - Play a file that does not exist
 */
START_TEST (test_hildon_play_system_sound_invalid)
{
  GConfClient *client = NULL;

  /* Get a gconf client */
  client = gconf_client_get_default();
  fail_if (client == NULL, 
	   "hildon-system-sound: Could not get a GConf client");

  /* Test 1 */
  hildon_play_system_sound (NULL);

  /* Test 2 */
  hildon_play_system_sound ("file_that_does_not_exist.wav");
}
END_TEST

/* ---------- Suite creation ---------- */

Suite *create_hildon_system_sound_suite()
{
  /* Create the suite */
  Suite *s = suite_create("HildonSystemSound");

  /* Create test cases and add them to the suite */
  TCase *tc1 = tcase_create("hildon_play_system_sound");

  tcase_add_checked_fixture(tc1, fx_setup_default_defines, fx_teardown_default_defines);
  tcase_add_test(tc1, test_hildon_play_system_sound_regular);
  tcase_add_test(tc1, test_hildon_play_system_sound_invalid);
  suite_add_tcase (s, tc1);

  /* Return created suite */
  return s;
}
