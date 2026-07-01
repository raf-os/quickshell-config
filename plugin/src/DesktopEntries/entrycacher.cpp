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

std::optional<QHash<QString,
                    EntryData>>
EntryCacher::readFromCache() {
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

  auto                      jRoot = jDoc.object();
  QHash<QString, EntryData> data;

  for (auto it = jRoot.constBegin(); it != jRoot.constEnd(); ++it) {
    if (!it.value().isObject())
      continue;
    auto id    = it.key();
    auto _data = it.value().toObject();

    auto name    = _data.value("Name").toString();
    auto execCmd = _data.value("Exec").toString();
    if (name.isEmpty() || execCmd.isEmpty()) {
      continue;
    }
    auto noDisplay        = _data.value("NoDisplay").toBool();
    auto isTerminal       = _data.value("Terminal").toBool();
    auto genericName      = _data.value("GenericName").toString();
    auto comment          = _data.value("Comment").toString();
    auto icon             = _data.value("Icon").toString();
    auto workingDirectory = _data.value("Path").toString();
    auto actions =
        _data.value("Actions").toString().split(";", Qt::SkipEmptyParts);
    auto categories =
        _data.value("Categories").toString().split(";", Qt::SkipEmptyParts);
    auto keywords =
        _data.value("Keywords").toString().split(";", Qt::SkipEmptyParts);

    EntryData eData;
    eData.id               = id;
    eData.name             = name;
    eData.genericName      = genericName;
    eData.execStr          = execCmd;
    eData.noDisplay        = noDisplay;
    eData.terminal         = isTerminal;
    eData.comment          = comment;
    eData.icon             = icon;
    eData.workingDirectory = workingDirectory;
    eData.categories       = categories;
    eData.keywords         = keywords;
  }

  return data;
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
