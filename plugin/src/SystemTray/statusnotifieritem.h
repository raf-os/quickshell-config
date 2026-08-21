#pragma once

#include <qobject.h>
#include <qtmetamacros.h>

namespace ns::systemtray {
class StatusNotifierItem : public QObject {
  Q_OBJECT

public:
  explicit StatusNotifierItem(const QString &address,
                              QObject       *parent = nullptr);
};
} // namespace ns::systemtray
