#ifndef IDIOT_H
#define IDIOT_H

#ifdef __cpluscplus
extern "C" {
#endif

#include <idiot-mqtt.h>
#include <idiot-mqtt-source.h>
#include <idiot-subscribe.h>
#include <idiot-publish.h>

void
idiot_library_init();

void
idiot_library_exit();

#ifdef __cpluscplus
}
#endif

#endif /* end of include guard: IDIOT_H */
