#include "cmdhandler.h"

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
#include <qlogging.h>
#include <qobject.h>
#include <qprocess.h>
#include <qqmllist.h>

namespace myqmlplugin {
CmdEntry::CmdEntry(QString basePath, QJsonObject jsonData, QObject *parent)
    : QObject(parent), m_data(jsonData), m_path(basePath) {
  if (!jsonData.contains("prefix") || !jsonData.contains("path")) {
    qWarning() << "Invalid config JSON config detected. Aborting. "
               << jsonData["prefix"].toString("(NO FUNCTION PREFIX)");
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

QString CmdEntry::icon() const { return m_data["icon"].toString(); }

int CmdEntry::arguments() const { return m_data["arguments"].toInt(0); }

bool CmdEntry::captureOutput() const {
  return m_data["captureOutput"].toBool(false);
}

bool CmdEntry::isCoreCommand() const { return m_isCoreCommand; }

void CmdEntry::setIsCoreCommand(bool coreCommand) {
  m_isCoreCommand = coreCommand;
}

// =============================================

CmdHandler::CmdHandler(QObject *parent) : QObject(parent) {}

QString CmdHandler::path() const { return m_path; }

void CmdHandler::setPath(const QString &newPath) {
  if (m_path == newPath) {
    return;
  }

  updateEntries(EntryUpdateFlags::SkipBasePath);
  m_path = newPath;
  emit pathChanged();
}

QString CmdHandler::basePath() const { return m_path; }

void CmdHandler::setBasePath(const QString &newPath) {
  if (m_basePath == newPath) {
    return;
  }

  updateEntries(EntryUpdateFlags::SkipPath);
  m_basePath = newPath;
  emit basePathChanged();
}

void CmdHandler::refreshCommandList() {
  qDebug() << "Refreshing command list from files...";
  updateEntries();
}

QList<CmdEntry *> &CmdHandler::getSortedCommands() {
  if (!m_shouldReSort) {
    return m_sortedCommands;
  }

  m_sortedCommands = m_cmdEntries.values();
  std::sort(m_sortedCommands.begin(), m_sortedCommands.end(),
            [this](CmdEntry *a, CmdEntry *b) {
              if (a->label() == "")
                return true;
              if (b->label() == "")
                return false;
              return a->label().localeAwareCompare(b->label()) < 0;
            });
  m_shouldReSort = false;
  return m_sortedCommands;
};

QQmlListProperty<CmdEntry> CmdHandler::entries() {
  return QQmlListProperty<CmdEntry>(this, &getFilteredCommands());
}

QString CmdHandler::queryString() const { return m_queryString; }

void CmdHandler::setQueryString(const QString &newQuery) {
  if (newQuery == m_queryString)
    return;

  m_queryString = newQuery;
  emit queryStringChanged();
  emit entriesChanged();
}

QList<CmdEntry *> &CmdHandler::getFilteredCommands() {
  if (m_queryString == "") {
    m_filteredCommands = getSortedCommands();
    return m_filteredCommands;
  }

  m_filteredCommands.clear();

  for (const auto &entry : getSortedCommands()) {
    const auto cmdPrefix = entry->prefix();
    QString sliced = m_queryString;
    if (m_queryString.length() > cmdPrefix.length()) {
      // Without this, the function was reading memory out of bounds
      // This should fix it
      sliced.slice(0, cmdPrefix.length());
    }
    if (cmdPrefix.startsWith(sliced)) {
      m_filteredCommands.append(entry);
    }
  }

  return m_filteredCommands;
}

void CmdHandler::updateEntries(int flag) {
  bool dirty;
  QSet<QString> allCommands;

  if ((flag & EntryUpdateFlags::SkipBasePath) !=
      EntryUpdateFlags::SkipBasePath) {
    allCommands.unite(pathIterate(m_basePath, &dirty, [](CmdEntry *entry) {
      entry->setIsCoreCommand(true);
    }));
  }
  if ((flag & EntryUpdateFlags::SkipPath) != EntryUpdateFlags::SkipPath) {
    allCommands.unite(pathIterate(m_path, &dirty));
  }

  // Iterates and removes all non-existing commands
  for (auto it = m_cmdEntries.keyBegin(); it != m_cmdEntries.keyEnd(); ++it) {
    const auto &id = *it;
    if (!allCommands.contains(id)) {
      dirty = true;
      m_cmdEntries.take(id)->deleteLater();
    }
  }

  if (dirty) {
    m_shouldReSort = true;
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

bool CmdHandler::isProcessRunning() { return m_runningProcess != nullptr; }

QList<QString> CmdHandler::processOutput() const { return m_processOutput; }

QVariantMap CmdHandler::executeCommand(const QString &command) {
  QVariantMap responseBuffer;
  responseBuffer.insert("success", false);
  if (isProcessRunning()) {
    responseBuffer.insert("message", "A process is already running.");
    return responseBuffer;
  }

  CmdEntry *entryBuffer = nullptr;

  for (auto it = m_cmdEntries.cbegin(); it != m_cmdEntries.cend(); ++it) {
    const auto &id = it.key();
    if (command.startsWith(id)) {
      entryBuffer = it.value();
      break;
    }
  }

  if (entryBuffer == nullptr) {
    responseBuffer.insert("message", "Command does not exist.");
    return responseBuffer;
  }

  const auto cmdPrefixLength = entryBuffer->prefix().length();

  if (cmdPrefixLength > command.length()) {
    qDebug()
        << "myqmlplugin::CmdHandler::executeCommand potential buffer overflow."
        << " Command name definition is larger than the actual command "
           "received."
        << " Aborting.";
    responseBuffer.insert(
        "messagge", "Something fucky is going on. Aborting now to prevent "
                    "buffer overflow.");
    return responseBuffer;
  }

  QString cmdArgs = command.sliced(entryBuffer->prefix().length()).trimmed();
  QList<QString> args;

  if (cmdArgs.length() > 0 && entryBuffer->separator().length() > 0) {
    qDebug() << cmdArgs;
    auto splitArgs = cmdArgs.split(entryBuffer->separator());
    if (splitArgs.length() > entryBuffer->arguments()) {
      responseBuffer.insert(
          "message",
          QString("Invalid amount of arguments for command. Expected "
                  "maximum of %1, received %2.")
              .arg(entryBuffer->arguments())
              .arg(splitArgs.length()));
      return responseBuffer;
    }
    args = splitArgs;
  } else {
    args = QList<QString>({cmdArgs});
  }

  m_processOutput.clear();

  m_runningProcess = new QProcess(this);
  m_runningProcess->setProgram(entryBuffer->path());
  m_runningProcess->setArguments(args);
  QObject::connect(m_runningProcess, &QProcess::finished, this, [this]() {
    emit isProcessRunningChanged();
    m_runningProcess->deleteLater();
    m_runningProcess = nullptr;
  });
  QObject::connect(
      m_runningProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        auto output =
            QString::fromUtf8(m_runningProcess->readAllStandardOutput());
        m_processOutput.append(output);
        emit processOutputChanged();
      });
  QObject::connect(
      m_runningProcess, &QProcess::readyReadStandardError, this, [this]() {
        auto output =
            QString::fromUtf8(m_runningProcess->readAllStandardError());
        m_processOutput.append("[ERROR] " + output);
        emit processOutputChanged();
      });
  m_runningProcess->start();

  responseBuffer["success"] = true;
  responseBuffer.insert("captureOutput", entryBuffer->captureOutput());
  return responseBuffer;
}
} // namespace myqmlplugin
