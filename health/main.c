#include "health-monitor-impl.h"
#include <glib.h>
#include <gio/gio.h>

#include <dbus-health-monitor.h>

typedef struct {
	GMainLoop *loop;
	guint owner_id;
} HealthApplicationData;

static void
on_bus_acquired(GDBusConnection *connection,
		const gchar *name,
		gpointer user_data)
{
	g_message("%s(): %s", __FUNCTION__, name);
}

static void
on_name_acquired(GDBusConnection *connection,
		const gchar *name,
		gpointer user_data)
{
	HealthApplicationData *appData = user_data;
	GDBusInterfaceSkeleton *skeleton;
	GError *error = NULL;

	skeleton = health_monitor_impl_create();

	g_message("%s(): %s", __FUNCTION__, name);

	g_dbus_interface_skeleton_set_flags(skeleton,
			G_DBUS_INTERFACE_SKELETON_FLAGS_NONE);

	if (!g_dbus_interface_skeleton_export(skeleton,
			connection,
			"/net/piolink/switch/health/Monitor",
			&error)) {
		g_message("Error: %s", error->message);
	}

	health_monitor_impl_start(HEALTH_MONITOR_IMPL(skeleton), 5);
}

static void
on_name_lost(GDBusConnection *connection,
		const gchar *name,
		gpointer user_data)
{
	HealthApplicationData *appData = user_data;

	g_message("%s(): %s", __FUNCTION__, name);

	g_main_loop_quit(appData->loop);
}

int
main(int argc, char *argv[])
{
	HealthApplicationData *appData = g_new(HealthApplicationData, 1);

	appData->loop = g_main_loop_new(NULL, FALSE);
	appData->owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
			"net.piolink.switch.health",
			G_BUS_NAME_OWNER_FLAGS_NONE,
			on_bus_acquired,
			on_name_acquired,
			on_name_lost,
			appData,
			NULL);

	g_main_loop_run(appData->loop);
	g_main_loop_unref(appData->loop);
	g_free(appData);

	return EXIT_SUCCESS;
}
