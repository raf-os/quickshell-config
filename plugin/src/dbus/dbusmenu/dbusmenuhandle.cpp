// #include "dbusmenuhandle.h"
//
// #include <qloggingcategory.h>
// #include <qnamespace.h>
// #include <qobject.h>
// #include <qtypes.h>
//
// #include "dbusmenu.h"
// #include "dbusmenuitem.h"
//
// namespace ns::dbusmenu {
// Q_DECLARE_LOGGING_CATEGORY(logNSDBusMenu) // from dbusmenu.cpp
//
// void DBusMenuHandle::setAddress(const QString &service, const QString &path)
// {
//   if (service == m_service && path == m_path) return;
//
//   qCDebug(logNSDBusMenu).nospace().noquote()
//       << "DBusMenuHandle: new address: (" << service << ", " << path << ")";
//
//   m_service = service;
//   m_path    = path;
//   onMenuPathChanged();
// }
//
// bool DBusMenuHandle::isValid() const {
//   return !m_service.isEmpty() && !m_path.isEmpty();
// }
//
// DBusMenuItem *DBusMenuHandle::rootMenu() {
//   if (!m_menu) return nullptr;
//   return m_isLoaded ? m_menu->menu() : nullptr;
// }
//
// DBusMenuItem *DBusMenuHandle::getItemById(qint32 id) {
//   auto item = m_menu->getChildItem(id);
//   if (!item) return nullptr;
//
//   auto depth = item->getDepth();
//   if (depth > m_maxDepth) m_maxDepth = depth;
//
//   m_menu->prepareToShow(id, -1);
//   return m_menu->getChildItem(id);
// }
//
// void DBusMenuHandle::onMenuPathChanged() {
//   if (m_menu) {
//     const DBusMenuItem *rootItem = m_menu->menu();
//     QObject::disconnect(rootItem, nullptr, this, nullptr);
//     m_menu->deleteLater();
//     m_menu     = nullptr;
//     m_isLoaded = false;
//     emit menuChanged();
//   }
//
//   if (this->isValid()) {
//     m_menu                 = new DBusMenu(m_service, m_path, this);
//     DBusMenuItem *rootItem = m_menu->menu();
//
//     QObject::connect(
//         rootItem, &DBusMenuItem::layoutUpdated, this,
//         [this]() {
//           m_isLoaded = true;
//           emit menuChanged();
//         },
//         Qt::SingleShotConnection);
//
//     m_menu->updateLayout(0, 1);
//
//     // rootItem->setShowChildrenRecursive(true);
//   }
// }
//
// void DBusMenuHandle::addRef() {
//   m_refcount++;
//
//   if (!m_menu) return;
//
//   if (m_refcount == 1) {
//     // onMenuPathChanged();
//     m_menu->menu()->forceUpdateLayout();
//   } else {
//     // m_menu->menu()->forceUpdateLayout();
//   }
// }
//
// void DBusMenuHandle::removeRef() {
//   m_refcount--;
//
//   if (m_refcount == 0) {
//     // onMenuPathChanged();
//     if (m_menu) {
//       m_menu->collapseToRoot();
//       m_maxDepth = 1;
//     }
//   }
// }
//
// DBusMenuOpener::~DBusMenuOpener() {
//   if (m_menu) {
//     m_menu->removeRef();
//   }
// }
//
// DBusMenuHandle *DBusMenuOpener::menu() const { return m_menu; }
//
// void DBusMenuOpener::setMenu(DBusMenuHandle *menu) {
//   if (menu == m_menu) return;
//
//   if (m_menu != nullptr) {
//     QObject::disconnect(m_menu, nullptr, this, nullptr);
//     m_menu->removeRef();
//   }
//
//   if (m_item) {
//     QObject::disconnect(m_item, nullptr, this, nullptr);
//     m_item = nullptr;
//   }
//
//   m_menu = menu;
//
//   if (menu != nullptr) {
//     QObject::connect(
//         menu, &QObject::destroyed, this, &DBusMenuOpener::onMenuDestroyed);
//     QObject::connect(menu, &DBusMenuHandle::menuChanged, this,
//         [this] { emit menuChanged(); });
//
//     menu->addRef();
//   } else {
//     m_valid = false;
//   }
//
//   emit menuChanged();
//   tryAttach();
// }
//
// void DBusMenuOpener::setOffset(qint32 offset) {
//   m_valid        = true;
//   auto oldOffset = m_watchedIndex;
//
//   m_watchedIndex = offset;
//   if (offset != oldOffset) emit offsetChanged();
//
//   tryAttach();
// }
//
// DBusMenuItem *DBusMenuOpener::item() { return m_item; }
//
// void DBusMenuOpener::onMenuDestroyed() {
//   m_menu  = nullptr;
//   m_item  = nullptr;
//   m_valid = false;
//   emit menuChanged();
// }
//
// void DBusMenuOpener::onItemDestroyed() {
//   m_item  = nullptr;
//   m_valid = false;
//   emit itemChanged();
// }
//
// void DBusMenuOpener::tryAttach() {
//   if (!m_menu || !m_valid) return;
//
//   auto rootMenu = m_menu->rootMenu();
//   if (!rootMenu) {
//     QObject::connect(
//         m_menu, &DBusMenuHandle::menuChanged, this,
//         [this] { this->tryAttach(); }, Qt::SingleShotConnection);
//     return;
//   }
//
//   auto oldItem = m_item;
//   auto item    = m_menu->getItemById(m_watchedIndex);
//
//   if (oldItem && oldItem != item) {
//     QObject::disconnect(oldItem, nullptr, this, nullptr);
//   }
//
//   if (item) {
//     QObject::connect(
//         item, &QObject::destroyed, this, &DBusMenuOpener::onItemDestroyed);
//   }
//
//   m_item = item;
//
//   if (oldItem != item) emit itemChanged();
// }
// } // namespace ns::dbusmenu
