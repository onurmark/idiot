#ifndef IDIOT_MQTT_SOURCE_H
#define IDIOT_MQTT_SOURCE_H

#include <glib-object.h>
#include <gio/gio.h>

#include <idiot-types.h>

G_BEGIN_DECLS

typedef gboolean (*IdiotMqttSourceFunc)(IdiotPublish *publish,
		gpointer user_data);

GSource *
idiot_mqtt_source_new(IdiotMqtt *mosquitto,
		GDestroyNotify destroy,
		GCancellable *cancellable);

G_END_DECLS

#endif /* end of include guard: IDIOT_MQTT_SOURCE_H */
