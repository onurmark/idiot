#include "idiot.h"

#include <mosquitto.h>

void
idiot_library_init()
{
	mosquitto_lib_init();
}

void
idiot_library_exit()
{
	mosquitto_lib_cleanup();
}
