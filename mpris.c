/*
    DeaDBeeF - ultimate music player for GNU/Linux systems with X11
    Copyright (C) 2009-2011 Alexey Yakovenko <waker@users.sourceforge.net>

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
#include "mpris_common.h"

DB_functions_t *deadbeef;

DB_mpris_server_v1 *srv_v1 = NULL;

GThread *server_thread_id = NULL;
GMainLoop  *mpris_main_loop = NULL;

static gpointer server_thread(gpointer data)
{
    mpris_main_loop = g_main_loop_new(NULL, FALSE);
    DB_mpris_server_start_v1(&srv_v1);
    g_main_loop_run(mpris_main_loop);
    return NULL;
}


int mpris_start (void) 
{
    debug("MPRIS Started....");
    if(!g_thread_supported()){
        g_thread_init(NULL);
        debug("Init the thread...");
    }
    GError *err = NULL;
    server_thread_id = g_thread_create(server_thread, NULL, FALSE, &err);
    if(server_thread_id == NULL){
        debug("Create MPRIS thread error. %d:%s", err -> code, err -> message);
        g_error_free(err);
        return -1;
    }
    return 0;
}

int mpris_stop (void) 
{    
    debug("MPRIS Stoped....");
    DB_mpris_server_stop_v1(srv_v1);
    g_main_loop_quit(mpris_main_loop);
    return 0;
}

static int mpris_message (uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) 
{
    switch(id)
    {
    case DB_EV_SEEKED:
        break;
    case DB_EV_SONGCHANGED:
        debug("Trach changed.");
        DB_mpris_emit_trackchange_v1();
        break;
    case DB_EV_PLAYLISTCHANGED:
        debug("Playlist changed.");
        DB_mpris_emit_tracklistchange_v1();
    case DB_EV_SONGSTARTED:
    case DB_EV_CONFIGCHANGED:
    case DB_EV_PAUSED:
    case DB_EV_TOGGLE_PAUSE:
    case DB_EV_STOP:
        DB_mpris_emit_statuschange_v1();
    default:
        break;
    }
    return 0;
}
static const char settings_dlg[] =
    "property \"Enable\" checkbox mpris.enable 1;\n"
;

DB_plugin_t plugin = {
    .api_vmajor = 1,
    .api_vminor = 0,
    .type = DB_PLUGIN_MISC,
    .version_major = 1,
    .version_minor = 0,
    .id = "mpris",
    .name = "MPRIS Plugin",
    .descr = "Communicate with other application useing D-Bus.",
    .copyright = 
        "Copyright (C) 2009-2011 Alexey Yakovenko <waker@users.sourceforge.net>\n"
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
