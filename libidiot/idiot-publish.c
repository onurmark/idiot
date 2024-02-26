#include "idiot-publish.h"

G_DEFINE_BOXED_TYPE(IdiotPublish, idiot_publish,
		idiot_publish_copy,
		idiot_publish_free);

IdiotPublish *
idiot_publish_new(gint message_id,
	gchar *topic,
	gpointer payload,
	gint payload_len,
	gint qos,
	gboolean retain)
{
	IdiotPublish *publish;

	publish = g_slice_new(IdiotPublish);
	publish->message_id = publish->message_id;
	publish->topic = g_strdup(publish->topic);
	publish->payload = g_memdup(payload, payload_len);
	publish->payload_len = publish->payload_len;
	publish->qos = publish->qos;
	publish->retain = publish->retain;

	return publish;
}

IdiotPublish *
idiot_publish_copy(IdiotPublish *publish)
{
	IdiotPublish *copy;

	copy = g_slice_new(IdiotPublish);

	copy->message_id = publish->message_id;
	copy->topic = g_strdup(publish->topic);
	copy->payload = g_memdup(publish->payload, publish->payload_len);
	copy->payload_len = publish->payload_len;
	copy->qos = publish->qos;
	copy->retain = publish->retain;

	return copy;
}

void
idiot_publish_free(IdiotPublish *publish)
{
	g_free(publish->topic);
	g_free(publish->payload);
	g_slice_free(IdiotPublish, publish);
}

