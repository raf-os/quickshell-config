#include "dbusmenumodel.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <utility>

#include <qabstractitemmodel.h>
#include <qdatetime.h>
#include <qdbusconnection.h>
#include <qdbusextratypes.h>
#include <qdbusmetatype.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtpreprocessorsupport.h>
#include <qtypes.h>
#include <qvariant.h>

#include "dbus_menu.h"
#include "dbusmenu_enums.h"
#include "dbusmenu_types.h"
#include "iconprovider.h"

namespace ns::dbusmenu {
Q_LOGGING_CATEGORY(logNSDBusMenu, "ns.dbusmenu")

namespace {
template <typename T> struct BindableParams;

template <template <typename, typename, auto, auto> class B, typename C,
    typename T, auto O, auto S>
struct BindableParams<B<C, T, O, S>> {
  using Class = C;
  using Type  = T;
};

template <typename Bindable> struct BindableType {
  using Meta = BindableParams<Bindable>;
  using Type = Meta::Type;
};

// overengineering at its best
template <typename BindablePtr>
void simplePropertyExtract(const QVariantMap &propMap, const QString &propName,
    BindablePtr *bindable, std::function<bool(const QString &)> removeCompare,
    typename BindableType<BindablePtr>::Type defaultValue) {
  using Bindable = BindableType<BindablePtr>;
  // Extracts the actual type of the bindable, so it doesn't need to be passed
  // onto the template
  using BindableType = Bindable::Type;

  auto p = propMap.value(propName);
  if (p.canConvert<BindableType>()) {
    *bindable = p.value<BindableType>();
  } else if (removeCompare(propName)) {
    *bindable = defaultValue;
  }
}

template <typename T>
void complexPropertyExtract(const QVariantMap &propMap, const QString &propName,
    std::function<bool(const QString &)> removeCompare,
    std::function<void(T)> onSuccess, std::function<void()> onRemove) {

  auto p = propMap.value(propName);
  if (p.canConvert<T>()) {
    onSuccess(p.value<T>());
  } else if (removeCompare(propName)) {
    onRemove();
  }
}
} // namespace

DBusMenuModel::DBusMenuModel(
    const QString &service, const QString &path, QObject *parent)
    : QAbstractItemModel(parent),
      m_rootItem(std::make_unique<DBusMenuModelItem>(0, this, nullptr)) {
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
      &DBusMenuModel::onLayoutUpdated);

  QObject::connect(m_interface, &DBusMenuInterface::ItemsPropertiesUpdated,
      this, &DBusMenuModel::onItemsPropertiesUpdated);

  this->updateLayout(0, 1);
}

// required for unique_ptr
DBusMenuModel::~DBusMenuModel() = default;

QHash<int, QByteArray> DBusMenuModel::roleNames() const {
  return {
      {Roles::ModelDataRole, "modelData"},
      {Roles::SelfIndexRole, "selfIndex"}
  };
}

int DBusMenuModel::rowCount(const QModelIndex &parent) const {
  DBusMenuModelItem *parentItem =
      parent.isValid()
          ? static_cast<DBusMenuModelItem *>(parent.internalPointer())
          : m_rootItem.get();
  return parentItem->childCount();
}

int DBusMenuModel::columnCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return 1;
}

QModelIndex DBusMenuModel::index(
    int row, int column, const QModelIndex &parent) const {
  if (!this->hasIndex(row, column, parent)) return {};

  DBusMenuModelItem *parentItem =
      parent.isValid()
          ? static_cast<DBusMenuModelItem *>(parent.internalPointer())
          : m_rootItem.get();

  auto childId = parentItem->childAt(row);
  if (childId == -1) return {};

  auto childItem = m_items.value(childId, nullptr);
  return childItem ? createIndex(row, column, childItem) : QModelIndex();
}

QModelIndex DBusMenuModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return {};

  auto *childItem  = static_cast<DBusMenuModelItem *>(index.internalPointer());
  auto *parentItem = childItem->parentMenu();

  if (parentItem == m_rootItem.get()) return {};
  return createIndex(parentItem->row(), 0, parentItem);
}

QVariant DBusMenuModel::data(const QModelIndex &index, int role) const {
  switch (role) {
  case Roles::ModelDataRole: {
    DBusMenuModelItem *item = nullptr;

    if (index.isValid()) {
      item = static_cast<DBusMenuModelItem *>(index.internalPointer());
    } else {
      item = m_rootItem.get();
    }
    return QVariant::fromValue<DBusMenuModelItem *>(item);
  }

  case Roles::SelfIndexRole: return index;

  default: return QVariant();
  }
}

void DBusMenuModel::onLayoutUpdated(quint32 revision, qint32 parent) {
  this->updateLayout(parent, -1);
}

void DBusMenuModel::onItemsPropertiesUpdated(
    const DBusMenuItemPropertiesList    &updatedProps,
    const DBusMenuItemPropertyNamesList &removedProps) {}

void DBusMenuModel::updateLayout(qint32 parent, qint32 depth) {
  auto pending     = m_interface->GetLayout(parent, depth, {});
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

          // for (auto it = m_items.begin(); it != m_items.end(); it++) {
          //   qCDebug(logNSDBusMenu) << it.key() << ":" << it.value();
          // }
        }

        delete call;
      });
}

void DBusMenuModel::updateLayoutRecursively(
    const DBusMenuLayout &layout, DBusMenuModelItem *parent, qint32 depth) {
  auto item = m_items.value(layout.id);

  if (item == nullptr) {
    // key might exist in the hash table with a null value
    if (m_items.contains(layout.id)) {
      // it does, so a new item must be created
      item          = new DBusMenuModelItem(layout.id, this, parent);
      item->m_depth = parent->m_depth + 1;

      // this->addItem(item, parent);
      auto rowIdx = parent->childCount();
      m_items.insert(item->id(), item);
    } else {
      // TODO: debug log
      return;
    }
  }

  // pass the new properties onto the child so it can parse it
  item->handleUpdatePayload(layout.properties, {});
  if (item->m_depth > m_maxDepth) {
    m_maxDepth = item->m_depth;
  }

  // negative depth = keep recursion going
  if (depth != 0) {
    bool childrenChangedFlag = false;

    auto it = item->m_childIds.begin();
    while (it != item->m_childIds.end()) {
      auto existingIt =
          std::ranges::find_if(layout.children.begin(), layout.children.end(),
              [&](const DBusMenuLayout &layout) { return layout.id == *it; });

      if (existingIt == layout.children.end()) {
        // this item no longer has this child
        removeRecursively(*it);

        auto itemIdx = getModelIndexForItem(item);
        auto rowIdx  = std::distance(item->m_childIds.begin(), it);

        beginRemoveRows(itemIdx, rowIdx, rowIdx);
        it = item->m_childIds.erase(it);
        endRemoveRows();

        childrenChangedFlag = true;
      } else {
        it++;
      }
    }

    for (const auto &child : layout.children) {
      if (!item->m_childIds.contains(child.id)) {
        // new one, must be inserted
        // do not update model while the item is still nullptr
        m_items.insert(child.id, nullptr);
        item->m_childIds.append(child.id);
        childrenChangedFlag = true;
      }

      // maintain recursion
      // if resulting depth reaches 0, only updates child item itself
      updateLayoutRecursively(child, item, depth - 1);
    }

    // is this redundant?
    // if (childrenChangedFlag) {
    //   item->m_childIds.clear();
    //   for (const auto &child : layout.children) {
    //     item->m_childIds.append(child.id);
    //   }
    // }
  }
}

void DBusMenuModel::removeRecursively(qint32 id) {
  auto item = m_items.value(id);

  if (item != nullptr) {
    for (auto child : item->m_childIds) {
      this->removeRecursively(child);
    }
  }

  if (item) {
    this->removeItem(item);

    item->deleteLater();
  } else {
    m_items.remove(id);
  }
}

DBusMenuModelItem *DBusMenuModel::getChildById(qint32 id) {
  auto it = m_items.find(id);
  if (it == m_items.end()) return nullptr;
  else return it.value();
}

DBusMenuModelItem *DBusMenuModel::rootItem() { return m_rootItem.get(); }

QModelIndex DBusMenuModel::getModelIndexForItem(DBusMenuModelItem *item) {
  if (!item || item == m_rootItem.get()) return QModelIndex();
  return this->createIndex(item->row(), 0, item);
}

void DBusMenuModel::addItem(
    DBusMenuModelItem *parent, DBusMenuModelItem *item) {
  auto rowIdx = parent->childCount();
  item->m_row = rowIdx;

  auto idx = getModelIndexForItem(item);
  beginInsertRows(idx, rowIdx, rowIdx);
  m_items.insert(item->id(), item);
  endInsertRows();
}

void DBusMenuModel::removeItem(DBusMenuModelItem *item) {
  if (!item) return;

  auto idx = getModelIndexForItem(item);
  auto row = item->row();

  beginRemoveRows(idx, row, row);
  m_items.remove(item->id());
  endRemoveRows();
}

void DBusMenuModel::sendEvent(qint32 item, const QString &event) {
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

void DBusMenuModel::prepareToShow(qint32 item, qint32 depth) {
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

void DBusMenuModel::prepareToShowWithCallback(
    qint32 item, QObject *handler, std::function<void(bool)> callback) {
  auto  pending = m_interface->AboutToShow(item);
  auto *call    = new QDBusPendingCallWatcher(pending, handler);

  QObject::connect(call, &QDBusPendingCallWatcher::finished, handler,
      [this, item, callback](QDBusPendingCallWatcher *call) {
        const QDBusPendingReply<bool> reply = *call;

        bool shouldUpdate = true;
        if (!reply.isError()) {
          shouldUpdate = reply.value();
        }

        updateLayout(item, 1);
        callback(shouldUpdate);

        if (shouldUpdate) {
          auto maxDepth = m_maxDepth;
          if (auto i = m_items.value(item, nullptr)) {
            if (i->m_depth + 1 > maxDepth) {
              maxDepth = i->m_depth + 1;
            }
          }
          updateLayout(0, maxDepth);
        }
        delete call;
      });
}

void DBusMenuModel::collapseToRoot() {
  m_maxDepth = 1;
  for (auto childId : m_rootItem->m_childIds) {
    auto child = m_items.value(childId);

    if (child) {
      for (auto victim : child->m_childIds) {
        this->removeRecursively(victim);
      }
    }
  }
}

void DBusMenuModel::addRef() {
  m_refcount++;

  if (m_refcount == 1) {
    this->updateLayout(0, 1);
  }
}

void DBusMenuModel::removeRef() {
  m_refcount--;

  if (m_refcount == 0) {
    this->collapseToRoot();
  }
}

DBusMenuModelItem::DBusMenuModelItem(
    qint32 id, DBusMenuModel *rootMenu, DBusMenuModelItem *parentMenu)
    : QObject(parentMenu), m_id(id), m_rootModel(rootMenu),
      m_parentMenu(parentMenu) {
  b_text.setBinding([this] {
    QString cleanLabel = b_rawText.value();
    cleanLabel.replace("_", "");
    return std::move(cleanLabel);
  });

  b_hasChildren.setBinding(
      [this] { return m_id == 0 || b_childrenDisplay.value(); });
}

qsizetype DBusMenuModelItem::childCount() const { return m_childIds.size(); }

int    DBusMenuModelItem::row() const { return m_row; }
qint32 DBusMenuModelItem::id() const { return m_id; }

int DBusMenuModelItem::childAt(int row) const {
  if (row < 0 || row >= m_childIds.size()) return -1;
  return m_childIds.at(static_cast<qsizetype>(row));
}

DBusMenuModelItem *DBusMenuModelItem::parentMenu() { return m_parentMenu; }
DBusMenuModel     *DBusMenuModelItem::rootModel() { return m_rootModel; }

void DBusMenuModelItem::handleUpdatePayload(
    const QVariantMap &properties, const QStringList &removedItems) {
  if (properties.isEmpty() && removedItems.isEmpty()) {
    qCDebug(logNSDBusMenu) << "Item" << this
                           << "received empty property update, ignoring.";
    return;
  }

  auto shouldRemove = [&removedItems](const QString &name) {
    return removedItems.isEmpty() || removedItems.contains(name);
  };

  complexPropertyExtract<QString>(
      properties, "label", shouldRemove,
      [this](auto text) {
        for (auto i = 0; i < text.length() - 1;) {
          if (text.at(i) == '_') {
            text.remove(i, 1);
            text.insert(i + 1, "</u>");
            text.insert(i, "<u>");
            i += 8;
          } else {
            i++;
          }
        }

        b_rawText = std::move(text);
      },
      [this] { b_rawText = ""; });

  simplePropertyExtract(
      properties, "enabled", &b_isEnabled, shouldRemove, false);
  simplePropertyExtract(
      properties, "visible", &b_isVisible, shouldRemove, true);
  simplePropertyExtract(properties, "icon-name", &b_iconName, shouldRemove, {});

  complexPropertyExtract<QByteArray>(
      properties, "icon-data", shouldRemove,
      [this](auto data) {
        if (data.isEmpty()) {
          m_imageHandler.data.clear();
        } else if (!m_imageHandler.hasData() || m_imageHandler.data != data) {
          m_imageHandler.data = data;
          m_imageHandler.imageChanged();
        }
      },
      [this] { m_imageHandler.data.clear(); });

  complexPropertyExtract<QString>(
      properties, "type", shouldRemove,
      [this](QString type) {
        if (type == "separator") {
          b_isSeparator = true;
        } else {
          b_isSeparator = false;
        }
      },
      [this] { b_isSeparator = false; });

  complexPropertyExtract<QString>(
      properties, "toggle-type", shouldRemove,
      [this](auto toggleType) {
        b_toggleType = ItemToggleType::fromString(toggleType);
      },
      [this] { b_toggleType = ItemToggleType::None; });

  complexPropertyExtract<qint32>(
      properties, "toggle-state", shouldRemove,
      [this](auto stateInt) {
        switch (stateInt) {
        case 0:  b_checkState = ItemCheckState::Unchecked; break;
        case 1:  b_checkState = ItemCheckState::Checked; break;
        default: b_checkState = ItemCheckState::PartiallyChecked; break;
        }
      },
      [this] { b_checkState = ItemCheckState::Unchecked; });

  complexPropertyExtract<QString>(
      properties, "children-display", shouldRemove,
      [this](auto childrenDisplayString) {
        if (childrenDisplayString == "") b_childrenDisplay = false;
        else if (childrenDisplayString == "submenu") b_childrenDisplay = true;
        else b_childrenDisplay = false;
      },
      [this] { b_hasChildren = false; });

  complexPropertyExtract<QString>(
      properties, "disposition", shouldRemove,
      [this](auto disposition) {
        b_disposition = ItemDisposition::fromString(disposition);
      },
      [this] { b_disposition = ItemDisposition::Normal; });

  if (m_imageHandler.hasData()) {
    b_iconUrl = m_imageHandler.urlFor();
  } else if (!b_iconName.value().isEmpty()) {
    auto iconPath = iconprovider::IconImageProvider::getSystemIconRequestString(
        b_iconName.value(), {}, {});
    b_iconUrl = iconPath;
  } else {
    b_iconUrl = "";
  }

  // qCDebug(logNSDBusMenu) << "ITEM:" << b_text.value()
  //                        << "\nIS SEPARATOR:" << b_isSeparator.value();
}

void DBusMenuModelItem::sendTriggered() {
  m_rootModel->sendEvent(m_id, "clicked");
}
} // namespace ns::dbusmenu
