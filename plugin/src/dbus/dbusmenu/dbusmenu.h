// #pragma once
//
// #include <memory>
//
// #include <qcontainerfwd.h>
// #include <qhash.h>
// #include <qobject.h>
// #include <qproperty.h>
// #include <qqmlintegration.h>
// #include <qtmetamacros.h>
// #include <qtypes.h>
//
// #include "dbusmenu_types.h"
//
// class DBusMenuInterface;
//
// namespace ns::dbusmenu {
// class DBusMenuItem;
//
// class DBusMenu : public QObject {
//   Q_OBJECT
//   QML_ELEMENT
//   Q_MOC_INCLUDE("dbusmenuitem.h")
//   Q_PROPERTY(ns::dbusmenu::DBusMenuItem *menu READ menu CONSTANT)
//
// public:
//   explicit DBusMenu(
//       const QString &service, const QString &path, QObject *parent =
//       nullptr);
//   ~DBusMenu() override;
//
//   void updateLayout(qint32 parent, qint32 depth);
//   void sendEvent(qint32 item, const QString &event);
//   void prepareToShow(qint32 item, qint32 depth);
//
//   // may be nullptr
//   DBusMenuItem *getChildItem(qint32 childId);
//   DBusMenuItem *menu();
//
//   QBindable<QStringList> bindableIconThemePath() const {
//     return &b_iconThemePath;
//   }
//
//   void removeRecursively(qint32 id);
//
//   // Drops the entire tree except for children of the root item
//   void collapseToRoot();
//
// signals:
//   void iconThemePathChanged();
//
// private slots:
//   void onLayoutUpdated(quint32 revision, qint32 parent);
//   void onItemsPropertiesUpdated(const DBusMenuItemPropertiesList
//   &updatedProps,
//       const DBusMenuItemPropertyNamesList &removedProps);
//
// private:
//   QString                       m_iconThemePath;
//   DBusMenuInterface            *m_interface = nullptr;
//   std::unique_ptr<DBusMenuItem> m_rootItem;
//   QHash<qint32, DBusMenuItem *> m_items;
//
//   void updateLayoutRecursively(
//       const DBusMenuLayout &layout, DBusMenuItem *parent, qint32 depth);
//
//   Q_OBJECT_BINDABLE_PROPERTY(
//       DBusMenu, QStringList, b_iconThemePath,
//       &DBusMenu::iconThemePathChanged)
// };
// } // namespace ns::dbusmenu
