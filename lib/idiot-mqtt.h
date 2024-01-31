#ifndef IDIOT_MQTT_H
#define IDIOT_MQTT_H

#include <glib-object.h>

#include "idiot-types.h"

G_BEGIN_DECLS

#define IDIOT_TYPE_MQTT idiot_mqtt_get_type()
G_DECLARE_DERIVABLE_TYPE(IdiotMqtt,
		idiot_mqtt,
		IDIOT,
		MQTT,
		GObject);

struct _IdiotMqttClass {
	GObjectClass parnet_class;
};

IdiotMqtt *
idiot_mqtt_new(void);

gboolean
idiot_mqtt_connect(IdiotMqtt *self,
		const gchar *host, gint port, gint keepalive);

gboolean
idiot_mqtt_subscribe(IdiotMqtt *self, IdiotSubscribe *subscribe);

void
idiot_mqtt_start(IdiotMqtt *self);

void
idiot_mqtt_stop(IdiotMqtt *self, gboolean force);

void
idiot_mqtt_reconnect(IdiotMqtt *self);

gboolean
idiot_mqtt_is_pending_message(IdiotMqtt *self);

IdiotPublish *
idiot_mqtt_dispatch_message(IdiotMqtt *self);

G_END_DECLS

#endif /* end of include guard: IDIOT_MQTT_H */
