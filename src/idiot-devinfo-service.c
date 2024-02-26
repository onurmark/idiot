#include "idiot-devinfo-service.h"
#include "idiot-message.h"
#include <jansson.h>

typedef struct {
	IdiotMessage *devinfo;
} IdiotDevinfoServicePrivate;

static void
idiot_devinfo_service_iface_init(IdiotMqttServiceInterface *iface);

G_DEFINE_TYPE_WITH_CODE(IdiotDevinfoService, idiot_devinfo_service, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(IDIOT_TYPE_MQTT_SERVICE, idiot_devinfo_service_iface_init)
		G_ADD_PRIVATE(IdiotDevinfoService));

static void
connected(IdiotMqttService *iface)
{
	IdiotDevinfoServicePrivate *priv =
		idiot_devinfo_service_get_instance_private(
				IDIOT_DEVINFO_SERVICE(iface));

	g_signal_emit_by_name(iface,
			IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH,
			g_boxed_copy(IDIOT_TYPE_MESSAGE, priv->devinfo));
}

static void
idiot_devinfo_service_iface_init(IdiotMqttServiceInterface *iface)
{
	iface->connected = connected;
}

static void
idiot_devinfo_service_class_init(IdiotDevinfoServiceClass *klass)
{
}

static void
idiot_devinfo_service_init(IdiotDevinfoService *self)
{
	IdiotDevinfoServicePrivate *priv =
		idiot_devinfo_service_get_instance_private(self);

	priv->devinfo = idiot_message_new_json_decref(0,
			"status/client-id",
			json_pack("{s:i}", "online", 1),
			1,
			1);
}

IdiotMqttService *
idiot_devinfo_service_create(void)
{
	return g_object_new(IDIOT_TYPE_DEVINFO_SERVICE, NULL);
}
