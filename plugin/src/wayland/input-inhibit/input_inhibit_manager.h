#pragma once

#include <private/qwaylandinputdevice_p.h>
#include <qhash.h>
#include <qpointer.h>
#include <qquickwindow.h>
#include <qtmetamacros.h>
#include <qwaylandclientextension.h>

#include "qwayland-keyboard-shortcuts-inhibit-unstable-v1.h"
#include "shortcut_inhibitor.h"
#include "wl_ii_shared_decl.h"

namespace ns::wayland::inputinhibit {
class NSWLII_EXPORT WlInputInhibitManager
    : public QWaylandClientExtensionTemplate<WlInputInhibitManager>,
      public QtWayland::zwp_keyboard_shortcuts_inhibit_manager_v1 {
  using WinInhibitorPair = QHash<QQuickWindow *, WlShortcutInhibitor *>;

  Q_OBJECT

public:
  static WlInputInhibitManager *instance();
  ~WlInputInhibitManager() override;

  void initWindow(QQuickWindow *window);
  bool inhibitForWindow(QQuickWindow        *window,
                        WlShortcutInhibitor *outInhibitor = nullptr);
  bool stopInhibitingWindow(QQuickWindow *window);

signals:
  void ready();
  void windowInhibited(QQuickWindow *window);

private slots:
  void onWindowDestroyed(QQuickWindow *window);

private:
  explicit WlInputInhibitManager();

  QPointer<QtWaylandClient::QWaylandInputDevice> m_inputDevice = nullptr;
  WinInhibitorPair                               m_activeInhibitors;
};
} // namespace ns::wayland::inputinhibit
