#include "qml_lock_surface.h"

#include <qlogging.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmllist.h>
#include <qquickitem.h>
#include <qscreen.h>
#include <qtypes.h>

#include "qml_session_lock.h"
#include "window_attached_lock.h"

namespace ns::wayland {
LockSurfaceQML::LockSurfaceQML(QObject *parent)
    : QObject(parent), m_contentItem(new QQuickItem()),
      m_lock(new sessionlock::WindowAttachedLock(this)) {
  QQmlEngine::setObjectOwnership(m_contentItem, QQmlEngine::CppOwnership);
  m_contentItem->setParent(this);
}

LockSurfaceQML::~LockSurfaceQML() {
  if (m_window) {
    m_window->destroy();
    m_window->deleteLater();
  }
}

void LockSurfaceQML::attach() {
  if (m_lock->isAttached()) return;

  if (auto *parent = qobject_cast<SessionLockQML *>(this->parent())) {
    if (!m_lock->attach(m_window)) {
      qFatal() << "Failed attaching LockSurfaceQML";
    }
  } else {
    qFatal() << "Attempted to attach a LockSurfaceQML with invalid parent";
  }
}

void LockSurfaceQML::show() {
  attach();
  m_lock->setVisible();
}

QQmlListProperty<QObject> LockSurfaceQML::data() {
  return m_contentItem->property("data").value<QQmlListProperty<QObject>>();
}

QScreen *LockSurfaceQML::screen() {
  if (m_window) {
    return m_window->screen();
  }

  return m_screen;
}
void LockSurfaceQML::setScreen(QScreen *screen) {
  if (m_screen) {
    QObject::disconnect(m_screen, nullptr, this, nullptr);
  }

  if (screen) {
    QObject::connect(
        screen, &QObject::destroyed, this, &LockSurfaceQML::onScreenDestroyed);
  }

  if (m_window) {
    m_window->setScreen(screen);
  } else {
    m_screen = screen;
  }

  emit screenChanged();
}

void LockSurfaceQML::onScreenDestroyed() {
  m_screen = nullptr;
  emit screenChanged();
}

bool LockSurfaceQML::isVisible() const {
  if (m_window) return m_window->isVisible();
  return false;
}

qint32 LockSurfaceQML::width() const {
  if (!m_window) return 0;
  return m_window->width();
}
qint32 LockSurfaceQML::height() const {
  if (!m_window) return 0;
  return m_window->height();
}

void LockSurfaceQML::onWidthChanged() { m_contentItem->setWidth(width()); }
void LockSurfaceQML::onHeightChanged() { m_contentItem->setHeight(height()); }
} // namespace ns::wayland
