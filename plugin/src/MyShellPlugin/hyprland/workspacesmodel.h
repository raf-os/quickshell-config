#pragma once

#include <qlist.h>
#include <qobject.h>
#include <qqmlintegration.h>
#include <qstringview.h>
#include <qtmetamacros.h>

#include "hyprworkspace.h"

namespace ns::hyprland {
class WorkspacesModel : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_UNCREATABLE("")

  Q_PROPERTY(QList<ns::hyprland::HyprWorkspace *> values READ values NOTIFY
                 workspacesChanged)

public:
  explicit WorkspacesModel(QObject *parent = nullptr);

  void                   updateFromPayload(const QByteArray &data);
  QList<HyprWorkspace *> values() const;

signals:
  void workspacesChanged();

private:
  QList<HyprWorkspace *> m_workspaces;
};
} // namespace ns::hyprland
