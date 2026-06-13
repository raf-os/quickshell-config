#pragma once

#include <qcontainerfwd.h>
#include <qdbusservicewatcher.h>
#include <qhash.h>
#include <qjsengine.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qqmlintegration.h>
#include <qtypes.h>

namespace ns {
namespace notifications {
class NotificationServer : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

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
  void notification(void *notification);

  void NotificationClosed(quint32 id, quint32 reason);
  void ActionInvoked(quint32 id, QString action);
  void NotificationReplied(quint32 id, QString replyText);

private:
  explicit NotificationServer(QObject *parent = nullptr);

  static void tryRegister();

  QDBusServiceWatcher m_serviceWatcher{this};
  quint32 m_curId = 1;
  QHash<quint32, void *> m_notifications;
};
} // namespace notifications
} // namespace ns
