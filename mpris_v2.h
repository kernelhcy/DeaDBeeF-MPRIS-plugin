#ifndef __MPRIS_SERVER_H_
#define __MPRIS_SERVER_H_
#include <glib.h>
#include <deadbeef/deadbeef.h>

/*
 * The deadbeef apis
 */
extern DB_functions_t *deadbeef;

/*
 * The server struct.
 */
typedef struct _DB_mpris_server_v2 DB_mpris_server_v2;

/*
 * Start and stop the MPRIS server.
 * Usage:
 * 		db_mpris_server *srv;
 * 		db_mpris_server_start_v2(&srv);
 * 		...
 * 		db_mpris_server_stop_v2(srv);
 * 		srv = NULL;
 */
gint DB_mpris_server_start_v2(DB_mpris_server_v2 **srv);
gint DB_mpris_server_stop_v2(DB_mpris_server_v2 *srv);

/*
 * emit the signal
 */
void DB_mpris_emit_trackchange_v2();
void DB_mpris_emit_statuschange_v2();
void DB_mpris_emit_capschange_v2();

void DB_mpris_emit_tracklistchange_v2();
#endif
