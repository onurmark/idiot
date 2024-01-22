#ifndef I_MOSQUITTO_H_
#define I_MOSQUITTO_H_

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

G_DECLARE_DERIVABLE_TYPE(IMosquitto, i_mosquitto, I, MOSQUITTO, GObject);

struct _IMosquittoClass {
	GObjectClass parent_class;
};

G_END_DECLS

#endif /* ifndef I_MOSQUITTO_H_ */

