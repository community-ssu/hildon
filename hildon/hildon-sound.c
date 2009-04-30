/*
 * This file is a part of libhildon
 *
 * Copyright (C) 2005-2008 Nokia Corporation. All rights reserved.
 *
 * Contact: Kimmo Hämäläinen <kimmo.hamalainen@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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

/**
 * SECTION:hildon-sound
 * @short_description: libcanberra-based utility function for playing a sound.
 * 
 */

#include <unistd.h>
#include <gconf/gconf-client.h>
#include <canberra.h>

#include "hildon-sound.h"

#define ALARM_GCONF_PATH "/apps/osso/sound/system_alert_volume"

static ca_context *hildon_ca_context_get (void);

/*
 * hildon_ca_context_get:
 *
 * hildon maintains a single application-global ca_context object.
 *
 * This functions is based on ca_gtk_context_get
 *
 * Returns: a ca_context object
 */
static ca_context *
hildon_ca_context_get (void)
{
    static GStaticPrivate context_private = G_STATIC_PRIVATE_INIT;
    ca_context *c = NULL;
    const gchar *name = NULL;
    gint ret;

    if ((c = g_static_private_get(&context_private)))
        return c;

    if ((ret = ca_context_create(&c)) != CA_SUCCESS) {
        g_warning("ca_context_create: %s\n", ca_strerror(ret));
        return NULL;
    }
    if ((ret = ca_context_open(c)) != CA_SUCCESS) {
        g_warning("ca_context_open: %s\n", ca_strerror(ret));
        ca_context_destroy(c);
        return NULL;
    }

    if ((name = g_get_application_name()))
        ca_context_change_props(c, CA_PROP_APPLICATION_NAME, name, NULL);

    g_static_private_set(&context_private, c, (GDestroyNotify) ca_context_destroy);

    return c;
}

/**
 * hildon_play_system_sound:
 * @sample: sound file to play
 * 
 * Plays the given sample using libcanberra.
 * Volume level is received from gconf. 
 */
void 
hildon_play_system_sound(const gchar *sample)
{
    float volume = 0;
    int ret;
    ca_context *ca_con = NULL;
    ca_proplist *pl = NULL;

#if 0 /* FIXME: Check volume handling. Would be great not to use Gconf... */
    GConfClient *client;
    GConfValue *value;
    gint gconf_vol;

    /*
     * The volume is from -0dB to -6dB,
       The full volume is marked as 2 in gconf */
    client = gconf_client_get_default ();
    value = gconf_client_get (client, ALARM_GCONF_PATH, NULL);

    /* We want error cases to match full volume, not silence, so
       we do not want to use gconf_client_get_int */
    if (!value || value->type != GCONF_VALUE_INT)
        gconf_vol = 2;
    else
        gconf_vol = gconf_value_get_int(value);

    if (value)
        gconf_value_free(value);
    g_object_unref (client);

    volume = ((1.0 - (float)gconf_vol / 2.0)) * (-6.0);
#endif

    ca_con = hildon_ca_context_get ();

    ca_proplist_create(&pl);
    ca_proplist_sets(pl, CA_PROP_MEDIA_FILENAME, sample);
    ca_proplist_setf(pl, CA_PROP_CANBERRA_VOLUME, "%f", volume);

    ret = ca_context_play_full(ca_con, 0, pl, NULL, NULL);
    g_debug("ca_context_play_full (vol %f): %s\n", volume, ca_strerror(ret));

    ca_proplist_destroy(pl);
}
