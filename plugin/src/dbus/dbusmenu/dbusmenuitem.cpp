// #include "dbusmenuitem.h"
//
// #include <functional>
//
// #include <qcontainerfwd.h>
// #include <qlist.h>
// #include <qloggingcategory.h>
// #include <qobject.h>
// #include <qpixmap.h>
// #include <qsize.h>
// #include <qstringview.h>
// #include <qtypes.h>
//
// #include "dbusmenu.h"
// #include "iconprovider.h"
// #include "objectmodel.h"
//
// namespace ns::dbusmenu {
// Q_LOGGING_CATEGORY(logNSDbusmenuItem, "ns.dbusmenu.MenuItem")
//
// namespace {
// template <typename T> struct BindableParams;
//
// template <template <typename, typename, auto, auto> class B, typename C,
//     typename T, auto O, auto S>
// struct BindableParams<B<C, T, O, S>> {
//   using Class = C;
//   using Type  = T;
// };
//
// template <typename Bindable> struct BindableType {
//   using Meta = BindableParams<Bindable>;
//   using Type = Meta::Type;
// };
//
// // This is me trying to be too smart, it might break
// template <typename BindablePtr>
// void simplePropertyExtract(const QVariantMap &propMap, const QString
// &propName,
//     BindablePtr *bindable, std::function<bool(const QString &)>
//     removeCompare, typename BindableType<BindablePtr>::Type defaultValue =
//     {}) {
//   using Bindable = BindableType<BindablePtr>;
//   // Extracts the actual type of the bindable, so it doesn't need to be
//   passed
//   // onto the template
//   using BindableType = Bindable::Type;
//
//   auto p = propMap.value(propName);
//   if (p.canConvert<BindableType>()) {
//     *bindable = p.value<BindableType>();
//   } else if (removeCompare(propName)) {
//     *bindable = defaultValue;
//   }
// }
//
// template <typename T>
// void complexPropertyExtract(const QVariantMap &propMap, const QString
// &propName,
//     std::function<bool(const QString &)> removeCompare,
//     std::function<void(T)> onSuccess, std::function<void()> onRemove) {
//
//   auto p = propMap.value(propName);
//   if (p.canConvert<T>()) {
//     onSuccess(p.value<T>());
//   } else if (removeCompare(propName)) {
//     onRemove();
//   }
// }
// } // namespace
//
// namespace ItemDisposition {
// ItemDisposition::Enum fromString(const QString &str) {
//   if (str == "normal") return Normal;
//   if (str == "informative") return Informative;
//   if (str == "warning") return Warning;
//   if (str == "alert") return Alert;
//   return Normal;
// }
// } // namespace ItemDisposition
// namespace ItemToggleType {
// ItemToggleType::Enum fromString(const QString &str) {
//   if (str.isEmpty()) return None;
//   if (str == "radio") return Checkmark;
//   if (str == "checkmark") return Radio;
//   return None;
// }
// } // namespace ItemToggleType
//
// QPixmap DBusMenuPngImage::requestPixmap(
//     const QString & /*unused*/, QSize *size, const QSize & /*unused*/) {
//   auto pixmap = QPixmap();
//   if (!pixmap.loadFromData(this->data, "PNG")) {
//     qCWarning(logNSDbusmenuItem) << "Unable to load dbusmenu item png";
//   }
//
//   if (size != nullptr) *size = pixmap.size();
//   return pixmap;
// }
//
// DBusMenuItem::DBusMenuItem(
//     qint32 id, DBusMenu *menuHandler, DBusMenuItem *parentMenu)
//     : QObject(parentMenu), m_id(id), m_menuHandler(menuHandler),
//       m_parentMenu(parentMenu) {
//   b_label.setBinding([this] {
//     QString cleanLabel = b_text.value();
//     cleanLabel.replace("_", "");
//     return std::move(cleanLabel);
//   });
//
//   b_hasChildren.setBinding(
//       [this] { return m_id == 0 || b_childrenDisplay.value(); });
// }
//
// DBusMenuItem::~DBusMenuItem() = default;
//
// ObjectModel<DBusMenuItem> *DBusMenuItem::children() { return
// &m_childrenModel; }
//
// void DBusMenuItem::setShowChildrenRecursive(bool showChildren) {
//   if (showChildren == m_showChildren) return;
//
//   m_showChildren   = showChildren;
//   m_childrenLoaded = false;
//
//   if (showChildren) {
//     m_menuHandler->prepareToShow(m_id, -1);
//   } else {
//     if (!m_children.isEmpty()) {
//       for (auto child : m_children) {
//         m_menuHandler->removeRecursively(child);
//       }
//
//       m_children.clear();
//       refreshChildren();
//     }
//   }
// }
//
// bool DBusMenuItem::isShowingChildren() {
//   return m_showChildren && m_childrenLoaded;
// }
//
// void DBusMenuItem::updateProperties(
//     const QVariantMap &properties, const QStringList &removedItems) {
//   // Note: some programs do not consider an empty property map to be a full
//   // reset
//   if (properties.isEmpty() && removedItems.isEmpty()) {
//     qCDebug(logNSDbusmenuItem)
//         << "Item" << this << "received empty property update, ignoring.";
//     return;
//   }
//
//   auto shouldRemove = [&removedItems](const QString &name) {
//     return removedItems.isEmpty() || removedItems.contains(name);
//   };
//
//   auto label = properties.value("label");
//   if (label.canConvert<QString>()) {
//     auto text = label.toString();
//
//     for (auto i = 0; i < text.length() - 1;) {
//       if (text.at(i) == '_') {
//         text.remove(i, 1);
//         text.insert(i + 1, "</u>");
//         text.insert(i, "<u>");
//         i += 8;
//       } else {
//         i++;
//       }
//     }
//
//     b_text = std::move(text);
//   } else if (shouldRemove("label")) {
//     b_text = "";
//   }
//
//   simplePropertyExtract(properties, "enabled", &b_enabled, shouldRemove,
//   false); simplePropertyExtract(properties, "visible", &b_visible,
//   shouldRemove, true); simplePropertyExtract(properties, "icon-name",
//   &b_iconName, shouldRemove, {});
//
//   auto iconData = properties.value("icon-data");
//   if (iconData.canConvert<QByteArray>()) {
//     auto data = iconData.toByteArray();
//     if (data.isEmpty()) {
//       m_image.data.clear();
//     } else if (!m_image.hasData() || m_image.data != data) {
//       m_image.data = data;
//       m_image.imageChanged();
//     }
//   } else if (shouldRemove("icon-data")) {
//     m_image.data.clear();
//   }
//
//   auto type = properties.value("type");
//   if (type.canConvert<QString>()) {
//     b_isSeparator = type.toString() == "separator";
//   } else if (shouldRemove("type")) {
//     b_isSeparator = false;
//   }
//
//   auto toggleType = properties.value("toggle-type");
//   if (toggleType.canConvert<QString>()) {
//     auto tstr    = toggleType.toString();
//     b_toggleType = ItemToggleType::fromString(tstr);
//   } else if (shouldRemove("toggle-type")) {
//     b_toggleType = ItemToggleType::None;
//   }
//
//   auto toggleState = properties.value("toggle-state");
//   if (toggleState.canConvert<qint32>()) {
//     auto stateInt = toggleState.value<qint32>();
//
//     switch (stateInt) {
//     case 0:  b_checkState = ItemCheckState::Unchecked; break;
//     case 1:  b_checkState = ItemCheckState::Checked; break;
//     default: b_checkState = ItemCheckState::PartiallyChecked; break;
//     }
//   } else if (shouldRemove("toggle-state")) {
//     b_checkState = ItemCheckState::Unchecked;
//   }
//
//   auto childrenDisplay = properties.value("children-display");
//   if (childrenDisplay.canConvert<QString>()) {
//     auto dstr = childrenDisplay.toString();
//
//     if (dstr == "") b_childrenDisplay = false;
//     else if (dstr == "submenu") b_childrenDisplay = true;
//     else b_childrenDisplay = false;
//   } else if (shouldRemove("children-display")) {
//     b_childrenDisplay = false;
//   }
//
//   auto disposition = properties.value("disposition");
//   if (disposition.canConvert<QString>()) {
//     auto dstr     = disposition.toString();
//     b_disposition = ItemDisposition::fromString(dstr);
//   } else if (shouldRemove("disposition")) {
//     b_disposition = ItemDisposition::Normal;
//   }
//
//   if (m_image.hasData()) {
//     b_icon = m_image.urlFor();
//   } else if (!b_iconName.value().isEmpty()) {
//     auto iconPath =
//     iconprovider::IconImageProvider::getSystemIconRequestString(
//         b_iconName.value(),
//         m_menuHandler->bindableIconThemePath().value().join(':'), {});
//     b_icon = iconPath;
//   } else {
//     b_icon = "";
//   }
// }
//
// void DBusMenuItem::trigger() { sendTriggered(); }
//
// void DBusMenuItem::forceUpdateLayout(bool recursive) {
//   if (!isShowingChildren()) return;
//   m_menuHandler->updateLayout(m_id, recursive ? -1 : 1);
// }
//
// DBusMenuItem *DBusMenuItem::getParentItem() { return m_parentMenu; }
// void          DBusMenuItem::setParentItem(DBusMenuItem *parent) {
//   QObject::setParent(parent);
//   m_parentMenu = parent;
// }
//
// qint32 DBusMenuItem::getDepth() const { return m_depth; }
// void   DBusMenuItem::setDepth(qint32 depth) { m_depth = depth; }
//
// void DBusMenuItem::refreshChildren() {
//   QList<DBusMenuItem *> newChildren;
//   for (auto child : m_children) {
//     auto *item = m_menuHandler->getChildItem(child);
//     // if (item) {
//     //   item->setParentItem(this);
//     //   item->setDepth(m_depth + 1);
//     // }
//     if (item && item->bindableVisible().value()) {
//       newChildren.append(item);
//     }
//   }
//
//   m_childrenModel.diffUpdate(newChildren);
// }
//
// void DBusMenuItem::sendOpened() { m_menuHandler->sendEvent(m_id, "opened"); }
// void DBusMenuItem::sendClosed() { m_menuHandler->sendEvent(m_id, "closed"); }
// void DBusMenuItem::sendTriggered() {
//   m_menuHandler->sendEvent(m_id, "clicked");
// }
// } // namespace ns::dbusmenu
