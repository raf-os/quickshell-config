#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::systemtray {
class SystemTrayQML : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit SystemTrayQML(QObject *parent = nullptr);
};
} // namespace ns::systemtray
