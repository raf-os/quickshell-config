#include "statemanager.h"

#include <optional>
#include <utility>

#include <QtCore>
#include <qbuffer.h>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qjsengine.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qloggingcategory.h>
#include <qobject.h>
#include <qqmlengine.h>
#include <qtimer.h>

#include "helpermacros.h"
#include "paths.h"

namespace ns::utils {
Q_LOGGING_CATEGORY(logNSUtilsStateManager, "ns.utils.statemanager")

namespace {
static const QString USER_DATA_FILE = "userdata.json";
}

StateManager::StateManager(QObject *parent) : QObject(parent) {
  onStatePathChanged();
  QObject::connect(Paths::instance(), &Paths::stateChanged, this,
      &StateManager::onStatePathChanged);

  m_saveTimer.setInterval(5000);
  m_saveTimer.setSingleShot(true);
  QObject::connect(
      &m_saveTimer, &QTimer::timeout, this, &StateManager::onSaveTimerTimeout);
}

AUTO_MEYERS_SINGLETON_QML_IMPL(StateManager)

QStringList StateManager::favoriteApps() const { return m_favoriteApps; }

void StateManager::addFavoriteApp(const QString &appId) {
  if (!m_favoriteApps.contains(appId)) {
    m_favoriteApps.append(appId);
    emit favoriteAppsChanged();
  }
  queueSave();
}

void StateManager::removeFavoriteApp(const QString &appId) {
  if (m_favoriteApps.removeOne(appId)) {
    emit favoriteAppsChanged();
  }
  queueSave();
}

std::optional<UserStateData> StateManager::readUserStateData(
    const QString &path) {
  QFile userStateFile(path);

  if (!userStateFile.exists()) {
    qCDebug(logNSUtilsStateManager) << "User state file does not exist.";
    return std::nullopt;
  }

  if (!userStateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCWarning(logNSUtilsStateManager)
        << "Error opening user state file for reading.";
    return std::nullopt;
  }

  auto            data = userStateFile.readAll();
  QJsonParseError error;
  auto            jDoc = QJsonDocument::fromJson(data, &error);

  if (error.error != QJsonParseError::NoError) {
    qCWarning(logNSUtilsStateManager)
        << "Error parsing user state file json:" << error.errorString();
    return std::nullopt;
  }

  if (!jDoc.isObject()) {
    qCWarning(logNSUtilsStateManager) << "User state file is malformed";
    return std::nullopt;
  }

  auto          jObj = jDoc.object();
  UserStateData stateData;

  auto favs = jObj.value("favoriteApps").toArray();
  stateData.favoriteApps.reserve(favs.size());
  for (auto fav : favs) {
    if (!fav.isString()) continue;
    auto favstr = fav.toString();
    if (!favstr.isEmpty()) stateData.favoriteApps.append(favstr);
  }
  stateData.favoriteApps.removeDuplicates();

  return std::move(stateData);
}

void StateManager::writeUserStateData(const QString &path) {
  QJsonObject obj;
  obj.insert("favoriteApps", QJsonArray::fromStringList(m_favoriteApps));

  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly)) {
    qCWarning(logNSUtilsStateManager)
        << "Unable to open user state file for writing.";
    return;
  }

  file.write(QJsonDocument(obj).toJson(QJsonDocument::Compact));

  if (!file.commit()) {
    qCWarning(logNSUtilsStateManager) << "Unable to save user state data.";
  }
}

void StateManager::onStatePathChanged() {
  const auto path = Paths::instance()->bindableState().value();

  auto uData = readUserStateData(path + "/" + USER_DATA_FILE);
  if (uData.has_value()) {
    auto d = uData.value();
    if (m_favoriteApps != d.favoriteApps) {
      m_favoriteApps = d.favoriteApps;
      emit favoriteAppsChanged();
    }
  }
}

void StateManager::onSaveTimerTimeout() {
  const auto path = Paths::instance()->bindableState().value();

  writeUserStateData(path + "/" + USER_DATA_FILE);
}

void StateManager::queueSave() { m_saveTimer.start(); }
} // namespace ns::utils
