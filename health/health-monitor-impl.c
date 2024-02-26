#include "health-monitor-impl.h"
#include "dbus-health-monitor.h"

static void
health_monitor_impl_iface_init(DBusHealthMonitorIface *iface);

G_DEFINE_TYPE_WITH_CODE(HealthMonitorImpl,
		health_monitor_impl,
		DBUS_TYPE_HEALTH_MONITOR_SKELETON,
		G_IMPLEMENT_INTERFACE(DBUS_TYPE_HEALTH_MONITOR,
			health_monitor_impl_iface_init));

static void
health_monitor_impl_iface_init(DBusHealthMonitorIface *iface)
{
}

static void
health_monitor_impl_class_init(HealthMonitorImplClass *klass)
{

}

static void
health_monitor_impl_init(HealthMonitorImpl *self)
{

}

gboolean
perioc_report(gpointer user_data)
{
	HealthMonitorImpl *self = user_data;

	dbus_health_monitor_emit_cpu_usage(DBUS_HEALTH_MONITOR(self),
			g_random_double_range(0, 100));

	return TRUE;
}

void
health_monitor_impl_start(HealthMonitorImpl *self, gint interval)
{
	g_timeout_add_seconds(interval, (GSourceFunc)perioc_report, self);
}

GDBusInterfaceSkeleton *
health_monitor_impl_create(void)
{
	return g_object_new(HEALTH_TYPE_MONITOR_IMPL, NULL);
}
