#include <mosquitto.h>

#include "idiot-mqtt-source.h"
#include "idiot.h"
#include "idiot-mqtt.h"
#include "idiot-publish.h"

typedef struct {
	GSource parent;
	IdiotMqtt *idiot;
	GDestroyNotify destroy_message;
} IdiotMqttSource;

static gboolean
idiot_mqtt_source_prepare(GSource *source,
		gint *timeout_)
{
	IdiotMqttSource *idiot_source = (IdiotMqttSource *)source;

	g_message("prepare()");

	return idiot_mqtt_is_pending_message(idiot_source->idiot);
}

static gboolean
idiot_mqtt_source_dispatch(GSource *source,
		GSourceFunc callback,
		gpointer user_data)
{
	IdiotMqttSource *idiot_source = (IdiotMqttSource *)source;
	IdiotMqttSourceFunc func = (IdiotMqttSourceFunc)callback;

	IdiotPublish *publish;

	g_message("dispatch()");

	publish = idiot_mqtt_dispatch_message(idiot_source->idiot);
	if (publish == NULL) {
		return TRUE;
	}

	if (func == NULL) {
		if (idiot_source->destroy_message != NULL) {
			idiot_source->destroy_message(publish);
		}

		return TRUE;
	}

	return func(publish, user_data);
}

static void
idiot_mqtt_source_finalize(GSource *source)
{
	IdiotMqttSource *idiot_source = (IdiotMqttSource *)source;

	g_message("finalize()");

	g_object_unref(idiot_source->idiot);
}

static gboolean
idiot_mqtt_source_closure_callback(IdiotPublish *publish,
		gpointer user_data)
{
	GClosure *closure = user_data;
	GValue param_value = G_VALUE_INIT;
	GValue result_value = G_VALUE_INIT;
	gboolean retval;

	g_value_init(&result_value, G_TYPE_BOOLEAN);
	g_value_init(&param_value, G_TYPE_BOXED);
	g_value_set_boxed(&param_value, publish);

	g_closure_invoke(closure, &result_value, 1, &param_value, NULL);
	retval = g_value_get_boolean(&result_value);

	g_value_unset(&param_value);
	g_value_unset(&result_value);

	return retval;
}

static GSourceFuncs idiot_mqtt_source_funcs = {
	idiot_mqtt_source_prepare,
	NULL,
	idiot_mqtt_source_dispatch,
	idiot_mqtt_source_finalize,
	(GSourceFunc)idiot_mqtt_source_closure_callback,
	NULL,
};

GSource *
idiot_mqtt_source_new(IdiotMqtt *idiot,
		GDestroyNotify destroy,
		GCancellable *cancellable)
{
	GSource *source;
	IdiotMqttSource *idiot_source;

	g_return_val_if_fail(cancellable == NULL ||
			G_IS_CANCELLABLE(cancellable), NULL);

	source = g_source_new(&idiot_mqtt_source_funcs,
			sizeof(IdiotMqttSource));
	idiot_source = (IdiotMqttSource *)source;

	g_source_set_name(source, "IdiotMqttSource");

	idiot_source->idiot = g_object_ref(idiot);
	idiot_source->destroy_message = destroy;

	if (cancellable != NULL) {
		GSource *cancellable_source;

		cancellable_source = g_cancellable_source_new(cancellable);
		g_source_set_dummy_callback(cancellable_source);
		g_source_add_child_source(source, cancellable_source);
		g_source_unref(cancellable_source);
	}

	return source;
}

