#include "shell_global.h"

#include "workspace_manager.h"

namespace ns {
static ShellGlobal *s_instance = nullptr;

ShellGlobal::ShellGlobal(ns::wayland::CompositorData compositor,
                         QGuiApplication *app, QObject *parent)
    : QObject(parent), m_app(app), m_compositor(compositor) {
  if (s_instance) {
    qWarning() << "Attempted to create a ShellGlobal instance, yet one already "
                  "exists.";
    this->deleteLater();
    return;
  }

  s_instance = this;
  for (auto screen : app->screens()) {
    auto sInfo = new ScreenInfo(screen);
    QObject::connect(sInfo, &QObject::destroyed, this,
                     &ShellGlobal::onScreenDestroyed);
    m_screenMap.insert(screen, sInfo);
  }

  QObject::connect(app, &QGuiApplication::screenAdded, this,
                   &ShellGlobal::onScreenAdded);
  QObject::connect(app, &QGuiApplication::screenRemoved, this,
                   &ShellGlobal::onScreenRemoved);
}

ShellGlobal::~ShellGlobal() { s_instance = nullptr; }

ShellGlobal *ShellGlobal::instance() {
  if (!s_instance) {
    auto *app  = QGuiApplication::instance();
    s_instance = new ShellGlobal(ns::wayland::CompositorData(),
                                 static_cast<QGuiApplication *>(app), nullptr);
  }
  return s_instance;
}

QList<ScreenInfo *> ShellGlobal::screens() const {
  return m_screenMap.values();
}

ns::wayland::CompositorData *ShellGlobal::compositor() { return &m_compositor; }

ns::wayland::workspaces::WorkspaceManager *ShellGlobal::workspaceManager() {
  if (!m_workspaceManager) {
    m_workspaceManager =
        new ns::wayland::workspaces::WorkspaceManager(&m_compositor, this);
  }
  return m_workspaceManager;
}

void ShellGlobal::onScreenAdded(QScreen *screen) {
  if (!screen) return;

  if (!m_screenMap.contains(screen)) {
    auto sInfo = new ScreenInfo(screen);
    m_screenMap.insert(screen, sInfo);
    QObject::connect(sInfo, &QObject::destroyed, this,
                     &ShellGlobal::onScreenDestroyed);

    emit screensChanged();
  }
}

void ShellGlobal::onScreenRemoved(QScreen *screen) {
  auto it = m_screenMap.find(screen);
  if (it == m_screenMap.end()) return;

  auto sInfo = it.value();
  QObject::disconnect(sInfo, &QObject::destroyed, this,
                      &ShellGlobal::onScreenDestroyed);
  m_screenMap.erase(it);

  emit screensChanged();
}

void ShellGlobal::onScreenDestroyed() {
  auto screen = static_cast<ScreenInfo *>(sender());

  if (screen) onScreenRemoved(screen->screen());
}
} // namespace ns
