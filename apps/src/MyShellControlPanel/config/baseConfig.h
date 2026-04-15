#pragma once

#include <qcontainerfwd.h>
#include <qmap.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace Config {
class BaseConfig : public QObject {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit BaseConfig(QObject *parent = nullptr);

private:
  QVariantMap m_configValues;
};
} // namespace Config
