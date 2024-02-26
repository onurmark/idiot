mqtt_client_interfaces= net.piolink.switch.idiot.xml
mqtt_client_sources = \
	dbus-mqtt-client.c \
	dbus-mqtt-client.h

$(mqtt_client_sources): $(mqtt_client_interfaces)
	$(AM_V_GEN) $(GDBUS_CODEGEN) \
		--interface-prefix=net.piolink.switch.idiot \
		--c-namespace=DBus \
		--generate-c-code $* $<

NO_DIST_BUILD_SOURCES = $(mqtt_client_sources)

health_interfaces = net.piolink.switch.health.xml
health_sources = \
	dbus-health-monitor.c \
	dbus-health-monitor.h

$(health_sources): $(health_interfaces)
	$(AM_V_GEN) $(GDBUS_CODEGEN) \
		--interface-prefix=net.piolink.switch \
		--c-namespace=DBus \
		--generate-c-code $* $<

NO_DIST_BUILD_SOURCES += $(health_sources)

noinst_LTLIBRARIES = libdbus.la

libdbus_la_CFLAGS = \
	$(GLIB_CFLAGS)

libdbus_la_LIBADD = \
	$(GLIB_LIBS)

nodist_libdbus_la_SOURCES = \
	$(NO_DIST_BUILD_SOURCES)

CLEANFILES = $(NO_DIST_BUILD_SOURCES)

EXTRA_DIST = \
	$(mqtt_client_interfaces) \
	$(health_interfaces)

