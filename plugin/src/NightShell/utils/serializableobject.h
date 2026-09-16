#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

#include "ns_utils_shared_global.h"

namespace ns::utils {
class NS_UTILS_EXPORT SerializableObject : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString className READ className CONSTANT)

public:
  virtual ~SerializableObject() = default;
  [[nodiscard]] QObject *getRoot();
  [[nodiscard]] QString  className() const;

protected:
  explicit SerializableObject(
      const QString &className, QObject *root, QObject *parent);
  explicit SerializableObject(
      const QString &className, QObject *parent = nullptr);

  QObject *m_rootObject = nullptr;

private:
  const QString m_className;
};
} // namespace ns::utils
