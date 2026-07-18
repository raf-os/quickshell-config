#include "hyprmonitorsmodel.h"

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

#include "hyprdefs.h"
#include "hyprmonitor.h"
#include "qlisthelpers.h"

namespace ns::hyprland {
Q_DECLARE_LOGGING_CATEGORY(logNSHyprland) // from hyprland.cpp

HyprMonitorsModel::HyprMonitorsModel(QObject *parent) : QObject(parent) {}

QQmlListProperty<HyprMonitor> HyprMonitorsModel::values() {
  return readonlyQmlList(this, &m_monitors);
}

QList<HyprMonitor *> HyprMonitorsModel::listValues() const {
  return m_monitors;
}

void HyprMonitorsModel::processMonitorData(QByteArray data) {
  QJsonParseError parseError;
  auto            jDoc = QJsonDocument::fromJson(data, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qCWarning(logNSHyprland) << "Error parsing monitor data from hyprland.";
    return;
  }

  auto old = m_monitors;
  m_monitors.clear();

  auto jArray = jDoc.array();
  for (auto entry : jArray) {
    auto jObj = entry.toObject();
    if (jObj.isEmpty()) continue;

    auto id   = jObj.value("id").toInt(-1);
    auto name = jObj.value("name").toString();
    if (id == -1 || name.isEmpty()) continue;

    HyprMonitor *monitor = nullptr;

    auto it = std::ranges::find_if(
        m_monitors.begin(), m_monitors.end(), [id](HyprMonitor *mon) {
          return mon->id() == id;
        });

    if (it != m_monitors.end()) {
      old.erase(it);
      monitor = *it;
    } else {
      monitor = new HyprMonitor(id, this);
    }

    common::HyprMonitorData data;
    data.id       = id;
    data.name     = name;
    data.model    = jObj.value("model").toString();
    data.serial   = jObj.value("serial").toString();
    data.width    = jObj.value("width").toInt();
    data.height   = jObj.value("height").toInt();
    data.x        = jObj.value("x").toInt();
    data.y        = jObj.value("y").toInt();
    data.disabled = jObj.value("disabled").toBool();

    monitor->processData(std::move(data));

    m_monitors.append(monitor);
  }

  std::sort(m_monitors.begin(),
            m_monitors.end(),
            [](HyprMonitor *a, HyprMonitor *b) { return a->id() < b->id(); });

  for (auto *cleanup : old) {
    cleanup->deleteLater();
  }

  emit valuesChanged();
}

void HyprMonitorsModel::removeMonitorById(int id,
                                          const QString & /*unused*/) {
  auto it =
      std::ranges::find_if(m_monitors.begin(),
                           m_monitors.end(),
                           [id](HyprMonitor *mon) { return mon->id() == id; });

  if (it != m_monitors.end()) {
    m_monitors.erase(it);
    emit valuesChanged();
  }
}
} // namespace ns::hyprland
