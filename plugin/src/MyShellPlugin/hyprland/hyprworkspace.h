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
  Q_PROPERTY(int monitorId READ default NOTIFY monitorIdChanged BINDABLE
                 bindableMonitorId)
  Q_PROPERTY(QString monitorName READ default NOTIFY monitorNameChanged BINDABLE
                 bindableMonitorName)

public:
  explicit HyprWorkspace(int      id,
                         QObject *parent = nullptr);

  [[nodiscard]] int                id() const { return m_id; }
  [[nodiscard]] QBindable<QString> bindableName() { return &b_name; }
  [[nodiscard]] QBindable<bool>    bindableIsPersistent() {
    return &b_isPersistent;
  }
  [[nodiscard]] QBindable<int>     bindableMonitorId() { return &b_monitorId; }
  [[nodiscard]] QBindable<QString> bindableMonitorName() {
    return &b_monitorName;
  }

  void updateData(common::HyprWorkspaceData data);

signals:
  void nameChanged();
  void isPersistentChanged();
  void monitorIdChanged();
  void monitorNameChanged();

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
  Q_OBJECT_BINDABLE_PROPERTY(HyprWorkspace,
                             int,
                             b_monitorId,
                             &HyprWorkspace::monitorIdChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprWorkspace,
                             QString,
                             b_monitorName,
                             &HyprWorkspace::monitorNameChanged)
};
} // namespace ns::hyprland
