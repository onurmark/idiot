
AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"idiot\" \
	-DLOCALEDIR=$(localedir) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(GLIB_CFLAGS) \
	$(MOSQUITTO_CFLAGS)

libidiotincludedir = $(includedir)/libidiot-1.0/libidiot

idiot_headers = \
	idiot.h \
	idiot-types.h \
	idiot-mqtt.h \
	idiot-mqtt-source.h \
	idiot-subscribe.h \
	idiot-publish.h

libidiotinclude_HEADERS = \
	$(idiot_headers)

lib_LTLIBRARIES = libidiot.la

libidiot_la_LIBADD = \
	$(GLIB_LIBS) \
	$(MOSQUITTO_LIBS)

libidiot_la_SOURCES = \
	idiot.c \
	idiot-mqtt.c \
	idiot-mqtt-source.c \
	idiot-subscribe.c \
	idiot-publish.c

