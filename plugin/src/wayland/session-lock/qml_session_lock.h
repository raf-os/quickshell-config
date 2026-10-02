#pragma once

#include <qmap.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlcomponent.h>
#include <qqmlintegration.h>
#include <qscreen.h>
#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

namespace ns::wayland {
class LockSurfaceQML;

class SessionLockQML : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(SessionLockManager)
  Q_CLASSINFO("DefaultProperty", "surface")

  Q_PROPERTY(QQmlComponent *surface READ surfaceComponent WRITE
          setSurfaceComponent NOTIFY surfaceComponentChanged)
  Q_PROPERTY(bool isLocked READ default NOTIFY isLockedChanged BINDABLE
          bindableIsLocked)
  Q_PROPERTY(bool isSecure READ default NOTIFY isSecureChanged BINDABLE
          bindableIsSecure)

public:
  explicit SessionLockQML(QObject *parent = nullptr);
  ~SessionLockQML() override;
  Q_DISABLE_COPY_MOVE(SessionLockQML)

  [[nodiscard]] QQmlComponent *surfaceComponent() const;
  void setSurfaceComponent(QQmlComponent *surfaceComponent);

  [[nodiscard]] QBindable<bool> bindableIsLocked() const;
  [[nodiscard]] QBindable<bool> bindableIsSecure() const;

  void realizeLockTarget();

public slots:
  void lock();
  void unlock();
  void updateSurfaces(bool show);

signals:
  void surfaceComponentChanged();
  void isLockedChanged();
  void isSecureChanged();

private slots:
  void onScreensChanged();
  void doUnlock();

private:
  QQmlComponent                    *m_surfaceComponent = nullptr;
  QMap<QScreen *, LockSurfaceQML *> m_surfaces;

  bool m_lockTarget;
  bool m_isLocking;
  bool m_awaitingLock;

  Q_OBJECT_BINDABLE_PROPERTY(
      SessionLockQML, bool, b_isLocked, &SessionLockQML::isLockedChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      SessionLockQML, bool, b_isSecure, &SessionLockQML::isSecureChanged)
};
} // namespace ns::wayland
