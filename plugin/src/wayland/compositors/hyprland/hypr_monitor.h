#pragma once

#include <cstdint>

#include <QObject>
#include <QProperty>
#include <QVariant>

#include "iscreen_data.h"

namespace ns::wayland::hyprland {
class HyprScreenData : public QObject, public IScreenData {
  Q_INTERFACES(IScreenData)

  Q_PROPERTY(int id READ default NOTIFY idChanged BINDABLE bindableId)
  Q_PROPERTY(QString activeWorkspaceId READ default NOTIFY
                 activeWorkspaceIdChanged BINDABLE bindableActiveWorkspaceId)

public:
  ~HyprScreenData() override = default;

  enum class ExportedProps : uint8_t { Id, ActiveWorkspaceId, PROPS_MAX };

  [[nodiscard]] int      propCount() const override;
  [[nodiscard]] QVariant varAt(qsizetype idx) const override;

  [[nodiscard]] QBindable<int> bindableId() { return &b_id; }

  [[nodiscard]] QBindable<QString> bindableActiveWorkspaceId() {
    return &b_activeWorkspaceId;
  }

signals:
  void idChanged();
  void activeWorkspaceIdChanged();

private:
  Q_OBJECT_BINDABLE_PROPERTY(HyprScreenData, int, b_id,
                             &HyprScreenData::idChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprScreenData, int, b_activeWorkspaceId,
                             &HyprScreenData::activeWorkspaceIdChanged)
};
} // namespace ns::wayland::hyprland
