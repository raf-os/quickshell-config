#include "entrycacher.h"
#include "entryscanner.h"
#include "entryutils.h"
#include "paths.h"

#include <optional>
#include <qbuffer.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstringview.h>
#include <utility>

namespace ns::desktop::entries {
Q_DECLARE_LOGGING_CATEGORY(logNSDesktopEntries) // from entrymanager.cpp

EntryCacher::EntryCacher(QObject *parent) : QObject(parent) {}

// If `true` is returned, it means cache values are still ok
bool EntryCacher::isCacheValid() {
  const auto cachePath = myqmlplugin::utils::Paths::instance()->cache();

  QFile cacheFile(cachePath + "/" + m_dateCacheFilename);
  if (!cacheFile.exists())
    return false;
  if (!cacheFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QByteArray readBuffer(cacheFile.size(), Qt::Uninitialized);
  cacheFile.read(readBuffer.data(), readBuffer.size());

  QJsonParseError parseError;
  auto            jDoc = QJsonDocument::fromJson(readBuffer, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    qCWarning(logNSDesktopEntries)
        << "EntryCacher: error parsing date cache json:"
        << parseError.errorString();
    return false;
  }

  if (!jDoc.isObject()) {
    qCWarning(logNSDesktopEntries)
        << "EntryCacher: invalid json file provided.";
    return false;
  }

  auto jObj = jDoc.object();

  bool       shouldChange = false;
  const auto entryPaths   = EntryUtils::desktopPaths();
  for (const auto &path : entryPaths) {
    auto it = jObj.constFind(path);

    if (it == jObj.constEnd()) {
      shouldChange = true;
      break;
    }

    QFileInfo _dir(path);
    if (!_dir.exists())
      continue;

    auto lastModified = _dir.lastModified().toSecsSinceEpoch();
    auto cacheDate    = it.value().toString().toLongLong();

    if (lastModified > cacheDate) {
      shouldChange = true;
      break;
    }
  }

  return shouldChange;
}

std::optional<QList<EntryData>> EntryCacher::readFromCache() {
  const auto cachePath = myqmlplugin::utils::Paths::instance()->cache();
  QFile      file(cachePath + "/" + m_caheFilename);

  if (!file.exists())
    return std::nullopt;

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return std::nullopt;
  }

  QByteArray buffer(file.size(), Qt::Uninitialized);
  file.read(buffer.data(), buffer.size());

  QJsonParseError parseError;
  auto            jDoc = QJsonDocument::fromJson(buffer, &parseError);

  if (parseError.error != QJsonParseError::NoError) {
    return std::nullopt;
  } else if (!jDoc.isObject()) {
    return std::nullopt;
  }

  auto             jRoot = jDoc.object();
  QList<EntryData> data;

  for (auto it = jRoot.constBegin(); it != jRoot.constEnd(); ++it) {
    if (!it.value().isObject())
      continue;
    auto id = it.key();
    // if (data.contains(id)) {
    //   qCDebug(logNSDesktopEntries)
    //       << "EntryCacher::readFromCache: Skipping duplicate entry" << id;
    //   continue;
    // }
    auto _data = it.value().toObject();

    auto name    = _data.value("Name").toString();
    auto execCmd = _data.value("Exec").toString();
    if (name.isEmpty() || execCmd.isEmpty()) {
      continue;
    }
    auto noDisplay        = _data.value("NoDisplay").toBool();
    auto hidden           = _data.value("Hidden").toBool();
    auto isTerminal       = _data.value("Terminal").toBool();
    auto genericName      = _data.value("GenericName").toString();
    auto comment          = _data.value("Comment").toString();
    auto icon             = _data.value("Icon").toString();
    auto workingDirectory = _data.value("Path").toString();
    auto startupClass     = _data.value("StartupWMClass").toString();
    auto categories =
        _data.value("Categories").toString().split(";", Qt::SkipEmptyParts);
    auto keywords =
        _data.value("Keywords").toString().split(";", Qt::SkipEmptyParts);
    auto command = EntryUtils::parseExecString(execCmd);

    EntryData eData;
    eData.id               = id;
    eData.name             = name;
    eData.genericName      = genericName;
    eData.execStr          = execCmd;
    eData.command          = command;
    eData.noDisplay        = noDisplay;
    eData.hidden           = hidden;
    eData.terminal         = isTerminal;
    eData.comment          = comment;
    eData.icon             = icon;
    eData.workingDirectory = workingDirectory;
    eData.startupClass     = startupClass;
    eData.categories       = categories;
    eData.keywords         = keywords;

    auto _aList = _data.value("Actions");
    if (_aList.isArray()) {
      auto actionList = _aList.toObject();

      for (const auto &act : actionList) {
        if (!act.isObject())
          continue;
        auto actData = act.toObject();

        auto actName    = actData.value("Name").toString();
        auto actIcon    = actData.value("Icon").toString();
        auto actExecStr = actData.value("Exec").toString();
        if (actName.isEmpty() || actExecStr.isEmpty()) {
          // invalid action
          continue;
        }
        auto actExecList = EntryUtils::parseExecString(actExecStr);

        EntryActionData aData;
        aData.name       = actName;
        aData.icon       = actIcon;
        aData.execString = actExecStr;
        aData.command    = actExecList;

        eData.actions.append(std::move(aData));
      }
    }

    data.append(std::move(eData));
  }

  return data;
}

void EntryCacher::saveToCache(const QList<EntryData> &data) {
  QJsonObject jsonRoot;

  for (const auto &entry : data) {
    QJsonObject jEntry;

    auto eId = entry.id;
    if (jsonRoot.contains(eId))
      continue;

    jEntry.insert("Name", QJsonValue(entry.name));
    jEntry.insert("GenericName", QJsonValue(entry.genericName));
    jEntry.insert("Icon", QJsonValue(entry.icon));
    jEntry.insert("StartupWMClass", QJsonValue(entry.startupClass));
    jEntry.insert("Comment", QJsonValue(entry.comment));
    jEntry.insert("Exec", QJsonValue(entry.execStr));
    jEntry.insert("Path", QJsonValue(entry.workingDirectory));
    jEntry.insert("Categories", QJsonValue(entry.categories.join(';')));
    jEntry.insert("Keywords", QJsonValue(entry.keywords.join(';')));
    jEntry.insert("NoDisplay", QJsonValue(entry.noDisplay));
    jEntry.insert("Hidden", QJsonValue(entry.hidden));
    jEntry.insert("Terminal", QJsonValue(entry.terminal));

    QJsonArray jActionList;
    if (!entry.actions.isEmpty()) {
      for (const auto &eAction : entry.actions) {
        QJsonObject jAction;
        jAction.insert("Name", QJsonValue(eAction.name));
        jAction.insert("Icon", QJsonValue(eAction.icon));
        jAction.insert("Exec", QJsonValue(eAction.execString));
        jActionList.append(std::move(jAction));
      }
    }
    jEntry.insert("Actions", jActionList);

    jsonRoot.insert(eId, jEntry);
  }

  const auto cachePath = myqmlplugin::utils::Paths::instance()->cache();
  QFile      file(cachePath + "/" + m_caheFilename);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qCWarning(logNSDesktopEntries)
        << "Unable to save application database to cache.";
    return;
  }

  auto fileData = QJsonDocument(jsonRoot).toJson(QJsonDocument::Compact);
  file.write(fileData);

  this->recordDirectoryModificationDates();
}

void EntryCacher::recordDirectoryModificationDates() {
  const auto paths = EntryUtils::desktopPaths();

  QJsonObject jObj;

  for (const auto &path : paths) {
    QFileInfo fileInfo(path);

    if (!fileInfo.exists())
      continue;

    auto curModDate =
        QString::number(fileInfo.lastModified().toSecsSinceEpoch());
    jObj.insert(path, QJsonValue(curModDate));
  }

  const auto cachePath = myqmlplugin::utils::Paths::instance()->cache();
  QFile      file(cachePath + "/" + m_dateCacheFilename);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qCWarning(logNSDesktopEntries)
        << "EntryCacher: error opening date cache file for writing.";
    return;
  }

  auto jDoc = QJsonDocument(jObj).toJson(QJsonDocument::Compact);
  file.write(jDoc);
}
} // namespace ns::desktop::entries
