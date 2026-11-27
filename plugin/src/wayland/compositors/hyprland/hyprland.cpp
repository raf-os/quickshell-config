#include "hyprland.h"

#include <algorithm>
#include <chrono>

#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QList>
#include <QLoggingCategory>
#include <QtEnvironmentVariables>

#include "hypr_monitor.h"
#include "hyprland_p.h"
#include "ns_hyprland_debug.h"
#include "screen_info.h"

namespace ns::wayland::hyprland {
namespace {
static constexpr std::chrono::milliseconds KEYBOARD_SWITCH_COOLDOWN{250};
}

Hyprland::Hyprland(const HyprlandDependencies &dependencies, QObject *parent)
    : QObject(parent), d(std::make_unique<HyprlandPrivate>()) {
  d->workspaceManager = dependencies.workspaceManager;
  d->shellGlobal      = dependencies.shellGlobal;

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

  m_socketPath = std::move(hyprDir);

  m_hyprctl = new Hyprctl(m_socketPath + "/.socket.sock", this);

  m_events = new HyprEvents(m_socketPath + "/.socket2.sock", this);
  m_events->connect();

  m_hyprConfigs = new HyprConfigs(
      m_hyprctl, dependencies.keyboardLayoutXMLSerializer, this);
  m_hyprConfigs->queryConfigs();

  queryMonitors();

  QObject::connect(m_events, &HyprEvents::configReloaded, m_hyprConfigs,
                   &HyprConfigs::queryConfigs);

  QObject::connect(m_events, &HyprEvents::keyboardLayoutChanged, m_hyprConfigs,
                   &HyprConfigs::onActiveLayoutChanged);

  if (d->shellGlobal) {
    QObject::connect(d->shellGlobal, &ShellGlobal::screensChanged, this,
                     &Hyprland::queryMonitors);
  }
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

void Hyprland::queryMonitors() {
  if (!m_hyprctl || d->shellGlobal) return;

  if (isPending(PendingCall::RequestMonitors)) {
    m_queuedCalls.set(static_cast<size_t>(PendingCall::RequestMonitors), true);
    return;
  }

  setPending(PendingCall::RequestMonitors, true);

  auto cb = [this](bool success, const QByteArray &data) {
    this->setPending(PendingCall::RequestMonitors, false);

    if (!success) {
      return;
    }

    QJsonParseError error;
    auto            doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
      qCWarning(NS_LOG_HYPRLAND)
          << "Error parsing j/devices:" << error.errorString();
      return;
    }

    if (!doc.isArray()) {
      qCWarning(NS_LOG_HYPRLAND)
          << "Invalid json data received from j/devices.";
      return;
    }

    auto rootArray = doc.array();
    auto screens   = d->shellGlobal->screens();

    for (auto mon : rootArray) {
      if (!mon.isObject()) continue;

      auto monitor = mon.toObject();
      auto serial  = monitor.value("serial").toString();

      if (serial.isEmpty()) continue;

      auto it = std::ranges::find_if(screens.begin(), screens.end(),
                                     [&serial](ScreenInfo *screenInfo) {
                                       return screenInfo->serial() == serial;
                                     });

      if (it == screens.end()) {
        qCDebug(NS_LOG_HYPRLAND)
            << "Unable to match screen with serial" << serial;
        continue;
      }

      auto sData = (*it)->getData<HyprScreenData>();
      if (!d) {
        qCFatal(NS_LOG_HYPRLAND)
            << "Unable to cast screen data to HyprScreenData!";
      }

      auto mId = monitor.value("id").toInteger();
      sData->bindableId().setValue(static_cast<int>(mId));
    }

    if (m_queuedCalls.test(static_cast<size_t>(PendingCall::RequestDevices))) {
      m_queuedCalls.set(static_cast<size_t>(PendingCall::RequestDevices),
                        false);
      this->queryMonitors();
    }
  };
}

[[nodiscard]] bool Hyprland::isPending(PendingCall value) {
  return m_pendingCalls.test(static_cast<size_t>(value));
}

void Hyprland::setPending(PendingCall value, bool on) {
  m_pendingCalls.set(static_cast<size_t>(value), on);
}
} // namespace ns::wayland::hyprland
