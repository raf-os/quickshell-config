#pragma once

#include <QObject>
#include <QProperty>
#include <QtTypes>

#include "iworkspace_data.h"

namespace ns::wayland::hyprland {
class HyprWorkspaceData : public QObject, public IWorkspaceData {
  Q_INTERFACES(IWorkspaceData)

  Q_PROPERTY(int id READ default NOTIFY idChanged BINDABLE bindableId)
  Q_PROPERTY(int monitorId READ default NOTIFY monitorIdChanged BINDABLE
                 bindableMonitorId)
  Q_PROPERTY(QString address READ default NOTIFY addressChanged BINDABLE
                 bindableAddress)
  Q_PROPERTY(QString name READ default NOTIFY nameChanged BINDABLE bindableName)
  Q_PROPERTY(quint64 lastWindowAddress READ default NOTIFY
                 lastWindowAddressChanged BINDABLE bindableLastWindowAddress)

public:
  ~HyprWorkspaceData() override = default;

  enum class ExportedProps : uint8_t {
    Id,
    MonitorId,
    Address,
    Name,
    LastWindowAddress,
    PROPS_MAX
  };

  [[nodiscard]] int      propCount() const override;
  [[nodiscard]] QVariant varAt(qsizetype idx) const override;

  [[nodiscard]] QBindable<int>     bindableId() const;
  [[nodiscard]] QBindable<int>     bindableMonitorId() const;
  [[nodiscard]] QBindable<QString> bindableAddress() const;
  [[nodiscard]] QBindable<QString> bindableName() const;
  [[nodiscard]] QBindable<quint64> bindableLastWindowAddress() const;

signals:
  void idChanged();
  void monitorIdChanged();
  void addressChanged();
  void nameChanged();
  void lastWindowAddressChanged();

private:
#define BPROP(Type, Name)                                                      \
  Q_OBJECT_BINDABLE_PROPERTY(HyprWorkspaceData, Type, b_##Name,                \
                             &HyprWorkspaceData::Name##Changed)

  BPROP(int, id)
  BPROP(int, monitorId)
  BPROP(QString, address)
  BPROP(QString, name)
  BPROP(quint64, lastWindowAddress)
#undef BPROP
};
} // namespace ns::wayland::hyprland
