#ifndef IDIOT_SUBSCRIBE_H
#define IDIOT_SUBSCRIBE_H

#include <glib-object.h>

G_BEGIN_DECLS

#define IDIOT_TYPE_SUBSCRIBE idiot_subscribe_get_type()

typedef struct _IdiotSubscribe {
	gint message_id;
	gchar *topic_filter;
	gint qos;
	gint options;
	gpointer properties;
} IdiotSubscribe;

GType idiot_subscribe_get_type(void);

IdiotSubscribe *
idiot_subscribe_copy(IdiotSubscribe *subscribe);

void
idiot_subscribe_free(IdiotSubscribe *subscribe);

G_END_DECLS

#endif /* end of include guard: I_MQTT_SUBSCRIBE_H */
