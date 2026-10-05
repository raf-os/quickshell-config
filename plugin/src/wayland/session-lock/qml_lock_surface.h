#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qscreen.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "screencopy_qml_view.h"
#include "window_attached_lock.h"

namespace ns::wayland {
class LockSurfaceQML : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(LockSurface)
  Q_CLASSINFO("DefaultProperty", "data")

  Q_PROPERTY(QQmlListProperty<QObject> data READ data)

public:
  explicit LockSurfaceQML(QObject *parent = nullptr);
  ~LockSurfaceQML() override;
  Q_DISABLE_COPY_MOVE(LockSurfaceQML)

  [[nodiscard]] QQmlListProperty<QObject> data();

  [[nodiscard]] qint32 width() const;
  [[nodiscard]] qint32 height() const;
  [[nodiscard]] bool   isVisible() const;
  [[nodiscard]] bool   isScreencopyReady() const;

  void setupWindow();

  [[nodiscard]] QScreen *screen();
  void                   setScreen(QScreen *screen);

public slots:
  void attach();
  void show();

signals:
  void screenChanged();
  void screenCopyReady();
  void widthChanged();
  void heightChanged();

private slots:
  void onScreenDestroyed();
  void onWidthChanged();
  void onHeightChanged();
  void onScreenCopyCreated();

private:
  QQuickItem                      *m_contentItem;
  QQuickWindow                    *m_window = nullptr;
  QScreen                         *m_screen = nullptr;
  sessionlock::WindowAttachedLock *m_lock;
  screencopy::ScreencopyQMLView   *m_screenCopy = nullptr;
};
} // namespace ns::wayland
