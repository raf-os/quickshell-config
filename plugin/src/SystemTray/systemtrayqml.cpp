#include "systemtrayqml.h"

#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>

#include "objectmodel.h"
#include "statusnotifierhost.h"
#include "statusnotifieritem.h"

namespace ns::systemtray {
Q_LOGGING_CATEGORY(logSystemTrayQML, "ns.systemtray.qml")

SystemTrayQML::SystemTrayQML(QObject *parent) : QObject(parent) {
  auto host = StatusNotifierHost::instance();

  QObject::connect(host, &StatusNotifierHost::itemReady, this,
      &SystemTrayQML::onItemRegistered);
  QObject::connect(host, &StatusNotifierHost::itemUnregistered, this,
      &SystemTrayQML::onItemUnregistered);

  for (auto *item : host->items()) {
    m_items.insertObjectSorted(item, &SystemTrayQML::sortCompare);
  }
}

bool SystemTrayQML::sortCompare(StatusNotifierItem *a, StatusNotifierItem *b) {
  return a->bindableCategory().value() < b->bindableCategory().value() ||
         a->bindableId().value().compare(
             b->bindableId().value(), Qt::CaseInsensitive) >= 0;
}

void SystemTrayQML::onItemRegistered(StatusNotifierItem *item) {
  m_items.insertObjectSorted(item, &SystemTrayQML::sortCompare);
}
void SystemTrayQML::onItemUnregistered(StatusNotifierItem *item) {
  m_items.removeObject(item);
}

ObjectModel<StatusNotifierItem> *SystemTrayQML::items() { return &m_items; }
} // namespace ns::systemtray
