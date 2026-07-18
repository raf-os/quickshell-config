#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include "hyprmonitor.h"

namespace ns::hyprland {
class HyprMonitorsModel : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QQmlListProperty<ns::hyprland::HyprMonitor> values READ values
                 NOTIFY valuesChanged)

public:
  explicit HyprMonitorsModel(QObject *parent = nullptr);

  [[nodiscard]] QQmlListProperty<HyprMonitor> values();
  QList<HyprMonitor *>                        listValues() const;

  void processMonitorData(QByteArray data);

private slots:
  void removeMonitorById(int id,
                         const QString & /*unused*/);

signals:
  void valuesChanged();

private:
  QList<HyprMonitor *> m_monitors;
};
} // namespace ns::hyprland
