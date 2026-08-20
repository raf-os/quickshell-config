#include "qmlinputinhibitor.h"

#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qquickitem.h>
#include <qquickwindow.h>

#include "input_inhibit_manager.h"
#include "shortcut_inhibitor.h"

namespace ns::wayland::inputinhibit {
Q_DECLARE_LOGGING_CATEGORY(logNSInputInhibit) // from input_inhibit_manager.cpp

InputInhibitAttached::InputInhibitAttached(QQuickItem *parent)
    : QQuickItem(parent) {}

void InputInhibitAttached::componentComplete() {
  QQuickItem::componentComplete();

  auto win = this->window();
  if (win) {
    this->initWindow();
  }

  QObject::connect(this,
                   &QQuickItem::windowChanged,
                   this,
                   &InputInhibitAttached::initWindow);
}

void InputInhibitAttached::initWindow() {
  auto win = this->window();
  if (win) {
    m_window = win;
    WlInputInhibitManager::instance()->initWindow(win);

    if (m_inhibiting) this->doInhibit();
  } else {
    m_window = nullptr;
  }
}

bool InputInhibitAttached::isActive() const {
  if (m_handle.isNull()) return false;
  return m_handle->isActive();
}

bool InputInhibitAttached::doInhibit() {
  if (!m_window) {
    return false;
  }

  WlShortcutInhibitor *handle = nullptr;
  auto                 succ =
      WlInputInhibitManager::instance()->inhibitForWindow(m_window, handle);

  if (succ) {
    if (handle) {
      m_handle = handle;
      QObject::connect(m_handle,
                       &WlShortcutInhibitor::isActiveChanged,
                       this,
                       &InputInhibitAttached::isActiveChanged);
    }
    emit isActiveChanged();
  }

  return succ;
}

bool InputInhibitAttached::inhibit() const { return m_inhibiting; }
void InputInhibitAttached::setInhibit(bool value) {
  if (m_inhibiting == value) return;
  const auto winst = WlInputInhibitManager::instance();

  m_inhibiting = value;
  emit inhibitChanged();

  if (!m_window) return;

  if (value) {
    this->doInhibit();
  } else {
    auto succ = winst->stopInhibitingWindow(m_window);
    if (succ) {
      if (!m_handle.isNull()) {
        QObject::disconnect(m_handle, nullptr, this, nullptr);
      }
      m_handle.clear();
      emit isActiveChanged();
    }
  }
}
} // namespace ns::wayland::inputinhibit
