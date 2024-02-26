#ifndef IDIOT_MQTT_H
#define IDIOT_MQTT_H

#include <glib-object.h>

#include <idiot-types.h>
#include <idiot-message.h>

G_BEGIN_DECLS

#define IDIOT_TYPE_MQTT idiot_mqtt_get_type()
G_DECLARE_DERIVABLE_TYPE(IdiotMqtt,
		idiot_mqtt,
		IDIOT,
		MQTT,
		GObject);

#define IDIOT_MQTT_SIGNAL_CONNECTED    "connected"
#define IDIOT_MQTT_SIGNAL_MESSAGE      "message"
#define IDIOT_MQTT_SIGNAL_DISCONNECTED "disconnected"

struct _IdiotMqttClass {
	GObjectClass parnet_class;

	/* signals */
	void (*connected)(IdiotMqtt *self);
	void (*message)(IdiotMqtt *self, IdiotMessage *message);
	void (*disconnected)(IdiotMqtt *self);
};

IdiotMqtt *
idiot_mqtt_new(void);

gboolean
idiot_mqtt_subscribe(IdiotMqtt *self,
		gchar *topic_filter,
		gint qos,
		gint options);

gboolean
idiot_mqtt_publish(IdiotMqtt *self, IdiotMessage *message);

gboolean
idiot_mqtt_will(IdiotMqtt *self, IdiotMessage *message);

void
idiot_mqtt_reconnect(IdiotMqtt *self);

int
idiot_mqtt_get_socket_fd(IdiotMqtt *self);

void
idiot_mqtt_run_async(IdiotMqtt *self,
		const gchar *host,
		gint port,
		gint keepalive,
		GCancellable *cancellable,
		GAsyncReadyCallback callback,
		gpointer user_data);

gboolean
idiot_mqtt_run_finish(IdiotMqtt *self,
		GAsyncResult *result,
		GError **error);

void
idiot_mqtt_stop(IdiotMqtt *self);

gboolean
idiot_mqtt_is_running(IdiotMqtt *self);

G_END_DECLS

#endif /* end of include guard: IDIOT_MQTT_H */
