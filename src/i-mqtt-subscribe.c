#include "i-mqtt-subscribe.h"

G_DEFINE_TYPE(IMqttSubscribe, i_mqtt_subscribe, G_TYPE_OBJECT);

static void
i_mqtt_subscribe_class_init(IMqttSubscribeClass *klass)
{
}

static void
i_mqtt_subscribe_init(IMqttSubscribe *self)
{
}

IMqttSubscribe *
i_mqtt_subscribe_create(void)
{
	return g_object_new(I_TYPE_MQTT_SUBSCRIBE, NULL);
}

