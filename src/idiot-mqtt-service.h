#ifndef IDIOT_MQTT_SERVICE_H
#define IDIOT_MQTT_SERVICE_H

#include <glib-object.h>
#include <jansson.h>

#include <idiot-mqtt.h>

G_BEGIN_DECLS

#define IDIOT_TYPE_MQTT_SERVICE idiot_mqtt_service_get_type()
G_DECLARE_INTERFACE(IdiotMqttService,
		idiot_mqtt_service,
		IDIOT,
		MQTT_SERVICE,
		GObject);

#define IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH "publish"
#define IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH_TEXT "publish-text"
#define IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH_JSON "publish-json"

struct _IdiotMqttServiceInterface {
	GTypeInterface parent_iface;

	void (*connected)(IdiotMqttService *self);

	gboolean (*entry_point)(IdiotMqttService *self,
			IdiotMessage *message,
			GError **error);

	void (*disconnected)(IdiotMqttService *self);

	gboolean (*connect_dbus)(IdiotMqttService *self,
			GDBusConnection *connection,
			GError **error);

	/* signals */
	void (*publish)(IdiotMqttService *self, IdiotMessage *message);
	void (*publish_text)(IdiotMqttService *self, gchar *topic, gchar *message);
	void (*publish_json)(IdiotMqttService *self, gchar *topic, json_t *message);
};

void
idiot_mqtt_service_connected(IdiotMqttService *service);

gboolean
idiot_mqtt_service_entry_point(IdiotMqttService *self,
		IdiotMessage *message,
		GError **error);

void
idiot_mqtt_service_disconnected(IdiotMqttService *service);

gboolean
idiot_mqtt_service_connect_dbus(IdiotMqttService *self,
		GDBusConnection *connection,
		GError **error);

G_END_DECLS

#endif /* end of include guard: IDIOT_MQTT_SERVICE_H */
