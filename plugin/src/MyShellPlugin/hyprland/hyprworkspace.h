#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qpointer.h>
#include <qproperty.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qtmetamacros.h>
#include <qtypes.h>

#include "hyprdefs.h"
#include "toplevelmodel.h"

namespace ns::hyprland {
class HyprWorkspace : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(int id READ id CONSTANT)
  Q_PROPERTY(QString name READ default NOTIFY nameChanged BINDABLE bindableName)
  Q_PROPERTY(bool isPersistent READ default NOTIFY isPersistentChanged BINDABLE
          bindableIsPersistent)
  Q_PROPERTY(bool isFullScreen READ default NOTIFY isFullScreenChanged BINDABLE
          bindableIsFullScreen)
  Q_PROPERTY(int monitorId READ default NOTIFY monitorIdChanged BINDABLE
          bindableMonitorId)
  Q_PROPERTY(QString monitorName READ default NOTIFY monitorNameChanged BINDABLE
          bindableMonitorName)
  Q_PROPERTY(QQmlListProperty<ns::hyprland::ToplevelInstance> toplevels READ
          toplevels NOTIFY toplevelsChanged)
  Q_PROPERTY(ns::hyprland::ToplevelInstance *activeToplevel READ activeToplevel
          NOTIFY activeToplevelChanged)

public:
  explicit HyprWorkspace(int id, QObject *parent = nullptr);

  [[nodiscard]] int                id() const { return m_id; }
  [[nodiscard]] QBindable<QString> bindableName() { return &b_name; }
  void                             setName(const QString &name);
  [[nodiscard]] QBindable<bool>    bindableIsPersistent() {
    return &b_isPersistent;
  }
  [[nodiscard]] QBindable<bool>    bindableIsFullScreen();
  [[nodiscard]] QBindable<int>     bindableMonitorId() { return &b_monitorId; }
  [[nodiscard]] QBindable<QString> bindableMonitorName() {
    return &b_monitorName;
  }
  [[nodiscard]] QQmlListProperty<ToplevelInstance> toplevels();
  [[nodiscard]] ToplevelInstance                  *activeToplevel();
  void setActiveToplevel(quint64 address);

  void updateData(common::HyprWorkspaceData data);

  void attachToplevel(ToplevelInstance *toplevel);
  void detachToplevel(ToplevelInstance *toplevel);

signals:
  void nameChanged();
  void isPersistentChanged();
  void isFullScreenChanged();
  void monitorIdChanged();
  void monitorNameChanged();
  void toplevelsChanged();
  void activeToplevelChanged();

private slots:
  void onAddressActivated(quint64 address);

private:
  const int                  m_id;
  QList<ToplevelInstance *>  m_childToplevels;
  QPointer<ToplevelInstance> m_activeToplevel = nullptr;
  quint64                    m_lastAddress    = 0;

  Q_OBJECT_BINDABLE_PROPERTY(
      HyprWorkspace, QString, b_name, &HyprWorkspace::nameChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprWorkspace, bool, b_isPersistent, &HyprWorkspace::isPersistentChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprWorkspace, bool, b_isFullScreen, &HyprWorkspace::isFullScreenChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprWorkspace, int, b_monitorId, &HyprWorkspace::monitorIdChanged)
  Q_OBJECT_BINDABLE_PROPERTY(
      HyprWorkspace, QString, b_monitorName, &HyprWorkspace::monitorNameChanged)
};
} // namespace ns::hyprland
