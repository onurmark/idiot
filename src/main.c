#include <glib.h>

#include "idiot-dbus.h"
#include "idiot-devinfo-service.h"
#include "idiot-health-service.h"
#include "idiot-mqtt-client-impl.h"

typedef struct {
	GMainLoop *loop;
	IdiotDbus *dbus;
} IdiotApplication;

static IdiotApplication *
application_create(void)
{
	IdiotApplication *application = g_new0(IdiotApplication, 1);
	GDBusInterfaceSkeleton *skeleton;

	application->loop = g_main_loop_new(NULL, FALSE);

	application->dbus = idiot_dbus_new();

	/* append dbus skeleton */
	skeleton = idiot_mqtt_client_impl_create();
	idiot_dbus_append_skeleton(application->dbus,
			"/net/piolink/switch/idiot/MqttClient",
			skeleton);

	idiot_mqtt_client_impl_append_service(IDIOT_MQTT_CLIENT_IMPL(skeleton),
			"device", idiot_health_service_create());
	idiot_mqtt_client_impl_append_service(IDIOT_MQTT_CLIENT_IMPL(skeleton),
			"status", idiot_devinfo_service_create());

	g_object_unref(skeleton);


	return application;
}

static void
application_destroy(IdiotApplication *application)
{
	g_main_loop_unref(application->loop);
	g_object_unref(application->dbus);

	g_free(application);
}

int
main(int argc, char *argv[])
{
	IdiotApplication *application;

	application = application_create();
	
	idiot_dbus_connect(application->dbus);

	g_main_loop_run(application->loop);

	application_destroy(application);

	return EXIT_SUCCESS;
}
