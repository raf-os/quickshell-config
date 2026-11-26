#pragma once

#include <bitset>
#include <cstdint>
#include <memory>

#include <QObject>
#include <QProperty>
#include <QString>
#include <QTimer>
#include <QVariantMap>
#include <QtQmlIntegration>

#include "hypr_configs.h"
#include "hyprctl.h"

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
  QML_ELEMENT
  QML_SINGLETON
  QML_UNCREATABLE("Singleton must be provided by C++")

  Q_PROPERTY(bool keyboardSwitchOnCooldown READ keyboardSwitchOnCooldown NOTIFY
                 keyboardSwitchOnCooldownChanged)

public:
  explicit Hyprland(const HyprlandDependencies &dependencies,
                    QObject                    *parent = nullptr);
  ~Hyprland() override;

  enum class PendingCall : uint8_t {
    SwitchKeyboardLayout,
    RequestDevices,
    RequestClients,
    RequestWorkspaces,
    RequestMonitors,
    PENDING_CALL_MAX
  };

  [[nodiscard]] bool keyboardSwitchOnCooldown() const;

  Q_INVOKABLE void applyOptions(const QVariantMap &options);

public slots:
  void switchKeyboardLayout(const QString &cmd,
                            const QString &device = "current");

signals:
  void keyboardSwitchOnCooldownChanged();

private slots:
  void onWorkspacesChanged();

private:
  using pending_bitset_t =
      std::bitset<static_cast<size_t>(PendingCall::PENDING_CALL_MAX)>;

  [[nodiscard]] bool isPending(PendingCall value);
  void               setPending(PendingCall value, bool on = true);

  std::unique_ptr<HyprlandPrivate> d;

  QString m_socketPath;
  QTimer  m_kbdCooldownTimer;

  Hyprctl     *m_hyprctl     = nullptr;
  HyprConfigs *m_hyprConfigs = nullptr;

  pending_bitset_t m_pendingCalls;
  pending_bitset_t m_queuedCalls;
};
} // namespace ns::wayland::hyprland
