#include "dbusmenu.h"

#include <algorithm>
#include <memory>

#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdbusconnection.h>
#include <qdbusextratypes.h>
#include <qdbusmetatype.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qpair.h>
#include <qtypes.h>

#include "dbus_menu.h"
#include "dbusmenu_types.h"
#include "dbusmenuitem.h"

namespace ns::dbusmenu {
Q_LOGGING_CATEGORY(logNSDBusMenu, "ns.dbusmenu")

DBusMenu::DBusMenu(const QString &service, const QString &path, QObject *parent)
    : QObject(parent),
      m_rootItem(std::make_unique<DBusMenuItem>(0, this, nullptr)) {
  qDBusRegisterMetaType<DBusMenuLayout>();
  qDBusRegisterMetaType<DBusMenuIdList>();
  qDBusRegisterMetaType<DBusMenuItemProperties>();
  qDBusRegisterMetaType<DBusMenuItemPropertiesList>();
  qDBusRegisterMetaType<DBusMenuItemPropertyNames>();
  qDBusRegisterMetaType<DBusMenuItemPropertyNamesList>();

  m_items.insert(0, m_rootItem.get());

  m_interface =
      new DBusMenuInterface(service, path, QDBusConnection::sessionBus(), this);

  if (!m_interface->isValid()) {
    qCWarning(logNSDBusMenu)
        << "Cannot create DBusMenu for" << service << "at" << path;
    return;
  }

  QObject::connect(m_interface, &DBusMenuInterface::LayoutUpdated, this,
      &DBusMenu::onLayoutUpdated);

  QObject::connect(m_interface, &DBusMenuInterface::ItemsPropertiesUpdated,
      this, &DBusMenu::onItemsPropertiesUpdated);
}

void DBusMenu::onLayoutUpdated(quint32 revision, qint32 parent) {
  updateLayout(parent, -1);
}

// required for unique_ptr
DBusMenu::~DBusMenu() {}

void DBusMenu::onItemsPropertiesUpdated(
    const DBusMenuItemPropertiesList    &updatedProps,
    const DBusMenuItemPropertyNamesList &removedProps) {
  for (const auto &propset : updatedProps) {
    auto item = m_items.value(propset.id);
    if (item) {
      item->updateProperties(propset.properties);
    }
  }

  for (const auto &propset : removedProps) {
    auto item = m_items.value(propset.id);
    if (item) {
      item->updateProperties({}, propset.properties);
    }
  }
}

void DBusMenu::updateLayout(qint32 parent, qint32 depth) {
  auto pending     = m_interface->GetLayout(parent, depth, QStringList());
  auto callWatcher = new QDBusPendingCallWatcher(pending, this);

  QObject::connect(callWatcher, &QDBusPendingCallWatcher::finished, this,
      [this, parent, depth](QDBusPendingCallWatcher *call) {
        const QDBusPendingReply<uint, DBusMenuLayout> reply = *call;

        if (reply.isError()) {
          qCWarning(logNSDBusMenu) << "Error updating layout for menu" << parent
                                   << "of" << this << ":\n"
                                   << reply.error();
        } else {
          auto layout = reply.argumentAt<1>();
          this->updateLayoutRecursively(layout, m_items.value(parent), depth);
        }

        delete call;
      });
}

void DBusMenu::updateLayoutRecursively(
    const DBusMenuLayout &layout, DBusMenuItem *parent, qint32 depth) {
  auto item = m_items.value(layout.id);

  if (item == nullptr) {
    // The VALUE might be nullptr, but the KEY might exist
    if (m_items.contains(layout.id)) {
      item = new DBusMenuItem(layout.id, this, parent);
      m_items.insert(layout.id, item);
    } else {
      return;
    }
  }

  qCDebug(logNSDBusMenu).nospace() << "Recursively updating layout item "
                                   << layout.id << " (" << this << ")";
  item->updateProperties(layout.properties);

  if (depth != 0) {
    bool childrenChangedFlag = false;
    auto it                  = item->m_children.begin();
    while (it != item->m_children.end()) {
      auto existingIt = std::ranges::find_if(layout.children,
          [&](const DBusMenuLayout &layout) { return layout.id == *it; });

      if (!item->m_showChildren || existingIt == layout.children.end()) {
        removeRecursively(*it);
        it                  = item->m_children.erase(it);
        childrenChangedFlag = true;
      } else {
        it++;
      }
    }

    for (const auto &child : layout.children) {
      if (item->m_showChildren && !item->m_children.contains(child.id)) {
        m_items.insert(child.id, nullptr);
        childrenChangedFlag = true;
      }

      updateLayoutRecursively(child, item, depth - 1);
    }

    if (childrenChangedFlag) {
      item->m_children.clear();
      for (const auto &child : layout.children) {
        item->m_children.append(child.id);
      }

      item->refreshChildren();
    }
  }

  if (item->m_showChildren && item->m_childrenLoaded) {
    item->m_childrenLoaded = true;
  }

  emit item->layoutUpdated();
}

void DBusMenu::removeRecursively(qint32 id) {
  auto item = m_items.value(id);

  if (item != nullptr) {
    for (auto child : item->m_children) {
      this->removeRecursively(child);
    }
  }

  m_items.remove(id);

  if (item) {
    item->deleteLater();
  }
}

DBusMenuItem *DBusMenu::getChildItem(qint32 childId) {
  auto it = m_items.find(childId);
  if (it == m_items.end()) return nullptr;
  else return it.value();
}

DBusMenuItem *DBusMenu::menu() { return m_rootItem.get(); }

void DBusMenu::sendEvent(qint32 item, const QString &event) {
  auto pending = m_interface->Event(
      item, event, QDBusVariant(0), QDateTime::currentSecsSinceEpoch());
  auto *call = new QDBusPendingCallWatcher(pending, this);
  QObject::connect(call, &QDBusPendingCallWatcher::finished, this,
      [this, item, event](QDBusPendingCallWatcher *call) {
        const QDBusPendingReply<> reply = *call;
        if (reply.isError()) {
          qCWarning(logNSDBusMenu) << "Error sending event" << event << "to"
                                   << item << "of" << this << "\n"
                                   << reply.error();
        }
        delete call;
      });
}

void DBusMenu::prepareToShow(qint32 item, qint32 depth) {
  auto  pending = m_interface->AboutToShow(item);
  auto *call    = new QDBusPendingCallWatcher(pending, this);

  QObject::connect(call, &QDBusPendingCallWatcher::finished, this,
      [this, item, depth](QDBusPendingCallWatcher *call) {
        const QDBusPendingReply<bool> reply = *call;
        if (reply.isError()) {
          qCDebug(logNSDBusMenu)
              << "Error in AboutToShow method for" << item << "of" << this
              << "(ignored):" << reply.error();
        }

        this->updateLayout(item, depth);
        delete call;
      });
}
} // namespace ns::dbusmenu
