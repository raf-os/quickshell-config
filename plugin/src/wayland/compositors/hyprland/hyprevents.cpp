#include "hyprevents.h"

#include <QList>
#include <QLocalSocket>

#include "ns_hyprland_debug.h"

namespace ns::wayland::hyprland {
namespace {
static QList<QByteArrayView> split_args(QByteArrayView args) {
  QList<QByteArrayView> _args{};

  qsizetype idx = 0;
  while (idx < args.size()) {
    auto i = args.indexOf(',', idx);
    if (i == -1) break;

    _args.append(args.sliced(idx, i));
    idx += i + 1;
  }

  return _args;
}
} // namespace

class HyprEventsPrivate {
public:
  QLocalSocket socket;
};

HyprEvents::HyprEvents(const QString &socketPath, QObject *parent)
    : QObject(parent), d(std::make_unique<HyprEventsPrivate>()) {
  d->socket.setServerName(socketPath);

  QObject::connect(&d->socket, &QLocalSocket::readyRead, this,
                   &HyprEvents::onReadyRead);
  QObject::connect(&d->socket, &QLocalSocket::stateChanged, this,
                   [this](QLocalSocket::LocalSocketState state) {
                     b_isConnected = state == QLocalSocket::ConnectedState;
                   });
  QObject::connect(&d->socket, &QLocalSocket::errorOccurred, this,
                   [this](QLocalSocket::LocalSocketError error) {
                     qCWarning(NS_LOG_HYPRLAND) << "Hyprland socket2 error:"
                                                << d->socket.errorString();
                   });
}

HyprEvents::~HyprEvents() = default;

QBindable<bool> HyprEvents::bindableIsConnected() const {
  return &b_isConnected;
}

bool HyprEvents::isConnected() const { return b_isConnected.value(); }

void HyprEvents::connect() {
  if (isConnected() || d->socket.serverName().isEmpty()) return;

  d->socket.connectToServer(QLocalSocket::ReadOnly);
}

void HyprEvents::disconnect() {
  if (!isConnected()) return;

  d->socket.disconnectFromServer();
}

void HyprEvents::onReadyRead() {
  m_buffer.append(d->socket.readAll());
  parseBuffer();
}

void HyprEvents::parseBuffer() {
  while (m_buffer.contains('\n')) {
    const auto     newLine = m_buffer.indexOf('\n');
    QByteArrayView bv      = m_buffer;

    bv.slice(newLine);
    bv = bv.trimmed();

    if (bv.isEmpty()) {
      m_buffer.slice(newLine + 1);
      continue;
    }

    const auto separatorIdx = bv.indexOf(">>");
    if (separatorIdx == -1) {
      m_buffer.slice(newLine + 1);
      continue;
    }

    QByteArrayView event = bv.sliced(separatorIdx).trimmed();
    QByteArrayView data  = bv.sliced(separatorIdx + 2).trimmed();

    dispatchEvent(event, data);
    m_buffer.slice(newLine + 1);
  }
}

void HyprEvents::dispatchEvent(QByteArrayView event, QByteArrayView data) {
  if (event == "configreloaded") {
    emit configReloaded();
  } else if (event == "bell") {
    emit bellRang();
  } else if (event == "activelayout") {
    const auto separatorIdx = data.indexOf(',');
    if (separatorIdx == -1) {
      emit keyboardLayoutChanged({}, {});
      return;
    }

    QString kbdName    = data.sliced(0, separatorIdx).toByteArray();
    QString layoutName = data.sliced(separatorIdx + 1).toByteArray();

    emit keyboardLayoutChanged(kbdName, layoutName);
  } else if (event == "activewindowv2") {
    bool isOk = false;
    auto addr = data.toULongLong(&isOk, 16);
    if (isOk) {
      emit activeWindowChanged(addr);
    }
  } else if (event == "createworkspace") {
    emit workspaceCreated();
  } else if (event == "destroyworkspace") {
    emit workspaceDestroyed(data.toByteArray());
  } else if (event == "moveworkspace") {
    auto separatorIdx = data.indexOf(',');
    if (separatorIdx == -1) return;

    auto    addr  = data.sliced(0, separatorIdx).toByteArray();
    QString mName = data.sliced(separatorIdx).toByteArray();

    emit workspaceMoved(addr, mName);
  } else if (event == "renameworkspace") {
    auto args = split_args(data);
    if (args.size() != 2) return;

    auto    addr = args.at(0).toByteArray();
    QString name = args.at(1).toByteArray();

    emit workspaceRenamed(addr, name);
  } else if (event == "workspace") {
    emit userWorkspaceChanged(data.toByteArray());
  } else if (event == "movewindow") {
    auto args = split_args(data);

    if (args.size() != 2) return;

    bool isOk    = false;
    auto winAddr = args.at(0).toULongLong(&isOk, 16);

    if (!isOk) return;

    auto wpAddr = args.at(1).toByteArray();
    emit windowMoved(winAddr, wpAddr);
  } else if (event == "focusedmon") {
    auto args = split_args(data);

    if (args.size() != 2) return;

    QString mName = args.at(0).toByteArray();
    auto    wAddr = args.at(1).toByteArray();

    emit monitorFocused(mName, wAddr);
  } else if (event == "fullscreen") {
    emit windowFullscreen();
  } else if (event == "urgent") {
    bool ok   = false;
    auto addr = data.toULongLong(&ok, 16);

    if (!ok) return;

    emit windowUrgent(addr);
  } else if (event == "openwindow") {
    auto args = split_args(data);

    if (args.size() < 1) return;

    bool ok      = false;
    bool winAddr = args.at(0).toULongLong(&ok, 16);

    if (!ok) return;

    auto wpAddr = args.at(1).toByteArray();

    emit windowOpened(winAddr, wpAddr);
  }
}
} // namespace ns::wayland::hyprland
