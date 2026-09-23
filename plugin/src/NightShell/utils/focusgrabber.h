#pragma once

#include <qobject.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "ns_utils_shared_global.h"

namespace ns::utils {
class FocusGrabberQML;

class NS_UTILS_EXPORT FocusGrabberManager : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML(FocusGrabberManager)

  Q_PROPERTY(
      bool active READ default NOTIFY activeChanged BINDABLE bindableActive)

public:
  [[nodiscard]] QBindable<bool> bindableActive() const { return &b_active; }
  void                          grabFocus(FocusGrabberQML *target);
  void                          releaseFocus(FocusGrabberQML *target);
  Q_INVOKABLE void              forceClear();

signals:
  void activeChanged();

private slots:
  void onCurrentLost();

private:
  explicit FocusGrabberManager(QObject *parent = nullptr);

  QPointer<FocusGrabberQML> m_current = nullptr;

  void detachCurrent();

  Q_OBJECT_BINDABLE_PROPERTY(
      FocusGrabberManager, bool, b_active, &FocusGrabberManager::activeChanged)
};

class NS_UTILS_EXPORT FocusGrabberQML : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(FocusGrabber)

  Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
  explicit FocusGrabberQML(QObject *parent = nullptr);

  [[nodiscard]] bool active() const;
  void               setActive(bool value);

  void setActiveInternal(bool value);

signals:
  void activeChanged();
  void focusLost();

private:
  bool m_active = false;
};
} // namespace ns::utils
