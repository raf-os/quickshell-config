#include "qml_lock_surface.h"

#include <qlogging.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmllist.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qscreen.h>
#include <qtypes.h>
#include <qwindow.h>

#include "manager.h"
#include "qml_session_lock.h"
#include "screencopy_qml_view.h"
#include "wayland_logging.h"
#include "window_attached_lock.h"

namespace ns::wayland {
LockSurfaceQML::LockSurfaceQML(QObject *parent)
    : QObject(parent), m_contentItem(new QQuickItem()),
      m_lock(new sessionlock::WindowAttachedLock(this)) {
  QQmlEngine::setObjectOwnership(m_contentItem, QQmlEngine::CppOwnership);
  m_contentItem->setParent(this);

  QObject::connect(this, &LockSurfaceQML::widthChanged, this,
      &LockSurfaceQML::onWidthChanged);
  QObject::connect(this, &LockSurfaceQML::heightChanged, this,
      &LockSurfaceQML::onHeightChanged);

  auto setupScreenCopy = [this] {
    m_screenCopy = new screencopy::ScreencopyQMLView();
    m_screenCopy->setParent(this);
    m_screenCopy->setParentItem(m_contentItem);
    m_screenCopy->componentComplete();
    QObject::connect(m_contentItem, &QQuickItem::widthChanged, m_screenCopy,
        [this] { m_screenCopy->setWidth(m_contentItem->width()); });
    QObject::connect(m_contentItem, &QQuickItem::heightChanged, m_screenCopy,
        [this] { m_screenCopy->setHeight(m_contentItem->height()); });
    QObject::connect(m_screenCopy,
        &screencopy::ScreencopyQMLView::hasContentChanged, this, [this] {
          if (!m_screenCopy || !m_screenCopy->bindableHasContent().value())
            return;
          emit screenCopyReady();
        });
    onScreenCopyCreated();
  };

  auto buf = buffer::WlBufferManager::instance();
  if (!buf->isReady()) {
    QObject::connect(buf, &buffer::WlBufferManager::ready, this,
        setupScreenCopy, Qt::SingleShotConnection);
  } else {
    setupScreenCopy();
  }

  // m_screenCopy->setParent(this);
  // m_screenCopy->setParentItem(m_contentItem);
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

void LockSurfaceQML::onScreenCopyCreated() {
  if (!m_screen) return;

  m_screenCopy->setCaptureSource(m_screen);
  // QObject::connect(m_screenCopy,
  //     &screencopy::ScreencopyQMLView::hasContentChanged, this,
  //     [this] { emit screenCopyReady(); });
  m_screenCopy->captureSingleFrame();
}

void LockSurfaceQML::show() {
  attach();
  m_lock->setVisible();
}

QQmlListProperty<QObject> LockSurfaceQML::data() {
  return m_contentItem->property("data").value<QQmlListProperty<QObject>>();
}

void LockSurfaceQML::setupWindow() {
  if (!m_window) {
    m_window = new QQuickWindow();
  }
  m_contentItem->setParentItem(m_window->contentItem());
  m_contentItem->setWidth(width());
  m_contentItem->setHeight(height());

  if (m_screen) {
    m_window->setScreen(m_screen);
    m_window->setColor("black");
  }

  QObject::connect(
      m_window, &QWindow::widthChanged, this, &LockSurfaceQML::widthChanged);
  QObject::connect(
      m_window, &QWindow::heightChanged, this, &LockSurfaceQML::heightChanged);
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

  m_screen = screen;

  if (m_screenCopy && !m_screenCopy->bindableHasContent().value()) {
    m_screenCopy->setCaptureSource(screen);
    QObject::connect(m_screenCopy,
        &screencopy::ScreencopyQMLView::hasContentChanged, this,
        &LockSurfaceQML::onScreenCopyCreated);
    m_screenCopy->captureSingleFrame();
  }

  if (m_window) {
    m_window->setScreen(screen);
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

bool LockSurfaceQML::isScreencopyReady() const {
  return true;
  return m_screenCopy->bindableHasContent().value();
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
