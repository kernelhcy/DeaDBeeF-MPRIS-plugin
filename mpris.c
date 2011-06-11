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
#include <dbus/dbus.h>
#include "../../deadbeef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../gettext.h"
#include "../artwork/artwork.h"

DB_functions_t *deadbeef;
DB_misc_t plugin;
DB_artwork_plugin_t *artwork_plugin;


int mpris_start (void) 
{
    return 0;
}

int mpris_stop (void) 
{	
    return 0;
}

static int mpris_connect (void) {
    return 0;
}

static int mpris_disconnect (void) {
    return 0;
}

static int mpris_message (uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) 
{
	return 0;
}
static const char settings_dlg[] =
    "property \"Enable\" checkbox mpris.enable 0;\n"
;

DB_misc_t plugin = {
    .plugin.api_vmajor = 1,
    .plugin.api_vminor = 0,
    .plugin.type = DB_PLUGIN_MISC,
    .plugin.version_major = 1,
    .plugin.version_minor = 0,
    .plugin.id = "mpirs",
    .plugin.name = "MPRIS Implement",
    .plugin.descr = "",
    .plugin.copyright = 
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
    .plugin.website = "",
    .plugin.start = mpris_start,
    .plugin.stop = mpris_stop,
    .plugin.connect = mpris_connect,
    .plugin.disconnect = mpris_disconnect,
    .plugin.configdialog = settings_dlg,
    .plugin.message = mpris_message,
};

DB_plugin_t * mpris_load (DB_functions_t *ddb) {
    deadbeef = ddb;
    return &plugin.plugin;
}
