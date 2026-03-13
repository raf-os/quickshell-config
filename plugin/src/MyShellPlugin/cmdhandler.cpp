#include "cmdhandler.h"

#include <algorithm>
#include <qcontainerfwd.h>
#include <qdir.h>
#include <qdirlisting.h>
#include <qfiledevice.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmllist.h>

namespace myqmlplugin {
CmdEntry::CmdEntry(QString basePath, QJsonObject jsonData, QObject *parent)
    : QObject(parent), m_data(jsonData), m_path(basePath) {
  if (!jsonData.contains("prefix") || !jsonData.contains("path")) {
    qWarning() << "Invalid config JSON config detected. Aborting.";
    deleteLater();
    return;
  }

  if (const QJsonValue v = jsonData["path"]; v.isString()) {
    QDir baseDir(basePath);
    m_path = QFileInfo(baseDir, v.toString()).canonicalPath();
    QString scriptName = jsonData["prefix"].toString("<<INVALID SCRIPT>>");

    if (m_path == "") {
      qWarning() << "Script path for " << scriptName
                 << " is invalid. Aborting.";
      deleteLater();
      return;
    }
  } else {
    qWarning()
        << "myqmlplugin::CmdEntry: Undefined behavior occurred in constructor.";
  }
}

QString CmdEntry::prefix() const { return m_data["prefix"].toString(); }

QString CmdEntry::separator() const {
  return m_data["separator"].toString(" ");
}

QString CmdEntry::path() const { return m_path; }

QString CmdEntry::description() const {
  return m_data["description"].toString();
}

QString CmdEntry::label() const { return m_data["label"].toString(); }

bool CmdEntry::isCoreCommand() const { return m_isCoreCommand; }

void CmdEntry::setIsCoreCommand(bool coreCommand) {
  m_isCoreCommand = coreCommand;
}

CmdHandler::CmdHandler(QObject *parent) : QObject(parent) {}

QString CmdHandler::path() const { return m_path; }

void CmdHandler::setPath(const QString &newPath) {
  if (m_path == newPath) {
    return;
  }

  m_path = newPath;
  emit pathChanged();
}

QString CmdHandler::basePath() const { return m_path; }

void CmdHandler::setBasePath(const QString &newPath) {
  if (m_basePath == newPath) {
    return;
  }

  m_basePath = newPath;
  emit basePathChanged();
}

void CmdHandler::refreshCommandList() {
  qDebug() << "Refreshing command list from files...";
  updateEntries();
}

QList<CmdEntry *> &CmdHandler::getSortedCommands() {
  m_sortedCommands = m_cmdEntries.values();
  std::sort(m_sortedCommands.begin(), m_sortedCommands.end(),
            [this](CmdEntry *a, CmdEntry *b) {
              if (a->label() == "")
                return true;
              if (b->label() == "")
                return false;
              return a->label().localeAwareCompare(b->label()) < 0;
            });
  return m_sortedCommands;
};

QQmlListProperty<CmdEntry> CmdHandler::entries() {
  return QQmlListProperty<CmdEntry>(this, &getSortedCommands());
}

QString CmdHandler::queryString() { return m_queryString; }

void CmdHandler::setQueryString(const QString &newQuery) {
  // TODO: This
}

void CmdHandler::updateEntries() {
  bool dirty;
  QSet<QString> allCommands;

  allCommands.unite(pathIterate(m_basePath, &dirty, [](CmdEntry *entry) {
    entry->setIsCoreCommand(true);
  }));
  allCommands.unite(pathIterate(m_path, &dirty));

  // Iterates and removes all non-existing commands
  for (auto it = m_cmdEntries.keyBegin(); it != m_cmdEntries.keyEnd(); ++it) {
    const auto &id = *it;
    if (!allCommands.contains(id)) {
      dirty = true;
      m_cmdEntries.take(id)->deleteLater();
    }
  }

  if (dirty) {
    emit entriesChanged();
  }
}

/**
 * Checks the provided path for all .json files, and returns a list of all
 * script names.
 * NOTE: This is a glorified lambda function. The calling function should then
 * clean up non-existing entries.
 */
template <typename Func>
QSet<QString> CmdHandler::pathIterate(const QString &path, bool *isDirty,
                                      Func callback) {
  QSet<QString> allReadFiles;

  if (!QDir(path).exists()) {
    qWarning() << "myqmlplugin::CmdHandler::pathIterate found invalid path: "
               << path << ". Aborting iteration.";
    return allReadFiles;
  }

  using ItFlag = QDirListing::IteratorFlag;
  for (const auto &dirEntry : QDirListing(path, ItFlag::ResolveSymlinks)) {
    if (!dirEntry.fileName().endsWith(u".json")) {
      continue;
    }

    QFile f(dirEntry.filePath());
    QStringList lines;

    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QByteArray fileBuffer;
      while (!f.atEnd()) {
        fileBuffer.append(f.readLine());
      }
      f.close();

      QJsonParseError parseError;
      auto jDoc = QJsonDocument::fromJson(fileBuffer, &parseError);

      if (jDoc.isNull()) {
        qWarning() << "Error parsing JSON document: "
                   << parseError.errorString();
        continue;
      }

      // At this point, all should be good
      if (jDoc.isObject()) {
        QJsonObject obj = jDoc.object();
        if (const QJsonValue v = obj["prefix"]; v.isString()) {
          const auto cmdName = v.toString();
          allReadFiles.insert(cmdName);

          if (!m_cmdEntries.contains(cmdName)) {
            if (isDirty != nullptr)
              *isDirty = true;

            auto *newEntry = new CmdEntry(path, obj, this);
            QObject::connect(newEntry, &QObject::destroyed, this,
                             [cmdName, this]() {
                               if (m_cmdEntries.remove(cmdName)) {
                                 emit entriesChanged();
                               }
                             });
            m_cmdEntries.insert(cmdName, newEntry);

            callback(newEntry);
          }
        }
      }
    }
  }

  return allReadFiles;
}

QSet<QString> CmdHandler::pathIterate(const QString &path, bool *isDirty) {
  return pathIterate(path, isDirty, [](CmdEntry *) {});
}
} // namespace myqmlplugin
