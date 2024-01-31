#ifndef IDIOT_PUBLISH_H
#define IDIOT_PUBLISH_H

#include <gio/gio.h>

#include "idiot-types.h"

G_BEGIN_DECLS

#define IDIOT_TYPE_PUBLISH idiot_publish_get_type()

typedef struct _IdiotPublish {
	gint message_id;
	gchar *topic;
	gpointer payload;
	gint payload_len;
	gint qos;
	gboolean retain;
} IdiotPublish;

GType idiot_publish_get_type(void);

IdiotPublish *
idiot_publish_copy(IdiotPublish *publish);

void
idiot_publish_free(IdiotPublish *publish);

G_END_DECLS

#endif /* end of include guard: IDIOT_PUBLISH_H */
