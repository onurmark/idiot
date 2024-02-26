#ifndef IDIOT_DEVINFO_SERVICE_H
#define IDIOT_DEVINFO_SERVICE_H

#include <glib-object.h>

#include "idiot-mqtt-service.h"

G_BEGIN_DECLS

#define IDIOT_TYPE_DEVINFO_SERVICE idiot_devinfo_service_get_type()
G_DECLARE_DERIVABLE_TYPE(IdiotDevinfoService,
		idiot_devinfo_service,
		IDIOT,
		DEVINFO_SERVICE,
		GObject);

struct _IdiotDevinfoServiceClass {
	GObjectClass parent_class;
};

IdiotMqttService *
idiot_devinfo_service_create(void);

G_END_DECLS

#endif /* end of include guard: IDIOT_DEVINFO_SERVICE_H */
