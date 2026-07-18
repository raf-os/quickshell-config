#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qpoint.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>

#include "hyprdefs.h"
#include "hyprworkspace.h"

namespace ns::hyprland {
class HyprMonitor : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(int id READ id CONSTANT)
  Q_PROPERTY(QString name READ default NOTIFY nameChanged BINDABLE bName)
  Q_PROPERTY(QString model READ default NOTIFY modelChanged BINDABLE bModel)
  Q_PROPERTY(
      bool disabled READ default NOTIFY disabledChanged BINDABLE bDisabled)
  Q_PROPERTY(
      QPoint position READ default NOTIFY positionChanged BINDABLE bPosition)
  Q_PROPERTY(QPoint size READ default NOTIFY sizeChanged BINDABLE bSize)

  Q_PROPERTY(QQmlListProperty<HyprWorkspace> workspaces READ workspaces NOTIFY
                 workspacesChanged)

public:
  explicit HyprMonitor(int      id,
                       QObject *parent = nullptr);

  [[nodiscard]] int                id() const { return m_id; }
  [[nodiscard]] QBindable<QString> bName() const { return &m_name; }
  [[nodiscard]] QBindable<QString> bModel() const { return &m_model; }
  [[nodiscard]] QBindable<bool>    bDisabled() const { return &m_disabled; }
  [[nodiscard]] QBindable<QPoint>  bPosition() const { return &m_position; }
  [[nodiscard]] QBindable<QPoint>  bSize() const { return &m_size; }

  [[nodiscard]] QQmlListProperty<HyprWorkspace> workspaces();

  void processData(common::HyprMonitorData data);

private slots:
  void onWorkspacesChanged(const QList<HyprWorkspace *> &newWorkspaces);

signals:
  void nameChanged();
  void modelChanged();
  void disabledChanged();
  void positionChanged();
  void sizeChanged();
  void workspacesChanged();

private:
  const int              m_id;
  QList<HyprWorkspace *> m_workspaces;

  Q_OBJECT_BINDABLE_PROPERTY(HyprMonitor,
                             QString,
                             m_name,
                             &HyprMonitor::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprMonitor,
                             QString,
                             m_model,
                             &HyprMonitor::modelChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprMonitor,
                             bool,
                             m_disabled,
                             &HyprMonitor::disabledChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprMonitor,
                             QPoint,
                             m_position,
                             &HyprMonitor::positionChanged)
  Q_OBJECT_BINDABLE_PROPERTY(HyprMonitor,
                             QPoint,
                             m_size,
                             &HyprMonitor::sizeChanged)
};
} // namespace ns::hyprland
