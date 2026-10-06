#pragma once

#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "helpermacros.h"

namespace ns::services {
class UserData : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML_DECL(UserData)

  Q_PROPERTY(QString name READ default NOTIFY nameChanged BINDABLE bindableName)
  Q_PROPERTY(QString realName READ default NOTIFY realNameChanged BINDABLE
          bindableRealName)
  Q_PROPERTY(quint32 uid READ default NOTIFY uidChanged BINDABLE bindableUid)

public:
  [[nodiscard]] QBindable<QString> bindableName() const;
  [[nodiscard]] QBindable<QString> bindableRealName() const;
  [[nodiscard]] QBindable<quint32> bindableUid() const;

signals:
  void nameChanged();
  void realNameChanged();
  void uidChanged();

private:
  explicit UserData(QObject *parent = nullptr);

  void fetchUserData();

#define B(Type, Name)                                                          \
  Q_OBJECT_BINDABLE_PROPERTY(UserData, Type, b_##Name, &UserData::Name##Changed)

  B(QString, name)
  B(QString, realName)
  B(quint32, uid)
#undef B
};
} // namespace ns::services
