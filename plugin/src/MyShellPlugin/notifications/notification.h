#pragma once

#include "dbusimage.h"

#include <qcontainerfwd.h>
#include <qmap.h>
#include <qobject.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns {
namespace notifications {
class NotificationUrgency : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  enum Enum : quint8 {
    Low      = 0,
    Normal   = 1,
    Critical = 2,
  };
  Q_ENUM(Enum)
};

class NotificationCloseReason : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  enum Enum : quint8 { Expired = 1, Dismissed = 2, CloseRequested = 3 };
  Q_ENUM(Enum)
};

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
  Q_PROPERTY(QString imageUrl READ default NOTIFY imageUrlChanged BINDABLE
                 bindableImageUrl)
  Q_PROPERTY(
      QVariantMap hints READ default NOTIFY hintsChanged BINDABLE bindableHints)
  Q_PROPERTY(bool resident READ default NOTIFY residentChanged BINDABLE
                 bindableResident)
  Q_PROPERTY(QString desktopEntry READ default NOTIFY desktopEntryChanged
                 BINDABLE bindableDesktopEntry)
  Q_PROPERTY(ns::notifications::NotificationUrgency::Enum urgency
                 READ default NOTIFY urgencyChanged BINDABLE bindableUrgency)

public:
  explicit Notification(quint32  id,
                        QObject *parent);

  Q_INVOKABLE void expire();
  Q_INVOKABLE void dismiss();

  void close(NotificationCloseReason::Enum reason);

  void updateProperties(const QString     &appName,
                        QString            appIcon,
                        const QString     &summary,
                        const QString     &body,
                        const QStringList &actions,
                        QVariantMap        hints,
                        qint32             expireTimeout);

  [[nodiscard]] quint32 id() const;

  [[nodiscard]] QBindable<qreal> bindableExpireTimeout() const {
    return &m_expireTimeout;
  }
  [[nodiscard]] QBindable<QString> bindableAppName() const {
    return &m_appName;
  }
  [[nodiscard]] QBindable<QString> bindableAppIcon() const {
    return &m_appIcon;
  }
  [[nodiscard]] QBindable<QString> bindableSummary() const {
    return &m_summary;
  }
  [[nodiscard]] QBindable<QString> bindableImageUrl() const {
    return &m_imageUrl;
  }
  [[nodiscard]] QBindable<QString>     bindableBody() const { return &m_body; }
  [[nodiscard]] QBindable<QVariantMap> bindableHints() const {
    return &m_hints;
  }
  [[nodiscard]] QBindable<QString> bindableDesktopEntry() const {
    return &m_desktopEntry;
  }
  [[nodiscard]] QBindable<bool> bindableResident() const { return &m_resident; }
  [[nodiscard]] QBindable<NotificationUrgency::Enum> bindableUrgency() const {
    return &m_urgency;
  }

  [[nodiscard]] NotificationCloseReason::Enum closeReason() const;

signals:
  void closed(ns::notifications::NotificationCloseReason::Enum reason);

  void expireTimeoutChanged();
  void appNameChanged();
  void appIconChanged();
  void summaryChanged();
  void imageUrlChanged();
  void bodyChanged();
  void hintsChanged();
  void residentChanged();
  void desktopEntryChanged();
  void urgencyChanged();

private:
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             qreal,
                             m_expireTimeout,
                             &Notification::expireTimeoutChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QString,
                             m_appName,
                             &Notification::appNameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QString,
                             m_appIcon,
                             &Notification::appIconChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QString,
                             m_summary,
                             &Notification::summaryChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QString,
                             m_body,
                             &Notification::bodyChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QString,
                             m_imageUrl,
                             &Notification::imageUrlChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QVariantMap,
                             m_hints,
                             &Notification::hintsChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             bool,
                             m_resident,
                             &Notification::residentChanged)
  Q_OBJECT_BINDABLE_PROPERTY(Notification,
                             QString,
                             m_desktopEntry,
                             &Notification::desktopEntryChanged)
  Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Notification,
                                       NotificationUrgency::Enum,
                                       m_urgency,
                                       NotificationUrgency::Normal,
                                       &Notification::urgencyChanged)

  quint32                       m_id;
  NotificationCloseReason::Enum m_closeReason =
      NotificationCloseReason::Dismissed;
  dbusprovider::DBusImageHandler m_imageHandler;
};
} // namespace notifications
} // namespace ns
