#ifndef I_MOSQUITTO_H_
#define I_MOSQUITTO_H_

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define I_TYPE_MOSQUITTO i_mosquitto_get_type()
G_DECLARE_DERIVABLE_TYPE(IMosquitto, i_mosquitto, I, MOSQUITTO, GObject);

struct _IMosquittoClass {
	GObjectClass parent_class;
};

gboolean
i_mosquitto_add_subscribe(IMosquitto *self, gint *mid, gchar *topic);

gboolean
i_mosquitto_connect(IMosquitto *self,
		const gchar *host, gint port, gint keepalive);

void
i_mosquitto_start(IMosquitto *self);

void
i_mosquitto_stop(IMosquitto *self);

IMosquitto *
i_mosquitto_new(void);

GSource *
i_mosquitto_source_new(IMosquitto *mosquitto,
		GDestroyNotify destroy,
		GCancellable *cancellable);

G_END_DECLS

#endif /* ifndef I_MOSQUITTO_H_ */

