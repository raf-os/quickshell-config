#include "shortcut_inhibitor.h"

#include <qloggingcategory.h>
#include <qwaylandclientextension.h>

#include "qwayland-keyboard-shortcuts-inhibit-unstable-v1.h"
#include "wayland-keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h"

namespace ns::wayland::inputinhibit {
Q_DECLARE_LOGGING_CATEGORY(logNSInputInhibit) // from input_inhibit_manager.cpp

WlShortcutInhibitor::WlShortcutInhibitor(
    ::zwp_keyboard_shortcuts_inhibitor_v1 *inhibitor)
    : QtWayland::zwp_keyboard_shortcuts_inhibitor_v1(inhibitor) {}

WlShortcutInhibitor::~WlShortcutInhibitor() {
  if (this->isInitialized()) this->destroy();
}

bool WlShortcutInhibitor::isActive() const { return m_isInhibiting; }

void WlShortcutInhibitor::stopInhibiting() { delete this; }

void WlShortcutInhibitor::zwp_keyboard_shortcuts_inhibitor_v1_active() {
  m_isInhibiting = true;
  emit isActiveChanged(true);
}

void WlShortcutInhibitor::zwp_keyboard_shortcuts_inhibitor_v1_inactive() {
  m_isInhibiting = false;
  emit isActiveChanged(false);
}
} // namespace ns::wayland::inputinhibit
