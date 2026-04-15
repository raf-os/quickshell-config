#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Config {
class ConfigHandler : public QObject {
  Q_OBJECT
  QML_SINGLETON

public:
  explicit ConfigHandler(QObject *parent = nullptr);
};
} // namespace Config
