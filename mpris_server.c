#include "mpris_server.h"
#include "introspection_xml.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <gio/gio.h>

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
  	server -> registration_id = g_dbus_connection_register_object(
		  					connection
                            , MPRIS_PLAYER_PATH 
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
	
	server -> introspection_data = g_dbus_node_info_new_for_xml(introspection_xml
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
