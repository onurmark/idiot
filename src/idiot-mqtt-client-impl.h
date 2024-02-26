#ifndef IDIOT_MQTT_CLIENT_IMPL_H
#define IDIOT_MQTT_CLIENT_IMPL_H

#include <glib-object.h>

#include <dbus-mqtt-client.h>

#include "idiot-mqtt-service.h"

G_BEGIN_DECLS

#define IDIOT_TYPE_MQTT_CLIENT_IMPL idiot_mqtt_client_impl_get_type()
G_DECLARE_DERIVABLE_TYPE(IdiotMqttClientImpl,
		idiot_mqtt_client_impl,
		IDIOT,
		MQTT_CLIENT_IMPL,
		DBusMqttClientSkeleton);

struct _IdiotMqttClientImplClass {
	DBusMqttClientSkeletonClass parent_class;
};

GDBusInterfaceSkeleton *
idiot_mqtt_client_impl_create(void);

void
idiot_mqtt_client_impl_append_service(IdiotMqttClientImpl *self,
		const gchar *topic,
		IdiotMqttService *service);

void
idiot_mqtt_client_impl_export_service(IdiotMqttClientImpl *self);

G_END_DECLS

#endif /* end of include guard: IDIOT_MQTT_CLIENT_IMPL_H */
