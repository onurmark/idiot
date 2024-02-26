#ifndef IDIOT_MESSAGE_H
#define IDIOT_MESSAGE_H

#include <glib-object.h>
#include <jansson.h>

G_BEGIN_DECLS

#define IDIOT_TYPE_MESSAGE idiot_message_get_type()

typedef struct _IdiotMessage {
	gint id;
	char *topic;
	gpointer payload;
	gint payload_len;
	gint qos;
	gboolean retain;
} IdiotMessage;

GType idiot_message_get_type(void);

IdiotMessage *
idiot_message_new(gint id,
	gchar *topic,
	gpointer payload,
	gint payload_len,
	gint qos,
	gboolean retain);

IdiotMessage *
idiot_message_new_json(gint id,
		gchar *topic,
		json_t *payload,
		gint qos,
		gboolean retain);

IdiotMessage *
idiot_message_new_json_decref(gint id,
		gchar *topic,
		json_t *payload,
		gint qos,
		gboolean retain);

IdiotMessage *
idiot_message_copy(IdiotMessage *message);

void
idiot_message_free(IdiotMessage *message);

G_END_DECLS

#endif /* end of include guard: IDIOT_MESSAGE_H */
