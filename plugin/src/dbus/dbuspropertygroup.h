#pragma once

#include <qcontainerfwd.h>
#include <qdbusabstractinterface.h>
#include <qlist.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qvariant.h>

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
  void setInterface(QDBusAbstractInterface *interface);
  void getAllAndUpdate();

public slots:
  void onPropertiesChanged(const QString     &interfaceName,
                           const QVariantMap &changedProperties,
                           const QStringList &invalidatedProperties);

private:
  QList<DBusPropertyBase *> m_properties;

  QDBusAbstractInterface *m_interface;
};
} // namespace ns::dbus
