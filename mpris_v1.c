#include "mpris_v1.h"
#include "introspection_xml.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gio/gio.h>
#include <glib-2.0/glib.h>

#include <../../streamer.h>

#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <signal.h>
#ifdef __linux__
#include <execinfo.h>
#endif
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <unistd.h>

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

/*
 * Debug
 */
void do_debug(const char *fmt, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    printf("\e[32m\e[1mMPRIS Debug Info: \e[0m\e[34m");
    vprintf(fmt, arg_ptr);
    printf("\e[0m\n");
    va_end(arg_ptr);
}

struct _DB_mpris_server
{
	GDBusNodeInfo *introspection_data;
	guint owner_id;
  	guint registration_id;
};

/*
 * Information. 
 */
static DB_mpris_server *server = NULL;

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
	//Identity
	if(g_strcmp0(method_name, MPRIS_METHOD_IDENTITY) == 0){
		gchar identify[100];
		sprintf(identify, "DeadBeef %d.%d", deadbeef -> vmajor, deadbeef -> vminor);
		g_dbus_method_invocation_return_value(invocation
							, g_variant_new("(s)", identify));
		return;
	}

	//Quit
	if(g_strcmp0(method_name, MPRIS_METHOD_QUIT) == 0){
		g_dbus_method_invocation_return_value(invocation, NULL);

		//deadbeef -> quit();
		return;
	}

	//MprisVersion
	if(g_strcmp0(method_name, MPRIS_METHOD_VERSION) == 0){
		g_dbus_method_invocation_return_value(invocation
						, g_variant_new("((qq))", 1, 0));
		return;
	}
}

static void get_metadata_value(GVariantBuilder *builder
                                , DB_playItem_t *track
                                , const char *key)
{
    const char *value;
	value = deadbeef -> pl_find_meta(track, "artist");
	if(value != NULL){
		g_variant_builder_add(builder, "{sv}", "artist"
								, g_variant_new_string(value));
		debug("artist: %s", value);
	}
}

/*
 * Get the meta data of the playing track
 */
static GVariant* get_metadata()
{
	DB_playItem_t *track = deadbeef -> streamer_get_playing_track();

	const char *value;
	GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE_ARRAY);
	if(track == NULL){
		goto no_track_playing;
	}
	
        get_metadata_value(builder, track, "artist");
        get_metadata_value(builder, track, "album");
        get_metadata_value(builder, track, "title");
        get_metadata_value(builder, track, "location");
        get_metadata_value(builder, track, "tracknumber");
        //get_metadata_value(builder, track, "");

	//unref the track item
	deadbeef -> pl_item_unref(track);

no_track_playing:
	/*
	 * We MUST have at least one element!
	 */
	g_variant_builder_add(builder, "{sv}", "dump"
							, g_variant_new_string("nothing"));

	/*
	 * We need a tuple containing a array of dict.
	 * We MUST pass the builder to g_variant_new!!
	 */
	GVariant *ret = g_variant_new("(a{sv})", builder);
	g_variant_builder_unref(builder);
	return ret;
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
		return;
	}
	
	//Prev
	if(g_strcmp0(method_name, MPRIS_METHOD_PREV) == 0){
		g_dbus_method_invocation_return_value(invocation, NULL);
		deadbeef -> sendmessage(DB_EV_PREV, 0, 0, 0);
		return;
	}
	
	//Play
	if(g_strcmp0(method_name, MPRIS_METHOD_PLAY) == 0){
		g_dbus_method_invocation_return_value(invocation, NULL);
		deadbeef -> sendmessage(DB_EV_PLAY_CURRENT, 0, 0, 0);
		return;
	}
	
	//Stop
	if(g_strcmp0(method_name, MPRIS_METHOD_STOP) == 0){
		g_dbus_method_invocation_return_value(invocation, NULL);
		deadbeef -> sendmessage(DB_EV_STOP, 0, 0, 0);
		return;
	}
	
	//Pause
	if(g_strcmp0(method_name, MPRIS_METHOD_PAUSE) == 0){
		g_dbus_method_invocation_return_value(invocation, NULL);
    	int state = deadbeef->get_output()->state();
    	if (state == OUTPUT_STATE_PLAYING){
        	deadbeef->sendmessage(DB_EV_PAUSE, 0, 0, 0);
    	}
    	else {
        	deadbeef->sendmessage(DB_EV_PLAY_CURRENT, 0, 0, 0);
   	 	}
		return;
	}
	
	//Repeat
	if(g_strcmp0(method_name, MPRIS_METHOD_REPEAT) == 0){
		g_dbus_method_invocation_return_value(invocation, NULL);
		/*
		 * NOT supported!
		 */
		return;
	}
	
	GVariant *ret_val = NULL;
	//GetStatus
	if(g_strcmp0(method_name, MPRIS_METHOD_GETSTATUS) == 0){
		ret_val =  g_variant_new("((iiii))" , 1, 1, 1, 1);
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}

	//GetMetadata
	if(g_strcmp0(method_name, MPRIS_METHOD_GETMETA) == 0){
		ret_val = get_metadata();
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}

	//GetCaps
	if(g_strcmp0(method_name, MPRIS_METHOD_GETCAPS) == 0){
		ret_val =  g_variant_new("((i))" , CAN_GO_NEXT | CAN_GO_PREV
                                           | CAN_PAUSE | CAN_PLAY
                                           | CAN_SEEK | CAN_PROVIDE_METADATA);
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}

    //PositionGet
	if(g_strcmp0(method_name, MPRIS_METHOD_POSITIONGET) == 0){
        float pos = deadbeef -> streamer_get_playpos(); 
		ret_val =  g_variant_new("(i)", (int)(pos * 1000));;
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}
/* 
	if(g_strcmp0(method_name, MPRIS_METHOD_GETSTATUS) == 0){
		ret_val =  g_variant_new("((iiii))" , 1, 1, 1, 1);
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}

	if(g_strcmp0(method_name, MPRIS_METHOD_GETSTATUS) == 0){
		ret_val =  g_variant_new("((iiii))" , 1, 1, 1, 1);
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}

	if(g_strcmp0(method_name, MPRIS_METHOD_GETSTATUS) == 0){
		ret_val =  g_variant_new("((iiii))" , 1, 1, 1, 1);
		g_dbus_method_invocation_return_value(invocation, ret_val);
		g_variant_unref(ret_val);
		return;
	}
*/
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
}

/*
 * Handle the method call.
 */
static void handle_method_call(GDBusConnection *connection,
                    			const gchar *sender,
                    			const gchar *object_path,
                    			const gchar *interface_name,
                    			const gchar *method_name,
                    			GVariant *parameters,
                    			GDBusMethodInvocation *invocation,
                    			gpointer user_data)
{
	debug("Method call: %s %s %s", object_path, interface_name, method_name);

	// /
	if(g_strcmp0(object_path, MPRIS_ROOT_PATH) == 0){
		handle_root_method_call(connection, sender, object_path
							, interface_name, method_name
							, parameters, invocation
							, user_data);
		return;
	}

	// /Player
	if(g_strcmp0(object_path, MPRIS_PLAYER_PATH) == 0){
		handle_player_method_call(connection, sender, object_path
							, interface_name, method_name
							, parameters, invocation
							, user_data);
		return;
	}

	// /TrackList
	if(g_strcmp0(object_path, MPRIS_TRACKLIST_PATH) == 0){
		handle_tracklist_method_call(connection, sender, object_path
							, interface_name, method_name
							, parameters, invocation
							, user_data);
		return;
	}
	g_dbus_method_invocation_return_value(invocation, NULL);
}

static GVariant *handle_get_property(GDBusConnection *connection,
                     				const gchar *sender,
                     				const gchar *object_path,
                     				const gchar *interface_name,
                     				const gchar *property_name,
                     				GError **error,
                     				gpointer user_data)
{
	return NULL;
}

static gboolean handle_set_property(GDBusConnection *connection,
                     				const gchar *sender,
                     				const gchar *object_path,
                     				const gchar *interface_name,
                     				const gchar*property_name,
                     				GVariant *value,
                     				GError **error,
                     				gpointer user_data)
{

}

/* vtable */
static const GDBusInterfaceVTable interface_vtable =
{
	handle_method_call,
  	handle_get_property,
  	handle_set_property
};
static void on_bus_acquired (GDBusConnection *connection,
                	 			const gchar *name,
                 				gpointer user_data)
{
	//The /Player object implemets org.freedesktop.MediaPalyer interface
  	server -> registration_id = g_dbus_connection_register_object(
		  					connection
                            , MPRIS_PLAYER_PATH 
                            , server -> introspection_data -> interfaces[0]
                            , &interface_vtable
                            , NULL  	/* user_data */
                            , NULL  	/* user_data_free_func */
                            , NULL); 	/* GError** */
  	g_assert(server -> registration_id > 0);

	//The /object implemets org.freedesktop.MediaPalyer interface
  	server -> registration_id = g_dbus_connection_register_object(
		  					connection
                            , MPRIS_ROOT_PATH 
                            , server -> introspection_data -> interfaces[0]
                            , &interface_vtable
                            , NULL  	/* user_data */
                            , NULL  	/* user_data_free_func */
                            , NULL); 	/* GError** */
  	g_assert(server -> registration_id > 0);
}

static void on_name_acquired(GDBusConnection *connection,
                  				const gchar *name,
                  				gpointer user_data)
{

}

static void on_name_lost (GDBusConnection *connection,
              				const gchar *name,
              				gpointer user_data)
{

}


gint DB_mpris_server_start(DB_mpris_server **srv)
{
	g_type_init();

	server = g_new(DB_mpris_server, 1);
	if(server == NULL){
		printf("Create DB_mpris_server error!!\n");
		return DB_MPRIS_ERROR;
	}
	
	server -> introspection_data = g_dbus_node_info_new_for_xml(xml_v1
																, NULL);
	if(server -> introspection_data == NULL){
		printf("Create dbus node info error!!\n", __FILE__, __LINE__);
		return DB_MPRIS_ERROR;
	}

	server -> owner_id = g_bus_own_name(G_BUS_TYPE_SESSION
									, MPRIS_SERVICE
									, G_BUS_NAME_OWNER_FLAGS_NONE
									, on_bus_acquired
									, on_name_acquired
									, on_name_lost
									, NULL, NULL);
	
	*srv = server;
	return DB_MPRIS_OK;
}
gint DB_mpris_server_stop(DB_mpris_server *srv)
{

	g_free(srv);
	return DB_MPRIS_OK;
}
