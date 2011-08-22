/*
    MPRIS plugin for Deadbeef player
    Copyright (C) 2011 HuangCongyu <huangcongyu2006@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef __MPRIS_SERVER_V1_H_
#define __MPRIS_SERVER_V1_H_
#include <glib.h>
#include <deadbeef/deadbeef.h>

/*
 * The deadbeef apis
 */
extern DB_functions_t *deadbeef;

/*
 * The server struct.
 */
typedef struct _DB_mpris_server_v1 DB_mpris_server_v1;

/*
 * Start and stop the MPRIS server.
 * Usage:
 * 		db_mpris_server *srv;
 * 		db_mpris_server_start_v1(&srv);
 * 		...
 * 		db_mpris_server_stop_v1(srv);
 * 		srv = NULL;
 */
gint DB_mpris_server_start_v1(DB_mpris_server_v1 **srv);
gint DB_mpris_server_stop_v1(DB_mpris_server_v1 *srv);

/*
 * emit the signal
 */
void DB_mpris_emit_trackchange_v1();
void DB_mpris_emit_statuschange_v1();
void DB_mpris_emit_capschange_v1();

void DB_mpris_emit_tracklistchange_v1();
#endif
