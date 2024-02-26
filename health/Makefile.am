bin_PROGRAMS = health

health_SOURCES = \
	main.c \
	health-monitor-impl.h \
	health-monitor-impl.c

health_CFLAGS = \
	$(GLIB_CFLAGS) \
	-I$(top_builddir)/lib

health_LDADD = \
	$(GLIB_LIBS) \
	$(top_builddir)/lib/libdbus.la

# D-BUS policy files
dbuspolicydir = $(sysconfdir)/dbus-1/system.d
dbuspolicy_DATA = net.piolink.switch.health.conf

# D-BUS service files
dbusservicedir = $(datadir)/dbus-1/system-services
dbusservice_DATA = net.piolink.switch.health.service

EXTRA_DIST = \
	net.piolink.switch.health.conf \
	net.piolink.switch.health.service.in

