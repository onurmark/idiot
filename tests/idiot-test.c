#include "idiot-mqtt-source.h"
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include <idiot.h>

static gboolean
on_message(IdiotPublish *publish, gpointer user_data)
{
	g_message("topic: %s", (gchar *)publish->payload);

	return TRUE;
}

gboolean
attach_context(IdiotMqtt *idiot, GMainContext *context)
{
	GSource *source;

	source = idiot_mqtt_source_new(idiot, NULL, NULL);

	g_source_set_callback(source, (GSourceFunc)on_message, g_object_ref(idiot), g_object_unref);
	g_source_attach(source, context);
	g_source_unref(source);

	return TRUE;
}

int main(int argc, char *argv[])
{
	IdiotMqtt *mqtt;
	IdiotSubscribe subscribe;
	GMainContext *context;
	GMainLoop *loop;

	memset(&subscribe, 0, sizeof(IdiotSubscribe));
	subscribe.topic_filter = g_strdup("device/config");
	subscribe.qos = 1;
	
	idiot_library_init();

	mqtt = idiot_mqtt_new();

	idiot_mqtt_connect(mqtt, "192.168.228.153", 1883, 60);
	idiot_mqtt_subscribe(mqtt, &subscribe);

	idiot_mqtt_start(mqtt);


	// g_object_unref(mqtt);

	context = g_main_context_new();
	attach_context(mqtt, context);
	loop = g_main_loop_new(context, TRUE);
	g_main_context_unref(context);

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	idiot_library_exit();

	return EXIT_SUCCESS;
}
