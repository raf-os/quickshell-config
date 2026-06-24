#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::ipc::server {
class IPCServer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON
};
} // namespace ns::ipc::server
