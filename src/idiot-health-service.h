#ifndef IDIOT_HEALTH_SERVICE_H
#define IDIOT_HEALTH_SERVICE_H

#include <glib-object.h>

#include "idiot-mqtt-service.h"

G_BEGIN_DECLS

#define IDIOT_TYPE_HEALTH_SERVICE idiot_health_service_get_type()
G_DECLARE_DERIVABLE_TYPE(IdiotHealthService,
		idiot_health_service,
		IDIOT,
		HEALTH_SERVICE,
		GObject);

struct _IdiotHealthServiceClass {
	GObjectClass parent_class;
};

IdiotMqttService *
idiot_health_service_create(void);

gboolean
idiot_health_service_connect_dbus(IdiotHealthService *self,
		GDBusConnection *connection);

G_END_DECLS

#endif /* end of include guard: IDIOT_HEALTH_SERVICE_H */
