#pragma once

#include <memory>

#include <QObject>

namespace ns::wayland::workspaces {
class WorkspaceManager;
}

namespace ns::wayland::hyprland {
struct HyprlandDependencies {
  workspaces::WorkspaceManager *workspaceManager = nullptr;
};

class HyprlandPrivate;

class Hyprland : public QObject {
  Q_OBJECT

public:
  explicit Hyprland(const HyprlandDependencies &dependencies,
                    QObject                    *parent = nullptr);
  ~Hyprland() override;

private slots:
  void onWorkspacesChanged();

private:
  std::unique_ptr<HyprlandPrivate> d;
};
} // namespace ns::wayland::hyprland
