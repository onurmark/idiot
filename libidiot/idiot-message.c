#include "idiot-message.h"

G_DEFINE_BOXED_TYPE(IdiotMessage, idiot_message,
		idiot_message_copy,
		idiot_message_free);

IdiotMessage *
idiot_message_new(gint id,
	gchar *topic,
	gpointer payload,
	gint payload_len,
	gint qos,
	gboolean retain)
{
	IdiotMessage *message;

	message = g_slice_new(IdiotMessage);
	message->id = id;
	message->topic = g_strdup(topic);
	message->payload = g_memdup(payload, payload_len);
	message->payload_len = payload_len;
	message->qos = qos;
	message->retain = retain;

	return message;
}

IdiotMessage *
idiot_message_new_json(gint id,
		gchar *topic,
		json_t *payload,
		gint qos,
		gboolean retain)
{
	IdiotMessage *message;
	gchar *dumps;

	dumps = json_dumps(payload, 0);
	message = idiot_message_new(id,
			topic,
			dumps,
			strlen(dumps) + 1,
			qos,
			retain);
	g_free(dumps);

	return message;
}

IdiotMessage *
idiot_message_new_json_decref(gint id,
		gchar *topic,
		json_t *payload,
		gint qos,
		gboolean retain)
{
	IdiotMessage *message;

	message = idiot_message_new_json(id,
			topic,
			payload,
			qos,
			retain);

	json_decref(payload);

	return message;
}

IdiotMessage *
idiot_message_copy(IdiotMessage *message)
{
	IdiotMessage *copy;

	copy = g_slice_new(IdiotMessage);
	copy->id = message->id;
	copy->topic = g_strdup(message->topic);
	copy->payload = g_memdup(message->payload, message->payload_len);
	copy->payload_len = message->payload_len;
	copy->qos = message->qos;
	copy->retain = message->retain;

	return copy;
}

void
idiot_message_free(IdiotMessage *message)
{
	g_message("free");
	g_free(message->topic);
	g_free(message->payload);

	g_slice_free(IdiotMessage, message);
}
