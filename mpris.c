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
#include <deadbeef/deadbeef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "mpris_v1.h"
#include "mpris_v2.h"
#include "mpris_common.h"

DB_functions_t *deadbeef;

static gint mpris_v1_enable = 1;
static DB_mpris_server_v1 *srv_v1 = NULL;
static gint mpris_v2_enable = 1;
static DB_mpris_server_v2 *srv_v2 = NULL;

static GThread *server_thread_id = NULL;
static GMainLoop  *mpris_main_loop = NULL;

static gpointer server_thread(gpointer data)
{
    mpris_main_loop = g_main_loop_new(NULL, FALSE);
    if(mpris_v1_enable == 1){
        debug("MPRIS V1 Starting...");
        DB_mpris_server_start_v1(&srv_v1);
    }
    if(mpris_v2_enable == 1){
        debug("MPRIS V2 Starting...");
        DB_mpris_server_start_v2(&srv_v2);
    }
    g_main_loop_run(mpris_main_loop);
    return NULL;
}


static gint mpris_start() 
{
#if (GLIB_MAJOR_VERSION <= 2 && GLIB_MINOR_VERSION < 32)
    if(!g_thread_supported()){
        g_thread_init(NULL);
        debug("Init the thread...");
    }
#endif
    GError *err = NULL;
#if (GLIB_MAJOR_VERSION <= 2 && GLIB_MINOR_VERSION < 32)
    server_thread_id = g_thread_create(server_thread, NULL, FALSE, &err);
#else
    server_thread_id = g_thread_new(NULL, server_thread, NULL);
#endif
    if(server_thread_id == NULL){
        debug("Create MPRIS thread error. %d:%s", err -> code, err -> message);
        g_error_free(err);
        return -1;
    }
    return 0;
}

static gint mpris_stop() 
{    
    debug("MPRIS Stoped....");
    if(mpris_v1_enable == 1){
        DB_mpris_server_stop_v1(srv_v1);
    }
    if(mpris_v2_enable == 1){
        DB_mpris_server_stop_v2(srv_v2);
    }
    g_main_loop_quit(mpris_main_loop);
    return 0;
}

/*
 * Restart the MPRIS.
 */
static void mpris_restart()
{
    int v1_enable = 1, v2_enable = 1;
    v1_enable = deadbeef -> conf_get_int("mpris_v1.enable", v1_enable);
    v2_enable = deadbeef -> conf_get_int("mpris_v2.enable", v2_enable);

    if(v1_enable != mpris_v1_enable || v2_enable != mpris_v2_enable){
        mpris_stop();
        mpris_v1_enable = v1_enable;
        mpris_v2_enable = v2_enable;
        mpris_start();
    }
}

static gint mpris_message (uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) 
{
    ddb_event_playpos_t *pp = NULL; 

    switch(id)
    {
    case DB_EV_SEEKED:
        if(mpris_v2_enable == 1){
            pp = (ddb_event_playpos_t*)ctx;
            DB_mpris_emit_seeked_v2((gint64)(pp -> playpos * 1000.0));
        }
        break;
    case DB_EV_SONGCHANGED:
        debug("Track changed.");
        if(mpris_v1_enable == 1){
            DB_mpris_emit_trackchange_v1();
        }
        break;
    case DB_EV_PLAYLISTCHANGED:
        debug("Playlist changed.");
        if(mpris_v1_enable == 1){
            DB_mpris_emit_tracklistchange_v1();
        }
        break;
    case DB_EV_CONFIGCHANGED:
        //Maybe we need to restart...
        mpris_restart();
    case DB_EV_SONGSTARTED:
    case DB_EV_PAUSED:
    case DB_EV_TOGGLE_PAUSE:
    case DB_EV_STOP:
        if(mpris_v1_enable == 1){
            DB_mpris_emit_statuschange_v1();
        }
        break;
    default:
        break;
    }
    return 0;
}
static const gchar settings_dlg[] =
    "property \"Enable MPRIS V1\" checkbox mpris_v1.enable 1;\n"
    "property \"Enable MPRIS V2\" checkbox mpris_v2.enable 1;\n"
;

DB_plugin_t plugin = {
    .api_vmajor = 1,
    .api_vminor = 0,
    .type = DB_PLUGIN_MISC,
    .version_major = 2,
    .version_minor = 1,
    .id = "mpris",
    .name ="MPRIS v1 and v2 plugin",
    .descr = "Communicate with other applications using D-Bus.",
    .copyright = 
        "Copyright (C) 2009-2011 HuangCongyu <huangcongyu2006@gmail.com>\n"
        "\n"
        "This program is free software; you can redistribute it and/or\n"
        "modify it under the terms of the GNU General Public License\n"
        "as published by the Free Software Foundation; either version 2\n"
        "of the License, or (at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
        "GNU General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU General Public License\n"
        "along with this program; if not, write to the Free Software\n"
        "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.\n"
    ,
    .website = "http://code.google.com/p/deadbeef-mpris-plugin/",
    .start = mpris_start,
    .stop = mpris_stop,
    .connect = NULL,
    .disconnect = NULL,
    .configdialog = settings_dlg,
    .message = mpris_message,
};

DB_plugin_t * mpris_load (DB_functions_t *ddb) 
{
    debug("Load...");
    deadbeef = ddb;
    return &plugin;
}
