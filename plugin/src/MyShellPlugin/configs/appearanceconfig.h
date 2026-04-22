#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "propertymacros.h"

namespace myqmlplugin {
namespace configs {
class AppearanceConfig : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

public:
  explicit AppearanceConfig(QObject *parent = nullptr);
};
} // namespace configs
} // namespace myqmlplugin
