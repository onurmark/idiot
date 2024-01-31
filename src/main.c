#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "i-mosquitto.h"

#define SERIAL_NUMBER "A12131213"

static GMainLoop *loop = NULL;

static gboolean
on_message(gpointer messge, gpointer user_data)
{
	g_message("Message received");

	return TRUE;
}

static gint id1 = 1, id2 = 2;

static void
mosquitto_start(const gchar *host, gint port)
{
	IMosquitto *mosquitto = NULL;
	GSource *source = NULL;

	mosquitto = i_mosquitto_new();

	i_mosquitto_connect(mosquitto, host, port, 60);

	i_mosquitto_add_subscribe(mosquitto, &id1, "device/" SERIAL_NUMBER "/config");
	i_mosquitto_add_subscribe(mosquitto, &id2, "device/" SERIAL_NUMBER "/config2");

	i_mosquitto_start(mosquitto);

	source = i_mosquitto_source_new(mosquitto, NULL, NULL);
	g_object_unref(mosquitto);

	g_source_set_callback(source, (GSourceFunc)on_message, NULL, NULL);
	g_source_attach(source, g_main_loop_get_context(loop));
	g_source_unref(source);
}

int
main(int argc, char *argv[])
{
	loop = g_main_loop_new(NULL, FALSE);

	mosquitto_start("192.168.228.153", 1883);

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	return EXIT_SUCCESS;
}
