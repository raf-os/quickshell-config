#pragma once

#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpointer.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "hyprmonitor.h"

namespace ns::hyprland {
class HyprMonitorsModel : public QAbstractListModel {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QQmlListProperty<ns::hyprland::HyprMonitor> values READ values
          NOTIFY valuesChanged)
  Q_PROPERTY(ns::hyprland::HyprMonitor *focusedMonitor READ focusedMonitor
          NOTIFY focusedMonitorChanged)

public:
  explicit HyprMonitorsModel(QObject *parent = nullptr);

  enum Roles { ModelDataRole = Qt::UserRole + 1 };
  QHash<int, QByteArray> roleNames() const override {
    return {
        {Roles::ModelDataRole, "modelData"}
    };
  }

  qint32   rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, qint32 role) const override;

  [[nodiscard]] QQmlListProperty<HyprMonitor> values();
  QList<HyprMonitor *>                        listValues() const;

  [[nodiscard]] HyprMonitor *focusedMonitor();

  void processMonitorData(QByteArray data);

public slots:
  void onMonitorFocused(const QString &monitor, int /*unused*/);

private slots:
  void onMonitorDestroyed();
  // void removeMonitorById(int id, const QString & /*unused*/);

signals:
  void valuesChanged();
  void focusedMonitorChanged();

private:
  QList<HyprMonitor *>  m_monitors;
  QPointer<HyprMonitor> m_focusedMonitor = nullptr;
};
} // namespace ns::hyprland
