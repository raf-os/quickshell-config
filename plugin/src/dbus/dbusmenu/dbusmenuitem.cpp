#include "dbusmenuitem.h"

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qtypes.h>

#include "dbusmenu.h"

namespace ns::dbusmenu {
DBusMenuItem::DBusMenuItem(
    qint32 id, DBusMenu *menuHandler, DBusMenuItem *parentMenu)
    : QObject(parentMenu), m_id(id), m_menuHandler(menuHandler),
      m_parentMenu(parentMenu) {}

void DBusMenuItem::updateProperties(
    const QVariantMap &properties, const QStringList &removedItems) {}

void DBusMenuItem::onChildrenUpdated() {}
} // namespace ns::dbusmenu
