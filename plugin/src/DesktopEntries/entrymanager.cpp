#include "entrymanager.h"
#include "desktopentry.h"
#include "entryaction.h"
#include "entrycacher.h"
#include "entrymonitor.h"
#include "entryscanner.h"
#include "paths.h"

#include <algorithm>
#include <qbuffer.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qdir.h>
#include <qdiriterator.h>
#include <qfileinfo.h>
#include <qhash.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpair.h>
#include <qprocess.h>
#include <qscopeguard.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <qstringview.h>
#include <qtenvironmentvariables.h>
#include <qthreadpool.h>
#include <qtypes.h>
#include <quuid.h>

namespace ns::desktop::entries {
Q_LOGGING_CATEGORY(logNSDesktopEntries,
                   "nightshell.desktop.entries")

EntryManager::EntryManager(QObject *parent)
    : QObject(parent),
      m_monitor(new EntryMonitor(this)),
      m_entryCacher(new EntryCacher(this)),
      m_uuid(QUuid::createUuid().toString()) {
  QObject::connect(m_monitor,
                   &EntryMonitor::entriesChanged,
                   this,
                   &EntryManager::scanDesktopEntries);

  auto isCacheValid = m_entryCacher->isCacheValid();
  if (isCacheValid) {
    qCDebug(logNSDesktopEntries)
        << "Desktop entry cache found: attempting to read from cache...";
    auto results = m_entryCacher->readFromCache();
    if (results.has_value()) {
      m_scanInProgress = true;
      this->processEntryList(results.value());
      qCDebug(logNSDesktopEntries)
          << "Succesfully read desktop entry list from cache!";
    } else {
      qCDebug(logNSDesktopEntries)
          << "Failed acquiring entry cache. Scanning as usual.";
    }
  } else {
    this->scanDesktopEntries();
  }

  if (QSqlDatabase::drivers().contains("QSQLITE")) {
    auto db = QSqlDatabase::addDatabase("QSQLITE", m_uuid);

    auto error = this->initDb();
    if (error.type() != QSqlError::NoError) {
      qCWarning(logNSDesktopEntries)
          << "Unable to open app frequency sqlite database.";
    } else {
      m_dbSuccess = true;
    }
  } else {
    qCWarning(logNSDesktopEntries)
        << "QSQLITE driver is missing! App frequency ranking will not work!";
  }
}

void EntryManager::init() {}

QSqlError EntryManager::initDb() {
  // TODO: Logging
  auto db = QSqlDatabase::database(m_uuid, false);
  if (!db.isValid()) {
    return db.lastError();
  }
  const auto path =
      myqmlplugin::utils::Paths::instance()->state() + "/appdb.sqlite";
  db.setDatabaseName(path);
  if (!db.open()) {
    return db.lastError();
  }

  QSqlQuery query(db);
  auto      succ = query.exec("CREATE TABLE IF NOT EXISTS \
      frequencies (id TEXT PRIMARY KEY, frequency INTEGER)");

  if (!succ)
    return db.lastError();

  return QSqlError();
}

quint32 EntryManager::getFrequencyForApp(const QString &id) const {
  if (m_dbSuccess == false)
    return 0;

  auto      db = QSqlDatabase::database(m_uuid);
  QSqlQuery query(db);

  query.prepare("SELECT frequency FROM frequencies WHERE id = :id");
  query.bindValue(":id", id);

  if (query.exec() && query.next()) {
    return query.value(0).toUInt();
  }

  return 0;
}

void EntryManager::incrementFrequencyFor(DesktopEntry *target) {
  if (!m_dbSuccess && !target)
    return;

  auto      db = QSqlDatabase::database(m_uuid);
  QSqlQuery query(db);
  query.prepare("INSERT INTO frequencies (id, frequency) \
      VALUES (:id, 1) \
      ON CONFLICT (id) DO UPDATE SET frequency = frequency + 1");
  query.bindValue(":id", target->id());
  auto succ = query.exec();

  if (succ) {
    target->incrementFrequency();
    emit applicationsFrequencyChanged();
  }
}

void EntryManager::executeGeneric(const QStringList &cmd,
                                  const QString     &workingDirectory,
                                  DesktopEntry      *reference) {
  if (reference)
    this->incrementFrequencyFor(reference);

  const bool isTerminal = reference->bindableRunInTerminal().value();

  QString     app;
  QStringList args;

  if (isTerminal) {
    app  = "alacritty";
    args = cmd;
    args.prepend("-e");
  } else {
    app  = cmd.first();
    args = cmd.sliced(1);
  }

  QProcess process;
  if (!workingDirectory.isEmpty()) {
    process.setWorkingDirectory(workingDirectory);
  }

  process.setProgram(app);
  process.setArguments(args);

  process.setStandardInputFile(QProcess::nullDevice());
  process.setStandardOutputFile(QProcess::nullDevice());
  process.setStandardErrorFile(QProcess::nullDevice());

  process.startDetached();
}

void EntryManager::scanDesktopEntries() {
  qCDebug(logNSDesktopEntries) << "Starting desktop entry scan...";

  if (m_scanInProgress) {
    qCDebug(logNSDesktopEntries)
        << "A scan is already in progress - current request queued.";
    m_scanQueued = true;
    return;
  }

  m_scanInProgress = true;
  m_scanQueued     = false;
  auto scanner     = new DesktopEntryScanner(this);
  QObject::connect(scanner,
                   &DesktopEntryScanner::scanCompleted,
                   this,
                   &EntryManager::onScanCompleted);
  QThreadPool::globalInstance()->start(scanner);
}

void EntryManager::processEntryList(const QList<EntryData> &results) {
  auto guard = qScopeGuard([this] {
    m_scanInProgress = false;
    if (m_scanQueued) {
      m_scanQueued = false;
      this->scanDesktopEntries();
    }
  });

  auto                           oldEntries = m_desktopEntries;
  QHash<QString, DesktopEntry *> newEntries;

  for (const auto &data : results) {
    if (data.hidden) {
      if (auto *target = newEntries.take(data.id))
        target->deleteLater();

      if (auto it = oldEntries.find(data.id); it != oldEntries.end()) {
        it.value()->deleteLater();
        oldEntries.erase(it);
      }
    }

    DesktopEntry *entry = nullptr;

    if (auto it = oldEntries.find(data.id); it != oldEntries.end()) {
      entry = it.value();
      oldEntries.erase(it);
      entry->updateState(data);
    } else {
      entry = new DesktopEntry(data.id, this);
      entry->updateState(data);
      entry->bindableFrequency().setValue(this->getFrequencyForApp(data.id));
    }

    if (!entry->isValid()) {
      if (!oldEntries.contains(data.id)) {
        entry->deleteLater();
      }
      continue;
    }

    auto conflictingId = newEntries.contains(data.id);

    if (conflictingId) {
      if (auto target = newEntries.take(data.id))
        target->deleteLater();
    }

    newEntries.insert(data.id, entry);
  }

  m_desktopEntries = newEntries;

  emit applicationsChanged();

  for (auto *e : oldEntries)
    e->deleteLater();
}

void EntryManager::onScanCompleted(const QList<EntryData> &results) {
  qCDebug(logNSDesktopEntries)
      << "Desktop entry scan completed. Processing results...";
  m_entryCacher->saveToCache(results);
  this->processEntryList(results);
}

DesktopEntry *EntryManager::findEntryById(const QString &id) {
  if (auto entry = m_desktopEntries.value(id))
    return entry;
  else
    return nullptr;
}

DesktopEntry *EntryManager::findEntry(const QString &name) {
  // TODO: if QString's toLower() brings performance issues somehow, cache the
  // lowered versions beforehand
  if (auto entry = this->findEntryById(name))
    return entry;

  auto entryList = m_desktopEntries.values();

  auto it = std::ranges::find_if(entryList, [&](DesktopEntry *entry) {
    return name.toLower() == entry->bindableName().value().toLower();
  });
  if (it != entryList.end())
    return *it;

  it = std::ranges::find_if(entryList, [&](DesktopEntry *entry) {
    return name == entry->bindableStartupClass().value();
  });
  if (it != entryList.end())
    return *it;

  it = std::ranges::find_if(entryList, [&](DesktopEntry *entry) {
    return name.toLower() == entry->bindableStartupClass().value().toLower();
  });
  if (it != entryList.end())
    return *it;
  return nullptr;
}

QHash<QString,
      DesktopEntry *>
EntryManager::getEntries() const {
  return m_desktopEntries;
}
} // namespace ns::desktop::entries
