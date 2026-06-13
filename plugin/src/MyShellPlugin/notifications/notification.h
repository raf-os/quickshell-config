#pragma once

#include <qcontainerfwd.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns {
namespace notifications {
class Notification : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(quint32 id READ id CONSTANT)

  Q_PROPERTY(qreal expireTimeout READ default NOTIFY expireTimeoutChanged
                 BINDABLE bindableExpireTimeout)
  Q_PROPERTY(QString appName READ default NOTIFY appNameChanged BINDABLE
                 bindableAppName)
  Q_PROPERTY(QString appIcon READ default NOTIFY appIconChanged BINDABLE
                 bindableAppIcon)
  Q_PROPERTY(QString summary READ default NOTIFY summaryChanged BINDABLE
                 bindableSummary)
  Q_PROPERTY(QString body READ default NOTIFY bodyChanged BINDABLE bindableBody)
  Q_PROPERTY(
      QVariantMap hints READ default NOTIFY hintsChanged BINDABLE bindableHints)

public:
  explicit Notification(quint32 id, QObject *parent);
  [[nodiscard]] QBindable<qreal> bindableExpireTimeout() const {
    return &m_expireTimeout;
  }
  [[nodiscard]] QBindable<QString> bindableAppName() const {
    return &m_bindableAppName;
  }

signals:
  void expireTimeoutChanged();
  void appNameChanged();

private:
  Q_OBJECT_BINDABLE_PROPERTY(Notification, qreal, m_expireTimeout,
                             &Notification::expireTimeoutChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification, QString, m_bindableAppName,
                             &Notification::appNameChanged)
};
} // namespace notifications
} // namespace ns
