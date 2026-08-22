#include "statusnotifieritem.h"

#include <qdbusconnection.h>
#include <qdbusmetatype.h>
#include <qloggingcategory.h>
#include <qobject.h>

#include "dbus_item.h"
#include "dbustypes.h"

namespace ns::systemtray {
Q_LOGGING_CATEGORY(logNSStatusNotifierItem, "ns.systemtray.StatusNotifierItem")

StatusNotifierItem::StatusNotifierItem(const QString &address, QObject *parent)
    : QObject(parent), m_watcherId(address) {
  qDBusRegisterMetaType<DBusTrayIconPixmap>();
  qDBusRegisterMetaType<DBusTrayIconPixmapList>();
  qDBusRegisterMetaType<DBusTrayTooltip>();

  auto splitIdx = address.indexOf("/");
  auto conn     = splitIdx == -1 ? address : address.sliced(0, splitIdx);
  auto path = splitIdx == -1 ? "/StatusNotifierItem" : address.sliced(splitIdx);

  m_item = new QDBusStatusNotifierItem(
      conn, path, QDBusConnection::sessionBus(), this);

  if (!m_item->isValid()) {
    qCWarning(logNSStatusNotifierItem)
        << "Unable to create item for connection" << conn;
    return;
  }
}

bool StatusNotifierItem::isValid() const { return m_item->isValid(); }
} // namespace ns::systemtray
