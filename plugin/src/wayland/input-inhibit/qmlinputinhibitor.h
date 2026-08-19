#pragma once

#include <qobject.h>
#include <qpointer.h>
#include <qqmlintegration.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qtmetamacros.h>

#include "shortcut_inhibitor.h"
#include "wl_ii_shared_decl.h"

namespace ns::wayland::inputinhibit {
// Not an actual attached property, quickshell uses a proxy object for its
// windows, so can't do qobject_cast
class NSWLII_EXPORT InputInhibitAttached : public QQuickItem {
  Q_OBJECT
  QML_ELEMENT
  QML_NAMED_ELEMENT(InputInhibit)

  Q_PROPERTY(bool inhibit READ inhibit WRITE setInhibit NOTIFY inhibitChanged)
  Q_PROPERTY(bool isActive READ isActive NOTIFY isActiveChanged)

public:
  explicit InputInhibitAttached(QQuickItem *parent = nullptr);

  void componentComplete() override;

  [[nodiscard]] bool inhibit() const;
  void               setInhibit(bool value);

  [[nodiscard]] bool isActive() const;

signals:
  void inhibitChanged();
  void isActiveChanged();

private slots:
  void initWindow();

private:
  bool doInhibit();

  bool                          m_inhibiting = false;
  QQuickWindow                 *m_window     = nullptr;
  QPointer<WlShortcutInhibitor> m_handle     = nullptr;
};
} // namespace ns::wayland::inputinhibit
