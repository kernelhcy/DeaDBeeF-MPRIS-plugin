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
#include "mpris_common.h"
#include <stdarg.h>
#include <stdio.h>
#include <gio/gio.h>
#include <glib/gprintf.h>
#include <glib.h>

/*
 * Debug
 */
void do_debug(const char *fmt, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    g_printf("\e[32m\e[1mMPRIS Debug Info: \e[0m\e[34m");
    g_vprintf(fmt, arg_ptr);
    g_printf("\e[0m\n");
    va_end(arg_ptr);
}

/*
 * Cache the metadata
 */
static GVariant *curr_metadata = NULL;
static DB_playItem_t *curr_track = NULL;

/*
 * Get the meta data of the playing track
 */
GVariant* get_metadata(int track_id)
{
    DB_playItem_t *track = NULL;
    if(track_id < 0){
        track = deadbeef -> streamer_get_playing_track();
    }else{
        ddb_playlist_t *pl = deadbeef -> plt_get_curr();
        track = deadbeef -> plt_get_item_for_idx(
                                pl, track_id, PL_MAIN);
        deadbeef -> plt_unref(pl);
    }

    GVariant *tmp;
    GVariant *ret = NULL;

    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
    if(track == NULL){
        goto no_track_playing;
    }
    
    if(track == curr_track && curr_metadata != NULL){
        g_variant_ref(curr_metadata);
        deadbeef -> pl_item_unref(track);
        return curr_metadata;
    }

    if(curr_metadata != NULL){
        g_variant_unref(curr_metadata);
        curr_metadata = NULL;
    }

    char buf[500];
    int buf_size = sizeof(buf);

    gchar *uri_str; 
    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%F");
    uri_str = g_strdup_printf("file://%s", buf);
    debug("get_metadata_v1: uri %s", uri_str);  
    g_variant_builder_add (builder, "{sv}", "location", g_variant_new("s"
                                                , uri_str));
    g_free(uri_str);

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%t");
    debug("get_metadata_v1: title %s", buf);
    g_variant_builder_add(builder, "{sv}", "title", g_variant_new("s", buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%a");
    debug("get_metadata_v1: artist %s", buf);
    g_variant_builder_add(builder, "{sv}", "artist", g_variant_new("s", buf));
   
    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%b");
    debug("get_metadata_v1: album %s", buf);
    g_variant_builder_add(builder, "{sv}", "album", g_variant_new("s", buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%g");
    debug("get_metadata_v1: genre %s", buf);  
    g_variant_builder_add(builder, "{sv}", "genre", g_variant_new("s", buf));
    
    gint32 duration = (gint32)((deadbeef -> pl_get_item_duration(track)) * 1000.0);
    debug("get_metadata_v1: mtime %d", duration / 1000);
    g_variant_builder_add(builder, "{sv}", "mtime", g_variant_new("i", duration));
    debug("get_metadata_v1: time %d", duration);
    g_variant_builder_add(builder, "{sv}", "time", g_variant_new("i", duration / 1000));

    gint32 bitrate = (gint32)(deadbeef -> streamer_get_apx_bitrate());
    debug("get_metadata_v1: audio-bitrate: %d", bitrate);
    g_variant_builder_add(builder, "{sv}", "audio-bitrate", g_variant_new("i", bitrate));

    //unref the track item
    deadbeef -> pl_item_unref(track);

no_track_playing:
    tmp = g_variant_builder_end(builder);

    /*
     * We need a tuple containing a array of dict.
     */
    GVariantBuilder *ret_builder = g_variant_builder_new(G_VARIANT_TYPE("(a{sv})"));
    g_variant_builder_add_value(ret_builder, tmp);
    ret = g_variant_builder_end(ret_builder);
    g_variant_builder_unref(builder);
    g_variant_builder_unref(ret_builder);

    //cache the metadata
    g_variant_ref(ret);
    curr_metadata = ret;
    curr_track = track;

    return ret;
}

/*
 * Get the meta data of the playing track of MPRIS V2
 */
GVariant* get_metadata_v2(int track_id)
{
    DB_playItem_t *track = NULL;
    int id;
    ddb_playlist_t *pl = deadbeef -> plt_get_curr();
    if(track_id < 0){
        track = deadbeef -> streamer_get_playing_track();
        id = deadbeef -> plt_get_item_idx(pl, track, PL_MAIN);
    }else{
        track = deadbeef -> plt_get_item_for_idx(
                                pl, track_id, PL_MAIN);
        id = track_id;
    }
    deadbeef -> plt_unref(pl);

    GVariant *tmp;
    GVariant *ret = NULL;

    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
    if(track == NULL){
        goto no_track_playing;
    }
    
    char buf[500];
    int buf_size = sizeof(buf);

    g_sprintf(buf, "/org/mpris/MediaPlayer2/Track/track%d", id);
    debug("get_metadata_v2: mpris:trackid %s", buf);
    g_variant_builder_add (builder, "{sv}", "mpris:trackid", g_variant_new("o"
                                                , buf));

    gint32 duration = (gint32)((deadbeef -> pl_get_item_duration(track)) * 1000.0);
    debug("get_metadata_v2: length %d", duration);  
    g_variant_builder_add (builder, "{sv}", "mpris:length", g_variant_new("x"
                                                , (gint64)duration));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%b");
    debug("get_metadata_v2: album %s", buf);  
    g_variant_builder_add (builder, "{sv}", "xesam:album", g_variant_new("s"
                                                , buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%a");
    debug("get_metadata_v2: artist %s", buf);  
    g_variant_builder_add (builder, "{sv}", "xesam:artist", g_variant_new("s"
                                                , buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%t");
    debug("get_metadata_v2: tile %s", buf);  
    g_variant_builder_add (builder, "{sv}", "xesam:tile", g_variant_new("s"
                                                , buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%B");
    debug("get_metadata_v2: albumArtist %s", buf);  
    g_variant_builder_add (builder, "{sv}", "xesam:albumArtist"
                                , g_variant_new("s", buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%g");
    debug("get_metadata_v2: genre %s", buf);  
    g_variant_builder_add (builder, "{sv}", "xesam:genre", g_variant_new("s"
                                                , buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%c");
    debug("get_metadata_v2: comment %s", buf);  
    g_variant_builder_add (builder, "{sv}", "xesam:comment", g_variant_new("s"
                                                , buf));

    deadbeef -> pl_format_title(track, -1, buf, buf_size, -1, "%F");
    gchar *fullurl = g_strdup_printf("file://%s", buf);
    debug("get_metadata_v2: url %s", fullurl);  
    g_variant_builder_add (builder, "{sv}", "xesam:url", g_variant_new("s"
                                                , fullurl));
    g_free(fullurl);

    //unref the track item
    deadbeef -> pl_item_unref(track);

no_track_playing:
    tmp = g_variant_builder_end(builder);

    /*
     * We need a tuple containing a array of dict.
     */
    GVariantBuilder *ret_builder = g_variant_builder_new(G_VARIANT_TYPE("(a{sv})"));
    g_variant_builder_add_value(ret_builder, tmp);
    ret = g_variant_builder_end(ret_builder);
    g_variant_builder_unref(builder);
    g_variant_builder_unref(ret_builder);

    return ret;
}
/*
 * Set the loop status.
 * @param value 
 *          "None" no loop
 *          "Playlist" loop the play list
 *          "Track" loop the current track
 */
void set_loop_status(GVariant *value)
{    
    gchar *loop_status;
    g_variant_get(value, "s", &loop_status);

    if (g_strcmp0(loop_status, "None") == 0) {
        deadbeef -> conf_set_int("playback.loop", PLAYBACK_MODE_NOLOOP);
    } else if (g_strcmp0(loop_status, "Playlist") == 0) {
        deadbeef -> conf_set_int("playback.loop", PLAYBACK_MODE_LOOP_ALL);
    } else if (g_strcmp0(loop_status, "Track") == 0) {
        deadbeef -> conf_set_int("playback.loop", PLAYBACK_MODE_LOOP_SINGLE);
    }
    deadbeef -> sendmessage(DB_EV_CONFIGCHANGED, 0, 0, 0);
    return;
}

/*
 * Get the status of the player.
 */
GVariant *get_status()
{
    DB_output_t *output = deadbeef -> get_output();
    int first = 0;
    if(output == NULL){
        first = 2;
    }else{
        switch(output -> state())
        {
        case OUTPUT_STATE_PLAYING:
            first = 0;
            break;
        case OUTPUT_STATE_PAUSED:
            first = 1;
            break;
        case OUTPUT_STATE_STOPPED:
            first = 2;
            break;
        default:
            break;
        }
    }

    int second = 1;
    int order = deadbeef -> conf_get_int("playback.order", 0);
    if(order == PLAYBACK_ORDER_LINEAR){
        second = 0;
    }else if(order == PLAYBACK_ORDER_RANDOM){
        second = 1;
    }

    int loop = deadbeef -> conf_get_int("playback.loop", 0);
    int third, forth;
    switch(loop)
    {
    case PLAYBACK_MODE_NOLOOP:
        third = forth = 0;
        break;
    case PLAYBACK_MODE_LOOP_ALL:
        forth = 1;
        third = 0;
        break;
    case PLAYBACK_MODE_LOOP_SINGLE:
        forth = 0;
        third = 1;
        break;
    default:
        forth = 0;
        third = 0;
        break;
    }    

    return g_variant_new("((iiii))" , first, second, third, forth);
}

/*
 * The callback of timeout in main loop.
 */
static gboolean emit_signal_cb(gpointer data)
{
    SignalPar *par = data;
    
    g_dbus_connection_emit_signal(
                        par -> con
                        , NULL
                        , par -> object_path
                        , par -> interface
                        , par -> signal_name
                        , par -> data, NULL);
    g_free(par);
    return FALSE;
}

/*
 * Emit a signal of signal_name with data
 */
void emit_signal(GDBusConnection *con, const gchar *interface, const gchar *obj
                , const gchar * signal_name, gpointer data)
{
    GMainContext *ctx = g_main_context_get_thread_default();
    if(ctx == NULL){
        ctx = g_main_context_default();
    }

    SignalPar *par = g_malloc(sizeof(SignalPar));
    par -> con = con;
    par -> interface = interface;
    par -> data = data;
    par -> signal_name = signal_name;
    par -> object_path = obj;
    GSource *src = g_timeout_source_new((guint)0);
    g_source_set_callback(src, (GSourceFunc)emit_signal_cb, par, NULL);
    g_source_attach(src, ctx);
    g_source_unref(src);

}

