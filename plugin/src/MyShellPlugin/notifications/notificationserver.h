#pragma once

#include "notification.h"
#include "notificationsmodel.h"

#include <qcontainerfwd.h>
#include <qdbusservicewatcher.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace ns {
namespace notifications {
class NotificationServer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  Q_PROPERTY(ns::notifications::NotificationsModel *model READ model NOTIFY
                 modelChanged)
  Q_PROPERTY(
      bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)

public:
  static NotificationServer *instance() {
    static NotificationServer *s_instance = new NotificationServer();
    return s_instance;
  }

  static NotificationServer *create(QQmlEngine *qmlEngine, QJSEngine *) {
    auto i = instance();
    if (qmlEngine)
      qmlEngine->setObjectOwnership(i, QJSEngine::CppOwnership);
    return i;
  }

  void deleteNotification(Notification *notification,
                          NotificationCloseReason::Enum reason);

  [[nodiscard]] NotificationsModel *model() { return &m_model; }
  [[nodiscard]] bool isActive() const { return m_isActive; }
  void setIsActive(const bool &value);

  Q_INVOKABLE void closeAllNotifications();

  // DBus methods
  // clang-format off
  void CloseNotification(uint id);
  QStringList GetCapabilities() const;
  static QString GetServerInformation(
      QString &vendor,
      QString &version,
      QString &specVersion
  );
  uint Notify(
      const QString &appName,
      uint replacesId,
      const QString &appIcon,
      const QString &summary,
      const QString &body,
      const QStringList &actions,
      const QVariantMap &hints,
      int expireTimeout
  );
  // clang-format on
  // End DBus methods

private slots:
  static void onServiceUnregistered(const QString &service);

signals:
  void notification(Notification *notification);

  void NotificationClosed(quint32 id, NotificationCloseReason::Enum reason);
  void ActionInvoked(quint32 id, QString action);
  void NotificationReplied(quint32 id, QString replyText);

  void modelChanged();
  void isActiveChanged();

private:
  explicit NotificationServer(QObject *parent = nullptr);

  static void tryRegister();
  static void closeConnection();

  void resetServerState();

  bool m_isActive = false;
  QDBusServiceWatcher m_serviceWatcher{this};
  quint32 m_curId = 1;
  QHash<quint32, Notification *> m_notificationsMap;
  NotificationsModel m_model{this};
};
} // namespace notifications
} // namespace ns
