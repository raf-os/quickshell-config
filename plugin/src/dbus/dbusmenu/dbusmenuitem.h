#pragma once

#include <qcontainerfwd.h>
#include <qlist.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "dbusmenu.h"

namespace ns::dbusmenu {
namespace ItemDisposition {
Q_NAMESPACE

enum Enum : quint8 { Normal = 0, Informative = 1, Warning = 2, Alert = 3 };

Q_ENUM_NS(Enum)
} // namespace ItemDisposition

namespace ItemToggleType {
Q_NAMESPACE

enum Enum : quint8 {
  Disabled  = 0,
  Checkmark = 1,
  Radio     = 2,
};

Q_ENUM_NS(Enum)
} // namespace ItemToggleType

struct ItemData {
  bool                  enabled     = true;
  bool                  visible     = true;
  ItemToggleType::Enum  toggleType  = ItemToggleType::Disabled;
  ItemDisposition::Enum disposition = ItemDisposition::Normal;
  QString               type        = "standard";
  QString               label;
  QString               iconName;
  QString               childrenDisplay;
  QList<QStringList>    shortcut;
  qint32                toggleState;
  QByteArray            iconData;
};

class DBusMenuItem : public QObject {
  Q_OBJECT

public:
  explicit DBusMenuItem(qint32 id, DBusMenu *menuHandler, DBusMenuItem *parent);

  void updateProperties(
      const QVariantMap &properties, const QStringList &removedItems = {});

signals:
  void layoutUpdated();

private slots:
  void onChildrenUpdated();

private:
  bool          m_showChildren;
  bool          m_childrenLoaded;
  qint32        m_id;
  DBusMenu     *m_menuHandler = nullptr;
  DBusMenuItem *m_parentMenu  = nullptr;

  QList<qint32> m_children;

  friend class DBusMenu;
};
} // namespace ns::dbusmenu
