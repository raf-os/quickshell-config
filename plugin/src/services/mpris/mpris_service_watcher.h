#pragma once

#include <qdbusservicewatcher.h>
#include <qobject.h>
#include <qtmetamacros.h>
namespace ns::mpris {
class MprisServiceWatcher : public QObject {
  Q_OBJECT

public:
  explicit MprisServiceWatcher(QObject *parent = nullptr);

private slots:
  void onServiceRegistered(const QString &service);
  void onServiceUnregistered(const QString &service);

private:
  QDBusServiceWatcher m_watcher;
};
} // namespace ns::mpris
