#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>

namespace ns::dbus {
class DBusPropertyBase {
public:
  DBusPropertyBase()          = default;
  virtual ~DBusPropertyBase() = default;
  Q_DISABLE_COPY_MOVE(DBusPropertyBase)

  [[nodiscard]] virtual QString name() const = 0;
};

class DBusPropertyGroup : public QObject {
  Q_OBJECT

public:
  explicit DBusPropertyGroup(QObject *parent = nullptr);

  void attachProperty(DBusPropertyBase *property);

private:
  QList<DBusPropertyBase *> m_properties;
};
} // namespace ns::dbus
