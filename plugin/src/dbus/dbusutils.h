#pragma once

#include <functional>

#include <qdbusabstractinterface.h>
#include <qdbusargument.h>
#include <qdbuserror.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qobject.h>

namespace ns::dbus {
template <typename T>
void asyncReadProperty(QDBusAbstractInterface            &interface,
                       const QString                     &property,
                       std::function<void(T, QDBusError)> callback) {
  auto callMessage =
      QDBusMessage::createMethodCall(interface.service(),
                                     interface.path(),
                                     "org.freedesktop.DBus.Properties",
                                     "Get");

  callMessage << interface.interface() << property;

  auto pendingCall = interface.connection().asyncCall(callMessage);
  auto callWatcher = new QDBusPendingCallWatcher(pendingCall, &interface);

  QObject::connect(callWatcher,
                   &QDBusPendingCallWatcher::finished,
                   [callback](QDBusPendingCallWatcher *w) {
                     QDBusPendingReply<QDBusVariant> reply = *w;
                     QDBusError                      error =
                         reply.isError() ? reply.error() : QDBusError();
                     T value{};
                     if (!error.isValid()) {
                       value = qdbus_cast<T>(reply.value().variant());
                     }
                     callback(value, error);
                     w->deleteLater();
                   });
}
} // namespace ns::dbus
