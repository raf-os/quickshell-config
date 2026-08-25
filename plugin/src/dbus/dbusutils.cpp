#include "dbusutils.h"

#include <qcontainerfwd.h>
#include <qdbuserror.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qobject.h>

namespace ns::dbus {
void asyncGetAll(QDBusAbstractInterface                      &interface,
                 std::function<void(QVariantMap, QDBusError)> callback) {
  auto callMessage =
      QDBusMessage::createMethodCall(interface.service(),
                                     interface.path(),
                                     "org.freedesktop.DBus.Properties",
                                     "GetAll");

  auto pendingCall = interface.connection().asyncCall(callMessage);
  auto callWatcher = new QDBusPendingCallWatcher(pendingCall, &interface);

  QObject::connect(callWatcher,
                   &QDBusPendingCallWatcher::finished,
                   [callback](QDBusPendingCallWatcher *w) {
                     QDBusPendingReply<QDBusVariant> reply = *w;
                     QDBusError                      error =
                         reply.isError() ? reply.error() : QDBusError();
                     QVariantMap value;
                     if (!error.isValid()) {
                       auto variant = reply.value().variant();
                       if (variant.canConvert<QVariantMap>()) {
                         value = variant.toMap();
                       }
                     }
                     callback(value, error);
                     w->deleteLater();
                   });
}
} // namespace ns::dbus
