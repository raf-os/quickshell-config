#include "wallpapermanager.h"
#include "paths.h"
#include "wallpapercommon.h"
#include "wallpapermeta.h"

#include <QtCore>
#include <algorithm>
#include <qbuffer.h>
#include <qdebug.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qfilesystemwatcher.h>
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
#include <qtimer.h>

namespace ns::wallpaper {
Q_LOGGING_CATEGORY(logNSWallpaper,
                   "nightshell.wallpaper")

WallpaperManager::WallpaperManager(QObject *parent) : QObject(parent) {
  m_switchTimer.setSingleShot(true);
  QObject::connect(
      &m_switchTimer, &QTimer::timeout, this, &WallpaperManager::moveForward);

  m_fileDebounceTimer.setSingleShot(true);
  m_fileDebounceTimer.setInterval(300);
  QObject::connect(&m_fileDebounceTimer,
                   &QTimer::timeout,
                   this,
                   &WallpaperManager::parseConfig);

  QString cfgPath =
      myqmlplugin::utils::Paths::instance()->config() + "/wallpapers.json";

  if (QFileInfo(cfgPath).exists()) {
    m_configPath = cfgPath;
    m_fileWatcher.addPath(cfgPath);
    parseConfig();
  }

  QObject::connect(&m_fileWatcher,
                   &QFileSystemWatcher::fileChanged,
                   this,
                   [this]() { this->m_fileDebounceTimer.start(); });
}

WallpaperMeta *WallpaperManager::current() {
  if (m_currentIndex < 0 || m_currentIndex > m_instances.length())
    return nullptr;

  return m_instances[m_currentIndex];
}

WallpaperMeta *WallpaperManager::moveForward() {
  WallpaperMeta *next = nullptr;

  m_switchTimer.stop();

  if (m_instances.length() == 0)
    return next;

  const auto cur = current();

  if (m_instances.length() < 2) {
    return cur;
  }

  m_currentIndex = (m_currentIndex + 1) % m_instances.length();
  next           = m_instances[m_currentIndex];

  if (next != cur) {
    emit currentChanged();
  }

  if (next) {
    setupTimer();
  }

  return next;
}

void WallpaperManager::parseConfig() {
  QFile cfgFile(m_configPath);

  if (!cfgFile.exists()) {
    qCWarning(logNSWallpaper)
        << "ns::wallpaper::WallpaperManager::parseConfig: Non-existant "
           "wallpaper config file provided:"
        << m_configPath;
    return;
  }

  if (!cfgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qCWarning(logNSWallpaper)
        << "Unable to open wallpaper config file for parsing.";
    this->attachWatcher();
    return;
  }

  QByteArray fileBuffer(cfgFile.size(), Qt::Uninitialized);
  cfgFile.read(fileBuffer.data(), fileBuffer.size());

  cfgFile.close();

  QJsonParseError parseError;
  auto            jDoc = QJsonDocument::fromJson(fileBuffer, &parseError);

  if (parseError.error == QJsonParseError::NoError) {
    if (jDoc.isArray()) {
      auto                    arr = jDoc.array();
      QList<ParsedJsonObject> parseBuffer;

      for (auto &old : m_instances) {
        old->deleteLater();
      }

      m_instances.clear();
      m_switchTimer.stop();

      for (const auto &item : arr) {
        if (!item.isObject())
          continue;

        const QJsonObject obj = item.toObject();

        auto path = obj.value("path").toString();

        if (path.isEmpty())
          continue;

        auto interval = obj.value("interval").toInt(-1);
        auto fillMode = obj.value("fillMode").toString("fit");

        parseBuffer.append({path, fillMode, interval});
      }

      for (auto item : parseBuffer) {
        if (!QFileInfo(item.path).exists())
          continue;

        auto wpp = new WallpaperMeta(item.path, this);
        wpp->setInterval(std::min(std::max(-1, item.interval), 2147483));
        wpp->setFillMode(item.fillMode);
        m_instances.append(wpp);
      }
    } else {
      qCWarning(logNSWallpaper) << "Malformed wallpaper config file.";
      this->attachWatcher();
      return;
    }
  } else {
    qCWarning(logNSWallpaper)
        << "Error parsing wallpaper config: " << parseError.errorString();
    this->attachWatcher();
    return;
  }

  m_currentIndex = 0;
  emit currentChanged();

  setupTimer();

  this->attachWatcher();
}

void WallpaperManager::setupTimer() {
  m_switchTimer.stop();

  if (m_instances.length() < 2)
    return;

  auto cur = current();

  if (!cur)
    return;
  if (cur->interval() < 1)
    return;

  m_switchTimer.setInterval(std::max(cur->interval() * 1000, 5000));
  m_switchTimer.start();
}

void WallpaperManager::forceSingleWallpaper(const QString &path,
                                            const QString &fillMode) {
  if (m_instances.length() == 1) {
    auto existingInstance = m_instances[0];
    if (existingInstance->path() == path) {
      existingInstance->setFillMode(fillMode);
      saveConfigToFile();
      return;
    }
  }

  const QFileInfo desiredFile(path);

  if (!desiredFile.exists()) {
    qCWarning(logNSWallpaper)
        << "ns::wallpapers::WallpaperManager::forceSingleWallpaper: Requested "
           "path does not exist.";
    return;
  }

  const QList<QString> allowedFileTypes{"jpg", "jpeg", "png"};

  if (!allowedFileTypes.contains(desiredFile.suffix())) {
    qCWarning(logNSWallpaper)
        << "ns::wallpapers::WallpaperManager::forceSingleWallpaper: Provided "
           "file type is not supported.";
    return;
  }

  for (auto instance : m_instances) {
    instance->deleteLater();
  }

  m_instances.clear();
  auto newInstance = new WallpaperMeta(path, this);
  newInstance->setFillMode(fillMode);
  newInstance->setInterval(-1);
  m_instances.append(newInstance);

  saveConfigToFile();
  emit currentChanged();
}

void WallpaperManager::saveConfigToFile() {
  QSaveFile cfgFile(m_configPath, this);

  QJsonArray wpArray;

  for (const auto instance : m_instances) {
    const QJsonObject wpinfoObj{
        {"path",     instance->path()                                 },
        {"fillMode", WallpaperFillMode::toString(instance->fillMode())},
        {"interval", instance->interval()                             }
    };
    wpArray.append(QJsonValue(wpinfoObj));
  }

  QByteArray jDoc = QJsonDocument(wpArray).toJson();

  if (!cfgFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qCWarning(logNSWallpaper) << "ns::wallpaper::WallpaperManager: Unable to "
                                 "open config file for saving.";
    return;
  }

  QTextStream out(&cfgFile);
  out << jDoc;

  if (!cfgFile.commit()) {
    qCWarning(logNSWallpaper)
        << "ns::wallpaper::WallpaperManager: Saving config file failed. Check "
           "if folder has correct permissions or if the partition it's located "
           "at is full.";
    return;
  }
}

void WallpaperManager::triggerParse() { return parseConfig(); }

void WallpaperManager::attachWatcher() {
  QFileInfo cfgFile(m_configPath);
  if (!cfgFile.exists())
    return;

  if (!m_fileWatcher.files().contains(m_configPath)) {
    m_fileWatcher.addPath(m_configPath);
  }
}
} // namespace ns::wallpaper
