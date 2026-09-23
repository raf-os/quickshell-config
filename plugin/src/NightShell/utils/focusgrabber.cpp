#include "focusgrabber.h"

#include <qobject.h>

namespace ns::utils {
FocusGrabberManager::FocusGrabberManager(QObject *parent) : QObject(parent) {}

void FocusGrabberManager::grabFocus(FocusGrabberQML *target) {
  if (target == m_current.get()) return;

  detachCurrent();

  m_current = target;
  b_active  = target != nullptr;

  if (target) {
    QObject::connect(
        target, &QObject::destroyed, this, &FocusGrabberManager::onCurrentLost);
    target->setActiveInternal(true);
  }
}

void FocusGrabberManager::releaseFocus(FocusGrabberQML *target) {
  if (target != m_current.get()) return;

  detachCurrent();

  m_current = nullptr;
  b_active  = false;
}

void FocusGrabberManager::forceClear() {
  detachCurrent();

  m_current = nullptr;
  b_active  = false;
}

void FocusGrabberManager::detachCurrent() {
  if (m_current) {
    QObject::disconnect(m_current, nullptr, this, nullptr);
    m_current->setActiveInternal(false);
  }
}

void FocusGrabberManager::onCurrentLost() { b_active = false; }

// FocusGrabberQML
FocusGrabberQML::FocusGrabberQML(QObject *parent) : QObject(parent) {}

bool FocusGrabberQML::active() const { return m_active; }
void FocusGrabberQML::setActive(bool value) {
  if (value == m_active) return;

  auto manager = FocusGrabberManager::instance();

  if (value) {
    manager->grabFocus(this);
  } else {
    manager->releaseFocus(this);
  }
}

void FocusGrabberQML::setActiveInternal(bool value) {
  if (value == m_active) return;

  m_active = value;
  emit activeChanged();

  if (value == false) emit focusLost();
}
} // namespace ns::utils
