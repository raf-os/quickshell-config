#pragma once

#include <qobject.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>

namespace ns::dbusmenu {
class ScopedDBusMenuView : public QObject {
  Q_OBJECT
  QML_ELEMENT

public:
  explicit ScopedDBusMenuView(QObject *parent = nullptr);
};
} // namespace ns::dbusmenu
