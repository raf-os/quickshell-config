#pragma once

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "hyprdefs.h"

namespace ns::hyprland {
class HyprWorkspace : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(int id READ id CONSTANT)
  Q_PROPERTY(QString name READ default NOTIFY nameChanged BINDABLE bindableName)
  Q_PROPERTY(bool isPersistent READ default NOTIFY isPersistentChanged BINDABLE
                 bindableIsPersistent)

public:
  explicit HyprWorkspace(int      id,
                         QObject *parent = nullptr);

  [[nodiscard]] int                id() const { return m_id; }
  [[nodiscard]] QBindable<QString> bindableName() { return &b_name; }
  [[nodiscard]] QBindable<bool>    bindableIsPersistent() {
    return &b_isPersistent;
  }

  void updateData(common::HyprWorkspaceData data);

signals:
  void nameChanged();
  void isPersistentChanged();

private:
  int m_id;

  Q_OBJECT_BINDABLE_PROPERTY(HyprWorkspace,
                             QString,
                             b_name,
                             &HyprWorkspace::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprWorkspace,
                             bool,
                             b_isPersistent,
                             &HyprWorkspace::isPersistentChanged)
};
} // namespace ns::hyprland
