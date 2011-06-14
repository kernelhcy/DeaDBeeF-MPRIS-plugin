#ifndef __MPRIS_SERVER_H_
#define __MPRIS_SERVER_H_
#include <glib-2.0/glib.h>

#define MPRIS_SERVICE "org.mpris.deadbeef"
#define MPRIS_ROOT_PATH "/"
#define MPRIS_PLAYER_PATH "/Player"
#define MPRIS_TRACKLIST_PATH "/TrackList"

#define MPRIS__DEBUG 1

#ifndef MPRIS__DEBUG
	#define debug(...)
#else
	#define debug(...)  do_debug(__VA_ARGS__)
#endif

void do_debug(const char *fmt, ...);

/*
 * The server struct.
 */
typedef struct _DB_mpris_server DB_mpris_server;

/*
 * return status.
 */
enum{
	DB_MPRIS_OK,
	DB_MPRIS_ERROR
};

/*
 * Start and stop the MPRIS server.
 * Usage:
 * 		db_mpris_server *srv;
 * 		db_mpris_server_start(&srv);
 * 		...
 * 		db_mpris_server_stop(srv);
 * 		srv = NULL;
 */
gint DB_mpris_server_start(DB_mpris_server **srv);
gint DB_mpris_server_stop(DB_mpris_server *srv);

#endif
