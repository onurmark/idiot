#include "idiot-mqtt.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include <idiot.h>

static gboolean
on_message(IdiotPublish *publish, gpointer user_data)
{
	g_message("topic: %s, [%d]: %s",
			publish->topic, publish->message_id, (gchar *)publish->payload);

	return TRUE;
}

static void
idiot_mqtt_run_complete(GObject *source_object,
				     GAsyncResult *res,
				     gpointer user_data)
{
	IdiotMqtt *mqtt = IDIOT_MQTT(source_object);
	GError *error = NULL;

	idiot_mqtt_run_finish(mqtt, res, &error);

	g_message("Stop mqtt client");
}

int main(int argc, char *argv[])
{
	IdiotMqtt *mqtt;
	GMainLoop *loop;
	GError *error = NULL;

	idiot_library_init();

	mqtt = idiot_mqtt_new();

	idiot_mqtt_subscribe(mqtt, "device/+/config", 1, 0);
	idiot_mqtt_subscribe(mqtt, "device/#", 1, 0);

	idiot_mqtt_run_async(mqtt, "192.168.228.153", 1883, 60, NULL, (GAsyncReadyCallback)idiot_mqtt_run_complete, NULL);

	loop = g_main_loop_new(NULL, FALSE);

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	idiot_library_exit();

	return EXIT_SUCCESS;
}
