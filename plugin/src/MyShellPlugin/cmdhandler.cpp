#include "cmdhandler.h"

#include <qcontainerfwd.h>
#include <qdir.h>
#include <qdirlisting.h>
#include <qfiledevice.h>
#include <qhash.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonparseerror.h>
#include <qjsonvalue.h>
#include <qlist.h>
#include <qobject.h>
#include <qqmllist.h>

namespace myqmlplugin {
CmdHandler::CmdHandler(QObject *parent) : QObject(parent) {}

QString CmdHandler::path() const { return m_path; }

void CmdHandler::setPath(const QString &newPath) {
  if (m_path == newPath) {
    return;
  }

  m_path = newPath;
  emit pathChanged();
}

QQmlListProperty<CmdEntry> CmdHandler::entries() {
  auto ent = m_cmdEntries.values();
  return QQmlListProperty<CmdEntry>(this, &ent);
}

/**
 * Checks the provided path for all .json files, and returns a list of all
 * script names.
 * NOTE: This is a glorified lambda function. The calling function should then
 * clean up non-existing entries.
 */
QSet<QString> CmdHandler::pathIterate(const QString &path, bool *isDirty) {
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
        return allReadFiles;
      }

      // At this point, all should be good
      if (jDoc.isObject()) {
        QJsonObject obj = jDoc.object();
        if (const QJsonValue v = obj["name"]; v.isString()) {
          const auto cmdName = v.toString();
          allReadFiles.insert(cmdName);

          if (!m_cmdEntries.contains(cmdName)) {
            if (isDirty != nullptr)
              *isDirty = true;

            auto *newEntry = new CmdEntry(path, this);
            QObject::connect(newEntry, &QObject::destroyed, this,
                             [cmdName, this]() {
                               if (m_cmdEntries.remove(cmdName)) {
                                 emit entriesChanged();
                               }
                             });
            m_cmdEntries.insert(cmdName, newEntry);
          }
        }
      }
    }
  }

  return allReadFiles;
}
} // namespace myqmlplugin
