#include "idiot-dbus.h"

#include "idiot-mqtt-client-impl.h"

typedef struct {
	guint owner_id;
	GHashTable *skeletons;
} IdiotDbusPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(IdiotDbus, idiot_dbus, G_TYPE_OBJECT);

static void
idiot_dbus_dispose(GObject *object)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(IDIOT_DBUS(object));

	g_print("dispose()\n");

	g_clear_pointer(&priv->skeletons, g_hash_table_unref);

	G_OBJECT_CLASS(idiot_dbus_parent_class)->dispose(object);
}

static void
idiot_dbus_finalize(GObject *object)
{
	g_print("finalize()\n");

	G_OBJECT_CLASS(idiot_dbus_parent_class)->finalize(object);
}

static void
idiot_dbus_class_init(IdiotDbusClass *klass)
{
	GObjectClass *g_object_class = G_OBJECT_CLASS(klass);

	g_object_class->dispose  = idiot_dbus_dispose;
	g_object_class->finalize = idiot_dbus_finalize;
}

static void
idiot_dbus_init(IdiotDbus *self)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(self);

	priv->skeletons = g_hash_table_new_full(g_str_hash,
				g_str_equal,
				g_free,
				g_object_unref);
}

static gboolean
export_skeleton(IdiotDbus *self, GDBusConnection *connection)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(self);
	GList *l;
	GError *error = NULL;
	GHashTableIter iter;
	gchar *object_path;
	GDBusInterfaceSkeleton *skeleton;

	g_hash_table_iter_init(&iter, priv->skeletons);

	while (g_hash_table_iter_next(&iter,
				(gpointer *)&object_path,
				(gpointer *)&skeleton)) {
		g_dbus_interface_skeleton_set_flags(skeleton,
				G_DBUS_INTERFACE_SKELETON_FLAGS_NONE);

		if (!g_dbus_interface_skeleton_export(skeleton,
					connection,
					object_path,
					&error)) {
			g_error("Error: %s", error->message);
			return FALSE;
		}

		idiot_mqtt_client_impl_export_service(IDIOT_MQTT_CLIENT_IMPL(skeleton));
	}


	return TRUE;
}

static void
unexport_skeleton(IdiotDbus *self)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(self);
	GList *l;
	GError *error = NULL;

	for (l = g_hash_table_get_values(priv->skeletons);
			l != NULL;
			l = l->next) {
		GDBusInterfaceSkeleton *skeleton = l->data;

		g_dbus_interface_skeleton_unexport(skeleton);
	}
}

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
	g_message("%s(): %s", __FUNCTION__, name);

	export_skeleton(IDIOT_DBUS(user_data), connection);
}

static void
on_name_lost(GDBusConnection *connection,
		const gchar *name,
		gpointer user_data)
{
	g_message("%s(): %s", __FUNCTION__, name);

	unexport_skeleton(IDIOT_DBUS(user_data));

	idiot_dbus_shutdown(IDIOT_DBUS(user_data));
}

void
idiot_dbus_connect(IdiotDbus *self)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(self);
	
	priv->owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
			"net.piolink.switch.idiot",
			G_BUS_NAME_OWNER_FLAGS_NONE,
			on_bus_acquired,
			on_name_acquired,
			on_name_lost,
			g_object_ref(self),
			g_object_unref);
}

void
idiot_dbus_shutdown(IdiotDbus *self)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(self);

	g_bus_unown_name(priv->owner_id);
}

void
idiot_dbus_append_skeleton(IdiotDbus *self,
		gchar *object_path,
		GDBusInterfaceSkeleton *skeleton)
{
	IdiotDbusPrivate *priv =
		idiot_dbus_get_instance_private(self);

	if (!g_hash_table_insert(priv->skeletons,
				g_strdup(object_path),
				g_object_ref(skeleton))) {
		g_error("Fail to add skeleton");
	}
}

IdiotDbus *
idiot_dbus_new(void)
{
	return g_object_new(IDIOT_TYPE_DBUS, NULL);
}

