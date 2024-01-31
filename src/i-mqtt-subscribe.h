#ifndef I_MQTT_SUBSCRIBE_H
#define I_MQTT_SUBSCRIBE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define I_TYPE_MQTT_SUBSCRIBE i_mqtt_subscribe_get_type()
G_DECLARE_FINAL_TYPE(IMqttSubscribe,
		i_mqtt_subscribe,
		I,
		MQTT_SUBSCRIBE,
		GObject);

struct _IMqttSubscribe {
	GObject parent_instance;

	gint message_id;
	gchar *topic_filter;
	gint qos;
	gint options;
	gpointer properties;
	GCallback callback;
};

IMqttSubscribe *
i_mqtt_subscribe_create(void);

G_END_DECLS

#endif /* end of include guard: I_MQTT_SUBSCRIBE_H */
