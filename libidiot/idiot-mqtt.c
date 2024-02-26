#include <mosquitto.h>
#include <stdbool.h>
#include <wchar.h>

#include "idiot-mqtt.h"
#include "idiot-message.h"

typedef struct {
	struct mosquitto *instance;
	gchar *client_id;
	gboolean running;
} IdiotMqttPrivate;

G_DEFINE_TYPE_WITH_CODE(IdiotMqtt, idiot_mqtt, G_TYPE_OBJECT,
		G_ADD_PRIVATE(IdiotMqtt));

enum {
	PROP_CLIENT_ID = 1,
	N_PROPERTIES
};

static GParamSpec *obj_properties[N_PROPERTIES] = {0, };

enum {
	SIGNAL_CONNECTED,
	SIGNAL_MESSAGE,
	SIGNAL_DISCONNECTED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0, };

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

	if (reason_code == 0) {
		g_signal_emit(IDIOT_MQTT(obj), signals[SIGNAL_CONNECTED], 0, NULL);
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
	int rc;
	IdiotMessage *message;

	g_message("[%d] %s %d %s\n",
			msg->mid, msg->topic, msg->qos, (char *)msg->payload);

	message = idiot_message_new(msg->mid,
			msg->topic,
			msg->payload,
			msg->payloadlen,
			msg->qos,
			msg->retain);

	g_signal_emit(IDIOT_MQTT(obj), signals[SIGNAL_MESSAGE], 0, message);
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
on_disconnect(struct mosquitto *mosq,
		void *obj,
		int reason_code,
		const mosquitto_property *props)
{
	IdiotMqtt *self = IDIOT_MQTT(obj);

	g_message("on_disconnect: %s\n", mosquitto_connack_string(reason_code));

	g_signal_emit(self, signals[SIGNAL_DISCONNECTED], 0, NULL);
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

	signals[SIGNAL_CONNECTED] =
		g_signal_new(IDIOT_MQTT_SIGNAL_CONNECTED,
				G_TYPE_FROM_CLASS(g_object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET(IdiotMqttClass, connected),
				NULL, NULL, NULL,
				G_TYPE_NONE,
				0,
				NULL);
	
	signals[SIGNAL_MESSAGE] =
		g_signal_new(IDIOT_MQTT_SIGNAL_MESSAGE,
				G_TYPE_FROM_CLASS(g_object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET(IdiotMqttClass, message),
				NULL, NULL, NULL,
				G_TYPE_NONE,
				1,
				IDIOT_TYPE_MESSAGE);

	signals[SIGNAL_DISCONNECTED] =
		g_signal_new(IDIOT_MQTT_SIGNAL_DISCONNECTED,
				G_TYPE_FROM_CLASS(g_object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET(IdiotMqttClass, disconnected),
				NULL, NULL, NULL,
				G_TYPE_NONE,
				0,
				NULL);

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

	g_object_class_install_properties(g_object_class, N_PROPERTIES, obj_properties);
}

static void
idiot_mqtt_init(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);
}

IdiotMqtt *
idiot_mqtt_new(void)
{
	return g_object_new(IDIOT_TYPE_MQTT, NULL);
}

gboolean
idiot_mqtt_subscribe(IdiotMqtt *self,
		gchar *topic_filter,
		gint qos,
		gint options)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	int rc;

	rc = mosquitto_sub_topic_check2(
			topic_filter, strlen(topic_filter));
	if (rc != MOSQ_ERR_SUCCESS) {
		g_error("Invalid topic: %s", topic_filter);
		return FALSE;
	}

	rc = mosquitto_subscribe_v5(priv->instance,
			NULL,
			topic_filter, 
			qos, 
			options,
			NULL);
	if (rc != MOSQ_ERR_SUCCESS) {
		g_message("Fail to subscribe()");
		return FALSE;
	}

	return TRUE;
}

gboolean
idiot_mqtt_publish(IdiotMqtt *self, IdiotMessage *message)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);
	int rc;

	rc = mosquitto_publish_v5(priv->instance,
			&message->id,
			message->topic,
			message->payload_len,
			message->payload,
			message->qos,
			message->retain,
			NULL);
	if (rc != MOSQ_ERR_SUCCESS) {
		g_message("Error: Fail to publish message");
		return FALSE;
	}

	return TRUE;
}

gboolean
idiot_mqtt_will(IdiotMqtt *self, IdiotMessage *message)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);
	int rc;

	rc = mosquitto_will_set_v5(priv->instance,
			message->topic,
			message->payload_len,
			message->payload,
			message->qos,
			message->retain,
			NULL);
	if (rc != MOSQ_ERR_SUCCESS) {
		g_message("Error: Fail to set will message");
		return FALSE;
	}

	return TRUE;
}

void
idiot_mqtt_reconnect(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	mosquitto_reconnect(priv->instance);
}

int
idiot_mqtt_get_socket_fd(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	return mosquitto_socket(priv->instance);
}

typedef struct {
	gchar *host;
	gint port;
	gint keepalive;
} IdiotMqttConnectData;

static void
idiot_mqtt_run_in_thread(GTask *task,
		gpointer source_object,
		gpointer task_data,
		GCancellable *cancellable)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(IDIOT_MQTT(source_object));
	IdiotMqttConnectData *data;

	int rc;

	data = g_task_get_task_data(task);

	rc = mosquitto_connect_async(priv->instance,
			data->host,
			data->port,
			data->keepalive);
	if (rc != MOSQ_ERR_SUCCESS) {
		g_message("Fail to connect_async()");
		
		g_task_return_boolean(task, FALSE);
		return;
	}

	priv->running = TRUE;

	rc = mosquitto_loop_forever(priv->instance, -1, 1);

	priv->running = FALSE;

	g_task_return_boolean(task, TRUE);
}

void
idiot_mqtt_run_async(IdiotMqtt *self,
		const gchar *host,
		gint port,
		gint keepalive,
		GCancellable *cancellable,
		GAsyncReadyCallback callback,
		gpointer user_data)
{
	GTask *task;

	IdiotMqttConnectData *data;

	data = g_new(IdiotMqttConnectData, 1);

	data->host = g_strdup(host);
	data->port = port;
	data->keepalive = keepalive;

	task = g_task_new(self, cancellable, callback, user_data);
	g_task_set_task_data(task, data, g_free);
	g_task_run_in_thread(task, idiot_mqtt_run_in_thread);
	g_object_unref(task);
}

gboolean
idiot_mqtt_run_finish(IdiotMqtt *self,
		GAsyncResult *result,
		GError **error)
{
	g_return_val_if_fail(g_task_is_valid(result, self), FALSE);

	return g_task_propagate_boolean(G_TASK(result), error);
}

void
idiot_mqtt_stop(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	mosquitto_disconnect(priv->instance);
}

gboolean
idiot_mqtt_is_running(IdiotMqtt *self)
{
	IdiotMqttPrivate *priv =
		idiot_mqtt_get_instance_private(self);

	return priv->running;
}

