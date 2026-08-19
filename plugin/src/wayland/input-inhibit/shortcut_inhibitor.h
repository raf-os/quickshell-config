#pragma once

#include <qobject.h>
#include <qtmetamacros.h>
#include <qwaylandclientextension.h>

#include "qwayland-keyboard-shortcuts-inhibit-unstable-v1.h"
#include "wayland-keyboard-shortcuts-inhibit-unstable-v1-client-protocol.h"
#include "wl_ii_shared_decl.h"

namespace ns::wayland::inputinhibit {
class NSWLII_EXPORT WlShortcutInhibitor
    : public QObject,
      public QtWayland::zwp_keyboard_shortcuts_inhibitor_v1 {
  Q_OBJECT

public:
  explicit WlShortcutInhibitor(
      ::zwp_keyboard_shortcuts_inhibitor_v1 *inhibitor);
  ~WlShortcutInhibitor() override;

  [[nodiscard]] bool isActive() const;
  void               stopInhibiting();

protected:
  void zwp_keyboard_shortcuts_inhibitor_v1_active() override;
  void zwp_keyboard_shortcuts_inhibitor_v1_inactive() override;

signals:
  void isActiveChanged(bool active);

private:
  bool m_isInhibiting = false;
};
} // namespace ns::wayland::inputinhibit
