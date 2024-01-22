#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <mosquitto.h>
#include <string.h>

#define SERIAL_NUMBER "A12131213"

static void
on_connect(struct mosquitto *mosq,
		void *obj,
		int reason_code,
		int flags,
		const mosquitto_property *prop)
{
	char hello[50];
	int rc;

	printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
	if (reason_code != 0) {
		mosquitto_disconnect(mosq);
	}

	rc = mosquitto_subscribe(mosq, NULL, "device/" SERIAL_NUMBER "/config", 1);
	if (rc != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
		mosquitto_disconnect(mosq);
	}

	snprintf(hello, sizeof(hello), "{ \"type\": \"CS2710G\", \"version\": \"1.0.0-rc1000\"}");

	rc = mosquitto_publish(mosq,
			NULL,
			"device/" SERIAL_NUMBER "/hello",
			strlen(hello),
			hello,
			2,
			false);
	if  (rc != MOSQ_ERR_SUCCESS) {
		fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
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
	int i;
	bool have_subscription = false;

	for (i = 0; i < qos_count; i++) {
		printf("on_subscribe: %d: granted qos = %d\n", i, granted_qos[i]);

		if (granted_qos[i] <= 2) {
			have_subscription = true;
		}
	}

	if (have_subscription == false) {
		fprintf(stderr, "Error: All subscriptions rejected.\n");
		mosquitto_disconnect(mosq);
	}
}

static void
on_message(struct mosquitto *mosq,
		void *obj,
		const struct mosquitto_message *msg,
		const mosquitto_property *prop)
{
	printf("%s %d %s\n", msg->topic, msg->qos, (char *)msg->payload);
}

int
main(int argc, char *argv[])
{
	struct mosquitto *mosq;
	int rc;

	mosquitto_lib_init();

	mosq = mosquitto_new(NULL, true, NULL);

	mosquitto_connect_v5_callback_set(mosq, on_connect);
	mosquitto_subscribe_v5_callback_set(mosq, on_subscribe);
	mosquitto_message_v5_callback_set(mosq, on_message);
	mosquitto_will_set_v5(mosq, "device/" SERIAL_NUMBER "/will", 0, NULL, 1, 1, NULL);

	rc = mosquitto_connect(mosq, "192.168.228.153", 1883, 60);
	if (rc != MOSQ_ERR_SUCCESS) {
		mosquitto_destroy(mosq);
		fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
		return EXIT_FAILURE;
	}

	mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_lib_cleanup();

	return EXIT_SUCCESS;
}
