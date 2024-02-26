
AM_CPPFLAGS = \
	-DG_LOG_DOMAIN=\"idiot\" \
	-DLOCALEDIR=$(localedir) \
	-I$(top_srcdir) \
	-I$(top_builddir) \
	$(GLIB_CFLAGS) \
	$(MOSQUITTO_CFLAGS) \
	$(JANSSON_CFLAGS)

libidiotincludedir = $(includedir)/libidiot-1.0/libidiot

idiot_headers = \
	idiot.h \
	idiot-types.h \
	idiot-mqtt.h \
	idiot-publish.h \
	idiot-message.h

libidiotinclude_HEADERS = \
	$(idiot_headers)

lib_LTLIBRARIES = libidiot.la

libidiot_la_LIBADD = \
	$(GLIB_LIBS) \
	$(MOSQUITTO_LIBS) \
	$(JANSSON_LIBS)

libidiot_la_SOURCES = \
	idiot.c \
	idiot-mqtt.c \
	idiot-publish.c \
	idiot-message.c

