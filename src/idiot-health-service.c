#include "idiot-health-service.h"

#include "dbus-health-monitor.h"
#include "idiot-message.h"
#include "idiot-mqtt-service.h"
#include <jansson.h>

typedef struct {
	DBusHealthMonitor *monitor;
} IdiotHealthServicePrivate;

static void
idiot_health_service_iface_init(IdiotMqttServiceInterface *iface);

G_DEFINE_TYPE_WITH_CODE(IdiotHealthService, idiot_health_service, G_TYPE_OBJECT,
		G_IMPLEMENT_INTERFACE(IDIOT_TYPE_MQTT_SERVICE, idiot_health_service_iface_init)
		G_ADD_PRIVATE(IdiotHealthService));

static gboolean
entry_point(IdiotMqttService *iface,
		IdiotMessage *message,
		GError **error)
{
	g_message("%s", message->topic);

	return TRUE;
}

static void
on_cpu_usage(DBusHealthMonitor *monitor,
		gdouble usage,
		gpointer user_data)
{
	IdiotMqttService *service = user_data;
	IdiotMessage *message;

	g_message("cpu usage: %f", usage);

	message = idiot_message_new_json_decref(1,
			"device/1",
			json_pack("{s:i}", "cpu usage", usage),
			1,
			0);
	g_signal_emit_by_name(service, IDIOT_MQTT_SERVICE_SIGNAL_PUBLISH, message);
}

static gboolean
connect_dbus(IdiotMqttService *iface,
		GDBusConnection *connection,
		GError **error)
{
	IdiotHealthServicePrivate *priv =
		idiot_health_service_get_instance_private(
				IDIOT_HEALTH_SERVICE(iface));

	priv->monitor = dbus_health_monitor_proxy_new_sync(connection,
			G_DBUS_PROXY_FLAGS_NONE,
			"net.piolink.switch.health",
			"/net/piolink/switch/health/Monitor",
			NULL, NULL);

	g_signal_connect(priv->monitor,
			"cpu-usage", (GCallback)on_cpu_usage, iface);

	return TRUE;
}

static void
idiot_health_service_iface_init(IdiotMqttServiceInterface *iface)
{
	iface->entry_point = entry_point;
	iface->connect_dbus = connect_dbus;
}

static void
idiot_health_service_class_init(IdiotHealthServiceClass *klass)
{
}

static void
idiot_health_service_init(IdiotHealthService *self)
{
	IdiotHealthServicePrivate *priv =
		idiot_health_service_get_instance_private(self);
}

IdiotMqttService *
idiot_health_service_create(void)
{
	return g_object_new(IDIOT_TYPE_HEALTH_SERVICE, NULL);
}

