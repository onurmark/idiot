#include "i-mosquitto.h"
#include "gobject/gmarshal.h"

#include <mosquitto.h>

typedef struct {
	struct mosquitto *mosq;
} IMosquittoPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IMosquitto, i_mosquitto, G_TYPE_OBJECT);

static void
on_connect(struct mosquitto *mosq,
		void *obj,
		int reason_code,
		int flags,
		const mosquitto_property *prop);

static void
on_subscribe(struct mosquitto *mosq,
		void *obj,
		int mid,
		int qos,
		const int *granted_qos,
		const mosquitto_property *prop);

static void
on_message(struct mosquitto *mosq,
		void *obj,
		const struct mosquitto_message *msg,
		const mosquitto_property *prop);

static void
i_mosquitto_class_init(IMosquittoClass *klass)
{
}

static void
i_mosquitto_init(IMosquitto *self)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(self);

	priv->mosq = mosquitto_new(NULL, true, self);

	mosquitto_connect_v5_callback_set(priv->mosq, on_connect);
	mosquitto_subscribe_v5_callback_set(priv->mosq, on_subscribe);
	mosquitto_message_v5_callback_set(priv->mosq, on_message);
}

static void
on_connect(struct mosquitto *mosq,
		void *obj,
		int reason_code,
		int flags,
		const mosquitto_property *prop)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(I_MOSQUITTO(obj));

	int rc;

	g_message("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if (reason_code != 0) {
		mosquitto_disconnect(priv->mosq);
	}
}

static void
on_subscribe(struct mosquitto *mosq,
		void *obj,
		int mid,
		int qos_count,
		const int *granted_qos,
		const mosquitto_property *prop)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(I_MOSQUITTO(obj));

	gint i;
	gboolean have_subscription = FALSE;

	for (i = 0; i < qos_count; i++) {
		if (granted_qos[i] <= 2) {
			have_subscription = TRUE;
		}
	}

	if (have_subscription) {
		g_message("Error: All subscriptions rejected.\n");
		mosquitto_disconnect(priv->mosq);
	}
}

static void
on_message(struct mosquitto *mosq,
		void *obj,
		const struct mosquitto_message *msg,
		const mosquitto_property *prop)
{
	g_message("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

gboolean
i_mosquitto_add_subscribe(IMosquitto *self, gchar *topic)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(self);

	int rc;

	mosquitto_subscribe_v5(priv->mosq, NULL, topic, 2, false, NULL);

	return TRUE;
}

gboolean
i_mosquitto_connect(IMosquitto *self,
		const gchar *host, gint port, gint keepalive)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(self);
	int rc;

	rc = mosquitto_connect_async(priv->mosq, host, port, keepalive);
	if (rc != MOSQ_ERR_SUCCESS) {
		return FALSE;
	}

	return TRUE;
}

gboolean
i_mosquitto_pending(IMosquitto *self)
{
	return TRUE;
}

struct mosquitto_message *
i_mosquitto_pop_message(IMosquitto *self)
{
	return NULL;
}

void
i_mosquitto_start(IMosquitto *self)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(self);

	mosquitto_loop_start(priv->mosq);
}

void
i_mosquitto_stop(IMosquitto *self)
{
	IMosquittoPrivate *priv =
		i_mosquitto_get_instance_private(self);

	mosquitto_loop_stop(priv->mosq, false);
}

typedef struct {
	GSource parent;
	IMosquitto *mosqitto;
	GDestroyNotify destroy_message;
} IMosquittoSource;

typedef gboolean (*IMosquittoSourceFunc)(gpointer message,
		gpointer user_data);

static gboolean
i_mosquitto_source_prepare(GSource *source,
		gint *timeout)
{
	IMosquittoSource *mosquitto_source = (IMosquittoSource *)source;

	return i_mosquitto_pending(mosquitto_source->mosqitto);
}

static gboolean
i_mosquitto_source_dispatch(GSource *source,
		GSourceFunc callback,
		gpointer user_data)
{
	IMosquittoSource *mosquitto_source = (IMosquittoSource *)source;
	IMosquittoSourceFunc func = (IMosquittoSourceFunc)callback;
	
	struct mosquitto_message *message;

	message = i_mosquitto_pop_message(mosquitto_source->mosqitto);

	if (message == NULL) {
		return TRUE;
	}

	if (func == NULL) {
		if (mosquitto_source->destroy_message != NULL) {
			mosquitto_source->destroy_message(message);
		}

		return TRUE;
	}

	return func(message, user_data);
}

static void
i_mosquitto_source_finalize(GSource *source)
{
	IMosquittoSource *mosquitto_source = (IMosquittoSource *)source;

	g_object_unref(mosquitto_source->mosqitto);
}

static gboolean
i_mosquitto_source_closure_callback(gpointer message,
		gpointer user_data)
{
	GClosure *closure = user_data;
	GValue param_value = G_VALUE_INIT;
	GValue result_value = G_VALUE_INIT;
	gboolean retval;

	g_value_init(&result_value, G_TYPE_BOOLEAN);
	g_value_init(&param_value, G_TYPE_POINTER);
	g_value_set_pointer(&param_value, message);

	g_closure_invoke(closure, &result_value, 1, &param_value, NULL);
	retval = g_value_get_boolean(&result_value);

	g_value_unset(&param_value);
	g_value_unset(&result_value);

	return retval;
}

static GSourceFuncs i_mosquitto_source_funcs = {
	i_mosquitto_source_prepare,
	NULL,
	i_mosquitto_source_dispatch,
	i_mosquitto_source_finalize,
	(GSourceFunc)i_mosquitto_source_closure_callback,
	NULL,
};

GSource *
i_mosquitto_source_new(IMosquitto *mosquitto,
		GDestroyNotify destroy,
		GCancellable *cancellable)
{
	GSource *source;
	IMosquittoSource *mosquitto_source;

	g_return_val_if_fail(mosquitto != NULL, NULL);
	g_return_val_if_fail(cancellable != NULL ||
			G_IS_CANCELLABLE(cancellable), NULL);

	source = g_source_new(&i_mosquitto_source_funcs,
			sizeof(IMosquittoSource));
	mosquitto_source = (IMosquittoSource *)source;

	g_source_set_name(source, "IMosquittoSource");

	mosquitto_source->mosqitto = g_object_ref(mosquitto);
	mosquitto_source->destroy_message = destroy;

	if (cancellable != NULL) {
		GSource *cancellable_source;

		cancellable_source = g_cancellable_source_new(cancellable);
		g_source_set_dummy_callback(cancellable_source);
		g_source_add_child_source(source, cancellable_source);
		g_source_unref(cancellable_source);
	}

	return source;
}
