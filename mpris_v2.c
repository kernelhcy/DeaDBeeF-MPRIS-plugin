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
#include "mpris_v2.h"
#include "mpris_common.h"
#include "introspection_xml.h"

#include <gio/gio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>

struct _DB_mpris_server_v2
{
    GDBusConnection *con;

    GDBusNodeInfo *introspection_data_root;
    GDBusNodeInfo *introspection_data_player;
    GDBusNodeInfo *introspection_data_tracklist;
    GDBusNodeInfo *introspection_data_playlists;

    guint owner_id;

    guint root_reg_id;
    guint player_reg_id;
    guint tracklist_reg_id;
    guint playlists_reg_id;
};

/*
 * Information. 
 */
static DB_mpris_server_v2 *server = NULL;

////////////////////////////  / /////////////////////////////////////////
/*
 * Handle the / object method call
 */
static void handle_root_method_call(GDBusConnection *connection,
                                    const gchar *sender,
                                    const gchar *object_path,
                                    const gchar *interface_name,
                                    const gchar *method_name,
                                    GVariant *parameters,
                                    GDBusMethodInvocation *invocation,
                                    gpointer user_data)
{
    debug("/ method: %s", method_name);

    //Quit
    if(g_strcmp0(method_name, MPRIS_METHOD_QUIT) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        debug("Quit...");
        /*
         * Quit the media player
         */
        deadbeef -> sendmessage(DB_EV_TERMINATE, 0, 0, 0);
        return;
    }

    //Raise
    if(g_strcmp0(method_name, MPRIS_METHOD_RAISE) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        return;
    }

    debug("Error! Unsupported method. %s.%s", interface_name, method_name);
    g_dbus_method_invocation_return_error(invocation
                                    , G_DBUS_ERROR
                                    , G_DBUS_ERROR_NOT_SUPPORTED
                                    , "Method %s.%s not supported"
                                    , interface_name
                                    , method_name);
    return;
}

/*
 * Handle root get property
 */
static GVariant *handle_root_get_property(GDBusConnection *connection,
                                        const gchar *sender,
                                        const gchar *object_path,
                                        const gchar *interface_name,
                                        const gchar *property_name,
                                        GError **error,
                                        gpointer user_data)
{
    GVariant *ret = NULL;
    if(g_strcmp0(property_name, "CanQuit") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "HasTrackList") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "CanRaise") == 0){
        ret = g_variant_new_boolean(FALSE);
    }else if(g_strcmp0(property_name, "Identity") == 0){
        ret = g_variant_new_string("DeaDBeeF");
    }else if(g_strcmp0(property_name, "DesktopEntry") == 0){
        ret = g_variant_new_string("deadbeef");
    }else if(g_strcmp0(property_name, "SupportedUriSchemes") == 0){
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
        /*
         * I don't know where to find the supported uri schemes.
         * These two are found from the gui.
         */
        g_variant_builder_add(builder, "s", "file");
        g_variant_builder_add(builder, "s", "http");
        ret = g_variant_builder_end(builder);
        g_variant_builder_unref(builder);
    }else if(g_strcmp0(property_name, "SupportedMimeTypes") == 0){
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("as"));
        /*
         * I also don't know where to find the supported mime types.
         */
        g_variant_builder_add(builder, "s", "application/ogg");
        g_variant_builder_add(builder, "s", "audio/x-vorbis+ogg");
        g_variant_builder_add(builder, "s", "audio/x-flac");
        g_variant_builder_add(builder, "s", "audio/mpeg");
        ret = g_variant_builder_end(builder);
    }


    return ret;
}

/* root vtable */
static const GDBusInterfaceVTable root_vtable =
{
    handle_root_method_call,
    handle_root_get_property,
    NULL
};

////////////////////////////  /Player /////////////////////////////////////////
/*
 * Handle /Player method call
 */
static void handle_player_method_call(GDBusConnection *connection,
                                    const gchar *sender,
                                    const gchar *object_path,
                                    const gchar *interface_name,
                                    const gchar *method_name,
                                    GVariant *parameters,
                                    GDBusMethodInvocation *invocation,
                                    gpointer user_data)
{
    //Next
    if(g_strcmp0(method_name, MPRIS_METHOD_NEXT) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef -> sendmessage(DB_EV_NEXT, 0, 0, 0);
        goto go_return;
    }
    
    //Previous
    if(g_strcmp0(method_name, MPRIS_METHOD_PREVIOUS) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef -> sendmessage(DB_EV_PREV, 0, 0, 0);
        goto go_return;
    }
    
    //Play
    if(g_strcmp0(method_name, MPRIS_METHOD_PLAY) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef -> sendmessage(DB_EV_PLAY_CURRENT, 0, 0, 0);
        goto go_return;
    }
    
    //PlayPause
    if(g_strcmp0(method_name, MPRIS_METHOD_PLAYPAUSE) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef -> sendmessage(DB_EV_TOGGLE_PAUSE, 0, 0, 0);
        goto go_return;
    }
    
    //Stop
    if(g_strcmp0(method_name, MPRIS_METHOD_STOP) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef -> sendmessage(DB_EV_STOP, 0, 0, 0);
        goto go_return;
    }
    
    //Pause
    if(g_strcmp0(method_name, MPRIS_METHOD_PAUSE) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef->sendmessage(DB_EV_PAUSE, 0, 0, 0);
        goto go_return;
    }
    
    //SetPosition
    if(g_strcmp0(method_name, MPRIS_METHOD_SETPOSITION) == 0){
        gint64 pos = 0;
        const gchar *trackid = NULL;
        g_variant_get(parameters, "(&ox)", &trackid, &pos);
        debug("Set %s position %d.", trackid, pos);
        
        DB_playItem_t *track = deadbeef -> streamer_get_playing_track();
        if(track != NULL){
            ddb_playlist_t *pl = deadbeef -> plt_get_curr();
            gint playid = deadbeef -> plt_get_item_idx(pl, track, PL_MAIN);
            gchar buf[200];
            g_sprintf(buf, "/org/mpris/MediaPlayer2/Track/track%d", playid);
            if(g_strcmp0(buf, trackid) == 0){
                deadbeef -> sendmessage(DB_EV_SEEK, 0, pos, 0);
            }
            deadbeef -> pl_item_unref(track);
            deadbeef -> plt_unref(pl);
        }
        
        g_dbus_method_invocation_return_value(invocation, NULL);
        goto go_return;
    }

    //OpenUri
    if(g_strcmp0(method_name, MPRIS_METHOD_OPENURI) == 0){
        const gchar *uri = NULL;
        g_variant_get(parameters, "(&s)", &uri);
        debug("OpenUri: %s\n", uri);
        ddb_playlist_t *pl = deadbeef -> plt_get_curr();
        int ret = deadbeef -> plt_add_file(pl, uri, NULL, NULL);
        if(ret == 0){
            //play it.
            ddb_playlist_t *pl = deadbeef -> plt_get_curr();
            DB_playItem_t *track = deadbeef -> plt_get_last(pl, PL_MAIN);
            int track_id = deadbeef -> plt_get_item_idx(pl, track, PL_MAIN);
            deadbeef -> plt_unref(pl);
            deadbeef -> pl_item_unref(track);
            deadbeef -> sendmessage(DB_EV_PLAY_NUM, 0, track_id, 0);
        }
        g_dbus_method_invocation_return_value(invocation, NULL);
        goto go_return;
    }

    debug("Error! Unsupported method. %s.%s", interface_name, method_name);
    g_dbus_method_invocation_return_error(invocation
                                    , G_DBUS_ERROR
                                    , G_DBUS_ERROR_NOT_SUPPORTED
                                    , "Method %s.%s not supported"
                                    , interface_name
                                    , method_name);
go_return: 
    return;
}

/*
 * Handle player get property
 */
static GVariant *handle_player_get_property(GDBusConnection *connection,
                                        const gchar *sender,
                                        const gchar *object_path,
                                        const gchar *interface_name,
                                        const gchar *property_name,
                                        GError **error,
                                        gpointer user_data)
{
    GVariant *ret = NULL;
    if(g_strcmp0(property_name, "PlaybackStatus") == 0){
        DB_output_t *output = deadbeef -> get_output();
        /*
         * issue 2
         * I don't know why get_output will return NULL on his computer.
         * I can not reproduce this bug...
         */
        if(output == NULL){
            return g_variant_new_string("Stopped");
        }
        switch(output -> state())
        {
        case OUTPUT_STATE_PLAYING:
            ret = g_variant_new_string("Playing");
            break;
        case OUTPUT_STATE_PAUSED:
            ret = g_variant_new_string("Paused");
            break;
        case OUTPUT_STATE_STOPPED:
            ret = g_variant_new_string("Stopped");
            break;
        default:
            ret = g_variant_new_string("UnknonwPlaybackStatus");
            break;
        }
    }else if(g_strcmp0(property_name, "LoopStatus") == 0){
        int loop = deadbeef -> conf_get_int("playback.loop", 0);
        switch(loop)
        {
        case PLAYBACK_MODE_NOLOOP:
            ret = g_variant_new_string("None");
            break;
        case PLAYBACK_MODE_LOOP_ALL:
            ret = g_variant_new_string("Playlist");
            break;
        case PLAYBACK_MODE_LOOP_SINGLE:
            ret = g_variant_new_string("Track");
            break;
        default:
            ret = g_variant_new_string("UnknownLoopStatus");
            break;
        }    

    }else if(g_strcmp0(property_name, "Rate") == 0){
        ret = g_variant_new("d", 1.0);
    }else if(g_strcmp0(property_name, "Shuffle") == 0){
        int order = deadbeef -> conf_get_int("playback.order", 0);
        if(order == PLAYBACK_ORDER_LINEAR){
            ret = g_variant_new_boolean(FALSE);
        }else if(order == PLAYBACK_ORDER_RANDOM){
            ret = g_variant_new_boolean(TRUE);
        }   
    }else if(g_strcmp0(property_name, "Metadata") == 0){
        ret = get_metadata_v2(-1);
    }else if(g_strcmp0(property_name, "Volume") == 0){
        float min_vol = deadbeef -> volume_get_min_db();
        float volume = deadbeef -> volume_get_db() - min_vol;
        debug("Get Volume: %f", volume);
        ret =  g_variant_new("(d)" , volume / ( - min_vol) * 100);
    }else if(g_strcmp0(property_name, "Position") == 0){
        DB_playItem_t *track = NULL;
        track = deadbeef -> streamer_get_playing_track();
        if(track == NULL){
            ret =  g_variant_new("(x)", 0);    
        }else{
            float duration = deadbeef -> pl_get_item_duration(track);
            float pos = deadbeef -> playback_get_pos(); 
            ret =  g_variant_new("(x)", (gint64)(pos * duration * 10));    
            deadbeef -> pl_item_unref(track);
        }
    }else if(g_strcmp0(property_name, "MinimumRate") == 0){
        ret = g_variant_new("d", 1.0);
    }else if(g_strcmp0(property_name, "MaximumRate") == 0){
        ret = g_variant_new("d", 1.0);
    }else if(g_strcmp0(property_name, "CanGoNext") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "CanGoPrevious") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "CanPlay") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "CanPause") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "CanSeek") == 0){
        ret = g_variant_new_boolean(TRUE);
    }else if(g_strcmp0(property_name, "CanControl") == 0){
        ret = g_variant_new_boolean(TRUE);
    }

    return ret;
}

static gboolean handle_player_set_property(GDBusConnection  *connection,
                                        const gchar *sender,
                                        const gchar *object_path,
                                        const gchar *interface_name,
                                        const gchar *property_name,
                                        GVariant *value,
                                        GError **error,
                                        gpointer user_data)
{
    if(g_strcmp0(property_name, "LoopStatus") == 0){
        set_loop_status(value);
    }else if(g_strcmp0(property_name, "Rate") == 0){
        /*
         * Not supported!
         */
        debug("Not supported Rate!!\n");
        return TRUE;
    }else if(g_strcmp0(property_name, "Shuffle") == 0){
        gboolean random;
        g_variant_get(value, "b", &random);
        if(random == FALSE){
            deadbeef -> conf_set_int("playback.order", PLAYBACK_ORDER_LINEAR);
        }else{
            deadbeef -> conf_set_int("playback.order", PLAYBACK_ORDER_RANDOM);
        }
        deadbeef -> sendmessage(DB_EV_CONFIGCHANGED, 0, 0, 0);
    }else if(g_strcmp0(property_name, "Volume") == 0){
        double volume = 0;
        g_variant_get(value, "d", &volume);
        if(volume > 100.0){
            volume = 100.0;
        }
        float vol_f = 50 - ((float)volume / 100.0 * 50.0);
        debug("Set Volume: %f %f", volume, vol_f);
        deadbeef -> volume_set_db(-vol_f);
    }
    /*
     * Emit the org.freedesktop.DBus.Properties.PropertiesChanged signal
     */
    GVariant *sigpar = g_variant_new("(v)", value);
    debug("Emit PropertiesChanges signal\n");
    emit_signal(server -> con, "org.freedesktop.DBus.Properties", MPRIS_V2_PATH
                    , "PropertiesChanged", sigpar);
    return TRUE;
}

void DB_mpris_emit_seeked_v2(gint64 seeked)
{
    debug("emit status seeked signl. %d", seeked);
    GVariant *ret = g_variant_new("(x)", seeked);
    emit_signal(server -> con, MPRIS_V2_INTERFACE_PLAYER, MPRIS_V2_PATH
                    , MPRIS_SIGNAL_SEEKED, ret);
}

/* player vtable */
static const GDBusInterfaceVTable player_vtable =
{
    handle_player_method_call,
    handle_player_get_property,
    handle_player_set_property
};
#if 0
/*
 * The callback of plt_add_file
 */
static int add_file_cb(DB_playItem_t *it, void *data) {
    gboolean play_imm = (gboolean)data;
    if (it != NULL) {
        debug("add_file_cb %d\n", play_imm);
        if (play_imm == TRUE) {
            int idx = deadbeef -> pl_get_idx_of(it);
            deadbeef -> sendmessage(DB_EV_PLAY_NUM, 0, idx, 0);
        }
    } else {
        debug("add_file_cb NULL\n");
    }
    return 0;
}

/*
 * Handle /TrackList object method call
 */
static void handle_tracklist_method_call(GDBusConnection *connection,
                                    const gchar *sender,
                                    const gchar *object_path,
                                    const gchar *interface_name,
                                    const gchar *method_name,
                                    GVariant *parameters,
                                    GDBusMethodInvocation *invocation,
                                    gpointer user_data)
{
    //GetMetadata
    if(g_strcmp0(method_name, MPRIS_METHOD_GETMETA) == 0){
        int track_id = 0;
        g_variant_get(parameters, "(i)", & track_id);
        GVariant *ret_val = get_metadata(track_id);
        g_dbus_method_invocation_return_value(invocation, ret_val);
        return;
    }

    //GetCurrentTrack
    if(g_strcmp0(method_name, MPRIS_METHOD_GETCURRENTTRACK) == 0){
        int track_id = 0;
        DB_playItem_t *track = deadbeef -> streamer_get_playing_track();
        ddb_playlist_t *pl = deadbeef -> plt_get_curr();
        track_id = deadbeef -> plt_get_item_idx(pl ,track, PL_MAIN);
        deadbeef -> plt_unref(pl);
        GVariant *ret_val = g_variant_new("(i)", track_id);
        g_dbus_method_invocation_return_value(invocation, ret_val);
        return;
    }

    //GetLength
    if(g_strcmp0(method_name, MPRIS_METHOD_GETLENGTH) == 0){
        int track_id = 0;
        /*
         * We use the index of the last track to calculate the length
         * of the play list.
         */
        ddb_playlist_t *pl = deadbeef -> plt_get_curr();
        DB_playItem_t *track = deadbeef -> plt_get_last(pl, PL_MAIN);
        track_id = deadbeef -> plt_get_item_idx(pl, track, PL_MAIN);
        deadbeef -> plt_unref(pl);
        deadbeef -> pl_item_unref(track);
        GVariant *ret_val = g_variant_new("(i)", track_id + 1);
        g_dbus_method_invocation_return_value(invocation, ret_val);
        return;
    }

    //AddTrack
    if(g_strcmp0(method_name, MPRIS_METHOD_ADDTRACK) == 0){
        const char *uri;
        gboolean play;
        g_variant_get (parameters, "(sb)", &uri, &play);
        debug("Add Track: %s %d", uri, play);
        ddb_playlist_t *pl = deadbeef -> plt_get_curr();
        /*
         * !!add_file_cb can NOT be called...
         */
        int ret = deadbeef -> plt_add_file(pl, uri, add_file_cb, (void*)play);
        deadbeef -> plt_unref(pl);
        if (ret == 0) {
            deadbeef -> sendmessage(DB_EV_PLAYLISTCHANGED, 0, 0, 0);
        }
        
        g_dbus_method_invocation_return_value(invocation
                                        , g_variant_new("(i)", ret));
        return;
    }

    //DelTrack
    if(g_strcmp0(method_name, MPRIS_METHOD_DELTRACK) == 0){
        /*
         * No Supported!
         */
        debug("DelTrack is not supported...");
        g_dbus_method_invocation_return_value(invocation, NULL);
        return;
    }

    //SetLoop
    if(g_strcmp0(method_name, MPRIS_METHOD_SETLOOP) == 0){
        gboolean loop;
        g_variant_get(parameters, "(b)", &loop);
        if(loop == TRUE){
            set_loop_status(g_variant_new_string("Playlist"));
        }else{
            set_loop_status(g_variant_new_string("None"));
        }
        g_dbus_method_invocation_return_value(invocation, NULL);
        return;
    }

    //SetRandom
    if(g_strcmp0(method_name, MPRIS_METHOD_SETRANDOM) == 0){
        gboolean random;
        g_variant_get(parameters, "(b)", &random);
        if(random == FALSE){
            deadbeef -> conf_set_int("playback.order", PLAYBACK_ORDER_LINEAR);
        }else{
            deadbeef -> conf_set_int("playback.order", PLAYBACK_ORDER_RANDOM);
        }
        deadbeef -> sendmessage(DB_EV_CONFIGCHANGED, 0, 0, 0);
        g_dbus_method_invocation_return_value(invocation, NULL);
        return;
    }

    debug("Error! Unsupported method. %s.%s", interface_name, method_name);
    g_dbus_method_invocation_return_error(invocation
                                    , G_DBUS_ERROR
                                    , G_DBUS_ERROR_NOT_SUPPORTED
                                    , "Method %s.%s not supported"
                                    , interface_name
                                    , method_name);
    return;
}

void DB_mpris_emit_trackchange_v2()
{

    GVariant *metadata = get_metadata(-1);
    debug("emit track change signl.");
    emit_signal(server -> con, MPRIS_V1_INTERFACE, MPRIS_V1_PLAYER_PATH
                    , MPRIS_SIGNAL_TRACKCHANGE, metadata);
}

void DB_mpris_emit_statuschange_v2()
{
    debug("emit status change signl.");
    emit_signal(server -> con, MPRIS_V1_INTERFACE, MPRIS_V1_PLAYER_PATH
                    , MPRIS_SIGNAL_STATUSCHANGE, get_status());
}
void DB_mpris_emit_capschange_v2()
{
    /*
     * Will NOT change any more!!
     */
}

void DB_mpris_emit_tracklistchange_v2()
{
    int track_id = 0;
    /*
     * We use the index of the last track to calculate the length
     * of the play list.
     */
    ddb_playlist_t *pl = deadbeef -> plt_get_curr();
    DB_playItem_t *track = deadbeef -> plt_get_last(pl, PL_MAIN);
    track_id = deadbeef -> plt_get_item_idx(pl, track, PL_MAIN);
    deadbeef -> plt_unref(pl);
    deadbeef -> pl_item_unref(track);
    GVariant *ret_val = g_variant_new("(i)", track_id + 1);
    debug("emit tracklist change signl.");
    emit_signal(server -> con, MPRIS_V1_INTERFACE, MPRIS_V1_TRACKLIST_PATH
                    , MPRIS_SIGNAL_TRACKLISTCHAGE, ret_val);
}
#endif


#if 0
/* tracklist vtable */
static const GDBusInterfaceVTable tracklist_vtable =
{
    handle_tracklist_method_call,
    NULL,
    NULL
};
#endif

static void on_bus_acquired (GDBusConnection *connection,
                                 const gchar *name,
                                 gpointer user_data)
{
    GError *err = NULL;
    //The /Player object implemets org.freedesktop.MediaPalyer interface
    server -> player_reg_id = g_dbus_connection_register_object(
                            connection
                            , MPRIS_V2_PATH 
                            , server -> introspection_data_player -> interfaces[0]
                            , &player_vtable
                            , NULL      /* user_data */
                            , NULL      /* user_data_free_func */
                            , &err);     /* GError** */
    if(server -> player_reg_id == 0){
        debug("ERROR! Unable register interface %s on dbus. %s\n"
                            ,MPRIS_V2_INTERFACE_PLAYER , err -> message);
        DB_mpris_server_stop_v2(server);
        g_error_free(err);
        return;
    }

    //The / object implemets org.freedesktop.MediaPalyer interface
    server -> root_reg_id = g_dbus_connection_register_object(
                            connection
                            , MPRIS_V2_PATH 
                            , server -> introspection_data_root -> interfaces[0]
                            , &root_vtable
                            , NULL      /* user_data */
                            , NULL      /* user_data_free_func */
                            , &err);     /* GError** */
    if(server -> root_reg_id == 0){
        debug("ERROR! Unable register interface %s on dbus. %s\n"
                            , MPRIS_V2_INTERFACE_ROOT, err -> message);
        g_error_free(err);
        DB_mpris_server_stop_v2(server);
        return;
    }

#if 0
    //The /Tracklist object implemets org.freedesktop.MediaPalyer interface
    server -> tracklist_reg_id = g_dbus_connection_register_object(
                            connection
                            , MPRIS_V1_TRACKLIST_PATH
                            , server -> introspection_data_tracklist -> interfaces[0]
                            , &tracklist_vtable
                            , NULL      /* user_data */
                            , NULL      /* user_data_free_func */
                            , NULL);     /* GError** */
    g_assert(server -> tracklist_reg_id > 0);
#endif
    server -> con = connection;
}

static void on_name_acquired(GDBusConnection *connection,
                                  const gchar *name,
                                  gpointer user_data)
{
    debug("name acquired: %s", name);
}

static void on_name_lost (GDBusConnection *connection,
                              const gchar *name,
                              gpointer user_data)
{
    debug("name lost: %s", name);
}


gint DB_mpris_server_start_v2(DB_mpris_server_v2 **srv)
{
#if (GLIB_MAJOR_VERSION <= 2 && GLIB_MINOR_VERSION < 36)
    g_type_init();
#endif
    server = g_new(DB_mpris_server_v2, 1);
    if(server == NULL){
        debug("Create DB_mpris_server error!!\n");
        return DB_MPRIS_ERROR;
    }
    
    server -> introspection_data_root 
                        = g_dbus_node_info_new_for_xml(xml_v2_root, NULL);
    if(server -> introspection_data_root == NULL){
        debug("Create root dbus node info error!! %s %d\n", __FILE__, __LINE__);
        return DB_MPRIS_ERROR;
    }
    server -> introspection_data_player 
                        = g_dbus_node_info_new_for_xml(xml_v2_player, NULL);
    if(server -> introspection_data_player == NULL){
        debug("Create player dbus node info error!! %s %d\n", __FILE__, __LINE__);
        return DB_MPRIS_ERROR;
    }
  
#if 0
    server -> introspection_data_tracklist 
                        = g_dbus_node_info_new_for_xml(xml_v2_tracklist, NULL);
    if(server -> introspection_data_tracklist == NULL){
        debug("Create tracklist dbus node info error!! %s %d\n", __FILE__, __LINE__);
        return DB_MPRIS_ERROR;
    }
#endif
    server -> owner_id = g_bus_own_name(G_BUS_TYPE_SESSION
                                    , MPRIS_V2_SERVICE
                                    , G_BUS_NAME_OWNER_FLAGS_NONE
                                    , on_bus_acquired
                                    , on_name_acquired
                                    , on_name_lost
                                    , NULL, NULL);
    
    *srv = server;
    return DB_MPRIS_OK;
}

gint DB_mpris_server_stop_v2(DB_mpris_server_v2 *srv)
{
    g_dbus_connection_unregister_object(srv -> con, srv -> root_reg_id);
    g_dbus_connection_unregister_object(srv -> con, srv -> player_reg_id);
//    g_dbus_connection_unregister_object(srv -> con, srv -> tracklist_reg_id);
     
    g_bus_unown_name(srv -> owner_id);

    g_dbus_node_info_unref(srv -> introspection_data_root);
//    g_dbus_node_info_unref(srv -> introspection_data_tracklist);
    g_dbus_node_info_unref(srv -> introspection_data_player);

    g_free(srv);
    return DB_MPRIS_OK;
}
