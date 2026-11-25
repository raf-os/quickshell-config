#include "shell_global.h"

namespace ns {
static ShellGlobal *s_instance = nullptr;

ShellGlobal::ShellGlobal(ns::wayland::CompositorSupport compositor,
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
    s_instance = new ShellGlobal(ns::wayland::CompositorSupport::None,
                                 static_cast<QGuiApplication *>(app), nullptr);
  }
  return s_instance;
}

QList<ScreenInfo *> ShellGlobal::screens() const {
  return m_screenMap.values();
}

ns::wayland::CompositorSupport ShellGlobal::compositor() const {
  return m_compositor;
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
