#pragma once

#include <memory>

#include <qabstractitemmodel.h>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>
#include <qvariant.h>

#include "dbus_menu.h"
#include "dbusmenu_types.h"
#include "dbusmenuitem.h"

#define READONLY_BINDABLE_PROP(ClassName, Type, Name)                          \
  Q_PROPERTY(                                                                  \
      Type Name READ default NOTIFY Name##Changed BINDABLE bindable_##Name)    \
public:                                                                        \
  [[nodiscard]] QBindable<Type> bindable_##Name() const { return &b_##Name; }  \
  Q_SIGNAL void                 Name##Changed();                               \
                                                                               \
private:                                                                       \
  Q_OBJECT_BINDABLE_PROPERTY(                                                  \
      ClassName, Type, b_##Name, &ClassName::Name##Changed)

#define MODELITEM_READONLY_PROPERTY(Type, Name)                                \
  READONLY_BINDABLE_PROP(DBusMenuModelItem, Type, Name)

namespace ns::dbusmenu {
class DBusMenuModelItem;

class DBusMenuModel : public QAbstractItemModel {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

public:
  explicit DBusMenuModel(
      const QString &service, const QString &path, QObject *parent = nullptr);
  ~DBusMenuModel() override;

  enum Roles { ModelDataRole = Qt::UserRole, SelfIndexRole = Qt::UserRole + 1 };

  // may be null
  DBusMenuModelItem *getChildById(qint32 id);
  QModelIndex        getModelIndexForItem(DBusMenuModelItem *item);

  QHash<int, QByteArray> roleNames() const override;
  int                    rowCount(const QModelIndex &parent) const override;
  int                    columnCount(const QModelIndex &parent) const override;

  QModelIndex index(
      int row, int column, const QModelIndex &parent) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  QVariant    data(const QModelIndex &index, int role) const override;

  void sendEvent(qint32 item, const QString &event);
  void prepareToShow(qint32 item, qint32 depth);

  void collapseToRoot();

private slots:
  void onLayoutUpdated(quint32 revision, qint32 parent);
  void onItemsPropertiesUpdated(const DBusMenuItemPropertiesList &updatedProps,
      const DBusMenuItemPropertyNamesList                        &removedProps);

private:
  DBusMenuInterface                 *m_interface = nullptr;
  std::unique_ptr<DBusMenuModelItem> m_rootItem;
  QHash<qint32, DBusMenuModelItem *> m_items;

  void updateLayout(qint32 parent, qint32 depth);
  void updateLayoutRecursively(
      const DBusMenuLayout &layout, DBusMenuModelItem *parent, qint32 depth);
  void removeRecursively(qint32 id);

  void addItem(DBusMenuModelItem *parent, DBusMenuModelItem *item);
  void removeItem(DBusMenuModelItem *item);
};

class DBusMenuModelItem : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  MODELITEM_READONLY_PROPERTY(QString, text)
  MODELITEM_READONLY_PROPERTY(bool, isSeparator)
  MODELITEM_READONLY_PROPERTY(bool, isEnabled)
  MODELITEM_READONLY_PROPERTY(bool, isVisible)
  MODELITEM_READONLY_PROPERTY(bool, hasChildren)
  MODELITEM_READONLY_PROPERTY(QString, iconUrl)
  MODELITEM_READONLY_PROPERTY(ns::dbusmenu::ItemToggleType::Enum, toggleType)
  MODELITEM_READONLY_PROPERTY(ns::dbusmenu::ItemCheckState::Enum, checkState)
  MODELITEM_READONLY_PROPERTY(ns::dbusmenu::ItemDisposition::Enum, disposition)

public:
  explicit DBusMenuModelItem(
      qint32 id, DBusMenuModel *rootMenu, DBusMenuModelItem *parentMenu);

  qsizetype childCount() const;

  [[nodiscard]] int    row() const;
  [[nodiscard]] int    childAt(int row) const;
  [[nodiscard]] qint32 id() const;

  [[nodiscard]] DBusMenuModelItem *parentMenu();

signals:
  void childrenLoaded();

private:
  const qint32       m_id;
  bool               m_childrenLoaded = false;
  int                m_row;
  DBusMenuModel     *m_rootModel  = nullptr;
  DBusMenuModelItem *m_parentMenu = nullptr;

  QList<qint32>    m_childIds;
  DBusMenuPngImage m_imageHandler;

  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuModelItem, QString, b_rawText)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuModelItem, QString, b_iconName)
  Q_OBJECT_BINDABLE_PROPERTY(DBusMenuModelItem, bool, b_childrenDisplay)

  void handleUpdatePayload(
      const QVariantMap &properties, const QStringList &removedItems);

  void sendTriggered();

  friend class DBusMenuModel;
};
} // namespace ns::dbusmenu
