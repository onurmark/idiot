#ifndef IDIOT_DBUS_H
#define IDIOT_DBUS_H

#include <glib-object.h>

#include <gio/gio.h>

#define IDIOT_TYPE_DBUS idiot_dbus_get_type()
G_DECLARE_DERIVABLE_TYPE(IdiotDbus, idiot_dbus, IDIOT, DBUS, GObject);

struct _IdiotDbusClass {
	GObjectClass parent_class;
};

IdiotDbus *
idiot_dbus_new(void);

void
idiot_dbus_connect(IdiotDbus *self);

void
idiot_dbus_shutdown(IdiotDbus *self);

void
idiot_dbus_append_skeleton(IdiotDbus *self,
		gchar *object_path,
		GDBusInterfaceSkeleton *skeleton);

#endif /* end of include guard: IDIOT_DBUS_H */
