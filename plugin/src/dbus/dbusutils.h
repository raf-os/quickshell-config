#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include <concepts>
#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qdbusargument.h>
#include <qdbuserror.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qlogging.h>
#include <qobject.h>
#include <qtdbusglobal.h>
#include <qvariant.h>

#include "dbusbindableproperty.h"

namespace ns::dbus {
template <typename T>
void asyncReadProperty(QDBusAbstractInterface &interface,
    const QString &property, std::function<void(T, QDBusError)> callback) {
  auto callMessage = QDBusMessage::createMethodCall(interface.service(),
      interface.path(), "org.freedesktop.DBus.Properties", "Get");

  callMessage << interface.interface() << property;

  auto pendingCall = interface.connection().asyncCall(callMessage);
  auto callWatcher = new QDBusPendingCallWatcher(pendingCall, &interface);

  QObject::connect(callWatcher, &QDBusPendingCallWatcher::finished,
      [callback](QDBusPendingCallWatcher *w) {
        QDBusPendingReply<QDBusVariant> reply = *w;
        QDBusError error = reply.isError() ? reply.error() : QDBusError();
        T          value{};
        if (!error.isValid()) {
          value = qdbus_cast<T>(reply.value().variant());
        }
        callback(value, error);
        w->deleteLater();
      });
}

void asyncReadPropertyInternal(const QMetaType &type,
    QDBusAbstractInterface &interface, const QString &property,
    std::function<void(std::function<QDBusError(QVariant *)>)> callback);

template <typename BindablePtr>
void asyncReadSimpleProperty(QDBusAbstractInterface *interface,
    const QString &propertyName, BindablePtr *bindable) {
  using Bindable_t = BindableType<BindablePtr>::Type;
  asyncReadPropertyInternal(QMetaType::fromType<Bindable_t>(), *interface,
      propertyName,
      [bindable](std::function<QDBusError(QVariant *)> internalCallback) {
        QVariant slot;
        auto     error = internalCallback(&slot);
        if (!error.isValid()) {
          *bindable = qdbus_cast<Bindable_t>(slot);
        }
      });
}

void asyncGetAll(QDBusAbstractInterface         &interface,
    std::function<void(QVariantMap, QDBusError)> callback);

// Code below is heavily assisted by a clanker, keep an eye on it
template <typename... Types, typename Func>
  requires std::invocable<Func, const Types &...>
void asyncCallback(
    QDBusPendingReply<Types...> &pending, Func callback, QObject *owner) {
  auto call = new QDBusPendingCallWatcher(pending, owner);

  QObject::connect(call, &QDBusPendingCallWatcher::finished, owner,
      [callback](QDBusPendingCallWatcher *call) {
        QDBusPendingReply<Types...> reply = *call;

        if (reply.isError()) {
          qDebug() << reply.error();
          call->deleteLater();
          return;
        }

        []<std::size_t... Is>(std::index_sequence<Is...>, auto &rep, auto &cb) {
          cb(rep.template argumentAt<Is>()...);
        }(std::index_sequence_for<Types...>{}, reply, callback);

        call->deleteLater();
      });
}
} // namespace ns::dbus
