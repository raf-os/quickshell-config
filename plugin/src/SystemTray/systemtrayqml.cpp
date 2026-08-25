#include "systemtrayqml.h"

#include <qobject.h>

#include "statusnotifierhost.h"

namespace ns::systemtray {
SystemTrayQML::SystemTrayQML(QObject *parent) : QObject(parent) {
  auto host = StatusNotifierHost::instance();
}
} // namespace ns::systemtray
