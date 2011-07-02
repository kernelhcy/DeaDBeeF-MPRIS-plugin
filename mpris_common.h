#ifndef __MPRIS_COMMON_H__
#define __MPRIS_COMMON_H__
#include <glib.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <deadbeef/deadbeef.h>
#define MPRIS__DEBUG 1

#ifndef MPRIS__DEBUG
	#define debug(...)
#else
	#define debug(...)  do_debug(__VA_ARGS__)
#endif

#define MPRIS_V1_INTERFACE          "org.freedesktop.MediaPlayer"
#define MPRIS_V1_SERVICE 			"org.mpris.deadbeef"
#define MPRIS_V1_ROOT_PATH 		    "/"
#define MPRIS_V1_PLAYER_PATH 		"/Player"
#define MPRIS_V1_TRACKLIST_PATH 	"/TrackList"

//Method names
#define MPRIS_METHOD_IDENTITY 			"Identity"
#define MPRIS_METHOD_QUIT 				"Quit"
#define MPRIS_METHOD_VERSION			"MprisVersion"
#define MPRIS_METHOD_GETMETA			"GetMetadata"
#define MPRIS_METHOD_GETCURRENTTRACK	"GetCurrentTrack"
#define MPRIS_METHOD_GETLENGTH			"GetLength"
#define MPRIS_METHOD_ADDTRACK			"AddTrack"
#define MPRIS_METHOD_DELTRACK			"DelTrack"
#define MPRIS_METHOD_SETLOOP			"SetLoop"
#define MPRIS_METHOD_SETRANDOM			"SetRandom"
#define MPRIS_METHOD_NEXT				"Next"
#define MPRIS_METHOD_PREV				"Prev"
#define MPRIS_METHOD_PAUSE				"Pause"
#define MPRIS_METHOD_STOP				"Stop"
#define MPRIS_METHOD_PLAY				"Play"
#define MPRIS_METHOD_REPEAT				"Repeat"
#define MPRIS_METHOD_GETSTATUS			"GetStatus"
#define MPRIS_METHOD_GETCAPS			"GetCaps"
#define MPRIS_METHOD_VOLUMESET			"VolumeSet"
#define MPRIS_METHOD_VOLUMEGET			"VolumeGet"
#define MPRIS_METHOD_POSITIONSET		"PositionSet"
#define MPRIS_METHOD_POSITIONGET		"PositionGet"

//Signal names
#define MPRIS_SIGNAL_TRACKCHANGE		"TrackChange"
#define MPRIS_SIGNAL_STATUSCHANGE		"StatusChange"
#define MPRIS_SIGNAL_CAPSCHANGE			"CapsChange"
#define MPRIS_SIGNAL_TRACKLISTCHAGE		"TrackListChange"

/*
 * The deadbeef apis
 */
extern DB_functions_t *deadbeef;

void do_debug(const char *fmt, ...);

/*
 * Signal emit parameter strut.
 */
typedef struct
{
    GDBusConnection *con;
    gpointer data;
    const gchar *interface;
    const gchar *signal_name;
    const gchar *object_path;
}SignalPar;

/*
 * Emit a signal of signal_name with data
 */
void emit_signal(GDBusConnection *con, const gchar *interface, const gchar *obj
                , const gchar * signal_name, gpointer data);

/*
 * Get the status of the player.
 */
GVariant *get_status();

/*
 * Set the loop status.
 * @param value 
 *          "None" no loop
 *          "Playlist" loop the play list
 *          "Track" loop the current track
 */
void set_loop_status(GVariant *value);

/*
 * Get the meta data of the playing track
 */
GVariant* get_metadata(int track_id);
#endif
