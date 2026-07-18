#include "workspacesmodel.h"

#include <algorithm>
#include <utility>

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

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland)

WorkspacesModel::WorkspacesModel(QObject *parent) : QObject(parent) {}

QQmlListProperty<HyprWorkspace> WorkspacesModel::values() {
  return readonlyQmlList<HyprWorkspace>(this, &m_workspaces);
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

  auto                   old = m_workspaces;
  QList<HyprWorkspace *> nlist;
  nlist.reserve(std::clamp<qsizetype>(aSize, 0, 100));

  for (auto entry : jArray) {
    auto obj = entry.toObject();

    if (obj.isEmpty()) continue;

    const auto id = obj.value("id").toInt(-1);
    if (id == -1) continue;
    const auto name = obj.value("name").toString();
    if (name.isEmpty()) continue;

    HyprWorkspace *workspace = nullptr;

    auto it =
        std::ranges::find_if(old.begin(), old.end(), [id](HyprWorkspace *w) {
          return w->id() == id;
        });

    if (it != old.end()) {
      workspace = *it;
      old.erase(it);
    } else {
      workspace = new HyprWorkspace(id, this);
    }

    if (!workspace) continue; // huh

    nlist.append(workspace);

    common::HyprWorkspaceData wpdata;
    wpdata.id           = id;
    wpdata.name         = name;
    wpdata.isPersistent = obj.value("ispersistent").toBool(false);
    wpdata.monitorId    = obj.value("monitorID").toInt(-1);
    wpdata.monitorName  = obj.value("monitorName").toString();

    workspace->updateData(std::move(wpdata));
  }

  std::sort(nlist.begin(), nlist.end(), [](HyprWorkspace *a, HyprWorkspace *b) {
    auto mona = a->bindableMonitorId().value();
    auto monb = b->bindableMonitorId().value();

    if (mona != monb) {
      return monb > mona;
    }

    return b->id() > a->id();
  });

  if (nlist != m_workspaces) {
    m_workspaces = nlist;
    emit workspacesChanged(m_workspaces);
  }

  for (auto *cleanup : old) {
    cleanup->deleteLater();
  }
}
} // namespace ns::hyprland
