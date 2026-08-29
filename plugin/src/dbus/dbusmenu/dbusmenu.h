#pragma once

#include <memory>

#include <qhash.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbusmenu_types.h"

class DBusMenuInterface;

namespace ns::dbusmenu {
class DBusMenuItem;

class DBusMenu : public QObject {
  Q_OBJECT

public:
  explicit DBusMenu(
      const QString &service, const QString &path, QObject *parent = nullptr);
  ~DBusMenu() override;

  void updateLayout(qint32 parent, qint32 depth);

private slots:
  void onLayoutUpdated(quint32 revision, qint32 parent);
  void onItemsPropertiesUpdated(const DBusMenuItemPropertiesList &updatedProps,
      const DBusMenuItemPropertyNamesList                        &removedProps);

private:
  DBusMenuInterface            *m_interface = nullptr;
  std::unique_ptr<DBusMenuItem> m_rootItem;
  QHash<qint32, DBusMenuItem *> m_items;

  void updateLayoutRecursively(
      const DBusMenuLayout &layout, DBusMenuItem *parent, qint32 depth);
  void removeRecursively(qint32 id);
};
} // namespace ns::dbusmenu
