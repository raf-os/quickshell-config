#include "dbusutils.h"

#include <functional>

#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qdbuserror.h>
#include <qdbusmessage.h>
#include <qdbusmetatype.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtdbusglobal.h>
#include <qvariant.h>

namespace ns::dbus {
void asyncReadPropertyInternal(const QMetaType &type,
    QDBusAbstractInterface &interface, const QString &property,
    std::function<void(std::function<QDBusError(QVariant *)>)> callback) {
  if (type.id() != QMetaType::QVariant) {
    const char *expectedSignature = QDBusMetaType::typeToSignature(type);
    if (expectedSignature == nullptr) {
      qFatal() << "ns::dbus::asyncReadPropertyInternal: called with "
                  "unregistered dbus meta-type"
               << type;
    }
  }

  auto msg = QDBusMessage::createMethodCall(interface.service(),
      interface.path(), "org.freedesktop.DBus.Properties", "Get");

  msg << interface.interface() << property;
  auto pending = interface.connection().asyncCall(msg);
  auto call    = new QDBusPendingCallWatcher(pending, &interface);

  QObject::connect(call, &QDBusPendingCallWatcher::finished, &interface,
      [type, callback](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QDBusVariant> reply = *w;

        callback([&](QVariant *slot) {
          if (reply.isError()) return reply.error();
          else {
            *slot = reply.value().variant();
            return QDBusError();
          }
        });

        delete w;
      });
}

void asyncGetAll(QDBusAbstractInterface         &interface,
    std::function<void(QVariantMap, QDBusError)> callback) {
  auto callMessage = QDBusMessage::createMethodCall(interface.service(),
      interface.path(), "org.freedesktop.DBus.Properties", "GetAll");

  auto pendingCall = interface.connection().asyncCall(callMessage);
  auto callWatcher = new QDBusPendingCallWatcher(pendingCall, &interface);

  QObject::connect(callWatcher, &QDBusPendingCallWatcher::finished,
      [callback](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QDBusVariant> reply = *w;
        QDBusError  error = reply.isError() ? reply.error() : QDBusError();
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
