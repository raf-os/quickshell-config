#include "workspacesmodel.h"

#include <algorithm>
#include <utility>

#include <qhash.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmllist.h>
#include <qstringview.h>
#include <qtypes.h>

#include "hyprdefs.h"
#include "hyprworkspace.h"
#include "qlisthelpers.h"
#include "toplevelmodel.h"

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland)

WorkspacesModel::WorkspacesModel(QObject *parent) : QObject(parent) {}

QQmlListProperty<HyprWorkspace> WorkspacesModel::values() {
  return readonlyQmlList<HyprWorkspace>(this, &m_workspacesByMonitor);
}

void WorkspacesModel::updateFromPayload(const QByteArray &data) {
  QJsonParseError parseError;
  auto            jDoc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qCWarning(logNSHyprland)
        << "Error parsing workspace json data from hyprland.";
    return;
  }

  auto       jArray = jDoc.array();
  const auto aSize  = jArray.size();

  auto old = m_workspaces;

  for (auto entry : jArray) {
    auto obj = entry.toObject();

    if (obj.isEmpty()) continue;

    const auto id = obj.value("id").toInt(-1);
    if (id == -1) continue;
    const auto name = obj.value("name").toString();
    if (name.isEmpty()) continue;

    HyprWorkspace *workspace = nullptr;

    auto hasOld = old.contains(id);

    if (hasOld) {
      workspace = old.value(id);
      old.remove(id);
    } else {
      workspace = new HyprWorkspace(id, this);
      m_workspaces.insert(id, workspace);
    }

    if (!workspace) continue; // huh

    common::HyprWorkspaceData wpdata;
    wpdata.id           = id;
    wpdata.name         = name;
    wpdata.isPersistent = obj.value("ispersistent").toBool(false);
    wpdata.monitorId    = obj.value("monitorID").toInt(-1);
    wpdata.monitorName  = obj.value("monitorName").toString();

    workspace->updateData(std::move(wpdata));
  }

  for (auto it = old.begin(); it != old.end(); ++it) {
    it.value()->deleteLater();
    m_workspaces.remove(it.key());
  }

  auto nlist = m_workspaces.values();

  std::sort(nlist.begin(), nlist.end(), [](HyprWorkspace *a, HyprWorkspace *b) {
    auto mona = a->bindableMonitorId().value();
    auto monb = b->bindableMonitorId().value();

    if (mona != monb) {
      return monb > mona;
    }

    return b->id() > a->id();
  });

  if (nlist != m_workspacesByMonitor) {
    m_workspacesByMonitor = nlist;
    emit workspacesChanged(m_workspacesByMonitor);
  }

  for (auto *cleanup : old) {
    cleanup->deleteLater();
  }
}

void WorkspacesModel::onToplevelsChanged(
    const QList<ToplevelInstance *> &newToplevels) {
  for (const auto &toplevel : newToplevels) {
    auto workspaceId = toplevel->workspaceId();
    if (!m_workspaces.contains(workspaceId)) continue;
    m_workspaces.value(workspaceId)->attachToplevel(toplevel);
  }
}

void WorkspacesModel::onWindowMoved(ToplevelInstance *toplevel) {
  auto workspaceId = toplevel->workspaceId();
  if (!m_workspaces.contains(workspaceId)) return;
  m_workspaces.value(workspaceId)->attachToplevel(toplevel);
}
} // namespace ns::hyprland
