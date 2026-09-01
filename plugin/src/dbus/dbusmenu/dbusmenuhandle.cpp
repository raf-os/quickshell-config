#include "dbusmenuhandle.h"

#include <qnamespace.h>
#include <qobject.h>

#include "dbusmenu.h"
#include "dbusmenuitem.h"

namespace ns::dbusmenu {
void DBusMenuHandle::setAddress(const QString &service, const QString &path) {
  if (service == m_service && path == m_path) return;

  m_service = service;
  m_path    = path;
  emit onMenuPathChanged();
}

bool DBusMenuHandle::isValid() const {
  return !m_service.isEmpty() && !m_path.isEmpty();
}

DBusMenuItem *DBusMenuHandle::menu() {
  return m_isLoaded ? m_menu->menu() : nullptr;
}

void DBusMenuHandle::onMenuPathChanged() {
  if (m_menu) {
    const DBusMenuItem *rootItem = m_menu->menu();
    QObject::disconnect(rootItem, nullptr, this, nullptr);
    m_menu->deleteLater();
    m_menu     = nullptr;
    m_isLoaded = false;
    emit menuChanged();
  }

  if (m_refcount > 0 && this->isValid()) {
    m_menu                 = new DBusMenu(m_service, m_path, this);
    DBusMenuItem *rootItem = m_menu->menu();

    QObject::connect(
        rootItem, &DBusMenuItem::layoutUpdated, this,
        [this]() {
          m_isLoaded = true;
          emit menuChanged();
        },
        Qt::SingleShotConnection);

    rootItem->setShowChildrenRecursive(true);
  }
}

void DBusMenuHandle::addRef() {
  m_refcount++;

  if (m_refcount == 1 || !m_menu) {
    onMenuPathChanged();
  } else {
    m_menu->menu()->forceUpdateLayout();
  }
}

void DBusMenuHandle::removeRef() {
  m_refcount--;

  if (m_refcount == 0) {
    onMenuPathChanged();
  }
}

DBusMenuOpener::~DBusMenuOpener() {
  if (m_menu) {
    m_menu->removeRef();
  }
}

DBusMenuHandle *DBusMenuOpener::menu() const { return m_menu; }

void DBusMenuOpener::setMenu(DBusMenuHandle *menu) {
  if (menu == m_menu) return;

  if (m_menu != nullptr) {
    QObject::disconnect(m_menu, nullptr, this, nullptr);
    m_menu->removeRef();
  }

  m_menu = menu;

  if (menu != nullptr) {
    QObject::connect(
        menu, &QObject::destroyed, this, &DBusMenuOpener::onMenuDestroyed);

    menu->addRef();
  }

  emit menuChanged();
}

void DBusMenuOpener::onMenuDestroyed() {
  m_menu = nullptr;
  emit menuChanged();
}
} // namespace ns::dbusmenu
