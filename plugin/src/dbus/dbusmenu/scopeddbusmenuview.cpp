#include "scopeddbusmenuview.h"

#include <qobject.h>

namespace ns::dbusmenu {
ScopedDBusMenuView::ScopedDBusMenuView(QObject *parent) : QObject(parent) {}
} // namespace ns::dbusmenu
