#ifndef I_MOSQUITTO_MESSAGE_H
#define I_MOSQUITTO_MESSAGE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define I_TYPE_MOSQUITTO_MESSAGE i_mosquitto_message_get_type()
G_DECLARE_FINAL_TYPE(IMosquittoMessage,
		i_mosquitto_message,
		I,
		MOSQUITTO_MESSAGE,
		GObject);

struct _IMosquittoMessage {
	GObjectClass parent_class;

	gint mid;
	gchar *topic;
	gpointer payload;
	gint payload_len;
	gint qos;
	gboolean retain;
};

IMosquittoMessage *
i_mosquitto_message_new(gint mid,
		gchar *topic,
		gpointer payload,
		gint payload_len,
		gint qos,
		gboolean retain);

G_END_DECLS

#endif /* end of include guard: I_MOSQUITTO_MESSAGE_H */
