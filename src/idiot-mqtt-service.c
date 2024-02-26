#include "idiot-mqtt-service.h"

G_DEFINE_INTERFACE(IdiotMqttService, idiot_mqtt_service, G_TYPE_OBJECT);

enum {
	SIGNAL_PUBLISH,
	SIGNAL_PUBLISH_TEXT,
	SIGNAL_PUBLISH_JSON,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0, };

static void
idiot_mqtt_service_default_init(IdiotMqttServiceInterface *self)
{
	signals[SIGNAL_PUBLISH] =
		g_signal_new(IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH,
				G_TYPE_FROM_INTERFACE(self),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET(IdiotMqttServiceInterface, publish),
				NULL, NULL, NULL,
				G_TYPE_NONE,
				1,
				IDIOT_TYPE_MESSAGE);

	signals[SIGNAL_PUBLISH_TEXT] =
		g_signal_new(IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH_TEXT,
				G_TYPE_FROM_INTERFACE(self),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET(IdiotMqttServiceInterface, publish_text),
				NULL, NULL, NULL,
				G_TYPE_NONE,
				2,
				G_TYPE_STRING,
				G_TYPE_STRING);

	signals[SIGNAL_PUBLISH_JSON] =
		g_signal_new(IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH_JSON,
				G_TYPE_FROM_INTERFACE(self),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET(IdiotMqttServiceInterface, publish_json),
				NULL, NULL, NULL,
				G_TYPE_NONE,
				2,
				G_TYPE_STRING,
				G_TYPE_POINTER);
}

void
idiot_mqtt_service_connected(IdiotMqttService *self)
{
	IdiotMqttServiceInterface *iface;

	g_return_if_fail(IDIOT_IS_MQTT_SERVICE);
	
	iface = IDIOT_MQTT_SERVICE_GET_IFACE(self);

	if (iface->connected) {
		iface->connected(self);
	}
}

gboolean
idiot_mqtt_service_entry_point(IdiotMqttService *self,
		IdiotMessage *message,
		GError **error)
{
	IdiotMqttServiceInterface *iface;

	g_return_val_if_fail(IDIOT_IS_MQTT_SERVICE, FALSE);
	
	iface = IDIOT_MQTT_SERVICE_GET_IFACE(self);

	g_return_val_if_fail(iface->entry_point != NULL, FALSE);

	return iface->entry_point(self, message, error);
}

void
idiot_mqtt_service_disconnected(IdiotMqttService *self)
{
	IdiotMqttServiceInterface *iface;

	g_return_if_fail(IDIOT_IS_MQTT_SERVICE);
	
	iface = IDIOT_MQTT_SERVICE_GET_IFACE(self);

	if (iface->disconnected) {
		iface->disconnected(self);
	}
}

gboolean
idiot_mqtt_service_connect_dbus(IdiotMqttService *self,
		GDBusConnection *connection,
		GError **error)
{
	IdiotMqttServiceInterface *iface;

	g_return_val_if_fail(IDIOT_IS_MQTT_SERVICE, FALSE);
	
	iface = IDIOT_MQTT_SERVICE_GET_IFACE(self);

	if (iface->connect_dbus) {
		return iface->connect_dbus(self, connection, error);
	}

	return TRUE;
}
