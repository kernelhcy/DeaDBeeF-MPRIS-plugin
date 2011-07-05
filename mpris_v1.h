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
