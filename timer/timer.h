/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
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

#ifndef HILDON_USE_TIMESTAMPING

#define TIMER_START( filename )
#define TIMER_STOP()
#define TIMER_STOP_AND_EXIT()
#define TIMESTAMP( message )

#else

#ifndef __TIMER_H__
#define __TIMER_H__

#define TIMER_START( filename ) timer_start( filename );
#define TIMER_STOP() timer_stop()
#define TIMER_STOP_AND_EXIT() timer_stop(); return 0;
#define TIMESTAMP( message ) print_timestamp( message );

void timer_start( char * );
void timer_stop( void );
void print_timestamp( char * );

#endif /* __TIMER_H__ */

#endif /* HILDON_USE_TIMESTAMPING */
