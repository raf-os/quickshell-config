#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include "hyprworkspace.h"

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

signals:
  void workspacesChanged(const QList<HyprWorkspace *> &newWorkspaces);

private:
  QList<HyprWorkspace *> m_workspaces;
};
} // namespace ns::hyprland
