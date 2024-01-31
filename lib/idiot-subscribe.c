#include "idiot-subscribe.h"

G_DEFINE_BOXED_TYPE(IdiotSubscribe, idiot_subscribe,
		idiot_subscribe_copy,
		idiot_subscribe_free);

IdiotSubscribe *
idiot_subscribe_copy(IdiotSubscribe *subscribe)
{
	IdiotSubscribe *copy;

	copy = g_slice_new(IdiotSubscribe);

	copy->message_id = subscribe->message_id;
	copy->topic_filter = g_strdup(subscribe->topic_filter);
	copy->qos = subscribe->qos;
	copy->options = subscribe->options;
	copy->properties = subscribe->properties;

	return copy;
}

void
idiot_subscribe_free(IdiotSubscribe *subscribe)
{
	g_free(subscribe->topic_filter);
	g_slice_free(IdiotSubscribe, subscribe);
}

