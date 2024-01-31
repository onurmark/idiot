#include <mosquitto.h>
#include <stdbool.h>
#include <wchar.h>

#include "idiot-mqtt.h"
#include "idiot-publish.h"
#include "idiot-subscribe.h"

typedef struct {
	struct mosquitto *instance;
	gchar *client_id;

	GAsyncQueue *topic_queue;
} IdiotMqttPrivate;

G_DEFINE_TYPE_WITH_CODE(IdiotMqtt, idiot_mqtt, G_TYPE_OBJECT,
		G_ADD_PRIVATE(IdiotMqtt));

enum {
	PROP_CLIENT_ID = 1,
	N_PROPERITIES
};

static GParamSpec *obj_properties[N_PROPERITIES] = {0, };

static void
set_property(GObject *object,
		guint property_id,
		const GValue *value,
		GParamSpec *pspec)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(object));

	switch (property_id) {
		case PROP_CLIENT_ID:
			priv->client_id = g_value_dup_string(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, value);
	}
}

static void
get_property(GObject *object,
		guint property_id,
		GValue *value,
		GParamSpec *pspec)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(object));

	switch (property_id) {
		case PROP_CLIENT_ID:
			g_value_set_string(value, priv->client_id);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, value);
	}
}

static void
on_connect(struct mosquitto *mosq,
		void *obj,
		int reason_code,
		int flags,
		const mosquitto_property *prop)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(obj));

	int rc;

	g_message("on_connect: %s\n", mosquitto_connack_string(reason_code));

	if (reason_code != 0) {
		mosquitto_disconnect(priv->instance);
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
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(obj));

	gint i;
	gboolean have_subscription = FALSE;

	for (i = 0; i < qos_count; i++) {
		if (granted_qos[i] <= 2) {
			have_subscription = TRUE;
		}
	}

	if (have_subscription == false) {
		g_message("Error: All subscriptions rejected.\n");
		mosquitto_disconnect(priv->instance);
	}
}

static void
on_message(struct mosquitto *mosq,
		void *obj,
		const struct mosquitto_message *msg,
		const mosquitto_property *prop)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(obj));
	IdiotPublish publish;

	g_message("[%d] %s %d %s\n", msg->mid, msg->topic, msg->qos, (char *)msg->payload);

	publish.message_id = msg->mid;
	publish.topic = msg->topic;
	publish.payload = msg->payload;
	publish.payload_len = msg->payloadlen;
	publish.qos = msg->qos;
	publish.retain = msg->retain;

	g_async_queue_push(priv->topic_queue,
			g_boxed_copy(IDIOT_TYPE_PUBLISH, &publish));
}

static void
on_disconnect(struct mosquitto *mosq,
		void *obj,
		int reason_code,
		const mosquitto_property *props)
{
	g_message("on_disconnect: %s\n", mosquitto_connack_string(reason_code));
}

static void
idiot_mqtt_constructed(GObject *object)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(object));

	priv->instance = mosquitto_new(priv->client_id, true, IDIOT_MQTT(object));

	mosquitto_reconnect_delay_set(priv->instance, 2, 10, FALSE);

	mosquitto_connect_v5_callback_set(priv->instance, on_connect);
	mosquitto_subscribe_v5_callback_set(priv->instance, on_subscribe);
	mosquitto_message_v5_callback_set(priv->instance, on_message);
	mosquitto_disconnect_v5_callback_set(priv->instance, on_disconnect);

	G_OBJECT_CLASS(idiot_mqtt_parent_class)->constructed(object);
}

static void
idiot_mqtt_dispose(GObject *object)
{
	G_OBJECT_CLASS(idiot_mqtt_parent_class)->dispose(object);
}

static void
idiot_mqtt_finalize(GObject *object)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(object));

	mosquitto_destroy(priv->instance);

	g_free(priv->client_id);

	G_OBJECT_CLASS(idiot_mqtt_parent_class)->finalize(object);
}

static void
idiot_mqtt_class_init(IdiotMqttClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(klass);
	gchar *uuid;

	g_object_class->constructed = idiot_mqtt_constructed;
	g_object_class->dispose = idiot_mqtt_dispose;
	g_object_class->finalize = idiot_mqtt_finalize;

	g_object_class->set_property = set_property;
	g_object_class->get_property = get_property;

	/* Use random UUID */
	uuid = g_uuid_string_random();
	obj_properties[PROP_CLIENT_ID] =
		g_param_spec_string("client-id",
				"client-id",
				"client id",
				uuid,
				G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
	g_free(uuid);

	g_object_class_install_properties(g_object_class, N_PROPERITIES, obj_properties);
}

static void
idiot_mqtt_init(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	priv->topic_queue = g_async_queue_new();
}

IdiotMqtt *
idiot_mqtt_new(void)
{
	return g_object_new(IDIOT_TYPE_MQTT, NULL);
}

gboolean
idiot_mqtt_connect(IdiotMqtt *self,
		const gchar *host, gint port, gint keepalive)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	int rc;

	rc = mosquitto_connect_async(priv->instance, host, port, keepalive);
	if (rc != MOSQ_ERR_SUCCESS) {
		g_message("Fail to connect_async()");
		return FALSE;
	}

	return TRUE;
}

gboolean
idiot_mqtt_subscribe(IdiotMqtt *self, IdiotSubscribe *subscribe)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	int rc;

	rc = mosquitto_subscribe_v5(priv->instance,
			&subscribe->message_id,
			subscribe->topic_filter, 
			subscribe->qos, 
			subscribe->options, 
			subscribe->properties);
	if (rc != MOSQ_ERR_SUCCESS) {
		g_message("Fail to subscribe()");
		return FALSE;
	}

	return TRUE;
}

void
idiot_mqtt_start(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	mosquitto_loop_start(priv->instance);
}

void
idiot_mqtt_stop(IdiotMqtt *self, gboolean force)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	mosquitto_loop_stop(priv->instance, force);
}

void
idiot_mqtt_reconnect(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	mosquitto_reconnect(priv->instance);
}

gboolean
idiot_mqtt_is_pending_message(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	g_message("length: %d", g_async_queue_length(priv->topic_queue));

	return g_async_queue_length(priv->topic_queue) > 0 ? TRUE : FALSE;
}

IdiotPublish *
idiot_mqtt_dispatch_message(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	return g_async_queue_try_pop(priv->topic_queue);
}
