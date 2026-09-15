#pragma once

#include <qobject.h>
#include <qtmetamacros.h>
namespace ns::utils {
class SerializableObject : public QObject {
  Q_OBJECT

public:
  virtual ~SerializableObject() = default;

protected:
  explicit SerializableObject(QObject *parent = nullptr);
  QObject *m_rootObject = nullptr;
};
} // namespace ns::utils
