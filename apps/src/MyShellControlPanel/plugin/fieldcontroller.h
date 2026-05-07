#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace mscp {
class FieldController : public QObject {
  Q_OBJECT
  QML_ELEMENT

  Q_PROPERTY(QObject *controller READ controller WRITE setController NOTIFY
                 controllerChanged)
  Q_PROPERTY(
      QVariant value READ value WRITE setValue NOTIFY valueChanged REQUIRED)

public:
  explicit FieldController(QObject *parent = nullptr);
  ~FieldController();

  [[nodiscard]] QObject *controller() const;
  void setController(QObject *controller);

  [[nodiscard]] QVariant value() const;
  void setValue(const QVariant &value);

signals:
  void controllerChanged();
  void valueChanged();

private:
  QObject *m_controller = nullptr;
  QVariant m_value;
};
} // namespace mscp
