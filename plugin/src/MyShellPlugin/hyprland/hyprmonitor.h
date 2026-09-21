#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qpoint.h>
#include <qpointer.h>
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
  Q_PROPERTY(ns::hyprland::HyprWorkspace *activeWorkspace READ activeWorkspace
          NOTIFY activeWorkspaceChanged)

public:
  explicit HyprMonitor(int id, QObject *parent = nullptr);

  [[nodiscard]] int                id() const { return m_id; }
  [[nodiscard]] QBindable<QString> bName() const { return &b_name; }
  [[nodiscard]] QBindable<QString> bModel() const { return &b_model; }
  [[nodiscard]] QBindable<bool>    bDisabled() const { return &b_disabled; }
  [[nodiscard]] QBindable<QPoint>  bPosition() const { return &b_position; }
  [[nodiscard]] QBindable<QPoint>  bSize() const { return &b_size; }

  [[nodiscard]] QQmlListProperty<HyprWorkspace> workspaces();
  [[nodiscard]] HyprWorkspace                  *activeWorkspace();

  void processData(common::HyprMonitorData data);

private slots:
  void onWorkspacesChanged(const QList<HyprWorkspace *> & /*unused*/);

signals:
  void nameChanged();
  void modelChanged();
  void disabledChanged();
  void positionChanged();
  void sizeChanged();
  void workspacesChanged();
  void activeWorkspaceChanged();

private:
  const int              m_id;
  QList<HyprWorkspace *> m_workspaces;
  HyprWorkspace         *m_activeWorkspace = nullptr;

  struct {
    int     id;
    QString name;
  } m_activeWorkspaceData;

  void tryAssignActiveWorkspace();

  Q_OBJECT_BINDABLE_PROPERTY(
      HyprMonitor, QString, b_name, &HyprMonitor::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprMonitor, QString, b_model, &HyprMonitor::modelChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprMonitor, bool, b_disabled, &HyprMonitor::disabledChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprMonitor, QPoint, b_position, &HyprMonitor::positionChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprMonitor, QPoint, b_size, &HyprMonitor::sizeChanged)
};
} // namespace ns::hyprland
