#include "i-mosquitto-message.h"

G_DEFINE_TYPE(IMosquittoMessage, i_mosquitto_message, G_TYPE_OBJECT);

static void
i_mosquitto_message_class_init(IMosquittoMessageClass *klass)
{
}

static void
i_mosquitto_message_init(IMosquittoMessage *self)
{
}

IMosquittoMessage *
i_mosquitto_message_new(gint mid,
		gchar *topic,
		gpointer payload,
		gint payload_len,
		gint qos,
		gboolean retain)
{
	return g_object_new(I_TYPE_MOSQUITTO_MESSAGE, NULL);
}
