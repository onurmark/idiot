#ifndef HEALTH_MONITOR_IMPL_H
#define HEALTH_MONITOR_IMPL_H

#include <glib-object.h>

#include <dbus-health-monitor.h>

G_BEGIN_DECLS

#define HEALTH_TYPE_MONITOR_IMPL health_monitor_impl_get_type()
G_DECLARE_DERIVABLE_TYPE(HealthMonitorImpl,
		health_monitor_impl,
		HEALTH,
		MONITOR_IMPL,
		DBusHealthMonitorSkeleton);

struct _HealthMonitorImplClass {
	DBusHealthMonitorSkeletonClass parent_class;
};

GDBusInterfaceSkeleton *
health_monitor_impl_create(void);

void
health_monitor_impl_start(HealthMonitorImpl *self, gint interval);

G_END_DECLS

#endif /* end of include guard: HEALTH_MONITOR_IMPL_H */
