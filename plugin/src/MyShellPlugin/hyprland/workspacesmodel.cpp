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
#include <qstringview.h>
#include <qtypes.h>

#include "hyprdefs.h"
#include "hyprworkspace.h"

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland)

WorkspacesModel::WorkspacesModel(QObject *parent) : QObject(parent) {}

QList<HyprWorkspace *> WorkspacesModel::values() const { return m_workspaces; }

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

    workspace->updateData(std::move(wpdata));
  }

  std::sort(nlist.begin(), nlist.end(), [](HyprWorkspace *a, HyprWorkspace *b) {
    return a->id() > b->id();
  });

  if (nlist != m_workspaces) {
    m_workspaces = nlist;
    emit workspacesChanged();
  }

  for (auto *cleanup : old) {
    cleanup->deleteLater();
  }
}
} // namespace ns::hyprland
