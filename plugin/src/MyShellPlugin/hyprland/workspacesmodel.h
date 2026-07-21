#pragma once

#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include "hyprworkspace.h"
#include "toplevelmodel.h"

namespace ns::hyprland {
class WorkspacesModel : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QQmlListProperty<ns::hyprland::HyprWorkspace> values READ values
                 NOTIFY workspacesChanged)

public:
  explicit WorkspacesModel(QObject *parent = nullptr);

  void                            updateFromPayload(const QByteArray &data);
  QQmlListProperty<HyprWorkspace> values();

public slots:
  void onWindowMoved(ToplevelInstance *instance);
  void onToplevelsChanged(const QList<ToplevelInstance *> &newToplevels);

signals:
  void workspacesChanged(const QList<HyprWorkspace *> &newWorkspaces);

private:
  QHash<int, HyprWorkspace *> m_workspaces;
  QList<HyprWorkspace *>      m_workspacesByMonitor;
};
} // namespace ns::hyprland
