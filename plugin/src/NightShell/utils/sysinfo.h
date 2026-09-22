#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "helpermacros.h"

namespace ns::utils {
class SysInfo : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML(SysInfo)

  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(QString id READ id CONSTANT)
  Q_PROPERTY(QString iconName READ iconName CONSTANT)

public:
  [[nodiscard]] QString name() const;
  [[nodiscard]] QString id() const;
  [[nodiscard]] QString iconName() const;

private:
  explicit SysInfo(QObject *parent = nullptr);

  QString m_name;
  QString m_id;
  QString m_iconName;
};
} // namespace ns::utils
