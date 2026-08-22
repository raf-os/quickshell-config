#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

#include "dbus_item.h"

namespace ns::systemtray {
class StatusNotifierItem : public QObject {
  Q_OBJECT

public:
  explicit StatusNotifierItem(const QString &address,
                              QObject       *parent = nullptr);

  [[nodiscard]] bool isValid() const;

signals:
  void ready();

private:
  QDBusStatusNotifierItem *m_item = nullptr;
  QString                  m_watcherId;
};
} // namespace ns::systemtray
