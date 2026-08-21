#include "statusnotifierwatcher.h"

#include <qdbusmetatype.h>
#include <qobject.h>

namespace ns::systemtray {
StatusNotifierWatcher::StatusNotifierWatcher(QObject *parent)
    : QObject(parent) {}

StatusNotifierWatcher *StatusNotifierWatcher::instance() {
  static auto s_instance = new StatusNotifierWatcher();
  return s_instance;
}
} // namespace ns::systemtray
