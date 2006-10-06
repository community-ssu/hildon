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

#ifdef HILDON_USE_TIMESTAMPING

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "timer.h"


static struct timeval _timer_start;
FILE *timer_logfile;

void timer_start( char *filename )
{
	timer_logfile = fopen( filename, "w" );

	gettimeofday( &_timer_start, NULL );

	return;
}

void timer_stop(void)
{
	fclose( timer_logfile );

	return;
}

void print_timestamp( char *info ) {

	struct timeval _timer_end;
	double t1, t2, t3;

	gettimeofday( &_timer_end, NULL );

	t1 =  (double)_timer_start.tv_sec + (double)_timer_start.tv_usec/(1000*1000);
	t2 =  (double)_timer_end.tv_sec + (double)_timer_end.tv_usec/(1000*1000);
	
	t3 = t2 - t1;
	
	fprintf( timer_logfile, "%4.8f %s\n", t3, info );

	return;
}

#endif /* HILDON_USE_TIMESTAMPING */
