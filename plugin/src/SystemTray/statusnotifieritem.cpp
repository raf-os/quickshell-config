#include "statusnotifieritem.h"

#include <qdbusconnection.h>
#include <qdbuserror.h>
#include <qdbusmetatype.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qproperty.h>

#include "dbus_item.h"

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

  QObject::connect(m_item, &QDBusStatusNotifierItem::NewTitle, this, [this]() {
    b_title = m_item->title();
  });

  QObject::connect(m_item,
                   &QDBusStatusNotifierItem::NewStatus,
                   this,
                   [this](const QString &status) { b_status = status; });

  readAllParameters();
}

void StatusNotifierItem::readAllParameters() {
  if (m_item == nullptr) return;

  QScopedPropertyUpdateGroup scopeGuard;
  b_id            = m_item->id();
  b_title         = m_item->title();
  b_status        = m_item->status();
  b_category      = m_item->category();
  b_windowId      = m_item->windowId();
  b_iconThemePath = m_item->iconThemePath();
  b_iconName      = m_item->iconName();
  b_menuPath      = m_item->menu();
}

bool StatusNotifierItem::isValid() const { return m_item->isValid(); }
bool StatusNotifierItem::isReady() const { return m_isReady; }
} // namespace ns::systemtray
