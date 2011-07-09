#include "mpris_v1.h"
#include "mpris_common.h"
#include "introspection_xml.h"

#include <gio/gio.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>

//caps
enum{
    NONE                  = 0,
    CAN_GO_NEXT           = 1 << 0,
    CAN_GO_PREV           = 1 << 1,
    CAN_PAUSE             = 1 << 2,
    CAN_PLAY              = 1 << 3,
    CAN_SEEK              = 1 << 4,
    CAN_PROVIDE_METADATA  = 1 << 5,
    CAN_HAS_TRACKLIST     = 1 << 6
};


struct _DB_mpris_server_v1
{
    GDBusConnection *con;

    GDBusNodeInfo *introspection_data_root;
    GDBusNodeInfo *introspection_data_player;
    GDBusNodeInfo *introspection_data_tracklist;

    guint owner_id;

    guint root_reg_id;
    guint player_reg_id;
    guint tracklist_reg_id;
};

/*
 * Information. 
 */
static DB_mpris_server_v1 *server = NULL;

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
    //Identity
    if(g_strcmp0(method_name, MPRIS_METHOD_IDENTITY) == 0){
        gchar identify[100];
        g_sprintf(identify, "DeadBeef %d.%d", deadbeef -> vmajor, deadbeef -> vminor);
        g_dbus_method_invocation_return_value(invocation
                            , g_variant_new("(s)", identify));
        return;
    }

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

    //MprisVersion
    if(g_strcmp0(method_name, MPRIS_METHOD_VERSION) == 0){
        g_dbus_method_invocation_return_value(invocation
                        , g_variant_new("((qq))", 1, 0));
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
    
    //Prev
    if(g_strcmp0(method_name, MPRIS_METHOD_PREV) == 0){
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
    
    //Stop
    if(g_strcmp0(method_name, MPRIS_METHOD_STOP) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        deadbeef -> sendmessage(DB_EV_STOP, 0, 0, 0);
        goto go_return;
    }
    
    //Pause
    if(g_strcmp0(method_name, MPRIS_METHOD_PAUSE) == 0){
        g_dbus_method_invocation_return_value(invocation, NULL);
        int state = deadbeef->get_output()->state();
        if (state == OUTPUT_STATE_PLAYING){
            deadbeef->sendmessage(DB_EV_PAUSE, 0, 0, 0);
        }else{
            deadbeef->sendmessage(DB_EV_PLAY_CURRENT, 0, 0, 0);
        }
        goto go_return;
    }
    
    //Repeat
    if(g_strcmp0(method_name, MPRIS_METHOD_REPEAT) == 0){
        gboolean loop;
        g_variant_get(parameters, "(b)", &loop);
        if(loop == TRUE){
            set_loop_status(g_variant_new_string("Track"));
        }else{
            set_loop_status(g_variant_new_string("None"));
        }
        g_dbus_method_invocation_return_value(invocation, NULL);
        goto go_return;
    }
    
    GVariant *ret_val = NULL;
    //GetStatus
    if(g_strcmp0(method_name, MPRIS_METHOD_GETSTATUS) == 0){
        g_dbus_method_invocation_return_value(invocation
                            , get_status());
        goto go_return;
    }

    //GetMetadata
    if(g_strcmp0(method_name, MPRIS_METHOD_GETMETA) == 0){
        ret_val = get_metadata(-1);
        g_dbus_method_invocation_return_value(invocation, ret_val);
        goto go_return;
    }

    //GetCaps
    if(g_strcmp0(method_name, MPRIS_METHOD_GETCAPS) == 0){
        ret_val =  g_variant_new("((i))" , CAN_GO_NEXT | CAN_GO_PREV
                                           | CAN_PAUSE | CAN_PLAY
                                           | CAN_SEEK | CAN_PROVIDE_METADATA
                                           | CAN_HAS_TRACKLIST);
        g_dbus_method_invocation_return_value(invocation, ret_val);
        goto go_return;
    }

    //PositionGet
    if(g_strcmp0(method_name, MPRIS_METHOD_POSITIONGET) == 0){
        DB_playItem_t *track = NULL;
        track = deadbeef -> streamer_get_playing_track();
        float duration = deadbeef -> pl_get_item_duration(track);
        float pos = deadbeef -> playback_get_pos(); 
        //we need ms
        ret_val =  g_variant_new("(i)", (int)(pos * duration * 10));    
        g_dbus_method_invocation_return_value(invocation, ret_val);

        deadbeef -> pl_item_unref(track);
        goto go_return;
    }

    //PositionSet
    if(g_strcmp0(method_name, MPRIS_METHOD_POSITIONSET) == 0){
        int pos = 0;
        g_variant_get(parameters, "(i)", &pos);
        debug("Set position %d.", pos);
        deadbeef -> sendmessage(DB_EV_SEEK, 0, pos, 0);
        g_dbus_method_invocation_return_value(invocation, NULL);
        goto go_return;
    }

    //VolumeGet
    if(g_strcmp0(method_name, MPRIS_METHOD_VOLUMEGET) == 0){
        float min_vol = deadbeef -> volume_get_min_db();
        float volume = deadbeef -> volume_get_db() - min_vol;
        debug("Get Volume: %f", volume);
        ret_val =  g_variant_new("(i)" , (int)(volume / ( - min_vol) * 100));
        g_dbus_method_invocation_return_value(invocation, ret_val);
        goto go_return;
    }

    //VolumeSet
    if(g_strcmp0(method_name, MPRIS_METHOD_VOLUMESET) == 0){
        int volume = 0;
        g_variant_get(parameters, "(i)", &volume);
        float vol_f = 50 - ((float)volume / (float)100 * (float)50);
        debug("Set Volume: %d %f", volume, vol_f);
        deadbeef -> volume_set_db(-vol_f);
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
 * The callback of plt_add_file
 */
static int add_file_cb(DB_playItem_t *it, void *data) {
    gboolean *play_imm = (gboolean*)data;
    if (it != NULL) {
        debug("add_file_cb %d\n", play_imm);
        if (*play_imm == TRUE) {
            ddb_playlist_t *pl = deadbeef -> plt_get_curr();
            int idx = deadbeef -> plt_get_item_idx(pl, it, PL_MAIN);
            deadbeef -> sendmessage(DB_EV_PLAY_NUM, 0, idx, 0);
            deadbeef -> plt_unref(pl);
        }
    } else {
        debug("add_file_cb NULL\n");
    }
    g_free(play_imm);
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
        gboolean *play, playnow;
        play = g_malloc(sizeof(gboolean));
        g_variant_get (parameters, "(sb)", &uri, play);
        playnow = *play;
        debug("Add Track: %s %d", uri, *play);
        ddb_playlist_t *pl = deadbeef -> plt_get_curr();
        /*
         * !!add_file_cb can NOT be called...
         */
        int ret = deadbeef -> plt_add_file(pl, uri, add_file_cb, (void*)play);
        deadbeef -> plt_unref(pl);
        if (ret == 0) {
            deadbeef -> sendmessage(DB_EV_PLAYLISTCHANGED, 0, 0, 0);
        }

        if(playnow){
            /*
             * Play the new added track.
             * I don't know why plt_add_file do not call the add_file_cb.
             * The track is added at the last of the playlist, so we can play it
             * using as following.
             */
            ddb_playlist_t *pl = deadbeef -> plt_get_curr();
            DB_playItem_t *track = deadbeef -> plt_get_last(pl, PL_MAIN);
            int track_id = deadbeef -> plt_get_item_idx(pl, track, PL_MAIN);
            deadbeef -> plt_unref(pl);
            deadbeef -> pl_item_unref(track);
            deadbeef -> sendmessage(DB_EV_PLAY_NUM, 0, track_id, 0);
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

void DB_mpris_emit_trackchange_v1()
{

    GVariant *metadata = get_metadata(-1);
    debug("emit track change signl.");
    emit_signal(server -> con, MPRIS_V1_INTERFACE, MPRIS_V1_PLAYER_PATH
                    , MPRIS_SIGNAL_TRACKCHANGE, metadata);
}

void DB_mpris_emit_statuschange_v1()
{
    debug("emit status change signl.");
    emit_signal(server -> con, MPRIS_V1_INTERFACE, MPRIS_V1_PLAYER_PATH
                    , MPRIS_SIGNAL_STATUSCHANGE, get_status());
}
void DB_mpris_emit_capschange_v1()
{
    /*
     * Will NOT change any more!!
     */
}

void DB_mpris_emit_tracklistchange_v1()
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

/* root vtable */
static const GDBusInterfaceVTable root_vtable =
{
    handle_root_method_call,
    NULL,
    NULL
};
/* player vtable */
static const GDBusInterfaceVTable player_vtable =
{
    handle_player_method_call,
    NULL,
    NULL
};
/* tracklist vtable */
static const GDBusInterfaceVTable tracklist_vtable =
{
    handle_tracklist_method_call,
    NULL,
    NULL
};
static void on_bus_acquired (GDBusConnection *connection,
                                 const gchar *name,
                                 gpointer user_data)
{
    //The /Player object implemets org.freedesktop.MediaPalyer interface
    server -> player_reg_id = g_dbus_connection_register_object(
                            connection
                            , MPRIS_V1_PLAYER_PATH 
                            , server -> introspection_data_player -> interfaces[0]
                            , &player_vtable
                            , NULL      /* user_data */
                            , NULL      /* user_data_free_func */
                            , NULL);     /* GError** */
    g_assert(server -> player_reg_id > 0);

    //The / object implemets org.freedesktop.MediaPalyer interface
    server -> root_reg_id = g_dbus_connection_register_object(
                            connection
                            , MPRIS_V1_ROOT_PATH 
                            , server -> introspection_data_root -> interfaces[0]
                            , &root_vtable
                            , NULL      /* user_data */
                            , NULL      /* user_data_free_func */
                            , NULL);     /* GError** */
    g_assert(server -> root_reg_id > 0);

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


gint DB_mpris_server_start_v1(DB_mpris_server_v1 **srv)
{
    g_type_init();

    server = g_new(DB_mpris_server_v1, 1);
    if(server == NULL){
        debug("Create DB_mpris_server error!!\n");
        return DB_MPRIS_ERROR;
    }
    
    server -> introspection_data_root 
                        = g_dbus_node_info_new_for_xml(xml_v1_root, NULL);
    if(server -> introspection_data_root == NULL){
        debug("Create root dbus node info error!! %s %d\n", __FILE__, __LINE__);
        return DB_MPRIS_ERROR;
    }
    server -> introspection_data_player 
                        = g_dbus_node_info_new_for_xml(xml_v1_player, NULL);
    if(server -> introspection_data_player == NULL){
        debug("Create player dbus node info error!! %s %d\n", __FILE__, __LINE__);
        return DB_MPRIS_ERROR;
    }
    server -> introspection_data_tracklist 
                        = g_dbus_node_info_new_for_xml(xml_v1_tracklist, NULL);
    if(server -> introspection_data_tracklist == NULL){
        debug("Create tracklist dbus node info error!! %s %d\n", __FILE__, __LINE__);
        return DB_MPRIS_ERROR;
    }

    server -> owner_id = g_bus_own_name(G_BUS_TYPE_SESSION
                                    , MPRIS_V1_SERVICE
                                    , G_BUS_NAME_OWNER_FLAGS_NONE
                                    , on_bus_acquired
                                    , on_name_acquired
                                    , on_name_lost
                                    , NULL, NULL);
    
    *srv = server;
    return DB_MPRIS_OK;
}

gint DB_mpris_server_stop_v1(DB_mpris_server_v1 *srv)
{
    g_dbus_connection_unregister_object(srv -> con, srv -> root_reg_id);
    g_dbus_connection_unregister_object(srv -> con, srv -> player_reg_id);
    g_dbus_connection_unregister_object(srv -> con, srv -> tracklist_reg_id);
     
    g_bus_unown_name(srv -> owner_id);

    g_dbus_node_info_unref(srv -> introspection_data_root);
    g_dbus_node_info_unref(srv -> introspection_data_tracklist);
    g_dbus_node_info_unref(srv -> introspection_data_player);

    g_free(srv);
    return DB_MPRIS_OK;
}
