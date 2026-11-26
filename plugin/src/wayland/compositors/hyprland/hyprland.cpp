#include "hyprland.h"

#include <chrono>

#include <QFileInfo>
#include <QLoggingCategory>
#include <QtEnvironmentVariables>

#include "hyprland_p.h"
#include "ns_hyprland_debug.h"

namespace ns::wayland::hyprland {
namespace {
static constexpr std::chrono::milliseconds KEYBOARD_SWITCH_COOLDOWN{250};
}

Hyprland::Hyprland(const HyprlandDependencies &dependencies, QObject *parent)
    : QObject(parent), d(std::make_unique<HyprlandPrivate>()) {
  d->workspaceManager = dependencies.workspaceManager;

  m_kbdCooldownTimer.setInterval(KEYBOARD_SWITCH_COOLDOWN);
  m_kbdCooldownTimer.setSingleShot(true);

  QObject::connect(&m_kbdCooldownTimer, &QTimer::timeout, this,
                   &Hyprland::keyboardSwitchOnCooldownChanged);

  auto his = qEnvironmentVariable("HYPRLAND_INSTANCE_SIGNATURE");
  if (his.isEmpty()) {
    qCWarning(NS_LOG_HYPRLAND) << "Unable to connect to hyprland's sockets as "
                                  "HYPRLAND_INSTANCE_SIGNATURE is unset.";
    return;
  }

  auto runDir  = qEnvironmentVariable("XDG_RUNTIME_DIR");
  auto hyprDir = runDir + "/hypr/" + his;

  if (!QFileInfo(hyprDir).isDir()) {
    hyprDir = "/tmp/hypr/" + his;
  }
  if (!QFileInfo(hyprDir).isDir()) {
    qCWarning(NS_LOG_HYPRLAND) << "Unable to locate hyprland's sockets. "
                                  "Hyprland integration will not work.";
    return;
  }

  m_socketPath = hyprDir + "/.socket.sock";

  m_hyprctl = new Hyprctl(m_socketPath, this);

  m_hyprConfigs = new HyprConfigs(m_hyprctl, nullptr, this);
}

Hyprland::~Hyprland() = default;

bool Hyprland::keyboardSwitchOnCooldown() const {
  return m_kbdCooldownTimer.isActive();
}

void Hyprland::applyOptions(const QVariantMap &options) {
  if (!m_hyprctl || options.isEmpty()) return;

  QString req;
  req.reserve(1024);

  std::function<void(const QVariantMap &)> parseNested =
      [&](const QVariantMap &map) {
        req += "{";
        for (auto it = map.cbegin(); it != map.cend(); ++it) {
          QString thisOption;
          thisOption.reserve(128);

          auto parts = it.key().split(':');
          req += parts.join(" = {") + " = ";

          if (it.value().canConvert<QVariantMap>()) {
            parseNested(it.value().toMap());
          } else {
            req += it.value().toString();
          }

          req += QString("}").repeated(parts.size() - 1);

          if (std::distance(it, map.cend()) > 1) {
            req += ", ";
          }
        }
        req += "}";
      };

  parseNested(options);

  QString request = QString("eval hl.config(%1)").arg(req);

  m_hyprctl->call(
      this, request.toUtf8(), [](bool success, const QByteArray &res) {
        if (!success) {
          qCWarning(NS_LOG_HYPRLAND) << "Error applying hyprland options:\n"
                                     << res;
        } else {
          qCDebug(NS_LOG_HYPRLAND) << res;
        }
      });
}

void Hyprland::switchKeyboardLayout(const QString &cmd, const QString &device) {
  if (!m_hyprctl || m_kbdCooldownTimer.isActive() ||
      isPending(PendingCall::SwitchKeyboardLayout))
    return;

  setPending(PendingCall::SwitchKeyboardLayout);
  m_kbdCooldownTimer.start();

  const auto command = QString("switchxkblayout %1 %2").arg(device).arg(cmd);

  m_hyprctl->call(this, command.toUtf8(), [this](auto, const auto &) {
    this->setPending(PendingCall::SwitchKeyboardLayout, false);
  });
}

void Hyprland::onWorkspacesChanged() {}

[[nodiscard]] bool Hyprland::isPending(PendingCall value) {
  return m_pendingCalls.test(static_cast<size_t>(value));
}

void Hyprland::setPending(PendingCall value, bool on) {
  m_pendingCalls.set(static_cast<size_t>(value), on);
}
} // namespace ns::wayland::hyprland
