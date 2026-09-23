#pragma once

#include <qdatetime.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qtimer.h>
#include <qtmetamacros.h>

#include "helpermacros.h"
#include "ns_services_shared_global.h"

namespace ns::utils {
class NS_SERVICES_EXPORT SystemTime : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

  AUTO_MEYERS_SINGLETON_QML(SystemTime)

  Q_PROPERTY(QDateTime date READ date NOTIFY dateChanged)
  AUTO_BINDABLE_WRITABLE_DEFAULT(SystemTime, bool, enabled, true)

public:
  [[nodiscard]] QDateTime date() const;

signals:
  void dateChanged();

private slots:
  void onTimerTimeout();

private:
  explicit SystemTime(QObject *parent = nullptr);

  QTimer    m_timer;
  QDateTime m_currentTime;
  QDateTime m_targetTime;

  void update();
  void setTime(const QDateTime &target);
  void schedule(const QDateTime &target);
};
} // namespace ns::utils
