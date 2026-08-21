#include "statusnotifieritem.h"

#include <qdbusmetatype.h>
#include <qobject.h>

#include "dbustypes.h"

namespace ns::systemtray {
StatusNotifierItem::StatusNotifierItem(const QString &address, QObject *parent)
    : QObject(parent) {
  qDBusRegisterMetaType<DBusTrayIconPixmap>();
  qDBusRegisterMetaType<DBusTrayIconPixmapList>();
  qDBusRegisterMetaType<DBusTrayTooltip>();
}
} // namespace ns::systemtray
