bin_PROGRAMS = idiot

idiot_SOURCES = \
	main.c \
	idiot-dbus.h \
	idiot-dbus.c \
	idiot-mqtt-client-impl.c \
	idiot-mqtt-client-impl.h \
	idiot-mqtt-service.h \
	idiot-mqtt-service.c \
	idiot-health-service.h \
	idiot-health-service.c \
	idiot-devinfo-service.h \
	idiot-devinfo-service.c

idiot_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(MOSQUITTO_CFLAGS) \
	$(JANSSON_CFLAGS) \
	-I$(top_builddir)/lib \
	-I$(top_builddir)/libidiot

idiot_LDADD = \
	$(GLIB_LIBS) \
	$(MOSQUITTO_LIBS) \
	$(JANSSON_LIBS) \
	$(top_builddir)/lib/libdbus.la \
	$(top_builddir)/libidiot/libidiot.la

# D-BUS policy files
dbuspolicydir = $(sysconfdir)/dbus-1/system.d
dbuspolicy_DATA = net.piolink.switch.idiot.conf

# D-BUS service files
dbusservicedir = $(datadir)/dbus-1/system-services
dbusservice_DATA = net.piolink.switch.idiot.service

EXTRA_DIST = \
	net.piolink.switch.idiot.conf \
	net.piolink.switch.idiot.service.in

