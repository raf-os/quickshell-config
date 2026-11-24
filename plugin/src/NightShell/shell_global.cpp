#include "shell_global.h"

namespace ns {
static ShellGlobal *s_instance = nullptr;

ShellGlobal::ShellGlobal(QGuiApplication *app, QObject *parent)
    : QObject(parent), m_app(app) {
  if (s_instance) {
    qWarning() << "Attempted to create a ShellGlobal instance, yet one already "
                  "exists.";
    this->deleteLater();
    return;
  }

  s_instance = this;

  QObject::connect(app, &QGuiApplication::screenAdded, this,
                   &ShellGlobal::onScreenAdded);
  QObject::connect(app, &QGuiApplication::screenRemoved, this,
                   &ShellGlobal::onScreenRemoved);
}

ShellGlobal::~ShellGlobal() { s_instance = nullptr; }

ShellGlobal *ShellGlobal::instance() {
  if (!s_instance) {
    auto *app  = QGuiApplication::instance();
    s_instance = new ShellGlobal(static_cast<QGuiApplication *>(app), nullptr);
  }
  return s_instance;
}

QList<QScreen *> ShellGlobal::screens() const { return m_screens; }

void ShellGlobal::onScreenAdded(QScreen *screen) {
  if (!screen) return;

  if (!m_screens.contains(screen)) {
    m_screens.append(screen);
    QObject::connect(screen, &QObject::destroyed, this,
                     &ShellGlobal::onScreenDestroyed);

    emit screensChanged();
  }
}

void ShellGlobal::onScreenRemoved(QScreen *screen) {
  auto idx = m_screens.indexOf(screen);

  if (idx != -1) {
    m_screens.removeAt(idx);
    QObject::disconnect(screen, &QObject::destroyed, this,
                        &ShellGlobal::onScreenDestroyed);

    emit screensChanged();
  }
}

void ShellGlobal::onScreenDestroyed() {
  auto screen = static_cast<QScreen *>(sender());

  if (screen) onScreenRemoved(screen);
}
} // namespace ns
