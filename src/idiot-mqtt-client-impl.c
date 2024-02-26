#include "idiot-mqtt-client-impl.h"

#include <idiot.h>

#include "dbus-mqtt-client.h"
#include "idiot-message.h"
#include "idiot-mqtt-service.h"
#include "idiot-mqtt.h"

typedef struct {
	IdiotMqtt *mqtt;
	guint connected_handler_id;
	guint message_handler_id;
	guint disconnected_handler_id;

	GHashTable *services;
	IdiotMessage *will;
} IdiotMqttClientImplPrivate;

static void
idiot_mqtt_client_iface_init(DBusMqttClientIface *iface);

G_DEFINE_TYPE_WITH_CODE(IdiotMqttClientImpl,
		idiot_mqtt_client_impl,
		DBUS_TYPE_MQTT_CLIENT_SKELETON,
		G_IMPLEMENT_INTERFACE(DBUS_TYPE_MQTT_CLIENT,
			idiot_mqtt_client_iface_init)
		G_ADD_PRIVATE(IdiotMqttClientImpl));

static void
idiot_mqtt_run_complete(GObject *source_object,
				     GAsyncResult *res,
				     gpointer user_data)
{
	IdiotMqtt *mqtt = IDIOT_MQTT(source_object);
	GError *error = NULL;

	idiot_mqtt_run_finish(mqtt, res, &error);

	g_message("Stop mqtt client");
}

static gboolean
handle_connect(
		DBusMqttClient *object,
		GDBusMethodInvocation *invocation,
		const gchar *arg_host,
		gint arg_port)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(
				IDIOT_MQTT_CLIENT_IMPL(object));

	g_message("%s(): host: %s, port: %d",
			__FUNCTION__, arg_host, arg_port);

	if (idiot_mqtt_is_running(priv->mqtt)) {
		g_message("Already started, stop and reconnect");
		idiot_mqtt_stop(priv->mqtt);
	}

	idiot_mqtt_run_async(priv->mqtt,
			arg_host,
			arg_port,
			60,
			NULL,
			(GAsyncReadyCallback)idiot_mqtt_run_complete,
			NULL);

	dbus_mqtt_client_complete_connect(object, invocation);

	return TRUE;
}

static gboolean
handle_disconnect(
		DBusMqttClient *object,
		GDBusMethodInvocation *invocation)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(
				IDIOT_MQTT_CLIENT_IMPL(object));

	g_message("%s()", __FUNCTION__);

	if (!idiot_mqtt_is_running(priv->mqtt)) {
		g_message("Already stopped.");
		goto skip;
	}
	idiot_mqtt_publish(priv->mqtt, priv->will);

	idiot_mqtt_stop(priv->mqtt);

skip:
	dbus_mqtt_client_complete_disconnect(object, invocation);

	return TRUE;
}

static void
idiot_mqtt_client_iface_init(DBusMqttClientIface *iface)
{
	iface->handle_connect    = handle_connect;
	iface->handle_disconnect = handle_disconnect;
}

static void
idiot_mqtt_client_impl_dispose(GObject *object)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(
				IDIOT_MQTT_CLIENT_IMPL(object));

	if (priv->mqtt) {
		g_signal_handler_disconnect(priv->mqtt, priv->connected_handler_id);
		g_signal_handler_disconnect(priv->mqtt, priv->message_handler_id);
		g_signal_handler_disconnect(priv->mqtt, priv->disconnected_handler_id);

		g_clear_object(&priv->mqtt);
	}

	g_clear_pointer(&priv->services, g_hash_table_unref);

	G_OBJECT_CLASS(idiot_mqtt_client_impl_parent_class)->dispose(object);
}

static void
idiot_mqtt_client_impl_finalize(GObject *object)
{
	G_OBJECT_CLASS(idiot_mqtt_client_impl_parent_class)->finalize(object);
}

static void
idiot_mqtt_client_impl_class_init(IdiotMqttClientImplClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(klass);

	g_object_class->dispose = idiot_mqtt_client_impl_dispose;
	g_object_class->finalize = idiot_mqtt_client_impl_finalize;
}

static void
on_connected(IdiotMqtt *mqtt, gpointer user_data)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(user_data);

	g_message("connected");

	GList *services, *l;

	services = g_hash_table_get_values(priv->services);
	for (l = services; l != NULL; l = l->next) {
		idiot_mqtt_service_connected(l->data);
	}
	g_list_free(services);
}

static void
on_message(IdiotMqtt *mqtt, IdiotMessage *message, gpointer user_data)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(user_data);
	IdiotMqttService *service;
	GError *error = NULL;

	g_message("message");

	service = g_hash_table_lookup(priv->services, message->topic);
	if (service == NULL) {
		g_message("Not found topic: %s matched service:", message->topic);
		return;
	}

	if (!idiot_mqtt_service_entry_point(service, message, &error)) {
		g_message("Error in service '%s': %s",
				message->topic,
				error->message);
		g_clear_error(&error);
	}
}

static void
on_disconnected(IdiotMqtt *mqtt, gpointer user_data)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(user_data);

	g_message("disconnected");

	GList *services, *l;

	services = g_hash_table_get_values(priv->services);
	for (l = services; l != NULL; l = l->next) {
		idiot_mqtt_service_disconnected(l->data);
	}
	g_list_free(services);
}

static void
idiot_mqtt_client_impl_init(IdiotMqttClientImpl *self)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(self);
	GSource *source;

	priv->mqtt = idiot_mqtt_new();
	priv->connected_handler_id = g_signal_connect(priv->mqtt,
			IDIOT_MQTT_SIGNAL_CONNECTED,
			(GCallback)on_connected,
			self);
	priv->message_handler_id = g_signal_connect(priv->mqtt,
			IDIOT_MQTT_SIGNAL_MESSAGE,
			(GCallback)on_message,
			self);
	priv->disconnected_handler_id = g_signal_connect(priv->mqtt,
			IDIOT_MQTT_SIGNAL_DISCONNECTED,
			(GCallback)on_disconnected,
			self);

	priv->will = idiot_message_new(0, "status/client-id", "0", 1, 1, 0);
	idiot_mqtt_will(priv->mqtt, priv->will);

	priv->services =
		g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_object_unref);
}

static void
on_publish(IdiotMqttService *service,
		IdiotMessage *message,
		gpointer user_data)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(user_data);
	
	if (idiot_mqtt_is_running(priv->mqtt)) {
		idiot_mqtt_publish(priv->mqtt, message);
	}
}

static void
on_publish_text(IdiotMqttService *service,
		gchar *topic,
		gchar *message,
		gpointer user_data)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(user_data);
	IdiotMessage *mqtt_message;

	if (idiot_mqtt_is_running(priv->mqtt)) {
		mqtt_message = idiot_message_new(0, topic, message, strlen(message), 1, 0);
		idiot_mqtt_publish(priv->mqtt, mqtt_message);
		idiot_message_free(mqtt_message);
	}
}

static void
on_publish_json(IdiotMqttService *service,
		gchar *topic,
		json_t *message,
		gpointer user_data)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(user_data);
	IdiotMessage *mqtt_message;

	if (idiot_mqtt_is_running(priv->mqtt)) {
		mqtt_message = idiot_message_new_json(0, topic, message, 1, 0);
		idiot_mqtt_publish(priv->mqtt, mqtt_message);
		idiot_message_free(mqtt_message);
	}
}

void
idiot_mqtt_client_impl_append_service(IdiotMqttClientImpl *self,
		const gchar *topic,
		IdiotMqttService *service)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(self);

	g_signal_connect(service,
			IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH,
			(GCallback)on_publish,
			self);

	g_signal_connect(service,
			IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH_TEXT,
			(GCallback)on_publish_text,
			self);

	g_signal_connect(service,
			IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH_JSON,
			(GCallback)on_publish_json,
			self);

	g_hash_table_insert(priv->services, g_strdup(topic), service);
}

void
idiot_mqtt_client_impl_export_service(IdiotMqttClientImpl *self)
{
	IdiotMqttClientImplPrivate *priv =
		idiot_mqtt_client_impl_get_instance_private(self);

	GDBusConnection *connection;

	connection = g_dbus_interface_skeleton_get_connection(G_DBUS_INTERFACE_SKELETON(self));

	if (connection) {
		GList *services, *l;

		services = g_hash_table_get_values(priv->services);
		for (l = services; l != NULL; l = l->next) {
			idiot_mqtt_service_connect_dbus(l->data, connection, NULL);
		}

		g_list_free(services);
	}
}

GDBusInterfaceSkeleton *
idiot_mqtt_client_impl_create(void)
{
	return g_object_new(IDIOT_TYPE_MQTT_CLIENT_IMPL, NULL);
}
